// POSIX semaphores, Linux scheduling priority ranges and process signal state.
// SHIMS: sem_init sem_destroy sem_post sem_wait sem_trywait sem_timedwait
// SHIMS: sched_get_priority_max sched_get_priority_min
// SHIMS: sigaction sigemptyset sigaddset sigprocmask signal raise
//
// i386 glibc sem_t is 16 guest bytes. As for mutexes, the native state lives in
// a registry keyed by the guest address and is never cast onto guest memory;
// word 0 mirrors glibc's `value << 1` so a debugger sees a plausible count.
//
// Signals: this runtime never delivers asynchronous signals to guest code.
// Dispositions and masks are recorded with the Linux i386 layouts; SIG_IGN and
// SIG_DFL are applied to the host for the signals whose meaning is identical.
// A guest handler is accepted only for SIGCHLD (the runtime creates no child
// process able to raise it); any other guest handler traps.
#include "rt.h"
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

enum { LINUX_EINVAL = 22, LINUX_EAGAIN = 11, LINUX_ETIMEDOUT = 110, LINUX_EOVERFLOW = 75, LINUX_EBUSY = 16 };
static uint32_t argument(CPU *c, unsigned i) { return R32(c->esp + 4 + 4 * i); }
static void fail(CPU *c, int linux_errno) { rt_set_errno(c, linux_errno); c->eax = UINT32_MAX; }

typedef struct Semaphore {
    uint32_t address, value, waiters;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    struct Semaphore *next;
} Semaphore;
static Semaphore *semaphores;
static Semaphore *find_semaphore(uint32_t address) {
    for (Semaphore *s = semaphores; s; s = s->next) if (s->address == address) return s;
    return NULL;
}
static void mirror(Semaphore *s) { W32(s->address, s->value << 1); }

// Gate held on entry and exit; the count itself is protected by s->mutex.
// Returns 0 or a Linux errno.
static int semaphore_wait(Semaphore *s, const struct timespec *deadline, int nonblocking) {
    pthread_mutex_lock(&s->mutex);
    int error = 0;
    if (!s->value && nonblocking) error = LINUX_EAGAIN;
    else if (!s->value) {
        ++s->waiters;
        unsigned saved = rt_guest_suspend();
        while (!s->value && !error)
            error = deadline ? pthread_cond_timedwait(&s->cond, &s->mutex, deadline) : pthread_cond_wait(&s->cond, &s->mutex);
        if (s->value) error = 0;
        if (!error) --s->value;
        --s->waiters;
        pthread_mutex_unlock(&s->mutex);
        rt_guest_resume(saved);
        if (error) return error == ETIMEDOUT ? LINUX_ETIMEDOUT : LINUX_EINVAL;
        mirror(s);
        return 0;
    }
    if (!error) --s->value;
    pthread_mutex_unlock(&s->mutex);
    if (!error) mirror(s);
    return error;
}

static int semaphore_shim(CPU *c, const char *name) {
    // Nom testé avant tout accès : un sem_* non pris en charge doit piéger
    // (fournisseur suivant puis rt_trap), pas renvoyer EINVAL selon l'argument.
    static const char *const known[] = {"sem_init", "sem_destroy", "sem_post", "sem_wait",
                                        "sem_trywait", "sem_timedwait"};
    size_t k = 0;
    while (k < sizeof(known) / sizeof(*known) && strcmp(name, known[k])) ++k;
    if (k == sizeof(known) / sizeof(*known)) return 0;
    uint32_t address = argument(c, 0);
    rt_guest_enter();
    int error = 0;
    if (!strcmp(name, "sem_init")) {
        uint32_t value = argument(c, 2);
        rt_write_span(c, address, 16);
        if (value > 0x7fffffffu) error = LINUX_EINVAL;   // SEM_VALUE_MAX
        else {
            Semaphore *s = find_semaphore(address);
            if (s && s->waiters) rt_trap(c, c->eip, "sem_init on a semaphore with blocked waiters");
            if (!s) {
                s = calloc(1, sizeof(*s));
                if (!s || pthread_mutex_init(&s->mutex, NULL) || pthread_cond_init(&s->cond, NULL))
                    rt_trap(c, c->eip, "cannot allocate native semaphore");
                s->address = address; s->next = semaphores; semaphores = s;
            }
            // pshared only matters across processes; this runtime has one.
            s->value = value;
            memset(GPTR(address), 0, 16); mirror(s);
            W32(address + 4, argument(c, 1) ? 0 : 128);  // glibc: FUTEX_PRIVATE_FLAG unless shared
        }
    } else {
        Semaphore *s = find_semaphore(address);
        if (!s) error = LINUX_EINVAL;
        else if (!strcmp(name, "sem_destroy")) {
            if (s->waiters) rt_trap(c, c->eip, "sem_destroy with blocked waiters");
            Semaphore **link = &semaphores; while (*link != s) link = &(*link)->next;
            *link = s->next;
            pthread_cond_destroy(&s->cond); pthread_mutex_destroy(&s->mutex); free(s);
        } else if (!strcmp(name, "sem_post")) {
            pthread_mutex_lock(&s->mutex);
            if (s->value == 0x7fffffffu) error = LINUX_EOVERFLOW;
            else { ++s->value; pthread_cond_signal(&s->cond); }
            pthread_mutex_unlock(&s->mutex);
            if (!error) mirror(s);
        } else if (!strcmp(name, "sem_wait")) error = semaphore_wait(s, NULL, 0);
        else if (!strcmp(name, "sem_trywait")) error = semaphore_wait(s, NULL, 1);
        else if (!strcmp(name, "sem_timedwait")) {
            uint32_t pointer = argument(c, 1);
            struct timespec deadline;
            // glibc validates the timeout before trying the fast path.
            if (rt_read_timespec(c, pointer, &deadline)) error = LINUX_EINVAL;
            else error = semaphore_wait(s, &deadline, 0);
        } else { rt_guest_leave(); return 0; }
    }
    if (error) fail(c, error); else c->eax = 0;
    rt_guest_leave();
    return 1;
}
RT_SHIM_PROVIDER(semaphore_shim)

