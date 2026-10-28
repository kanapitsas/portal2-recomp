#include "rt.h"
#include "posix.h"
#include <dlfcn.h>
#include <mach/mach.h>
#include <mach/mach_vm.h>
#include <execinfo.h>
#include <pthread.h>
#include <signal.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

uint8_t *M;
static const uint64_t address_space = UINT64_C(1) << 32;
// Page de garde sans droit juste après les 4 Go invités : un accès de
// plusieurs octets commençant sous 4 Go et débordant (sans contrôle logiciel,
// RT_FAST_MEMORY) y fait une faute au lieu de toucher une autre projection.
static const uint64_t guard_size = UINT64_C(1) << 16;
typedef struct { uint32_t target, caller; } GuestFrame;
static _Thread_local GuestFrame frames[256];
static _Thread_local unsigned frame_depth;
_Thread_local CPU *rt_fault_cpu;   // diagnostic : CPU invité du thread (rt_invoke)
static void guest_backtrace(void) {
    unsigned count=frame_depth<256?frame_depth:256;
    unsigned first=count>16?count-16:0;
    for(unsigned i=count;i>first;--i)
        fprintf(stderr,"  guest call %#x, return %#x\n",frames[i-1].target,frames[i-1].caller);
}

// Faute hôte (SIGSEGV/SIGBUS) : adresse invitée fautive, fonction générée en
// cours (symbole fn_<adresse invitée>) et pile d'appels invitée, puis le
// signal par défaut. Diagnostic seulement ; n'essaie pas de reprendre.
static void host_fault(int signal, siginfo_t *info, void *context) {
    uintptr_t address = (uintptr_t)info->si_addr, base = (uintptr_t)M;
    char line[160];
    int n;
    if (M && address >= base && address - base < address_space)
        n = snprintf(line, sizeof(line), "host %s on guest address %#x\n",
                     signal == SIGBUS ? "SIGBUS" : "SIGSEGV", (unsigned)(address - base));
    else
        n = snprintf(line, sizeof(line), "host %s on host address %#lx\n",
                     signal == SIGBUS ? "SIGBUS" : "SIGSEGV", (unsigned long)address);
    if (n > 0) write(2, line, (size_t)n);
    if (M && address >= base && address - base < address_space) {
        n = rt_vm_describe((uint32_t)(address - base), line, sizeof(line));
        if (n > 0) write(2, line, (size_t)n);
    }
#if defined(__aarch64__)
    ucontext_t *uc = context;
    void *pc = (void *)uc->uc_mcontext->__ss.__pc;
    Dl_info where;
    if (dladdr(pc, &where) && where.dli_sname) {
        n = snprintf(line, sizeof(line), "  in %s + %#lx\n", where.dli_sname,
                     (unsigned long)((uintptr_t)pc - (uintptr_t)where.dli_saddr));
        if (n > 0) write(2, line, (size_t)n);
    }
#else
    (void)context;
#endif
    fflush(NULL);
    void *host_frames[24];
    int depth = backtrace(host_frames, 24);
    backtrace_symbols_fd(host_frames, depth, 2);
    if (rt_fault_cpu) {
        CPU *g = rt_fault_cpu;
        n = snprintf(line, sizeof(line), "  guest eax=%#x ecx=%#x edx=%#x ebx=%#x esp=%#x ebp=%#x esi=%#x edi=%#x\n",
                     g->eax, g->ecx, g->edx, g->ebx, g->esp, g->ebp, g->esi, g->edi);
        if (n > 0) write(2, line, (size_t)n);
    }
    guest_backtrace();
    struct sigaction fallback = {0};
    fallback.sa_handler = SIG_DFL;
    sigaction(signal, &fallback, NULL);
    raise(signal);
}

static void install_fault_handlers(void) {
    static char alternate[65536];
    stack_t stack = {.ss_sp = alternate, .ss_size = sizeof(alternate)};
    sigaltstack(&stack, NULL);
    struct sigaction action = {0};
    action.sa_sigaction = host_fault;
    action.sa_flags = SA_SIGINFO | SA_ONSTACK;
    sigemptyset(&action.sa_mask);
    sigaction(SIGSEGV, &action, NULL);
    sigaction(SIGBUS, &action, NULL);
}

