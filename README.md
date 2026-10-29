# Portal 2, statically recompiled for Apple silicon (macOS and iPhone)

[![Portal 2 running natively on iPhone (video)](https://img.youtube.com/vi/YWX2BeMnlQI/maxresdefault.jpg)](https://youtu.be/YWX2BeMnlQI)

*Video: Portal 2 running natively on an iPhone 15 Pro Max.*

<p>
  <img src="docs/images/iphone-menu.jpg" alt="Portal 2 main menu on iPhone, 60 fps" width="49%">
  <img src="docs/images/iphone-ingame.jpg" alt="Portal 2 in game on iPhone with touch controls" width="49%">
</p>

This project translates the **x86 (i386) machine code of Portal 2's Linux
release into C**, then compiles it into a native ARM64 executable for macOS
and iOS. No emulator, no Wine, no Rosetta, no interpreter: every function of
the game's 31 Linux modules (engine, game DLL, renderer, materials, sound,
VGUI, ...) becomes a C function, and a small runtime supplies what the Linux
binaries expect from the outside world (glibc, pthreads, SDL2, OpenGL, Steam).

- **macOS**: fully playable, 235-280 fps on an M3 Pro (1280×720 to 1512×982).
- **iPhone**: fully playable on an iPhone 15 Pro Max, a locked 60 fps with
  4× MSAA, touch controls, and portals fired by **tapping the back of the
  phone** (left or right side, detected with the accelerometer and gyroscope).

**You must own the game.** This repository contains no game files, no Valve
code and no decompiled source. Everything is generated on your machine from
your own copy of Portal 2, and the result is for your own use only: **do not
share the built app or the generated code**, they contain Valve's game.

> Unofficial fan project, not affiliated with or endorsed by Valve. Portal,
> Portal 2, Steam and Source are trademarks of Valve Corporation. Provided as
> is, without warranty (see [LICENSE](LICENSE)).

## How it works

1. **Preparation** (`tools/gameprep.py`, `tools/elfprep.py`): the i386 ELF
   modules of the Linux release are loaded at fixed guest addresses and
   relocated, as the Linux dynamic loader would.
2. **Lifting** (`tools/lift.py`, `tools/float_lift.py`): every known function
   (symbols, exports, init arrays, code pointers found in data, vtables) is
   disassembled and translated instruction by instruction into C operating on
   a guest CPU state and a 4 GiB guest address space. Branches become C
   branches, calls become C calls, x87 and SSE become host floating point. The
   full game is about **125,000 functions and 15 million instructions**, split
   in ~320 C files compiled in parallel and cached. An instruction or import
   that is not supported is never silently skipped: it becomes a trap that
   stops the game with its address.
3. **Runtime** (`runtime/`): guest memory, threads (serialized on a guest
   lock with yield points), TLS, futexes, and bridges for the Linux imports
   the game uses: libc, pthreads, SDL2, OpenGL (through Valve's own
   `togl` D3D9-to-GL layer), OpenAL, Fontconfig/FreeType, and Steam (the
   official Steamworks `libsteam_api.dylib` on macOS, a minimal offline stub
   on iOS).
4. **iOS** (`runtime/ios/`, `ios/`): OpenGL 2.1 is turned into OpenGL ES 3 by
   a patched [gl4es](https://github.com/ptitSeb/gl4es), with shader rewrites
   for sRGB, texture borders and vertex color swizzling; a UIKit overlay
   provides the touch controls; an iOS Settings page exposes the options.

More details: [docs/ios.md](docs/ios.md) (iOS port, controls, settings),
[docs/steam.md](docs/steam.md) (Steam bridge),
[docs/fast-path.md](docs/fast-path.md) (fast memory mode),
[docs/gl-audit.md](docs/gl-audit.md),
[docs/lifter-coverage.md](docs/lifter-coverage.md), and
[STATUS.md](STATUS.md), the development log with every measurement.

The code comments and commit messages are in French; the documentation is in
English.

## What you need

- An Apple silicon Mac, the Xcode command line tools (`xcode-select
  --install`), Python 3 and [Homebrew](https://brew.sh):
  `brew install sdl2 freetype fontconfig`.
- **Portal 2 on Steam**, in two parts:
  - the **game data**: a Portal 2 install folder, the one containing
    `portal2/`, `platform/` and `bin/`. The Windows release works (installed
    through Whisky or CrossOver, for example);
  - the **Linux binaries** (depot 661, about 90 MB). Open the Steam console
    (`open steam://open/console` in a terminal) and type
    `download_depot 620 661`.
- The Steam client running and signed in when you play on the Mac.
- For the iPhone: Xcode with an Apple ID added in Settings > Accounts (a free
  account works, with 7-day profiles), `brew install xcodegen cmake ninja`,
  the iPhone connected by cable with Developer Mode on, and ~12 GB free on it.
  Tested on an iPhone 15 Pro Max (A17 Pro, 8 GB); other models are untested.

## Quick start

```sh
git clone https://github.com/kanapitsas/portal2-recomp.git
cd portal2-recomp
./setup.sh "/path/to/Portal 2" "/path/to/depot_661"          # macOS
./setup.sh --ios "/path/to/Portal 2" "/path/to/depot_661"    # macOS + iPhone
```

`setup.sh` checks the tools, downloads Valve's macOS `libsteam_api.dylib`
(from the [Steamworks.NET 20.1.0](https://github.com/rlabrecque/Steamworks.NET/releases/tag/20.1.0)
package, checksum verified), writes `config/local.json`, recompiles the game
(about 10 minutes the first time) and creates `~/Applications/Portal 2
Natif.app`. Without paths, it looks for the folders `download_depot` writes
under `~/Library/Application Support/Steam` (not tested yet: pass the paths
if it does not find them). Re-running it is safe and quick.

With `--ios` it also reads your team from your Apple Development
certificate, picks an app identifier unique to that team, gets a provisioning
profile through Xcode, builds the iPhone app, installs it and copies the game
data to the phone (about 11.5 GB, only the first time). On the phone, trust
your developer account in Settings > General > VPN & Device Management.
Re-run `./setup.sh --ios` every 7 days with a free account.

The app carries its own copy of the prepared Linux modules (about 75 MB, in
`Contents/Resources/prep`); saves and settings go to `~/Library/Application
Support/Portal 2 Natif/userdata`. The game files (about 12 GB) and the Linux
depot stay where Steam put them, and the game folder is never written to. The
app still runs the executable from this folder's `build/`, so keep the folder.

### Game language

The game starts in the language chosen by `tools/language.py`, which adds
`-language <name>` to the launch. First match wins:

1. a `-language <name>` you pass yourself (for example
   `python3 tools/make_app.py -- -game portal2 -novid -language french`);
2. the environment variable `P2_LANGUAGE`
   (`P2_LANGUAGE=german "~/Applications/Portal 2 Natif.app/Contents/MacOS/launcher"`);
3. `"language": "french"` in `config/local.json` (no rebuild needed, the app
   reads it at each launch);
4. the language set in the Steam properties of Portal 2, read from
   `steamapps/appmanifest_620.acf` next to the game folder;
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

## Manual build

`config/local.json` (ignored by git; `setup.sh` writes it):

```json
{
  "game_root": "/path/to/Portal 2",
  "linux_root": "/path/to/depot_661",
  "steam_appid": 620,
  "steam_api_dylib": "/path/to/steam_api.bundle/Contents/MacOS/libsteam_api.dylib",
  "user_dir": "/path/to/saves",
  "ios_bundle_id": "com.yourname.portal2"
}
```

`ios_bundle_id` must be unique to your Apple account (free accounts cannot
share identifiers); it can also be given with `P2_BUNDLE_ID`. Without
`user_dir`, the app keeps saves in `~/Library/Application Support/Portal 2
Natif/userdata`; `tools/make_app.py` copies the older default
`../work/portal2-userdata` there once if it exists (the original stays), and
`tools/run_game.py` still uses that older default.

macOS:

```sh
python3 tools/gameprep.py                 # prepare the Linux modules (seconds)
python3 tools/build_game.py --release     # lift and compile (~10 min the first time)
python3 tools/run_game.py --show --timeout 3600
python3 tools/make_app.py                 # optional: an app for the Dock / Finder
```

iPhone:

```sh
python3 tools/gameprep.py                         # if not done for macOS
ios/deps/build.sh device                          # SDL2, FreeType, gl4es (once)
python3 tools/build_ios.py --target device --release
tools/ios_device.sh install                       # iPhone connected by cable, unlocked, Developer Mode on
tools/ios_device.sh data                          # copy the game data to the app (~11.5 GB, once)
```

The first device build needs a provisioning profile: see
[docs/ios.md](docs/ios.md). The simulator works too
(`ios/deps/build.sh sim`, `python3 tools/build_ios.py`, then
`tools/run_ios_sim.sh`), but renders in software and shows clipping
artifacts that do not exist on the device.

## Playing on iPhone

**Controls**: left stick to move, right half of the screen to look (stick
or trackpad), buttons for jump, crouch, use and pause; tap the back of the phone on
the left or right to fire the blue or orange portal. Everything can be
tuned in the iOS Settings app, under Portal 2.

## Uninstall

```sh
./uninstall.sh
```

It lists what is installed, then asks twice (default: no): first to move the
app and its log (`~/Library/Logs/Portal2Natif.log`) to the Trash, then
separately for the saves and settings (`~/Library/Application Support/Portal 2
Natif`, and `user_dir` if it points elsewhere). It never touches the Steam game
files or the Linux depot, which may be your real Steam install: it prints their
paths from `config/local.json` and what to do with them. Delete this folder
afterwards if you no longer need it.

## Tests

```sh
make test INPUT_DIR=/path/to/depot_661/bin/linux32   # runtime and ABI checks against the real binaries
make sanitize INPUT_DIR=/path/to/depot_661/bin/linux32   # the same with UBSan
```

`INPUT_DIR` is the folder holding `libtier0.so` and `libvstdlib.so` (in a Steam depot, `bin/linux32`); it defaults to `../work/portal2-linux/bin`.

## Limits

- iOS is single player and offline: no co-op, achievements or Steam features.
- Built and tested against one specific release of the Linux binaries
  (depot 661, manifest 8940703274703023246); another build needs new indirect
  call roots (`config/game-roots.json`, `tools/discover_game.py`).
- x87 is computed in `double` precision, not 80-bit.
- The free Apple account limits a device to 3 sideloaded apps and 7-day
  profiles; rebuild and reinstall to renew.

## How it was made

Built in a few days of October 2026 by Raphaël Kanapitsas, with AI coding
agents (Anthropic's Claude Code and OpenAI's Codex) writing most of the code
under his direction; the history and [STATUS.md](STATUS.md) show the whole
process.

## Credits

- [gl4es](https://github.com/ptitSeb/gl4es) (MIT) by ptitSeb, patched in
  `ios/deps/gl4es-ios.patch`.
- [SDL2](https://libsdl.org) (zlib), [FreeType](https://freetype.org) (FTL).
- [Steamworks.NET](https://github.com/rlabrecque/Steamworks.NET) (MIT), whose
  interface metadata generates the Steam bridge.
- Touch controls inspired by
  [MacNativeGames](https://github.com/ly05010419/MacNativeGames) (MIT).

## License

The code in this repository is licensed under the GNU General Public License
v3.0 ([LICENSE](LICENSE)). It does not cover Portal 2 or any Valve material,
which remain the property of Valve Corporation.
