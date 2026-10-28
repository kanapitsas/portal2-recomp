// Guest dynamic loader: module initialisation, dlopen/dlsym/dlclose and the
// loader services the statically linked runtime pieces of each module use.
// SHIMS: dl_iterate_phdr dlclose dlmopen __cxa_finalize _Jv_RegisterClasses __libc_start_main
//
// Every prepared image is mapped at startup at its fixed address; "loading" a
// module means running its initialisers once. dlclose keeps the image and its
// state (as with RTLD_NODELETE, or musl, where dlclose never unloads): images
// cannot be unmapped from the static recompilation, and re-running
// constructors on a reused image would need fresh TLS blocks in every thread.
// Module destructors run at process exit through rt_finalize_modules.
#include "modules.h"
#include <stdio.h>
#include <stdlib.h>

enum { UNLOADED, INITIALIZING, READY, FAILED, FINALIZED };
static unsigned char *state;
static uint32_t *opens;        // outstanding dlopen handles per module
static uint32_t *names;        // guest copies of dlpi_name
static size_t *order, loaded;  // link-map order: when initialisation started
static size_t *ready, ready_count;  // completion order, for destructors
static uint32_t boot_argc, boot_argv, boot_envp;
static uint32_t error_string;

#define HANDLE_BASE 0xcc000000u
#define PROGRAM_HANDLE 0xccfff000u
#define PT_LOAD 1
#define PT_DYNAMIC 2
#define PT_TLS 7
#define PT_PHDR 6
#define DT_NULL 0
#define DT_FINI_ARRAY 26
#define DT_FINI_ARRAYSZ 28

const GuestModule *rt_find_module(const char *name) {
    const char *base = strrchr(name, '/'); base = base ? base + 1 : name;
    for (size_t i = 0; i < rt_module_count; ++i)
        if (!strcmp(rt_modules[i].name, base)) return &rt_modules[i];
    return NULL;
}
uint32_t rt_module_symbol(const GuestModule *module, const char *name) {
    if (!module) {
        for (size_t i = 0; i < rt_module_count; ++i) {
            uint32_t value = rt_module_symbol(&rt_modules[i], name);
            if (value) return value;
        }
        return 0;
    }
    size_t lo = 0, hi = module->symbol_count;
    while (lo < hi) {
        size_t middle = lo + (hi - lo) / 2;
        int order = strcmp(name, module->symbols[middle].name);
        if (!order) return module->symbols[middle].address;
        if (order < 0) hi = middle; else lo = middle + 1;
    }
    return 0;
}
uint32_t rt_invoke(CPU *c, uint32_t address, const uint32_t *args, size_t count) {
    extern _Thread_local CPU *rt_fault_cpu;
    if (!rt_fault_cpu) rt_fault_cpu = c;
    uint32_t original = c->esp;
    c->esp -= (c->esp - (uint32_t)count * 4) & 15;
    for (size_t i = count; i; --i) PUSH(c, args[i - 1]);
    uint32_t expected = c->esp;
    rt_call(c, address, RT_RETURN_SENTINEL);
    if (c->esp != expected) rt_trap(c, address, "cdecl stack imbalance");
    c->esp = original;
    return c->eax;
}
void rt_loader_set_arguments(uint32_t argc, uint32_t argv, uint32_t envp) {
    boot_argc = argc; boot_argv = argv; boot_envp = envp;
}
static int ensure_tables(void) {
    if (state) return 0;
    state = calloc(rt_module_count + 1, 1);
    opens = calloc(rt_module_count + 1, sizeof(*opens));
    names = calloc(rt_module_count + 1, sizeof(*names));
    order = calloc(rt_module_count + 1, sizeof(*order));
    ready = calloc(rt_module_count + 1, sizeof(*ready));
    if (state && opens && names && order && ready) return 0;
    free(state); free(opens); free(names); free(order); free(ready);
    state = NULL; opens = names = NULL; order = ready = NULL;
    return -1;
}
int rt_initialize_module(CPU *c, const GuestModule *module) {
    if (ensure_tables()) return -1;
    size_t index = (size_t)(module - rt_modules);
    if (index >= rt_module_count) return -1;
    if (state[index]) return state[index] == FAILED ? -1 : 0;
    state[index] = INITIALIZING;
    order[loaded++] = index;
    for (size_t i = 0; i < module->needed_count; ++i) {
        const GuestModule *dependency = rt_find_module(module->needed[i]);
        if (dependency && rt_initialize_module(c, dependency)) { state[index] = FAILED; return -1; }
    }
    fprintf(stderr, "initializing %s (%zu constructors)\n", module->name, module->constructor_count);
    uint32_t args[] = {boot_argc, boot_argv, boot_envp};
    if (module->init) rt_invoke(c, module->init, args, 3);
    for (size_t i = 0; i < module->constructor_count; ++i)
        if (module->constructors[i] && module->constructors[i] != UINT32_MAX)
            rt_invoke(c, module->constructors[i], args, 3);
    state[index] = READY;
    ready[ready_count++] = index;
    return 0;
}
static void loader_error(const char *prefix, const char *name) {
    char buffer[1024];
    snprintf(buffer, sizeof(buffer), "%s: %s", prefix, name);
    size_t size = strlen(buffer) + 1;
    if (error_string) rt_free(error_string);
    error_string = rt_alloc((uint32_t)size);
    if (error_string) memcpy(GPTR(error_string), buffer, size);
}

