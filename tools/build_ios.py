#!/usr/bin/env python3
"""Build a recompiled game as an iOS app (arm64 simulator by default).

Same generated C as the macOS build (tools/build_game.py; generation and object
cache reused unchanged), compiled for iOS and linked with the work/ios-deps
dependencies (SDL2, FreeType, gl4es; see its build.sh), the Steam stub and the
minimal fontconfig of the iOS build (runtime/ios). Produces
<ios_dir>/<target>/<ios_app> of the game's profile (tools/games.py):
build/ios/<target>/Portal2.app, build/hl2/ios/<target>/HalfLife2.app; ad hoc
signed for the simulator, or signed with the profile Xcode obtained for a
device (ios/signing). The game (--game, else P2_GAME, else config/local.json)
gives the roots, program, name, icon and Info.plist key P2Game
(runtime/ios/game_ios.c).

    python3 tools/build_ios.py [--game hl2]          # simulator
    tools/run_ios_sim.sh [--game hl2]                # headless run

Metal renderer (native Direct3D 9, docs/d3d9-bridge.md): --d3d9metal links
libd3d9metal.a (mtld3d, make native-ios) into the executable, no dlopen; it is
the default renderer, the player can go back to OpenGL in Settings > <game> > Renderer.
"""
import argparse
import json
import plistlib
import os
import shutil
import subprocess
import sys
import time
from pathlib import Path
from types import SimpleNamespace

from ios_bundle_id import bundle_id

sys.path.insert(0, str(Path(__file__).resolve().parent))
import build_game
import games
import language
import localconfig

ROOT = Path(__file__).resolve().parent.parent
FRAMEWORKS = ['UIKit', 'Foundation', 'CoreGraphics', 'QuartzCore', 'OpenGLES', 'Metal', 'AVFoundation',
              'AudioToolbox', 'CoreAudio', 'CoreMotion', 'GameController', 'CoreHaptics', 'CoreBluetooth', 'OpenAL']
