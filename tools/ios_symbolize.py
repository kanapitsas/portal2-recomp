#!/usr/bin/env python3
"""Resolve and aggregate a raw iOS profile (P2_PROFILE_RAW=1, .raw file).

Shared-cache frames are "library+offset"; atos resolves them with the device
symbols copied by Xcode
(~/Library/Developer/Xcode/iOS DeviceSupport/<device>/arm64e/Symbols).

    python3 tools/ios_symbolize.py build/ios/device/profile.txt.raw [--all-threads] [--top 40]
"""
import argparse
import collections
import re
import subprocess
from pathlib import Path

SUPPORT = Path.home() / 'Library/Developer/Xcode/iOS DeviceSupport'
LOCATED = re.compile(r'^(.+)\+0x([0-9a-f]+)$')


def symbol_files():
    """Symbol file by name, preferring the most recently written DeviceSupport folder."""
    found = {}
    for root in sorted(SUPPORT.glob('*/*/Symbols'), key=lambda p: p.stat().st_mtime, reverse=True):
        for path in root.rglob('*'):
            if path.is_file() and '.dSYM' not in str(path) and path.suffix in ('', '.dylib'):
                found.setdefault(path.name, path)
    return found


def resolve(tokens):
    files, by_lib, names = symbol_files(), collections.defaultdict(set), {}
    for token in tokens:
        m = LOCATED.match(token)
        if m: by_lib[m.group(1)].add(int(m.group(2), 16))
    for lib, offsets in by_lib.items():
        path = files.get(lib)
        offsets = sorted(offsets)
        if not path:
            for o in offsets: names[f'{lib}+{o:#x}'] = f'{lib}+{o:#x}'
            continue
        # Adresse de chargement fictive : décalage + __TEXT de l'image.
        out = subprocess.run(['atos', '-o', str(path), '-l', '0x0', *[f'{o:#x}' for o in offsets]],
                             capture_output=True, text=True).stdout.splitlines()
        for o, line in zip(offsets, out):
            name = re.sub(r' \(in [^)]*\)', '', line).split(' + ')[0].strip()
            names[f'{lib}+{o:#x}'] = f'{name} [{lib}]' if name and not name.startswith('0x') else f'{lib}+{o:#x}'
    return names


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('raw', type=Path)
    ap.add_argument('--all-threads', action='store_true')
    ap.add_argument('--top', type=int, default=40)
    args = ap.parse_args()
    rows = []
    for line in args.raw.read_text().splitlines():
        parts = line.split('\t')
        if len(parts) < 3: continue
        if not args.all_threads and parts[1] != '1': continue
        rows.append((int(parts[0]), parts[2:]))
    names = resolve({t for _, frames in rows for t in frames})
    total = sum(h for h, _ in rows) or 1
    leaf, inclusive = collections.Counter(), collections.Counter()
    for hits, frames in rows:
        resolved = [names.get(f, f) for f in frames]
        leaf[resolved[0]] += hits
        for name in set(resolved): inclusive[name] += hits
    print(f'{total} samples')
    print('== leaf')
    for name, n in leaf.most_common(args.top): print(f'{100 * n / total:6.2f}% {name}')
    print('== inclusive (function present in the stack)')
    for name, n in inclusive.most_common(args.top): print(f'{100 * n / total:6.2f}% {name}')


if __name__ == '__main__':
    main()
