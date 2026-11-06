#!/usr/bin/env python3
"""Learn missing AOT roots from real native runs; never emulate or skip code.

Stops immediately at a missing instruction, import, signal or timeout. It only
adds genuine code addresses that belong to prepared executable ELF segments.
"""
import argparse
import json
from pathlib import Path
import re
import subprocess
import sys
from table_roots import adjacent_roots
import games
import localconfig

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--steps', type=int, default=10)
parser.add_argument('--timeout', type=int, default=30)
parser.add_argument('--opt', default='0', choices=['0','1','2','3'])
parser.add_argument('game_args', nargs=argparse.REMAINDER, help='after --: game arguments (e.g. +load save)')
args = parser.parse_args()
layout = json.loads(Path('build/game/prep/layout.json').read_text())
roots_path = Path(games.profile(localconfig.load())['roots'])
for attempt in range(args.steps):
    built = subprocess.run([sys.executable, '-u', 'tools/build_game.py', '--opt', args.opt])
    if built.returncode: sys.exit(built.returncode)
    result = subprocess.run([sys.executable, 'tools/run_game.py', '--timeout', str(args.timeout), *args.game_args])
    if not result.returncode:
        print('The native process returned normally; this is not a launch-success claim.')
        break
    log = Path('build/game/launch.log').read_text()
    matches = re.findall(r'guest trap at (0x[0-9a-f]+).*?: target not recompiled', log)
    if not matches:
        sys.exit(result.returncode)
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
