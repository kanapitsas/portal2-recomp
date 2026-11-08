#!/usr/bin/env python3
"""Frame-rate bench on the iPhone: runs a series of configurations and measures.

Each configuration sets the antialiasing (setting.mat_antialias of
update/cfg/video.txt), the render scale (P2_IOS_RENDER_SCALE) and the matching
resolution (932x430 points x scale on an iPhone 15 Pro Max), launches the game
(Documents/launch.args picks the map), waits, then reports the in-game
P2_FRAME_LOG frame rates. The original video.txt is restored at the end.

With --renderer opengl (default) frames are rendered but never presented
(P2_IOS_NO_PRESENT=1), so the figures are uncapped. The Metal renderer
(--renderer d3d9) always presents: its figures are capped by the display.

    python3 tools/ios_bench.py --seconds 120 msaa4:4:3 msaa0:0:3 msaa0-x2:0:2
    (name:antialias:scale[:VAR=val,VAR=val]; scale "native" or a number;
    extra environment variables, e.g. P2_IOS_SRGB=0)
"""
import argparse
import json
import re
import statistics
import subprocess
import sys
import time
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from ios_bundle_id import bundle_id  # noqa: E402

APP = bundle_id(game='portal2')   # Portal 2 only (build/ios/device, Portal2.app)
VIDEO = 'Documents/portal2-userdata/update/cfg/video.txt'
OUT = Path('build/ios/device/bench')


def device():
    listing = subprocess.run(['xcrun', 'devicectl', 'list', 'devices'], capture_output=True, text=True).stdout
    for line in listing.splitlines():
        if 'physical' in line and ('connected' in line or 'available (paired)' in line):
            m = re.search(r'[0-9A-F]{8}-[0-9A-F]{16}', line)
            if m: return m.group(0)
    sys.exit('no iPhone connected')


def devicectl(udid, *args, check=True):
    sub, rest = list(args[:3]), list(args[3:])
    return subprocess.run(['xcrun', 'devicectl', *sub, '--device', udid, *rest], capture_output=True, text=True,
                          check=check)


def pull(udid, source, destination):
    devicectl(udid, 'device', 'copy', 'from', '--domain-type', 'appDataContainer', '--domain-identifier', APP,
              '--source', source, '--destination', str(destination))


def push(udid, source, destination):
    devicectl(udid, 'device', 'copy', 'to', '--domain-type', 'appDataContainer', '--domain-identifier', APP,
              '--source', str(source), '--destination', destination)


def stop_game(udid):
    """Stop the game (SIGKILL) before touching its configuration: a normal
    exit could rewrite video.txt with its own settings."""
    listing = devicectl(udid, 'device', 'info', 'processes', check=False).stdout
    for line in listing.splitlines():
        if line.rstrip().endswith('/Portal2.app/portal2'):
            devicectl(udid, 'device', 'process', 'terminate', '--pid', line.split()[0], '--kill', check=False)
    time.sleep(2)


def in_game_fps(text, teleported):
    """Frame rates measured in game only: after entering the game (and after
    the last P2_IOS_AUTOEXEC command when it is used to get in position),
    skipping the first samples (texture loading, transition)."""
    started, skip, values = False, 0, []
    for line in text.splitlines():
        if '[ios] interface du jeu masquée' in line and not teleported: started, skip, values = True, 5, []
        elif teleported and line.startswith('[ios] commande cmd '): started, skip, values = True, 3, []
        elif '[ios] interface du jeu affichée' in line: started = False
        m = re.search(r'\[diag\] frame \d+, ([0-9.]+) fps', line)
        if m and started:
            if skip: skip -= 1
            else: values.append(float(m.group(1)))
    return values


def with_settings(text, values):
    for key, value in values.items():
        pattern = rf'("setting\.{key}"\s+)"[^"]*"'
        if re.search(pattern, text): text = re.sub(pattern, rf'\g<1>"{value}"', text)
        else: text = text.replace('}', f'\t"setting.{key}"\t\t"{value}"\n}}', 1)
    return text


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('configs', nargs='+', help='name:antialias:scale[:VAR=val,...]')
    ap.add_argument('--seconds', type=int, default=120)
    ap.add_argument('--points', default='932x430', help='screen size in points (landscape)')
    ap.add_argument('--vsync', default='0', help='setting.mat_vsync (0: no vertical sync)')
    ap.add_argument('--renderer', choices=['opengl', 'd3d9'], default='opengl',
                    help='P2_RENDERER; only opengl runs uncapped (P2_IOS_NO_PRESENT), d3d9 (Metal) is '
                         'capped by the display refresh rate')
    args = ap.parse_args()
    if args.renderer == 'd3d9':
        print('warning: the Metal renderer always presents: frame rates are capped by the display '
              '(P2_IOS_NO_PRESENT only applies to OpenGL)', file=sys.stderr)
    udid = device()
    OUT.mkdir(parents=True, exist_ok=True)
    original = OUT / 'video.orig.txt'
    stop_game(udid)
    pull(udid, VIDEO, original)
    base = original.read_text()
    pw, ph = (int(v) for v in args.points.split('x'))
    results = []
    try:
        for spec in args.configs:
            name, aa, scale, *extra = spec.split(':')
            factor = 3.0 if scale == 'native' else float(scale)
            video = OUT / f'video.{name}.txt'
            video.write_text(with_settings(base, {'mat_antialias': aa, 'mat_aaquality': '0', 'mat_vsync': args.vsync,
                                                  'defaultres': round(pw * factor), 'defaultresheight': round(ph * factor)}))
            stop_game(udid)
            push(udid, video, VIDEO)
            env = {'P2_LOG_FILE': 'Documents/game.log', 'P2_FRAME_LOG': '1', 'P2_IOS_RENDER_SCALE': scale,
                   'P2_RENDERER': args.renderer}
            if args.renderer == 'opengl':
                env['P2_IOS_NO_PRESENT'] = '1'   # no display cap (frozen image); OpenGL path only
            env.update(dict(item.split('=', 1) for item in ','.join(extra).split(',') if item))
            launched = devicectl(udid, 'device', 'process', 'launch', '--terminate-existing', '--environment-variables',
                                 json.dumps(env), APP, check=False)
            if launched.returncode:
                sys.exit(f'{name}: launch refused (iPhone locked?)\n{launched.stderr[-400:]}')
            time.sleep(args.seconds)
            log = OUT / f'game.{name}.log'
            pull(udid, 'Documents/game.log', log)
            fps = in_game_fps(log.read_text(), 'P2_IOS_AUTOEXEC' in env)
            # Size line of either renderer: runtime/ios/gl_ios.m or runtime/ios/metal_ios.m.
            size = re.search(r'contexte ES 3, FBO système \d+, (\d+x\d+)|Metal view: .*drawable (\d+x\d+)',
                             log.read_text())
            steady = fps or [0]
            results.append((name, (size.group(1) or size.group(2)) if size else '?', statistics.mean(steady),
                            min(steady), max(steady)))
            print(f'{name:12} {results[-1][1]:>10}  mean {results[-1][2]:5.1f}  min {results[-1][3]:5.1f}  '
                  f'max {results[-1][4]:5.1f} fps ({1000 / max(results[-1][2], 0.1):.1f} ms/frame)', flush=True)
    finally:
        stop_game(udid)
        push(udid, original, VIDEO)
        print('original video.txt restored')


if __name__ == '__main__':
    main()
