// Native plugins (native_plugin.h): a Mach-O image the guest dlopen()s in
// place of a Linux shared object. Loaded with the host's dlopen, given the
// RtPluginHost table, then reached by the guest through dynamic import
// addresses ("rtplugin:<name>", see rt_dynamic_import), like any other bridge.
//
// Handles returned to the guest's dlopen are 0xcd000000 + 0x100 * index,
// outside the recompiled modules' range (modules.c). A plugin is never
// unloaded: dlclose only counts down, as for the recompiled modules.
#include "rt.h"
#include "modules.h"
#include "native_plugin.h"
#include <SDL2/SDL.h>
#include <dlfcn.h>
#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define PLUGIN_HANDLE_BASE 0xcd000000u
#define PLUGIN_HANDLE_STRIDE 0x100u
#define MAX_PLUGINS 16
#define PREFIX "rtplugin:"

typedef struct { char *name; uint32_t address; } Export;
typedef void (*DeviceHook)(void *device, void *user);
struct RtPlugin {
    char path[PATH_MAX];
    void *library;
    Export *exports;
    size_t export_count;
    uint32_t opens;
    DeviceHook present, reset;
    void *present_user, *reset_user;
    void (*events)(void *user);
    void *events_user;
};
static RtPlugin plugins[MAX_PLUGINS];
static size_t plugin_count;

typedef struct Defined { char *name; RtPluginFn fn; void *data; struct Defined *next; } Defined;
static Defined *defined;
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

// ---------------------------------------------------------------- guest -> plugin
static Defined *find_defined(const char *name) {
    if (strncmp(name, PREFIX, sizeof(PREFIX) - 1)) return NULL;
    pthread_mutex_lock(&lock);
    Defined *d = defined;
    while (d && strcmp(d->name, name)) d = d->next;
    pthread_mutex_unlock(&lock);
    return d;
}
static int defined_handler(CPU *c, const char *name, void *data) {
    (void)name;
    Defined *d = data;
    d->fn(c, c->esp + 4, d->data);
    return 1;
}
static int plugin_shim(CPU *c, const char *name) {
    Defined *d = find_defined(name);
    return d ? defined_handler(c, name, d) : 0;
}
static int plugin_resolve(const char *name, RtShimRoute *route) {
    Defined *d = find_defined(name);
    if (!d) return 0;
    route->handler = defined_handler;
    route->data = d;
    return 1;
}
RT_SHIM_PROVIDER_RESOLVER(plugin_shim, plugin_resolve)

static uint32_t host_define(const char *name, RtPluginFn fn, void *data) {
    if (!name || !fn) return 0;
    size_t length = sizeof(PREFIX) + strlen(name);
    char *full = malloc(length);
    if (!full) return 0;
    snprintf(full, length, PREFIX "%s", name);
    Defined *d = find_defined(full);
    if (d) {   // redefined (a plugin reloaded its code): same address, new target
        d->fn = fn; d->data = data;
        free(full);
    } else {
        d = calloc(1, sizeof(*d));
        if (!d) { free(full); return 0; }
        *d = (Defined){full, fn, data, NULL};
        pthread_mutex_lock(&lock);
        d->next = defined; defined = d;
        pthread_mutex_unlock(&lock);
    }
    return rt_dynamic_import(d->name);
}

static void host_export_symbol(RtPlugin *plugin, const char *name, uint32_t address) {
    if (!plugin || !name) return;
    for (size_t i = 0; i < plugin->export_count; ++i)
        if (!strcmp(plugin->exports[i].name, name)) { plugin->exports[i].address = address; return; }
    Export *grown = realloc(plugin->exports, (plugin->export_count + 1) * sizeof(*grown));
    char *copy = strdup(name);
    if (!grown || !copy) { free(copy); if (grown) plugin->exports = grown; return; }
    plugin->exports = grown;
    plugin->exports[plugin->export_count++] = (Export){copy, address};
}

static void host_set_return(void *cpu, uint32_t eax) { ((CPU *)cpu)->eax = eax; }
static void host_set_return_float(void *cpu, double value) { rt_return_st0((CPU *)cpu, value); }

// ---------------------------------------------------------------- plugin -> guest
static void *host_current_cpu(void) { return rt_shim_cpu(); }

static uint32_t host_call(void *cpu, uint32_t function, const uint32_t *words, size_t count, int returns_float,
                          uint32_t *edx, double *st0) {
    CPU *c = cpu;
    uint32_t eax = rt_invoke(c, function, words, count);
    if (edx) *edx = c->edx;
    if (returns_float) {
        double value = fpop(c);
        if (st0) *st0 = value;
    }
    return eax;
}

