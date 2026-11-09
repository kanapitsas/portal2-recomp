#!/usr/bin/env python3
"""Create "Portal 2 Natif.app" (or, with --game hl2, "Half-Life 2 Natif.app";
tools/games.py), launchable from the Finder or the Dock.

The app does not copy the game data; it embeds a copy of the game's binary
(build/portal2-native, build/hl2/hl2-native) and the Homebrew dylibs it
depends on (Contents/Frameworks), so it runs without Homebrew. Re-run this script after a rebuild to update the app. The launcher
takes its paths from config/local.json, starts Steam if it is not running and
logs to ~/Library/Logs/Portal2Natif.log (HalfLife2Natif.log for HL2).

The prepared modules (build/game/prep or build/hl2/prep, ~75 MB) are copied into
Contents/Resources/prep: rebuild the app after tools/gameprep.py. Without
user_dir in config/local.json, saves go to
~/Library/Application Support/<app name>/userdata (Portal 2's old default
../work/portal2-userdata is copied there once). ./uninstall.sh removes it all.

    python3 tools/make_app.py                      # ~/Applications
    python3 tools/make_app.py --game hl2
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

sys.path.insert(0, str(Path(__file__).resolve().parent))
import games  # noqa: E402
import localconfig  # noqa: E402

ROOT = Path(__file__).resolve().parent.parent
# Données propres à l'app, hors du dépôt : ./uninstall.sh supprime ce dossier.
SUPPORT = localconfig.SUPPORT
OLD_USER_DIR = '../work/portal2-userdata'  # ancien défaut, relatif au dépôt

LAUNCHER = '''#!/bin/bash
# Lanceur généré par tools/make_app.py : le modifier là-bas.
LOG="$HOME/Library/Logs/{log_name}"
HERE="$(cd "$(dirname "$0")" && pwd)"
alert() {{
    /usr/bin/osascript - "$1" >/dev/null 2>&1 <<'APPLESCRIPT'
on run argv
    display alert "{name}" message (item 1 of argv) as critical
end run
APPLESCRIPT
}}
GAME_DIR={game_dir}   # quoted by shlex: never re-parsed by the shell
if [ ! -x "$HERE/{executable}" ]; then alert "Exécutable introuvable dans l'app. Relancer tools/make_app.py."; exit 1; fi
if [ ! -d "$GAME_DIR" ]; then alert "Données du jeu introuvables (disque externe débranché ?) : $GAME_DIR"; exit 1; fi
mkdir -p "$(dirname "$LOG")" {user_dir}
# Check the effective game data (including user overrides) before starting Steam.
if ! "$HERE/{executable}" --check-game-data --game-dir {game_dir} --linux-root {linux_root} --user-dir {user_dir} >"$LOG" 2>&1; then
    alert "$(cat "$LOG")"
    exit 1
fi
# L'API Steam a besoin du client Steam ouvert et connecté.
if ! /usr/bin/pgrep -qx steam_osx; then
    /usr/bin/open -ga Steam
    for _ in $(seq 60); do /usr/bin/pgrep -qx steam_osx && break; sleep 0.5; done
    sleep 5
fi
export SteamAppId={appid} SteamGameId={appid}
# libsteam_api.dylib de Valve, copiée dans l'app : le runtime la cherche sinon
# à côté du dépôt, absent ici.
export P2_STEAM_API_DYLIB="$HERE/../Frameworks/libsteam_api.dylib"
# Configuration Fontconfig embarquée (sinon cherchée dans le préfixe de compilation).
[ -d "$HERE/../Resources/fonts" ] && export FONTCONFIG_PATH="$HERE/../Resources/fonts"
# Langue : P2_LANGUAGE, "language" de config/local.json, Steam, système (tools/language.py).
# Un -language passé au lanceur l'emporte ; en cas d'échec, le jeu reste en anglais.
LANGUAGE_ARGS=$(/usr/bin/python3 "$HERE/../Resources/language.py" --config "$HERE/../Resources/local.json" -- {game_args} "$@" 2>/dev/null)
exec "$HERE/{executable}" --images "$(dirname "$HERE")/Resources/prep" --game-dir {game_dir} --linux-root {linux_root} \\
    --user-dir {user_dir} -- {game_args} $LANGUAGE_ARGS "$@" >"$LOG" 2>&1
'''


# --- Bibliothèques embarquées ----------------------------------------------
# On compile contre Homebrew ou le bundle précompilé de setup.sh (SDL2 =
# sdl2-compat, FreeType, Fontconfig ; localconfig.host_prefix), mais
# l'app ne doit pas en dépendre à l'exécution : ces dylibs et leurs dépendances
# non système sont copiées dans Contents/Frameworks, référencées par @rpath.
# @executable_path suit les liens symboliques : l'exécutable est donc copié
# (~300 Mo ; install_name_tool et codesign réécrivent tout le fichier, un clone
# APFS n'économiserait rien).
SYSTEM_PREFIXES = ('/usr/lib/', '/System/')
FRAMEWORKS_RPATH = '@executable_path/../Frameworks'
# sdl2-compat ne lie pas SDL3 : il le charge par dlopen, en cherchant d'abord
# @loader_path/libSDL3.dylib, donc à côté de lui dans Frameworks.
SDL3_NAME = 'libSDL3.dylib'


def run(*cmd):
    return subprocess.run(cmd, check=True, capture_output=True, text=True).stdout


def macho_deps(path):
    """Dependencies (LC_LOAD_*) of a Mach-O, without its own install name."""
    lines = run('otool', '-L', str(path)).splitlines()[1:]
    deps = [line.strip().rsplit(' (compatibility', 1)[0] for line in lines if line.strip()]
    ident = run('otool', '-D', str(path)).splitlines()[1:]
    return [d for d in deps if d not in ident]


def parse_rpaths(otool_l):
    """LC_RPATH paths of `otool -l` output; a path may contain spaces."""
    out = otool_l.splitlines()
    return [out[i + 2].strip()[len('path '):].rsplit(' (offset', 1)[0]
            for i, line in enumerate(out) if line.strip() == 'cmd LC_RPATH']


def macho_rpaths(path):
    return parse_rpaths(run('otool', '-l', str(path)))


def resolve_dep(dep, loader, executable):
    """Source file of a dependency, None when it comes from the system."""
    if dep.startswith(SYSTEM_PREFIXES): return None
    expand = lambda p: p.replace('@loader_path', str(loader.parent)).replace('@executable_path', str(executable.parent))
    if dep.startswith('@rpath/'):
        candidates = [Path(expand(r)) / dep[len('@rpath/'):] for r in macho_rpaths(loader)]
    else:
        candidates = [Path(expand(dep))]
    for c in candidates:
        if c.exists(): return c.resolve()
    sys.exit(f'make_app: {dep} (dépendance de {loader}) introuvable')


def sdl3_library(prefix):
    """SDL3 under `prefix`: Homebrew keeps it in its own keg, the bundle in lib/."""
    for candidate in (prefix / 'opt/sdl3/lib/libSDL3.0.dylib', prefix / 'lib/libSDL3.0.dylib'):
        if candidate.exists(): return candidate.resolve()
    sys.exit(f'make_app: libSDL3.0.dylib introuvable sous {prefix} (brew install sdl3, ou relancer ./setup.sh)')


def bundle_libraries(app, binary, prefix, executable='portal2-native'):
    """Copy the executable into the app (Contents/MacOS/<executable>) with its
    non-system dylibs, rewritten to @rpath."""
    frameworks = app / 'Contents/Frameworks'
    frameworks.mkdir(parents=True, exist_ok=True)
    target = app / 'Contents/MacOS' / executable
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
            if b'SDL2COMPAT' in source.read_bytes():
                sdl3 = sdl3_library(prefix)
                if sdl3 not in bundled:
                    bundled[sdl3] = SDL3_NAME
                    queue.append((sdl3, frameworks / SDL3_NAME))
            # Les rpath Homebrew (@loader_path/../lib...) ne veulent plus rien dire ici.
            rpaths = macho_rpaths(source)
            changes += [arg for r in rpaths if r != '@loader_path' for arg in ('-delete_rpath', r)]
            if '@loader_path' not in rpaths: changes += ['-add_rpath', '@loader_path']
        else:
            # Seul rpath de l'exécutable : celui du préfixe de compilation ferait
            # charger les bibliothèques hors de l'app quand elles y sont encore.
            rpaths = macho_rpaths(source)
            changes += [arg for r in rpaths if r != FRAMEWORKS_RPATH for arg in ('-delete_rpath', r)]
            if FRAMEWORKS_RPATH not in rpaths: changes += ['-add_rpath', FRAMEWORKS_RPATH]
        if changes: run('install_name_tool', *changes, str(dest))

    leftovers = [f'{f.name}: {d}' for f in [target, *frameworks.iterdir()] for d in macho_deps(f)
                 if not d.startswith(SYSTEM_PREFIXES + ('@rpath/',))]
    if leftovers: sys.exit('make_app: dépendances hors de l\'app :\n  ' + '\n  '.join(leftovers))

    # Fontconfig lit sa configuration dans <préfixe>/etc/fonts ; le lanceur
    # pointe FONTCONFIG_PATH vers cette copie (liens de conf.d résolus).
    fonts = prefix / 'etc/fonts'
    if fonts.is_dir():
        shutil.copytree(fonts, app / 'Contents/Resources/fonts', symlinks=False, dirs_exist_ok=True,
                        ignore=shutil.ignore_patterns('*.default'))


def bundle_steam_api(app, config):
    """Copy the configured libsteam_api.dylib (dlopen'ed, so absent from
    otool -L) into Frameworks, where the launcher points to it."""
    source = Path(config.get('steam_api_dylib') or '').expanduser()
    if not source.is_file():
        sys.exit(f'libsteam_api.dylib not found ({source}): re-run ./setup.sh')
    frameworks = app / 'Contents/Frameworks'
    frameworks.mkdir(parents=True, exist_ok=True)
    shutil.copy(source, frameworks / 'libsteam_api.dylib')


def bundle_d3d9metal(app, config):
    """Copy the configured libd3d9metal.dylib (native Direct3D 9 renderer,
    dlopen'ed from Frameworks/, the default renderer); nothing without the key."""
    if not config.get('d3d9metal_dylib'): return
    source = Path(config['d3d9metal_dylib']).expanduser()
    if not source.is_file(): sys.exit(f'make_app: d3d9metal_dylib {source} not found')
    frameworks = app / 'Contents/Frameworks'
    frameworks.mkdir(parents=True, exist_ok=True)
    shutil.copy(source, frameworks / 'libd3d9metal.dylib')


def sign_app(app):
    """Ad hoc signature: install_name_tool invalidates the linker's."""
    for lib in sorted((app / 'Contents/Frameworks').glob('*.dylib')):
        run('codesign', '--force', '-s', '-', str(lib))
    for executable in sorted((app / 'Contents/MacOS').iterdir()):   # the game's (portal2-native...)
        if executable.name != 'launcher':
            run('codesign', '--force', '-s', '-', str(executable))
    run('codesign', '--force', '-s', '-', str(app))


def user_dir(config):
    """Saves folder: user_dir of the config, else the game's support folder
    (SUPPORT for Portal 2), whose userdata Portal 2 initialises once by
    copying (never moving) the old default."""
    if config.get('user_dir'): return localconfig.user_dir(config)
    if games.profile(config)['name'] != 'portal2': return localconfig.user_dir(config)
    new, old = localconfig.user_dir(config, support=SUPPORT), ROOT / OLD_USER_DIR
    if not new.exists() and old.is_dir():
        partial = new.with_name(new.name + '.partial')
        if partial.exists(): shutil.rmtree(partial)
        shutil.copytree(old, partial, symlinks=True)
        partial.rename(new)
        print(f'saves copied: {old.resolve()} -> {new} (original kept)')
    return new


def copy_tree(source, dest):
    """Copy source to dest; APFS clones (cp -c, no extra disk space) when possible."""
    if dest.exists(): shutil.rmtree(dest)
    if subprocess.run(['/bin/cp', '-cR', str(source), str(dest)], capture_output=True).returncode:
        if dest.exists(): shutil.rmtree(dest)
        shutil.copytree(source, dest)


def default_args(game):
    """Game arguments of the app when make_app.py is given none."""
    # Sans -w/-h/-windowed : résolution et mode choisis dans le menu Vidéo du jeu
    # (enregistrés dans update/cfg/video.txt) ; Retina selon la résolution.
    return ['-game', game['mod'], '-novid']


def build_app(app, binary, prep, config, paths, game_args):
    macos = app / 'Contents/MacOS'
    resources = app / 'Contents/Resources'
    macos.mkdir(parents=True)
    resources.mkdir()
    # Dans l'app plutôt que dans Application Support : 75 Mo liés à cette
    # compilation précise, retirés avec l'app (corbeille comprise).
    copy_tree(prep, resources / 'prep')
    # Le lanceur ne lit plus rien dans le dépôt : script de langue (bibliothèque
    # standard seule) et instantané de la config de ce jeu (vue fusionnée,
    # localconfig.game_config) ; relancer make_app.py après une modification
    # de config/local.json.
    shutil.copy(ROOT / 'tools/language.py', resources / 'language.py')
    (resources / 'local.json').write_text(json.dumps(config, indent=2) + '\n')

    game = games.profile(config)
    launcher = macos / 'launcher'
    launcher.write_text(LAUNCHER.format(
        name=game['app_name'], log_name=game['log_name'], appid=int(game['steam_appid']), executable=game['executable'],
        game_args=shlex.join(game_args or default_args(game)),
        **{key: shlex.quote(str(value)) for key, value in paths.items()}))
    launcher.chmod(launcher.stat().st_mode | stat.S_IXUSR | stat.S_IXGRP | stat.S_IXOTH)
    bundle_libraries(app, binary, localconfig.host_prefix(config), executable=game['executable'])
    bundle_steam_api(app, config)
    bundle_d3d9metal(app, config)

    icon = paths['game_dir'] / game['icon']
    if icon.exists(): shutil.copy(icon, resources / 'portal2.icns')
    plist = {
        'CFBundleName': game['app_name'], 'CFBundleDisplayName': game['app_name'],
        'CFBundleIdentifier': game['bundle_id'],
        'CFBundleExecutable': 'launcher', 'CFBundlePackageType': 'APPL', 'CFBundleVersion': '1',
        'CFBundleShortVersionString': '1.0', 'CFBundleIconFile': 'portal2', 'LSMinimumSystemVersion': '13.0',
        'NSHighResolutionCapable': True, 'LSApplicationCategoryType': 'public.app-category.games',
    }
    with (app / 'Contents/Info.plist').open('wb') as f: plistlib.dump(plist, f)
    sign_app(app)


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    games.add_argument(parser)
    parser.add_argument('--dest', type=Path, default=Path.home() / 'Applications')
    parser.add_argument('game_args', nargs=argparse.REMAINDER)
    args = parser.parse_args()
    game_args = args.game_args[1:] if args.game_args[:1] == ['--'] else args.game_args
    config = localconfig.game_config(localconfig.load(ROOT / 'config/local.json'), args.game)
    game = games.profile(config)
    if not config.get('game_root') or not config.get('linux_root'):
        raise SystemExit(f'{game["name"]} is not set up in config/local.json: run ./setup.sh --game {game["name"]}')
    binary = ROOT / game['binary']
    prep = ROOT / game['build_dir'] / 'prep'
    if not (prep / 'layout.json').exists():
        raise SystemExit(f'{prep} absent : lancer tools/gameprep.py --game {game["name"]}')
    if not binary.exists():
        raise SystemExit(f'{binary} absent : lancer tools/build_game.py --game {game["name"]} --release')
    paths = {
        'binary': binary, 'game_dir': Path(config['game_root']),
        'linux_root': Path(config['linux_root']), 'user_dir': user_dir(config),
    }

    # Construite à côté puis échangée : un échec en cours de route laisse
    # l'app existante intacte.
    name = game['app_name']
    final = args.dest / f'{name}.app'
    app = args.dest / f'.{name}.app.partial'
    if app.exists(): shutil.rmtree(app)
    try:
        build_app(app, binary, prep, config, paths, game_args)
    except BaseException:
        shutil.rmtree(app, ignore_errors=True)
        raise
    old = args.dest / f'.{name}.app.old'
    if old.exists(): shutil.rmtree(old)
    if final.exists(): final.rename(old)
    app.rename(final)
    shutil.rmtree(old, ignore_errors=True)
    print(final)


if __name__ == '__main__':
    main()