# Frameworks de libd3d9metal en plus de ceux de l'app (otool -L de sa dylib iOS).
D3D9METAL_FRAMEWORKS = ['MetalFX', 'CoreVideo', 'CoreFoundation']
TARGETS = {
    'sim': dict(sdk='iphonesimulator', triple='arm64-apple-ios17.0-simulator', platform='ios-simulator',
                plist='iPhoneSimulator'),
    'device': dict(sdk='iphoneos', triple='arm64-apple-ios17.0', platform='ios', plist='iPhoneOS'),
}


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--target', choices=sorted(TARGETS), default='sim')
    ap.add_argument('--opt', default='1', choices=['0', '1', '2', '3'])
    ap.add_argument('--fast-memory', action='store_true', help='-DRT_FAST_MEMORY (see docs/fast-path.md)')
    ap.add_argument('--release', action='store_true',
                    help='build to play: --opt 2 --fast-memory, like build_game.py --release')
    ap.add_argument('--deps', type=Path, help='dependency prefix (default ../work/ios-deps/<target>)')
    ap.add_argument('--d3d9metal', type=Path,
                    help='libd3d9metal.a for this target (default: "d3d9metal_ios" of config/local.json, '
                         'one path per target: {"device": ..., "sim": ...}); without it, OpenGL only')
    games.add_argument(ap)
    ap.add_argument('--layout', type=Path, help='default: <build_dir>/prep/layout.json of the game (build/game, build/hl2)')
    ap.add_argument('--jobs', type=int, default=os.cpu_count() or 1)
    args = ap.parse_args()
    if args.release:
        args.opt, args.fast_memory = '2', True
    os.chdir(ROOT)
    config = localconfig.for_game(args.game)
    game = games.profile(config)
    if not game['ios']:
        sys.exit(f'build_ios: {game["app_name"]} has no iPhone build yet')
    build_dir, ios_dir = Path(game['build_dir']), Path(game['ios_dir']) / args.target
    args.layout = args.layout or build_dir / 'prep/layout.json'
    target = TARGETS[args.target]
    deps = (args.deps or ROOT.parent / 'work/ios-deps' / args.target).resolve()
    d3d9metal = d3d9metal_library(args.d3d9metal, args.target, config)
    sdk = subprocess.run(['xcrun', '--sdk', target['sdk'], '--show-sdk-path'], check=True,
                         capture_output=True, text=True).stdout.strip()
    started = time.time()
    layout = json.loads(args.layout.read_text())
    manifest, _ = build_game.generate(SimpleNamespace(layout=args.layout, all=True, jobs=args.jobs),
                                      layout, build_game.roots(layout, game['roots']), build_dir / 'gen')
    base = ['-target', target['triple'], '-isysroot', sdk]
    memory = ['-DRT_FAST_MEMORY'] if args.fast_memory else []
    runtime_flags = [*base, f'-O{args.opt}', '-g', *build_game.WARNINGS, *memory, '-Iruntime', '-Iruntime/ios',
                     f'-I{deps}/include', f'-I{deps}/include/SDL2', f'-I{deps}/include/gl4es',
                     f'-I{deps}/include/freetype2', '-DGL_SILENCE_DEPRECATION', '-DGLES_SILENCE_DEPRECATION',
                     '-DOPENAL_DEPRECATED=', '-DAL_SILENCE_DEPRECATION']
    generated_flags = [*base, f'-O{args.opt}', '-gline-tables-only', *build_game.GENERATED_TUNING, *build_game.WARNINGS, *memory, '-Iruntime']
    sources = build_dir / 'gen/src'
    # runtime/d3d9.c : symboles de libd3d9metal.a liés, pas de dlopen ;
    # runtime/ios/metal_ios.m s'en sert pour savoir si Metal est disponible.
    static = ['-DRT_D3D9METAL_STATIC'] if d3d9metal else []
    units = [(str(p), runtime_flags + (static if p.name == 'd3d9.c' else [])) for p in sorted(Path('runtime').glob('*.c'))]
    units += [(str(p), runtime_flags) for p in sorted(Path('runtime/ios').glob('*.c'))]
    units += [(str(p), runtime_flags + static + ['-fobjc-arc']) for p in sorted(Path('runtime/ios').glob('*.m'))]
    units.append(('src/main.c', runtime_flags + ['-Dmain=p2_main', f'-DRT_GAME_PROGRAM="{game["program"]}"']))
    units.append(('ios/main_ios.c', runtime_flags))
    units += [(str(sources / name), generated_flags) for name in sorted(manifest['shards'])]
    units.append((str(sources / 'dispatch.c'), generated_flags))
    # Greffons PE de Miles traduits (voix MP3...), comme le build macOS.
    plugin_sources, _ = build_game.generate_plugins(config, build_dir / 'gen/pe')
    units += [(str(p), generated_flags) for p in plugin_sources]
    identity = subprocess.run(['clang', '--version'], capture_output=True, text=True, check=True).stdout
    cache = ios_dir / 'obj'
    objects, compiled, cpu = build_game.compile_units(units, cache, args.jobs, identity)
    print(f'{compiled}/{len(units)} fichiers compilés ({cpu:.0f} s CPU)')

    app = games.ios_app(game, args.target)
    app.mkdir(parents=True, exist_ok=True)
    response = ios_dir / 'link.rsp'
    response.write_text('\n'.join(str(o) for o in objects) + '\n')
    libs = [str(deps / 'lib' / n) for n in ('libSDL2main.a', 'libSDL2.a', 'libgl4es.a', 'libfreetype.a')]
    frameworks = FRAMEWORKS + (D3D9METAL_FRAMEWORKS if d3d9metal else [])
    link = ['clang', *base, f'@{response}', *libs, *([str(d3d9metal)] if d3d9metal else []), '-liconv',
            *[x for f in frameworks for x in ('-framework', f)],
            # Le SDK 27 impose UIScene, que SDL2 ignore : déclarer un SDK 18
            # garde le cycle de vie classique d'UIKit (voir ios/probe).
            f'-Wl,-platform_version,{target["platform"]},17.0,18.0',
            '-o', str(app / game['ios_executable'])]
    subprocess.run(link, check=True)
    plist = (ROOT / 'ios/Info.plist.in').read_text()
    for key, value in dict(EXECUTABLE=game['ios_executable'], IDENTIFIER=bundle_id(game=game['name']), NAME=game['ios_name'],
                           GAME=game['name'], PLATFORM=target['plist']).items():
        plist = plist.replace(f'@{key}@', value)
    (app / 'Info.plist').write_text(plist)
    # Page du jeu dans l'app Réglages d'iOS (lue par runtime/ios/settings.c).
    shutil.copytree(ROOT / 'ios/Settings.bundle', app / 'Settings.bundle', dirs_exist_ok=True)
    settings_for_game(app / 'Settings.bundle/Root.plist', game['name'])
    app_icon(app, config, game['icon'])
    (app / 'launch.args').write_text(''.join(f'{word}\n' for word in launch_args(game, config)))
    if args.target == 'sim':
        subprocess.run(['codesign', '-s', '-', '-f', str(app)], check=True, capture_output=True)
    else:
        subprocess.run([sys.executable, str(ROOT / 'ios/signing/sign.py'), str(app)], check=True)
    size = (app / game['ios_executable']).stat().st_size
    print(f'{app} : exécutable {size / 1e6:.0f} Mo, {time.time() - started:.0f} s au total')
    print(f'rendu Metal : {d3d9metal}' if d3d9metal else 'rendu Metal : absent (OpenGL seulement, voir --d3d9metal)')


