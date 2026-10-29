"""tools/bench_renderer.py without launching the game: frame-log parsing and
statistics on a fake run, video.txt patching, the plan and --dry-run."""
import contextlib
import io
import json
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent / 'tools'))
import bench_renderer as bench  # noqa: E402

S = 1_000_000_000
BASE = 1_000 * S          # CLOCK_UPTIME_RAW origin of the fake run


def fake_run(directory, renderer='d3d9', marker=True):
    """Launch at BASE, loading-screen presents every 2 s, load marker at 10 s,
    then 200 fps with a 120 ms hitch at 12 s and a 60 ms one at 30 s."""
    times = [BASE + k * 2 * S for k in range(1, 5)]
    t = BASE + 10 * S + S // 10
    while t < BASE + 55 * S:
        times.append(t)
        if abs(t - (BASE + 12 * S)) < 2_500_000: t += 120_000_000
        elif abs(t - (BASE + 30 * S)) < 2_500_000: t += 60_000_000
        else: t += 5_000_000
    letter = {'gl': 'g', 'd3d9': 'd'}[renderer]
    lines = ['# p2-frame-times v1 clock=uptime_raw_ns pid=1']
    lines += [f'{letter} {t} 300000 {(i + 1) * 2_000_000}' for i, t in enumerate(times)]
    lines.append(f'{letter} 123')                  # cut by the kill
    directory.mkdir(parents=True)
    (directory / 'frames.txt').write_text('\n'.join(lines))
    meta = {'renderer': renderer, 'round': 2, 'cold': False, 'status': 'ok', 'load_average': 1.0,
            'video_reset': False, 'error': None, 'launch_ns': BASE,
            'load_marker_ns': BASE + 10 * S if marker else None,
            'video': {'mat_antialias': '4'}, 'video_after': {'setting.mat_antialias': '4'}}
    (directory / 'meta.json').write_text(json.dumps(meta))
    samples = [{'t_ns': BASE + k * S // 4, 'cpu_ns': int(1.5 * k * S // 4), 'cycles': k * 1_000_000,
                'instructions': k * 2_000_000, 'footprint': 512 * 2**20} for k in range(0, 4 * 56)]
    (directory / 'rusage.json').write_text(json.dumps(samples))
    log = [(BASE + 15 * S, 'perf-kv v1 window_s=2.0 frames=400 gpu_ms=99.000'),
           (BASE + 25 * S, 'perf-kv v1 window_s=2.0 frames=400 gpu_ms=7.500 comp_async_skipped_draws_total=3'),
           (BASE + 35 * S, 'perf-kv v1 window_s=2.0 frames=400 gpu_ms=8.500')]
    (directory / 'log-lines.json').write_text(json.dumps(log))
    return times


