// Runtime commun au code recompilé : état du CPU invité, mémoire 32 bits, indicateurs.
#pragma once
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <stdatomic.h>

typedef union {
    uint8_t b[16]; uint16_t w[8]; uint32_t d[4]; int32_t i[4];
    uint64_t q[2]; float f[4]; double g[2];
} XMM;

// Indicateurs persistants : CALL/RET et les thunks PIC ne les modifient pas.
// pf stocke un octet dont la parité représente PF.
typedef struct { uint32_t cf, zf, sf, of, pf, af; } FL;

typedef struct CPU {
    FL flags;
    uint32_t eip;
    uint32_t gs_base;
    uint32_t eax, ecx, edx, ebx, esp, ebp, esi, edi;
    uint32_t df;          // indicateur de direction (0/1)
    uint32_t eflags_sys;  // bits écrits par popfd et non modélisés (ID, AC…)
    uint32_t ftop;        // sommet de la pile x87
    double st[8];
    uint16_t fcw, fsw;
    uint8_t ftags;
    uint32_t mxcsr;
    XMM xmm[8];
    uint32_t tid;         // index du thread invité
    uint64_t mm[8];       // MMX : registres distincts de st[] (x87 en double, voir float_lift.py)
} CPU;

extern uint8_t *M;   // base hôte de l'espace d'adressage invité (4 Go réservés)

// Les fonctions du runtime appelées par le code généré ne conservent pas le
// pointeur CPU après leur retour (shim_cpu et once_cpu sont restaurés).
// Sans cette garantie, clang considère `c` comme échappé dès le premier
// appel et suppose qu'une écriture invitée peut modifier *c.
#define RT_NOESCAPE __attribute__((noescape))

typedef void (*gfn)(CPU *);
void rt_dispatch(RT_NOESCAPE CPU *c, uint32_t target);                       // appelle le code invité à target
__attribute__((noreturn)) void rt_trap(RT_NOESCAPE CPU *c, uint32_t pc, const char *why);
void rt_div32(RT_NOESCAPE CPU *c, uint32_t divisor, int is_signed, uint32_t pc);
void rt_div_narrow(RT_NOESCAPE CPU *c, uint32_t divisor, int width, int is_signed, uint32_t pc);  // DIV/IDIV 8 et 16 bits
void rt_fprem(RT_NOESCAPE CPU *c, int ieee);                       // FPREM (0) / FPREM1 (1), bits C0-C3
double rt_load_extended(uint32_t address);             // flottant x87 80 bits -> double
void rt_store_extended(uint32_t address, double value);
double rt_sse_round(RT_NOESCAPE CPU *c, double value);
double rt_x87_round(RT_NOESCAPE CPU *c, double value);
void rt_cpu_reset(RT_NOESCAPE CPU *c);
void rt_fxsave(RT_NOESCAPE CPU *c, uint32_t address);
void rt_ldmxcsr(RT_NOESCAPE CPU *c, uint32_t value);
void rt_cpuid(RT_NOESCAPE CPU *c);
unsigned rt_guest_cpu_count(void);   // processeurs logiques annoncés (cpu.c)
uint64_t rt_rdtsc(void);
uint32_t rt_rand32(void);

#define GPTR(a) ((void *)(M + (uint32_t)(a)))