static void host_free(uint32_t address) { if (address) rt_free(address); }

static int host_readable(uint32_t address, uint32_t size) {
    if (!size) return 1;
    if (address > UINT32_MAX - (size - 1)) return 0;
    uint32_t last = address + size - 1;
    for (uint32_t page = address & ~4095u;; page += 4096) {
        if (!rt_guest_readable(page < address ? address : page)) return 0;
        if (page >= (last & ~4095u)) return 1;
    }
}

static int host_patch(uint32_t address, const void *bytes, uint32_t size) {
    if (!size) return 1;
    if (!host_readable(address, size)) return 0;
    uintptr_t page = (uintptr_t)getpagesize();
    uintptr_t begin = (uintptr_t)(M + address) & ~(page - 1);
    uintptr_t end = ((uintptr_t)(M + address) + size + page - 1) & ~(page - 1);
    // Guest images are data to the host (their code was recompiled): read
    // and write is all the host needs, and it stays so.
    if (mprotect((void *)begin, end - begin, PROT_READ | PROT_WRITE)) return 0;
    memcpy(M + address, bytes, size);
    return 1;
}

static int host_module_range(const char *name, uint32_t *base, uint32_t *size) {
    return rt_module_range(name, base, size);
}

static uint32_t host_module_symbol(const char *module, const char *symbol) {
    const GuestModule *m = module ? rt_find_module(module) : NULL;
    return m && symbol ? rt_module_symbol(m, symbol) : 0;
}

// ---------------------------------------------------------------- hooks
static void host_set_present_hook(RtPlugin *plugin, void (*hook)(void *, void *), void *user) {
    if (!plugin) return;
    plugin->present_user = user;
    __atomic_store_n(&plugin->present, hook, __ATOMIC_RELEASE);
}
static void host_set_reset_hook(RtPlugin *plugin, void (*hook)(void *, void *), void *user) {
    if (!plugin) return;
    plugin->reset_user = user;
    __atomic_store_n(&plugin->reset, hook, __ATOMIC_RELEASE);
}
void rt_native_plugins_present(void *device) {
    for (size_t i = 0; i < plugin_count; ++i) {
        DeviceHook hook = __atomic_load_n(&plugins[i].present, __ATOMIC_ACQUIRE);
        if (hook) hook(device, plugins[i].present_user);
    }
}
void rt_native_plugins_reset(void *device) {
    for (size_t i = 0; i < plugin_count; ++i) {
        DeviceHook hook = __atomic_load_n(&plugins[i].reset, __ATOMIC_ACQUIRE);
        if (hook) hook(device, plugins[i].reset_user);
    }
}

static void host_set_events_hook(RtPlugin *plugin, void (*hook)(void *), void *user) {
    if (!plugin) return;
    plugin->events_user = user;
    __atomic_store_n(&plugin->events, hook, __ATOMIC_RELEASE);
}
void rt_native_plugins_events(void) {
    for (size_t i = 0; i < plugin_count; ++i) {
        void (*hook)(void *) = __atomic_load_n(&plugins[i].events, __ATOMIC_ACQUIRE);
        if (hook) hook(plugins[i].events_user);
    }
}

// The mouse: what the game last asked for, applied again when it is given back.
static int mouse_released;
static int wanted_relative, wanted_grab, wanted_cursor = 1;
static SDL_Window *grab_window;
int rt_mouse_released(void) { return mouse_released; }
void rt_mouse_note_relative(int on) { wanted_relative = on; }
void rt_mouse_note_grab(void *window, int on) { grab_window = window; wanted_grab = on; }
void rt_mouse_note_cursor(int shown) { wanted_cursor = shown; }
static void host_release_mouse(int released) {
    released = !!released;
    if (released == mouse_released) return;
    mouse_released = released;
    if (released) {
        SDL_SetRelativeMouseMode(SDL_FALSE);
        if (grab_window) SDL_SetWindowGrab(grab_window, SDL_FALSE);
        SDL_ShowCursor(SDL_ENABLE);
    } else {
        SDL_ShowCursor(wanted_cursor ? SDL_ENABLE : SDL_DISABLE);
        if (grab_window) SDL_SetWindowGrab(grab_window, wanted_grab ? SDL_TRUE : SDL_FALSE);
        SDL_SetRelativeMouseMode(wanted_relative ? SDL_TRUE : SDL_FALSE);
    }
}