class Analysis(unittest.TestCase):
    def test_fake_run(self):
        with tempfile.TemporaryDirectory() as tmp:
            times = fake_run(Path(tmp) / 'run')
            r = bench.analyze_run(Path(tmp) / 'run', warmup=10, duration=30)
        self.assertIsNone(r['error'])
        self.assertEqual(r['load_end_source'], 'console')
        self.assertAlmostEqual(r['load_seconds'], 10.0)
        window = [t for t in times if BASE + 20 * S <= t < BASE + 50 * S]
        st = r['steady']
        self.assertEqual(st['frames'], len(window) - 1)
        self.assertAlmostEqual(st['avg_fps'], (len(window) - 1) / ((window[-1] - window[0]) / S))
        self.assertAlmostEqual(st['p50_ms'], 5.0)
        self.assertAlmostEqual(st['max_ms'], 60.0)
        worst = sorted((b - a) / 1e6 for a, b in zip(window, window[1:]))[::-1][:(len(window) - 1) // 100]
        self.assertAlmostEqual(st['low1_fps'], 1000 / (sum(worst) / len(worst)))
        self.assertAlmostEqual(st['present_ms_mean'], 0.3)
        self.assertAlmostEqual(st['thread_cpu_ms_per_frame'], 2.0)
        # Both hitches are after the load (12 s and 30 s), loading-screen gaps are not.
        self.assertEqual(r['stutter']['over_50ms'], 2)
        self.assertEqual(r['stutter']['over_100ms'], 1)
        self.assertAlmostEqual(r['stutter']['max_ms'], 120.0)
        cpu = r['cpu']
        self.assertAlmostEqual(cpu['process_cpu_cores'], 1.5, places=3)
        self.assertAlmostEqual(cpu['process_cpu_ms_per_frame'], 1.5 * 30_000 / st['frames'], places=2)
        self.assertAlmostEqual(cpu['footprint_mb_max'], 512)
        self.assertAlmostEqual(r['gpu']['gpu_ms_per_frame'], 8.0)      # 99 ms line is before the window
        self.assertEqual(r['gpu']['async_skipped_draws'], 3)

    def test_frame_gap_fallback_and_wrong_renderer(self):
        with tempfile.TemporaryDirectory() as tmp:
            fake_run(Path(tmp) / 'run', renderer='gl', marker=False)
            meta = json.loads((Path(tmp) / 'run/meta.json').read_text())
            meta['renderer'] = 'd3d9'
            (Path(tmp) / 'run/meta.json').write_text(json.dumps(meta))
            r = bench.analyze_run(Path(tmp) / 'run', warmup=10, duration=30)
        self.assertEqual(r['load_end_source'], 'frame-gap')
        self.assertAlmostEqual(r['load_seconds'], 10.1)                # end of the 2 s -> 10.1 s gap
        self.assertIn("expected 'd'", r['error'])

    def test_gl_gpu_lines(self):
        lines = [(10, '[gpu] 6.50 ms GPU par image, 9.00 ms entre deux échanges'), (11, '[gpu] 7.50 ms GPU par image')]
        self.assertEqual(bench.gpu_stats(lines, 0, 20)['gpu_ms_per_frame'], 7.0)
        self.assertIsNone(bench.gpu_stats(lines, 12, 20))

    def test_report(self):
        with tempfile.TemporaryDirectory() as tmp:
            out = Path(tmp)
            fake_run(out / '01-r2-gl', renderer='gl')
            fake_run(out / '02-r2-d3d9', renderer='d3d9')
            settings = bench.settings_of(bench.parse_args([]), 'test')
            (out / 'results.json').write_text(json.dumps({'settings': settings, 'runs': []}))
            with contextlib.redirect_stdout(io.StringIO()):
                self.assertEqual(bench.main(['--analyze', str(out)]), 0)
            data = json.loads((out / 'results.json').read_text())
            markdown = (out / 'results.md').read_text()
        self.assertEqual(sorted(data['summary']), ['d3d9', 'gl'])
        self.assertEqual(data['summary']['gl']['warm_runs'], 1)
        self.assertIn('| gl | 1 |', markdown)
        self.assertIn('| 2 | d3d9 | warm | ok |', markdown)


class Plan(unittest.TestCase):
    def test_patch_video_keeps_ids(self):
        text = '"config"\n{\n\t"VendorID"\t\t"4203"\n\t"setting.mat_antialias"\t\t"2"\n}\n'
        out = bench.patch_video(text, {'mat_antialias': '4', 'defaultres': '2560'})
        values = bench.read_video(out)
        self.assertEqual(values['VendorID'], '4203')
        self.assertEqual(values['setting.mat_antialias'], '4')
        self.assertEqual(values['setting.defaultres'], '2560')
        self.assertEqual(bench.read_video(bench.patch_video('', {'mat_vsync': '0'})), {'setting.mat_vsync': '0'})

    def test_plan_alternates_and_isolates(self):
        args = bench.parse_args(['--rounds', '3', '--d3d9-dylib', '/x/libd3d9metal.dylib'])
        runs = bench.build_plan(args, {'game_root': '/g', 'linux_root': '/l', 'user_dir': '/tmp/u/portal2-userdata'})
        self.assertEqual([(r['round'], r['renderer'], r['cold']) for r in runs],
                         [(1, 'gl', True), (1, 'd3d9', True), (2, 'gl', False), (2, 'd3d9', False),
                          (3, 'gl', False), (3, 'd3d9', False)])
        for r in runs:
            self.assertNotEqual(Path(r['copy']).name, 'portal2-userdata')
            self.assertIn('--user-dir', r['command'])
            self.assertEqual(r['command'][r['command'].index('--user-dir') + 1], r['copy'])
            self.assertEqual(r['command'][-2:], ['+map', 'sp_a2_laser_over_goo'])
            self.assertEqual(r['env'].get('P2_RENDERER'), 'd3d9' if r['renderer'] == 'd3d9' else 'opengl')
        self.assertEqual(runs[1]['env']['P2_D3D9METAL'], '/x/libd3d9metal.dylib')
        self.assertTrue(runs[1]['cache'].endswith('portal2-userdata-bench-d3d9/d3d9metal-cache'))
        self.assertTrue(runs[0]['cache'].endswith('portal2-userdata-bench-gl/update/glshaders.cfg'))

    def test_dry_run_changes_nothing(self):
        with tempfile.TemporaryDirectory() as tmp:
            config = Path(tmp) / 'local.json'
            config.write_text(json.dumps({'game_root': '/g', 'linux_root': '/l',
                                          'user_dir': str(Path(tmp) / 'portal2-userdata')}))
            out = Path(tmp) / 'out'
            text = io.StringIO()
            with contextlib.redirect_stdout(text):
                self.assertEqual(bench.main(['--dry-run', '--rounds', '2', '--config', str(config),
                                             '--out', str(out)]), 0)
            self.assertEqual(sorted(p.name for p in Path(tmp).iterdir()), ['local.json'])
        printed = text.getvalue()
        self.assertIn('4 runs', printed)
        self.assertEqual(printed.count('P2_RENDERER=d3d9'), 2)
        self.assertEqual(printed.count('remove: '), 2)
        self.assertIn('+map sp_a2_laser_over_goo', printed)


if __name__ == '__main__':
    unittest.main()
