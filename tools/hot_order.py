#!/usr/bin/env python3
"""Linker order file from P2_PROFILE_ORDER profiles.

Each profile lists the executable addresses seen in the samples ("self and
inclusive offsets", see runtime/profile.c). Addresses are resolved to symbols
with nm on the profiled executable; functions are ranked by self then inclusive
samples, and the linker lays them out in that order at the head of __text (hot
code grouped: fewer instruction cache and TLB misses in a ~380 MB binary).

    python3 tools/hot_order.py --binary build/portal2-native profile1.txt profile2.txt -o config/hot.order
"""
import argparse
import bisect
import collections
import subprocess
from pathlib import Path

TEXT_BASE = 0x100000000   # vmaddr de __TEXT (exécutable arm64 non PIE-décalé dans nm)


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('--binary', type=Path, default=Path('build/portal2-native'),
                        help='build/hl2/hl2-native for Half-Life 2')
    parser.add_argument('-o', '--output', type=Path, required=True)
    parser.add_argument('profiles', nargs='+', type=Path)
    args = parser.parse_args()
    listing = subprocess.run(['nm', '-n', str(args.binary)], capture_output=True, text=True, check=True).stdout
    addresses, names = [], []
    for line in listing.splitlines():
        parts = line.split()
        if len(parts) == 3 and parts[1] in 'tT':
            addresses.append(int(parts[0], 16)); names.append(parts[2])
    own, inclusive = collections.defaultdict(float), collections.defaultdict(float)
    for profile in args.profiles:
        total = sum(int(line.split()[1]) for line in profile.read_text().splitlines()) or 1
        for line in profile.read_text().splitlines():
            offset, self_hits, all_hits = line.split()
            # Adresse de retour (pile) : l'instruction d'appel est juste avant.
            pc = TEXT_BASE + int(offset, 16) - 1
            k = bisect.bisect_right(addresses, pc) - 1
            if k < 0: continue
            # Profils de durées différentes : chacun pèse pareil.
            own[names[k]] += int(self_hits) / total
            inclusive[names[k]] += int(all_hits) / total
    order = sorted(inclusive, key=lambda name: (-own[name], -inclusive[name]))
    args.output.write_text(''.join(f'{name}\n' for name in order))
    print(f'{len(order)} fonctions ({sum(n.startswith("_fn_") for n in order)} traduites) -> {args.output}')


if __name__ == '__main__':
    main()
