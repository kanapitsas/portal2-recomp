# Status — 26 October 2026, night (resumed by Claude)

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

The main thread mostly waits on Apple's OpenGL driver (Metal submissions on
glBufferSubData of in-flight buffers, on-the-fly GLSL compilation, cached by
togl in `update/glshaders.cfg`). The GPU has headroom: the frame rate varies
little with resolution.

Run (from this directory, with the Steam client open):

```sh
python3 tools/build_game.py --release  # to play: -O2 without memory checks (~10 min from scratch)
python3 tools/build_game.py            # diagnostics: -O1, checked memory (~20 min from scratch)
SteamAppId=620 python3 tools/run_game.py --show --timeout 3600
```

To play without a terminal: `python3 tools/make_app.py` creates
`~/Applications/Portal 2 Natif.app` (a link to `build/portal2-native`, paths
from `config/local.json`, starts Steam if needed, no `-w/-h`: resolution and
mode come from the Video menu, saved in `update/cfg/video.txt`).
Log: `~/Library/Logs/Portal2Natif.log`. Only needs redoing if the paths or
arguments change; otherwise a rebuild is enough.

Retina (on by default, `P2_RETINA=0` restores the old behavior): the screen
is reported in pixels (3024×1964), so the Video menu also offers the large
resolutions from the game's table (up to 2560×1600). A resolution that fits
on screen in points gives a normal window (×1 rendering); a larger one gives a
Retina window half its size in points (2560×1440 → 1280×720-point window,
native rendering). `wantsBestResolutionOpenGLSurface` is toggled on the fly.
Fullscreen not verified in Retina.
F3 shows/hides a host frame counter (`P2_FPS=1`: visible from the start).

Diagnostics: `P2_GPU_TIMING=1` (GPU time per frame, GL_TIME_ELAPSED
queries), `P2_BLIT_TRACE=<n>` (300 glBlitFramebuffer copies starting from the
n-th, with the draws made into the source surface since the previous copy),
`P2_FRAME_LOG=1` (frame pacing), `P2_FRAME_DUMP=/chemin.ppm`
(back-buffer capture), `P2_WATCH_CALL=0x<adresse>` (arguments of a guest
call), host faults translated into guest address + guest stack.
Without `--show`, the window is hidden and vsync is disabled.

Decisive fixes of the evening: O(1) guest heap (the old one was
quadratic), switch tables no longer split into functions, ld.so-style symbol
scoping (libprotobuf in client.so/server.so), /proc generated from the host,
Windows binaries in the game copy made invisible (Miles was loading a PE
.asi), minimal GLX, filtered SDL3 events, Steam GameCoordinator.

## Miles PE plugins (MP3 voices) — branch feat/pe-plugins

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

---

# Checkpoint — 25 October 2026

Work paused at the user's request after the thread bridge was validated.
**The game does not launch yet: no menu and no game window has been
validated.**

## Last real startup

The ARM64 Mach-O calls the real `portal2_linux main`, initializes the modules
`libmimalloc`, `libtier0`, `libvstdlib`, `libsteam_api`, `libtogl`, `launcher` and
`filesystem_stdio`, then runs the engine's first threads. The last concrete
stop is the import **`SDL_SetHint`**, called by `launcher.so`.
The traces are in `build/game/launch.log` and `build/game/discovery.log`.

The counts of generated functions and instructions measure compilation, not
execution coverage. Methods adjacent to vtables are compiled as static roots
even before they first execute.

## Batch completed before the pause

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

## Next step, once resumption is authorized

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

## Limitations to keep explicit

Runtime teardown refuses workers that are still active, to avoid freeing their
memory while they run. Coordinated worker shutdown, detached threads,
condition attributes, other futex operations, some Linux calls and module
finalization remain incomplete. x87 is modeled in double precision. Directory
reads do not yet merge the three file sources. No workaround for missing
imports may be used to declare a successful launch.

## Conventions for native bridges (added on 25 October, Claude takeover)

- **New imports**: one `runtime/<domaine>.c` file per domain. It declares its
  names on `// SHIMS: nom1 nom2 ...` lines (read by `tools/lift.py`) and
  registers itself with `RT_SHIM_PROVIDER(fonction)`; the function returns 0 for
  an unknown name. Do not extend the `_CORE_SHIMS` list in `lift.py` or the
  `rt_shim` chain in `shims.c` any further.
- **Dynamic addresses** (`SDL_GL_GetProcAddress`, `alGetProcAddress`, `dlsym`
  into a host library): `rt_dynamic_import(nom)` returns a stable guest
  address (range `0xf8000000`); a guest call to it ends up in
  `rt_shim(c, nom)` just like a static import.
- **Host pointers**: `rt_handle_new/get/find/free` (unmapped range
  `0xe0000000`, typed by an identifier). A structure that guest code reads
  directly (SDL_Surface, FT_Face, SDL_Event...) must be copied into guest
  memory in i386 layout.
- **Floating-point return**: `rt_return_st0(c, valeur)`. **Guest callback**
  from the host: `rt_invoke` (`modules.h`).
- **Guest callback from a foreign host thread** (SDL audio thread, CoreAudio):
  `CPU *c = rt_foreign_thread_attach()` (idempotent: its own protected stack,
  TCB, TLS, errno), then `rt_guest_enter(); rt_invoke(c, …); rt_guest_leave();`.
  Detach explicitly or automatically on thread exit. Any host call that may
  wait on this thread (SDL device lock, join) must be made with the guest lock
  released (`rt_guest_suspend/resume`), otherwise it deadlocks.
- **Tests**: `tests/checks_<domaine>.c` with `RT_CHECK(nom) { ... }`, linked
  automatically into `abi-smoke` (and its UBSan variant).
- **Host libraries**: `HOST_CFLAGS`/`HOST_LIBS` in the Makefile, mirrored in
  `tools/build_game.py` (SDL2, FreeType, Fontconfig, iconv, OpenGL, OpenAL).
- Caution: `make clean` also wipes `build/game/prep`; regenerate it with
  `python3 tools/gameprep.py` (a few seconds).
