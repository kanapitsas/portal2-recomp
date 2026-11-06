#!/usr/bin/env python3
"""Prepare the real game's Linux modules using a local installation config."""
import argparse
import json
from pathlib import Path
import subprocess
import sys
from elfprep import Elf
import games


def plan(config):
    game = games.profile(config)
    root = Path(config['linux_root'])
    paths = [p for d in game['lib_dirs'] for p in sorted((root / d).glob('*.so'))]
    # Excluded modules (tools, DXVK...): see tools/games.py.
    paths = [p for p in paths if p.name not in game['excluded']]
    order = ['libtier0.so', 'libvstdlib.so', 'libsteam_api.so', 'libtogl.so']
    by_name = {p.name: p for p in paths}
    paths = [by_name.pop(name) for name in order if name in by_name] + sorted(by_name.values())
    result = []
    base = 0x10000000
    for p in paths:
        module = Elf(p, base, len(result) + 1)
        extent = max(load[2] + load[5] for load in module.loads)
        result.append((p, base, extent))
        # Preserve the existing two-library spike's guest addresses.
        step = 0x1000000 if len(result) <= 2 else 0x100000
        base = (base + extent + step - 1) & ~(step - 1)
    p = root / game['program']
    if p.is_file():
        module = Elf(p, base, len(result) + 1)
        extent = max(load[2] + load[5] for load in module.loads)
        result.append((p, base, extent))
        base += extent
    if base >= 0x70000000:
        raise ValueError('module plan overlaps reserved guest stack space')
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--config', type=Path, default=Path('config/local.json'))
    parser.add_argument('--out', type=Path, default=Path('build/game/prep'))
    args = parser.parse_args()
    config = json.loads(args.config.read_text())
    modules = plan(config)
    args.out.mkdir(parents=True, exist_ok=True)
    (args.out / 'plan.json').write_text(json.dumps([{'path': str(p), 'base': hex(base), 'size': size}
                                                 for p, base, size in modules], indent=2) + '\n')
    command = [sys.executable, str(Path(__file__).with_name('elfprep.py')), '--out', str(args.out),
               '--program', games.profile(config)['program']]
    command.extend(f'{p}@{base:#x}' for p, base, _ in modules)
    subprocess.run(command, check=True)


if __name__ == '__main__':
    main()