// Program headers of a module, read from its mapped image like ld.so does.
typedef struct { uint32_t address; uint16_t count; } Headers;
static Headers program_headers(CPU *c, const GuestModule *module) {
    uint32_t base = module->base;
    if (memcmp(rt_span(c, base, 52), "\177ELF\001\001", 6) || R16(base + 42) != 32)
        rt_trap(c, c->eip, "guest module image has no i386 ELF header at its base");
    Headers h = {base + R32(base + 28), R16(base + 44)};
    rt_span(c, h.address, (uint64_t)h.count * 32);
    for (uint16_t i = 0; i < h.count; ++i)
        if (R32(h.address + 32 * i) == PT_PHDR) h.address = base + R32(h.address + 32 * i + 8);
    return h;
}
static uint32_t segment(CPU *c, const GuestModule *module, uint32_t type, uint32_t *size) {
    Headers h = program_headers(c, module);
    for (uint16_t i = 0; i < h.count; ++i) {
        uint32_t entry = h.address + 32 * i;
        if (R32(entry) == type) { if (size) *size = R32(entry + 16); return module->base + R32(entry + 8); }
    }
    return 0;
}

static uint32_t module_name(const GuestModule *module, size_t index) {
    if (!names[index]) {
        // The main program is reported with an empty name, as by glibc.
        const char *text = rt_module_symbol(module, "main") ? "" : module->name;
        size_t size = strlen(text) + 1;
        names[index] = rt_alloc((uint32_t)size);
        if (names[index]) memcpy(GPTR(names[index]), text, size);
    }
    return names[index];
}

static int iterate_phdr(CPU *c) {
    uint32_t callback = R32(c->esp + 4), data = R32(c->esp + 8);
    if (ensure_tables()) rt_trap(c, c->eip, "loader tables unavailable");
    uint32_t saved = c->esp;
    int result = 0;
    for (size_t k = 0; k < loaded && !result; ++k) {
        size_t index = order[k];
        if (state[index] == UNLOADED || state[index] == FAILED) continue;
        const GuestModule *module = &rt_modules[index];
        Headers h = program_headers(c, module);
        uint32_t name = module_name(module, index);
        if (!name) rt_trap(c, c->eip, "cannot allocate dl_phdr_info name");
        uint32_t tls_size = 0;
        int has_tls = segment(c, module, PT_TLS, &tls_size) && tls_size;
        // struct dl_phdr_info, i386 glibc layout (40 bytes), in dead stack space.
        c->esp = (saved - 64) & ~15u;
        uint32_t info = c->esp;
        W32(info, module->base); W32(info + 4, name); W32(info + 8, h.address);
        W16(info + 12, h.count); W16(info + 14, 0);
        W64(info + 16, loaded); W64(info + 24, 0);     // dlpi_adds, dlpi_subs (nothing unloaded)
        W32(info + 32, has_tls ? module->id : 0);
        W32(info + 36, has_tls ? rt_tls_address(c, module->id, 0) : 0);
        uint32_t args[] = {info, 40, data};
        result = (int32_t)rt_invoke(c, callback, args, 3);
        c->esp = saved;
    }
    c->eax = (uint32_t)result;
    return 1;
}