// Adresses des imports statiques (0xf0000000+) et dynamiques (0xf8000000+) :
// jamais du code traduit.
#define RT_IMPORT_BASE 0xf0000000u
#define RT_VM_BEGIN 0x21000000u  // au-dessus des modules (< 0x19000000) et de la zone de test 0x20000000
#define RT_VM_END 0x70000000u
extern uint8_t rt_vm_permissions[(RT_VM_END - RT_VM_BEGIN) / 4096];
__attribute__((noreturn)) void rt_memory_trap(uint32_t address, uint64_t length, int mode);
// RT_FAST_MEMORY : aucun contrôle logiciel par accès. La protection repose
// alors sur les pages hôtes de 16 Kio (vm.c leur donne l'union des droits des
// pages invitées de 4 Kio qu'elles contiennent) : un accès à une page invitée
// sans droit qui partage sa page hôte avec une page accessible n'est plus
// détecté (par ex. une page de garde de pile de 4 Kio). Un accès qui déborde
// 4 Go tombe dans la page de garde réservée après l'espace invité (rt.c).
// Toute autre faute reste une faute matérielle, traduite par host_fault.
static inline void rt_check_memory(uint32_t address, unsigned size, int mode) {
#ifdef RT_FAST_MEMORY
    (void)address; (void)size; (void)mode;
#else
    if (address > UINT32_MAX - (size - 1)) rt_memory_trap(address, size, mode);
    uint32_t last = address + size - 1;
    if (address >= RT_VM_BEGIN && address < RT_VM_END &&
        (rt_vm_permissions[(address - RT_VM_BEGIN) >> 12] & mode) != mode)
        rt_memory_trap(address, size, mode);
    if ((last >> 12) != (address >> 12) && last >= RT_VM_BEGIN && last < RT_VM_END &&
        (rt_vm_permissions[(last - RT_VM_BEGIN) >> 12] & mode) != mode)
        rt_memory_trap(address, size, mode);
#endif
}
void rt_check_span(uint32_t address, uint64_t length, int mode);
// Accès mémoire invités. La base vaut RT_MEM : M partout, mais le code
// généré la recopie dans une variable locale (`mem_`, voir tools/lift.py).
// Une écriture invitée passe par memcpy, sans type : clang devrait sinon
// supposer qu'elle peut modifier le pointeur global M et le relire après
// chaque écriture. Avec `CPU *restrict c` dans les fonctions générées
// (le CPU vit hors de la mémoire invitée), les registres invités restent
// dans des registres hôtes entre deux accès mémoire.
#ifndef RT_MEM
#define RT_MEM M
#endif
static inline uint8_t  rt_r8 (uint8_t *m, uint32_t a) { rt_check_memory(a, 1, 1); return m[a]; }
static inline uint16_t rt_r16(uint8_t *m, uint32_t a) { rt_check_memory(a, 2, 1); uint16_t v; memcpy(&v, m + a, 2); return v; }
static inline uint32_t rt_r32(uint8_t *m, uint32_t a) { rt_check_memory(a, 4, 1); uint32_t v; memcpy(&v, m + a, 4); return v; }
static inline uint64_t rt_r64(uint8_t *m, uint32_t a) { rt_check_memory(a, 8, 1); uint64_t v; memcpy(&v, m + a, 8); return v; }
static inline float    rt_rf (uint8_t *m, uint32_t a) { rt_check_memory(a, 4, 1); float v; memcpy(&v, m + a, 4); return v; }
static inline double   rt_rd (uint8_t *m, uint32_t a) { rt_check_memory(a, 8, 1); double v; memcpy(&v, m + a, 8); return v; }
static inline XMM      rt_rx (uint8_t *m, uint32_t a) { rt_check_memory(a, 16, 1); XMM v; memcpy(&v, m + a, 16); return v; }
static inline void rt_w8 (uint8_t *m, uint32_t a, uint32_t v) { rt_check_memory(a, 1, 2); m[a] = (uint8_t)v; }
static inline void rt_w16(uint8_t *m, uint32_t a, uint32_t v) { rt_check_memory(a, 2, 2); uint16_t x = (uint16_t)v; memcpy(m + a, &x, 2); }
static inline void rt_w32(uint8_t *m, uint32_t a, uint32_t v) { rt_check_memory(a, 4, 2); memcpy(m + a, &v, 4); }
static inline void rt_w64(uint8_t *m, uint32_t a, uint64_t v) { rt_check_memory(a, 8, 2); memcpy(m + a, &v, 8); }
static inline void rt_wf (uint8_t *m, uint32_t a, float v)    { rt_check_memory(a, 4, 2); memcpy(m + a, &v, 4); }
static inline void rt_wd (uint8_t *m, uint32_t a, double v)   { rt_check_memory(a, 8, 2); memcpy(m + a, &v, 8); }
static inline void rt_wx (uint8_t *m, uint32_t a, XMM v)      { rt_check_memory(a, 16, 2); memcpy(m + a, &v, 16); }
#define R8(a)  rt_r8 (RT_MEM, (a))
#define R16(a) rt_r16(RT_MEM, (a))
#define R32(a) rt_r32(RT_MEM, (a))
#define R64(a) rt_r64(RT_MEM, (a))
#define RF(a)  rt_rf (RT_MEM, (a))
#define RD(a)  rt_rd (RT_MEM, (a))
#define RX(a)  rt_rx (RT_MEM, (a))
#define W8(a, v)  rt_w8 (RT_MEM, (a), (v))
#define W16(a, v) rt_w16(RT_MEM, (a), (v))
#define W32(a, v) rt_w32(RT_MEM, (a), (v))
#define W64(a, v) rt_w64(RT_MEM, (a), (v))
#define WF(a, v)  rt_wf (RT_MEM, (a), (v))
#define WD(a, v)  rt_wd (RT_MEM, (a), (v))
#define WX(a, v)  rt_wx (RT_MEM, (a), (v))

