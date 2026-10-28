#!/usr/bin/env python3
"""Construit Portal 2 recompilé en application iOS (simulateur arm64 par défaut).

Même C généré que le build macOS (tools/build_game.py, génération et cache
d'objets réutilisés sans modification), compilé pour iOS et lié avec les
dépendances de work/ios-deps (SDL2, FreeType, gl4es ; voir son build.sh), le
stub Steam et fontconfig minimal du build iOS (runtime/ios). Produit
build/ios/<cible>/Portal2.app, signée ad hoc pour le simulateur, avec le profil
obtenu par Xcode pour un appareil (ios/signing).

    python3 tools/build_ios.py                       # simulateur
    xcrun simctl install booted build/ios/sim/Portal2.app
"""
import argparse
import json
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

ROOT = Path(__file__).resolve().parent.parent
FRAMEWORKS = ['UIKit', 'Foundation', 'CoreGraphics', 'QuartzCore', 'OpenGLES', 'Metal', 'AVFoundation',
              'AudioToolbox', 'CoreAudio', 'CoreMotion', 'GameController', 'CoreHaptics', 'CoreBluetooth', 'OpenAL']
TARGETS = {
    'sim': dict(sdk='iphonesimulator', triple='arm64-apple-ios17.0-simulator', platform='ios-simulator',
                plist='iPhoneSimulator'),
    'device': dict(sdk='iphoneos', triple='arm64-apple-ios17.0', platform='ios', plist='iPhoneOS'),
}


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--target', choices=sorted(TARGETS), default='sim')
    ap.add_argument('--opt', default='1', choices=['0', '1', '2', '3'])
    ap.add_argument('--fast-memory', action='store_true', help='-DRT_FAST_MEMORY (voir docs/fast-path.md)')
    ap.add_argument('--release', action='store_true',
                    help='version pour jouer : --opt 2 --fast-memory, comme build_game.py --release')
    ap.add_argument('--deps', type=Path, help='préfixe des dépendances (défaut ../work/ios-deps/<cible>)')
    ap.add_argument('--layout', type=Path, default=Path('build/game/prep/layout.json'))
    ap.add_argument('--jobs', type=int, default=os.cpu_count() or 1)
    args = ap.parse_args()
    if args.release:
        args.opt, args.fast_memory = '2', True
    os.chdir(ROOT)
    target = TARGETS[args.target]
    deps = (args.deps or ROOT.parent / 'work/ios-deps' / args.target).resolve()
    sdk = subprocess.run(['xcrun', '--sdk', target['sdk'], '--show-sdk-path'], check=True,
                         capture_output=True, text=True).stdout.strip()
    started = time.time()
    layout = json.loads(args.layout.read_text())
    manifest, _ = build_game.generate(SimpleNamespace(layout=args.layout, all=True, jobs=args.jobs),
                                      layout, build_game.roots(layout))
    base = ['-target', target['triple'], '-isysroot', sdk]
    memory = ['-DRT_FAST_MEMORY'] if args.fast_memory else []
    runtime_flags = [*base, f'-O{args.opt}', '-g', *build_game.WARNINGS, *memory, '-Iruntime', '-Iruntime/ios',
                     f'-I{deps}/include', f'-I{deps}/include/SDL2', f'-I{deps}/include/gl4es',
                     f'-I{deps}/include/freetype2', '-DGL_SILENCE_DEPRECATION', '-DGLES_SILENCE_DEPRECATION',
                     '-DOPENAL_DEPRECATED=', '-DAL_SILENCE_DEPRECATION']
    generated_flags = [*base, f'-O{args.opt}', '-gline-tables-only', *build_game.WARNINGS, *memory, '-Iruntime']
    sources = Path('build/game/gen/src')
    units = [(str(p), runtime_flags) for p in sorted(Path('runtime').glob('*.c'))]
    units += [(str(p), runtime_flags) for p in sorted(Path('runtime/ios').glob('*.c'))]
    units += [(str(p), runtime_flags + ['-fobjc-arc']) for p in sorted(Path('runtime/ios').glob('*.m'))]
    units.append(('src/main.c', runtime_flags + ['-Dmain=p2_main']))
    units.append(('ios/main_ios.c', runtime_flags))
    units += [(str(sources / name), generated_flags) for name in sorted(manifest['shards'])]
    units.append((str(sources / 'dispatch.c'), generated_flags))
    # Greffons PE de Miles traduits (voix MP3...), comme le build macOS.
    plugin_sources, _ = build_game.generate_plugins(Path('config/local.json'))
    units += [(str(p), generated_flags) for p in plugin_sources]
    identity = subprocess.run(['clang', '--version'], capture_output=True, text=True, check=True).stdout
    cache = Path(f'build/ios/{args.target}/obj')
    objects, compiled, cpu = build_game.compile_units(units, cache, args.jobs, identity)
    print(f'{compiled}/{len(units)} fichiers compilés ({cpu:.0f} s CPU)')

    app = Path(f'build/ios/{args.target}/Portal2.app')
    app.mkdir(parents=True, exist_ok=True)
    response = Path(f'build/ios/{args.target}/link.rsp')
    response.write_text('\n'.join(str(o) for o in objects) + '\n')
    libs = [str(deps / 'lib' / n) for n in ('libSDL2main.a', 'libSDL2.a', 'libgl4es.a', 'libfreetype.a')]
    link = ['clang', *base, f'@{response}', *libs, '-liconv',
            *[x for f in FRAMEWORKS for x in ('-framework', f)],
            # Le SDK 27 impose UIScene, que SDL2 ignore : déclarer un SDK 18
            # garde le cycle de vie classique d'UIKit (voir ios/probe).
            f'-Wl,-platform_version,{target["platform"]},17.0,18.0',
            '-o', str(app / 'portal2')]
    subprocess.run(link, check=True)
    plist = (ROOT / 'ios/Info.plist.in').read_text()
    for key, value in dict(EXECUTABLE='portal2', IDENTIFIER=bundle_id(), NAME='Portal 2',
                           PLATFORM=target['plist']).items():
        plist = plist.replace(f'@{key}@', value)
    (app / 'Info.plist').write_text(plist)
    # Page du jeu dans l'app Réglages d'iOS (lue par runtime/ios/settings.c).
    shutil.copytree(ROOT / 'ios/Settings.bundle', app / 'Settings.bundle', dirs_exist_ok=True)
    app_icon(app)
    # Lancement depuis l'écran d'accueil : arguments du jeu par défaut (les
    # dossiers de données sont ajoutés par ios/main_ios.c).
    # La langue vient de tools/language.py au moment du build : pour la changer,
    # régler "language" dans config/local.json et refaire le build.
    language = subprocess.run([sys.executable, str(ROOT / 'tools/language.py')], capture_output=True, text=True).stdout.split()
    (app / 'launch.args').write_text('--\n-game\nportal2\n-novid\n-fullscreen\n' + ''.join(f'{word}\n' for word in language))
    if args.target == 'sim':
        subprocess.run(['codesign', '-s', '-', '-f', str(app)], check=True, capture_output=True)
    else:
        subprocess.run([sys.executable, str(ROOT / 'ios/signing/sign.py'), str(app)], check=True)
    size = (app / 'portal2').stat().st_size
    print(f'{app} : exécutable {size / 1e6:.0f} Mo, {time.time() - started:.0f} s au total')


def app_icon(app):
    """Icône de l'app tirée de l'icône macOS du jeu (portal2/resource/game.icns) :
    une image de Valve, prise dans la copie du joueur au lieu d'être versionnée.
    Fichiers à l'ancienne (CFBundleIconFiles d'ios/Info.plist.in), sans
    catalogue d'images. Sans le fichier : icône par défaut d'iOS."""
    config = json.loads(Path('config/local.json').read_text()) if Path('config/local.json').exists() else {}
    icns = Path(config.get('game_root', '')) / 'portal2/resource/game.icns'
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
