#!/usr/bin/env python3
"""In-game frame-rate bench (diagnostic: hidden window, muted sound, simulated focus).

Launches the given map N times and reports the mean of the frame rates measured
in the second half of each run (after loading and the first shader compilations),
along with the machine load.
"""
import argparse
import os
import re
import statistics
import subprocess
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import games  # noqa: E402
import localconfig  # noqa: E402

parser = argparse.ArgumentParser(description=__doc__)
games.add_argument(parser)
parser.add_argument('--runs', type=int, default=3)
parser.add_argument('--timeout', type=int, default=150)
parser.add_argument('--map', help='default: sp_a1_intro1 (Portal 2), d1_trainstation_02 (Half-Life 2)')
parser.add_argument('--size', default='1280x720')
parser.add_argument('--label', default='')
parser.add_argument('--binary', help='default: the game\'s (build/portal2-native, build/hl2/hl2-native)')
parser.add_argument('--extra', default='', help='extra game arguments (convars: "+mat_forceaniso 1")')
args = parser.parse_args()
game = games.profile(localconfig.for_game(args.game))
args.map = args.map or {'portal2': 'sp_a1_intro1', 'hl2': 'd1_trainstation_02'}[game['name']]
args.binary = args.binary or game['binary']
width, height = args.size.split('x')
results = []
for run in range(args.runs):
    env = dict(os.environ, P2_FAKE_FOCUS='1', P2_FRAME_LOG='1')   # run_game.py sets SteamAppId
    subprocess.run([sys.executable, 'tools/run_game.py', '--game', game['name'], '--binary', args.binary,
                    '--timeout', str(args.timeout), '--', '-game', game['mod'], '-novid', '-windowed', '-w', width, '-h', height, *args.extra.split(), '+map', args.map],
                   env=env, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    log = (Path(game['build_dir']) / 'launch.log').read_text(errors='replace')
    if re.search(r'guest trap|host SIG', log):
        print(f'run {run}: erreur dans le journal', file=sys.stderr)
        sys.exit(1)
    fps = [float(m) for m in re.findall(r'\[diag\] frame \d+, ([0-9.]+) fps', log)]
    tail = fps[len(fps) // 2:]
    if tail:
        results.append(statistics.mean(tail))
load = os.getloadavg()[0]
if results:
    print(f'{args.label} {args.size}: {statistics.mean(results):.1f} fps '
          f'(lancements {", ".join(f"{r:.0f}" for r in results)}, charge {load:.1f})')
