# Half-Life 2 (experimental)

The same pipeline also recompiles **Half-Life 2** (20th anniversary build,
Steam app 220, mod `hl2_complete`). It is experimental: it installs and plays
on Mac, but only a few hours of play have been tested, the episodes and Lost
Coast have only been started, and the iPhone build has had a first short test
on an iPhone 15 Pro Max (see [iPhone](#iphone)).

**You must own Half-Life 2.** As for Portal 2, everything is generated from
your own copy and nothing from Valve is in this repository. The app and the
generated code contain Valve's game: keep them for yourself.

## Install

You need what Portal 2 needs (see the [README](../README.md#what-you-need)),
Half-Life 2's game data, and its Linux binaries (two separate downloads):

1. Install Half-Life 2 with Steam (the current version, which has
   `hl2_complete/`), or open the Steam console (`open steam://open/console`)
   and type `download_depot 220 221` to download its game data. Wait for
   "Depot download complete".
2. Open the Steam console (`open steam://open/console`) and type
   `download_depot 220 224` (about 320 MB). Wait for "Depot download
   complete".
3. Tick Half-Life 2 in the Natif Installer, or run `./setup.sh --game hl2`
   (`./setup.sh` alone builds every game found) in a checkout, the same one
   as Portal 2 if you have it ([README](../README.md#install-on-mac)).
   Without paths, it finds the game in
   `~/Library/Application Support/Steam/steamapps/common/Half-Life 2` or
   `depot_221`, and the Linux binaries in `depot_224`, where `download_depot`
   put them; otherwise pass them:
   `./setup.sh --game hl2 "/path/to/Half-Life 2" "/path/to/depot_224"`.

What differs from the Portal 2 install: `setup.sh` downloads Valve's
`libsteam_api.dylib` from Steamworks.NET 2024.8.0 (HL2 needs a newer SDK than
Portal 2), builds in `build/hl2` (10 to 20 minutes and about 4 GB the first
time; Portal 2's `build/game` is left alone) and creates
`~/Applications/Half-Life 2 Natif.app`. Open Steam with the account that
owns Half-Life 2, then open the app.

- Saves and settings: `~/Library/Application Support/Half-Life 2 Natif/userdata`.
- Log: `~/Library/Logs/HalfLife2Natif.log`.
- Renderer: **Metal** when `libd3d9metal` is there, as for Portal 2. To force
  OpenGL: `open --env P2_RENDERER=opengl -a "Half-Life 2 Natif"`.
- `./uninstall.sh` offers to remove the HL2 app and, if you confirm, its
  saves, with separate questions for Portal 2's.

## What differs from Portal 2

- **Game profile** (`tools/games.py`): program `hl2_linux`, game libraries
  under `bin/` and `hl2_complete/bin/`, roots in `config/hl2-roots.json`,
  app name, log, data folder, and build paths: `build/hl2` (prep, gen, obj,
  `launch.log`) and the binary `build/hl2/hl2-native` (also its name in the
  app). The tools pick it with `--game hl2` (or `P2_GAME=hl2`).
- **Linux binaries**: depot 224 (all i386), with Miles' plugins
  (`bin/linux32/*.asi`), which Portal 2 takes from the game folder instead.
- **Steamworks**: HL2's `client.so` asks for interfaces from SDK 1.60 and
  1.61, newer than Portal 2's. The bridge calls them by vtable slot
  (`tools/gen_steam.py`), against Valve's `libsteam_api.dylib` from
  Steamworks.NET 2024.8.0 (`steam_api_dylib` in the config). See
  [steam.md](steam.md).
- **Renderer**: HL2's `shaderapivk.so` creates Direct3D through
  `Direct3DCreate9Ex` and has no fallback, so the D3D9 bridge also covers
  `IDirect3D9Ex`, `IDirect3DDevice9Ex` and `IDirect3DSwapChain9Ex`
  ([d3d9-bridge.md](d3d9-bridge.md)). Without `libd3d9metal`, the game uses
  OpenGL (togl); the runtime then adds `-gl_disablesamplerobjects`, because
  macOS's legacy OpenGL context has no sampler objects and HL2's togl would
  otherwise lose all texture filtering.
- **Newer toolchain**: the build is vectorised with SSSE3, uses glibc's
  fortified functions, and its Crypto++ picks AES-NI from CPUID. The lifter
  handles SSSE3, traps CPUID-gated instructions where they stand (they are
  never reached, since the reported CPU lacks them), and recognises more GCC
  switch forms (moves scheduled inside the jump sequence, fused table loads,
  pre-scaled indexes).

## Manual setup

The `config/local.json` sample and the `gameprep` → `build_game` →
`make_app`/`run_game` sequence are in [usage.md](usage.md#manual-build): add
`--game hl2` to each command. Half-Life 2's keys (`game_root`, `linux_root`,
`steam_api_dylib`, `steam_appid` 220, `depot_id` 224) go under `games.hl2`.

## Performance

On an M3 Pro at 1280×720 a `--release` build holds the display's 120 fps with
Metal (about 91 with OpenGL); the measurements, the ceiling estimate and the
rejected experiments are in
[STATUS.md](../STATUS.md#half-life-2-78-november-2026).

## iPhone

`./setup.sh --game hl2 --ios` builds the Mac app, then the iPhone app
(`build/hl2/ios/device/HalfLife2.app`, "Half-Life 2" on the home screen,
identifier `local.hl2recomp.<team>` in `games.hl2.ios_bundle_id` of
`config/local.json`, so it sits next to Portal 2), installs it and copies
about 10 GB of data (`bin platform hl2 episodic ep2 lostcoast hl2_complete`;
about 5 minutes over the cable, so keep 10 GB free on the phone).
`./setup.sh --ios` does the same for every game it builds; it has not been
tested end to end yet, the steps by hand below are what ran on the phone:
`python3 tools/build_ios.py --game hl2 --target device --release --deps deps/ios-deps/device`, then
`tools/ios_device.sh --game hl2 install|data|run` on a phone or
`tools/run_ios_sim.sh --game hl2 [seconds]` in a headless simulator.
Saves live in the app's `Documents/hl2-userdata`; `tools/ios_device.sh saves`
is Portal 2 only for now. Details, the touch layout and what has been checked:
[ios.md](ios.md#half-life-2).

Touch controls in short: move stick (left), camera stick or trackpad (right)
plus gyro aiming, fire (bottom right, drag it to aim), alt-fire, reload, jump,
use, crouch and sprint as toggles, weapon chevrons, flashlight, quick save
(hold it to quick load) and pause in the top bar. Back taps are off (`P2_BACKTAP=1` maps them to fire and alt-fire). A
controller hides the overlay, as for Portal 2.

## Known limits

- Indirect call targets are still being discovered; a crash ending with
  `target not recompiled` means one more root. Please open an issue with the
  end of `~/Library/Logs/HalfLife2Natif.log` and where it happened.
  `tools/discover_game.py --game hl2` finds them from real runs (arguments after `--`
  go to the game, e.g. `+load <save>`).
- The game wants the `en_US.UTF-8` locale and warns about glyph support: the
  runtime only provides `C` and `C.UTF-8`.
- iPhone: one short session on an iPhone 15 Pro Max (8 November 2026), at
  60 fps; not played for long.
- Only the main game has been played (a few hours). Episode One, Episode Two
  and Lost Coast are in the same `hl2_complete` build and start from the
  main menu, which switches the game's search paths (`SwitchExpansion`), so
  `+map ep1_citadel_00` alone says "No such map". Their first maps
  (`ep1_citadel_00`, `ep2_outland_01`, `d2_lostcoast`) load and render on
  Metal; they have not been played through. To reach one without the menu:
  launch with `-gamepadui` and run `gamepadui_switchexpansion episodic`
  (or `ep2`, `lostcoast`), then `map <name>`.

## Dark interiors

Indoors, Half-Life 2 is meant to be dark: its maps cap the automatic
exposure at 2× (`env_tonemap_controller`, `SetAutoExposureMax 2` on map
spawn), and the canals and later areas expect the suit flashlight (`F`).
Measured on 8 November 2026 at the start of `d1_trainstation_03`: the Metal
and OpenGL renderers give the same image, the exposure logic computes the
same values as the Source SDK 2013 code (target 10×, clamped to 2×), and the
default gamma ramp is the identity, so the default picture matches Windows.

**Options > Video > Brightness** brightens it. Source applies that setting as
a Direct3D 9 gamma ramp, which Direct3D 9 (and libd3d9metal) only applies to
a fullscreen device; on the Mac the game runs windowed or in desktop
fullscreen, so the runtime forwards the ramp to the game window
(`SDL_SetWindowGammaRamp`, as Portal 2's OpenGL path does). macOS applies it
to the whole display while the game has focus, and SDL removes it when the
game loses focus or quits. Mac only: on iPhone the slider is not checked.
