// Native plugins: arm64 dylibs that the guest loads as if they were Linux
// shared objects (a Source server plugin in addons/, for example).
//
// When the guest dlopen()s a path that is not a recompiled module and the
// file there is a Mach-O image, the runtime dlopen()s it natively and calls
// its entry point:
//
//     int rt_native_plugin_init(const RtPluginHost *host, RtPlugin *plugin);
//
// The plugin then gives the guest what it needs through the host table:
// native functions the guest can call (`define`, guest addresses of
// rt_dynamic_import), the symbols the guest's dlsym() finds (`export_symbol`),
// guest memory, calls into guest code, and a few hooks (Present, Reset, the
// mouse). Guest pointers are 32-bit addresses; `memory + address` is the host
// pointer to the same bytes.
//
// This header is the whole contract, plain C, versioned by
// RT_PLUGIN_HOST_VERSION (a plugin refuses a host older than the one it was
// built for). A plugin keeps a copy of it.
#pragma once
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RT_PLUGIN_HOST_VERSION 1u

typedef struct RtPlugin RtPlugin;   // opaque, one per loaded plugin

// A native function called by guest code (cdecl: the caller pops the
// arguments). `cpu` is the calling guest thread's CPU; `args` the guest
// address of the first argument (for a C++ method: `this`, then the others).
// The return value is set with set_return / set_return_float.
typedef void (*RtPluginFn)(void *cpu, uint32_t args, void *data);

typedef struct RtPluginHost {
    uint32_t size;      // sizeof(RtPluginHost)
    uint32_t version;   // RT_PLUGIN_HOST_VERSION
    uint8_t *memory;    // host base of the 4 GB guest address space

    // --- guest code calling the plugin ---
    // A stable guest address that calls `fn` (`name` must be unique per
    // process: prefix it with the plugin's name). 0 when the table is full.
    uint32_t (*define)(const char *name, RtPluginFn fn, void *data);
    // What the guest's dlsym(handle, name) on this plugin returns.
    void (*export_symbol)(RtPlugin *plugin, const char *name, uint32_t address);
    void (*set_return)(void *cpu, uint32_t eax);
    void (*set_return_float)(void *cpu, double value);   // in ST(0), as i386 returns float/double

    // --- the plugin calling guest code ---
    // The guest CPU of this thread while it runs guest code or a plugin
    // function (NULL on a thread that never ran guest code).
    void *(*current_cpu)(void);
    // cdecl call of `function` with `count` 32-bit words (the plugin packs
    // floats, pointers and `this`). Returns EAX; *edx and *st0 (popped from
    // the x87 stack when `returns_float`) when not NULL. Needs the guest lock:
    // call it from guest code's thread (a plugin function, a hook).
    uint32_t (*call)(void *cpu, uint32_t function, const uint32_t *words, size_t count, int returns_float,
                     uint32_t *edx, double *st0);

    // --- guest memory ---
    uint32_t (*alloc)(uint32_t size);   // guest heap, 16-byte aligned; 0 if out of memory
    void (*free)(uint32_t address);
    int (*readable)(uint32_t address, uint32_t size);
    // Writes bytes the guest maps read-only (vtables, constants).
    int (*patch)(uint32_t address, const void *bytes, uint32_t size);
    // A recompiled module's image ("server.so"): base and size. 0 if unknown.
    int (*module_range)(const char *name, uint32_t *base, uint32_t *size);
    // An exported symbol of a recompiled module ("client.so", "CreateInterface"); 0 if none.
    uint32_t (*module_symbol)(const char *module, const char *symbol);

    // --- hooks (one per plugin; NULL removes it) ---
    // Right before IDirect3DDevice9::Present on the Metal renderer, with the
    // native device (an arm64 IDirect3DDevice9*), on the presenting thread.
    void (*set_present_hook)(RtPlugin *plugin, void (*hook)(void *device, void *user), void *user);
    // Right before IDirect3DDevice9::Reset / ResetEx (release D3DPOOL_DEFAULT resources).
    void (*set_reset_hook)(RtPlugin *plugin, void (*hook)(void *device, void *user), void *user);
    // Each time the game polls its window events (SDL_PollEvent, its main
    // thread, also in the menus), as a guest call: `current_cpu` and `call`
    // work there. Runs often (several times a frame): keep it cheap.
    void (*set_events_hook)(RtPlugin *plugin, void (*hook)(void *user), void *user);
    // While released, the game's relative mouse mode, window grab and cursor
    // warps are held back (the OS cursor is free for a plugin's own UI); the
    // game's last request is applied again when the mouse is given back.
    void (*release_mouse)(int released);
} RtPluginHost;

typedef int (*RtPluginInitFn)(const RtPluginHost *host, RtPlugin *plugin);
#define RT_PLUGIN_INIT_SYMBOL "rt_native_plugin_init"

#ifdef __cplusplus
}
#endif
