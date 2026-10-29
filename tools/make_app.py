#!/usr/bin/env python3
"""Crée « Portal 2 Natif.app », lançable depuis le Finder ou le Dock.

L'application ne recopie pas les données du jeu, mais embarque une copie de
build/portal2-native et les dylibs Homebrew dont il dépend
(Contents/Frameworks) : elle fonctionne sans Homebrew. Après une recompilation,
relancer ce script pour mettre l'app à jour. Le lanceur reprend les chemins de
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
import sys
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
if [ ! -x "$HERE/portal2-native" ]; then alert "Exécutable introuvable dans l'app. Relancer tools/make_app.py."; exit 1; fi
if [ ! -d {game_dir} ]; then alert "Données du jeu introuvables (disque externe débranché ?) : {game_dir_text}"; exit 1; fi
# L'API Steam a besoin du client Steam ouvert et connecté.
if ! /usr/bin/pgrep -qx steam_osx; then
    /usr/bin/open -ga Steam
    for _ in $(seq 60); do /usr/bin/pgrep -qx steam_osx && break; sleep 0.5; done
    sleep 5
fi
export SteamAppId=620 SteamGameId=620
# libsteam_api.dylib de Valve, copiée dans l'app : le runtime la cherche sinon
# à côté du dépôt, absent ici.
export P2_STEAM_API_DYLIB="$HERE/../Frameworks/libsteam_api.dylib"
# Configuration Fontconfig embarquée (sinon cherchée dans /opt/homebrew/etc/fonts).
[ -d "$HERE/../Resources/fonts" ] && export FONTCONFIG_PATH="$HERE/../Resources/fonts"
mkdir -p "$(dirname "$LOG")" {user_dir}
# Langue : P2_LANGUAGE, "language" de config/local.json, Steam, système (tools/language.py).
# Un -language passé au lanceur l'emporte ; en cas d'échec, le jeu reste en anglais.
LANGUAGE_ARGS=$(/usr/bin/python3 "$HERE/../Resources/language.py" --config "$HERE/../Resources/local.json" -- {game_args} "$@" 2>/dev/null)
exec "$HERE/portal2-native" --images "$(dirname "$HERE")/Resources/prep" --game-dir {game_dir} --linux-root {linux_root} \\
    --user-dir {user_dir} -- {game_args} $LANGUAGE_ARGS "$@" >"$LOG" 2>&1
'''


# --- Bibliothèques embarquées ----------------------------------------------
# On compile contre Homebrew (SDL2 = sdl2-compat, FreeType, Fontconfig), mais
# l'app ne doit pas en dépendre à l'exécution : ces dylibs et leurs dépendances
# non système sont copiées dans Contents/Frameworks, référencées par @rpath.
# @executable_path suit les liens symboliques : l'exécutable est donc copié
# (~300 Mo ; install_name_tool et codesign réécrivent tout le fichier, un clone
# APFS n'économiserait rien).
HOMEBREW = Path('/opt/homebrew')
SYSTEM_PREFIXES = ('/usr/lib/', '/System/')
FRAMEWORKS_RPATH = '@executable_path/../Frameworks'
# sdl2-compat ne lie pas SDL3 : il le charge par dlopen, en cherchant d'abord
# @loader_path/libSDL3.dylib, donc à côté de lui dans Frameworks.
SDL3 = HOMEBREW / 'opt/sdl3/lib/libSDL3.0.dylib'
SDL3_NAME = 'libSDL3.dylib'


def run(*cmd):
    return subprocess.run(cmd, check=True, capture_output=True, text=True).stdout


def macho_deps(path):
    """Dépendances (LC_LOAD_*) d'un Mach-O, sans son propre identifiant."""
    lines = run('otool', '-L', str(path)).splitlines()[1:]
    deps = [line.strip().rsplit(' (compatibility', 1)[0] for line in lines if line.strip()]
    ident = run('otool', '-D', str(path)).splitlines()[1:]
    return [d for d in deps if d not in ident]


