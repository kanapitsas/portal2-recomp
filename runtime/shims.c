#include "rt.h"
#include <stdlib.h>
#include <stdio.h>

typedef struct { uint32_t function, arg, dso; } ExitHandler;
static ExitHandler *handlers;
static size_t handler_count, handler_capacity;

void rt_shims_destroy(void) {
    rt_pthreads_destroy();
    rt_futex_destroy();
    rt_libc_destroy();
    rt_tls_destroy();
    rt_modules_destroy();
    rt_vm_destroy();
    rt_heap_reset();
    free(handlers); handlers = NULL; handler_count = handler_capacity = 0;
}

size_t rt_atexit_count(void) { return handler_count; }
// __cxa_finalize marks a handler as run (function 0); exit must skip those.
void rt_atexit_clear(size_t index) { if (index < handler_count) handlers[index].function = 0; }
int rt_atexit_get(size_t index, uint32_t *function, uint32_t *arg, uint32_t *dso) {
    if (index >= handler_count) return -1;
    *function = handlers[index].function;
    *arg = handlers[index].arg;
    *dso = handlers[index].dso;
    return 0;
}

static uint32_t arg(CPU *c, unsigned index) { return R32(c->esp + 4 + 4 * index); }
void *rt_span(CPU *c, uint32_t address, uint64_t length) {
    if (length > (UINT64_C(1) << 32) - address || (length && !address))
        rt_trap(c, c->eip, "invalid libc guest span");
    rt_check_span(address, length, 1);
    return GPTR(address);
}
void *rt_write_span(CPU *c, uint32_t address, uint64_t length) {
    void *result = rt_span(c, address, length); rt_check_span(address, length, 2); return result;
}
uint32_t rt_string_length(CPU *c, uint32_t address) {
    if (!address) rt_trap(c, c->eip, "null guest string");
    uint64_t pos = address;
    while (pos < (UINT64_C(1) << 32)) {
        if (!R8((uint32_t)pos)) return (uint32_t)(pos - address);
        ++pos;
    }
    rt_trap(c, c->eip, "unterminated guest string");
}

// Return 0 for an unsupported name. The dispatcher traps, with no fake result.
// Argument layouts here are i386 cdecl, never the host's variadic/pointer ABI.
// Imports les plus appelés en jeu (P2_IMPORT_COUNTS) : corps partagés par
// core_shim et par leurs routes directes (core_resolve), sans recherche par nom.
static void core_tls_get_addr(CPU *c) {
    uint32_t a = c->eax; rt_span(c, a, 8); // i386 TLS resolver uses regparm(1), not cdecl
    c->eax = rt_tls_address(c, R32(a), R32(a + 4));
}
static void core_memset(CPU *c) {
    uint32_t a = arg(c, 0), b = arg(c, 1), n = arg(c, 2);
    memset(rt_write_span(c, a, n), (int)b, n); c->eax = a;
}
static void core_memmove(CPU *c) {
    uint32_t a = arg(c, 0), b = arg(c, 1), n = arg(c, 2);
    void *dst = rt_write_span(c, a, n); void *src = rt_span(c, b, n);
    // memmove also gives a deterministic result for overlapping memcpy;
    // conforming callers must not rely on that extension.
    memmove(dst, src, n); c->eax = a;
}
static void core_memcmp(CPU *c) {
    uint32_t a = arg(c, 0), b = arg(c, 1), n = arg(c, 2);
    void *lhs = rt_span(c, a, n); void *rhs = rt_span(c, b, n);
    c->eax = (uint32_t)memcmp(lhs, rhs, n);
}
static void core_strlen(CPU *c) { c->eax = rt_string_length(c, arg(c, 0)); }
static void core_strcmp(CPU *c) {
    uint32_t a = arg(c, 0), b = arg(c, 1);
    rt_string_length(c, a); rt_string_length(c, b);
    c->eax = (uint32_t)strcmp(GPTR(a), GPTR(b));
}
static void core_malloc(CPU *c) { c->eax = rt_alloc(arg(c, 0)); }
static void core_free(CPU *c) { if (rt_free(arg(c, 0))) rt_trap(c, c->eip, "invalid guest free"); }

