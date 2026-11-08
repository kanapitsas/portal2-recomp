#!/usr/bin/env python3
"""Learn missing AOT roots from real native runs; never emulate or skip code.

Stops immediately at a missing instruction, import, signal or timeout. It only
adds genuine code addresses that belong to prepared executable ELF segments.
"""
import argparse
import json
import re
import subprocess
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from table_roots import adjacent_roots  # noqa: E402
import games  # noqa: E402
import lift  # noqa: E402
import localconfig  # noqa: E402
import run_game  # noqa: E402


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    games.add_argument(parser)
    parser.add_argument('--steps', type=int, default=10)
    parser.add_argument('--timeout', type=int, default=30)
    parser.add_argument('--opt', default='0', choices=['0', '1', '2', '3'])
    parser.add_argument('game_args', nargs=argparse.REMAINDER, help='after --: game arguments (e.g. +load save)')
    args = parser.parse_args(argv)
    config = localconfig.for_game(args.game)
    game = games.profile(config)
    for key in ('game_root', 'linux_root'):
        if not config.get(key):
            sys.exit(f'{game["name"]} is not set up in config/local.json ({key} missing): '
                     f'run ./setup.sh --game {game["name"]}')
    build_dir = games.ROOT / game['build_dir']
    layout = lift.load_layout(build_dir / 'prep/layout.json')
    roots_path = games.ROOT / game['roots']
    for attempt in range(args.steps):
        built = subprocess.run([sys.executable, '-u', str(games.ROOT / 'tools/build_game.py'), '--game', game['name'],
                                '--opt', args.opt])
        if built.returncode: return built.returncode
        status, log = run_game.run(config, game, args.game_args, timeout=args.timeout)
        if not status:
            print('The native process returned normally; this is not a launch-success claim.')
            break
        text = log.read_text()
        matches = re.findall(r'guest trap at (0x[0-9a-f]+).*?: target not recompiled', text)
        if not matches:
            print(text[-4000:])
            return status if status >= 0 else 128 - status
        address = int(matches[-1], 16)
        module = next((m for m in layout['modules'] if any(s['flags'] & 1 and s['vaddr'] <= address < s['vaddr'] + s['filesz'] for s in m['loads'])), None)
        if not module:
            parser.exit(1, f'Uncompiled target {address:#x} is outside prepared executable segments.\n')
        roots = json.loads(roots_path.read_text()) if roots_path.exists() else []
        spec = f'{module["name"]}:code:{address - module["base"]:#x}'
        if spec in roots:
            parser.exit(1, f'Target {spec} is already a root; inspect generation/dispatch rather than retrying.\n')
        new_roots = adjacent_roots(layout, address) or [spec]
        if address == module['init'] or address in module['init_array']:
            new_roots = [f'{module["name"]}:init:dt'] + [f'{module["name"]}:init:{i}' for i in range(len(module['init_array']))]
        roots_path.write_text(json.dumps(list(dict.fromkeys(roots + new_roots)), indent=2) + '\n')
        print('Added real ELF roots: ' + ', '.join(new_roots), flush=True)
    else:
        print(f'Stopped after {args.steps} discovery iterations; no launch-success claim.')
    return 0


if __name__ == '__main__':
    sys.exit(main())