static int priority_shim(CPU *c, const char *name) {
    int max = !strcmp(name, "sched_get_priority_max");
    if (!max && strcmp(name, "sched_get_priority_min")) return 0;
    uint32_t policy = argument(c, 0);
    // Linux: SCHED_OTHER 0, FIFO 1, RR 2, BATCH 3, IDLE 5 (DEADLINE 6 has no range).
    if (policy == 1 || policy == 2) c->eax = max ? 99 : 1;
    else if (policy == 0 || policy == 3 || policy == 5) c->eax = 0;
    else fail(c, LINUX_EINVAL);
    return 1;
}
RT_SHIM_PROVIDER(priority_shim)

// Linux i386 glibc struct sigaction: handler, sigset_t (128 bytes), flags, restorer.
#define SIGACTION_SIZE 140
#define SIGSET_SIZE 128
typedef struct { uint32_t handler, flags, restorer; uint8_t mask[SIGSET_SIZE]; } Disposition;
static Disposition dispositions[65];
static pthread_mutex_t signal_lock = PTHREAD_MUTEX_INITIALIZER;
static _Thread_local uint64_t blocked;  // kernel sigset: 64 bits

// Linux signals 32 and 33 are reserved by glibc (SIGCANCEL, SIGSETXID).
static int valid_signal(uint32_t signal) { return signal >= 1 && signal <= 64 && signal != 32 && signal != 33; }
static int host_signal(uint32_t signal) {
    switch (signal) {
    case 1: return SIGHUP; case 2: return SIGINT; case 3: return SIGQUIT; case 10: return SIGUSR1;
    case 12: return SIGUSR2; case 13: return SIGPIPE; case 14: return SIGALRM; case 15: return SIGTERM;
    case 17: return SIGCHLD; case 28: return SIGWINCH; default: return 0;
    }
}
static uint64_t kernel_set(const uint8_t *set) { uint64_t v; memcpy(&v, set, 8); return v; }
// Masque du thread invité courant, pour __sigsetjmp/siglongjmp (nonlocal.c).
uint64_t rt_signal_blocked_mask(void) { return blocked; }
void rt_signal_set_blocked_mask(uint64_t mask) {
    blocked = mask & ~((UINT64_C(1) << 8) | (UINT64_C(1) << 18) | (UINT64_C(3) << 31));
}