// Macros (et non fonctions) pour utiliser la base locale du code généré.
#define PUSH(cpu, v) do { CPU *push_cpu_ = (cpu); uint32_t push_value_ = (v); \
    push_cpu_->esp -= 4; W32(push_cpu_->esp, push_value_); } while (0)
#define POP(cpu) ({ CPU *pop_cpu_ = (cpu); uint32_t pop_value_ = R32(pop_cpu_->esp); \
    pop_cpu_->esp += 4; pop_value_; })

#define MSK(n) ((n) == 32 ? 0xffffffffu : ((1u << (n)) - 1))
#define SGN(n) (1u << ((n) - 1))
static inline uint32_t sx(uint32_t v, int n) { return n == 32 ? v : (uint32_t)((int32_t)(v << (32 - n)) >> (32 - n)); }

static inline void szp(FL *f, uint32_t r, int n) { r &= MSK(n); f->zf = r == 0; f->sf = (r >> (n - 1)) & 1; f->pf = r & 0xff; }
static inline uint32_t f_add(FL *f, uint32_t a, uint32_t b, int n) {
    a &= MSK(n); b &= MSK(n); uint64_t r = (uint64_t)a + b; uint32_t rr = (uint32_t)r & MSK(n);
    f->cf = (uint32_t)(r >> n) & 1; f->of = (((a ^ rr) & (b ^ rr)) >> (n - 1)) & 1; f->af = ((a ^ b ^ rr) >> 4) & 1; szp(f, rr, n); return rr;
}
static inline uint32_t f_adc(FL *f, uint32_t a, uint32_t b, int n) {
    a &= MSK(n); b &= MSK(n); uint64_t r = (uint64_t)a + b + f->cf; uint32_t rr = (uint32_t)r & MSK(n);
    f->cf = (uint32_t)(r >> n) & 1; f->of = (((a ^ rr) & (b ^ rr)) >> (n - 1)) & 1; f->af = ((a ^ b ^ rr) >> 4) & 1; szp(f, rr, n); return rr;
}
static inline uint32_t f_sub(FL *f, uint32_t a, uint32_t b, int n) {
    a &= MSK(n); b &= MSK(n); uint32_t rr = (a - b) & MSK(n);
    f->cf = a < b; f->of = (((a ^ b) & (a ^ rr)) >> (n - 1)) & 1; f->af = ((a ^ b ^ rr) >> 4) & 1; szp(f, rr, n); return rr;
}
static inline uint32_t f_sbb(FL *f, uint32_t a, uint32_t b, int n) {
    a &= MSK(n); b &= MSK(n); uint32_t ci = f->cf; uint32_t rr = (a - b - ci) & MSK(n);
    f->cf = (uint64_t)a < (uint64_t)b + ci; f->of = (((a ^ b) & (a ^ rr)) >> (n - 1)) & 1; f->af = ((a ^ b ^ rr) >> 4) & 1; szp(f, rr, n); return rr;
}
static inline uint32_t f_logic(FL *f, uint32_t r, int n) { f->cf = 0; f->of = 0; szp(f, r, n); return r & MSK(n); }
static inline uint32_t f_inc(FL *f, uint32_t a, int n) { uint32_t r = (a + 1) & MSK(n); f->of = r == SGN(n); f->af = ((a ^ 1u ^ r) >> 4) & 1; szp(f, r, n); return r; }
static inline uint32_t f_dec(FL *f, uint32_t a, int n) { a &= MSK(n); uint32_t r = (a - 1) & MSK(n); f->of = a == SGN(n); f->af = ((a ^ 1u ^ r) >> 4) & 1; szp(f, r, n); return r; }
static inline uint32_t f_neg(FL *f, uint32_t a, int n) { a &= MSK(n); uint32_t r = (0 - a) & MSK(n); f->cf = a != 0; f->of = a == SGN(n); f->af = ((a ^ r) >> 4) & 1; szp(f, r, n); return r; }
static inline uint32_t f_shl(FL *f, uint32_t a, uint32_t cnt, int n) {
    cnt &= 31; a &= MSK(n); if (!cnt) return a;
    uint64_t r = (uint64_t)a << cnt; uint32_t rr = (uint32_t)r & MSK(n);
    f->cf = cnt <= (uint32_t)n ? (uint32_t)(r >> n) & 1 : 0; f->of = ((rr >> (n - 1)) & 1) ^ f->cf; szp(f, rr, n); return rr;
}
static inline uint32_t f_shr(FL *f, uint32_t a, uint32_t cnt, int n) {
    cnt &= 31; a &= MSK(n); if (!cnt) return a;
    uint32_t rr = cnt >= 32 ? 0 : a >> cnt; f->cf = (uint32_t)(((uint64_t)a >> (cnt - 1)) & 1); f->of = (a >> (n - 1)) & 1; szp(f, rr, n); return rr;
}
static inline uint32_t f_sar(FL *f, uint32_t a, uint32_t cnt, int n) {
    cnt &= 31; int32_t s = (int32_t)sx(a & MSK(n), n); if (!cnt) return a & MSK(n);
    uint32_t rr = (uint32_t)(s >> cnt) & MSK(n); f->cf = (uint32_t)(s >> (cnt - 1)) & 1; f->of = 0; szp(f, rr, n); return rr;
}
static inline uint32_t f_rotate(FL *f, uint32_t value, uint32_t count, int n, int kind) {
    value &= MSK(n);
    uint32_t masked = count & 31;
    if (!masked) return value;
    unsigned bits = (unsigned)n + (kind >= 2);
    unsigned amount = masked % bits;
    uint64_t ring = value | (kind >= 2 ? (uint64_t)f->cf << n : 0);
    uint64_t mask = (UINT64_C(1) << bits) - 1;
    if (amount) {
        if (kind == 0 || kind == 2) ring = ((ring << amount) | (ring >> (bits - amount))) & mask;
        else ring = ((ring >> amount) | (ring << (bits - amount))) & mask;
    }
    uint32_t result = (uint32_t)ring & MSK(n);
    if (kind >= 2) f->cf = (uint32_t)(ring >> n) & 1;
    else f->cf = kind == 0 ? result & 1 : result >> (n - 1);
    if (masked == 1) f->of = kind == 0 || kind == 2 ? (result >> (n - 1)) ^ f->cf : ((result >> (n - 1)) ^ (result >> (n - 2))) & 1;
    return result;
}
static inline uint32_t f_imul(FL *f, uint32_t a, uint32_t b, int n) {
    int64_t r = (int64_t)(int32_t)sx(a, n) * (int64_t)(int32_t)sx(b, n); uint32_t rr = (uint32_t)r & MSK(n);
    f->cf = f->of = r != (int64_t)(int32_t)sx(rr, n); szp(f, rr, n); return rr;
}
static inline int parity(uint32_t v) { return !__builtin_parity(v & 0xff); }

