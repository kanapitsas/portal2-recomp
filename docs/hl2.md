# Half-Life 2 (experimental)

The same pipeline also recompiles **Half-Life 2** (20th anniversary build,
Steam app 220, mod `hl2_complete`). It is experimental: it installs and plays
on Mac, but only a few hours of play have been tested, the episodes and Lost
Coast have only been started, and the iPhone build has only run in the iOS
Simulator so far (see [iPhone](#iphone)).

**You must own Half-Life 2.** As for Portal 2, everything is generated from
your own copy and nothing from Valve is in this repository. The app and the
generated code contain Valve's game: keep them for yourself.

## Install

You need what Portal 2 needs (see the [README](../README.md#what-you-need)),
Half-Life 2 installed by Steam on your Mac, and its Linux binaries:

1. Install Half-Life 2 with Steam (the current version, which has
   `hl2_complete/`).
2. Open the Steam console (`open steam://open/console`) and type
   `download_depot 220 224` (about 320 MB). Wait for "Depot download
   complete".
3. Open the Natif Installer and tick Half-Life 2, or, in a checkout (the
   same one as Portal 2 if you have it):

   ```sh
   ./setup.sh               # every game found, or:
   ./setup.sh --game hl2
   ```

   Without paths, it finds the game in
   `~/Library/Application Support/Steam/steamapps/common/Half-Life 2` and the
   depot where `download_depot` put it; otherwise pass them:
   `./setup.sh --game hl2 "/path/to/Half-Life 2" "/path/to/depot_224"`.

`setup.sh` downloads Valve's `libsteam_api.dylib` from Steamworks.NET
2024.8.0 (SHA-256 checked; HL2 needs a newer SDK than Portal 2), builds the
game with `--release` in `build/hl2` (about 20 minutes and 4 GB the first
time; Portal 2's build in `build/game` is left alone) and creates
`~/Applications/Half-Life 2 Natif.app`. Open Steam with the account that
owns Half-Life 2, then open the app.

- Saves and settings: `~/Library/Application Support/Half-Life 2 Natif/userdata`.
- Log: `~/Library/Logs/HalfLife2Natif.log`.
- Renderer: **Metal** when `libd3d9metal` is there, as for Portal 2 (about
  120 fps, the display's limit, against 91 with OpenGL, see Performance). To force OpenGL:
  `open --env P2_RENDERER=opengl -a "Half-Life 2 Natif"`.
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

What `setup.sh --game hl2` adds to `config/local.json` (beside Portal 2's
keys, if any; [usage.md](usage.md#manual-build)):

```json
{
  "d3d9metal_dylib": "/path/to/libd3d9metal.dylib",
  "games": {
    "hl2": {
      "game_root": "~/Library/Application Support/Steam/steamapps/common/Half-Life 2",
      "linux_root": "/path/to/depot_224",
      "steam_api_dylib": "/path/to/steamworks-2024.8.0/libsteam_api.dylib",
      "steam_appid": 220,
      "depot_id": 224
    }
  }
}
```

Then:

```sh
python3 tools/gameprep.py --game hl2
python3 tools/build_game.py --game hl2 --release
python3 tools/make_app.py --game hl2                         # the app, or:
python3 tools/run_game.py --game hl2 --show --timeout 3600   # from the terminal
```

`run_game.py` uses Metal when `d3d9metal_dylib` is set (unlike the app);
`P2_RENDERER=opengl` forces OpenGL.

## Performance

Build to play with `python3 tools/build_game.py --release` (`-O2`, without
memory checks), as `setup.sh` does: the default `--opt 1` checked build is
2.5 times slower.
Measured on an M3 Pro (save `half-life-018`, 1280×720, hidden window,
simulated focus, idle machine, alternating runs):

| build (7 Nov. 2026) | OpenGL | Metal | CPU per frame (process) |
|---|---|---|---|
| `--opt 1` checked | 30.7 fps | | |
| `--release` | 77.0 fps | 60–67 fps | 47 M cycles (GL) |
| `--release --pgo-use` | 79.7 fps | ~83 fps | |
| + Nov. 6 audit (lifter, capture tracking) | ~91–93 fps | ~83 fps | 38 M (GL), 34 M (Metal) |
| + event queries reached at Present | ~91–93 fps | **~118 fps** | 35 M (Metal) |
| 8 Nov.: `--release` as `setup.sh` builds it | | **120 fps** (display limit) | 32 M (Metal) |
| 8 Nov.: same with `--pgo-use` | | 120 fps (display limit) | 30.5 M (Metal) |

On 8 November, Metal held the 120 Hz display's refresh rate with every
build, so the last column is what to compare: PGO saves about 5 % of the
CPU per frame (three alternating runs each, cycles from `proc_pid_rusage`).
`setup.sh` builds without a profile: a 155 MB profile per game is not worth
shipping for that. The NVIDIA vendor id that turns on flashlight shadow
depth textures ([d3d9-bridge.md](d3d9-bridge.md)) costs nothing measurable
on this save.

The audit's `-mllvm -capture-tracking-max-uses-to-explore` lets clang keep
guest registers in host registers in functions with loops (−9 %
instructions, IPC 2.1 → 2.4). On Metal, the game then slept 30 % of the
main thread in Source's frame sync: it waits for the event query of the
previous frame, which libd3d9metal only signals once the whole frame has
run, since it submits at Present. The bridge now reports an event issued
before a finished Present as reached ([d3d9-bridge.md](d3d9-bridge.md);
`P2_D3D9_STRICT_EVENTS=1` restores the library's answer). Metal is then
not CPU-bound at 640×480 either.

Rejected with measurements: Source's render thread (`mat_queue_mode 2`
with `P2_GUEST_CPUS=4`, 52 fps: the guest gate serializes both threads)
and Apple's multithreaded OpenGL engine (`kCGLCEMPEngine`, no gain).
OpenGL time is ~30 % of the frame: ~1,100 draw calls at ~1.8 µs, the
buffer swap and `glBlitFramebuffer` (`P2_GL_TIMING=1`).

PGO profile: an instrumented build
(`--game hl2 --release --pgo-generate --out build/hl2/hl2-native-pgogen`), runs with
`LLVM_PROFILE_FILE=…/%p.profraw` (the save on OpenGL and Metal,
`+map d1_trainstation_02`, `+map d3_c17_01`, ~1.2 GB each), then
`xcrun llvm-profdata merge -o build/hl2/pgo-hl2.profdata …` (155 MB, not in
git) and `--game hl2 --release --pgo-use build/hl2/pgo-hl2.profdata`.

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
- **The call share is mostly not protocol instructions.** In the release
  build, the protocol adds about 5 instructions per call: the poll, plus
  setting and checking `eip`. The flags save adds up to 13 when the flags are
  live, and the callee-saved register save and restore adds 10. Profiling at
  instruction level (`sample`, PCs disassembled with `llvm-mc`) puts 13 % of
  translated time on the first instruction of a function and 15 % on the host
  register save and restore. That looks like front-end stalls on a large hot
  code footprint (4.7 MB of functions called more than a million times during
  the PGO runs). Three experiments, none of which helped:
  - grouping those functions first with an order file built from the PGO
    profile (`--order-file`): 68.0 → 65.7 fps, within noise;
  - the `preserve_none` calling convention for translated functions: code
    grew by 6 % because callers now spill `c` around every call, and the
    bench did not change (×0.96–1.01);
  - dropping the poll and the `eip` store would save ~5 instructions per
    call, about 1–3 % at the estimated 300–650 k calls per frame; not done.
- **Typing guest accesses** so that clang keeps guest registers in host
  registers across guest stores is unsound, because 8/16/32-bit accesses to
  the same address must still alias. As an experiment it gained only 2–10 %
  on `tools/bench.py`. Inside functions with loops, clang reloads `c->…`
  after every guest store despite `CPU *restrict c`.

Rejected with measurements: dropping the flag save before every call (the
unsound version of the interprocedural flags analysis described in
STATUS.md) gains only +2.8 % here, so the sound version is not worth its
risk for now.

## iPhone

`./setup.sh --game hl2 --ios` builds the Mac app, then the iPhone app
(`build/hl2/ios/device/HalfLife2.app`, "Half-Life 2" on the home screen,
identifier `local.hl2recomp.<team>` in `games.hl2.ios_bundle_id` of
`config/local.json`, so it sits next to Portal 2), installs it and copies
about 10 GB of data (`bin platform hl2 episodic ep2 lostcoast hl2_complete`).
`./setup.sh --ios` does the same for every game it builds. By hand:
`python3 tools/build_ios.py --game hl2 [--target device]`, then
`tools/ios_device.sh --game hl2 install|data|run` on a phone or
`tools/run_ios_sim.sh --game hl2 [seconds]` in a headless simulator.
Saves live in the app's `Documents/hl2-userdata`; `tools/ios_device.sh saves`
is Portal 2 only for now. Details, the touch layout and what has been checked:
[ios.md](ios.md#half-life-2).

Touch controls in short: move stick (left), camera stick or trackpad (right),
fire (bottom right, drag it to aim), alt-fire, reload, jump, use, crouch and
sprint as toggles, weapon chevrons, flashlight, quick save and pause in the top
bar. Back taps are off (`P2_BACKTAP=1` maps them to fire and alt-fire). A
controller hides the overlay, as for Portal 2.

## Known limits

- Indirect call targets are still being discovered; a crash ending with
  `target not recompiled` means one more root. Please open an issue with the
  end of `~/Library/Logs/HalfLife2Natif.log` and where it happened.
  `tools/discover_game.py --game hl2` finds them from real runs (arguments after `--`
  go to the game, e.g. `+load <save>`).
- The game wants the `en_US.UTF-8` locale and warns about glyph support: the
  runtime only provides `C` and `C.UTF-8`.
- iPhone: built and played in the simulator only; untested on a device.
- Only the main game has been played (a few hours). Episode One, Episode Two
  and Lost Coast are in the same `hl2_complete` build and start from the
  main menu, which switches the game's search paths (`SwitchExpansion`), so
  `+map ep1_citadel_00` alone says "No such map". Their first maps
  (`ep1_citadel_00`, `ep2_outland_01`, `d2_lostcoast`) load and render on
  Metal; they have not been played through. To reach one without the menu:
  launch with `-gamepadui` and run `gamepadui_switchexpansion episodic`
  (or `ep2`, `lostcoast`), then `map <name>`.
