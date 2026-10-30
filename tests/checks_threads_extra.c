// Threads détachés/jonctions non bloquantes, sémaphores, mutex temporisés,
// signaux et threads hôtes étrangers appelant du code invité.
#include "checks.h"
#include "modules.h"
#include <pthread.h>
#include <signal.h>
#include <stdatomic.h>
#include <time.h>
#include <unistd.h>

#define DATA 0x2001c000u   // zone de données de ces tests
static uint32_t call(CPU *c, const char *name, const uint32_t *args, unsigned count) {
    uint32_t esp = c->esp; c->esp -= 64;
    for (unsigned i = 0; i < count; ++i) W32(c->esp + 4 + 4 * i, args[i]);
    assert(rt_shim(c, name));
    c->esp = esp;
    return c->eax;
}
static uint32_t guest_errno(CPU *c) { uint32_t esp = c->esp; c->esp -= 16; assert(rt_shim(c, "__errno_location")); c->esp = esp; return R32(c->eax); }
static void main_cpu(CPU *c) { rt_cpu_reset(c); c->esp = 0x2001b000; }
static void sleep_ms(unsigned ms) { unsigned saved = rt_guest_suspend(); usleep(ms * 1000); rt_guest_resume(saved); }
static void host_join(pthread_t t) { unsigned saved = rt_guest_suspend(); assert(!pthread_join(t, NULL)); rt_guest_resume(saved); }
static void deadline_in(uint32_t address, long ms) {
    struct timespec now; clock_gettime(CLOCK_REALTIME, &now);
    now.tv_sec += ms / 1000; now.tv_nsec += (ms % 1000) * 1000000;
    if (now.tv_nsec >= 1000000000) { ++now.tv_sec; now.tv_nsec -= 1000000000; }
    W32(address, (uint32_t)now.tv_sec); W32(address + 4, (uint32_t)now.tv_nsec);
}
static double elapsed_ms(const struct timespec *start) {
    struct timespec now; clock_gettime(CLOCK_MONOTONIC, &now);
    return (now.tv_sec - start->tv_sec) * 1e3 + (now.tv_nsec - start->tv_nsec) / 1e6;
}
static void wake_futex(CPU *c, uint32_t address) {
    struct timespec start; clock_gettime(CLOCK_MONOTONIC, &start);
    while (rt_futex(c, address, 129, 1, 0) != 1) { sleep_ms(1); assert(elapsed_ms(&start) < 5000); }
}
static void wait_word(uint32_t address, uint32_t value) {
    struct timespec start; clock_gettime(CLOCK_MONOTONIC, &start);
    while (R32(address) != value) { sleep_ms(1); assert(elapsed_ms(&start) < 5000); }
}

