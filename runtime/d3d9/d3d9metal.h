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
// Rules the implementation must follow (see docs/d3d9-bridge.md):
// - HWND values are SDL_Window* of the host SDL; call metal_layer to get the
//   CAMetalLayer to present into.
// - Every pointer returned by Lock/LockRect/LockBox must point into memory
//   obtained from alloc_app (guest-addressable): the game dereferences it.
// - Interface pointers returned (out parameters) are AddRef'd as d3d9.h
//   requires; Release returns the new reference count, 0 meaning the object
//   is gone. A child object (texture level, back buffer) may share the
//   container's count.
// - Around any wait that may last (nextDrawable, GPU fence, readback,
//   vsync), call blocking_begin/blocking_end on the calling thread so other
//   guest threads may run. Do not touch guest memory between the two.
// - Calls arrive on whatever guest thread issues them, serialized by the
//   guest gate except inside blocking_begin/end windows.
#pragma once
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define D3D9METAL_HOST_VERSION 1u

typedef struct D3D9MetalHost {
    uint32_t size;            // sizeof(D3D9MetalHost)
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
} D3D9MetalHost;

// Exported by libd3d9metal.dylib (C linkage).
int d3d9metal_init(const D3D9MetalHost *host);          // 0 on success; host stays valid until exit
void *d3d9metal_Direct3DCreate9(unsigned sdk_version);  // returns IDirect3D9* (host), NULL on failure

typedef int (*d3d9metal_init_fn)(const D3D9MetalHost *host);
typedef void *(*d3d9metal_create_fn)(unsigned sdk_version);

#ifdef __cplusplus
}
#endif