static RtPluginHost host = {
    .size = sizeof(RtPluginHost),
    .version = RT_PLUGIN_HOST_VERSION,
    .define = host_define,
    .export_symbol = host_export_symbol,
    .set_return = host_set_return,
    .set_return_float = host_set_return_float,
    .current_cpu = host_current_cpu,
    .call = host_call,
    .alloc = rt_alloc,
    .free = host_free,
    .readable = host_readable,
    .patch = host_patch,
    .module_range = host_module_range,
    .module_symbol = host_module_symbol,
    .set_present_hook = host_set_present_hook,
    .set_reset_hook = host_set_reset_hook,
    .set_events_hook = host_set_events_hook,
    .release_mouse = host_release_mouse,
};

// ---------------------------------------------------------------- loader (modules.c)
static int is_macho(const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) return 0;
    uint32_t magic = 0;
    size_t n = fread(&magic, 1, 4, f);
    fclose(f);
    return n == 4 && (magic == 0xfeedfacfu || magic == 0xbebafecau || magic == 0xcafebabeu);
}

// dlopen of `guest_path`: 1 if it is a native plugin (*handle set, 0 if it
// failed to load), 0 if not one (the caller reports a missing module).
int rt_native_plugin_open(const char *guest_path, uint32_t *handle) {
    const char *host_path = rt_read_path(guest_path);
    if (!host_path || access(host_path, R_OK) || !is_macho(host_path)) return 0;
    char real[PATH_MAX];
    if (!realpath(host_path, real)) snprintf(real, sizeof(real), "%s", host_path);
    *handle = 0;
    for (size_t i = 0; i < plugin_count; ++i)
        if (!strcmp(plugins[i].path, real)) {
            ++plugins[i].opens;
            *handle = PLUGIN_HANDLE_BASE + PLUGIN_HANDLE_STRIDE * (uint32_t)i;
            return 1;
        }
    if (plugin_count == MAX_PLUGINS) { fprintf(stderr, "native plugin %s: too many plugins\n", real); return 1; }
    host.memory = M;   // set before any guest code runs
    void *library = dlopen(real, RTLD_NOW | RTLD_LOCAL);
    if (!library) { fprintf(stderr, "native plugin %s: %s\n", real, dlerror()); return 1; }
    RtPluginInitFn init = (RtPluginInitFn)dlsym(library, RT_PLUGIN_INIT_SYMBOL);
    if (!init) {
        fprintf(stderr, "native plugin %s: no %s\n", real, RT_PLUGIN_INIT_SYMBOL);
        dlclose(library);
        return 1;
    }
    RtPlugin *plugin = &plugins[plugin_count];
    memset(plugin, 0, sizeof(*plugin));
    snprintf(plugin->path, sizeof(plugin->path), "%s", real);
    plugin->library = library;
    ++plugin_count;   // before init: it may set hooks and exports
    if (init(&host, plugin)) {
        fprintf(stderr, "native plugin %s: initialisation refused\n", real);
        plugin->library = NULL;   // stays in the table (hooks cleared), never handed out
        plugin->present = plugin->reset = NULL;
        plugin->events = NULL;
        return 1;
    }
    plugin->opens = 1;
    fprintf(stderr, "native plugin %s loaded\n", real);
    *handle = PLUGIN_HANDLE_BASE + PLUGIN_HANDLE_STRIDE * (uint32_t)(plugin - plugins);
    return 1;
}

static RtPlugin *plugin_of(uint32_t handle) {
    if (handle < PLUGIN_HANDLE_BASE || (handle - PLUGIN_HANDLE_BASE) % PLUGIN_HANDLE_STRIDE) return NULL;
    size_t index = (handle - PLUGIN_HANDLE_BASE) / PLUGIN_HANDLE_STRIDE;
    return index < plugin_count && plugins[index].library ? &plugins[index] : NULL;
}

// dlsym on a plugin handle: 1 if `handle` is one (*address 0 when the symbol is unknown).
int rt_native_plugin_symbol(uint32_t handle, const char *name, uint32_t *address) {
    RtPlugin *plugin = plugin_of(handle);
    if (!plugin) return 0;
    *address = 0;
    for (size_t i = 0; i < plugin->export_count; ++i)
        if (!strcmp(plugin->exports[i].name, name)) { *address = plugin->exports[i].address; break; }
    return 1;
}

// dlclose: 1 if `handle` is a plugin (*result as dlclose returns).
int rt_native_plugin_close(uint32_t handle, int *result) {
    RtPlugin *plugin = plugin_of(handle);
    if (!plugin) return 0;
    *result = plugin->opens ? (--plugin->opens, 0) : -1;
    return 1;
}
