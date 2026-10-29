# Direct3D 9 bridge (native renderer, default)

Portal 2's Linux release ships two renderers: `shaderapidx9.so` + `libtogl.so`
(Direct3D 9 translated to OpenGL) and `shaderapivk.so`,
selected with `-vulkan`, which expects DXVK (`libdxvk_d3d9.so`). Its only
graphics import is `Direct3DCreate9`. Instead of recompiling DXVK, the
runtime forwards the guest's Direct3D 9 COM calls to a **native arm64
Direct3D 9 implementation**, `libd3d9metal.dylib`, built separately (from
mtld3d). It is the default renderer whenever that library is found and
initializes; without it everything stays on OpenGL.

The library is [mtld3d](https://github.com/athei/mtld3d) (zlib licence) by
Alexander Theissen, built without Wine through a native host linkage that is
not upstream yet (see [athei/mtld3d#1193](https://github.com/athei/mtld3d/issues/1193)).
On macOS (laser_over_goo, 2560×1440, MSAA 4) it ran at 122 fps against 81 for
OpenGL, and the map loaded in 13 s instead of 25 s.

## Using it

`./setup.sh` does all of this when Rust is installed: it fetches a pinned
commit of the mtld3d native host branch into `deps/mtld3d`, builds
`libd3d9metal`, records it in `config/local.json` and bundles it into the app.
The steps below are the manual equivalent.

```sh
python3 tools/gameprep.py                 # shaderapivk.so is now prepared (module bases shift)
python3 tools/build_game.py --release     # full rebuild after gameprep
P2_D3D9METAL=/path/to/libd3d9metal.dylib \
    SteamAppId=620 python3 tools/run_game.py --show --timeout 3600
```

- By default the runtime loads and initializes the library
  (`d3d9metal_init`) before the game starts and adds `-vulkan` only if that
  succeeds; otherwise the game stays on OpenGL and the log says why.
  `P2_RENDERER=opengl` forces OpenGL; `P2_RENDERER=d3d9` (or `metal`) makes a
  missing library an explicit message. Passing
  `-vulkan` yourself forces the path (and `Direct3DCreate9` returns NULL if
  the library is missing or refuses to initialize: the game reports "Failed
  to create D3D9!").
- Library lookup (macOS; the iOS app links it statically): `P2_D3D9METAL`, then `<exe>/../Frameworks/libd3d9metal.dylib`
  (macOS app), `<exe>/Frameworks/` (iOS app), `<exe>/libd3d9metal.dylib`
  (e.g. `build/`). `tools/make_app.py` copies it into the app's
  `Contents/Frameworks` when `config/local.json` has
  `"d3d9metal_dylib": "/path/to/libd3d9metal.dylib"` (it links only system
  frameworks).
- From the app: the bundled library is used by default; `P2_RENDERER=opengl
  "~/Applications/Portal 2 Natif.app/Contents/MacOS/launcher"` forces OpenGL.
- `P2_D3D9_VENDOR_ID` / `P2_D3D9_DEVICE_ID` (hex): PCI ids handed to the
  library for `D3DADAPTER_IDENTIFIER9` (default Apple `106b` / `0000`; Source
  keys vendor-specific paths on them). `P2_D3D9_CACHE`: shader/PSO cache
  directory (default `<user_dir>/d3d9metal-cache`).
- Memory caps: before `d3d9metal_init` the runtime puts
  `memory.pageboxPoolCapMB=64;memory.vbibRetentionCapMB=192;memory.vramBudgetMB=1024`
  in front of `MTLD3D_CONFIG` (keys you set there come later and win).
  mtld3d's defaults (128 MiB recycle pool, 512 MiB VB/IB retention, no
  advertised-VRAM ceiling on a 64-bit build) target a 2-4 GiB 32-bit process;
  here VB/IB backings, texture staging mirrors and that pool all live in the
  ~1.2 GiB of guest address space the game also uses. The values are
  estimates, not measured in game.
- Display modes: the library enumerates the modes SDL reports to the game
  (desktop mode first, in game pixels, below the notch), read once at
  initialization. When the game has not started SDL video yet (the
  `P2_RENDERER` path), SDL video is initialized just long enough to read them.
- Missing indirect roots in the new module trap with their address, like any
  other: `P2_RENDERER=d3d9 python3 tools/discover_game.py` adds them to
  `config/game-roots.json`.

## How it works

- `launcher.so -vulkan` creates its window with `SDL_WINDOW_VULKAN` and no GL
  context. `sdl_video.c` turns that into `SDL_WINDOW_METAL` and creates an
  `SDL_MetalView` on the main thread; its `CAMetalLayer` is handed to the
  library through `D3D9MetalHost.metal_layer`. The library sets the layer's
  `drawableSize` itself (bounds × contentsScale) and resamples a back buffer
  of another size at present.
- `Direct3DCreate9` (shim in `runtime/d3d9.c`) loads the library, calls
  `d3d9metal_init` with the host services, then wraps the returned
  `IDirect3D9*`.
- Guest proxies are 16 bytes on the guest heap: `{guest vtable, tag, u64 host
  pointer}`. Each vtable slot is a dynamic import address
  (`"IDirect3DDevice9::SetRenderState"`); after the first call the import
  route goes straight to the thunk (no name lookup, no mutex). One proxy per
  host interface pointer, found through a hash table; it is freed when
  `Release` returns 0. COM identity: `QueryInterface(IID_IUnknown)` (and
  `GetContainer` with it) returns the proxy recorded for the host IUnknown
  pointer the first time, so it stays the same whichever interface is asked;
  another interface pointer of the same object obtained by `QueryInterface`
  shares its count and is retired with it. All bridge state is touched with
  the guest gate held.
- `tools/gen_d3d9.py` generates the thunks for all 17 interfaces (203
  methods, `d3d9.h` order) from an explicit signature table in the script.
  Every pointer parameter has a rule: in place (same layout on i386 and
  arm64), interface in/out, HWND, present parameters, lock results, shader
  tokens, vertex declarations. `P2_D3D9_HEADER=/path/to/d3d9.h python3 -m
  unittest discover -s tests -p test_gen_d3d9.py` checks names, order, arity
  and IIDs against an official header (verified against DXVK's native one).
- Translated structures (i386 → arm64): `D3DPRESENT_PARAMETERS` (56 → 64
  bytes, HWND at 0x1c → 0x20), `D3DADAPTER_IDENTIFIER9` (1100 → 1104),
  `D3DDEVICE_CREATION_PARAMETERS` (16 → 24), `D3DLOCKED_RECT` (8 → 16),
  `D3DLOCKED_BOX` (12 → 16). Offsets are `_Static_assert`ed in `d3d9.c`.
- Not bridged (logged once, `E_NOTIMPL` or 0): `RegisterSoftwareDevice`,
  `GetAdapterMonitor`, `DrawRectPatch`, `DrawTriPatch`, `GetDC`, `ReleaseDC`,
  `SetPrivateData` with `D3DSPD_IUNKNOWN`, `QueryInterface` for interfaces
  outside d3d9 (e.g. `IDirect3DDevice9Ex`). `pSharedHandle` is ignored.

## What the native library must do (beyond `runtime/d3d9/d3d9metal.h`)

1. Export `d3d9metal_init` and `d3d9metal_Direct3DCreate9` with C linkage.
   mtld3d copies the host table (only `cache_dir` and `modes` are borrowed;
   both stay valid) and refuses a second `d3d9metal_init`.
2. Implement the full `IDirect3D9`/`IDirect3DDevice9`/resource vtables in
   `d3d9.h` order with the arm64 ABI and DXVK-native type sizes (DWORD 32
   bits, HWND 64 bits). Any slot may be called; unimplemented ones should
   return `E_NOTIMPL`, never crash.
3. Treat HWND as a host `SDL_Window*` and get the layer from `metal_layer`
   (it may be called from any thread; the view already exists). mtld3d sets
   `drawableSize` from the layer's bounds and resamples the back buffer.
4. Return from `Lock`/`LockRect`/`LockBox` only pointers into memory from
   `alloc_app` (16 KiB pages, zeroed; usable with
   `newBufferWithBytesNoCopy`). Guest address space for it is ~1.2 GiB
   (0x21000000-0x70000000, shared with the game's own mmaps): prefer staging
   allocated per lock, or for dynamic/managed resources only. mtld3d's own
   threads read and write these blocks at any time, so each stays mapped at
   its address until `free_app`: `rt_mmap` never hands out a range that
   overlaps a live mapping, and the guest only learns these ranges through
   Lock pointers (it would have to `munmap` or `MAP_FIXED` over a pointer it
   was given, which d3d9 code does not do). `alloc_app` and `free_app` take
   the guest gate, so the library must not call them while holding a lock a
   guest thread can wait on outside `blocking_begin/end`.
5. Wrap every potentially long wait (`nextDrawable`, fences, readback, vsync,
   `GetData` with `D3DGETDATA_FLUSH` spinning) in `blocking_begin` /
   `blocking_end`, and touch no guest memory in between. mtld3d brackets
   nearly every call into its Metal runtime this way and never nests the
   pair; the runtime's implementation tolerates nesting.
6. `Release` returns the true remaining count (0 = destroyed). Texture
   levels, cube faces and volume levels must forward AddRef/Release to their
   container (d3d9 semantics): a level reaching 0 retires the container and
   all its levels, and the container reaching 0 retires its levels. Back
   buffers may keep their own count; they are retired with the device or
   swap chain that returned them. A host address reused by a new object
   of another type gets a new proxy. `QueryInterface` must answer for every
   d3d9 IID the object implements (the bridge uses it to type an object
   returned as `IUnknown`, e.g. a back buffer's swap chain container).
7. Fill `D3DADAPTER_IDENTIFIER9.VendorId/DeviceId` from `host->vendor_id` /
   `device_id`; honour `D3DPRESENT_PARAMETERS` write-back (e.g.
   `BackBufferCount` 0 → 1) as d3d9 does.
8. Answer `CheckDeviceFormat` honestly: `shaderapivk.so` contains the
   FOURCC codes `INTZ`, `DF24`, `DF16`, `RAWZ`, `NULL`, `ATI1`, `ATI2` and
   `ATOC` (found in its binary; which ones it actually requests is not
   checked), and it logs "D3DQUERYTYPE_EVENT not available on this driver"
   when event queries are missing. mtld3d (feat/native-host, d23f7dea) on an
   Apple M3 Pro, adapter format X8R8G8B8:

   | Format | Usage / type | Answer |
   |---|---|---|
   | INTZ, DF24, DF16 | depth-stencil texture | supported |
   | RAWZ | depth-stencil texture | not supported |
   | NULL | render-target surface | not supported (not implemented) |
   | ATI1, ATI2 | texture | not supported |
   | ATOC | surface | supported |
   | `D3DQUERYTYPE_EVENT` | `CreateQuery` | supported |

## Tests

- `make build/abi-smoke && ./build/abi-smoke`: the bridge against a fake
  library (`tests/checks_d3d9.c`), including COM identity across distinct
  interface pointers.
- With the real library:
  `P2_D3D9METAL=…/mtld3d/native/target/release/libd3d9metal.dylib ./build/abi-smoke`
  also runs `d3d9_bridge_against_real_native_library`: the guest thunks
  against mtld3d with no window (offscreen `CAMetalLayer`), the guest gate
  held like a game thread. It prints the table above, draws a vs_2_0/ps_2_0
  triangle, reads it back with `GetRenderTargetData` + `LockRect` from guest
  memory, presents and releases everything to 0. Skipped without the
  variable.

## Open questions

- The game was not launched with the library; only the headless check above
  ran against it. `shaderapivk.so` lifted and compiled (see
  STATUS.md) but its runtime behaviour (other imports such as `SDL_GL_*`,
  extra indirect roots) is untested.
- Vendor id: Apple (`106b`) avoids NVIDIA/AMD/Intel-specific branches in
  Source; whether `dxsupport.cfg` then picks sensible defaults is unknown.
- Retina: `metal_layer` reports the game's back buffer size (window points ×
  window scale), the same rule as the OpenGL path; mtld3d resamples to the
  layer's bounds × contentsScale if they differ.
- iOS: libd3d9metal is linked statically into the app
  (`tools/build_ios.py --d3d9metal`, `RT_D3D9METAL_STATIC`), selected in the
  Settings page; see "Metal renderer" in [ios.md](ios.md). Not run on a
  device yet.
- Threading: Source may call Direct3D from its material system thread
  (`mat_queue_mode`); the bridge relies on the guest gate, so calls are
  serialized, but the library sees them from different host threads.