def macho_rpaths(path):
    out = run('otool', '-l', str(path)).splitlines()
    return [out[i + 2].split()[1] for i, line in enumerate(out) if line.strip() == 'cmd LC_RPATH']


def resolve_dep(dep, loader, executable):
    """Fichier source d'une dépendance, None si elle vient du système."""
    if dep.startswith(SYSTEM_PREFIXES): return None
    expand = lambda p: p.replace('@loader_path', str(loader.parent)).replace('@executable_path', str(executable.parent))
    if dep.startswith('@rpath/'):
        candidates = [Path(expand(r)) / dep[len('@rpath/'):] for r in macho_rpaths(loader)]
    else:
        candidates = [Path(expand(dep))]
    for c in candidates:
        if c.exists(): return c.resolve()
    sys.exit(f'make_app: {dep} (dépendance de {loader}) introuvable')


def bundle_libraries(app, binary):
    """Copie l'exécutable dans l'app avec ses dylibs non système et les réécrit en @rpath."""
    frameworks = app / 'Contents/Frameworks'
    frameworks.mkdir(parents=True, exist_ok=True)
    target = app / 'Contents/MacOS/portal2-native'
    shutil.copy2(binary, target)

    # Parcours en largeur : fichier source -> nom dans Frameworks.
    bundled, queue = {}, [(binary, target)]
    while queue:
        source, dest = queue.pop(0)
        changes = []
        for dep in macho_deps(source):
            real = resolve_dep(dep, source, binary)
            if real is None: continue
            name = Path(dep).name
            if real not in bundled:
                bundled[real] = name
                queue.append((real, frameworks / name))
            changes += ['-change', dep, f'@rpath/{bundled[real]}']
        if source != binary:
            shutil.copy2(source, dest)
            dest.chmod(0o755)  # Homebrew les installe en lecture seule
            changes = ['-id', f'@rpath/{dest.name}'] + changes
            if b'SDL2COMPAT' in source.read_bytes() and SDL3.resolve() not in bundled:
                if not SDL3.exists(): sys.exit(f'make_app: {SDL3} introuvable (brew install sdl3)')
                bundled[SDL3.resolve()] = SDL3_NAME
                queue.append((SDL3.resolve(), frameworks / SDL3_NAME))
            # Les rpath Homebrew (@loader_path/../lib...) ne veulent plus rien dire ici.
            rpaths = macho_rpaths(source)
            changes += [arg for r in rpaths if r != '@loader_path' for arg in ('-delete_rpath', r)]
            if '@loader_path' not in rpaths: changes += ['-add_rpath', '@loader_path']
        elif FRAMEWORKS_RPATH not in macho_rpaths(source):
            changes += ['-add_rpath', FRAMEWORKS_RPATH]
        if changes: run('install_name_tool', *changes, str(dest))

    leftovers = [f'{f.name}: {d}' for f in [target, *frameworks.iterdir()] for d in macho_deps(f)
                 if not d.startswith(SYSTEM_PREFIXES + ('@rpath/',))]
    if leftovers: sys.exit('make_app: dépendances hors de l\'app :\n  ' + '\n  '.join(leftovers))

    # Fontconfig lit sa configuration dans /opt/homebrew/etc/fonts ; le lanceur
    # pointe FONTCONFIG_PATH vers cette copie (liens de conf.d résolus).
    fonts = HOMEBREW / 'etc/fonts'
    if fonts.is_dir():
        shutil.copytree(fonts, app / 'Contents/Resources/fonts', symlinks=False, dirs_exist_ok=True,
                        ignore=shutil.ignore_patterns('*.default'))


def bundle_steam_api(app, config):
    """Copie la libsteam_api.dylib configurée (chargée par dlopen, donc absente
    d'otool -L) dans Frameworks, où le lanceur la désigne."""
    source = Path(config.get('steam_api_dylib') or '').expanduser()
    if not source.is_file():
        sys.exit(f'libsteam_api.dylib not found ({source}): re-run ./setup.sh')
    frameworks = app / 'Contents/Frameworks'
    frameworks.mkdir(parents=True, exist_ok=True)
    shutil.copy(source, frameworks / 'libsteam_api.dylib')