RT_CHECK(pthread_tryjoin_timedjoin_detach) {
    CPU c; main_cpu(&c); rt_guest_enter();
    // 0x28000 (fixture) : incrémente [0x2000b104] puis FUTEX_WAIT sur [0x2000b100] == 0.
    W32(0x2000b100, 0); W32(0x2000b104, 0);
    uint32_t args[4] = {DATA, 0, 0x28000, 0};
    assert(!call(&c, "pthread_create", args, 4));
    uint32_t worker = R32(DATA);
    wait_word(0x2000b104, 1);
    args[0] = worker; args[1] = DATA + 8;
    assert(call(&c, "pthread_tryjoin_np", args, 2) == 16);            // EBUSY
    deadline_in(DATA + 16, 50); args[2] = DATA + 16;
    struct timespec start; clock_gettime(CLOCK_MONOTONIC, &start);
    assert(call(&c, "pthread_timedjoin_np", args, 3) == 110);         // ETIMEDOUT
    assert(elapsed_ms(&start) >= 40);
    W32(DATA + 20, 1000000000);
    assert(call(&c, "pthread_timedjoin_np", args, 3) == 22);          // tv_nsec invalide
    uint32_t kill_args[2] = {worker, 0};
    assert(call(&c, "pthread_kill", kill_args, 2) == 0);
    kill_args[1] = 65; assert(call(&c, "pthread_kill", kill_args, 2) == 22);
    wake_futex(&c, 0x2000b100);
    deadline_in(DATA + 16, 5000); W32(DATA + 8, 0xdead);
    assert(call(&c, "pthread_timedjoin_np", args, 3) == 0 && R32(DATA + 8) == 0); // syscall 0
    assert(call(&c, "pthread_tryjoin_np", args, 2) == 22);            // déjà joint
    kill_args[1] = 0; assert(call(&c, "pthread_kill", kill_args, 2) == 3);

    // Thread terminé puis tryjoin : valeur de retour de 0x6000 (99).
    args[0] = DATA; args[1] = 0; args[2] = 0x6000;
    assert(!call(&c, "pthread_create", args, 4));
    args[0] = R32(DATA); args[1] = DATA + 8;
    clock_gettime(CLOCK_MONOTONIC, &start);
    while (call(&c, "pthread_tryjoin_np", args, 2) == 16) { sleep_ms(1); assert(elapsed_ms(&start) < 5000); }
    assert(c.eax == 0 && R32(DATA + 8) == 99);

    // Détachement d'un thread bloqué : il disparaît seul à sa fin.
    W32(0x2000b100, 0); W32(0x2000b104, 0);
    args[0] = DATA; args[1] = 0; args[2] = 0x28000;
    assert(!call(&c, "pthread_create", args, 4));
    worker = R32(DATA); wait_word(0x2000b104, 1);
    args[0] = worker; assert(call(&c, "pthread_detach", args, 1) == 0);
    assert(call(&c, "pthread_detach", args, 1) == 22);
    args[1] = DATA + 8; assert(call(&c, "pthread_join", args, 2) == 22);
    wake_futex(&c, 0x2000b100);
    kill_args[0] = worker;
    clock_gettime(CLOCK_MONOTONIC, &start);
    while (call(&c, "pthread_kill", kill_args, 2) == 0) { sleep_ms(1); assert(elapsed_ms(&start) < 5000); }
    assert(c.eax == 3 && call(&c, "pthread_join", args, 2) == 3);     // ESRCH : ressources libérées

    // Détachement d'un thread déjà terminé.
    args[0] = DATA; args[1] = 0; args[2] = 0x6000;
    assert(!call(&c, "pthread_create", args, 4));
    worker = R32(DATA); kill_args[0] = worker;
    clock_gettime(CLOCK_MONOTONIC, &start);
    while (call(&c, "pthread_kill", kill_args, 2) == 0) { sleep_ms(1); assert(elapsed_ms(&start) < 5000); }
    args[0] = worker; assert(call(&c, "pthread_detach", args, 1) == 0);
    assert(call(&c, "pthread_detach", args, 1) == 3);
    rt_guest_leave();
}

RT_CHECK(thread_misc_imports) {
    CPU c; main_cpu(&c); rt_guest_enter();
    uint32_t self = call(&c, "pthread_self", NULL, 0);
    uint32_t args[3] = {self, 0};
    assert(call(&c, "pthread_kill", args, 2) == 0);
    args[0] = 0x12345678; assert(call(&c, "pthread_kill", args, 2) == 3);
    assert(call(&c, "pthread_yield", NULL, 0) == 0 && call(&c, "sched_yield", NULL, 0) == 0);
    W32(DATA + 0x100, 1); W32(DATA + 0x104, 0);
    args[0] = self; args[1] = 8; args[2] = DATA + 0x100;
    assert(call(&c, "pthread_setaffinity_np", args, 3) == 0);
    W32(DATA + 0x100, 0); assert(call(&c, "pthread_setaffinity_np", args, 3) == 22);
    args[0] = 0x12345678; assert(call(&c, "pthread_setaffinity_np", args, 3) == 3);
    args[0] = 1; assert(call(&c, "sched_get_priority_max", args, 1) == 99 && call(&c, "sched_get_priority_min", args, 1) == 1);
    args[0] = 2; assert(call(&c, "sched_get_priority_max", args, 1) == 99);
    args[0] = 0; assert(call(&c, "sched_get_priority_max", args, 1) == 0 && call(&c, "sched_get_priority_min", args, 1) == 0);
    args[0] = 4; assert(call(&c, "sched_get_priority_max", args, 1) == UINT32_MAX && guest_errno(&c) == 22);
    rt_guest_leave();
}