static int handle_module(uint32_t handle, size_t *index) {
    for (size_t i = 0; i < rt_module_count; ++i)
        if (handle == HANDLE_BASE + rt_modules[i].id * 0x100) { *index = i; return 1; }
    return 0;
}

static void open_module(CPU *c, uint32_t path, uint32_t mode) {
    if (!path) { c->eax = PROGRAM_HANDLE; return; }
    rt_string_length(c, path);
    const GuestModule *module = rt_find_module(GPTR(path));
    if (ensure_tables()) { loader_error("loader tables unavailable", GPTR(path)); c->eax = 0; return; }
    size_t index = module ? (size_t)(module - rt_modules) : 0;
    if (!module) { loader_error("guest module unavailable", GPTR(path)); c->eax = 0; return; }
    if ((mode & 4) && state[index] != READY && state[index] != INITIALIZING) {   // RTLD_NOLOAD
        c->eax = 0; return;
    }
    if (rt_initialize_module(c, module)) { loader_error("initialization failed", module->name); c->eax = 0; return; }
    ++opens[index];
    c->eax = HANDLE_BASE + module->id * 0x100;
}

int rt_loader_shim(CPU *c, const char *name) {
    if (!strcmp(name, "dlopen")) {
        open_module(c, R32(c->esp + 4), R32(c->esp + 8));
    } else if (!strcmp(name, "dlmopen")) {
        int32_t namespace = (int32_t)R32(c->esp + 4);
        uint32_t path = R32(c->esp + 8);
        if (namespace == 0) { open_module(c, path, R32(c->esp + 12)); return 1; }  // LM_ID_BASE
        // A new link-map namespace needs a second copy of the module at other
        // addresses, which a static recompilation cannot provide.
        if (path) rt_string_length(c, path);
        if (path && rt_find_module(GPTR(path)))
            rt_trap(c, c->eip, "dlmopen: separate link-map namespaces are not supported");
        loader_error("guest module unavailable", path ? (const char *)GPTR(path) : "(null)");
        c->eax = 0;
    } else if (!strcmp(name, "dlsym")) {
        uint32_t handle = R32(c->esp + 4), symbol = R32(c->esp + 8);
        rt_string_length(c, symbol);
        const GuestModule *module = NULL;
        if (handle == UINT32_MAX) rt_trap(c, c->eip, "RTLD_NEXT lookup not implemented");
        if (handle && handle != PROGRAM_HANDLE && handle != UINT32_MAX) {
            size_t index;
            if (handle_module(handle, &index)) module = &rt_modules[index];
            if (!module) { loader_error("invalid guest module handle", GPTR(symbol)); c->eax = 0; return 1; }
        }
        c->eax = rt_module_symbol(module, GPTR(symbol));
        if (!c->eax) loader_error("guest symbol unavailable", GPTR(symbol));
    } else if (!strcmp(name, "dlerror")) {
        c->eax = error_string; error_string = 0;   // the string stays valid until the next error
    } else if (!strcmp(name, "dlclose")) {
        uint32_t handle = R32(c->esp + 4);
        size_t index;
        if (handle == PROGRAM_HANDLE) { c->eax = 0; return 1; }
        if (ensure_tables() || !handle_module(handle, &index) || !opens[index]) {
            loader_error("dlclose", "shared object not open"); c->eax = UINT32_MAX; return 1;
        }
        --opens[index];   // the image stays initialised: see the comment at the top of this file
        c->eax = 0;
    } else if (!strcmp(name, "dl_iterate_phdr")) {
        return iterate_phdr(c);
    } else if (!strcmp(name, "__cxa_finalize")) {
        uint32_t dso = R32(c->esp + 4);
        // Run matching handlers in reverse registration order; each one is
        // cleared before it runs and the scan restarts, so handlers registered
        // meanwhile are honoured, as in glibc.
        for (;;) {
            size_t i = rt_atexit_count();
            uint32_t function = 0, arg = 0, owner = 0;
            while (i--) {
                if (rt_atexit_get(i, &function, &arg, &owner)) rt_trap(c, c->eip, "atexit table changed");
                if (function && (!dso || owner == dso)) break;
            }
            if (i == (size_t)-1) break;
            rt_atexit_clear(i);
            uint32_t argument[] = {arg};
            rt_invoke(c, function, argument, 1);
        }
    } else if (!strcmp(name, "_Jv_RegisterClasses")) {
        // Weak libgcj hook called by crtstuff only for a non-empty .jcr list.
        // No Java runtime exists here: registering nothing is what a process
        // without libgcj does.
    } else if (!strcmp(name, "__libc_start_main")) {
        uint32_t main_function = R32(c->esp + 4), argc = R32(c->esp + 8), argv = R32(c->esp + 12);
        uint32_t init = R32(c->esp + 16);
        uint32_t envp = argv + 4 * (argc + 1);
        // The loader already ran the program's DT_INIT/init_array when it
        // initialised the module; old-style `init` (__libc_csu_init) would run
        // the same constructors a second time.
        const GuestModule *program = NULL;
        for (size_t i = 0; i < rt_module_count && !program; ++i)
            if (rt_modules[i].base <= main_function && rt_module_symbol(&rt_modules[i], "main") == main_function)
                program = &rt_modules[i];
        int initialised = program && state && state[program - rt_modules] == READY;
        uint32_t args[] = {argc, argv, envp};
        if (init && !initialised) rt_invoke(c, init, args, 3);
        uint32_t status = rt_invoke(c, main_function, args, 3);
        // exit(status): delegated to the process shim; it must not return.
        uint32_t saved = c->esp;
        PUSH(c, status); PUSH(c, RT_RETURN_SENTINEL);
        if (!rt_shim(c, "exit")) rt_trap(c, c->eip, "__libc_start_main: exit is not implemented");
        c->esp = saved;
        rt_trap(c, c->eip, "__libc_start_main: exit returned");
    } else return 0;
    return 1;
}