def bundle_d3d9metal(app, config):
    """Copie la libd3d9metal.dylib configurée (rendu Direct3D 9 natif, chargée
    par dlopen depuis Frameworks/ avec P2_RENDERER=d3d9) ; rien sans clé."""
    if not config.get('d3d9metal_dylib'): return
    source = Path(config['d3d9metal_dylib']).expanduser()
    if not source.is_file(): sys.exit(f'make_app: d3d9metal_dylib {source} not found')
    frameworks = app / 'Contents/Frameworks'
    frameworks.mkdir(parents=True, exist_ok=True)
    shutil.copy(source, frameworks / 'libd3d9metal.dylib')


def sign_app(app):
    """Signature ad hoc : install_name_tool invalide celle de l'éditeur de liens."""
    for lib in sorted((app / 'Contents/Frameworks').glob('*.dylib')):
        run('codesign', '--force', '-s', '-', str(lib))
    run('codesign', '--force', '-s', '-', str(app / 'Contents/MacOS/portal2-native'))
    run('codesign', '--force', '-s', '-', str(app))


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


def build_app(app, binary, prep, config, paths, game_args):
    macos = app / 'Contents/MacOS'
    resources = app / 'Contents/Resources'
    macos.mkdir(parents=True)
    resources.mkdir()
    # Dans l'app plutôt que dans Application Support : 75 Mo liés à cette
    # compilation précise, retirés avec l'app (corbeille comprise).
    copy_tree(prep, resources / 'prep')
    # Le lanceur ne lit plus rien dans le dépôt : script de langue (bibliothèque
    # standard seule) et instantané de la config ; relancer make_app.py après
    # une modification de config/local.json.
    shutil.copy(ROOT / 'tools/language.py', resources / 'language.py')
    shutil.copy(ROOT / 'config/local.json', resources / 'local.json')

    launcher = macos / 'launcher'
    launcher.write_text(LAUNCHER.format(
        name=NAME, game_args=shlex.join(game_args or DEFAULT_ARGS),
        binary_text=str(binary), game_dir_text=str(paths['game_dir']).replace('"', ''),
        **{key: shlex.quote(str(value)) for key, value in paths.items()}))
    launcher.chmod(launcher.stat().st_mode | stat.S_IXUSR | stat.S_IXGRP | stat.S_IXOTH)
    bundle_libraries(app, binary)
    bundle_steam_api(app, config)
    bundle_d3d9metal(app, config)

    icon = paths['game_dir'] / 'portal2/resource/game.icns'
    if icon.exists(): shutil.copy(icon, resources / 'portal2.icns')
    plist = {
        'CFBundleName': NAME, 'CFBundleDisplayName': NAME, 'CFBundleIdentifier': 'fr.kanapitsas.portal2-native',
        'CFBundleExecutable': 'launcher', 'CFBundlePackageType': 'APPL', 'CFBundleVersion': '1',
        'CFBundleShortVersionString': '1.0', 'CFBundleIconFile': 'portal2', 'LSMinimumSystemVersion': '13.0',
        'NSHighResolutionCapable': True, 'LSApplicationCategoryType': 'public.app-category.games',
    }
    with (app / 'Contents/Info.plist').open('wb') as f: plistlib.dump(plist, f)
    sign_app(app)


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

    # Construite à côté puis échangée : un échec en cours de route laisse
    # l'app existante intacte.
    final = args.dest / f'{NAME}.app'
    app = args.dest / f'.{NAME}.app.partial'
    if app.exists(): shutil.rmtree(app)
    try:
        build_app(app, binary, prep, config, paths, game_args)
    except BaseException:
        shutil.rmtree(app, ignore_errors=True)
        raise
    old = args.dest / f'.{NAME}.app.old'
    if old.exists(): shutil.rmtree(old)
    if final.exists(): final.rename(old)
    app.rename(final)
    shutil.rmtree(old, ignore_errors=True)
    print(final)


if __name__ == '__main__':
    main()