RT_CHECK(signal_state_linux_layout) {
    CPU c; main_cpu(&c); rt_guest_enter();
    uint32_t set = DATA + 0x200, act = DATA + 0x300, old = DATA + 0x400;
    memset(GPTR(set), 0xff, 128);
    uint32_t args[3] = {set};
    assert(call(&c, "sigemptyset", args, 1) == 0);
    for (unsigned i = 0; i < 128; ++i) assert(R8(set + i) == 0);
    args[1] = 17; assert(call(&c, "sigaddset", args, 2) == 0 && R32(set) == 1u << 16);
    args[1] = 40; assert(call(&c, "sigaddset", args, 2) == 0 && R32(set + 4) == 1u << 7);
    args[1] = 65; assert(call(&c, "sigaddset", args, 2) == UINT32_MAX && guest_errno(&c) == 22);
    args[1] = 32; assert(call(&c, "sigaddset", args, 2) == UINT32_MAX && guest_errno(&c) == 22);
    // sigprocmask : SIG_BLOCK=0, SIG_UNBLOCK=1, SIG_SETMASK=2 ; le noyau écrit 8 octets.
    memset(GPTR(old), 0xee, 16);
    args[0] = 0; args[1] = set; args[2] = old;
    assert(call(&c, "sigprocmask", args, 3) == 0 && R32(old) == 0 && R32(old + 4) == 0 && R32(old + 8) == 0xeeeeeeee);
    args[0] = 1; args[1] = 0; assert(call(&c, "sigprocmask", args, 3) == 0);
    assert(R32(old) == 1u << 16 && R32(old + 4) == 1u << 7);
    args[1] = set; assert(call(&c, "sigprocmask", args, 3) == 0);
    args[1] = 0; assert(call(&c, "sigprocmask", args, 3) == 0 && R32(old) == 0 && R32(old + 4) == 0);
    W32(set, (1u << 8) | (1u << 1)); W32(set + 4, 0);
    args[0] = 2; args[1] = set; assert(call(&c, "sigprocmask", args, 3) == 0);
    args[1] = 0; assert(call(&c, "sigprocmask", args, 3) == 0 && R32(old) == 1u << 1);   // SIGKILL non bloquable
    args[0] = 7; args[1] = set; assert(call(&c, "sigprocmask", args, 3) == UINT32_MAX && guest_errno(&c) == 22);
    args[0] = 2; W32(set, 0); assert(call(&c, "sigprocmask", args, 3) == 0);
    // sigaction i386 : handler@0, sa_mask@4 (128), sa_flags@132, sa_restorer@136.
    memset(GPTR(act), 0, 140);
    W32(act, 0x26000); W32(act + 4, 1u << 16); W32(act + 132, 1); W32(act + 136, 0x6000);
    args[0] = 17; args[1] = act; args[2] = old;
    memset(GPTR(old), 0xcc, 140);
    assert(call(&c, "sigaction", args, 3) == 0 && R32(old) == 0 && R32(old + 132) == 0);
    args[1] = 0; assert(call(&c, "sigaction", args, 3) == 0);
    assert(R32(old) == 0x26000 && R32(old + 4) == 1u << 16 && R32(old + 132) == 1 && R32(old + 136) == 0x6000);
    W32(act, 0); args[1] = act; args[2] = 0; assert(call(&c, "sigaction", args, 3) == 0);
    args[0] = 9; assert(call(&c, "sigaction", args, 3) == UINT32_MAX && guest_errno(&c) == 22);
    args[0] = 0; assert(call(&c, "sigaction", args, 3) == UINT32_MAX && guest_errno(&c) == 22);
    args[0] = 9; args[1] = 0; args[2] = old; assert(call(&c, "sigaction", args, 3) == 0 && R32(old) == 0);  // consultation permise
    // SIG_IGN sur SIGPIPE (13) est appliqué à l'hôte, puis SIG_DFL restauré.
    W32(act, 1); args[0] = 13; args[1] = act; args[2] = 0;
    assert(call(&c, "sigaction", args, 3) == 0);
    struct sigaction native; assert(!sigaction(SIGPIPE, NULL, &native) && native.sa_handler == SIG_IGN);
    W32(act, 0); assert(call(&c, "sigaction", args, 3) == 0);
    assert(!sigaction(SIGPIPE, NULL, &native) && native.sa_handler == SIG_DFL);
    rt_guest_leave();
}

