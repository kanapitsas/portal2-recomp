#!/usr/bin/env python3
"""Bundle identifier of the iOS app (CFBundleIdentifier).

Each Apple account must sign under its own identifier: P2_BUNDLE_ID, else
"ios_bundle_id" in config/local.json, else an example identifier that is enough
for the simulator. The probe (ios/probe) uses the identifier followed by ".probe".

    python3 tools/ios_bundle_id.py [--probe]
"""
import json
import os
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
DEFAULT = 'com.example.portal2-recomp'


def bundle_id(probe=False):
    value = os.environ.get('P2_BUNDLE_ID')
    if not value:
        try:
            value = json.loads((ROOT / 'config/local.json').read_text()).get('ios_bundle_id')
        except FileNotFoundError:
            value = None
    value = value or DEFAULT
    return value + '.probe' if probe else value


if __name__ == '__main__':
    print(bundle_id('--probe' in sys.argv[1:]))
