"""Command line and environment of a native launch, shared by tools/run_game.py
(diagnostic runs, tools/bench_game.py, tools/discover_game.py) and
tools/bench_renderer.py. The apps (tools/make_app.py) have their own launcher
script and only mirror these choices.
"""
import os
from pathlib import Path


def command(binary, images, game_root, linux_root, user_dir, game_args):
    """Argument list of the native executable: `--images` (prepared ELF copies),
    the game copy, the Linux depot, the saves folder, then the game's own
    arguments after `--`."""
    return [str(Path(binary).resolve()), '--images', str(Path(images).resolve()),
            '--game-dir', str(game_root), '--linux-root', str(linux_root),
            '--user-dir', str(user_dir), '--', *game_args]


def environment(config, game, base=None, hidden=False, metal=True):
    """Environment of a launch, from `base` (the process environment by default)
    and one game's merged config (localconfig.game_config) and profile
    (games.profile): the Steam app id and Valve library (the runtime would
    otherwise read the top-level, Portal 2, keys of config/local.json), the
    Metal library declared by setup.sh (d3d9metal_dylib, unless `metal` is
    false or P2_D3D9METAL is already set; absent, the runtime stays on OpenGL).
    `hidden`: diagnostic launch, hidden window and no sound on the user's
    speakers. Values already present in `base` win."""
    env = dict(os.environ if base is None else base)
    appid = config.get('steam_appid') or game.get('steam_appid')
    if not env.get('SteamAppId') and appid:
        env['SteamAppId'] = env['SteamGameId'] = str(appid)
    if config.get('steam_api_dylib'):
        env.setdefault('P2_STEAM_API_DYLIB', str(Path(config['steam_api_dylib']).expanduser()))
    dylib = config.get('d3d9metal_dylib')
    if metal and dylib and Path(dylib).expanduser().is_file():
        env.setdefault('P2_D3D9METAL', str(Path(dylib).expanduser()))
    if hidden:
        env.setdefault('P2_HIDDEN_WINDOWS', '1')
        env.setdefault('SDL_AUDIODRIVER', 'dummy')
    return env
