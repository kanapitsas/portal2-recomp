# Portal (experimental)

The same pipeline also recompiles **Portal** (Steam app 400, the Source 2013
Linux build, mod `portal`). It is experimental: the Mac app starts, plays the
first chambers and renders on Metal or OpenGL, but only the first chambers
have been tested and the indirect call roots are still being discovered. The
iPhone app installs, starts and renders in a short session; no chamber has
been played through on the phone yet.

**You must own Portal.** As for Portal 2, everything is generated from your
own copy and nothing from Valve is in this repository. The app and the
generated code contain Valve's game: keep them for yourself.

## Install

You need what Portal 2 needs (see the [README](../README.md#what-you-need)),
Portal's game data and its Linux binaries (two separate downloads):

1. Install Portal with Steam (the version with `portal/` and `platform/`), or
   open the Steam console (`open steam://open/console`) and type
   `download_depot 400 401` to download its game data. Wait for "Depot
   download complete".
2. Open the Steam console and type `download_depot 400 404` (about 150 MB).
   Wait for "Depot download complete".
3. Tick Portal in the Natif Installer, or run `./setup.sh --game portal1`
   (`./setup.sh` alone builds every game found) in a checkout, the same one
   as Portal 2 if you have it ([README](../README.md#install-on-mac)).
   Without paths, it finds the game in
   `~/Library/Application Support/Steam/steamapps/common/Portal` or
   `depot_401`, and the Linux binaries in `depot_404`, where
   `download_depot` put them; otherwise pass them:
   `./setup.sh --game portal1 "/path/to/Portal" "/path/to/depot_404"`.

What differs from the Portal 2 install: `setup.sh` downloads Valve's
`libsteam_api.dylib` from Steamworks.NET 2024.8.0 (Portal asks the SDK 1.60
interfaces, like Half-Life 2), builds in `build/portal1` (about 10 minutes
and a few GB the first time; Portal 2's `build/game` is left alone) and
creates `~/Applications/Portal Natif.app`. Open Steam with the account that
owns Portal, then open the app.

- Saves and settings: `~/Library/Application Support/Portal Natif/userdata`.
- Log: `~/Library/Logs/PortalNatif.log`.
- Renderer: **Metal** when `libd3d9metal` is there, as for Portal 2. To force
  OpenGL: `open --env P2_RENDERER=opengl -a "Portal Natif"`.
- `./uninstall.sh` offers to remove the Portal app and, if you confirm, its
  saves, with separate questions for the other games.

## What differs from Portal 2

- **Game profile** (`tools/games.py`): program `hl2_linux`, game libraries
  under `bin/` and `portal/bin/`, roots in `config/portal1-roots.json` (three
  found so far), app name, log, data folder, and build paths: `build/portal1`
  (prep, gen, obj, `launch.log`) and the binary `build/portal1/portal1-native`
  (also its name in the app). The tools pick it with `--game portal1` (or
  `P2_GAME=portal1`).
- **Linux binaries**: depot 404, built like Half-Life 2's depot 224, with
  Miles' plugins (`bin/linux32/mssmp3.asi`, `mssmixer.mix`, `binkawin.asi`):
  Portal 1's sound and voices come from the Linux depot, not from the game
  folder, which is why the iPhone copy leaves `bin/` out and takes the whole
  Linux depot instead.
- **Steamworks**: SDK 1.60 interfaces, newer than Portal 2's and the same
  set as Half-Life 2's (`SteamClient021`, `SteamUser023`, `SteamUtils010`,
  ...). The bridge calls them by vtable slot against Valve's
  `libsteam_api.dylib` from Steamworks.NET 2024.8.0 (`steam_api_dylib` in the
  config). See [steam.md](steam.md).
- **Toolchain**: the same Source 2013 engine as Half-Life 2's recent depot,
  so the same D3D9 bridge (`shaderapivk.so` creates Direct3D through
  `Direct3DCreate9Ex`) applies; without `libd3d9metal` the game uses OpenGL.
- **No portal gun upgrade**: Portal 1 keeps the dual portal gun for most of
  the game, so Portal 2's touch layout (both portal buttons, back taps fire)
  fits as it is.

## Manual setup

The `config/local.json` sample and the `gameprep` → `build_game` →
`make_app`/`run_game` sequence are in [usage.md](usage.md#manual-build): add
`--game portal1` to each command. Portal's keys (`game_root`, `linux_root`,
`steam_api_dylib`, `steam_appid` 400, `depot_id` 404) go under
`games.portal1`.

## What was tested

Tested from this checkout on a Mac with an M5 Max (12 November 2026):

- the menu then `testchmb_a_00` (played from the app);
- `testchmb_a_01` to `testchmb_a_05` and the later `testchmb_a_10`, each
  started from the console with `+map` and left running 70 s: the map loads,
  the game loop runs, no `target not recompiled` and no crash (Metal, the
  default; window hidden for the test);
- the normal `env_cubemap` warnings from unbuilt maps are the only ones
  worth noting.

Not tested: a playthrough, a chamber with energy balls, the advanced
chambers, performance measurements, the Mac↔iPhone save copy (the cable
`saves` command is Portal 2 only for now). The iPhone app was installed and
launched in a short earlier session (menu and touch layout); no chamber has
been played on the phone.

To reach a chamber without the menu, with Steam running:

```sh
~/Applications/Portal\ Natif.app/Contents/MacOS/launcher -- -novid +map testchmb_a_02
```

(On the Mac app the arguments after `--` go to the game; they are not passed
when opened from Finder.)

## Known limits

- Indirect call targets are still being discovered; a crash ending with
  `target not recompiled` means one more root. Please open an issue with the
  end of `~/Library/Logs/PortalNatif.log` and where it happened.
  `tools/discover_game.py --game portal1` finds them from real runs
  (arguments after `--` go to the game, e.g. `+load <save>`).

## iPhone

`./setup.sh --game portal1 --ios` builds the Mac app, then the iPhone app
(`build/portal1/ios/device/Portal.app`, "Portal" on the home screen,
identifier `local.portal1recomp.<team>` in `games.portal1.ios_bundle_id` of
`config/local.json`, so it sits next to Portal 2's and Half-Life 2's),
installs it and copies about 4 GB of data (`platform portal hl2`; the whole
Linux depot comes along, about 150 MB, which is where the Miles plugins and
the Steam stub need it). By hand, as for Portal 2:

```sh
python3 tools/build_ios.py --game portal1 --target device --release --deps deps/ios-deps/device
tools/ios_device.sh --game portal1 install && tools/ios_device.sh --game portal1 data
```

Saves live in the app's `Documents/portal-userdata`. The touch layout is
Portal 2's (both portal buttons, back taps fire the portal; the Settings page
is Portal 2's, Portals group included). `tools/ios_device.sh saves` is still
Portal 2 only, so copying Portal's saves between the Mac and the phone is by
hand for now. What has been checked on the phone: install, signing, launch,
the menu and the touch layout in a short session; not a chamber.