enum { CC_O, CC_NO, CC_B, CC_AE, CC_E, CC_NE, CC_BE, CC_A, CC_S, CC_NS, CC_P, CC_NP, CC_L, CC_GE, CC_LE, CC_G };
static inline int cond(const FL *f, int cc) {
    switch (cc) {
    case CC_O: return f->of; case CC_NO: return !f->of;
    case CC_B: return f->cf; case CC_AE: return !f->cf;
    case CC_E: return f->zf; case CC_NE: return !f->zf;
    case CC_BE: return f->cf | f->zf; case CC_A: return !(f->cf | f->zf);
    case CC_S: return f->sf; case CC_NS: return !f->sf;
    case CC_P: return parity(f->pf); case CC_NP: return !parity(f->pf);
    case CC_L: return f->sf != f->of; case CC_GE: return f->sf == f->of;
    case CC_LE: return f->zf || f->sf != f->of; default: return !f->zf && f->sf == f->of;
    }
}
// Comparaison scalaire SSE (comisd/ucomisd) : ZF, PF, CF ; OF=SF=0.
static inline void f_comi(FL *f, double a, double b) {
    f->of = f->sf = f->af = 0;
    if (a != a || b != b) { f->zf = 1; f->cf = 1; f->pf = 0; }      // non ordonné : PF=1
    else { f->zf = a == b; f->cf = a < b; f->pf = 1; }               // PF=0
}
static inline uint32_t get_eflags(CPU *c, const FL *f) {
    return f->cf | (parity(f->pf) << 2) | (f->af << 4) | (f->zf << 6) | (f->sf << 7) | (c->df << 10) | (f->of << 11) | 0x202 | c->eflags_sys;
}
static inline void set_eflags(CPU *c, FL *f, uint32_t v) {
    f->cf = v & 1; f->af = (v >> 4) & 1; f->pf = (v >> 2) & 1 ? 0 : 1; f->zf = (v >> 6) & 1; f->sf = (v >> 7) & 1;
    c->df = (v >> 10) & 1; f->of = (v >> 11) & 1; c->eflags_sys = v & 0x00240000u;  // ID (21) et AC (18)
}

