# Usage details

More detail than the [README](../README.md) needs.

## Game language

The game starts in the language chosen by `tools/language.py`, which adds
`-language <name>` to the launch. First match wins:

1. a `-language <name>` you pass yourself (for example
   `python3 tools/make_app.py -- -game portal2 -novid -language french`);
2. the environment variable `P2_LANGUAGE`
   (`P2_LANGUAGE=german "$HOME/Applications/Portal 2 Natif.app/Contents/MacOS/launcher"`);
3. `"language": "french"` in `config/local.json` (the app keeps a copy of
   this file: re-run `python3 tools/make_app.py` after changing it);
4. the language set in the Steam properties of Portal 2, read from
   `appmanifest_620.acf` in the `steamapps/` folder that contains `common/`
   (then from Steam's own `steamapps/`);
5. your macOS system language;
6. English.

Names are Steam's: `french`, `german`, `spanish`, `italian`, `russian`,
`schinese`, `brazilian`, `koreana`, and so on (one `gameui_<name>.txt` per
language in `portal2/resource/`). A language missing from your game folder
falls back to English. Steam only downloads the voices of the language set in
its properties: if you installed the game in English, menus and subtitles are
translated but the voices stay English. To get them, switch the language in
Steam (Properties > Language) and let it update the game first.

`python3 tools/language.py --explain` prints the choice and where it comes
from. On the iPhone, the language is written into the app at build time
(same rules, rebuild after changing `config/local.json`).

## Controllers

**Mac**: turn Steam Input off for Portal 2 in Steam (*Portal 2 → Properties →
Controller → Disable Steam Input*). With Steam Input on, Steam takes the
controller as soon as the game starts and applies a gamepad-emulation
template to it. On macOS that emulation only reaches games that Steam itself
launches, so this app gets no input: the controller shows as connected, but
nothing works. Tested with an 8BitDo SN30 Pro over Bluetooth in Switch mode
(seen as a Nintendo Switch Pro Controller). With `P2_INPUT_LOG=1`, the log
(`~/Library/Logs/Portal2Natif.log`) prints the controller state every half
second, accelerometer included: if those values stay at zero, Steam still has
the controller.

**iPhone**: there is no Steam client, so nothing to set. When a controller
connects, the on-screen controls hide themselves and remove the virtual
controller they give the game. The game keeps one controller slot per
split-screen player and player 1 reads only the first; a controller that
connected while the virtual one held that slot would drive the menus but not
the game, so the app unplugs and replugs it for the game, which then puts it
in the first slot (`runtime/ios/touch_controls.m`).

## LAN co-op

Two copies of the game on the same network can play co-op without a Steam lobby.
Enable the developer console in the game's keyboard options, then:

- **Host**: `sv_lan 1`, then `map mp_coop_lobby_3`.
- **Partner**: `connect <host LAN address>`, for example `connect 192.168.1.21`.
  Use the LAN address even when both copies run on the same Mac: `127.0.0.1`
  does not reach the other copy.

On iPhone, add two lines to `Documents/launch.args` in the app's files:
`+connect` and the host's address. iOS asks once for local network access:
allow it.

| Case | Status |
|---|---|
| Two copies on one Mac | Tested: the partner loads the map and stays connected |
| Mac hosts, iPhone joins | Tested, same result |
| Two Macs | Should work the same way; not tested |
| iPhone hosts | Not tested |
| Steam invites and lobbies | Not tested; some Steam calls used by matchmaking are not bridged yet |
| Over the internet | Not tested; the host needs `sv_lan 0` and UDP port 27015 open |
| With a Windows or Linux PC | Not tested |

Only the connection and the map load were checked. A full co-op map played
by two people has not been tried yet.

## Saves between Mac and iPhone

Over the cable, with the iPhone unlocked and the game quit on both sides
(`tools/ios_device.sh`, logic in `tools/save_sync.py`):

```sh
tools/ios_device.sh saves status             # both sides, dates, which is newer per file
tools/ios_device.sh saves pull --dry-run     # what pull would do, without copying
tools/ios_device.sh saves pull               # iPhone -> Mac
tools/ios_device.sh saves push               # Mac -> iPhone
```

What is copied: the save files (`*.sav`, and `*.tga` screenshots if there are
any) of `portal2/SAVE/<SteamID64>/`, nothing else:

- Mac: `<user_dir>/portal2/SAVE/<your SteamID64>/` (the folder with the most
  recent change; `P2_SAVE_ID=<SteamID64>` picks another one). Saves the game
  sees in the Steam game folder (`<game_root>/portal2/SAVE/<id>/`, under the
  user data) count as Mac saves for `push`, unless deleted in the game.
- iPhone: `Documents/portal2-userdata/portal2/SAVE/76561197960265729/`, the ID
  of the offline Steam stub (`runtime/ios/steam_stub.c`). A file keeps its name
  and moves to the other side's folder.
- Not copied: `cfg/config.cfg` and `update/cfg/video.txt` (bindings, mouse,
  controller and touch settings, video modes: different on each device),
  `*.hl1`/`*.hl2`/`*.hl3` (level state that the engine rebuilds from a `.sav`),
  `steam_autocloud.vdf` (Steam Cloud's account marker),
  `update/save/game_instructor_counts.txt` (hint counters). Chapter progress
  in the menu: `sv_unlockedchapters` in `config.cfg` stays at 1 on the Mac even
  with late saves, so it is not what unlocks chapters on PC; not synced.

Never silently overwritten: after each copy, `~/Library/Application
Support/Portal 2 Natif/save-sync/<device>.json` records both sides' date and
size. A file changed on one side only since then is copied that way; with no
record yet, the newer date wins and the same size within 2 s counts as the
same file. A file newer on the destination, or changed on both sides, is kept
and named in the output; `--force` copies it anyway. Before any overwrite, the
old copy goes to `save-sync/backups/<date>-pull|push/` (for `push`, pulled from
the iPhone first). A pulled save arrives as `<name>.p2part` and replaces the
save only once complete, with the iPhone's date (the game sorts saves by date).
`pull` refuses to run while `portal2-native` runs on the Mac. `./uninstall.sh`
treats `save-sync/` like the rest of the app's folder (Trash, after asking).

Deletions are not synced: devicectl cannot delete a file on the device, so a
save deleted on one side comes back from the other on the next copy.

Steam Cloud: the cloud keeps the saves of the Steam game folder
(`<game_root>/portal2/SAVE/<id>/`, marked by `steam_autocloud.vdf`), synced by
the Steam client that launches the game from that folder. This app never writes
there: it reads that folder under its own user data and writes its saves to
`user_dir`. `saves pull` follows the same rule, so it never competes with Steam
Cloud; the other side of it is that saves made in this app (and those pulled
from the iPhone) do not reach Steam Cloud. The iPhone has no Steam client.

A version over iCloud, without the cable, needs the iCloud capability, which
free Apple developer accounts cannot sign: it would need a paid account.

## Manual build

`config/local.json` (ignored by git; `setup.sh` writes it):

```json
{
  "game_root": "/path/to/Portal 2",
  "linux_root": "/path/to/depot_661",
  "steam_appid": 620,
  "steam_api_dylib": "/path/to/steam_api.bundle/Contents/MacOS/libsteam_api.dylib",
  "user_dir": "/path/to/saves",
  "language": "french",
  "d3d9metal_dylib": "/path/to/libd3d9metal.dylib",
  "d3d9metal_ios": {"device": "/path/to/libd3d9metal.a"},
  "ios_bundle_id": "com.yourname.portal2",
  "host_prefix": "/path/to/deps/p2-host-deps-1-macos-arm64",
  "depot_id": 661,
  "games": {
    "hl2": {
      "game_root": "/path/to/Half-Life 2",
      "linux_root": "/path/to/depot_224",
      "steam_appid": 220,
      "steam_api_dylib": "/path/to/steamworks-2024.8.0/libsteam_api.dylib"
    }
  }
}
```

- One file for both games. The top-level keys are shared (`language`,
  `d3d9metal_dylib`, `d3d9metal_ios`, `host_prefix`) or Portal 2's own
  (`game_root`, `linux_root`, `steam_appid`, `depot_id`, `manifest`,
  `steam_api_dylib`, `user_dir`, `ios_bundle_id`), as in older single-game
  configs; Half-Life 2's own keys go in `games.hl2`
  (`localconfig.game_config` merges them). An older Half-Life 2 checkout's
  config (`"game": "hl2"` with its keys at the top level) is still read, and
  `setup.sh` moves those keys into `games.hl2`.
- Which game a tool works on: `--game portal2|hl2` (`GAME=` for `make
  game`), else `P2_GAME`, else `"game"` in this file, else the only game set
  up, else Portal 2. Portal 2 builds in `build/game` (binary
  `build/portal2-native`, iPhone builds in `build/ios`), Half-Life 2 in
  `build/hl2` (binary `build/hl2/hl2-native`, iPhone builds in
  `build/hl2/ios`).

- `language`, `d3d9metal_dylib` and `d3d9metal_ios` are optional:
  [game language](#game-language), [Metal renderer](d3d9-bridge.md#using-it),
  [Metal on iPhone](ios.md#metal-renderer-default).
- `ios_bundle_id` must be unique to your Apple account (free accounts cannot
  share identifiers); it can also be given with `P2_BUNDLE_ID`.
- Without `user_dir`, the game keeps saves in
  `~/Library/Application Support/Portal 2 Natif/userdata` (`Half-Life 2 Natif`
  for Half-Life 2; `tools/localconfig.py`,
  the same default for `run_game.py`, `make_app.py` and `bench_renderer.py`);
  `tools/make_app.py` copies the older default `../work/portal2-userdata`
  there once if it exists (the original stays).
- `host_prefix` is where SDL2, FreeType and Fontconfig are compiled from
  (`tools/localconfig.py`; `P2_HOST_PREFIX` overrides it, the Makefile reads
  it too): `setup.sh` sets it to the prebuilt bundle when Homebrew lacks
  them, and removes it to build against `/opt/homebrew`.
- `depot_id` is written by `setup.sh` for information only: nothing in the
  repository reads it.

macOS:

```sh
python3 tools/gameprep.py                 # prepare the Linux modules (seconds)
python3 tools/build_game.py --release     # lift and compile (~10 min the first time)
python3 tools/run_game.py --show --timeout 3600
python3 tools/make_app.py                 # optional: an app for the Dock / Finder (re-run after a rebuild)
```

The same with `--game hl2` for Half-Life 2.

iPhone: see [Building and running](ios.md#building-and-running) in ios.md.

## Run

From the repository, with the Steam client open:

```sh
python3 tools/build_game.py --release  # to play: -O2 without memory checks (~10 min from scratch)
python3 tools/build_game.py            # diagnostics: -O1, checked memory (~20 min from scratch)
python3 tools/run_game.py --show --timeout 3600
```

`run_game.py` sets `SteamAppId` and `P2_STEAM_API_DYLIB` from the game's
`steam_appid` and `steam_api_dylib` when they are not in the environment
(the runtime itself only reads the top-level, Portal 2 keys). Without
`--show`, the window is hidden (`P2_HIDDEN_WINDOWS=1`), vsync is disabled and
the game runs on a copy of the user data
(`<user_dir>/../portal2-userdata-diag`, `hl2-userdata-diag` for Half-Life 2).
Add `--game hl2` to each command for Half-Life 2.

The binary itself (`build/portal2-native`, what `run_game.py` and the app
launch) takes `--images <prep dir> --game-dir <game> [--linux-root <depot>]
[--user-dir <saves>] -- <game arguments>` (`src/main.c`).

To play without a terminal: `python3 tools/make_app.py` creates
`~/Applications/Portal 2 Natif.app` (`--dest` for another folder): a copy of
`build/portal2-native` with its host dylibs (Homebrew's or the prebuilt
bundle's) — SDL2/sdl2-compat and the
SDL3 it loads, FreeType, Fontconfig and their dependencies — in
`Contents/Frameworks` and the Fontconfig configuration in
`Contents/Resources/fonts`, so it runs without them; paths from
`config/local.json`, starts Steam if needed, no `-w/-h`: resolution and mode
come from the Video menu, saved in `update/cfg/video.txt`. Log:
`~/Library/Logs/Portal2Natif.log`. Re-run it after a rebuild (the app holds a
copy of the binary) or when the paths or arguments change.

Retina (on by default, `P2_RETINA=0` restores the old behavior): the screen
is reported in pixels (3024×1964), so the Video menu also offers the large
resolutions from the game's table (up to 2560×1600). A resolution that fits
on screen in points gives a normal window (×1 rendering); a larger one gives a
Retina window half its size in points (2560×1440 → 1280×720-point window,
native rendering). `wantsBestResolutionOpenGLSurface` is toggled on the fly.
Fullscreen not verified in Retina.
F3 shows/hides a host frame counter (`P2_FPS=1`: visible from the start).

## Diagnostics

Environment variables read by the runtime on Mac (the iPhone list is in
[ios.md](ios.md#diagnostics)). Output goes to stderr, so to the log.

| Variable | Effect |
|---|---|
| `P2_FRAME_LOG=1` | frame count and fps every 60 frames, on OpenGL and on the Direct3D 9 (Metal) renderer |
| `P2_FRAME_TIMES=<file>` | one line per present on both renderers (start time, time inside the call, CPU time); read by `tools/bench_renderer.py` |
| `P2_GPU_TIMING=1` | GPU time per frame (`GL_TIME_ELAPSED` queries; slows the game) |
| `P2_GL_TIMING=1` | time spent in each GL entry point (including the unsynchronized `glBufferSubData` path) and in the buffer swap |
| `P2_GATE_TIMING=1` | time the main thread spends waiting for the guest gate (`runtime/scheduler.c`) |
| `P2_BLIT_TRACE=<n>` | 300 `glBlitFramebuffer` copies starting from the n-th, with the draws made into the source surface since the previous copy |
| `P2_PASS_TRACE=<s>` | traces one frame's render passes after s seconds |
| `P2_FRAME_DUMP=/path/to/file.ppm` | back-buffer capture |
| `P2_FPS=1` | host frame counter visible from the start (F3 toggles it) |
| `P2_HIDDEN_WINDOWS=1` | hidden window, vsync off (what `run_game.py` does without `--show`) |
| `P2_FAKE_FOCUS=1` | with a hidden window, tells the game once that it has focus |
| `P2_RETINA=0` | screen reported in points instead of pixels |
| `P2_RENDERER=opengl` / `d3d9` | force OpenGL, or make a missing Metal library an explicit message |
| `MTLD3D_CONFIG=render.scale=0.75` | Metal renderer: render the scene at 75 % of the back buffer (the Video menu resolution) and upscale it with MetalFX; `0.5` is faster and softer, any value in (0, 1] works, and other mtld3d keys can follow after `;`. From the app: `MTLD3D_CONFIG=render.scale=0.75 "~/Applications/Portal 2 Natif.app/Contents/MacOS/launcher"` |
| `P2_INPUT_LOG=1` | timestamped mouse and keyboard events as the game receives them |
| `P2_SDL_TRACE=1` | every SDL video call made by the game, with arguments and result (`RT_SDL_TRACE` is the older name) |
| `P2_TRACE_WAIT=1` | `SDL_WaitEventTimeout` calls with their timeout and caller |
| `P2_TRACE_STAT=1` | every `stat`/`lstat` call and its result |
| `P2_TRACE_MMAP=1` | failed `mmap` and `malloc` calls with the arena and mapped sizes |
| `P2_WATCH_CALL=0x<address>` | arguments of a guest call (needs `--traced-calls`, see [fast-path.md](fast-path.md)) |
| `P2_GUEST_CPUS=<n>` | number of CPUs reported to the game (1 by default, up to 16; 2 or 4 measured slower) |
| `P2_NATIVE_<NAME>=0` | restores the translated function behind a native replacement (`PATHMATCH`, ... in `tools/lift.py` `NATIVE_OVERRIDES`) |
| `P2_IMPORT_COUNTS=1` | counts calls per import |
| `P2_PROFILE=<file>` | built-in SIGPROF profiler (over-weights system calls on macOS) |
| `P2_PE_TRACE=1` | Miles plugin functions called (`.asi`, MP3 voices) |
| `P2_GL_STREAM_UNSYNC=0` | disables unsynchronized `glBufferSubData` appends |

Host faults are translated into a guest address plus a guest stack.

## Releases

Maintainer steps; users never need them.

1. **Host libraries**, when a version in `tools/build_host_deps.sh` or
   `MTLD3D_REV` in `setup.sh` changes: bump `BUNDLE_VERSION`, run
   `tools/build_host_deps.sh` (needs `cmake`, `ninja`, `pkg-config`, Rust;
   meson goes in a venv), upload `p2-host-deps-<n>-macos-arm64.tar.xz` to a
   release tagged `host-deps-<n>`, then set `HOST_DEPS_NAME`,
   `HOST_DEPS_URL` and `HOST_DEPS_SHA256` in `setup.sh`. The script checks
   that every library is relocatable (`@rpath`), targets macOS 13 and holds
   no path of the build machine. Sources and checksums are Homebrew's.
2. **Signing, once per Mac**: a "Developer ID Application" certificate in
   the keychain (Xcode > Settings > Accounts > your team > Manage
   Certificates > + > Developer ID Application; the team's Account Holder
   creates it) and a notarytool profile with an app-specific password from
   account.apple.com:
   `xcrun notarytool store-credentials p2-notary --apple-id <Apple ID> --team-id <team ID>`.
3. **Installer**: tag the release, run `tools/build_installer.sh` (the app
   embeds `git archive HEAD`, so commit first) and upload
   `build/installer/Natif-Installer-<tag>.zip` (the name is
   `INSTALLER_NAME` in the script). With the certificate and
   the profile, the script signs with the hardened runtime, notarizes, staples
   and checks the result with `spctl`; it prints which of these it did.
   Without the certificate the app is ad hoc signed and users go through
   System Settings > Privacy & Security > Open Anyway once.
   `P2_SIGN_IDENTITY` picks another certificate, `P2_NOTARY_PROFILE` another
   profile.
