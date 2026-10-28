#!/usr/bin/env python3
"""Run the native executable against the user's real game data and log output."""
import argparse
import json
import shutil
import os
from pathlib import Path
import subprocess
import sys

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--build', action='store_true')
parser.add_argument('--timeout', type=int, default=60)
parser.add_argument('--show', action='store_true', help='afficher la fenêtre (sinon P2_HIDDEN_WINDOWS=1)')
parser.add_argument('--binary', type=Path, default=Path('build/portal2-native'),
                    help='exécutable à lancer (comparaisons A/B sans toucher build/portal2-native)')
parser.add_argument('arguments', nargs=argparse.REMAINDER)
args = parser.parse_args()
if args.build:
    result = subprocess.run([sys.executable, 'tools/build_game.py'])
    if result.returncode: sys.exit(result.returncode)
config = json.loads(Path('config/local.json').read_text())
game_args = args.arguments
if game_args[:1] == ['--']: game_args = game_args[1:]
if not game_args: game_args = ['-game', 'portal2', '-novid', '-windowed', '-w', '1280', '-h', '720']
# Langue du jeu (P2_LANGUAGE, config/local.json, Steam, système) : voir tools/language.py.
language_args = subprocess.run([sys.executable, str(Path(__file__).with_name('language.py')), '--', *game_args],
                               capture_output=True, text=True).stdout.split()
game_args = [*game_args, *language_args]
# Les lancements de diagnostic (fenêtre cachée) travaillent sur une copie des
# données utilisateur : jamais deux instances sur les mêmes sauvegardes et
# réglages, ni de modification des réglages de l'utilisateur.
user_dir = Path(config.get('user_dir') or '../work/portal2-userdata').expanduser().resolve()
if not args.show:
    diag_dir = user_dir.with_name('portal2-userdata-diag')
    if not diag_dir.exists(): shutil.copytree(user_dir, diag_dir, symlinks=True)
    user_dir = diag_dir
command = [str(args.binary.resolve()), '--images', str(Path('build/game/prep').resolve()),
           '--game-dir', config['game_root'], '--linux-root', config['linux_root'],
           '--user-dir', str(user_dir), '--', *game_args]
log = Path('build/game/launch.log')
with log.open('w') as output:
    try:
        env = dict(os.environ)
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
