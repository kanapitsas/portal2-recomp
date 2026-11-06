# Half-Life 2 (experimental)

The same pipeline also recompiles **Half-Life 2** (20th anniversary build,
Steam app 220, mod `hl2_complete`). It is work in progress: a save loads and
plays on Mac, but `setup.sh` does not install it yet and only a few hours of
play have been tested.

**You must own Half-Life 2.** As for Portal 2, everything is generated from
your own copy and nothing from Valve is in this repository.

## What differs from Portal 2

- **Game profile** (`tools/games.py`): program `hl2_linux`, game libraries
  under `bin/` and `hl2_complete/bin/`, roots in `config/hl2-roots.json`.
  `config/local.json` picks it with `"game": "hl2"`.
- **Linux binaries**: depot 224 (all i386). Download it from the Steam
  console (`steam://open/console`):
  `download_depot 220 224`, then point `linux_root` at the folder Steam
  prints.
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

## Example config/local.json

```json
{
  "game": "hl2",
  "game_root": "~/Library/Application Support/Steam/steamapps/common/Half-Life 2",
  "linux_root": "/path/to/depot_224",
  "steam_api_dylib": "/path/to/steamworks-2024.8.0/libsteam_api.dylib",
  "steam_appid": 220,
  "d3d9metal_dylib": "/path/to/libd3d9metal.dylib"
}
```

Then:

```sh
python3 tools/gameprep.py
python3 tools/build_game.py
python3 tools/run_game.py --show --timeout 3600
```

## Known limits

- Indirect call targets are still being discovered; a crash ending with
  `target not recompiled` means one more root. `tools/discover_game.py`
  finds them from real runs (arguments after `--` go to the game, e.g.
  `+load <save>`).
- The game wants the `en_US.UTF-8` locale and warns about glyph support: the
  runtime only provides `C` and `C.UTF-8`.
- Saves and settings share Portal 2's data folder
  (`~/Library/Application Support/Portal 2 Natif/userdata`).
- No app bundle and no iPhone build yet.
