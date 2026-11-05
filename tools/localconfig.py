"""config/local.json, shared by the tools: the local installation (game_root,
linux_root, steam_api_dylib, user_dir...) written by ./setup.sh.

Keeps the single default for the saves folder (user_dir), the one the app
made by tools/make_app.py uses; the other tools derive their copies from it.
"""
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
DEFAULT_PATH = ROOT / 'config/local.json'
APP_NAME = 'Portal 2 Natif'
# App-owned data, outside the repository: ./uninstall.sh removes this folder.
SUPPORT = Path.home() / 'Library/Application Support' / APP_NAME


def load(path=None):
    """Content of config/local.json (or `path`); {} when the file does not exist."""
    path = Path(path) if path is not None else DEFAULT_PATH
    return json.loads(path.read_text()) if path.exists() else {}


def user_dir(config, support=SUPPORT):
    """Saves and settings folder: user_dir of the config, else <support>/userdata."""
    if config.get('user_dir'):
        return Path(config['user_dir']).expanduser().resolve()
    return support / 'userdata'
