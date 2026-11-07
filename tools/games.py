"""Per-game layout of the Linux depot: which modules to prepare and how the
program is named, plus what setup.sh and tools/make_app.py need to install it
(Steam depot, Steamworks.NET release, app name, data folder). `game` in
config/local.json picks the profile (default portal2, so existing
configurations keep working). One checkout builds one game: build/ is shared.
"""
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
    ),
}


def profile(config):
    """Profile of the game named by `game` in the config (portal2 when absent)."""
    name = config.get('game', 'portal2')
    if name not in GAMES:
        raise SystemExit(f'unknown game {name!r} in config/local.json (known: {", ".join(GAMES)})')
    return dict(GAMES[name], name=name)