def launch_args(game, config):
    """Arguments of a launch from the home screen (launch.args; the data
    folders are added by ios/main_ios.c). The language comes from
    tools/language.py at build time, for this game's config (its folder and
    appmanifest): to change it, set "language" in config/local.json and
    rebuild."""
    words = ['-game', game['mod'], '-novid', '-fullscreen']
    return ['--', *words, *language.language_args(config, words)]


def d3d9metal_library(given, target, config):
    """libd3d9metal.a to link for this target (argument, else config/local.json), or None."""
    if given is None:
        entry = config.get('d3d9metal_ios')
        given = entry.get(target) if isinstance(entry, dict) else None
        if not given:
            return None
    path = Path(given).expanduser().resolve()
    if path.suffix != '.a' or not path.is_file():
        sys.exit(f'--d3d9metal : {path} n\'est pas une bibliothèque statique (libd3d9metal.a de make native-ios)')
    return path


def settings_for_game(root_plist, game):
    """Half-Life 2 has no portals: the Portals group of the settings page (back
    taps, portal buttons, vibration when firing) is left out, back taps stay
    off (runtime/ios/game_ios.c), and gyro aiming is shown on, its default there."""
    if game == 'portal2':
        return
    with open(root_plist, 'rb') as f:
        root = plistlib.load(f)
    kept, skipping = [], False
    for item in root['PreferenceSpecifiers']:
        if item.get('Type') == 'PSGroupSpecifier':
            skipping = item.get('Title') == 'Portals'
        if item.get('Key') == 'P2Gyro':
            item['DefaultValue'] = True   # gyro aiming on by default (touch_controls.m)
        if not skipping:
            kept.append(item)
    root['PreferenceSpecifiers'] = kept
    with open(root_plist, 'wb') as f:
        plistlib.dump(root, f, fmt=plistlib.FMT_BINARY)


def app_icon(app, config, relative):
    """App icon taken from the game's macOS icon (`icon` of the profile, e.g.
    portal2/resource/game.icns, under the game's game_root in `config`): a
    Valve image, read from the player's copy instead of being versioned.
    Old-style files (CFBundleIconFiles of ios/Info.plist.in), no asset
    catalog. Without the file: iOS default icon."""
    icns = Path(config.get('game_root', '')) / relative
    if not icns.is_file():
        print(f'icône : {icns} absent, icône par défaut')
        return
    work = app.parent / 'icon'
    shutil.rmtree(work, ignore_errors=True)
    subprocess.run(['iconutil', '-c', 'iconset', '-o', str(work / 'game.iconset'), str(icns)], check=True)
    largest = max((work / 'game.iconset').glob('icon_*.png'), key=lambda p: p.stat().st_size)
    for name, size in (('AppIcon60x60@2x.png', 120), ('AppIcon60x60@3x.png', 180), ('AppIcon76x76@2x~ipad.png', 152),
                       ('AppIcon83.5x83.5@2x~ipad.png', 167)):
        subprocess.run(['sips', '-z', str(size), str(size), str(largest), '--out', str(app / name)],
                       check=True, capture_output=True)


if __name__ == '__main__':
    main()