// Pile x87, modélisée en double (pas de 80 bits : aucun accès tbyte dans ces modules).
#define ST(i) (c->st[(c->ftop + (i)) & 7])
static inline void fpush(CPU *c, double v) { c->ftop = (c->ftop - 1) & 7; c->st[c->ftop] = v; c->ftags |= 1u << c->ftop; }
static inline double fpop(CPU *c) { double v = c->st[c->ftop]; c->ftags &= ~(1u << c->ftop); c->ftop = (c->ftop + 1) & 7; return v; }

static inline int32_t cvt_trunc32(double v) { return (v != v || v >= 2147483648.0 || v < -2147483648.0) ? INT32_MIN : (int32_t)v; }
static inline int64_t cvt_trunc64(double v) { return (v != v || v >= 9223372036854775808.0 || v < -9223372036854775808.0) ? INT64_MIN : (int64_t)v; }
static inline int32_t cvt_round32(double v) { return cvt_trunc32(nearbyint(v)); }

// Chargement et pont d'appels invités.
int rt_init(void);
void rt_destroy(void);
int rt_map(uint32_t addr, uint64_t size);
uint32_t rt_mmap(CPU *c, uint32_t hint, uint32_t length, int protection, uint32_t flags, int fd, uint64_t offset);
int rt_munmap(uint32_t address, uint32_t length);
int rt_mprotect(uint32_t address, uint32_t length, int protection);
int rt_madvise(uint32_t address, uint32_t length, int advice);
void rt_vm_destroy(void);
int rt_load_segment(const char *directory, const char *name, uint32_t offset,
                    uint32_t addr, uint32_t filesz, uint32_t memsz);
