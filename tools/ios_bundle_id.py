#!/usr/bin/env python3
"""Bundle identifier of a game's iOS app (CFBundleIdentifier).

Each Apple account must sign under its own identifier: P2_BUNDLE_ID, else the
game's "ios_bundle_id" in config/local.json (localconfig.game_config: Portal 2's
at the top level, Half-Life 2's in games.hl2), else an example identifier that
is enough for the simulator, one per game so that one app does not replace the
other. The probe (ios/probe) uses the identifier followed by ".probe".

    python3 tools/ios_bundle_id.py [--game portal2|hl2] [--probe]
"""
import argparse
import os
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import games  # noqa: E402
import localconfig  # noqa: E402

DEFAULT = 'com.example.portal2-recomp'


def bundle_id(probe=False, game=None, config=None):
    """`game`: a game name, or None for games.selected(); `config`: the raw
    config (default config/local.json)."""
    merged = localconfig.game_config(localconfig.load() if config is None else config, game)
    name = merged['game']
    value = os.environ.get('P2_BUNDLE_ID') or merged.get('ios_bundle_id')
    if not value:
        value = DEFAULT if name == games.DEFAULT else f'com.example.{name}-recomp'
    return value + '.probe' if probe else value


if __name__ == '__main__':
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    games.add_argument(ap)
    ap.add_argument('--probe', action='store_true')
    args = ap.parse_args()
    print(bundle_id(args.probe, args.game))