// --- Threads hôtes étrangers -------------------------------------------------
static atomic_int probe_calls;
static uint32_t probe_args[3];
static int foreign_probe(CPU *c, const char *name) {
    if (strcmp(name, "__test_foreign_probe")) return 0;
    for (unsigned i = 0; i < 3; ++i) probe_args[i] = R32(c->esp + 4 + 4 * i);
    c->eax = probe_args[0] + probe_args[1] + probe_args[2];
    atomic_fetch_add(&probe_calls, 1);
    return 1;
}
RT_SHIM_PROVIDER(foreign_probe)

typedef struct { uint32_t key, gs, errno_slot, sum, pthread_self; int detach; } ForeignResult;
static void *foreign_body(void *opaque) {
    ForeignResult *r = opaque;
    CPU *c = rt_foreign_thread_attach();
    assert(c && rt_foreign_thread_attach() == c);       // idempotent
    rt_guest_enter();
    rt_call(c, 0x1d000, RT_RETURN_SENTINEL);               // eax = gs:[0], edx = canari gs:[0x14]
    r->gs = c->eax; assert(c->eax == c->gs_base && c->edx == R32(c->gs_base + 20));
    uint32_t args[3] = {1, 20, 300};
    r->sum = rt_invoke(c, rt_dynamic_import("__test_foreign_probe"), args, 3);
    r->errno_slot = (call(c, "__errno_location", NULL, 0));
    r->pthread_self = call(c, "pthread_self", NULL, 0);
    uint32_t key_args[2] = {r->key, 0x77};
    assert(call(c, "pthread_setspecific", key_args, 2) == 0);
    rt_guest_leave();
    if (r->detach) rt_foreign_thread_detach();
    return NULL;
}

RT_CHECK(foreign_thread_invokes_guest_code) {
    CPU c; main_cpu(&c); rt_guest_enter();
    // Clé TLS invitée dont le destructeur (0x26000) incrémente [0x2000b098].
    uint32_t args[2] = {DATA + 0x500, 0x26000};
    assert(call(&c, "pthread_key_create", args, 2) == 0);
    uint32_t key = R32(DATA + 0x500), before = R32(0x2000b098);
    uint32_t main_errno = call(&c, "__errno_location", NULL, 0), main_self = call(&c, "pthread_self", NULL, 0);
    uint32_t first_tcb = 0;
    for (int detach = 1; detach >= 0; --detach) {   // détachement explicite, puis à la sortie du thread hôte
        ForeignResult r = {.key = key, .detach = detach};
        pthread_t host;
        unsigned saved = rt_guest_suspend();
        assert(!pthread_create(&host, NULL, foreign_body, &r));
        assert(!pthread_join(host, NULL));
        rt_guest_resume(saved);
        assert(r.sum == 321 && probe_args[0] == 1 && probe_args[2] == 300);
        assert(r.gs && r.gs == r.pthread_self && r.pthread_self != main_self);
        assert(r.errno_slot && r.errno_slot != main_errno);
        assert(R32(0x2000b098) == before + 2 - detach);   // destructeur TLS invité exécuté
        uint32_t kill_args[2] = {r.pthread_self, 0};
        assert(call(&c, "pthread_kill", kill_args, 2) == 3);   // enregistrement libéré
        // Le TCB invité est rendu au tas : le thread suivant réutilise le bloc.
        if (first_tcb) assert(r.pthread_self == first_tcb); else first_tcb = r.pthread_self;
    }
    assert(atomic_load(&probe_calls) == 2);
    args[0] = key; assert(call(&c, "pthread_key_delete", args, 1) == 0);
    rt_guest_leave();
}