// Destructors of every initialised module, newest first, as ld.so's _dl_fini
// does at exit: DT_FINI_ARRAY in reverse, then DT_FINI. Safe to call once.
void rt_finalize_modules(CPU *c) {
    if (!state) return;
    while (ready_count) {
        size_t index = ready[--ready_count];
        if (state[index] != READY) continue;
        state[index] = FINALIZED;
        const GuestModule *module = &rt_modules[index];
        uint32_t size = 0, dynamic = segment(c, module, PT_DYNAMIC, &size), array = 0, bytes = 0;
        for (uint32_t entry = dynamic; dynamic && entry + 8 <= dynamic + size; entry += 8) {
            int32_t tag = (int32_t)R32(entry);
            if (tag == DT_NULL) break;
            if (tag == DT_FINI_ARRAY) array = module->base + R32(entry + 4);
            if (tag == DT_FINI_ARRAYSZ) bytes = R32(entry + 4);
        }
        if (array) rt_span(c, array, bytes);
        for (uint32_t i = bytes / 4; array && i; --i) {
            uint32_t function = R32(array + 4 * (i - 1));
            if (function && function != UINT32_MAX) rt_invoke(c, function, NULL, 0);
        }
        if (module->fini) rt_invoke(c, module->fini, NULL, 0);
    }
}

void rt_modules_destroy(void) {
    free(state); free(opens); free(names); free(order); free(ready);
    state = NULL; opens = names = NULL; order = ready = NULL;
    loaded = ready_count = 0;
    boot_argc = boot_argv = boot_envp = error_string = 0;
}