int rt_init_import_data(const char *name, uint32_t address, uint32_t size);
int rt_time_data_import(const char *name, uint32_t address);  // time.c
int rt_load_images(const char *directory);  // généré par lift.py
uint32_t rt_export(const char *name);       // 0 si export non recompilé
gfn rt_lookup(uint32_t target);
uint32_t rt_atomic32(RT_NOESCAPE CPU *c, uint32_t address, uint32_t value, int operation);
uint32_t rt_exchange32(RT_NOESCAPE CPU *c, uint32_t address, uint32_t value);
uint32_t rt_compare_exchange32(RT_NOESCAPE CPU *c, uint32_t address, uint32_t expected, uint32_t value);
uint64_t rt_compare_exchange64(RT_NOESCAPE CPU *c, uint32_t address, uint64_t expected, uint64_t value);
// Retour d'une fonction traduite : macro (pas d'appel par retour, base
// mémoire locale du code généré).
#define rt_return(cpu) ((void)((cpu)->eip = POP(cpu)))
void rt_call(RT_NOESCAPE CPU *c, uint32_t target, uint32_t expected_return);
void rt_guest_poll(void);
// Threads bloqués sur le verrou invité (scheduler.c). Le code généré ne cède
// que s'il y en a : test en ligne, sans appel ni accès _Thread_local.
extern _Atomic unsigned rt_guest_waiting;
static inline void rt_poll(void) {
    if (__builtin_expect(atomic_load_explicit(&rt_guest_waiting, memory_order_relaxed) != 0, 0)) rt_guest_poll();
}
// Appel et saut directs du code généré vers une fonction traduite connue
// (tools/lift.py, link_direct). Hors RT_TRACED_CALLS, ni verrou invité (le
// code généré le tient déjà), ni pile d'appels de diagnostic, ni
// P2_WATCH_CALL, ni recherche de la cible : l'adresse de retour est empilée
// et la fonction appelée comme le ferait rt_dispatch. Le protocole des
// transferts non locaux est inchangé : l'appelant compare c->eip à son
// adresse de retour. Chaque chemin rapide cède le verrou comme rt_dispatch
// (rt_poll) : un cycle d'appels ou de sauts terminaux entre fonctions, sans
// branche arrière, ne doit pas affamer les autres threads invités.
static inline void rt_call_fn(CPU *c, gfn fn, uint32_t target, uint32_t expected_return) {
#ifdef RT_TRACED_CALLS
    (void)fn; rt_call(c, target, expected_return);
#else
    rt_poll(); PUSH(c, expected_return); c->eip = target; fn(c);
#endif
}
// Appels et sauts indirects (fonctions virtuelles, pointeurs de fonction) :
// une cible traduite est appelée directement ; toute autre (import, adresse
// dynamique, cible inconnue) prend le chemin complet de rt_call/rt_dispatch.
// Chaque site d'appel garde sa dernière cible traduite (les appels virtuels
// sont presque toujours monomorphes), ce qui évite la recherche de rt_lookup.
// Sans atomique : le code généré s'exécute sous le verrou invité.
typedef struct { uint32_t target; gfn fn; } RtSiteCache;
static inline gfn rt_site_lookup(RtSiteCache *site, uint32_t target) {
    if (__builtin_expect(site->target == target && site->fn != NULL, 1)) return site->fn;
    gfn fn = target < RT_IMPORT_BASE ? rt_lookup(target) : NULL;
    if (fn) { site->target = target; site->fn = fn; }
    return fn;
}
static inline void rt_call_any_site(CPU *c, uint32_t target, uint32_t expected_return, RtSiteCache *site) {
#ifndef RT_TRACED_CALLS
    gfn fn = rt_site_lookup(site, target);
    if (fn) { rt_poll(); PUSH(c, expected_return); c->eip = target; fn(c); return; }
#else
    (void)site;
#endif
    rt_call(c, target, expected_return);
}
static inline void rt_jump_any_site(CPU *c, uint32_t target, RtSiteCache *site) {
#ifndef RT_TRACED_CALLS
    gfn fn = rt_site_lookup(site, target);
    if (fn) { rt_poll(); c->eip = target; fn(c); return; }
#else
    (void)site;
#endif
    rt_dispatch(c, target);
}
#define rt_call_any(c, target, expected_return) \
    do { static RtSiteCache rt_site_; rt_call_any_site((c), (target), (expected_return), &rt_site_); } while (0)
#define rt_jump_any(c, target) \
    do { static RtSiteCache rt_site_; rt_jump_any_site((c), (target), &rt_site_); } while (0)
// Appel direct d'un import (shim) depuis le code généré, qui tient déjà le
// verrou invité : route mémorisée appelée directement, sinon rt_call.
void rt_call_import(RT_NOESCAPE CPU *c, uint32_t target, uint32_t expected_return);
static inline void rt_jump_fn(CPU *c, gfn fn, uint32_t target) {
#ifdef RT_TRACED_CALLS
    (void)fn; rt_dispatch(c, target);
#else
    rt_poll(); c->eip = target; fn(c);
#endif
}
const char *rt_import_name(uint32_t target);
int rt_shim(CPU *c, const char *name);
int rt_guest_readable(uint32_t address);   // diagnostic : page hôte lisible
CPU *rt_shim_cpu(void);   // CPU du shim en cours sur ce thread, NULL hors shim
void *rt_span(CPU *c, uint32_t address, uint64_t length);
void *rt_write_span(CPU *c, uint32_t address, uint64_t length);
uint32_t rt_string_length(CPU *c, uint32_t address);
int rt_register_tls(const char *directory, const char *name, uint32_t id, uint32_t offset, uint32_t filesz, uint32_t memsz, uint32_t alignment);
uint32_t rt_thread_pointer(RT_NOESCAPE CPU *c);
uint32_t rt_tls_address(RT_NOESCAPE CPU *c, uint32_t module, uint32_t offset);
void rt_tls_destroy(void);
void rt_tls_release(uint32_t thread);
int rt_loader_shim(CPU *c, const char *name);
void rt_modules_destroy(void);
int rt_scan_shim(CPU *c, const char *name);
int rt_libc_shim(CPU *c, const char *name);
void rt_libc_destroy(void);
uint32_t rt_standard_stream(int index);
void *rt_stream_file(CPU *c, uint32_t handle);
int rt_format_shim(CPU *c, const char *name);
int rt_pthread_shim(CPU *c, const char *name);
void rt_pthreads_destroy(void);
uint32_t rt_alloc(uint32_t size);
uint32_t rt_realloc(uint32_t ptr, uint32_t size);
int rt_free(uint32_t ptr);
uint32_t rt_heap_usable_size(uint32_t ptr);   // 0 si ptr ne vient pas du tas invité
void rt_heap_reset(void);
void rt_shims_destroy(void);
size_t rt_atexit_count(void);
int rt_atexit_get(size_t index, uint32_t *function, uint32_t *arg, uint32_t *dso);  // function 0: already run
void rt_atexit_clear(size_t index);
#define RT_RETURN_SENTINEL 0xfffffff0u