int rt_init(void) {
    if (M) return -1;
    install_fault_handlers();
    void *p = mmap(NULL, (size_t)(address_space + guard_size), PROT_NONE,
                   MAP_PRIVATE | MAP_ANON, -1, 0);
    if (p == MAP_FAILED) { perror("guest address space"); return -1; }
    M = p;
    // RT_RETURN_SENTINEL is the return address of every outermost guest
    // frame. Unwinders (libgcc's fallback for frames without FDE) read the
    // code at a return address to detect signal trampolines: back it with a
    // readable page of HLT bytes, which matches no trampoline and has no
    // unwind data, so stack walks end there as at the bottom of a process.
    if (rt_map(RT_RETURN_SENTINEL & ~UINT32_C(0xfff), 0x1000)) { perror("guest sentinel page"); return -1; }
    memset(M + (RT_RETURN_SENTINEL & ~UINT32_C(0xfff)), 0xf4, 0x1000);
    return 0;
}

void rt_native_destroy(void);   // native.c
void rt_destroy(void) {
    rt_shims_destroy();
    rt_native_destroy();
    if (M) munmap(M, (size_t)(address_space + guard_size));
    M = NULL;
}

int rt_map(uint32_t addr, uint64_t size) {
    if (!M || !size || size > address_space - addr) return -1;
    uint64_t page = (uint64_t)sysconf(_SC_PAGESIZE);
    uint64_t lo = addr & ~(page - 1);
    uint64_t hi = ((uint64_t)addr + size + page - 1) & ~(page - 1);
    return mprotect(M + lo, (size_t)(hi - lo), PROT_READ | PROT_WRITE);
}

int rt_load_segment(const char *directory, const char *name, uint32_t offset,
                    uint32_t addr, uint32_t filesz, uint32_t memsz) {
    if (filesz > memsz || !memsz) return -1;
    char path[4096];
    int n = snprintf(path, sizeof(path), "%s/%s", directory, name);
    if (n < 0 || (size_t)n >= sizeof(path)) return -1;
    FILE *file = fopen(path, "rb");
    if (!file) { perror(path); return -1; }
    int failed = fseek(file, (long)offset, SEEK_SET) || rt_map(addr, memsz);
    if (!failed) {
        failed = fread(M + addr, 1, filesz, file) != filesz;
        if (!failed) memset(M + (uint64_t)addr + filesz, 0, memsz - filesz);
    }
    fclose(file);
    return failed ? -1 : 0;
}

__attribute__((noreturn)) void rt_trap(CPU *c, uint32_t pc, const char *why) {
    fflush(NULL);
    fprintf(stderr, "guest trap at %#x (esp=%#x): %s\n", pc, c->esp, why);
    guest_backtrace();
    abort();
}

// CPU invité du shim en cours sur ce thread hôte : un rappel natif (par ex.
// un SDL_EventWatch déclenché pendant un appel SDL) s'en sert pour exécuter
// une fonction invitée sur le même thread et la même pile.
static _Thread_local CPU *shim_cpu;
CPU *rt_shim_cpu(void) { return shim_cpu; }

