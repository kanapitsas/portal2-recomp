#!/usr/bin/env python3
"""Compare the OpenGL (togl) and Direct3D 9 (mtld3d) renderers in game.

Runs N alternating rounds (GL, D3D9, GL, D3D9, ...) of the same scenario:
`+map <map>` from the command line, standing still at the spawn view, fixed
resolution / MSAA / anisotropy / vsync off written into update/cfg/video.txt
of a per-renderer copy of the user data (never the real one). Each launch
logs one line per present (P2_FRAME_TIMES, runtime/frame_times.c) and the
console (-condebug); the script waits for the end of the map load
("Redownloading all lightmaps" in console.log), lets `--warmup` seconds pass,
measures `--duration` seconds, then kills the game.

Per run: average fps, frame-time p50/p95/p99, 1 % low, frames > 50/100 ms
after the load (first round = cold application caches: mtld3d's
d3d9metal-cache, togl's update/glshaders.cfg), time inside present,
CPU time per frame of the presenting thread and of the whole process
(proc_pid_rusage: CPU time, cycles, instructions), memory footprint, load
time; GPU time with --gpu-timing (perturbs the run, see STATUS.md).

Results: build/bench/<date>/results.md and results.json, and per run the
game log, frame log and console log.

    python3 tools/bench_renderer.py --dry-run        # prints the plan, launches nothing
    python3 tools/bench_renderer.py                  # Steam client open, machine idle
    python3 tools/bench_renderer.py --analyze build/bench/<date>   # recompute from the logs
"""
import argparse
import ctypes
import json
import math
import os
import re
import shlex
import shutil
import signal
import statistics
import subprocess
import sys
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
DEFAULT_DYLIB = ROOT.parent / 'mtld3d/native/target/release/libd3d9metal.dylib'
LOAD_MARKER = 'Redownloading all lightmaps'
ERROR_PATTERN = re.compile(r'guest trap|host SIG|Failed to create D3D9')
# Variables that would change what a run measures; removed from the inherited environment.
SCRUBBED = ('P2_RENDERER', 'P2_D3D9METAL', 'P2_D3D9_CACHE', 'P2_FRAME_TIMES', 'P2_GPU_TIMING', 'P2_FPS',
            'P2_PROFILE', 'P2_PROFILE_WALL', 'P2_PASS_TRACE', 'P2_BLIT_TRACE', 'P2_FRAME_DUMP', 'P2_HIDDEN_WINDOWS',
            'RUST_LOG', 'MTL_HUD_ENABLED', 'MTL_CAPTURE_ENABLED', 'MTL_DEBUG_LAYER', 'LLVM_PROFILE_FILE')


# ------------------------------------------------------------------ parsing
def parse_frames(text):
    """P2_FRAME_TIMES log -> (renderer letters, [(start_ns, present_ns, cpu_ns)])."""
    letters, frames = set(), []
    for line in text.splitlines():
        if not line or line.startswith('#'): continue
        parts = line.split()
        if len(parts) != 4: continue       # last line cut by the kill
        try: frames.append((int(parts[1]), int(parts[2]), int(parts[3])))
        except ValueError: continue
        letters.add(parts[0])
    return letters, frames


def frame_gap_load_end(frames, gap_ns):
    """Fallback without a console marker: end of the last interval >= gap_ns."""
    end = None
    for a, b in zip(frames, frames[1:]):
        if b[0] - a[0] >= gap_ns: end = b[0]
    return end


def percentile(values, q):
    """Linear interpolation between closest ranks (numpy's default)."""
    if not values: return None
    s = sorted(values)
    k = (len(s) - 1) * q / 100
    lo, hi = math.floor(k), math.ceil(k)
    return s[lo] + (s[hi] - s[lo]) * (k - lo)


