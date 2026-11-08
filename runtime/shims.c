#include "rt.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct { uint32_t function, arg, dso; } ExitHandler;
static ExitHandler *handlers;
static size_t handler_count, handler_capacity;

void rt_strings_extra_destroy(void);   // strings_extra.c (posix.h)
void rt_shims_destroy(void) {
    rt_pthreads_destroy();
    rt_futex_destroy();
    rt_libc_destroy();
    rt_strings_extra_destroy();
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
// Une page écrivable est aussi lisible (page_flags, vm.c) : un seul contrôle
// de la table suffit pour chaque mode.
static void *span(CPU *c, uint32_t address, uint64_t length, int mode) {
    if (length > (UINT64_C(1) << 32) - address || (length && !address))
        rt_trap(c, c->eip, "invalid libc guest span");
    rt_check_span(address, length, mode);
    return GPTR(address);
}
void *rt_span(CPU *c, uint32_t address, uint64_t length) { return span(c, address, length, 1); }
void *rt_write_span(CPU *c, uint32_t address, uint64_t length) { return span(c, address, length, 2); }
uint32_t rt_bounded_strnlen(CPU *c, uint32_t address, uint32_t limit) {
    uint32_t n = 0;
    while (n < limit) {
        uint32_t at = address + n;
        uint64_t chunk = (((uint64_t)at | 4095) + 1) - at;   // jusqu'à la fin de la page de 4 Kio
        if (chunk > limit - n) chunk = limit - n;
        const uint8_t *page = rt_span(c, at, chunk), *nul = memchr(page, 0, (size_t)chunk);
        if (nul) return n + (uint32_t)(nul - page);
        n += (uint32_t)chunk;
    }
    return limit;
}
int rt_read_timespec(CPU *c, uint32_t pointer, struct timespec *out) {
    rt_span(c, pointer, 8);
    out->tv_sec = (int32_t)R32(pointer); out->tv_nsec = (int32_t)R32(pointer + 4);
    return out->tv_nsec < 0 || out->tv_nsec >= 1000000000 ? -1 : 0;
}
uint32_t rt_string_length(CPU *c, uint32_t address) {
    if (!address) rt_trap(c, c->eip, "null guest string");
    // memchr par page invitée de 4 Kio, chacune contrôlée avant d'être lue :
    // jamais au-delà de la page qui contient le NUL.
    for (uint64_t pos = address; pos < (UINT64_C(1) << 32);) {
        uint64_t chunk = ((pos | 4095) + 1) - pos;
        const uint8_t *page = rt_span(c, (uint32_t)pos, chunk), *nul = memchr(page, 0, (size_t)chunk);
        if (nul) return (uint32_t)(pos - address) + (uint32_t)(nul - page);
        pos += chunk;
    }
    rt_trap(c, c->eip, "unterminated guest string");
}
const char *rt_guest_cstr(CPU *c, uint32_t address) { rt_string_length(c, address); return GPTR(address); }
const char *rt_guest_cstr_or_null(CPU *c, uint32_t address) { return address ? rt_guest_cstr(c, address) : NULL; }
uint32_t rt_guest_strdup(const char *text) {
    if (!text) return 0;
    size_t length = strlen(text) + 1;
    uint32_t address = length <= UINT32_MAX ? rt_alloc((uint32_t)length) : 0;
    if (address) memcpy(GPTR(address), text, length);
    return address;
}
uint32_t rt_guest_strdup_or_trap(CPU *c, const char *text) {
    uint32_t address = rt_guest_strdup(text);
    if (text && !address) rt_trap(c, c->eip, "guest string allocation failed");
    return address;
}
__attribute__((noreturn, format(printf, 2, 3))) void rt_trapf(CPU *c, const char *format, ...) {
    static _Thread_local char message[512];
    va_list list;
    va_start(list, format);
    vsnprintf(message, sizeof(message), format, list);
    va_end(list);
    rt_trap(c, c->eip, message);
}
int rt_env_flag(const char *name) {
    const char *value = getenv(name);
    return value != NULL && strcmp(value, "0") != 0;
}
// strcmp sans lire les deux chaînes en entier : morceaux bornés par la fin de
// page de chacune, comparés jusqu'au premier écart ou NUL. Résultat de
// strcmp hôte (Darwin) : différence des octets non signés au premier écart.
static int32_t guest_strcmp(CPU *c, uint32_t a, uint32_t b) {
    if (!a || !b) rt_trap(c, c->eip, "null guest string");
    for (uint64_t done = 0; done < (UINT64_C(1) << 32);) {
        uint64_t at = a + done, bt = b + done;
        if (at >= (UINT64_C(1) << 32) || bt >= (UINT64_C(1) << 32)) break;
        uint64_t ka = ((at | 4095) + 1) - at, kb = ((bt | 4095) + 1) - bt, k = ka < kb ? ka : kb;
        const uint8_t *pa = rt_span(c, (uint32_t)at, k), *pb = rt_span(c, (uint32_t)bt, k);
        const uint8_t *nul = memchr(pa, 0, (size_t)k);
        size_t n = nul ? (size_t)(nul - pa) + 1 : (size_t)k;
        if (memcmp(pa, pb, n)) {
            size_t i = 0;
            while (pa[i] == pb[i]) ++i;
            return (int32_t)pa[i] - (int32_t)pb[i];
        }
        if (nul) return 0;
        done += k;
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
    c->eax = (uint32_t)guest_strcmp(c, arg(c, 0), arg(c, 1));
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
        uint8_t *to = rt_write_span(c, a, n);
        uint32_t i = rt_bounded_strnlen(c, b, n);
        memmove(to, GPTR(b), i);
        memset(to + i, 0, n - i);
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
int rt_pthread_resolve(const char *name, RtShimRoute *route);   // pthreads.c
static const rt_shim_resolver builtin_resolvers[BUILTIN] = {core_resolve, NULL, rt_pthread_resolve, rt_libc_resolve};
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