static int core_shim(CPU *c, const char *name) {
    uint32_t a, b, n;
    if (!strcmp(name, "___tls_get_addr") || !strcmp(name, "__tls_get_addr")) core_tls_get_addr(c);
    else if (!strcmp(name, "memset")) core_memset(c);
    else if (!strcmp(name, "memcpy") || !strcmp(name, "memmove")) core_memmove(c);
    else if (!strcmp(name, "memcmp")) core_memcmp(c);
    else if (!strcmp(name, "strlen")) core_strlen(c);
    else if (!strcmp(name, "strcpy") || !strcmp(name, "stpcpy")) {
        a = arg(c, 0); b = arg(c, 1); n = rt_string_length(c, b);
        memmove(rt_write_span(c, a, (uint64_t)n + 1), GPTR(b), (size_t)n + 1);
        c->eax = !strcmp(name, "stpcpy") ? a + n : a;
    } else if (!strcmp(name, "strncpy")) {
        a = arg(c, 0); b = arg(c, 1); n = arg(c, 2);
        rt_write_span(c, a, n);
        uint32_t i = 0;
        while (i < n) {
            rt_span(c, b, (uint64_t)i + 1);
            uint8_t value = R8(b + i);
            W8(a + i, value);
            if (!value) break;
            ++i;
        }
        if (i < n) memset(GPTR(a + i), 0, n - i);
        c->eax = a;
    } else if (!strcmp(name, "strcmp")) core_strcmp(c);
    else if (!strcmp(name, "malloc")) core_malloc(c);
    else if (!strcmp(name, "calloc")) {
        uint64_t size = (uint64_t)arg(c, 0) * arg(c, 1);
        c->eax = size > UINT32_MAX ? 0 : rt_alloc((uint32_t)size);
        if (c->eax) memset(GPTR(c->eax), 0, (size_t)size);
    } else if (!strcmp(name, "realloc")) {
        a = arg(c, 0); b = arg(c, 1);
        if (a && !rt_heap_usable_size(a)) rt_trap(c, c->eip, "invalid guest realloc");
        c->eax = rt_realloc(a, b);
    } else if (!strcmp(name, "free")) core_free(c); else if (!strcmp(name, "malloc_usable_size")) {
        a = arg(c, 0); uint32_t usable = a ? rt_heap_usable_size(a) : 0;
        if (a && !usable) rt_trap(c, c->eip, "invalid guest malloc_usable_size");
        c->eax = usable;
    } else if (!strcmp(name, "__cxa_atexit")) {
        if (handler_count == handler_capacity) {
            size_t capacity = handler_capacity ? handler_capacity * 2 : 32;
            ExitHandler *grown = realloc(handlers, capacity * sizeof(*grown));
            if (!grown) { c->eax = UINT32_MAX; return 1; }
            handlers = grown; handler_capacity = capacity;
        }
        handlers[handler_count++] = (ExitHandler){arg(c, 0), arg(c, 1), arg(c, 2)};
        c->eax = 0;
    } else if (!strcmp(name, "puts")) {
        a = arg(c, 0); rt_string_length(c, a);
        c->eax = (uint32_t)puts(GPTR(a));
    } else if (!strcmp(name, "abort")) {
        rt_trap(c, c->eip, "guest libc abort");
    } else return 0;
    return 1;
}

// Ordre d'aiguillage d'un nom : fournisseurs intégrés, puis fournisseurs
// enregistrés (RT_SHIM_PROVIDER) dans l'ordre des constructeurs. Le premier
// qui accepte le nom l'exécute ; c'est aussi lui que rt_dispatch met en cache.
static const rt_shim_provider builtin[] = {core_shim, rt_format_shim, rt_pthread_shim, rt_libc_shim,
                                           rt_scan_shim, rt_loader_shim, rt_stat_shim};
enum { BUILTIN = sizeof(builtin) / sizeof(*builtin) };
#define CORE_HANDLER(fn) static int fn##_handler(CPU *c, const char *name, void *data) { (void)name; (void)data; fn(c); return 1; }
CORE_HANDLER(core_tls_get_addr) CORE_HANDLER(core_memset) CORE_HANDLER(core_memmove) CORE_HANDLER(core_memcmp)
CORE_HANDLER(core_strlen) CORE_HANDLER(core_strcmp) CORE_HANDLER(core_malloc) CORE_HANDLER(core_free)
static int core_resolve(const char *name, RtShimRoute *route) {
    static const struct { const char *name; rt_shim_handler handler; } hot[] = {
        {"___tls_get_addr", core_tls_get_addr_handler}, {"__tls_get_addr", core_tls_get_addr_handler},
        {"memcpy", core_memmove_handler}, {"memmove", core_memmove_handler}, {"memset", core_memset_handler},
        {"memcmp", core_memcmp_handler}, {"strlen", core_strlen_handler}, {"strcmp", core_strcmp_handler},
        {"malloc", core_malloc_handler}, {"free", core_free_handler},
    };
    for (size_t i = 0; i < sizeof(hot) / sizeof(*hot); ++i)
        if (!strcmp(name, hot[i].name)) { route->handler = hot[i].handler; return 1; }
    return 0;
}
int rt_libc_resolve(const char *name, RtShimRoute *route);   // libc.c
static const rt_shim_resolver builtin_resolvers[BUILTIN] = {core_resolve, NULL, NULL, rt_libc_resolve};
static rt_shim_provider providers[64];
static rt_shim_resolver resolvers[64];
static size_t provider_count;
void rt_register_shim_resolver(rt_shim_provider provider, rt_shim_resolver resolver) {
    if (provider_count == sizeof(providers) / sizeof(*providers)) abort();
    resolvers[provider_count] = resolver;
    providers[provider_count++] = provider;
}
void rt_register_shim_provider(rt_shim_provider provider) { rt_register_shim_resolver(provider, NULL); }

int rt_shim_route(CPU *c, const char *name, RtShimRoute *route) {
    for (size_t i = 0; i < BUILTIN + provider_count; ++i) {
        rt_shim_provider provider = i < BUILTIN ? builtin[i] : providers[i - BUILTIN];
        if (!provider(c, name)) continue;
        if (route) {
            rt_shim_resolver resolver = i < BUILTIN ? builtin_resolvers[i] : resolvers[i - BUILTIN];
            RtShimRoute direct = {provider, NULL, NULL};
            if (!resolver || !resolver(name, &direct) || !direct.handler) direct = (RtShimRoute){provider, NULL, NULL};
            *route = direct;
        }
        return 1;
    }
    return 0;
}

int rt_shim(CPU *c, const char *name) { return rt_shim_route(c, name, NULL); }