def frame_stats(frames, start_ns, end_ns):
    """Statistics of the frames whose present starts inside [start_ns, end_ns)."""
    window = [f for f in frames if start_ns <= f[0] < end_ns]
    if len(window) < 3: return None
    ms = [(b[0] - a[0]) / 1e6 for a, b in zip(window, window[1:])]
    span = (window[-1][0] - window[0][0]) / 1e9
    worst = sorted(ms, reverse=True)[:max(1, len(ms) // 100)]
    cpu = [(b[2] - a[2]) / 1e6 for a, b in zip(window, window[1:])]
    # A thread change (other presenting thread) shows up as a negative delta.
    thread_cpu = statistics.mean(cpu) if all(c >= 0 for c in cpu) else None
    return {
        'frames': len(ms), 'seconds': round(span, 3),
        'avg_fps': len(ms) / span if span > 0 else None,
        'frame_ms_mean': statistics.mean(ms),
        'p50_ms': percentile(ms, 50), 'p95_ms': percentile(ms, 95), 'p99_ms': percentile(ms, 99),
        'max_ms': max(ms),
        'low1_fps': 1000 / statistics.mean(worst),     # average of the slowest 1 % of frames
        'present_ms_mean': statistics.mean(f[1] / 1e6 for f in window),
        'thread_cpu_ms_per_frame': thread_cpu,
    }


def stutter_stats(frames, start_ns, end_ns):
    """Hitches after the load: intervals that end inside [start_ns, end_ns)."""
    ms = [(b[0] - a[0]) / 1e6 for a, b in zip(frames, frames[1:]) if a[0] >= start_ns and b[0] < end_ns]
    if not ms: return None
    return {'frames': len(ms), 'over_50ms': sum(m > 50 for m in ms), 'over_100ms': sum(m > 100 for m in ms),
            'over_250ms': sum(m > 250 for m in ms), 'max_ms': max(ms),
            'stall_ms_over_50': sum(m - 50 for m in ms if m > 50)}


def interpolate(samples, t, key):
    """Value of samples[key] at time t (linear between the two closest samples)."""
    before = [s for s in samples if s['t_ns'] <= t]
    after = [s for s in samples if s['t_ns'] >= t]
    if not before or not after: return None
    a, b = before[-1], after[0]
    if b['t_ns'] == a['t_ns']: return a[key]
    return a[key] + (b[key] - a[key]) * (t - a['t_ns']) / (b['t_ns'] - a['t_ns'])


def rusage_stats(samples, start_ns, end_ns, frames):
    """Process-wide CPU cost over the window, per frame (all threads, mtld3d's included)."""
    if not frames or not samples: return None
    out = {}
    for key in ('cpu_ns', 'cycles', 'instructions'):
        a, b = interpolate(samples, start_ns, key), interpolate(samples, end_ns, key)
        if a is None or b is None: return None
        out[key] = b - a
    seconds = (end_ns - start_ns) / 1e9
    footprint = [s['footprint'] for s in samples if start_ns <= s['t_ns'] <= end_ns]
    return {'process_cpu_ms_per_frame': out['cpu_ns'] / 1e6 / frames,
            'process_cpu_cores': out['cpu_ns'] / 1e9 / seconds,
            'cycles_per_frame_m': out['cycles'] / 1e6 / frames,
            'instructions_per_frame_m': out['instructions'] / 1e6 / frames,
            'footprint_mb_max': max(footprint) / 2**20 if footprint else None}


def gpu_stats(lines, start_ns, end_ns):
    """GPU time per frame from timestamped log lines: '[gpu] X ms GPU' (GL,
    P2_GPU_TIMING) or mtld3d perf-kv 'gpu_ms=' (D3D9, MTLD3D_PERF build)."""
    gl, d3d, skipped = [], [], 0
    for t, line in lines:
        if not start_ns <= t <= end_ns: continue
        m = re.search(r'\[gpu\] ([0-9.]+) ms GPU', line)
        if m: gl.append(float(m.group(1)))
        if 'perf-kv' in line:
            kv = dict(re.findall(r'(\w+)=([0-9.]+)', line))
            if 'gpu_ms' in kv: d3d.append(float(kv['gpu_ms']))
            skipped += int(float(kv.get('comp_async_skipped_draws_total', 0)))
    values = gl or d3d
    if not values: return None
    return {'gpu_ms_per_frame': statistics.mean(values), 'samples': len(values),
            'source': 'P2_GPU_TIMING' if gl else 'mtld3d perf-kv', 'async_skipped_draws': skipped if d3d else None}


def analyze_run(run_dir, warmup, duration, settle=0.5, gap=1.0):
    """Recompute every metric of a run from the files it left (frames.txt,
    meta.json with the load marker time, rusage.json, log-lines.json)."""
    run_dir = Path(run_dir)
    meta = json.loads((run_dir / 'meta.json').read_text())
    letters, frames = parse_frames((run_dir / 'frames.txt').read_text(errors='replace')) \
        if (run_dir / 'frames.txt').exists() else (set(), [])
    result = {k: meta.get(k) for k in ('renderer', 'round', 'cold', 'status', 'load_average', 'video_reset',
                                        'video_after', 'error')}
    result['renderers_seen'] = sorted(letters)
    after = meta.get('video_after') or {}
    changed = {k: after.get(f'setting.{k}') for k, v in (meta.get('video') or {}).items()
               if f'setting.{k}' in after and after[f'setting.{k}'] != v}
    if changed: result['video_changed'] = changed
    expected = {'gl': 'g', 'd3d9': 'd'}[meta['renderer']]
    if frames and letters != {expected}:
        result['error'] = (result.get('error') or '') + f' presents from {sorted(letters)}, expected {expected!r}'
    load_end, source = meta.get('load_marker_ns'), 'console'
    if load_end is None:
        load_end, source = frame_gap_load_end(frames, int(gap * 1e9)), 'frame-gap'
    result['load_end_source'] = source if load_end is not None else None
    if load_end is None or not frames: return result
    if meta.get('launch_ns'): result['load_seconds'] = (load_end - meta['launch_ns']) / 1e9
    start = load_end + int(warmup * 1e9)
    end = start + int(duration * 1e9)
    if frames[-1][0] < end - int(0.5e9):
        result['error'] = (result.get('error') or '') + ' window not covered by frames'
    result['steady'] = frame_stats(frames, start, end)
    result['stutter'] = stutter_stats(frames, load_end + int(settle * 1e9), end)
    samples = json.loads((run_dir / 'rusage.json').read_text()) if (run_dir / 'rusage.json').exists() else []
    if result['steady']: result['cpu'] = rusage_stats(samples, start, end, result['steady']['frames'])
    lines = json.loads((run_dir / 'log-lines.json').read_text()) if (run_dir / 'log-lines.json').exists() else []
    result['gpu'] = gpu_stats(lines, start, end)
    return result


# ------------------------------------------------------------------ video.txt
VIDEO_TEMPLATE = '"config"\n{\n}\n'


def video_settings(width, height, msaa, aniso):
    return {'fullscreen': '0', 'nowindowborder': '0', 'mat_vsync': '0', 'mat_triplebuffered': '0',
            'mat_antialias': str(msaa), 'mat_aaquality': '0', 'mat_forceaniso': str(aniso),
            'defaultres': str(width), 'defaultresheight': str(height)}


def patch_video(text, settings):
    """Set "setting.<key>" values, keep everything else (VendorID/DeviceID
    included: a mismatch makes Source reset the file to its defaults)."""
    text = text or VIDEO_TEMPLATE
    for key, value in settings.items():
        pattern = rf'("setting\.{re.escape(key)}"\s+)"[^"]*"'
        if re.search(pattern, text): text = re.sub(pattern, lambda m: f'{m.group(1)}"{value}"', text)
        else: text = text.replace('}', f'\t"setting.{key}"\t\t"{value}"\n}}', 1)
    return text


def read_video(text):
    return dict(re.findall(r'"([\w.]+)"\s+"([^"]*)"', text or ''))


# ------------------------------------------------------------------ plan
def build_plan(args, config):
    width, height = (int(v) for v in args.size.lower().split('x'))
    renderers = [r.strip() for r in args.renderers.split(',') if r.strip()]
    for r in renderers:
        if r not in ('gl', 'd3d9'): sys.exit(f'unknown renderer {r!r} (gl, d3d9)')
    user_dir = Path(config.get('user_dir') or ROOT.parent / 'work/portal2-userdata').expanduser().resolve()
    diag = user_dir.with_name('portal2-userdata-diag')
    source = diag if diag.exists() else user_dir
    runs = []
    for round_index in range(1, args.rounds + 1):
        for renderer in renderers:
            copy = user_dir.with_name(f'portal2-userdata-bench-{renderer}')
            env = {'SteamAppId': str(config.get('steam_appid', 620)), 'P2_FAKE_FOCUS': '1',
                   'SDL_AUDIODRIVER': 'dummy', 'P2_FRAME_TIMES': '<run>/frames.txt'}
            if args.hidden: env['P2_HIDDEN_WINDOWS'] = '1'
            if renderer == 'd3d9':
                env.update(P2_RENDERER='d3d9', P2_D3D9METAL=str(args.d3d9_dylib),
                           P2_D3D9_CACHE=str(copy / 'd3d9metal-cache'))
                if args.mtld3d_config: env['MTLD3D_CONFIG'] = args.mtld3d_config
                if args.gpu_timing: env['RUST_LOG'] = 'mtld3d::perf=info'
            elif args.gpu_timing: env['P2_GPU_TIMING'] = '1'
            game_args = ['-game', 'portal2', '-novid', '-windowed', '-w', str(width), '-h', str(height),
                         '-condebug', *shlex.split(args.extra), '+map', args.map]
            command = [str(Path(args.binary).resolve()), '--images', str(ROOT / 'build/game/prep'),
                       '--game-dir', config.get('game_root', '<game_root>'),
                       '--linux-root', config.get('linux_root', '<linux_root>'),
                       '--user-dir', str(copy), '--', *game_args]
            cold = args.cold and round_index == 1
            cache = copy / ('d3d9metal-cache' if renderer == 'd3d9' else 'update/glshaders.cfg')
            runs.append({'renderer': renderer, 'round': round_index, 'cold': cold, 'copy': str(copy),
                         'copy_source': str(source), 'cache': str(cache), 'env': env, 'command': command,
                         'video': video_settings(width, height, args.msaa, args.aniso)})
    return runs


def describe(runs, args, out, language_args):
    per_run = 45 + args.warmup + args.duration + 2   # ~45 s from launch to the end of the load (not measured)
    print(f'bench_renderer: {len(runs)} runs, results in {out}/, about {len(runs) * per_run / 60:.0f} min '
          f'(plus one extra run per renderer if the game resets video.txt)')
    print(f'  map {args.map}, {args.size}, MSAA {args.msaa}, aniso {args.aniso}, vsync off, '
          f'{"hidden" if args.hidden else "visible"} window, warmup {args.warmup}s, window {args.duration}s, '
          f'timeout {args.timeout}s per run')
    for run in runs:
        print(f'\n# round {run["round"]} {run["renderer"]}{" (cold caches)" if run["cold"] else ""}')
        if not Path(run['copy']).exists():
            print(f'copy user data (first use only): {run["copy_source"]} -> {run["copy"]}')
        if run['cold']: print(f'remove: {run["cache"]}')
        print(f'video.txt ({run["copy"]}/update/cfg/video.txt): ' +
              ', '.join(f'setting.{k}={v}' for k, v in run['video'].items()))
        print(f'truncate: {run["copy"]}/portal2/console.log')
        print('env -u ' + ' -u '.join(SCRUBBED) + ' \\\n  ' +
              ' '.join(f'{k}={shlex.quote(v)}' for k, v in run['env'].items()) + ' \\\n  ' +
              ' '.join(shlex.quote(a) for a in [*run['command'], *language_args]))
        print(f'stop: SIGKILL {args.warmup + args.duration + 1.5:.1f}s after "{LOAD_MARKER}" in console.log '
              f'(or after {args.timeout}s)')


# ------------------------------------------------------------------ measuring
class RusageInfoV4(ctypes.Structure):
    _fields_ = [('ri_uuid', ctypes.c_uint8 * 16)] + [(name, ctypes.c_uint64) for name in (
        'user_time', 'system_time', 'pkg_idle_wkups', 'interrupt_wkups', 'pageins', 'wired_size', 'resident_size',
        'phys_footprint', 'proc_start_abstime', 'proc_exit_abstime', 'child_user_time', 'child_system_time',
        'child_pkg_idle_wkups', 'child_interrupt_wkups', 'child_pageins', 'child_elapsed_abstime',
        'diskio_bytesread', 'diskio_byteswritten', 'qos_default', 'qos_maintenance', 'qos_background',
        'qos_utility', 'qos_legacy', 'qos_user_initiated', 'qos_user_interactive', 'billed_system_time',
        'serviced_system_time', 'logical_writes', 'lifetime_max_phys_footprint', 'instructions', 'cycles',
        'billed_energy', 'serviced_energy', 'interval_max_phys_footprint', 'runnable_time')]


class Rusage:
    """proc_pid_rusage(pid, RUSAGE_INFO_V4): CPU times are in mach absolute
    time units (×125/3 on Apple silicon), converted with mach_timebase_info."""
    def __init__(self):
        self.lib = ctypes.CDLL('/usr/lib/libSystem.B.dylib')
        base = (ctypes.c_uint32 * 2)()
        self.lib.mach_timebase_info(base)
        self.numer, self.denom = base[0], base[1]

    def sample(self, pid):
        info = RusageInfoV4()
        if self.lib.proc_pid_rusage(pid, 4, ctypes.byref(info)) != 0: return None
        return {'t_ns': time.clock_gettime_ns(time.CLOCK_UPTIME_RAW),
                'cpu_ns': (info.user_time + info.system_time) * self.numer // self.denom,
                'cycles': info.cycles, 'instructions': info.instructions, 'footprint': info.phys_footprint}


def tail(path, offset):
    try:
        with open(path, 'rb') as f:
            f.seek(offset)
            data = f.read()
    except FileNotFoundError:
        return offset, ''
    cut = data.rfind(b'\n') + 1          # only complete lines
    return offset + cut, data[:cut].decode(errors='replace')


def prepare(run, run_dir):
    copy = Path(run['copy'])
    if not copy.exists(): shutil.copytree(run['copy_source'], copy, symlinks=True)
    if run['cold']:
        cache = Path(run['cache'])
        if cache.is_dir(): shutil.rmtree(cache)
        elif cache.exists(): cache.unlink()
    video = copy / 'update/cfg/video.txt'
    video.parent.mkdir(parents=True, exist_ok=True)
    before = video.read_text(errors='replace') if video.exists() else ''
    written = patch_video(before, run['video'])
    video.write_text(written)
    (run_dir / 'video.before.txt').write_text(written)
    console = copy / 'portal2/console.log'
    console.parent.mkdir(parents=True, exist_ok=True)
    console.write_text('')
    return video, written, console


def run_once(run, run_dir, args, rusage, language_args):
    run_dir.mkdir(parents=True, exist_ok=True)
    video, written, console = prepare(run, run_dir)
    env = {k: v for k, v in os.environ.items() if k not in SCRUBBED}
    env.update({k: v.replace('<run>', str(run_dir)) for k, v in run['env'].items()})
    command = [*run['command'], *language_args]
    meta = {k: run[k] for k in ('renderer', 'round', 'cold', 'video')}
    meta.update(command=command, env=run['env'], load_average=os.getloadavg()[0], status=None, error=None)
    samples, lines, marker = [], [], None
    log_offset = console_offset = 0
    saw_new_game = False
    game_log = run_dir / 'game.log'
    with game_log.open('wb') as output:
        meta['launch_ns'] = time.clock_gettime_ns(time.CLOCK_UPTIME_RAW)
        process = subprocess.Popen(command, stdout=output, stderr=subprocess.STDOUT, env=env, cwd=ROOT)
        deadline = time.monotonic() + args.timeout
        stop_after = (args.warmup + args.duration + 1.5) * 1e9
        try:
            while process.poll() is None:
                time.sleep(0.25)
                now = time.clock_gettime_ns(time.CLOCK_UPTIME_RAW)
                s = rusage.sample(process.pid)
                if s: samples.append(s)
                log_offset, text = tail(game_log, log_offset)
                for line in text.splitlines():
                    lines.append((now, line))
                    if ERROR_PATTERN.search(line) and not meta['error']: meta['error'] = line.strip()[:300]
                console_offset, text = tail(console, console_offset)
                for line in text.splitlines():
                    if line.startswith('Host_NewGame on map'): saw_new_game = True
                    if marker is None and saw_new_game and LOAD_MARKER in line: marker = now
                if meta['error']: meta['status'] = 'error'; break
                if marker is not None and now - marker >= stop_after: meta['status'] = 'ok'; break
                if time.monotonic() > deadline: meta['status'] = 'timeout'; break
            else:
                meta['status'] = 'exited'
                meta['error'] = meta['error'] or f'game exited with status {process.returncode}'
        finally:
            if process.poll() is None:
                process.send_signal(signal.SIGKILL)
                process.wait()
    meta['load_marker_ns'] = marker
    after = video.read_text(errors='replace') if video.exists() else ''
    ids = ('VendorID', 'DeviceID')
    meta['video_after'] = {k: v for k, v in read_video(after).items() if k in ids or k.startswith('setting.')}
    meta['video_reset'] = any(read_video(after).get(k) != read_video(written).get(k) for k in ids)
    shutil.copy(console, run_dir / 'console.log')
    (run_dir / 'meta.json').write_text(json.dumps(meta, indent=1))
    (run_dir / 'rusage.json').write_text(json.dumps(samples))
    (run_dir / 'log-lines.json').write_text(json.dumps([l for l in lines if '[gpu]' in l[1] or 'perf-kv' in l[1]]))
    return meta


# ------------------------------------------------------------------ report
def fmt(value, digits=1):
    return '—' if value is None else f'{value:.{digits}f}'


def summarize(results):
    """Per renderer: warm rounds (steady state) and cold rounds (stutter)."""
    summary = {}
    for renderer in sorted({r['renderer'] for r in results}):
        runs = [r for r in results if r['renderer'] == renderer and r.get('steady') and not r.get('error')
                and not r.get('video_reset')]
        warm = [r for r in runs if not r.get('cold')] or runs
        cold = [r for r in results if r['renderer'] == renderer and r.get('cold') and r.get('stutter')]

        def med(group, path):
            values = []
            for r in group:
                v = r
                for p in path.split('.'): v = (v or {}).get(p)
                if v is not None: values.append(v)
            return statistics.median(values) if values else None
        fps = [r['steady']['avg_fps'] for r in warm]
        summary[renderer] = {
            'warm_runs': len(warm), 'avg_fps_mean': statistics.mean(fps) if fps else None,
            'avg_fps_min': min(fps) if fps else None, 'avg_fps_max': max(fps) if fps else None,
            **{k: med(warm, f'steady.{k}') for k in ('p50_ms', 'p95_ms', 'p99_ms', 'low1_fps', 'present_ms_mean',
                                                     'thread_cpu_ms_per_frame')},
            **{k: med(warm, f'cpu.{k}') for k in ('process_cpu_ms_per_frame', 'process_cpu_cores',
                                                   'cycles_per_frame_m', 'footprint_mb_max')},
            'gpu_ms_per_frame': med(warm, 'gpu.gpu_ms_per_frame'),
            'warm_over_50ms': med(warm, 'stutter.over_50ms'), 'warm_load_s': med(warm, 'load_seconds'),
            'cold_over_50ms': med(cold, 'stutter.over_50ms'), 'cold_over_100ms': med(cold, 'stutter.over_100ms'),
            'cold_max_ms': med(cold, 'stutter.max_ms'), 'cold_load_s': med(cold, 'load_seconds'),
        }
    return summary


def render_markdown(settings, results, summary):
    out = [f'# Renderer benchmark — {settings["date"]}', '',
           f'{settings["map"]}, {settings["size"]}, MSAA {settings["msaa"]}, aniso {settings["aniso"]}, vsync off, '
           f'{settings["window"]} window; window {settings["duration"]} s starting {settings["warmup"]} s after '
           f'the end of the load; binary `{settings["binary"]}`, mtld3d `{settings["d3d9_dylib"]}`.', '',
           '## Summary (warm rounds: mean fps over runs, medians otherwise)', '',
           '| renderer | runs | avg fps (min–max) | p50 ms | p95 ms | p99 ms | 1% low fps | present ms | '
           'thread CPU ms/frame | process CPU ms/frame | cores | Mcycles/frame | GPU ms | footprint MB | '
           'load s (warm/cold) | >50 ms warm | cold >50/>100 ms, max |',
           '|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|']
    for renderer, s in summary.items():
        out.append(f'| {renderer} | {s["warm_runs"]} | {fmt(s["avg_fps_mean"])} ({fmt(s["avg_fps_min"], 0)}–'
                   f'{fmt(s["avg_fps_max"], 0)}) | {fmt(s["p50_ms"], 2)} | {fmt(s["p95_ms"], 2)} | '
                   f'{fmt(s["p99_ms"], 2)} | {fmt(s["low1_fps"])} | {fmt(s["present_ms_mean"], 2)} | '
                   f'{fmt(s["thread_cpu_ms_per_frame"], 2)} | {fmt(s["process_cpu_ms_per_frame"], 2)} | '
                   f'{fmt(s["process_cpu_cores"], 2)} | {fmt(s["cycles_per_frame_m"])} | '
                   f'{fmt(s["gpu_ms_per_frame"], 2)} | {fmt(s["footprint_mb_max"], 0)} | '
                   f'{fmt(s["warm_load_s"])}/{fmt(s["cold_load_s"])} | {fmt(s["warm_over_50ms"], 0)} | '
                   f'{fmt(s["cold_over_50ms"], 0)}/{fmt(s["cold_over_100ms"], 0)}, {fmt(s["cold_max_ms"], 0)} |')
    out += ['', '## Runs', '',
            '| round | renderer | caches | status | load | avg fps | p50 | p95 | p99 | 1% low | max ms | '
            '>50 ms | proc CPU ms/f | GPU ms | machine load | notes |',
            '|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|']
    for r in results:
        st, cpu, stut, gpu = r.get('steady') or {}, r.get('cpu') or {}, r.get('stutter') or {}, r.get('gpu') or {}
        notes = '; '.join(x for x in [r.get('error') and r['error'].strip(), r.get('video_reset') and
                                      'video.txt reset by the game (excluded)',
                                      r.get('video_changed') and f'video.txt after the run: {r["video_changed"]}',
                                      r.get('load_end_source') == 'frame-gap' and 'load end from frame gap'] if x)
        out.append(f'| {r["round"]} | {r["renderer"]} | {"cold" if r.get("cold") else "warm"} | {r.get("status")} | '
                   f'{fmt(r.get("load_seconds"))} | {fmt(st.get("avg_fps"))} | {fmt(st.get("p50_ms"), 2)} | '
                   f'{fmt(st.get("p95_ms"), 2)} | {fmt(st.get("p99_ms"), 2)} | {fmt(st.get("low1_fps"))} | '
                   f'{fmt(st.get("max_ms"), 0)} | {stut.get("over_50ms", "—")} | '
                   f'{fmt(cpu.get("process_cpu_ms_per_frame"), 2)} | {fmt(gpu.get("gpu_ms_per_frame"), 2)} | '
                   f'{fmt(r.get("load_average"))} | {notes} |')
    out += ['', 'Definitions: frame time = interval between two present calls (P2_FRAME_TIMES); 1% low = '
            '1000 / mean of the slowest 1 % of frame times; >50 ms counted from 0.5 s after the load marker to '
            'the end of the window; process CPU = proc_pid_rusage user+system of all threads (mtld3d workers '
            'included) / frames; thread CPU = presenting thread only.', '']
    return '\n'.join(out)


def report(out, settings, results):
    summary = summarize(results)
    (out / 'results.json').write_text(json.dumps({'settings': settings, 'summary': summary, 'runs': results},
                                                 indent=1))
    markdown = render_markdown(settings, results, summary)
    (out / 'results.md').write_text(markdown)
    print(markdown)
    print(f'\n{out / "results.md"}\n{out / "results.json"}')


# ------------------------------------------------------------------ main
def parse_args(argv=None):
    p = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    p.add_argument('--rounds', type=int, default=4, help='rounds of (GL, D3D9); the first is cold with --cold')
    p.add_argument('--renderers', default='gl,d3d9')
    p.add_argument('--map', default='sp_a2_laser_over_goo')
    p.add_argument('--size', default='2560x1440', help='game resolution in pixels (Retina window above the '
                   'screen size in points)')
    p.add_argument('--msaa', type=int, default=4, help='setting.mat_antialias (0 = off)')
    p.add_argument('--aniso', type=int, default=8, help='setting.mat_forceaniso')
    p.add_argument('--warmup', type=float, default=10, help='seconds skipped after the load marker')
    p.add_argument('--duration', type=float, default=30, help='seconds measured')
    p.add_argument('--timeout', type=float, default=240, help='seconds before a run is killed')
    p.add_argument('--cold', action=argparse.BooleanOptionalAction, default=True,
                   help='wipe the application shader caches before round 1')
    p.add_argument('--hidden', action='store_true', help='hidden window (P2_HIDDEN_WINDOWS=1, as older '
                   'measurements; presentation differs, D3D9 not tried hidden)')
    p.add_argument('--gpu-timing', action='store_true', help='P2_GPU_TIMING=1 on GL, RUST_LOG=mtld3d::perf=info '
                   'on D3D9 (needs an MTLD3D_PERF=1 build); both slow the run')
    p.add_argument('--binary', default=str(ROOT / 'build/portal2-native'))
    p.add_argument('--d3d9-dylib', default=os.environ.get('P2_D3D9METAL') or str(DEFAULT_DYLIB))
    p.add_argument('--mtld3d-config', default='', help='appended to MTLD3D_CONFIG (e.g. shader.asyncCompile=false)')
    p.add_argument('--extra', default='', help='more game arguments, e.g. "+mat_forceaniso 1"')
    p.add_argument('--config', default=str(ROOT / 'config/local.json'))
    p.add_argument('--out', default=None, help='results directory (default build/bench/<date>)')
    p.add_argument('--dry-run', action='store_true', help='print what would run, change nothing')
    p.add_argument('--analyze', metavar='DIR', help='recompute results.md/json from an earlier results directory')
    p.add_argument('--no-checks', action='store_true', help='skip the Steam / machine load / file checks')
    return p.parse_args(argv)


def settings_of(args, date):
    return {'date': date, 'map': args.map, 'size': args.size, 'msaa': args.msaa, 'aniso': args.aniso,
            'warmup': args.warmup, 'duration': args.duration, 'window': 'hidden' if args.hidden else 'visible',
            'rounds': args.rounds, 'cold': args.cold, 'gpu_timing': args.gpu_timing, 'binary': args.binary,
            'd3d9_dylib': args.d3d9_dylib, 'mtld3d_config': args.mtld3d_config, 'extra': args.extra}


def main(argv=None):
    args = parse_args(argv)
    if args.analyze:
        out = Path(args.analyze)
        settings = json.loads((out / 'results.json').read_text())['settings']
        results = [analyze_run(d, settings['warmup'], settings['duration'])
                   for d in sorted(p for p in out.iterdir() if (p / 'meta.json').exists() and
                                   not p.name.endswith('-reset'))]
        report(out, settings, results)
        return 0
    config_path = Path(args.config)
    config = json.loads(config_path.read_text()) if config_path.exists() else {}
    date = time.strftime('%Y%m%d-%H%M%S')
    out = Path(args.out) if args.out else ROOT / 'build/bench' / date
    runs = build_plan(args, config)
    language_args = subprocess.run([sys.executable, str(ROOT / 'tools/language.py'), '--config', str(config_path),
                                    '--', '-game', 'portal2'], capture_output=True, text=True, cwd=ROOT).stdout.split()
    if args.dry_run:
        describe(runs, args, out, language_args)
        return 0
    if not args.no_checks:
        problems = []
        if not config: problems.append(f'{config_path} missing')
        if not Path(args.binary).exists(): problems.append(f'{args.binary} missing (tools/build_game.py --release)')
        if 'd3d9' in args.renderers and not Path(args.d3d9_dylib).exists():
            problems.append(f'{args.d3d9_dylib} missing (cargo build --release -p d3d9metal in mtld3d/native)')
        if subprocess.run(['pgrep', '-x', 'steam_osx'], capture_output=True).returncode:
            problems.append('Steam client not running (open Steam and log in first)')
        if problems: sys.exit('bench_renderer: ' + '; '.join(problems) + ' (--no-checks to ignore)')
        load = os.getloadavg()[0]
        if load > 3: print(f'warning: load average {load:.1f}; numbers are only meaningful on an idle machine')
    out.mkdir(parents=True, exist_ok=True)
    settings = settings_of(args, date)
    (out / 'results.json').write_text(json.dumps({'settings': settings, 'runs': []}, indent=1))
    rusage = Rusage()
    results = []
    for index, run in enumerate(runs, 1):
        name = f'{index:02d}-r{run["round"]}-{run["renderer"]}{"-cold" if run["cold"] else ""}'
        print(f'[{index}/{len(runs)}] {name} ...', flush=True)
        meta = run_once(run, out / name, args, rusage, language_args)
        if meta['video_reset']:
            # Source reset video.txt (VendorID/DeviceID of the other renderer or of
            # the copied data): settings were not ours. The file now holds this
            # renderer's ids; run again with our settings on top of them.
            print('  video.txt was reset by the game; running again', flush=True)
            shutil.move(out / name, out / f'{name}-reset')
            meta = run_once(run, out / name, args, rusage, language_args)
        result = analyze_run(out / name, args.warmup, args.duration)
        results.append(result)
        st = result.get('steady') or {}
        print(f'  {meta["status"]}: {fmt(st.get("avg_fps"))} fps, p99 {fmt(st.get("p99_ms"), 2)} ms'
              f'{", " + result["error"] if result.get("error") else ""}', flush=True)
    report(out, settings, results)
    return 0


if __name__ == '__main__':
    sys.exit(main())