// --- Sémaphores et mutex temporisés depuis plusieurs threads ---------------------
static atomic_int sem_passed;
static uint32_t shared_sem;
static void *sem_waiter(void *opaque) {
    (void)opaque;
    CPU *c = rt_foreign_thread_attach();
    rt_guest_enter();
    uint32_t args[1] = {shared_sem};
    assert(call(c, "sem_wait", args, 1) == 0);
    atomic_fetch_add(&sem_passed, 1);
    rt_guest_leave();
    rt_foreign_thread_detach();
    return NULL;
}

RT_CHECK(semaphores_across_threads) {
    CPU c; main_cpu(&c); rt_guest_enter();
    uint32_t sem = DATA + 0x600, ts = DATA + 0x620;
    shared_sem = sem;
    uint32_t args[3] = {sem, 0, 0};
    memset(GPTR(sem), 0xaa, 16);
    assert(call(&c, "sem_init", args, 3) == 0 && R32(sem) == 0 && R32(sem + 4) == 128);
    pthread_t waiters[2];
    unsigned saved = rt_guest_suspend();
    for (int i = 0; i < 2; ++i) assert(!pthread_create(&waiters[i], NULL, sem_waiter, NULL));
    rt_guest_resume(saved);
    sleep_ms(30);
    assert(atomic_load(&sem_passed) == 0);
    args[0] = sem; assert(call(&c, "sem_post", args, 1) == 0);
    struct timespec start; clock_gettime(CLOCK_MONOTONIC, &start);
    while (atomic_load(&sem_passed) < 1) { sleep_ms(1); assert(elapsed_ms(&start) < 5000); }
    sleep_ms(20); assert(atomic_load(&sem_passed) == 1);
    assert(call(&c, "sem_post", args, 1) == 0);
    for (int i = 0; i < 2; ++i) host_join(waiters[i]);
    assert(atomic_load(&sem_passed) == 2 && R32(sem) == 0);
    assert(call(&c, "sem_trywait", args, 1) == UINT32_MAX && guest_errno(&c) == 11);
    W32(ts, 0); W32(ts + 4, 0); args[1] = ts;
    assert(call(&c, "sem_timedwait", args, 2) == UINT32_MAX && guest_errno(&c) == 110);
    deadline_in(ts, 50); clock_gettime(CLOCK_MONOTONIC, &start);
    assert(call(&c, "sem_timedwait", args, 2) == UINT32_MAX && guest_errno(&c) == 110 && elapsed_ms(&start) >= 40);
    W32(ts + 4, 1000000000);
    assert(call(&c, "sem_post", args, 1) == 0 && R32(sem) == 2);   // valeur << 1 comme glibc
    assert(call(&c, "sem_timedwait", args, 2) == UINT32_MAX && guest_errno(&c) == 22);  // validé avant
    W32(ts + 4, 0);
    assert(call(&c, "sem_timedwait", args, 2) == 0 && R32(sem) == 0);
    assert(call(&c, "sem_post", args, 1) == 0 && call(&c, "sem_trywait", args, 1) == 0);
    assert(call(&c, "sem_destroy", args, 1) == 0);
    assert(call(&c, "sem_post", args, 1) == UINT32_MAX && guest_errno(&c) == 22);
    args[1] = 0; args[2] = 0x80000000u;
    assert(call(&c, "sem_init", args, 3) == UINT32_MAX && guest_errno(&c) == 22);
    args[2] = 0x7fffffffu; assert(call(&c, "sem_init", args, 3) == 0);
    assert(call(&c, "sem_post", args, 1) == UINT32_MAX && guest_errno(&c) == 75);   // EOVERFLOW
    assert(call(&c, "sem_destroy", args, 1) == 0);
    rt_guest_leave();
}