// Cache d'aiguillage des imports (statiques 0xf0000000+, dynamiques
// 0xf8000000+, pas de 16) : pour chaque adresse, le nom et la route du
// fournisseur qui l'a accepté (rt_shim_route). Les entrées sont immuables,
// publiées par échange atomique et jamais libérées (les noms sont statiques
// ou conservés par bridge.c). Un nom inconnu n'est jamais mis en cache.
#define ROUTE_BASE 0xf0000000u
#define ROUTE_CHUNK_BITS 16
typedef struct { const char *name; RtShimRoute route; _Atomic uint64_t calls; } ImportRoute;
// Diagnostic P2_IMPORT_COUNTS=1 : nombre d'appels par import (route mémorisée),
// les 40 premiers écrits toutes les 10 s sur stderr.
static int import_counts = -1;
static ImportRoute *_Atomic counted_routes[8192];
static _Atomic unsigned counted_count;
static void *import_counts_writer(void *unused) {
    (void)unused;
    static uint64_t previous[8192];
    for (;;) {
        sleep(10);
        unsigned n = atomic_load(&counted_count); if (n > 8192) n = 8192;
        uint64_t best[40] = {0}; unsigned which[40] = {0}; uint64_t total = 0;
        for (unsigned i = 0; i < n; ++i) {
            ImportRoute *r = atomic_load(&counted_routes[i]); if (!r) continue;
            uint64_t now = atomic_load(&r->calls), delta = now - previous[i]; previous[i] = now; total += delta;
            for (int k = 0; k < 40; ++k) if (delta > best[k]) {
                memmove(&best[k + 1], &best[k], (39 - k) * sizeof(*best)); memmove(&which[k + 1], &which[k], (39 - k) * sizeof(*which));
                best[k] = delta; which[k] = i; break;
            }
        }
        fprintf(stderr, "[imports] %llu appels en 10 s :", (unsigned long long)total);
        for (int k = 0; k < 40 && best[k]; ++k)
            fprintf(stderr, " %s=%llu", atomic_load(&counted_routes[which[k]])->name, (unsigned long long)best[k]);
        fputc('\n', stderr);
    }
    return NULL;
}
static void count_route(ImportRoute *entry) {
    if (import_counts < 0) {
        import_counts = getenv("P2_IMPORT_COUNTS") != NULL;
        if (import_counts) { pthread_t t; pthread_create(&t, NULL, import_counts_writer, NULL); pthread_detach(t); }
    }
    if (!import_counts) return;
    unsigned i = atomic_fetch_add(&counted_count, 1);
    if (i < 8192) atomic_store(&counted_routes[i], entry);
}
typedef ImportRoute *_Atomic RouteSlot;
static RouteSlot *_Atomic route_chunks[((UINT64_C(1) << 32) - ROUTE_BASE) >> (4 + ROUTE_CHUNK_BITS)];

static RouteSlot *route_slot(uint32_t target, int create) {
    if (target < ROUTE_BASE || (target & 15)) return NULL;
    uint32_t index = (target - ROUTE_BASE) >> 4;
    RouteSlot *_Atomic *chunk = &route_chunks[index >> ROUTE_CHUNK_BITS];
    RouteSlot *slots = atomic_load_explicit(chunk, memory_order_acquire);
    if (!slots && create) {
        RouteSlot *fresh = calloc((size_t)1 << ROUTE_CHUNK_BITS, sizeof(*fresh));
        if (!fresh) return NULL;
        if (atomic_compare_exchange_strong_explicit(chunk, &slots, fresh, memory_order_acq_rel, memory_order_acquire))
            slots = fresh;
        else free(fresh);  // un autre thread l'a installé : slots le désigne
    }
    return slots ? &slots[index & ((1u << ROUTE_CHUNK_BITS) - 1)] : NULL;
}

static int run_shim(CPU *c, const char *name, const RtShimRoute *route, RtShimRoute *learned) {
    CPU *outer = shim_cpu;
    shim_cpu = c;
    int handled = route ? rt_route_call(route, c, name) : rt_shim_route(c, name, learned);
    shim_cpu = outer;
    return handled;
}

void rt_dispatch(CPU *c, uint32_t target) {
    rt_guest_poll();
    c->eip = target;
    gfn fn = target < RT_IMPORT_BASE ? rt_lookup(target) : NULL;
    // Pas une fonction des modules préparés : peut-être un greffon PE traduit
    // que libMiles a projeté à l'exécution (pe.c vérifie l'image).
    if (!fn && target < RT_IMPORT_BASE) fn = rt_pe_lookup(target);
    if (fn) { fn(c); return; }
    RouteSlot *slot = route_slot(target, 0);
    ImportRoute *cached = slot ? atomic_load_explicit(slot, memory_order_acquire) : NULL;
    // Route connue : même fournisseur que la chaîne complète (contrat dans
    // rt.h) ; s'il refuse malgré tout, la chaîne est reprise ci-dessous.
    if (cached && import_counts > 0) atomic_fetch_add_explicit(&cached->calls, 1, memory_order_relaxed);
    if (cached && run_shim(c, cached->name, &cached->route, NULL)) { rt_return(c); return; }
    const char *name = cached ? cached->name : rt_import_name(target);
    if (!name) name = rt_dynamic_import_name(target);
    if (name) {
        RtShimRoute route;
        if (run_shim(c, name, NULL, &route)) {
            if ((slot = route_slot(target, 1))) {
                ImportRoute *entry = malloc(sizeof(*entry));
                if (entry) {
                    *entry = (ImportRoute){name, route, 0};
                    count_route(entry);
                    ImportRoute *old = atomic_exchange_explicit(slot, entry, memory_order_acq_rel);
                    (void)old;  // éventuel lecteur concurrent : l'ancienne entrée reste valide, jamais libérée
                }
            }
            rt_return(c);
            return;
        }
    }
    rt_trap(c, target, name ? name : "target not recompiled");
}


