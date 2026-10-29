// Contract between the portal2-recomp runtime and a native arm64 Direct3D 9
// implementation (libd3d9metal.dylib, built separately).
//
// The game's renderer shaderapivk.so (Linux i386, translated) talks to
// Direct3D 9 through COM interfaces. runtime/d3d9.c turns every guest call
// into a call on the host objects returned by d3d9metal_Direct3DCreate9:
// standard d3d9.h interfaces, native arm64 ABI (first word of the object is
// a vtable of function pointers in d3d9.h order, `this` is the first
// argument). DWORD, UINT, INT, LONG, ULONG, BOOL, HRESULT, D3DCOLOR and the
// enums are 32 bits; HWND, HANDLE, HMONITOR and pointers are 64 bits, so
// D3DPRESENT_PARAMETERS (64 bytes, HWND at 0x20), D3DLOCKED_RECT (16, pBits
// at 8), D3DLOCKED_BOX (16, pBits at 8), D3DDEVICE_CREATION_PARAMETERS (24)
// and D3DADAPTER_IDENTIFIER9 (1104, LARGE_INTEGER aligned on 8) use the
// arm64 layout of DXVK's native headers. Every other d3d9 structure has the
// same layout as on i386 and may be read or written in place in guest memory.
//
// Rules the implementation must follow (see docs/d3d9-bridge.md; this file
// is an additive copy of mtld3d's native/include/d3d9metal.h):
// - HWND values are SDL_Window* of the host SDL; call metal_layer to get the
//   CAMetalLayer to present into. The library sets its drawableSize itself
//   (bounds x contentsScale) and resamples a back buffer of another size.
// - Every pointer returned by Lock/LockRect/LockBox must point into memory
//   obtained from alloc_app (guest-addressable): the game dereferences it.
//   The library's own threads read and write that memory at any time, so it
//   stays mapped at the same address until free_app.
// - Interface pointers returned (out parameters) are AddRef'd as d3d9.h
//   requires; Release returns the new reference count, 0 meaning the object
//   is gone. A child object (texture level, back buffer) may share the
//   container's count.
// - Around any wait that may last (nextDrawable, GPU fence, readback,
//   vsync), call blocking_begin/blocking_end on the calling thread so other
//   guest threads may run. Do not touch guest memory between the two. The
//   library brackets nearly every call this way and does not nest the pair;
//   the runtime's implementation tolerates nesting anyway.
// - Calls arrive on whatever guest thread issues them, serialized by the
//   guest gate except inside blocking_begin/end windows. No call needs the
//   main thread.
#pragma once
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define D3D9METAL_HOST_VERSION 2u   // 2: modes/mode_count

// One display mode the adapter reports. A refresh rate of 0 reads as 60 Hz.
typedef struct D3D9MetalDisplayMode {
    uint32_t width, height, refresh_hz;
} D3D9MetalDisplayMode;

// Copied by d3d9metal_init; cache_dir and modes are borrowed and must stay
// valid for the life of the process.
typedef struct D3D9MetalHost {
    uint32_t size;            // sizeof(D3D9MetalHost); fields past it read as absent
    void *user;
    // HWND given by the app (here: the host SDL_Window*) -> CAMetalLayer*,
    // plus drawable size in pixels (the size the game renders at).
    void *(*metal_layer)(void *user, void *hwnd, uint32_t *width, uint32_t *height);
    // Memory the app may dereference (Lock results): guest-addressable.
    // Page-aligned (16 KiB on Apple silicon) and zeroed; align may not
    // exceed the page size. NULL on failure.
    void *(*alloc_app)(void *user, size_t size, size_t align);
    void (*free_app)(void *user, void *ptr, size_t size);
    // Around host waits (nextDrawable, GPU sync, readback): release and
    // reacquire the guest gate. Nestable, per thread.
    void (*blocking_begin)(void *user);
    void (*blocking_end)(void *user);
    uint16_t vendor_id, device_id;   // reported in D3DADAPTER_IDENTIFIER9 (drives Source's vendor hacks)
    const char *cache_dir;           // shader/PSO cache, may be NULL
    // Optional (absent from a struct whose size stops before them): display
    // modes, the first being the current one; NULL/0 gives 1920x1080 at 60 Hz.
    const D3D9MetalDisplayMode *modes;
    uint32_t mode_count;
} D3D9MetalHost;

// d3d9metal_init results.
#define D3D9METAL_OK 0
#define D3D9METAL_E_NULL (-1)         // host is NULL
#define D3D9METAL_E_INVALID (-2)      // size too small, or a required callback is NULL
#define D3D9METAL_E_ALREADY (-3)      // already initialised; the first table stays

// Exported by libd3d9metal.dylib (C linkage).
int d3d9metal_init(const D3D9MetalHost *host);          // 0 on success; once per process
void *d3d9metal_Direct3DCreate9(unsigned sdk_version);  // returns IDirect3D9* (host), NULL on failure

typedef int (*d3d9metal_init_fn)(const D3D9MetalHost *host);
typedef void *(*d3d9metal_create_fn)(unsigned sdk_version);

#ifdef __cplusplus
}
#endif