static uint32_t timed_mutex, timed_deadline;
static uint32_t timed_result;
static void *timed_locker(void *opaque) {
    long ms = (long)(intptr_t)opaque;
    CPU *c = rt_foreign_thread_attach();
    rt_guest_enter();
    deadline_in(timed_deadline, ms);
    uint32_t args[2] = {timed_mutex, timed_deadline};
    timed_result = call(c, "pthread_mutex_timedlock", args, 2);
    if (!timed_result) assert(call(c, "pthread_mutex_unlock", args, 1) == 0);
    rt_guest_leave();
    rt_foreign_thread_detach();
    return NULL;
}
static void run_locker(long ms) {
    pthread_t t; unsigned saved = rt_guest_suspend();
    assert(!pthread_create(&t, NULL, timed_locker, (void *)(intptr_t)ms));
    assert(!pthread_join(t, NULL)); rt_guest_resume(saved);
}

RT_CHECK(mutex_timedlock_and_static_initializers) {
    CPU c; main_cpu(&c); rt_guest_enter();
    uint32_t m = DATA + 0x700, attr = DATA + 0x740, ts = DATA + 0x760;
    timed_mutex = m; timed_deadline = DATA + 0x780;
    memset(GPTR(m), 0, 24);                               // PTHREAD_MUTEX_INITIALIZER
    uint32_t args[2] = {m, 0};
    assert(call(&c, "pthread_mutex_lock", args, 1) == 0);
    struct timespec start; clock_gettime(CLOCK_MONOTONIC, &start);
    run_locker(50);
    assert(timed_result == 110 && elapsed_ms(&start) >= 40);
    assert(call(&c, "pthread_mutex_unlock", args, 1) == 0);
    run_locker(5000);
    assert(timed_result == 0);
    W32(ts, 0); W32(ts + 4, 2000000000u); args[1] = ts;
    assert(call(&c, "pthread_mutex_timedlock", args, 2) == 0);   // libre : délai non examiné
    assert(call(&c, "pthread_mutex_unlock", args, 1) == 0);
    assert(call(&c, "pthread_mutex_destroy", args, 1) == 0 && R32(m + 12) == UINT32_MAX);
    // Mutex à contrôle d'erreur : __kind à l'offset 12 (i386), EDEADLK sur soi.
    args[0] = attr; assert(call(&c, "pthread_mutexattr_init", args, 1) == 0);
    args[1] = 2; assert(call(&c, "pthread_mutexattr_settype", args, 2) == 0);
    args[0] = m; args[1] = attr; assert(call(&c, "pthread_mutex_init", args, 2) == 0);
    assert(R32(m + 12) == 2 && R32(m + 16) == 0);
    assert(call(&c, "pthread_mutex_lock", args, 1) == 0);
    W32(ts + 4, 0); args[1] = ts;
    assert(call(&c, "pthread_mutex_timedlock", args, 2) == 35 && call(&c, "pthread_mutex_lock", args, 1) == 35);
    assert(call(&c, "pthread_mutex_unlock", args, 1) == 0 && call(&c, "pthread_mutex_unlock", args, 1) == 1);  // EPERM
    assert(call(&c, "pthread_mutex_destroy", args, 1) == 0);
    args[0] = attr; assert(call(&c, "pthread_mutexattr_destroy", args, 1) == 0);
    // PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP : {0, 0, 0, 1, 0, 0}.
    memset(GPTR(m), 0, 24); W32(m + 12, 1); args[0] = m;
    assert(call(&c, "pthread_mutex_lock", args, 1) == 0 && call(&c, "pthread_mutex_trylock", args, 1) == 0);
    run_locker(30); assert(timed_result == 110);
    assert(call(&c, "pthread_mutex_unlock", args, 1) == 0 && call(&c, "pthread_mutex_unlock", args, 1) == 0);
    run_locker(5000); assert(timed_result == 0);
    assert(call(&c, "pthread_mutex_destroy", args, 1) == 0);
    W32(m + 12, 7); assert(call(&c, "pthread_mutex_lock", args, 1) == 22);   // ni initialiseur ni objet connu
    rt_guest_leave();
}

