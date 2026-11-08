#!/usr/bin/env python3
"""Benchmark the generated code (tests/bench.c) on libtier0/libvstdlib.

Each configuration is a set of clang options (e.g. -DRT_FAST_MEMORY); the same
generated C is built with each one, then run several times. The checksums must
be identical across configurations.

    python3 tools/bench.py                      # default configuration
    python3 tools/bench.py --config base= --config fast=-DRT_FAST_MEMORY
"""
import argparse
import re
import statistics
import subprocess
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import lift  # noqa: E402
import localconfig  # noqa: E402
from build_game import HOST_CFLAGS, HOST_LIBS   # noqa: E402  single source of the host flags (with the Makefile)

ROOT = Path(__file__).resolve().parent.parent
EXPORTS = ['EncodeBinaryToString', 'DecodeBinaryFromString', 'V_UTF8ToUnicode', 'V_UnicodeToUTF8', 'KeyValuesSystem']


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    linux_root = localconfig.for_game('portal2').get('linux_root')
    ap.add_argument('--input-dir', type=Path, help='libtier0.so and libvstdlib.so (default: <linux_root>/bin/linux32 '
                    'of config/local.json, else ../work/portal2-linux/bin)',
                    default=Path(linux_root) / 'bin/linux32' if linux_root else ROOT.parent / 'work/portal2-linux/bin')
    ap.add_argument('--opt', default='1')
    ap.add_argument('--objdump', default='objdump')
    ap.add_argument('--config', action='append', help='name=space-separated clang options')
    ap.add_argument('--runs', type=int, default=5)
    ap.add_argument('--rounds', type=int, default=40)
    args = ap.parse_args()
    out = ROOT / 'build/bench'
    out.mkdir(parents=True, exist_ok=True)
    prep = out / 'prep'
    if not (prep / 'layout.json').exists():
        d = Path(args.input_dir)
        subprocess.run([sys.executable, str(ROOT / 'tools/elfprep.py'), '--out', str(prep),
                        f'{d / "libtier0.so"}@0x10000000', f'{d / "libvstdlib.so"}@0x11000000'], check=True)
    layout = lift.load_layout(prep / 'layout.json')
    # Même chemin que le jeu : génération répartie de tous les points d'entrée
    # connus des deux modules (méthodes virtuelles comprises).
    specs = list(EXPORTS)
    for module in layout['modules']:
        if module['init']:
            specs.append(f'{module["name"]}:init:dt')
        specs += [f'{module["name"]}:init:{i}' for i in range(len(module['init_array']))]
    manifest = lift.generate_sharded(layout, specs, True, args.objdump, str(out / 'disasm'), str(out / 'gen'),
                                     8, log=lambda *_: None)
    sources = [str(out / 'gen/src' / name) for name in sorted(manifest['shards'])] + [str(out / 'gen/src/dispatch.c')]
    configs = [c.split('=', 1) for c in (args.config or ['base='])]
    results = {}
    for name, flags in configs:
        exe = out / f'bench-{name}'
        runtime = sorted(str(p) for p in (ROOT / 'runtime').glob('*.c'))
        subprocess.run(['clang', f'-O{args.opt}', '-g', '-Wall', '-Wextra', '-Werror', *flags.split(),
                        f'-I{ROOT / "runtime"}', *HOST_CFLAGS, *runtime, *sources, str(ROOT / 'tests/bench.c'),
                        *HOST_LIBS, '-o', str(exe)], check=True)
        times, sums = {}, set()
        for _ in range(args.runs):
            text = subprocess.run([str(exe), str(prep), str(args.rounds)], check=True,
                                  capture_output=True, text=True).stdout
            for m in re.finditer(r'^(\w+)\s+([\d.]+) ms\s+checksum (\w+)', text, re.M):
                times.setdefault(m[1], []).append(float(m[2]))
                sums.add((m[1], m[3]))
        results[name] = ({k: statistics.median(v) for k, v in times.items()}, sorted(sums))
    base = None
    for name, (times, sums) in results.items():
        line = '  '.join(f'{k} {v:8.1f} ms' + (f' (x{base[k] / v:.2f})' if base else '') for k, v in times.items())
        print(f'{name:10s} {line}   {" ".join(s for _, s in sums)}')
        base = base or times
    if len({tuple(s) for _, s in results.values()}) != 1:
        raise SystemExit('bench: checksums differ between configurations')


if __name__ == '__main__':
    main()
