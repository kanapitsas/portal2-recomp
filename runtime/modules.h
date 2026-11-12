#pragma once
#include "rt.h"
typedef struct { const char *name; uint32_t address; } GuestSymbol;
typedef struct {
    const char *name;
    uint32_t id, base, entry, init, fini;
    const uint32_t *constructors;
    size_t constructor_count;
    const GuestSymbol *symbols;
    size_t symbol_count;
    const char *const *needed;
    size_t needed_count;
} GuestModule;
extern const GuestModule rt_modules[];
extern const size_t rt_module_count;
const GuestModule *rt_find_module(const char *name);
uint32_t rt_module_symbol(const GuestModule *module, const char *name);
uint32_t rt_invoke(CPU *c, uint32_t address, const uint32_t *args, size_t count);
int rt_initialize_module(CPU *c, const GuestModule *module);
void rt_loader_set_arguments(uint32_t argc, uint32_t argv, uint32_t envp);
// Module destructors (DT_FINI_ARRAY reversed, then DT_FINI) of every
// initialised module, newest first, as ld.so does at exit. For exit().
void rt_finalize_modules(CPU *c);
// A module's image [base, base + size), the end of its highest PT_LOAD; 0 if
// unknown. Needs a guest thread (reads the program headers in guest memory).
int rt_module_range(const char *name, uint32_t *base, uint32_t *size);
// Native plugins (native_plugin.c): the loader's dlopen / dlsym / dlclose fall
// back to them.
int rt_native_plugin_open(const char *guest_path, uint32_t *handle);
int rt_native_plugin_symbol(uint32_t handle, const char *name, uint32_t *address);
int rt_native_plugin_close(uint32_t handle, int *result);
void rt_native_plugins_present(void *device);
void rt_native_plugins_reset(void *device);
void rt_native_plugins_events(void);   // SDL_PollEvent, main thread (sdl_input.c)
int rt_mouse_released(void);
void rt_mouse_note_relative(int on);
void rt_mouse_note_grab(void *window, int on);
void rt_mouse_note_cursor(int shown);
