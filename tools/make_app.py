#!/usr/bin/env python3
"""Crée « Portal 2 Natif.app », lançable depuis le Finder ou le Dock.

L'application ne recopie pas le jeu : Contents/MacOS/portal2-native est un
lien vers build/portal2-native de ce dépôt (une recompilation est donc prise en
compte sans refaire l'app), et le lanceur reprend les chemins de
config/local.json. Il démarre Steam s'il ne tourne pas et écrit le journal dans ~/Library/Logs/Portal2Natif.log.

Les modules préparés (build/game/prep, ~75 Mo) sont copiés dans
Contents/Resources/prep : refaire l'app après tools/gameprep.py. Sans user_dir
dans config/local.json, les sauvegardes vont dans
~/Library/Application Support/Portal 2 Natif/userdata (l'ancien défaut
../work/portal2-userdata y est recopié une fois). ./uninstall.sh retire le tout.

    python3 tools/make_app.py                      # ~/Applications
    python3 tools/make_app.py --dest /Applications
    python3 tools/make_app.py -- -game portal2 -novid -windowed -w 2560 -h 1440
"""
import argparse
import json
import plistlib
import shlex
import shutil
import stat
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
NAME = 'Portal 2 Natif'
# Sans -w/-h/-windowed : résolution et mode choisis dans le menu Vidéo du jeu
# (enregistrés dans update/cfg/video.txt) ; Retina selon la résolution.
DEFAULT_ARGS = ['-game', 'portal2', '-novid']
# Données propres à l'app, hors du dépôt : ./uninstall.sh supprime ce dossier.
SUPPORT = Path.home() / 'Library/Application Support' / NAME
OLD_USER_DIR = '../work/portal2-userdata'  # ancien défaut, relatif au dépôt

LAUNCHER = '''#!/bin/bash
# Lanceur généré par tools/make_app.py : le modifier là-bas.
LOG="$HOME/Library/Logs/Portal2Natif.log"
HERE="$(cd "$(dirname "$0")" && pwd)"
alert() {{ /usr/bin/osascript -e "display alert \\"{name}\\" message \\"$1\\" as critical" >/dev/null 2>&1; }}
if [ ! -x {binary} ]; then alert "Exécutable introuvable : {binary_text}. Lancer tools/build_game.py --release."; exit 1; fi
if [ ! -d {game_dir} ]; then alert "Données du jeu introuvables (disque externe débranché ?) : {game_dir_text}"; exit 1; fi
# L'API Steam a besoin du client Steam ouvert et connecté.
if ! /usr/bin/pgrep -qx steam_osx; then
    /usr/bin/open -ga Steam
    for _ in $(seq 60); do /usr/bin/pgrep -qx steam_osx && break; sleep 0.5; done
    sleep 5
fi
export SteamAppId=620 SteamGameId=620
mkdir -p "$(dirname "$LOG")" {user_dir}
# Langue : P2_LANGUAGE, "language" de config/local.json, Steam, système (tools/language.py).
# Un -language passé au lanceur l'emporte ; en cas d'échec, le jeu reste en anglais.
LANGUAGE_ARGS=$(/usr/bin/python3 {language_script} -- {game_args} "$@" 2>/dev/null)
exec "$HERE/portal2-native" --images "$(dirname "$HERE")/Resources/prep" --game-dir {game_dir} --linux-root {linux_root} \\
    --user-dir {user_dir} -- {game_args} $LANGUAGE_ARGS "$@" >"$LOG" 2>&1
'''


def user_dir(config):
    """Dossier des sauvegardes : user_dir de la config, sinon SUPPORT/userdata,
    initialisé une fois par copie (jamais déplacement) de l'ancien défaut."""
    if config.get('user_dir'): return Path(config['user_dir']).expanduser().resolve()
    new, old = SUPPORT / 'userdata', ROOT / OLD_USER_DIR
    if not new.exists() and old.is_dir():
        partial = new.with_name(new.name + '.partial')
        if partial.exists(): shutil.rmtree(partial)
        shutil.copytree(old, partial, symlinks=True)
        partial.rename(new)
        print(f'saves copied: {old.resolve()} -> {new} (original kept)')
    return new


def copy_tree(source, dest):
    """Copie source vers dest ; clones APFS (cp -c, sans espace disque en plus) si possible."""
    if dest.exists(): shutil.rmtree(dest)
    if subprocess.run(['/bin/cp', '-cR', str(source), str(dest)], capture_output=True).returncode:
        if dest.exists(): shutil.rmtree(dest)
        shutil.copytree(source, dest)


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('--dest', type=Path, default=Path.home() / 'Applications')
    parser.add_argument('game_args', nargs=argparse.REMAINDER)
    args = parser.parse_args()
    game_args = args.game_args[1:] if args.game_args[:1] == ['--'] else args.game_args
    config = json.loads((ROOT / 'config/local.json').read_text())
    binary = ROOT / 'build/portal2-native'
    prep = ROOT / 'build/game/prep'
    if not (prep / 'layout.json').exists(): raise SystemExit(f'{prep} absent : lancer tools/gameprep.py')
    paths = {
        'binary': binary, 'game_dir': Path(config['game_root']),
        'linux_root': Path(config['linux_root']), 'user_dir': user_dir(config),
    }

    app = args.dest / f'{NAME}.app'
    if app.exists(): shutil.rmtree(app)
    macos = app / 'Contents/MacOS'
    resources = app / 'Contents/Resources'
    macos.mkdir(parents=True)
    resources.mkdir()
    # Dans l'app plutôt que dans Application Support : 75 Mo liés à cette
    # compilation précise, retirés avec l'app (corbeille comprise).
    copy_tree(prep, resources / 'prep')

    launcher = macos / 'launcher'
    launcher.write_text(LAUNCHER.format(
        name=NAME, language_script=shlex.quote(str(ROOT / 'tools/language.py')), game_args=shlex.join(game_args or DEFAULT_ARGS),
        binary_text=str(binary), game_dir_text=str(paths['game_dir']).replace('"', ''),
        **{key: shlex.quote(str(value)) for key, value in paths.items()}))
    launcher.chmod(launcher.stat().st_mode | stat.S_IXUSR | stat.S_IXGRP | stat.S_IXOTH)
    # Lien et non copie (354 Mo) ; lancé par ce chemin, le processus appartient
    # à l'app (icône du Dock, nom dans le menu).
    (macos / 'portal2-native').symlink_to(binary)

    icon = paths['game_dir'] / 'portal2/resource/game.icns'
    if icon.exists(): shutil.copy(icon, resources / 'portal2.icns')
    plist = {
        'CFBundleName': NAME, 'CFBundleDisplayName': NAME, 'CFBundleIdentifier': 'fr.kanapitsas.portal2-native',
        'CFBundleExecutable': 'launcher', 'CFBundlePackageType': 'APPL', 'CFBundleVersion': '1',
        'CFBundleShortVersionString': '1.0', 'CFBundleIconFile': 'portal2', 'LSMinimumSystemVersion': '13.0',
        'NSHighResolutionCapable': True, 'LSApplicationCategoryType': 'public.app-category.games',
    }
    with (app / 'Contents/Info.plist').open('wb') as f: plistlib.dump(plist, f)
    print(app)


if __name__ == '__main__':
    main()