void rt_call_import(CPU *c, uint32_t target, uint32_t expected_return) {
#ifndef RT_TRACED_CALLS
    RouteSlot *slot = route_slot(target, 0);
    ImportRoute *cached = slot ? atomic_load_explicit(slot, memory_order_acquire) : NULL;
    if (cached) {
        if (__builtin_expect(import_counts > 0, 0)) atomic_fetch_add_explicit(&cached->calls, 1, memory_order_relaxed);
        PUSH(c, expected_return);
        c->eip = target;
        if (run_shim(c, cached->name, &cached->route, NULL)) { rt_return(c); return; }
        c->esp += 4;   // refus sans effet (contrat dans rt.h) : chemin complet
    }
#endif
    rt_call(c, target, expected_return);
}

// Non-local control flow (longjmp, an unwinder installing a landing pad):
// the guest resumes at an address other than the return site of the call in
// progress. Each lifted call site compares c->eip with its own return site;
// on mismatch it either resumes at a known point of its function (setjmp
// return sites, see tools/lift.py) or returns, so host C frames are dropped
// one by one until a frame expects that address. Every C frame can run any
// guest code because all guest state lives in CPU and guest memory. Calls
// made by native code use RT_RETURN_SENTINEL and never let a transfer escape.
static _Thread_local struct { uint32_t from_target, expected, actual; unsigned depth; } nonlocal;

// Lisibilité réelle d'une adresse invitée (protection de la page hôte),
// pour les diagnostics seulement.
int rt_guest_readable(uint32_t address) {
    if (!M) return 0;
    mach_vm_address_t region = (mach_vm_address_t)(uintptr_t)(M + address);
    mach_vm_size_t size = 0;
    vm_region_basic_info_data_64_t info;
    mach_msg_type_number_t count = VM_REGION_BASIC_INFO_COUNT_64;
    mach_port_t object = MACH_PORT_NULL;
    kern_return_t kr = mach_vm_region(mach_task_self(), &region, &size, VM_REGION_BASIC_INFO_64,
                                      (vm_region_info_t)&info, &count, &object);
    return kr == KERN_SUCCESS && region <= (mach_vm_address_t)(uintptr_t)(M + address) &&
           (info.protection & VM_PROT_READ);
}

// Débogage : P2_WATCH_CALL=0x<adresse> affiche les 4 premiers arguments (et
// les chaînes qu'ils désignent) de chaque appel invité de cette adresse.
static uint32_t watched_call(void) {
    static uint32_t value = UINT32_MAX;
    if (value == UINT32_MAX) {
        const char *e = getenv("P2_WATCH_CALL");
        value = e ? (uint32_t)strtoul(e, NULL, 0) : 0;
    }
    return value;
}
static void report_watched_call(CPU *c, uint32_t target) {
    fprintf(stderr, "[watch] call %#x from %#x:", target, R32(c->esp));
    for (unsigned i = 1; i <= 4; ++i) {
        uint32_t v = R32(c->esp + 4 * i);
        fprintf(stderr, " %#x", v);
        if (v >= 0x10000000u && v < 0xe0000000u && rt_guest_readable(v)) {
            char text[121]; unsigned n = 0;
            while (n < 120 && rt_guest_readable(v + n) && M[v + n]) { text[n] = (char)M[v + n]; ++n; }
            text[n] = 0;
            if (n >= 2) fprintf(stderr, " \"%s\"", text);
        }
    }
    fputc('\n', stderr);
}