int rt_paths_init(const char *game, const char *linux, const char *user);
const char *rt_read_path(const char *path);
int rt_is_proc_path(const char *path);
const char *rt_proc_path(const char *path);          // NULL + errno si non émulé
void rt_procfs_set_cmdline(const char *bytes, size_t size);
const char *rt_write_path(const char *path, int preserve_existing);
const char *rt_program_path(void);
int rt_stat_shim(CPU *c, const char *name);
void rt_set_errno(CPU *c, int value);
void rt_save_errno(CPU *c);

void rt_guest_enter(void);
void rt_guest_leave(void);
unsigned rt_guest_suspend(void);
void rt_guest_resume(unsigned saved);
void rt_guest_poll(void);
int rt_thread_shim(CPU *c, const char *name);
void rt_thread_destructors(CPU *c);
void rt_threads_destroy(void);
// Threads hôtes étrangers (thread audio SDL/CoreAudio...) appelant du code
// invité : attach donne au thread courant un CPU invité (pile protégée, TCB,
// TLS, errno), idempotent ; l'appelant encadre rt_invoke par
// rt_guest_enter/leave. detach exécute les destructeurs TLS invités ; il est
// aussi fait automatiquement à la sortie du thread hôte (threads.c).
CPU *rt_foreign_thread_attach(void);
void rt_foreign_thread_detach(void);

int rt_futex(CPU *c,uint32_t address,uint32_t operation,uint32_t value,uint32_t timeout);
// Fournisseurs de shims supplémentaires : chaque fichier runtime s'enregistre
// lui-même, sans modifier shims.c. Un fournisseur renvoie 0 s'il ne connaît
// pas le nom. Déclarer aussi les noms sur une ligne `// SHIMS: a b c` du
// fichier pour que tools/lift.py les accepte.
// Fonctions natives obtenues à l'exécution (SDL_GL_GetProcAddress,
// alGetProcAddress, dlsym sur une bibliothèque hôte) : une adresse invitée
// stable par nom ; un appel invité vers elle passe par rt_shim(c, nom) comme
// un import statique. Renvoie 0 si la table est pleine.
#define RT_DYNAMIC_IMPORT_BASE 0xf8000000u
#define RT_DYNAMIC_IMPORT_STRIDE 16u
#define RT_DYNAMIC_IMPORT_LIMIT 0x40000u
uint32_t rt_dynamic_import(const char *name);
const char *rt_dynamic_import_name(uint32_t address);
// Handles opaques pour les pointeurs hôtes (SDL_Window*, contexte GL, FT_Face...).
// Le code invité ne doit jamais les déréférencer : la plage 0xe8000000 n'est
// pas projetée, un accès invité y est une faute. `kind` est un identifiant non
// nul choisi par chaque pont (par ex. un FOURCC) ; get/free vérifient le type.
#define RT_HANDLE_BASE 0xe8000000u  // 0xe0000000 : données importées (elfprep SHIMDATA_BASE)
#define RT_HANDLE_STRIDE 16u
#define RT_HANDLE_LIMIT 0x800000u
uint32_t rt_handle_new(void *host, uint32_t kind);
void *rt_handle_get(uint32_t handle, uint32_t kind);
uint32_t rt_handle_find(const void *host, uint32_t kind);
void *rt_handle_free(uint32_t handle, uint32_t kind);
// Adresse invitée d'une fonction GL hôte, 0 si non prise en charge (gl.c).
uint32_t rt_gl_proc_address(const char *name);
// Valeur de retour flottante i386 (float/double/long double) : dans ST(0).
static inline void rt_return_st0(CPU *c, double value) { fpush(c, value); }
// Contrat : la réponse d'un fournisseur (accepter ou non) ne dépend que du
// nom, et un refus n'a aucun effet. rt_dispatch mémorise donc, par adresse
// d'import, le fournisseur qui a accepté et l'appelle directement ensuite ;
// s'il refuse quand même, la chaîne complète est reprise (puis piège).
typedef int (*rt_shim_provider)(CPU *c, const char *name);
void rt_register_shim_provider(rt_shim_provider provider);
#define RT_SHIM_PROVIDER(fn) \
    __attribute__((constructor)) static void rt_register_##fn(void) { rt_register_shim_provider(fn); }
