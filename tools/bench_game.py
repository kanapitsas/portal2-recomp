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
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import games  # noqa: E402
import localconfig  # noqa: E402
import run_game  # noqa: E402

DEFAULT_MAPS = {'portal2': 'sp_a1_intro1', 'hl2': 'd1_trainstation_02'}


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    games.add_argument(parser)
    parser.add_argument('--runs', type=int, default=3)
    parser.add_argument('--timeout', type=int, default=150)
    parser.add_argument('--map', help='default: sp_a1_intro1 (Portal 2), d1_trainstation_02 (Half-Life 2)')
    parser.add_argument('--size', default='1280x720')
    parser.add_argument('--label', default='')
    parser.add_argument('--binary', help='default: the game\'s (build/portal2-native, build/hl2/hl2-native)')
    parser.add_argument('--extra', default='', help='extra game arguments (convars: "+mat_forceaniso 1")')
    args = parser.parse_args(argv)
    config = localconfig.for_game(args.game)
    game = games.profile(config)
    for key in ('game_root', 'linux_root'):
        if not config.get(key):
            sys.exit(f'{game["name"]} is not set up in config/local.json ({key} missing): '
                     f'run ./setup.sh --game {game["name"]}')
    args.map = args.map or DEFAULT_MAPS[game['name']]
    width, height = args.size.split('x')
    results = []
    for run in range(args.runs):
        env = dict(os.environ, P2_FAKE_FOCUS='1', P2_FRAME_LOG='1')   # run_game sets SteamAppId
        game_args = ['-game', game['mod'], '-novid', '-windowed', '-w', width, '-h', height,
                     *args.extra.split(), '+map', args.map]
        status, log = run_game.run(config, game, game_args, binary=args.binary, timeout=args.timeout, env=env)
        text = log.read_text(errors='replace')
        # The game runs until the timeout: that is the normal end of a bench run.
        if status not in (0, run_game.TIMEOUT_STATUS) or re.search(r'guest trap|host SIG', text):
            print(f'run {run}: the game ended with status {status}; last log lines:\n{text[-3000:]}', file=sys.stderr)
            return 1
        fps = [float(m) for m in re.findall(r'\[diag\] frame \d+, ([0-9.]+) fps', text)]
        tail = fps[len(fps) // 2:]
        if tail:
            results.append(statistics.mean(tail))
    load = os.getloadavg()[0]
    if results:
        print(f'{args.label} {args.size}: {statistics.mean(results):.1f} fps '
              f'(lancements {", ".join(f"{r:.0f}" for r in results)}, charge {load:.1f})')
    return 0


if __name__ == '__main__':
    sys.exit(main())
