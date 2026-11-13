# Development log

## Current state (8 November 2026, v0.3.0)

- **Mac**: playable from `~/Applications/Portal 2 Natif.app`. Metal
  (Direct3D 9 bridge to mtld3d) is the default renderer when `libd3d9metal` is
  available (built by `setup.sh` with Rust, or prebuilt from the host-library
  download), OpenGL (togl) otherwise. Measurements below.
- **iPhone 15 Pro Max**: playable at a steady 60 fps, touch controls, portals
  fired by back taps, Settings page ([docs/ios.md](docs/ios.md)).
- **Co-op** over the LAN: Mac ↔ Mac and Mac ↔ iPhone, connection and map load
  checked ([docs/usage.md](docs/usage.md#lan-co-op-portal-2)).
- **Half-Life 2** (experimental): built from the same checkout into
  `build/hl2`, Metal by default, 120 fps on Mac, 60 fps on an iPhone 15 Pro Max
  in a short test ([docs/hl2.md](docs/hl2.md)).
- **Install**: the Natif Installer app or `./setup.sh` (and `--ios`), for
  one game or both ([README](README.md#install-on-mac)).

The rest of this file is the measurements, the performance analysis and a
dated history, newest first. Run and diagnostics instructions are in
[docs/usage.md](docs/usage.md#run); the conventions for native bridges are in
[CLAUDE.md](CLAUDE.md).

## Measurements

All on an M3 Pro (Mac) or an iPhone 15 Pro Max, idle machine, hidden window
and simulated focus unless said otherwise. Frame rates vary by ±15 % between
launches of the same view: only clear differences count. `P2_GPU_TIMING=1`
(GL timer queries) slows the game and cannot settle whether a scene is
GPU-bound; hardware counters from `proc_pid_rusage` can
(`tools/bench_game.py`, `tools/bench_renderer.py`).

**Portal 2, Mac, OpenGL** (sp_a1_intro1):

| build | 1280×720 | "2560×1440" without Retina (actually 1512×982) | 2560×1440 Retina |
|---|---|---|---|
| `-O1` with memory checks (25 Oct.) | ~117 fps | — | — |
| `--release` (`-O2 --fast-memory`), 26 Oct. | 235-255 fps | ~263-280 fps | ~215-230 fps (menu) |

Without Retina mode macOS shrinks a window larger than the screen (1512×982
points on this MacBook); Retina mode really renders 2560×1440 in a
1280×720-point window. Large rooms (sp_a2_laser_over_goo, sp_a2_column_blocker,
1440p Retina): ≈ 95 fps without MSAA (GPU ~9 ms/frame), ~38-40 fps with MSAA 4×
on a loaded machine. PGO (26 Oct.): 720p 156.8 → 167.3 fps (+7 %), 1440p
≈ +2 % (GPU-saturated).

**Portal 2, Mac, Metal vs OpenGL** (5 Nov., laser_over_goo, 2560×1440,
MSAA 4, `tools/bench_renderer.py`): **122 vs 81 fps**, map load 13 s instead of
25 s ([docs/d3d9-bridge.md](docs/d3d9-bridge.md)).

**MSAA and anisotropy** (26–27 Oct., 1440p Retina): togl performs 6 MSAA
resolves per frame (3 into the 1024² refraction texture, 2 full-size, 1 to
the screen), none redundant; MSAA 4× costs 13.4 ms of GPU time per frame
against 7.0 without (47 → 75 fps under load). 8× anisotropic filtering
(Video menu, "Filtering Mode") is the next most expensive: baseline ~100-110
fps, `mat_forceaniso 4` 132-149, 2: 120-158, 1: 123-124; 1080p rendering
132-134. No effect: `mat_reducefillrate 1`, `gl_nullqueries 1`. A traced
frame (`P2_PASS_TRACE=<s>`) has ~280 draws, 3 copies required by the game and
post-processing at 640×360.

**Portal 2, iPhone, OpenGL history** (sp_a2_laser_intro, native 2796×1290):

| Step | fps |
|---|---|
| -O1 with checks | ~5 |
| gl4es | 7-15 |
| `--release` + optimizations from the Mac work | ~11 |
| unsynchronized `glBufferSubData` | ~19 |
| BGRA colors in the buffer, swizzled by the shader | ~47 |

Uncapped (`tools/ios_bench.py`, `P2_IOS_NO_PRESENT=1`, OpenGL only),
sp_a2_laser_stairs at the spawn: native 90 fps (4× MSAA) or 94; scale 2
(1864×860) 154; 1.5 178; a heavy view of the same room ~117 fps sustained at
scale 2 with 4× MSAA (~150 before the phone heats up). The GPU is the
bottleneck, so scale 2 is the default. The cost is in Apple's ES driver (waits
on any write to an in-flight buffer, copies every vertex when one attribute
comes from client memory), not in the translated code: profile with
`P2_PROFILE_RAW=1` before guessing. **Metal vs OpenGL**: **71–77 vs 60 fps**
(same room, MSAA 4, scale 2, no vsync; `ios_bench.py --renderer d3d9`, where
the display caps at 60 and `P2_IOS_NO_PRESENT` has no effect). For playing:
60 Hz by default; `P2_IOS_MAX_FPS=120` lands between 60 and 120 since
ProMotion shows a frame over 8.3 ms at 60.

**Half-Life 2, Mac** (save `half-life-018`, 1280×720, alternating runs):

| build (7 Nov. 2026) | OpenGL | Metal | CPU per frame (process) |
|---|---|---|---|
| `--opt 1` checked | 30.7 fps | | |
| `--release` | 77.0 fps | 60–67 fps | 47 M cycles (GL) |
| `--release --pgo-use` | 79.7 fps | ~83 fps | |
| + Nov. 6 audit (lifter, capture tracking) | ~91–93 fps | ~83 fps | 38 M (GL), 34 M (Metal) |
| + event queries reached at Present | ~91–93 fps | **~118 fps** | 35 M (Metal) |
| 8 Nov.: `--release` as `setup.sh` builds it | | **120 fps** (display limit) | 32 M (Metal) |
| 8 Nov.: same with `--pgo-use` | | 120 fps (display limit) | 30.5 M (Metal) |

The default `--opt 1` checked build is 2.5 times slower than `--release`.
On 8 November Metal held the 120 Hz display with every build, so the last
column is what to compare: PGO saves about 5 % of the CPU per frame (three
alternating runs each). `setup.sh` builds without a profile: a 155 MB profile
per game is not worth shipping for that. The NVIDIA vendor id that turns on
flashlight shadow depth textures costs nothing measurable on this save.

PGO profile: an instrumented build (`--release --pgo-generate --out
build/<game>/…-pgogen`), runs with `LLVM_PROFILE_FILE=…/%p.profraw` (~1.2–1.5
GB each), `xcrun llvm-profdata merge -o build/<game>/pgo.profdata …` (not in
git), then `build_game.py --release --pgo-use <profdata>`.

## Performance analysis

### Half-Life 2 (7–8 November 2026)

The audit's `-mllvm -capture-tracking-max-uses-to-explore` lets clang keep
guest registers in host registers in functions with loops (−9 %
instructions, IPC 2.1 → 2.4). On Metal, the game then slept 30 % of the
main thread in Source's frame sync: it waits for the event query of the
previous frame, which libd3d9metal only signals once the whole frame has
run, since it submits at Present. The bridge now reports an event issued
before a finished Present as reached ([d3d9-bridge.md](docs/d3d9-bridge.md);
`P2_D3D9_STRICT_EVENTS=1` restores the library's answer). Metal is then
not CPU-bound at 640×480 either.

Rejected with measurements: Source's render thread (`mat_queue_mode 2`
with `P2_GUEST_CPUS=4`, 52 fps: the guest gate serializes both threads)
and Apple's multithreaded OpenGL engine (`kCGLCEMPEngine`, no gain).
OpenGL time is ~30 % of the frame: ~1,100 draw calls at ~1.8 µs, the
buffer swap and `glBlitFramebuffer` (`P2_GL_TIMING=1`).

**Ceiling (estimate, 7 Nov. 2026, OpenGL, before the audit)**

- **CPU-bound and single-threaded**: 640×480 runs at the same 80 fps as
  1280×720, and every thread but the main one sleeps (one guest CPU, guest
  gate).
- **Per frame** (hardware counters): 101 M ARM instructions, 46 M cycles
  (IPC 2.2, 3.7 GHz). Of the 12.5 ms: ~8.2 ms translated code, ~0.9 ms
  runtime, ~2.7 ms OpenGL driver and kernel.
- **Code expansion**: 5.0 ARM instructions per x86 instruction (static, whole
  game), 6.0 weighted by the profile.
- **Where translated time goes** (samples mapped to generated lines): ~38 %
  call protocol (calls, returns, entry and exit, tail jumps, polls); ~29 %
  memory accesses and push/pop; ~11 % flags; ~7 % SSE and x87.
- **Native-equivalent ceiling**: about one native ARM instruction per x86
  instruction at the same IPC would be 1.4–2 ms of translated code, so
  4.5–5 ms per frame with the same GL driver, or 200–230 fps. Native Source
  builds on Apple Silicon report 250–300 fps (often at the `fps_max 300` cap).
- **Realistic ceiling for this approach**: translated code 1.5–2× slower
  than native gives about 130–165 fps.
- **Beyond that**: overlapping the render thread (`mat_queue_mode 2`) would
  first need the guest gate removed, plus x86 memory ordering on ARM.
- **The call share is mostly not protocol instructions.** The protocol adds
  about 5 instructions per call (poll, `eip` set and check); the flags save
  up to 13 when flags are live; callee-saved save/restore 10. Instruction-level
  profiling (`sample`, PCs disassembled with `llvm-mc`) puts 13 % of translated
  time on the first instruction of a function and 15 % on the host register
  save and restore: front-end stalls on a large hot footprint (4.7 MB of
  functions called more than a million times during the PGO runs). Three
  experiments, none of which helped: an order file from the PGO profile
  (`--order-file`, 68.0 → 65.7 fps, noise); the `preserve_none` calling
  convention (code +6 %, callers spill `c` around every call, ×0.96–1.01);
  dropping the poll and the `eip` store (~5 instructions per call, 1–3 % at
  300–650 k calls per frame; not done).
- **Typing guest accesses** so that clang keeps guest registers in host
  registers across guest stores is unsound (8/16/32-bit accesses to the same
  address must alias) and gained only 2–10 % on `tools/bench.py`. Inside
  functions with loops, clang reloads `c->…` after every guest store despite
  `CPU *restrict c`.
- Dropping the flag save before every call (the unsound version of the
  interprocedural flags analysis below) gains only +2.8 % here, so the sound
  version is not worth its risk for now.

### Portal 2 CPU work (27–28 October 2026)

Measuring method (the only reliable one found): hardware counters of the
process (`proc_pid_rusage`: instructions and cycles) over a 15 s window,
taken 30 s after the frame counter starts, alternating binaries over 3
rounds; results vary by less than 1 % between rounds. Pitfalls: the first
~10 s after a map load do much more work (asset loading, Valve's
`pathmatch`); Time Profiler counts the main thread as 100 % busy, but cycles
per frame equal the frame time at a fixed 3.66 GHz, so in steady state the
game really is CPU-bound (laser_over_goo, 720p); `sample` takes 3 samples in
5 s on this 300 MB binary; `P2_PROFILE` (SIGPROF) puts ~85 % on system calls.

Steps, from main ~45.3 M instructions, 22.8 M cycles, 160 fps to 40.9 M,
18.6 M cycles, 195 fps (+22 %):

1. Native `pathmatch` (Linux case fixer in filesystem_stdio, replaced through
   `NATIVE_OVERRIDES` in lift.py; `P2_NATIVE_PATHMATCH=0` restores it), only
   active when every root is case-insensitive. GL mirror hash (`find_mirror`
   was 4.7 % of the main thread). Mutex cache, fast path and direct routes:
   20.9 → 19.3 M cycles.
2. `always_inline` helpers in rt.h and a TLS cache: −2 % instructions.
3. Dead x86 flags (`dead_flags` in lift.py; `P2_LIFT_KEEP_FLAGS=1` at
   generation disables it), **safe** version: `c->flags` is always saved
   before a call, only the restore after it is dropped (−1.5 % cycles). A
   version that no longer passed flags to callees reached −6 % but Codex
   showed it unsound: a callee, tail-jump thunk or indirect call that reads
   flags at entry would see stale flags. It now exists as the **ABI flags
   mode** (default for the whole-game lift, `generate_sharded`): liveness is
   per flag, and under the i386 SysV assumption a call neither reads nor
   returns flags (no save before it, no reload after it) and `ret` saves
   nothing; tail jumps keep the save. The assumption is proved per game at
   generation time (`lift.flags_check`, same pass): no call is followed by a
   flag read, and every function reading a flag at entry is either a
   pushfd/lahf-only reader (CPUID probes, emitted conservatively) or only a
   code-pointer root that no direct call or jump reaches (switch cases
   compiled as functions). Otherwise the whole game is lifted again in the
   conservative mode above. `P2_LIFT_ABI_FLAGS=0` at generation forces the
   conservative mode; `build/<game>/gen/manifest.json` records `flags_mode`
   and `flags_check`. PE plugins and smoke fixtures stay conservative.

Remaining profile (steady state): ~60 % translated code, flat (no function
above 1 %); Apple driver plus kernel ~30 %, of which `iokit_user_client_trap`
11 % (command-buffer submissions, one per `glBlitFramebuffer`). The main
thread mostly waits on Apple's OpenGL driver (Metal submissions on
`glBufferSubData` of in-flight buffers, on-the-fly GLSL compilation cached
by togl in `update/glshaders.cfg`). The guest gate (scheduler.c) serialises
all guest threads, so `mat_queue_mode` gains nothing without reworking it.
Leads not tried: regenerate the PGO profile, direct routes for the libc
imports still hot (`P2_IMPORT_COUNTS`), fewer blits.

Earlier gains (25–27 Oct., `tools/bench_game.py`): `--release` ×1.7,
interactive QoS +15 %, file caches, unsynchronized `glBufferSubData` appends
into fresh storage +30 % (`P2_GL_STREAM_UNSYNC=0` disables it), direct routes
for hot imports +4 %, per-call-site target cache for indirect calls +6 %.
Rejected: reporting 2 or 4 CPUs, releasing the guest lock during GL calls,
forcing buffer usage, `gl_bufmode`, an order file of hot functions
(`tools/hot_order.py`, no gain).

## History

- **8 Nov. 2026**: audit and cleanup: merged worktrees removed, `runtime/`,
  `tests/` and `tools/` factored, documentation pruned; HL2 iPhone first run.
- **7–8 Nov.**: HL2 performance work above (event queries reached at Present,
  PGO measured and not shipped); HL2 Brightness slider forwarded to the window
  gamma ramp ([docs/hl2.md](docs/hl2.md#dark-interiors)).
- **6 Nov.** (feat/save-sync): `tools/ios_device.sh saves status|pull|push`
  copies `.sav` files between Mac and iPhone over the cable, newer side only,
  with backups (`tools/save_sync.py`;
  [docs/usage.md](docs/usage.md#saves-between-mac-and-iphone-portal-2)).
- **6 Nov.** (feat/ios-polish): game data kept out of iCloud backups, pause
  while inactive or in the background, touch overlay as fixed layers, back-tap
  sensors stopped outside gameplay, haptic tap per portal shot, 30/40 fps caps
  and auto power saving, MetalFX upscaling ([docs/ios.md](docs/ios.md)).
- **6 Nov.** (perf/audit-nov06): generated code compiled with
  `-mllvm -capture-tracking-max-uses-to-explore=100000` (in the object cache
  key; `client_so.036.c` `__text` 453,804 → 383,664 bytes); return address
  pushed through the caller's `mem_` in `rt_call_fn`/`rt_call_any`; guest
  atomics inlined in `rt.h`; `strlen`/`strcmp` fast paths; direct routes for
  `pthread_cond_*`, `pthread_*specific`, `pthread_self`; Steam warnings from a
  Steam thread queued until the next `SteamAPI_RunCallbacks`; futex timeouts
  on the monotonic clock; `getcwd(buf, 0)` gives EINVAL.
- **30 Oct.–4 Nov.** (fix/audit-oct30, fix/lan-coop): Metal made the default
  renderer (PR #8), one-command Metal setup (PR #10), iOS menu taps (PR #9),
  `P2_FAKE_FOCUS` on the Metal window (PR #11); LAN co-op from the iPhone
  (local network use declared, PR #12; the engine may read `bin/client.dll`
  so clients stay connected); futex wake racing a timeout no longer lost;
  guest heap and D3D9 proxy tables rehashed in place; iPhone data copy marked
  incomplete while it runs; README roadmap table.
- **29 Oct.** (feat/d3d9-bridge): native Direct3D 9 bridge to libd3d9metal,
  `P2_RENDERER=d3d9`; design, contract and results in
  [docs/d3d9-bridge.md](docs/d3d9-bridge.md). Build with shaderapivk.so:
  126,854 functions, 15.2 M instructions, 379 files in 528 s, 359 MB binary.
- **27–28 Oct.** (perf/code-traduit): CPU work above (+22 %); PGO build is
  the app binary.
- **26–27 Oct.**: **playable** (menus, keyboard/mouse, sound, voice, music,
  Cmd-Q); measurements above. Decisive fixes: O(1) guest heap (was
  quadratic), switch tables no longer split into functions, ld.so-style symbol
  scoping (libprotobuf in client.so/server.so), /proc generated from the host,
  Windows binaries hidden from the game, minimal GLX, filtered SDL3 events,
  Steam GameCoordinator. Fullscreen: the mouse aims ~0.5 cm too high
  according to the user (notch safe area 32 points); the log records the
  window geometry (`[sdl] géométrie …`). A SIGBUS in `fn_17eabcc0` (3 times)
  happens at shutdown (CPortal_Player, probably an orphaned GC inventory
  listener), not in game.
- **25 Oct.** (feat/pe-plugins): Miles' Windows plugins (`bin/linux32/*.asi`,
  MP3 voices) translated ahead of time (`tools/peprep.py`) and dispatched by
  `runtime/pe.c`; `P2_PE_TRACE=1` lists the plugin functions called. MP3
  decoding matches afconvert (correlation 0.9999997).
- **25 Oct.**: thread bridge validated (Darwin threads, futexes, guest
  instructions serialized on one logical processor); the game did not launch
  yet (stopped at `SDL_SetHint` in `launcher.so`).
