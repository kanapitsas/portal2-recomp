#!/usr/bin/env python3
"""Run the native executable against the user's real game data and log output."""
import argparse
import shutil
import os
from pathlib import Path
import subprocess
import sys

sys.path.insert(0, str(Path(__file__).resolve().parent))
import games  # noqa: E402
import language  # noqa: E402
import localconfig  # noqa: E402

parser = argparse.ArgumentParser(description=__doc__)
games.add_argument(parser)
parser.add_argument('--build', action='store_true')
parser.add_argument('--timeout', type=int, default=60)
parser.add_argument('--show', action='store_true', help='show the window (otherwise P2_HIDDEN_WINDOWS=1)')
parser.add_argument('--binary', type=Path,
                    help='executable to run (default: the game\'s, e.g. build/portal2-native; '
                         'A/B comparisons without touching it)')
parser.add_argument('arguments', nargs=argparse.REMAINDER)
args = parser.parse_args()
config = localconfig.for_game(args.game)
game = games.profile(config)
if not config.get('game_root'): sys.exit(f'{game["name"]} is not set up in config/local.json: run ./setup.sh --game {game["name"]}')
if args.build:
    result = subprocess.run([sys.executable, 'tools/build_game.py', '--game', game['name']])
    if result.returncode: sys.exit(result.returncode)
binary = args.binary or Path(game['binary'])
build_dir = Path(game['build_dir'])
game_args = args.arguments
if game_args[:1] == ['--']: game_args = game_args[1:]
if not game_args: game_args = ['-game', game['mod'], '-novid', '-windowed', '-w', '1280', '-h', '720']
# Langue du jeu (P2_LANGUAGE, config/local.json, Steam, système) : voir tools/language.py.
game_args = [*game_args, *language.language_args(config, game_args)]   # the game's config: its folder and appmanifest
# Les lancements de diagnostic (fenêtre cachée) travaillent sur une copie des
# données utilisateur : jamais deux instances sur les mêmes sauvegardes et
# réglages, ni de modification des réglages de l'utilisateur.
user_dir = localconfig.user_dir(config)
if not args.show:
    diag_dir = user_dir.with_name(f'{game["name"]}-userdata-diag')   # portal2-userdata-diag as before
    if not diag_dir.exists():
        if user_dir.is_dir(): shutil.copytree(user_dir, diag_dir, symlinks=True)
        else: diag_dir.mkdir(parents=True)   # no saves yet: the game starts from scratch
    user_dir = diag_dir
command = [str(binary.resolve()), '--images', str((build_dir / 'prep').resolve()),
           '--game-dir', config['game_root'], '--linux-root', config['linux_root'],
           '--user-dir', str(user_dir), '--', *game_args]
log = build_dir / 'launch.log'
with log.open('w') as output:
    try:
        env = dict(os.environ)
        # The game's Steam app and Valve library: the runtime would otherwise
        # read the top-level (Portal 2) keys of config/local.json.
        if not env.get('SteamAppId') and config.get('steam_appid'):
            env['SteamAppId'] = env['SteamGameId'] = str(config['steam_appid'])
        if config.get('steam_api_dylib'):
            env.setdefault('P2_STEAM_API_DYLIB', str(Path(config['steam_api_dylib']).expanduser()))
        # Rendu Metal : la bibliothèque déclarée par setup.sh (config d3d9metal_dylib),
        # sauf si P2_D3D9METAL est déjà fixé ; absente, le runtime reste en OpenGL.
        metal = config.get('d3d9metal_dylib')
        if metal and Path(metal).expanduser().is_file():
            env.setdefault('P2_D3D9METAL', str(Path(metal).expanduser()))
        if not args.show:
            env.setdefault('P2_HIDDEN_WINDOWS', '1')
            # Lancement de diagnostic : jamais de son sur les haut-parleurs de l'utilisateur.
            env.setdefault('SDL_AUDIODRIVER', 'dummy')
        result = subprocess.run(command, stdout=output, stderr=subprocess.STDOUT, timeout=args.timeout, env=env)
        status = result.returncode
    except subprocess.TimeoutExpired:
        status = 124
        output.write('\nNative launch exceeded the diagnostic timeout. This is not proof of a running game.\n')
print(log.read_text()[-12000:])
print(f'native process exit: {status}')
sys.exit(status if status >= 0 else 128 - status)
