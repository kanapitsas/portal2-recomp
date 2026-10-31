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

Tested between two copies on one Mac, and with an iPhone joining a Mac. Co-op
through Steam invites is not tested yet.

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
python3 tools/make_app.py                 # optional: an app for the Dock / Finder (re-run after a rebuild)
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
[docs/ios.md](ios.md). The simulator works too
(`ios/deps/build.sh sim`, `python3 tools/build_ios.py`, then
`tools/run_ios_sim.sh`), but renders in software and shows clipping
artifacts that do not exist on the device.

