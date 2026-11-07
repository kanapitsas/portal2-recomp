"""Per-game layout of the Linux depot: which modules to prepare and how the
program is named, plus what setup.sh and tools/make_app.py need to install it
(Steam depot, Steamworks.NET release, app name, data folder), and where its
build lives in this checkout. One checkout builds both games side by side:
Portal 2 keeps its historical paths (build/game, build/portal2-native,
build/ios), Half-Life 2 lives under build/hl2.

Which game a tool works on: its --game option, else the P2_GAME environment
variable, else "game" in config/local.json, else the only game configured
there, else portal2 (selected()).
"""
import os
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent

# Modules never prepared, whatever the game: authoring tools, the optional
# legacy Steam client and DXVK (shaderapivk.so gets its Direct3D 9 from the
# runtime bridge, runtime/d3d9.c, so Direct3DCreate9 becomes a shim).
_COMMON_EXCLUDED = {'bsppack.so', 'crashhandler.so', 'dedicated.so', 'vrad_dll.so', 'vvis_dll.so',
                    'libsteam.so', 'libdxvk_d3d9.so'}

GAMES = {
    'portal2': dict(
        program='portal2_linux',
        lib_dirs=('bin/linux32', 'portal2/bin/linux32'),
        mod='portal2',
        excluded=_COMMON_EXCLUDED | {'puzzlemaker_dll.so'},
        roots='config/game-roots.json',
        steam_appid=620,
        depot_id=661,
        # Installed app and its data: unchanged since the first release, so
        # existing installs, saves and settings stay where they are.
        app_name='Portal 2 Natif',
        log_name='Portal2Natif.log',
        bundle_id='fr.kanapitsas.portal2-native',
        icon='portal2/resource/game.icns',
        renderer=None,   # Metal when libd3d9metal is there
        # Build tree (prep, gen, obj, launch.log), executable and its name in
        # the app, iPhone builds: the paths of the single-game checkouts.
        build_dir='build/game',
        binary='build/portal2-native',
        executable='portal2-native',
        ios_dir='build/ios',
        ios=True,
    ),
    # Half-Life 2 since the 20th anniversary update (depot 224): episodes and
    # Lost Coast live in the hl2_complete mod; hl2/ and hl2mp/ hold other
    # client.so/server.so with the same names, so only hl2_complete is taken.
    'hl2': dict(
        program='hl2_linux',
        lib_dirs=('bin', 'hl2_complete/bin'),
        mod='hl2_complete',
        # Chromium (libcef, chromehtml and its ffmpeg) only serves in-game web
        # pages; vtex is a tool; libTelemetryX64 is a 64-bit profiler and
        # ffmpegsumo.so a leftover Mach-O bundle. Nothing loads libmimalloc.so
        # (Portal 2 preloads it; hl2.sh does not). The bundled FreeType,
        # Fontconfig and HarfBuzz would need the system zlib, bzip2 and expat:
        # the host ones are used through runtime/fonts.c, as for Portal 2.
        # sourcevr.so needs OpenVR and libstdc++, absent here; the engine
        # starts without it.
        excluded=_COMMON_EXCLUDED | {'vtex_dll.so', 'libcef.so', 'chromehtml.so', 'libffmpegsumo.so',
                                     'ffmpegsumo.so', 'libTelemetryX64.so', 'libmimalloc.so',
                                     'libfreetype.so', 'libfontconfig.so', 'libharfbuzz.so',
                                     'sourcevr.so'},
        roots='config/hl2-roots.json',
        steam_appid=220,
        depot_id=224,
        app_name='Half-Life 2 Natif',
        log_name='HalfLife2Natif.log',
        bundle_id='fr.kanapitsas.hl2-native',
        icon='hl2/resource/game.icns',
        renderer=None,   # Metal when libd3d9metal is there, as for Portal 2 (docs/hl2.md)
        build_dir='build/hl2',
        binary='build/hl2/hl2-native',
        executable='hl2-native',
        ios_dir='build/hl2/ios',
        ios=False,   # Mac only for now
    ),
}


DEFAULT = 'portal2'
# Executable names of every game: "is a recompiled game running" checks
# (uninstall.sh, tests) must cover them all.
EXECUTABLES = tuple(g['executable'] for g in GAMES.values())


def check(name, origin='--game'):
    """`name` if it is a known game, else a clear exit."""
    if name not in GAMES:
        raise SystemExit(f'unknown game {name!r} ({origin}; known: {", ".join(GAMES)})')
    return name


def selected(name=None, config=None, environ=None):
    """The game a tool works on: `name` (its --game option), else P2_GAME,
    else "game" of the config, else the only game the config sets up (an
    HL2-only checkout needs no --game), else portal2."""
    environ = os.environ if environ is None else environ
    if name: return check(name)
    if environ.get('P2_GAME'): return check(environ['P2_GAME'], 'P2_GAME')
    config = config or {}
    if config.get('game'): return check(config['game'], 'config/local.json')
    if not config.get('game_root'):   # top-level game keys are Portal 2's
        configured = [n for n, section in (config.get('games') or {}).items()
                      if n in GAMES and isinstance(section, dict) and section.get('game_root')]
        if len(configured) == 1: return configured[0]
    return DEFAULT


def add_argument(parser):
    """--game option of the tools (default: selected())."""
    parser.add_argument('--game', choices=sorted(GAMES),
                        help='game to work on (default: P2_GAME, else "game" of config/local.json, else portal2)')


def profile(config):
    """Profile of the game named by `game` in the config (portal2 when absent);
    localconfig.game_config() returns such a config for any game."""
    name = config.get('game') or DEFAULT
    return dict(GAMES[check(name, 'config/local.json')], name=name)
