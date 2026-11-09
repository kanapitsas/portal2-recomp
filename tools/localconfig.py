"""config/local.json, shared by the tools: the local installation (game_root,
linux_root, steam_api_dylib, user_dir...) written by ./setup.sh.

One file for both games. Top-level keys are shared (host_prefix,
d3d9metal_dylib, d3d9metal_ios, language...) plus Portal 2's own game keys
(GAME_KEYS), as in single-game configs; another game's keys go in a section:

    {"game_root": "<Portal 2>", ..., "games": {"hl2": {"game_root": ..., "steam_appid": 220, ...}}}

game_config(config, name) is the merged view a tool works with. An older
Half-Life 2 checkout ("game": "hl2" with its keys at the top level) is read as
if its keys were in games.hl2 (migrate()).

Keeps the single default for the saves folder (user_dir), the one the app
made by tools/make_app.py uses; the other tools derive their copies from it.
Each game has its own folder (games.py app_name); Portal 2's is unchanged.
"""
import json
import os
from pathlib import Path

import games

ROOT = Path(__file__).resolve().parent.parent
DEFAULT_PATH = ROOT / 'config/local.json'
APP_NAME = games.GAMES['portal2']['app_name']
# App-owned data, outside the repository: ./uninstall.sh removes this folder.
SUPPORT = Path.home() / 'Library/Application Support' / APP_NAME
HOMEBREW = Path('/opt/homebrew')


# Keys that belong to one game; any other key is shared by every game.
GAME_KEYS = ('game_root', 'linux_root', 'steam_appid', 'depot_id', 'manifest', 'steam_api_dylib', 'user_dir',
             'ios_bundle_id')


def load(path=None):
    """Content of config/local.json (or `path`); {} when the file does not exist."""
    path = Path(path) if path is not None else DEFAULT_PATH
    return json.loads(path.read_text()) if path.exists() else {}


def migrate(config):
    """Copy of `config` in the current format: an older single-game config of
    another game than Portal 2 ("game": "hl2", game keys at the top level, no
    games.hl2) has those keys moved into its section."""
    config = dict(config)
    name = config.get('game')
    sections = dict(config.get('games') or {})
    if name and name != games.DEFAULT and name in games.GAMES and name not in sections:
        moved = {key: config.pop(key) for key in GAME_KEYS if key in config}
        if moved:
            sections[name] = moved
            config['games'] = sections
    return config


def game_config(config, name=None):
    """Merged view of `config` for one game (games.selected() when None): the
    shared keys, that game's keys, and "game": its name, so games.profile()
    and support_dir() apply to it. Portal 2's keys are the top-level ones
    (a games.portal2 section, if any, overrides them)."""
    config = migrate(config)
    name = games.selected(name, config)
    sections = config.get('games') or {}
    merged = {key: value for key, value in config.items() if key != 'games' and
              (key not in GAME_KEYS or name == games.DEFAULT)}
    section = sections.get(name)
    if isinstance(section, dict):
        merged.update(section)
    merged['game'] = name
    return merged


def for_game(name=None, path=None):
    """game_config() of config/local.json (or `path`)."""
    return game_config(load(path), name)


def configured_games(config):
    """Games that `config` sets up (a game_root), in games.GAMES order."""
    return [name for name in games.GAMES if game_config(config, name).get('game_root')]


def support_dir(config):
    """App-owned data folder of the configured game:
    ~/Library/Application Support/<app name> (SUPPORT for Portal 2)."""
    name = games.profile(config)['app_name']
    return SUPPORT if name == APP_NAME else SUPPORT.with_name(name)


def user_dir(config, support=None):
    """Saves and settings folder: user_dir of the config, else
    <support>/userdata, by default in the game's own support_dir."""
    if config.get('user_dir'):
        return Path(config['user_dir']).expanduser().resolve()
    return (support_dir(config) if support is None else support) / 'userdata'


def host_prefix(config=None):
    """Prefix of the host libraries (SDL2, FreeType, Fontconfig): P2_HOST_PREFIX,
    else host_prefix of the config (the prebuilt bundle setup.sh downloads),
    else Homebrew."""
    if os.environ.get('P2_HOST_PREFIX'):
        return Path(os.environ['P2_HOST_PREFIX']).expanduser()
    config = load() if config is None else config
    return Path(config['host_prefix']).expanduser() if config.get('host_prefix') else HOMEBREW


def host_flags(prefix=None):
    """Compiler and linker flags for the host libraries under `prefix`. The
    rpath lets build/portal2-native find the bundle's @rpath libraries;
    tools/make_app.py replaces it with the app's Frameworks folder."""
    prefix = host_prefix() if prefix is None else Path(prefix)
    cflags = [f'-I{prefix}/include', f'-I{prefix}/include/freetype2', '-DGL_SILENCE_DEPRECATION',
              '-DOPENAL_DEPRECATED=', '-DAL_SILENCE_DEPRECATION']
    libs = [f'-L{prefix}/lib', f'-Wl,-rpath,{prefix}/lib', '-lSDL2', '-lfreetype', '-lfontconfig', '-liconv',
            '-framework', 'OpenGL', '-framework', 'OpenAL', '-lobjc']
    return cflags, libs


def main(argv=None):
    """`python3 tools/localconfig.py --make KEY... [--game NAME]`: the Makefile's
    one-shot read of config/local.json. Prints `KEY=value` words on one line,
    spaces in values encoded as %20 (Make splits $(shell) output on
    whitespace); an unset key prints nothing. host_prefix is host_prefix()
    (P2_HOST_PREFIX, the config, else Homebrew)."""
    import argparse
    parser = argparse.ArgumentParser(description=__doc__)
    games.add_argument(parser)
    parser.add_argument('--make', nargs='+', metavar='KEY', required=True, help='keys to print as KEY=value words')
    args = parser.parse_args(argv)
    config = for_game(args.game)
    words = []
    for key in args.make:
        value = str(host_prefix(config)) if key == 'host_prefix' else config.get(key)
        if value: words.append(f'{key}={str(value).replace(" ", "%20")}')
    print(' '.join(words))
    return 0


if __name__ == '__main__':
    import sys
    sys.exit(main())