static int signal_shim(CPU *c, const char *name) {
    if (!strcmp(name, "sigemptyset")) {
        memset(rt_write_span(c, argument(c, 0), SIGSET_SIZE), 0, SIGSET_SIZE); c->eax = 0;
    } else if (!strcmp(name, "sigaddset")) {
        uint32_t set = argument(c, 0), signal = argument(c, 1);
        if (!valid_signal(signal)) { fail(c, LINUX_EINVAL); return 1; }
        uint8_t *word = rt_write_span(c, set + (signal - 1) / 32 * 4, 4);
        uint32_t bits; memcpy(&bits, word, 4); bits |= 1u << ((signal - 1) % 32); memcpy(word, &bits, 4);
        c->eax = 0;
    } else if (!strcmp(name, "sigprocmask")) {
        uint32_t how = argument(c, 0), set = argument(c, 1), old = argument(c, 2);
        if (set && how > 2) { fail(c, LINUX_EINVAL); return 1; }
        uint64_t previous = blocked;
        if (set) {
            uint64_t value = kernel_set(rt_span(c, set, 8));
            value &= ~((UINT64_C(1) << 8) | (UINT64_C(1) << 18) | (UINT64_C(3) << 31)); // KILL, STOP, glibc-internal
            blocked = how == 0 ? blocked | value : how == 1 ? blocked & ~value : value;
        }
        if (old) memcpy(rt_write_span(c, old, 8), &previous, 8);
        c->eax = 0;
    } else if (!strcmp(name, "raise")) {
        // Signal ignoré par l'invité : sans effet. Sinon l'action par défaut (fin, cœur) ou
        // un gestionnaire invité, qu'aucun des deux ce runtime ne livre : arrêt explicite.
        uint32_t signal = argument(c, 0);
        if (!valid_signal(signal)) { fail(c, LINUX_EINVAL); return 1; }
        pthread_mutex_lock(&signal_lock); uint32_t handler = dispositions[signal].handler; pthread_mutex_unlock(&signal_lock);
        if (handler == 1 && signal != 9 && signal != 19) { c->eax = 0; return 1; }
        char message[64]; snprintf(message, sizeof(message), "guest raise(%u)", signal);
        rt_trap(c, c->eip, message);
    } else if (!strcmp(name, "signal")) {
        // glibc : sémantique BSD (SA_RESTART), renvoie l'ancien gestionnaire ou SIG_ERR (-1).
        uint32_t signal = argument(c, 0), handler = argument(c, 1);
        if (!valid_signal(signal) || signal == 9 || signal == 19) { fail(c, LINUX_EINVAL); c->eax = UINT32_MAX; return 1; }
        // SIGCHLD ; SIGILL des sondes CPU de Crypto++ (signal + setjmp autour de `por` et
        // `cpuid`, que le code traduit exécute) ; SIGSEGV que vaudio_miles pose le temps
        // d'AIL_startup puis retire. Enregistrés sans être livrés : une instruction illégale
        // ou un accès invalide restent un arrêt explicite du runtime (rt_trap).
        if (handler > 1 && signal != 17 && signal != 4 && signal != 11)
            { char m[96]; snprintf(m, sizeof(m), "guest handler for signal %u cannot be delivered by this runtime", signal); rt_trap(c, c->eip, m); }
        pthread_mutex_lock(&signal_lock);
        int host = host_signal(signal);
        if (host && handler <= 1) {
            struct sigaction native; memset(&native, 0, sizeof(native));
            native.sa_handler = handler ? SIG_IGN : SIG_DFL;
            if (sigaction(host, &native, NULL)) { pthread_mutex_unlock(&signal_lock); rt_save_errno(c); c->eax = UINT32_MAX; return 1; }
        }
        uint32_t previous = dispositions[signal].handler;
        dispositions[signal] = (Disposition){0};
        dispositions[signal].handler = handler;
        dispositions[signal].flags = 0x10000000u;   // SA_RESTART (Linux)
        pthread_mutex_unlock(&signal_lock);
        c->eax = previous;
    } else if (!strcmp(name, "sigaction")) {
        uint32_t signal = argument(c, 0), act = argument(c, 1), old = argument(c, 2);
        if (!valid_signal(signal) || (act && (signal == 9 || signal == 19))) { fail(c, LINUX_EINVAL); return 1; }
        Disposition next = {0};
        if (act) {
            const uint8_t *raw = rt_span(c, act, SIGACTION_SIZE);
            memcpy(&next.handler, raw, 4); memcpy(next.mask, raw + 4, SIGSET_SIZE);
            memcpy(&next.flags, raw + 132, 4); memcpy(&next.restorer, raw + 136, 4);
            if (next.handler > 1 && signal != 17 && signal != 4 && signal != 11)
                { char m[96]; snprintf(m, sizeof(m), "guest handler for signal %u cannot be delivered by this runtime", signal); rt_trap(c, c->eip, m); }
        }
        pthread_mutex_lock(&signal_lock);
        Disposition previous = dispositions[signal];
        if (act) {
            int host = host_signal(signal);
            if (host && next.handler <= 1) {
                struct sigaction native; memset(&native, 0, sizeof(native));
                native.sa_handler = next.handler ? SIG_IGN : SIG_DFL;
                if (sigaction(host, &native, NULL)) { pthread_mutex_unlock(&signal_lock); rt_save_errno(c); c->eax = UINT32_MAX; return 1; }
            }
            dispositions[signal] = next;
        }
        pthread_mutex_unlock(&signal_lock);
        if (old) {
            uint8_t *raw = rt_write_span(c, old, SIGACTION_SIZE);
            memcpy(raw, &previous.handler, 4); memcpy(raw + 4, previous.mask, SIGSET_SIZE);
            memcpy(raw + 132, &previous.flags, 4); memcpy(raw + 136, &previous.restorer, 4);
        }
        c->eax = 0;
    } else return 0;
    return 1;
}
RT_SHIM_PROVIDER(signal_shim)