// Résolution optionnelle : après acceptation d'un nom, le résolveur peut
// fournir un handler direct (avec sa donnée) qui évite la recherche par nom
// du fournisseur aux appels suivants. Il renvoie 0 pour garder le fournisseur.
typedef int (*rt_shim_handler)(CPU *c, const char *name, void *data);
typedef struct { rt_shim_provider provider; rt_shim_handler handler; void *data; } RtShimRoute;
typedef int (*rt_shim_resolver)(const char *name, RtShimRoute *route);
void rt_register_shim_resolver(rt_shim_provider provider, rt_shim_resolver resolver);
#define RT_SHIM_PROVIDER_RESOLVER(fn, resolver) \
    __attribute__((constructor)) static void rt_register_##fn(void) { rt_register_shim_resolver(fn, resolver); }
// Exécute le nom comme rt_shim ; en cas de succès, *route (si non NULL)
// désigne le fournisseur ou handler à rappeler pour ce même nom.
int rt_shim_route(CPU *c, const char *name, RtShimRoute *route);
static inline int rt_route_call(const RtShimRoute *route, CPU *c, const char *name) {
    return route->handler ? route->handler(c, name, route->data) : route->provider(c, name);
}
void rt_futex_destroy(void);

// Greffons PE traduits d'avance (bin/linux32/*.asi chargés par libMiles,
// tools/lift.py generate_pe). Miles projette l'image à une base choisie à
// l'exécution et applique ses relocations ; le code traduit est indexé par RVA
// et lit `delta` (base réelle - ImageBase, pour la vue écrivable où Miles
// relocalise les données et pour la vue exécutable). Au premier transfert vers une
// adresse non traduite, rt_dispatch demande à rt_pe_lookup si elle appartient
// à une image connue : les octets de ses sections de code doivent être ceux du
// fichier, relocalisations comprises pour ce delta, sinon refus (piège).
typedef struct { uint32_t rva, size; const uint8_t *bytes; } RtPeSection;
typedef struct {
    const char *name;                 // nom du fichier (« mssmp3.asi »)
    uint32_t image_base, image_size, section_alignment;
    uint32_t file_size; uint64_t file_hash;   // FNV-1a 64 du fichier traduit
    const RtPeSection *sections; uint32_t section_count;   // sections de code
    // RVA HIGHLOW dans le code, triées ; bit 31 : la valeur désigne .text.
    const uint32_t *relocations; uint32_t relocation_count;
    const uint32_t *function_rvas; const gfn *functions; uint32_t function_count;  // triées
    uint32_t *delta;   // [0] vue écrivable (données), [1] vue exécutable (code) : base - ImageBase
} RtPeImage;
void rt_pe_register_image(const RtPeImage *image);   // constructeur du code généré
gfn rt_pe_lookup(uint32_t target);                   // NULL : pas une fonction traduite d'une image reconnue
void rt_pe_forget(uint32_t address, uint64_t length); // munmap : images projetées dans la plage oubliées
const RtPeImage *rt_pe_image_named(const char *name);  // image traduite de ce nom de fichier, NULL sinon
uint64_t rt_pe_hash(const uint8_t *data, size_t size);
int rt_pe_file_matches(const RtPeImage *image, const char *host_path);  // fichier identique au traduit
