# Development log

## Current state (8 November 2026, v0.3.0)

- **Mac**: playable from `~/Applications/Portal 2 Natif.app`. Metal
  (Direct3D 9 bridge to mtld3d) is the default renderer when `libd3d9metal` is
  available (built by `setup.sh` with Rust, or prebuilt from the host-library
  download), OpenGL (togl) otherwise. Measurements: 235–280 fps with
  OpenGL (M3 Pro, 720p to 1512×982, 26 Oct.); Metal 122 vs 81 fps at
  2560×1440 MSAA 4 ([docs/d3d9-bridge.md](docs/d3d9-bridge.md)).
- **iPhone 15 Pro Max**: playable at a steady 60 fps, touch controls, portals
  fired by back taps, Settings page; Metal 71–77 vs 60 fps for OpenGL
  ([docs/ios.md](docs/ios.md)).
- **Co-op** over the LAN: Mac ↔ Mac and Mac ↔ iPhone, connection and map load
  checked ([docs/usage.md](docs/usage.md#lan-co-op)).
- **Half-Life 2** (experimental): built from the same checkout into
  `build/hl2`, Metal by default, 120 fps on Mac, 60 fps on an iPhone 15 Pro Max
  in a short test; measurements below ([docs/hl2.md](docs/hl2.md) for the rest).
- **Install**: the Natif Installer app (Developer ID signed, not notarized
  yet) or `./setup.sh` (and `--ios`), for one game or both.

Everything below is dated and kept as written; sections are newest first, and
statements that no longer hold are marked _Superseded_ with a date. The Mac
run and diagnostics instructions that used to live here are in
[docs/usage.md](docs/usage.md#run); the conventions for native bridges are in
[CLAUDE.md](CLAUDE.md).

## 7–8 November 2026 — Half-Life 2 performance

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
depth textures ([d3d9-bridge.md](docs/d3d9-bridge.md)) costs nothing measurable
on this save.

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

## 6 November 2026 — saves between Mac and iPhone (branch feat/save-sync)

- 2026-11-06: `tools/ios_device.sh saves status|pull|push` copies the `.sav` files between the Mac user data and the iPhone over the cable, newer side only, with backups (`tools/save_sync.py`); not tested on a device yet.

## 6 November 2026 — iPhone polish (branch feat/ios-polish, not yet tested on the phone)

- 2026-11-06: iPhone: game data kept out of iCloud backups, game paused while
  the app is inactive or in the background, touch overlay drawn with fixed
  layers, back-tap sensors stopped outside gameplay, haptic tap on each portal
  shot, 30/40 fps caps and auto power saving, MetalFX upscaling setting
  ([docs/ios.md](docs/ios.md)).

## 6 November 2026 — performance and correctness audit (branch perf/audit-nov06)

- Generated code is compiled with `-mllvm -capture-tracking-max-uses-to-explore=100000`
  (macOS and iOS builds; part of the object cache key). With the inlined calls
  and atomics below, `client_so.036.c` goes from 453,804 to 383,664 bytes of
  `__text` at `-O2 -DRT_FAST_MEMORY` (440,856 without the flag).
- `rt_call_fn`/`rt_call_any` push the return address through the caller's
  `mem_` instead of reloading `M`; guest atomics are inlined in `rt.h`, and
  their alignment traps, like the SSE ones, report the instruction address.
- `strlen` scans guest strings with `memchr` per 4 KiB page; `strcmp` stops at
  the first difference instead of measuring both strings first.
- Direct routes for `pthread_cond_*`, `pthread_get/setspecific` and
  `pthread_self`, and a cache for condition lookups.
- Steam warnings raised on a Steam thread are queued and delivered by the next
  `SteamAPI_RunCallbacks` (the guest hook used to run without the guest gate).
- Relative futex timeouts use the monotonic clock; `fullscreen_height` reads
  its cache on any thread; five shims span-check their output pointers;
  `getcwd(buf, 0)` gives EINVAL.
- Checked: `make check`, the UBSan abi-smoke, the shard compiled with
  `-DRT_FAST_MEMORY`, checked memory and `-DRT_TRACED_CALLS`. Not measured:
  in-game frame rate, the full game build, the iOS build. `lock adc`/`lock sbb`
  already took the carry-in for their flags (audit item dropped, test added).

## 30 October – 4 November 2026 — fixes, LAN co-op, roadmap (branches fix/audit-oct30, fix/lan-coop, merged)

- 2026-11-04: `setup.sh`'s final summary also says which renderer the iPhone app uses.
- 2026-11-03: README roadmap added, then turned into a table with one status per line.
- 2026-10-31: README and usage docs say which co-op cases work and which are untested.
- 2026-10-31: local network use declared in the iOS app so the iPhone can join LAN co-op (PR #12).
- 2026-10-31: the engine may read `bin/client.dll`, so LAN co-op clients stay connected.
- 2026-10-31: copyright line in the README.
- 2026-10-31: `P2_FAKE_FOCUS` gets the Metal window when there is no GL context (PR #11).
- 2026-10-31: the iPhone data copy is marked incomplete while it runs.
- 2026-10-31: a futex wake racing with the waiter's timeout is no longer lost.
- 2026-10-31: CPU texture conversions skipped on iOS when a pixel-unpack buffer is bound.
- 2026-10-31: guest heap table and D3D9 proxy table rehashed in place when deleted slots fill them.
- 2026-10-31: `strdup` checked in the iOS case-path cache.
- 2026-10-30: the game data is checked on the iPhone itself, not with a Mac-side marker.
- 2026-10-30: Metal made the default renderer (PR #8), one-command Metal setup in `setup.sh` (PR #10), iOS menu taps fixed (PR #9).

## 29 October 2026 — Native Direct3D 9 renderer bridge (branch feat/d3d9-bridge, merged)

Goal: run Valve's `shaderapivk.so` (`-vulkan`) on a native arm64 Direct3D 9
library (`libd3d9metal.dylib`, built separately) instead of togl/OpenGL.
Details, contract and open questions: [docs/d3d9-bridge.md](docs/d3d9-bridge.md).
Done: COM bridge for the 17 d3d9 interfaces (generated by
`tools/gen_d3d9.py`, checked against DXVK's native `d3d9.h`), struct
translation, guest-addressable `alloc_app` memory for Lock, gate release
around host waits, Metal windows for `-vulkan`, `shaderapivk.so` prepared by
`gameprep.py`, `P2_RENDERER=d3d9`. Verified: unit and abi-smoke checks
against a fake native library (`tests/checks_d3d9.c`). Not verified: any run
of the game on this path (no native library yet); OpenGL remains the default.
_Superseded (30 Oct. 2026): the game runs on this path on Mac and iPhone and
Metal is the default when the library is present; see
[docs/d3d9-bridge.md](docs/d3d9-bridge.md)._
`build_game.py --release` with shaderapivk.so (29 Oct.): 126,854 functions,
15.2 M instructions, none of the 15 uncompiled functions (rdrand/rdseed) in
shaderapivk.so, no new unimplemented import; 379 files in 528 s, 359 MB binary.

### Comparing the two renderers (method; results to fill)

_Superseded (5 Nov. 2026): the measurement was made, 122 vs 81 fps at
2560×1440 MSAA 4 on laser_over_goo, map load 13 s instead of 25 s; see
[docs/d3d9-bridge.md](docs/d3d9-bridge.md). The method below still describes
`tools/bench_renderer.py`._

`tools/bench_renderer.py` runs alternating rounds GL, D3D9, GL, D3D9, … of
one scenario and writes `build/bench/<date>/results.md` and `results.json`
(plus, per run, `game.log`, `frames.txt`, `console.log`, `meta.json`,
`rusage.json`). `--dry-run` prints every command, environment and file
operation without touching anything; `--analyze <dir>` recomputes the
tables from the logs.

```sh
# Steam client open and logged in, machine idle (check `uptime`), nothing else on screen
python3 tools/bench_renderer.py --dry-run     # read the plan
python3 tools/bench_renderer.py               # ~11 min with the defaults
```

- Scenario: `+map sp_a2_laser_over_goo` (GPU-heavy large room, as on
  26-27 Oct.), standing still at the spawn view (no input; whatever the map
  scripts at the start plays the same way on every launch). No `timedemo`: there is no
  recorded demo, and demo playback in Portal 2 was not tried here.
- Settings: `update/cfg/video.txt` rewritten before each launch:
  windowed 2560×1440 (Retina window, `-w 2560 -h 1440`), MSAA 4
  (`--msaa`), anisotropy 8 (`--aniso`), vsync and triple buffering off.
  The window is visible by default (`--hidden` for the older diagnostic
  mode: the Direct3D 9 path was only run visible so far). Sound goes to
  SDL's dummy driver, focus is simulated (`P2_FAKE_FOCUS=1`).
- User data: `../work/portal2-userdata-bench-gl` and `-bench-d3d9`, copied
  once from `../work/portal2-userdata-diag` (or the real user directory if
  there is no diag copy, read only). One copy per renderer because Source
  resets video.txt to its defaults when VendorID/DeviceID differ from the
  adapter's, and the two renderers report different ids. A run after which
  the game rewrote the ids is set aside (`*-reset`) and run again.
- Timing: `P2_FRAME_TIMES=<file>` (runtime/frame_times.c): one line per
  `SDL_GL_SwapWindow` (GL) or `IDirect3DDevice9::Present` /
  `IDirect3DSwapChain9::Present` (D3D9): start time (CLOCK_UPTIME_RAW),
  time spent inside the call, CPU time of the presenting thread. Off, it
  costs one branch per swap on GL and nothing on the D3D9 path (the timed
  handler is chosen when the Present route is resolved).
- End of load: `-condebug`, first `Redownloading all lightmaps` after
  `Host_NewGame` in `portal2/console.log` (fallback: end of the last
  ≥ 1 s gap between presents). The window starts `--warmup` (10) s after
  it and lasts `--duration` (30) s; the game is then killed (SIGKILL).
- Metrics: average fps; p50/p95/p99 frame time; 1 % low (1000 / mean of
  the slowest 1 % of frames); time inside present; hitches > 50/100/250 ms
  from 0.5 s after the load to the end of the window; presenting-thread CPU
  per frame; whole-process CPU per frame, cores in use, cycles and
  instructions per frame and memory footprint from `proc_pid_rusage`
  sampled every 0.25 s (includes mtld3d's encoder/submit threads);
  launch-to-load time.
- Cold vs warm: round 1 deletes `d3d9metal-cache/` (mtld3d's shader/PSO
  cache) and `update/glshaders.cfg` (togl's list of shaders to precompile)
  in the copies; later rounds are warm. The summary takes steady-state
  numbers from warm rounds and hitch counts from the cold round. macOS's
  own GL and Metal compiler caches are not cleared, so "cold" means cold
  application caches only. mtld3d compiles asynchronously
  (`shader.asyncCompile=true`): a missing pipeline skips draws instead of
  stalling, so its hitch count is not directly comparable with togl's;
  `--mtld3d-config shader.asyncCompile=false` measures the blocking case.
- GPU time (`--gpu-timing`, separate runs): `P2_GPU_TIMING=1` on GL
  (timer queries, slows the game) and `RUST_LOG=mtld3d::perf=info` on D3D9,
  which only reports `gpu_ms` with a `MTLD3D_PERF=1` build of
  libd3d9metal (`--d3d9-dylib`). Without it, the GPU column stays empty.
- Limits: ±15 % between launches was seen before with the same view, hence
  the alternation and several warm rounds; only clear differences count.
  The scenario is one static view, not gameplay. Results only mean
  something on an idle machine (the load average is recorded per run).

Results: _to fill (date, commit, machine load, results.md table)._
_Superseded (5 Nov. 2026): see [docs/d3d9-bridge.md](docs/d3d9-bridge.md)._

## 27–28 October 2026 — CPU work of the translated code (branch perf/code-traduit, merged)

Measuring method (the only reliable one found): hardware counters of the
process (`proc_pid_rusage`: instructions and cycles) over a 15 s window,
taken 30 s after the frame counter starts, alternating binaries over 3
rounds. Results vary by less than 1 % between rounds. Pitfalls:
- The first ~10 s after a map load do much more work (asset loading,
  Valve's `pathmatch`). A profile taken there blames the file system.
- Time Profiler counts the main thread as 100 % busy. Cycles per frame equal
  the frame time at a fixed 3.66 GHz, so in steady state the game is really
  CPU-bound (sp_a2_laser_over_goo, 720p). `P2_GPU_TIMING` slows the game
  (timer queries) and is not a valid way to settle this question.
- `sample` takes 3 samples in 5 s on this 300 MB binary, so it cannot be
  used. `P2_PROFILE` (SIGPROF) puts ~85 % on system calls.

Results (sp_a2_laser_over_goo 720p, idle machine): main ~44.7 M instructions,
~21.9 M cycles, ~165 fps; final numbers for the branch: see the end of
this section. Steps:
1. Native `pathmatch` (Linux case fixer in filesystem_stdio, replaced through
   `NATIVE_OVERRIDES` in lift.py; `P2_NATIVE_PATHMATCH=0` restores it). It is
   only active when every root is case-insensitive, and only costs during
   loads. GL mirror hash (`find_mirror` was 4.7 % of the main thread in steady
   state). Mutex cache, fast path and direct routes: 20.9 → 19.3 M cycles.
2. `always_inline` helpers in rt.h and a TLS cache: -2 % of instructions,
   cycles unchanged.
3. Dead x86 flags (`dead_flags` in lift.py; `P2_LIFT_KEEP_FLAGS=1` at
   generation disables it). This is the **safe** version: `c->flags` is
   always saved before a call, and only the restore after the call is
   dropped. About -1.5 % of cycles. A first version that no longer passed
   flags to callees reached -6 %. Codex showed it was unsound: a callee (or a
   tail-jump thunk, or an indirect call) that reads flags at entry would see
   stale flags, and restoring the save afterwards does not bring back
   writers that were already removed. To recover that gain soundly:
   - per-flag liveness, so that cmpxchg8b, which writes only ZF, is no longer
     a "read";
   - a summary per function (flags read at entry, plus callees and tail-jump
     targets reached without a kill);
   - a global fixed point;
   - a second lifting pass that keeps the save only before calls to the
     readers. Indirect calls are safe only if no address-taken function
     reads flags.

Final measurement (28 Oct., idle machine, 3 rounds): main 45.3 M
instructions, 22.8 M cycles, 160 fps; branch (safe version)
40.9 M instructions, 18.6 M cycles, 195 fps (+22 %). The app binary
(`build/portal2-native`) is still the main one. The branch's binary is built
by `python3 tools/build_game.py --release --pgo-use build/game/pgo.profdata`.
_Superseded (28 Oct. 2026): the branch is merged; the app binary is this build._

Remaining profile (steady state): ~60 % translated code, flat (no function
above 1 %); Apple driver plus kernel ~30 %, of which `iokit_user_client_trap`
is 11 %. That is command-buffer submissions, each `glBlitFramebuffer` forcing
one. Leads not yet tried:
- regenerate the PGO profile on the new code;
- direct routes for the libc imports that are still hot (`P2_IMPORT_COUNTS`);
- fewer blits.

The guest gate (scheduler.c) serialises all guest threads. Source's
multithreaded rendering (`mat_queue_mode`) would therefore gain nothing
without reworking it. Not measured on iPhone, where the CPU limits more than
on Mac.

The main thread mostly waits on Apple's OpenGL driver (Metal submissions on
glBufferSubData of in-flight buffers, on-the-fly GLSL compilation, cached by
togl in `update/glshaders.cfg`). The GPU has headroom: the frame rate varies
little with resolution.

How to run, the standalone app, Retina and the diagnostics variables that
were documented here: [docs/usage.md](docs/usage.md#run).

Decisive fixes of the evening: O(1) guest heap (the old one was
quadratic), switch tables no longer split into functions, ld.so-style symbol
scoping (libprotobuf in client.so/server.so), /proc generated from the host,
Windows binaries in the game copy made invisible (Miles was loading a PE
.asi), minimal GLX, filtered SDL3 events, Steam GameCoordinator.

## October 2026 — iPhone performance history (OpenGL)

sp_a2_laser_intro, iPhone 15 Pro Max, native 2796×1290 (moved from docs/ios.md):

| Step | fps |
|---|---|
| -O1 with checks | ~5 |
| gl4es | 7-15 |
| `--release` + optimizations from the Mac work | ~11 |
| unsynchronized `glBufferSubData` | ~19 |
| BGRA colors in the buffer, swizzled by the shader | ~47 |

Uncapped by the display (`tools/ios_bench.py`, `P2_IOS_NO_PRESENT=1`; these
are OpenGL numbers, `ios_bench.py --renderer d3d9` measures Metal, where the
display caps at 60 fps and `P2_IOS_NO_PRESENT` has no effect),
sp_a2_laser_stairs at the spawn point: native 2796×1290 90 fps (4× MSAA) or
94 (without); scale 2 (1864×860) 154; 1.5 178. The GPU is the bottleneck: scale 2 is the
default on device. Heavy view of the same room (setpos -188.94 -396.92
-31.97): ~117 fps sustained at scale 2 with 4× MSAA (~150 before the
phone heats up). For playing: 60 Hz by default, steady;
`P2_IOS_MAX_FPS=120` requests 120 Hz (frame rate then between 60 and 120). The ProMotion display rounds the displayed frame rate (120, 60,
40…): a frame taking more than 8.3 ms is shown at 60.

Lessons: the translated game code costs little; the cost is in Apple's ES
driver, which waits for the GPU on any write to an in-flight buffer and
copies every vertex as soon as one attribute comes from client memory.
Profile with `P2_PROFILE_RAW=1` before guessing.

## 26–27 October 2026 — Playable; performance on Mac (main)

**The game is playable** (tested by the user: menus, keyboard/mouse, sound,
voice, music, starting a game, returning to the menu, Cmd-Q). Performance
measured in-game (sp_a1_intro1, simulated focus, hidden window, M3 Pro):

| build | 1280×720 | "2560×1440" without Retina (actually 1512×982) | 2560×1440 Retina |
|---|---|---|---|
| `-O1` with memory checks (25 Oct., evening) | ~117 fps | — | — |
| `--release` (`-O2 --fast-memory`), 26 Oct. | 235-255 fps | ~263-280 fps | ~215-230 fps (menu) |

Caution: without Retina mode, macOS shrinks a window that is larger than the
screen (1512×982 points on this MacBook), so the earlier "2560×1440"
measurements were actually rendering 1512×982 pixels. Retina mode really
renders 2560×1440 (2.5 times as many pixels) in a 1280×720-point window.

Gains from the night of 25 to 26 October (measured with `tools/bench_game.py`,
averaging the second half of several runs): `--release` (×1.7), interactive QoS
(+15 %), file caches (access/fopen of missing files, parents of the root,
indexed libc streams), and above all unsynchronized `glBufferSubData` appends
into fresh storage (+30 %, reviewed three times by Codex; `P2_GL_STREAM_UNSYNC=0`
disables it). Attempts rejected, with measurements to back them up: reporting 2
or 4 CPUs (slower), releasing the guest lock during GL calls (slower), forcing
buffer usage, `gl_bufmode`.

MSAA (26 Oct., sp_a1_intro6, 2560×1440 Retina, machine loaded by the iOS
simulator): togl performs 6 MSAA resolves per frame (3 downscaled into the
1024² refraction texture, 2 full-size, 1 to the screen), each one after new
draws into the MSAA surface: none of them is redundant. MSAA 4x: 13.4 ms of
GPU time per frame; without MSAA: 7.0 ms (47 → 75 fps under load). In Retina,
lowering antialiasing in the Video menu is the main lever.
Diagnostic launches (without `--show`) now use a copy of the user data
(`../work/portal2-userdata-diag`).

Reliable CPU profile (26 Oct., `/usr/bin/sample` on the game in steady state;
the built-in SIGPROF profiler over-weights system calls on macOS): main
thread ≈ 50 % translated game code, 16 % runtime, 11 % GL/Metal driver,
8 % presentation (WindowServer requests, possibly caused by the hidden
diagnostic window). Today's gains, without MSAA at 2560×1440 (CPU-bound),
alternating A/B: direct routes for hot imports (`P2_IMPORT_COUNTS` to count
them) 120.9 → 125.6 fps; per-call-site target cache for indirect calls +
`rt_return` as a macro 117.4 → 124.4 fps (loaded machine). With MSAA 4x at
1440p the GPU is the bottleneck (~13 ms/frame): these gains make no difference
there. Next leads: ThinLTO/PGO, inline TLS access, measuring presentation with
a visible window.

Large rooms (26 Oct., evening, 2560×1440 Retina, hidden window):
sp_a2_laser_over_goo and sp_a2_column_blocker ≈ 95 fps on an idle machine
without MSAA (GPU ~9 ms/frame, so a GPU ceiling of ~110 fps at 1440p), ~50 fps
while another game and a build are running, ~38-40 fps with MSAA 4x on top:
that was the user's situation (30 fps). Measurements are only meaningful on an
idle machine (check `uptime`). Attempt rejected: grouping hot functions
together (`P2_PROFILE_WALL=1 P2_PROFILE_ORDER=…`, `tools/hot_order.py`,
`build_game.py --order-file`): no visible gain (78.7 → 78.9 fps).
PGO (26 Oct., night): profile collected on laser_over_goo, column_blocker and
intro3 (instrumented build, ~1.5 GB of raw profile per run), merged into
`build/game/pgo.profdata` (173 MB, not in git). A/B on an idle machine: 720p
156.8 → 167.3 fps (+7 %); 1440p only ≈ +2 %, because the GPU is saturated
there (9.9 ms of GPU time out of 10 ms per frame, ~100 fps in the large rooms).
The app binary is the PGO build; the old one remains at
`build/portal2-native-nopgo`. Rebuild with the profile:
`python3 tools/build_game.py --release --pgo-use build/game/pgo.profdata`
(without the option, you get the non-PGO build back). Regenerate the profile:
`--pgo-generate --out build/portal2-native-pgogen`, runs with
`LLVM_PROFILE_FILE=…-%p.profraw`, `xcrun llvm-profdata merge`.
Next ceiling at 1440p: the GPU (120 fps will require a lighter render).

GPU cost at 1440p (27 Oct., laser_over_goo, alternating A/B, idle machine,
`bench_game.py --extra "+convar value"`): 8x anisotropic filtering
(Video menu, "Filtering Mode") is the most expensive. Baseline ~100-110 fps;
`mat_forceaniso 4` 132-149; 2: 120-158; 1: 123-124; 1080p rendering
132-134. No effect: `mat_reducefillrate 1`, `gl_nullqueries 1`. Measurements
vary by ±15 % from one launch to the next (same starting view): only draw
conclusions from clear differences. Traced frame (`P2_PASS_TRACE=<s>`):
~280 draws, main render at 2560×1440 interrupted by 3 copies (1024²
refraction, two full-size copies), post-processing at 640×360, final copy
flipped to the window; these copies are required by the game.
Comparing two binaries: `bench_game.py --binary build/<copy>` (the copy must
stay in `build/`, since the libsteam_api path is relative to the executable).

Fullscreen: according to the user, the mouse aims ~0.5 cm too high; the safe
area below the notch is 32 points on this MacBook. The log now records the
actual geometry (`[sdl] géométrie …`) at creation, on resize and on fullscreen
changes: a fullscreen launch will provide the numbers.
The SIGBUS in fn_17eabcc0 (3 times on 26 Oct.) happens when the game shuts
down (CPortal_Player, probably an orphaned GC inventory listener), not in game.

## 25 October 2026 — Miles PE plugins (MP3 voices) (branch feat/pe-plugins, merged)

libMiles loads the Windows DLLs `bin/linux32/*.asi` itself (mssmp3 decodes
the MP3 voice lines). They are translated ahead of time (`tools/peprep.py`,
`lift.generate_pe`, called by `build_game.py` → `build/game/gen/pe`) into
relocatable C indexed by RVA; `runtime/pe.c` recognizes the image on the first
control transfer into it (.text bytes identical to the file, relocations
included: data pointing into Miles' writable view, values pointing into .text
into the executable view), then dispatches. Only the translated .asi files
that are identical to the file on disk remain visible
(`rt_path_windows_binary`). Diagnostics: `P2_PE_TRACE=1` (plugin functions
called). Verified: no more "Can't create mixer", decoding of
`vo/announcer/openingexercise01.wav` identical to afconvert (correlation
0.9999997). With a hidden window, the map does not trigger the opening line
and the output stays silent: needs to be listened to for real.

## 25 October 2026 — Checkpoint (main)

Work paused at the user's request after the thread bridge was validated.
**The game does not launch yet: no menu and no game window has been
validated.** _Superseded (26 Oct. 2026): playable, see above._

### Last real startup

The ARM64 Mach-O calls the real `portal2_linux main`, initializes the modules
`libmimalloc`, `libtier0`, `libvstdlib`, `libsteam_api`, `libtogl`, `launcher` and
`filesystem_stdio`, then runs the engine's first threads. The last concrete
stop is the import **`SDL_SetHint`**, called by `launcher.so`.
The traces are in `build/game/launch.log` and `build/game/discovery.log`.

The counts of generated functions and instructions measure compilation, not
execution coverage. Methods adjacent to vtables are compiled as static roots
even before they first execute.

### Batch completed before the pause

- `stpcpy` copy, stdio files, 32/64-bit offsets and EOF/error detection.
- Explicit translation of the Linux i386 `stat`/`stat64` structures.
- Access to the original assets and to the Linux depot; configuration writes
  go under `../work/portal2-userdata`, without modifying the original copy.
- Darwin threads with CPU state, protected guest stack, and separate TCB, TLS
  and errno. Creation, join, stack size attribute and scheduling parameters.
- Mutexes/condition variables, timed waits, one-time initialization and TLS
  destructors.
- Serialization of guest instructions onto one logical processor. Calls and
  backward branches periodically yield; blocking waits release the execution
  lock. This choice favors consistency on ARM.
- Linux futex WAIT/WAKE, including the PRIVATE variant, with real waiting,
  bounded wakeups, value comparison and timeout.

`make test sanitize` passes: codec oracle (513 sizes), ABI/CPU/memory,
SSE comparisons/conversions, files, then three native threads. The thread tests
cover a concurrent AOT loop, condition wakeup, join, errno/TCB isolation,
MXCSR inheritance, pthread_once, TLS destructors and futex WAIT/WAKE.
The game binary is rebuilt separately with `--opt 0` for diagnostics.

A Claude review was requested but **could not take place**: the organization
currently forbids subscription access to Claude Code. This is not a
successful review; the available checks are the local tests and UBSan.

### Next step, once resumption is authorized

_Superseded (26 Oct. 2026): the SDL2, OpenGL and Steam bridges exist._

Implement a typed bridge to native ARM64 SDL2. The library and headers are
already present: `/opt/homebrew/lib/libSDL2.dylib` and
`/opt/homebrew/include/SDL2`. No new Steam download is needed for this step.
Never truncate 64-bit Darwin pointers into 32-bit guest addresses: use a
handle registry and translate SDL structures, events and callbacks. OpenGL
rendering and Steam are to be handled afterwards. The `steamclient.dylib`
shipped with Steam also has an ARM64 slice, but no native Steam bridge is
implemented yet.

Commands to resume, from this directory:

```sh
make test
python3 tools/discover_game.py --steps 20 --timeout 30
```

`discover_game.py` stops on any missing import/instruction; it treats neither
a normal exit nor a timeout as proof that the game launched.
The local, unversioned configuration is in `config/local.json`; the discovered
indirect roots are in `config/game-roots.json`.

### Limitations to keep explicit

Runtime teardown refuses workers that are still active, to avoid freeing their
memory while they run. Coordinated worker shutdown, detached threads,
condition attributes, other futex operations, some Linux calls and module
finalization remain incomplete. x87 is modeled in double precision. Directory
reads do not yet merge the three file sources. No workaround for missing
imports may be used to declare a successful launch.

The conventions for native bridges added on 25 October now live in
[CLAUDE.md](CLAUDE.md).