// Un mutex errorcheck libéré par pthread_cond_wait ne doit pas garder un
// propriétaire périmé : après le réveil, se reverrouiller donne EDEADLK.
RT_CHECK(errorcheck_owner_across_cond_wait) {
    CPU c; main_cpu(&c); rt_guest_enter();
    uint32_t m = DATA + 0x800, attr = DATA + 0x840, cond = DATA + 0x880, ts = DATA + 0x8c0;
    timed_mutex = m; timed_deadline = DATA + 0x8e0;
    uint32_t args[3] = {attr};
    assert(call(&c, "pthread_mutexattr_init", args, 1) == 0);
    args[1] = 2; assert(call(&c, "pthread_mutexattr_settype", args, 2) == 0);
    args[0] = m; args[1] = attr; assert(call(&c, "pthread_mutex_init", args, 2) == 0);
    args[0] = cond; args[1] = 0; assert(call(&c, "pthread_cond_init", args, 2) == 0);
    args[0] = m; assert(call(&c, "pthread_mutex_lock", args, 1) == 0);
    pthread_t other; unsigned saved = rt_guest_suspend();
    assert(!pthread_create(&other, NULL, timed_locker, (void *)(intptr_t)5000));
    rt_guest_resume(saved);
    deadline_in(ts, 150);
    args[0] = cond; args[1] = m; args[2] = ts;
    assert(call(&c, "pthread_cond_timedwait", args, 3) == 110);   // l'autre thread a pris et rendu m
    host_join(other);
    assert(timed_result == 0);
    W32(ts, 0); W32(ts + 4, 0); args[0] = m; args[1] = ts;
    assert(call(&c, "pthread_mutex_timedlock", args, 2) == 35);
    assert(call(&c, "pthread_mutex_unlock", args, 1) == 0);
    assert(call(&c, "pthread_mutex_destroy", args, 1) == 0);
    args[0] = cond; assert(call(&c, "pthread_cond_destroy", args, 1) == 0);
    args[0] = attr; assert(call(&c, "pthread_mutexattr_destroy", args, 1) == 0);
    rt_guest_leave();
}

RT_CHECK(unknown_semaphore_function_is_not_handled) {
    CPU c; rt_cpu_reset(&c); c.esp = 0x20010000; W32(c.esp + 4, 0x20011000);
    assert(rt_shim(&c, "sem_getvalue") == 0);   // non pris en charge : le dispatcher piégera
    assert(rt_shim(&c, "sem_open") == 0);
}

// Un FUTEX_WAKE qui compte un attendeur dont le délai expire au même moment
// doit le faire réussir : sinon ce réveil est perdu (Linux rend 0). Course
// provoquée en boucle : réveils comptés = attentes réussies.
#define FUTEX_RACE_WORD (DATA + 0x200)
#define FUTEX_RACE_TIMEOUT (DATA + 0x208)
static atomic_int futex_race_done;
static void *futex_race_waiter(void *successes) {
    CPU c; rt_cpu_reset(&c);
    for (int i = 0; i < 20000; ++i)
        if (rt_futex(&c, FUTEX_RACE_WORD, 128, 0, FUTEX_RACE_TIMEOUT) == 0) ++*(int *)successes;
    atomic_store(&futex_race_done, 1);
    return NULL;
}
RT_CHECK(futex_wake_racing_timeout_is_not_lost) {
    CPU c; main_cpu(&c);
    W32(FUTEX_RACE_WORD, 0); W32(FUTEX_RACE_TIMEOUT, 0); W32(FUTEX_RACE_TIMEOUT + 4, 20000);   // 20 µs
    int successes = 0; long woken = 0;
    atomic_store(&futex_race_done, 0);
    pthread_t waiter; assert(!pthread_create(&waiter, NULL, futex_race_waiter, &successes));
    while (!atomic_load(&futex_race_done)) woken += rt_futex(&c, FUTEX_RACE_WORD, 129, 1, 0);
    assert(!pthread_join(waiter, NULL));
    assert(woken == successes);
}
