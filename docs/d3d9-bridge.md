# Direct3D 9 bridge (native renderer, experimental)

Portal 2's Linux release ships two renderers: `shaderapidx9.so` + `libtogl.so`
(Direct3D 9 translated to OpenGL, the default here) and `shaderapivk.so`,
selected with `-vulkan`, which expects DXVK (`libdxvk_d3d9.so`). Its only
graphics import is `Direct3DCreate9`. Instead of recompiling DXVK, the
runtime forwards the guest's Direct3D 9 COM calls to a **native arm64
Direct3D 9 implementation**, `libd3d9metal.dylib`, built separately (from
mtld3d). Without that library everything stays on OpenGL.

## Using it

```sh
python3 tools/gameprep.py                 # shaderapivk.so is now prepared (module bases shift)
python3 tools/build_game.py --release     # full rebuild after gameprep
P2_RENDERER=d3d9 P2_D3D9METAL=/path/to/libd3d9metal.dylib \
    SteamAppId=620 python3 tools/run_game.py --show --timeout 3600
```

- `P2_RENDERER=d3d9` (or `metal`) adds `-vulkan` to the game arguments, only
  if the library can be loaded; otherwise a message says the game stays on
  OpenGL. Passing `-vulkan` yourself forces the path (and `Direct3DCreate9`
  returns NULL if the library is missing: the game reports "Failed to create
  D3D9!").
- Library lookup: `P2_D3D9METAL`, then `<exe>/../Frameworks/libd3d9metal.dylib`
  (macOS app), `<exe>/Frameworks/` (iOS app), `<exe>/libd3d9metal.dylib`
  (e.g. `build/`). `tools/make_app.py` does not bundle it yet.
- From the app: `P2_RENDERER=d3d9 P2_D3D9METAL=… "~/Applications/Portal 2
  Natif.app/Contents/MacOS/launcher"`.
- `P2_D3D9_VENDOR_ID` / `P2_D3D9_DEVICE_ID` (hex): PCI ids handed to the
  library for `D3DADAPTER_IDENTIFIER9` (default Apple `106b` / `0000`; Source
  keys vendor-specific paths on them). `P2_D3D9_CACHE`: shader/PSO cache
  directory (default `<user_dir>/d3d9metal-cache`).
- Missing indirect roots in the new module trap with their address, like any
  other: `P2_RENDERER=d3d9 python3 tools/discover_game.py` adds them to
  `config/game-roots.json`.

## How it works

- `launcher.so -vulkan` creates its window with `SDL_WINDOW_VULKAN` and no GL
  context. `sdl_video.c` turns that into `SDL_WINDOW_METAL` and creates an
  `SDL_MetalView` on the main thread; its `CAMetalLayer` is handed to the
  library through `D3D9MetalHost.metal_layer`.
- `Direct3DCreate9` (shim in `runtime/d3d9.c`) loads the library, calls
  `d3d9metal_init` with the host services, then wraps the returned
  `IDirect3D9*`.
- Guest proxies are 16 bytes on the guest heap: `{guest vtable, tag, u64 host
  pointer}`. Each vtable slot is a dynamic import address
  (`"IDirect3DDevice9::SetRenderState"`); after the first call the import
  route goes straight to the thunk (no name lookup, no mutex). One proxy per
  host object, found through a hash table; it is freed when `Release` returns
  0. All bridge state is touched with the guest gate held.
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

1. Export `d3d9metal_init` and `d3d9metal_Direct3DCreate9` with C linkage;
   keep the `D3D9MetalHost` pointer (it stays valid).
2. Implement the full `IDirect3D9`/`IDirect3DDevice9`/resource vtables in
   `d3d9.h` order with the arm64 ABI and DXVK-native type sizes (DWORD 32
   bits, HWND 64 bits). Any slot may be called; unimplemented ones should
   return `E_NOTIMPL`, never crash.
3. Treat HWND as a host `SDL_Window*` and get the layer from `metal_layer`
   (it may be called from any thread; the view already exists). Set
   `layer.drawableSize` to the back buffer size itself: SDL may resize the
   drawable on window events.
4. Return from `Lock`/`LockRect`/`LockBox` only pointers into memory from
   `alloc_app` (16 KiB pages, zeroed; usable with
   `newBufferWithBytesNoCopy`). Guest address space for it is ~1.2 GiB
   (0x21000000-0x70000000, shared with the game's own mmaps): prefer staging
   allocated per lock, or for dynamic/managed resources only.
5. Wrap every potentially long wait (`nextDrawable`, fences, readback, vsync,
   `GetData` with `D3DGETDATA_FLUSH` spinning) in `blocking_begin` /
   `blocking_end`, and touch no guest memory in between.
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
   when event queries are missing.

## Open questions

- Not run end to end: no native library existed when this was written, and
  the game was not launched. `shaderapivk.so` lifted and compiled (see
  STATUS.md) but its runtime behaviour (other imports such as `SDL_GL_*`,
  extra indirect roots) is untested.
- Vendor id: Apple (`106b`) avoids NVIDIA/AMD/Intel-specific branches in
  Source; whether `dxsupport.cfg` then picks sensible defaults is unknown.
- Retina: the reported drawable size is the game's back buffer size
  (window points × window scale), the same rule as the OpenGL path.
- iOS: compiles (syntax checked against the iOS SDK) but the library would
  have to be embedded and signed in the app; not tried.
- Threading: Source may call Direct3D from its material system thread
  (`mat_queue_mode`); the bridge relies on the guest gate, so calls are
  serialized, but the library sees them from different host threads.
