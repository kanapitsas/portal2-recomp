# Portal 2, statically recompiled for Apple silicon

[![Portal 2 running natively on iPhone (video)](https://img.youtube.com/vi/YWX2BeMnlQI/maxresdefault.jpg)](https://youtu.be/YWX2BeMnlQI)

*Video: Portal 2 running natively on an iPhone 15 Pro Max.*

<p>
  <img src="docs/images/iphone-menu.jpg" alt="Portal 2 main menu on iPhone, 60 fps" width="49%">
  <img src="docs/images/iphone-ingame.jpg" alt="Portal 2 in game on iPhone with touch controls" width="49%">
</p>

This project turns the Linux version of Portal 2 into a native ARM64 app for
macOS and iPhone. The game's x86 machine code is translated into C and
compiled: no emulator, no Wine, no Rosetta. A small runtime provides what the
Linux binaries expect (libc, threads, SDL2, graphics, Steam).

**You must own Portal 2.** This repository has no game files and no Valve
code: everything is generated on your machine from your own copy. The app and
the generated code contain Valve's game, so keep them for yourself and don't
share them.

> Unofficial fan project, not affiliated with or endorsed by Valve. Portal,
> Portal 2, Steam and Source are trademarks of Valve Corporation. Provided as
> is, without warranty.

## Status at a glance (5 November 2026)

- **Mac**: fully playable. 235–280 fps with OpenGL (M3 Pro, 720p to
  1512×982, 26 Oct.); Metal, now the default, adds about 50 % (122 vs 81 fps
  at 2560×1440, MSAA 4×).
- **iPhone**: fully playable on an iPhone 15 Pro Max at a steady 60 fps, with
  touch controls and portals fired by **tapping the back of the phone**.
- **Controllers**: physical controllers work on Mac and iPhone (tested with
  an 8BitDo SN30 Pro). On Mac, turn Steam Input off for Portal 2.
- **Co-op** over the local network: a Mac hosts, an iPhone or a second copy
  on the same Mac joins. Connection and map load are checked; a full co-op
  course has not been played yet.
- **Install**: the Portal 2 Installer app from the
  [releases](https://github.com/kanapitsas/portal2-recomp/releases), or one
  command, `setup.sh`. Only the Xcode command line tools are required.

## How it works

1. `tools/elfprep.py` and `tools/gameprep.py` lay out the Linux ELF modules
   (`engine.so`, `client.so`, ...) at fixed guest addresses, apply their
   relocations offline and record imports, exports and entry points.
2. `tools/lift.py` translates every x86 function into a C function; `clang`
   compiles the result together with the runtime into one ARM64 executable
   (`build/portal2-native`). Indirect call targets that the static pass
   misses are collected at run time into `config/game-roots.json`.
3. The runtime (`runtime/`) implements what the binaries import: libc,
   pthreads and futexes, SDL2, OpenGL, Direct3D 9 and Steam. Guest threads
   are real threads serialized by one lock, the guest gate
   ([docs/fast-path.md](docs/fast-path.md)).
4. Steam calls are forwarded to Valve's own arm64 `libsteam_api.dylib`
   (from Steamworks.NET) through generated i386 → arm64 proxies; there is no
   Steam emulator ([docs/steam.md](docs/steam.md)).
5. Rendering has two paths: Valve's `togl` translating Direct3D 9 to legacy
   OpenGL 2.1 ([docs/gl-audit.md](docs/gl-audit.md)), or Valve's
   `shaderapivk.so` talking to **mtld3d**, a native Direct3D 9 on Metal
   ([docs/d3d9-bridge.md](docs/d3d9-bridge.md)). mtld3d is the project; the
   library it builds is `libd3d9metal`.
6. On iPhone the OpenGL path goes through gl4es, with every shader rewritten
   to GLSL ES 3.00 ([docs/ios.md](docs/ios.md)); the Metal path is linked
   statically.

## What you need

- An Apple silicon Mac on macOS 13 or newer with the Xcode command line tools
  (`xcode-select --install`, which also brings Python 3.9). SDL2, FreeType,
  Fontconfig and the Metal renderer come prebuilt from this project's
  releases (about 4 MB, SHA-256 checked); with
  `brew install sdl2 freetype fontconfig`, Homebrew's are used instead.
- Portal 2 on Steam:
  - the game folder (the one with `portal2/`, `platform/` and `bin/`); the
    Windows version works;
  - the Linux binaries: open the Steam console (`open steam://open/console`)
    and type `download_depot 620 661` (about 90 MB).
- Optional: Rust 1.99 or newer from [rustup](https://rustup.rs) builds the
  Metal renderer from source instead of using the prebuilt one; the iPhone
  app needs it for Metal (Homebrew's `rust` has no iOS target).
- For iPhone: Xcode with your Apple ID (a free account works),
  `brew install xcodegen cmake ninja`, an iPhone on iOS 17 or newer
  connected by cable with Developer Mode on, and about 12 GB free on it.
  Tested on an iPhone 15 Pro Max only.

## Install on Mac

**Without Terminal**: download `Portal-2-Installer-<version>.zip` from the
[latest release](https://github.com/kanapitsas/portal2-recomp/releases/latest),
unzip it and open **Portal 2 Installer** (signed and notarized by Apple; if
macOS still refuses to open it, go to System Settings > Privacy & Security
and click **Open Anyway**). The installer checks the Xcode command
line tools (and offers to install them), finds the game and the Linux
binaries (with the Steam console commands to paste when they are missing),
then runs `setup.sh` for you. Its copy of the code and the build (about 5 GB)
live in `~/Library/Application Support/Portal 2 Natif Build`; its log in
`~/Library/Logs/Portal2Installer.log`.

**With Terminal**:

```sh
git clone https://github.com/kanapitsas/portal2-recomp.git
cd portal2-recomp
./setup.sh "/path/to/Portal 2" "/path/to/depot_661"
```

Without paths, `setup.sh` looks where Steam usually puts them. It downloads
Valve's `libsteam_api.dylib` (Steamworks.NET 20.1.0, SHA-256 checked),
builds the game (about 10 minutes the first time) and creates
`~/Applications/Portal 2 Natif.app`. Re-running it is safe and quick.
"Natif" is French for native; the name is kept so existing installs are
updated in place.

The app contains its own executable and libraries; the game files stay where
Steam put them. After a rebuild or a change to `config/local.json`, run
`python3 tools/make_app.py` to update the app (`--dest` for another folder).
Downloads and iOS libraries go to `deps/` (`P2_DEPS` overrides it).
`P2_HOST_DEPS=homebrew` or `bundle` forces where the host libraries come
from; `P2_APP_DIR` puts the app elsewhere than `~/Applications`.

## Install on iPhone

First create an Apple Development certificate (Xcode > Settings > Accounts >
Manage Certificates > + > Apple Development), then:

```sh
./setup.sh --ios "/path/to/Portal 2" "/path/to/depot_661"
```

It builds, signs and installs the app and copies the game data (about
11.5 GB, once; `tools/ios_device.sh data` copies it again after a reinstall).
On the phone, trust your account in Settings > General > VPN & Device
Management. With a free account, re-run `./setup.sh --ios` every 7 days.
`P2_TEAM_ID` sets the team when it cannot be read from the certificate;
`P2_DEVICE` picks the iPhone when several are connected.
More in [docs/ios.md](docs/ios.md).

## Play

On Mac, open Steam with the account that owns Portal 2, then open
**Portal 2 Natif** (it starts Steam if needed). The game language follows
your `-language` argument, `P2_LANGUAGE`, `config/local.json`, then Steam
and macOS ([details](docs/usage.md#game-language)).

On iPhone: left stick to move, right half of the screen to look, buttons to
jump, crouch, use and pause, and a tap on the back of the phone (left or
right) to fire a portal. Everything is tunable in Settings > Portal 2. The
iPhone version has no Steam, but it can join a Mac in LAN co-op.

**Controllers** work on both. On Mac, turn Steam Input off for Portal 2 in
Steam (Properties > Controller), otherwise Steam keeps the controller to
itself ([why](docs/usage.md#controllers)). On iPhone, the on-screen controls
hide themselves when a controller connects.

**Co-op** works over the local network: a Mac hosts, and an iPhone or a
second copy on the same Mac joins by IP address
([how](docs/usage.md#lan-co-op)). Two separate Macs, Steam invites and play
over the internet are not tested.

## Renderer: Metal or OpenGL

The game renders with **Metal** when `libd3d9metal` is available, and with
**OpenGL** otherwise. Metal measured about +50 % fps on a Mac and +20–25 % on
an iPhone, and fixes the too-dark image of OpenGL on iPhone.

- If Rust is installed, `setup.sh` fetches and builds `libd3d9metal` (about a
  minute) from a pinned commit of
  [mtld3d's native host branch](https://github.com/kanapitsas/mtld3d/tree/feat/native-host).
  Without Rust it uses the prebuilt library from the same commit (in the
  host libraries download); if neither works, the game uses OpenGL. If the build fails,
  a library built by an earlier run is kept as long as its file still
  exists; otherwise OpenGL. `P2_NO_METAL=1 ./setup.sh` skips it.
- The end of `setup.sh` tells which renderer the Mac app and the iPhone app
  use. The iPhone library is built separately and can still fall back to
  OpenGL.
- `P2_RENDERER=opengl` forces OpenGL; on iPhone, use Settings > Portal 2 >
  Rendering > Renderer.
- **F3** on Mac shows the renderer and the frame rate; on iPhone, the counter
  in the top-left corner does.

Details: [docs/d3d9-bridge.md](docs/d3d9-bridge.md).

## Uninstall

```sh
./uninstall.sh
```

It asks before moving the app and its log to the Trash, and asks again for
the saves. It never touches the Steam game files or the Linux binaries.

## Troubleshooting

- **"Steam is not running"**: the engine exits without a logged-in Steam
  client. Open Steam first (the app tries to start it).
- **"Failed to create D3D9!"**: `-vulkan` was passed but `libd3d9metal` is
  missing or failed to initialize. Drop `-vulkan` or set `P2_RENDERER=opengl`.
- **No music or voices**: `setup.sh` warns when `bin/linux32/mssmp3.asi`,
  `mssvoice.asi` or `binkawin.asi` is missing from the game folder (Miles
  decodes the MP3 voice lines with them). They come with the Linux game data
  (`download_depot 620 621`); a Windows copy may lack them.
- **iPhone app stops launching after a week**: free Apple accounts give
  7-day profiles and allow 3 sideloaded apps; re-run `./setup.sh --ios`.
- **Logs**: Mac, `~/Library/Logs/Portal2Natif.log`; iPhone,
  `Documents/game.log` in the app's files once the Journal setting is on
  (`tools/ios_device.sh run` pulls it back).
- Diagnostics variables: [docs/usage.md](docs/usage.md#diagnostics) (Mac) and
  [docs/ios.md](docs/ios.md#diagnostics) (iPhone).

## Roadmap and limits

Built and tested against one release of the Linux binaries (depot 661,
manifest 8940703274703023246); another release needs new indirect call
roots. iOS has no Steam features (achievements, invites, cloud saves); co-op
there is LAN only. A free Apple account allows 3 sideloaded apps and 7-day
profiles.

✅ Done · 🚧 In progress · 📅 Planned · 💡 Idea

| Area | Item | Status |
|---|---|:---:|
| Core | Static recompilation of the Linux x86 binaries into native ARM64 C, no emulator | ✅ |
| Mac | Single player fully playable, 235–280 fps on an M3 Pro (OpenGL) | ✅ |
| Mac | Steam bridge through Valve's official macOS `libsteam_api` (no Steam emulator) | ✅ |
| Rendering | Metal renderer through mtld3d (about +50 % fps on Mac), OpenGL fallback | ✅ |
| iPhone | Single player at a steady 60 fps, touch controls, Settings page | ✅ |
| iPhone | Portals fired by tapping the back of the phone | ✅ |
| iPhone | Pause in the background, haptic feedback, 30/40 fps caps with auto power saving, MetalFX upscaling | 🚧 |
| Install | One-command install (`setup.sh`) and a standalone Mac app | ✅ |
| Co-op | LAN co-op: a Mac hosts, an iPhone or a second copy on the same Mac joins by IP address | ✅ |
| Co-op | Find the host automatically on the local network (Bonjour), host/join in the iPhone settings | 🚧 |
| Co-op | Play a full co-op course with two people (only the connection and the map load are checked so far) | 📅 |
| Co-op | Co-op through Steam invites, over the internet and with Windows or Linux players | 📅 |
| iPhone | Record raw back taps from the Settings page | 🚧 |
| iPhone | Learn each player's back taps automatically from those recordings, instead of fixed thresholds | 📅 |
| Saves | Copy saves between Mac and iPhone over the cable ([how](docs/usage.md#saves-between-mac-and-iphone)); an iCloud version would need a paid Apple developer account | 🚧 |
| Controllers | Physical controller on Mac (needs Steam Input off for Portal 2, [why](docs/usage.md#controllers)) and iPhone, tested with an 8BitDo in Switch mode | ✅ |
| Controllers | A full playthrough with a controller, and other controllers (Xbox, DualSense) | 📅 |
| Controllers | Button prompts and default bindings that match the controller | 💡 |
| Community | Community maps copied by hand, loaded with `+map` | 📅 |
| Community | Steam Workshop on Mac (needs the UGC part of the Steam bridge) | 💡 |
| Rendering | Ray-traced lighting with Metal on M3 and A17 Pro GPUs, in the spirit of Portal with RTX (NVIDIA's RTX Remix is Windows and NVIDIA only, so it cannot be reused) | 💡 |

## Repository layout

- `src/`: `main.c`, the host entry point (`--images`, `--game-dir`,
  `--linux-root`, `--user-dir`, then the game's arguments).
- `runtime/`: the native runtime (libc, threads, SDL, GL, D3D9, Steam shims);
  `runtime/ios/` for the iPhone.
- `tools/`: ELF preparation, the lifter, build and run scripts, benchmarks.
- `tests/`: Python tests (`test_*.py`) and C checks (`checks_*.c`) linked
  into `abi-smoke`.
- `ios/`: app shell, Settings bundle, dependency build, signing project.
- `installer/`: the Mac installer app (SwiftUI, one file).
- `config/`: `game-roots.json` (indirect call roots); `local.json` is
  yours and ignored by git.
- `docs/`: the documents listed below. `experiments/`: the Steam probe.
- `build/` and `deps/`: generated, ignored by git.

## Development and tests

- `make check`: Python tests and `build/abi-smoke`, no game files needed.
- `make test INPUT_DIR=/path/to/depot_661/bin/linux32`: the full run; the
  path defaults to `linux_root` in `config/local.json`. `make sanitize` is
  the UBSan variant.
- `P2_STEAM_TESTS=1 make test`: live Steam tests, with the client running.
- Releases: `tools/build_host_deps.sh` builds the prebuilt host libraries
  (then update `HOST_DEPS_*` in `setup.sh`), `tools/build_installer.sh` the
  installer app and its zip; see [docs/usage.md](docs/usage.md#releases).
- CI (`.github/workflows/ci.yml`, macos-15): the Python tests, `abi-smoke`
  and `abi-smoke-ubsan`. It cannot build the full game, whose files are not
  in the repository.
- Manual build, run and diagnostics: [docs/usage.md](docs/usage.md).
  Project rules and bridge conventions: [CLAUDE.md](CLAUDE.md).

## Documentation

- [docs/usage.md](docs/usage.md): language, controllers, LAN co-op, manual build, diagnostics
- [docs/ios.md](docs/ios.md): iOS port, controls, settings, back taps
- [docs/d3d9-bridge.md](docs/d3d9-bridge.md): Metal renderer (Direct3D 9 bridge)
- [docs/steam.md](docs/steam.md): Steam bridge
- [docs/fast-path.md](docs/fast-path.md): fast memory mode and direct calls
- [docs/gl-audit.md](docs/gl-audit.md): OpenGL audit of togl (25 Oct. 2026)
- [docs/lifter-coverage.md](docs/lifter-coverage.md): lifter coverage
- [STATUS.md](STATUS.md): the development log, with every measurement

## Credits

- [mtld3d](https://github.com/athei/mtld3d) (zlib) by Alexander Theissen,
  the Direct3D 9 on Metal behind the Metal renderer; its Wine-free build is
  discussed in [athei/mtld3d#1193](https://github.com/athei/mtld3d/issues/1193).
- [gl4es](https://github.com/ptitSeb/gl4es) (MIT) by ptitSeb, patched for iOS.
- [SDL2](https://libsdl.org) (zlib), [FreeType](https://freetype.org) (FTL).
- [Steamworks.NET](https://github.com/rlabrecque/Steamworks.NET) (MIT), whose
  interface metadata generates the Steam bridge.
- Touch controls inspired by
  [MacNativeGames](https://github.com/ly05010419/MacNativeGames) (MIT).

Built in October 2026 by Raphaël Kanapitsas, with AI coding agents
(Anthropic's Claude Code and OpenAI's Codex) writing most of the code under
his direction. The history and [STATUS.md](STATUS.md) show the whole process.

## License

Copyright (C) 2026 Raphaël Kanapitsas and contributors.

[GNU GPL v3.0](LICENSE). It does not cover Portal 2 or any Valve material.