void rt_call(CPU *c, uint32_t target, uint32_t expected_return) {
    if (target == watched_call() && target) { PUSH(c, expected_return); report_watched_call(c, target); c->esp += 4; }
    rt_guest_enter();
    if(frame_depth<256)frames[frame_depth]=(GuestFrame){target,expected_return};
    ++frame_depth;
    PUSH(c, expected_return);
    rt_dispatch(c, target);
    if (c->eip != expected_return) {
        if (!nonlocal.actual || nonlocal.actual != c->eip)
            nonlocal = (typeof(nonlocal)){target, expected_return, c->eip, frame_depth};
        if (expected_return == RT_RETURN_SENTINEL) {
            fprintf(stderr, "non-local transfer to %#x (first seen returning from %#x, expected %#x, depth %u) "
                    "is not resumable by any recompiled frame\n", nonlocal.actual, nonlocal.from_target,
                    nonlocal.expected, nonlocal.depth);
            rt_trap(c, c->eip, "guest control left a native call boundary");
        }
    } else nonlocal.actual = 0;
    --frame_depth;
    rt_guest_leave();
}

uint32_t rt_exchange32(CPU *c, uint32_t address, uint32_t value) {
    if (address & 3) rt_trap(c, c->eip, "unaligned atomic exchange is unsupported");
    return __atomic_exchange_n((uint32_t *)(void *)(M + address), value, __ATOMIC_SEQ_CST);
}
uint32_t rt_compare_exchange32(CPU *c, uint32_t address, uint32_t expected, uint32_t value) {
    if (address & 3) rt_trap(c, c->eip, "unaligned atomic compare-exchange is unsupported");
    __atomic_compare_exchange_n((uint32_t *)(void *)(M + address), &expected, value,
                                0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
    return expected;
}

uint32_t rt_atomic32(CPU *c, uint32_t address, uint32_t value, int operation) {
    if (address & 3) rt_trap(c, c->eip, "unaligned atomic arithmetic is unsupported");
    uint32_t *pointer = (uint32_t *)(void *)(M + address);
    switch (operation) {
    case 0: return __atomic_fetch_add(pointer, value, __ATOMIC_SEQ_CST);
    case 1: return __atomic_fetch_sub(pointer, value, __ATOMIC_SEQ_CST);
    case 2: return __atomic_fetch_and(pointer, value, __ATOMIC_SEQ_CST);
    case 3: return __atomic_fetch_or(pointer, value, __ATOMIC_SEQ_CST);
    case 4: return __atomic_fetch_xor(pointer, value, __ATOMIC_SEQ_CST);
    default: rt_trap(c, c->eip, "unknown atomic arithmetic operation");
    }
}

uint64_t rt_compare_exchange64(CPU *c, uint32_t address, uint64_t expected, uint64_t value) {
    if (address & 7) rt_trap(c, c->eip, "unaligned 64-bit atomic compare-exchange is unsupported");
    __atomic_compare_exchange_n((uint64_t *)(void *)(M + address), &expected, value,
                                0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
    return expected;
}

__attribute__((noreturn)) void rt_memory_trap(uint32_t address, uint64_t length, int mode) {
    fprintf(stderr, "guest memory fault at %#x length=%llu access=%s\n", address,
            (unsigned long long)length, mode == 2 ? "write" : "read");
    char line[160];
    if (rt_vm_describe(address, line, sizeof(line)) > 0) fputs(line, stderr);
    guest_backtrace();
    abort();
}
void rt_check_span(uint32_t address, uint64_t length, int mode) {
    if (!length) return;
    uint64_t end = (uint64_t)address + length;
    if (end > (UINT64_C(1) << 32)) rt_memory_trap(address, length, mode);
    uint64_t lo = address < RT_VM_BEGIN ? RT_VM_BEGIN : address;
    uint64_t hi = end > RT_VM_END ? RT_VM_END : end;
    for (uint64_t page = lo & ~UINT64_C(4095); page < hi; page += 4096)
        if ((rt_vm_permissions[(page - RT_VM_BEGIN) >> 12] & mode) != mode)
            rt_memory_trap(address, length, mode);
}
