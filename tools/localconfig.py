"""config/local.json, shared by the tools: the local installation (game_root,
linux_root, steam_api_dylib, user_dir...) written by ./setup.sh.

Keeps the single default for the saves folder (user_dir), the one the app
made by tools/make_app.py uses; the other tools derive their copies from it.
"""
import json
import os
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
DEFAULT_PATH = ROOT / 'config/local.json'
APP_NAME = 'Portal 2 Natif'
# App-owned data, outside the repository: ./uninstall.sh removes this folder.
SUPPORT = Path.home() / 'Library/Application Support' / APP_NAME
HOMEBREW = Path('/opt/homebrew')


def load(path=None):
    """Content of config/local.json (or `path`); {} when the file does not exist."""
    path = Path(path) if path is not None else DEFAULT_PATH
    return json.loads(path.read_text()) if path.exists() else {}


def user_dir(config, support=SUPPORT):
    """Saves and settings folder: user_dir of the config, else <support>/userdata."""
    if config.get('user_dir'):
        return Path(config['user_dir']).expanduser().resolve()
    return support / 'userdata'


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
