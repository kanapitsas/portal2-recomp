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

## Performance

Build to play with `python3 tools/build_game.py --release` (`-O2`, without
memory checks): the default `--opt 1` checked build is twice as slow.
Measured on an M3 Pro (save `half-life-018`, 1280×720, hidden window,
simulated focus, idle machine, 3 alternating runs each):

| build | OpenGL | CPU time per frame (main thread) |
|---|---|---|
| `--opt 1` checked | 30.7 fps | 31 ms |
| `--release` | 77.0 fps | 12.6 ms |
| `--release --pgo-use` | 79.7 fps (+3.5 %) | 12.1 ms |

Metal (`-vulkan` path) is close: ~60–67 fps with `--release`. The guest gate
costs nothing here (`P2_GATE_TIMING=1`: 0.1 % of the main thread). With
`/usr/bin/sample`, the main thread spends ~66 % in translated game code,
spread thin (no function above 4 %), and ~20 % in Apple's OpenGL driver and
kernel, mostly command-buffer submissions forced by each render-target change
(`glClear`, `glBlitFramebuffer`).

PGO profile: an instrumented build
(`--release --pgo-generate --out build/portal2-native-pgogen`), runs with
`LLVM_PROFILE_FILE=…/%p.profraw` (the save on OpenGL and Metal,
`+map d1_trainstation_02`, `+map d3_c17_01`, ~1.2 GB each), then
`xcrun llvm-profdata merge -o build/game/pgo-hl2.profdata …` (155 MB, not in
git) and `--release --pgo-use build/game/pgo-hl2.profdata`.

### Ceiling (estimate, 7 Nov. 2026)

- **CPU-bound and single-threaded**: 640×480 runs at the same 80 fps as
  1280×720, and every thread but the main one sleeps (one guest CPU, guest
  gate).
- **Per frame** (hardware counters, `proc_pid_rusage`): 101 M ARM instructions,
  46 M cycles (IPC 2.2, 3.7 GHz). Of the 12.5 ms: ~8.2 ms translated code,
  ~0.9 ms runtime, ~2.7 ms OpenGL driver and kernel.
- **Code expansion**: 5.0 ARM instructions per x86 instruction (static, whole
  game), 6.0 weighted by the profile.
- **Where translated time goes** (samples mapped to generated lines):
  - ~38 % in the call protocol: calls, returns, entry and exit, tail jumps,
    polls;
  - ~29 % in memory accesses and push/pop;
  - ~11 % in flags;
  - ~7 % in SSE and x87.
- **Native-equivalent ceiling**: assuming about one native ARM instruction
  per x86 instruction and the same IPC, translated code would take about
  1.4–2 ms. With the same GL driver that is about 4.5–5 ms per frame, or
  200–230 fps. Native Source builds on Apple Silicon report 250–300 fps
  (often at the `fps_max 300` cap).
- **Realistic ceiling for this approach**: translated code 1.5–2× slower
  than native gives about 130–165 fps.
- **Beyond that**: overlapping the render thread (`mat_queue_mode 2`) would
  first need the guest gate removed, plus x86 memory ordering on ARM.
- **Typing guest accesses** so that clang keeps guest registers in host
  registers across guest stores is unsound, because 8/16/32-bit accesses to
  the same address must still alias. As an experiment it gained only 2–10 %
  on `tools/bench.py`. Inside functions with loops, clang reloads `c->…`
  after every guest store despite `CPU *restrict c`.

Rejected with measurements: dropping the flag save before every call (the
unsound version of the interprocedural flags analysis described in
STATUS.md) gains only +2.8 % here, so the sound version is not worth its
risk for now.

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
- Metal: the HDR sky (and the menu background behind the loading screen)
  shows garbage blocks. The six sky faces are DEFAULT-pool DYNAMIC
  1024×512 textures in `A16B16G16R16` (or `A16B16G16R16F` when 36 is
  refused), each filled twice through `IDirect3DSurface9::LockRect` (full
  rect, `D3DLOCK_NOSYSLOCK`). The data the game writes are valid and the
  bridge passes pitch and pointer through unchanged, so the upload goes wrong
  inside libd3d9metal. Refusing both formats in `CheckDeviceFormat` makes
  the game fall back to the LDR sky, which renders correctly.
