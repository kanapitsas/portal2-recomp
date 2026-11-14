// Linux i386 pthread objects do not have Darwin's size/layout. Keep the native
// objects in a registry keyed by their guest address; never cast guest storage.
// i386 glibc pthread_mutex_t: __lock, __count, __owner, __kind (offset 12),
// __nusers, __spins: the static initializers differ only in __kind.
// SHIMS: pthread_mutex_timedlock
#include "modules.h"
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

typedef struct Mutex {
    uint32_t address, kind;
    pthread_mutex_t native;
    pthread_t owner;   // tracked for error-checking mutexes only
    int owned;
    struct Mutex *next;
} Mutex;
typedef struct Attr {
    uint32_t address;
    pthread_mutexattr_t native;
    uint32_t kind;
    struct Attr *next;
} Attr;
// Clés TLS invitées : tableau indexé par id-1 (PTHREAD_KEYS_MAX glibc = 1024),
// recherche directe pour set/getspecific ; un id supprimé est réutilisé comme
// dans glibc. Un id hors table ou libre : EINVAL.
#define KEYS_MAX 1024u
typedef struct Key { int used; uint32_t destructor; pthread_key_t native; } Key;
typedef struct Cond { uint32_t address; pthread_cond_t native; struct Cond *next; } Cond;
typedef struct Once { uint32_t address; int done; pthread_once_t native; struct Once *next; } Once;
static Key keys[KEYS_MAX];
static Key *find_key(uint32_t id) { return id && id <= KEYS_MAX && keys[id - 1].used ? &keys[id - 1] : NULL; }
static _Thread_local CPU *once_cpu;
static _Thread_local uint32_t once_function, once_address;
static _Thread_local Once *once_item;
static void once_bridge(void) {
    rt_guest_enter();
    rt_invoke(once_cpu, once_function, NULL, 0);
    W32(once_address, 2);
    once_item->done = 1;
    rt_guest_leave();
}
static Attr *attributes;

// Code d'erreur pthread hôte -> Linux (0 reste 0 ; rt_linux_errno traduit
// EAGAIN, EDEADLK, ENOTSUP, ETIMEDOUT ; EINVAL, EBUSY, ENOMEM, EPERM coïncident).
static uint32_t linux_error(int error) { return error ? (uint32_t)rt_linux_errno(error) : 0; }
static Attr *find_attr(uint32_t address) {
    for (Attr *a = attributes; a; a = a->next) if (a->address == address) return a;
    return NULL;
}
// Tables de hachage chaînées par adresse invitée (champ next), toujours
// consultées sous la porte invitée : recherche, création et destruction en O(1).
#define BUCKETS 4096u
static unsigned bucket(uint32_t address) { return (address >> 2 ^ address >> 14) & (BUCKETS - 1); }
static Mutex *mutexes[BUCKETS];
static Cond *conditions[BUCKETS];
static Once *onces[BUCKETS];
static Mutex *find_mutex(uint32_t address) {
    Mutex *m = mutexes[bucket(address)];
    while (m && m->address != address) m = m->next;
    return m;
}
static Cond **cond_link(uint32_t address) {
    Cond **link = &conditions[bucket(address)];
    while (*link && (*link)->address != address) link = &(*link)->next;
    return link;
}
static Cond *find_cond(uint32_t address) { return *cond_link(address); }
static int create_mutex(uint32_t address, const pthread_mutexattr_t *attr, uint32_t kind) {
    if (!address || find_mutex(address)) return EINVAL;
    Mutex *m = calloc(1, sizeof(*m));
    if (!m) return ENOMEM;
    int error = pthread_mutex_init(&m->native, attr);
    if (error) { free(m); return error; }
    m->address = address; m->kind = kind; m->next = mutexes[bucket(address)]; mutexes[bucket(address)] = m;
    return 0;
}
static int native_type(uint32_t kind) {
    return kind == 1 ? PTHREAD_MUTEX_RECURSIVE : kind == 2 ? PTHREAD_MUTEX_ERRORCHECK : PTHREAD_MUTEX_NORMAL;
}
// Lazily adopt a statically initialized guest mutex: PTHREAD_MUTEX_INITIALIZER
// and the _NP recursive/errorcheck/adaptive initializers (kind 0..3).
static Mutex *static_mutex(CPU *c, uint32_t address) {
    const uint8_t *raw = rt_span(c, address, 24);
    uint32_t word[6]; memcpy(word, raw, sizeof(word));
    if (word[0] || word[1] || word[2] || word[4] || word[5] || word[3] > 3) return NULL;
    pthread_mutexattr_t attr;
    if (pthread_mutexattr_init(&attr)) return NULL;
    int error = pthread_mutexattr_settype(&attr, native_type(word[3]));
    if (!error) error = create_mutex(address, &attr, word[3] == 3 ? 0 : word[3]);
    pthread_mutexattr_destroy(&attr);
    return error ? NULL : find_mutex(address);
}
static void lock_acquired(Mutex *m) { if (m->kind == 2) { m->owner = pthread_self(); m->owned = 1; } }
static int deadline_passed(const struct timespec *deadline, struct timespec *remaining) {
    struct timespec now; clock_gettime(CLOCK_REALTIME, &now);
    if (now.tv_sec > deadline->tv_sec || (now.tv_sec == deadline->tv_sec && now.tv_nsec >= deadline->tv_nsec)) return 1;
    remaining->tv_sec = deadline->tv_sec - now.tv_sec; remaining->tv_nsec = deadline->tv_nsec - now.tv_nsec;
    if (remaining->tv_nsec < 0) { --remaining->tv_sec; remaining->tv_nsec += 1000000000; }
    return 0;
}
// Darwin has no pthread_mutex_timedlock: poll trylock with the gate released,
// backing off from 50 us to 1 ms, until the CLOCK_REALTIME deadline.
static int timed_lock(CPU *c, Mutex *m, uint32_t pointer) {
    if (m->kind == 2 && m->owned && pthread_equal(m->owner, pthread_self())) return EDEADLK;
    int error = pthread_mutex_trylock(&m->native);
    if (error != EBUSY) return error;
    struct timespec deadline, remaining;
    if (rt_read_timespec(c, pointer, &deadline)) return EINVAL;
    long backoff = 50000;
    unsigned saved = rt_guest_suspend();
    while ((error = pthread_mutex_trylock(&m->native)) == EBUSY) {
        if (deadline_passed(&deadline, &remaining)) { error = ETIMEDOUT; break; }
        struct timespec delay = {0, backoff};
        if (remaining.tv_sec == 0 && remaining.tv_nsec < backoff) delay.tv_nsec = remaining.tv_nsec;
        nanosleep(&delay, NULL);
        if (backoff < 1000000) backoff *= 2;
    }
    rt_guest_resume(saved);
    return error;
}

enum { MUTEX_LOCK, MUTEX_TRYLOCK, MUTEX_TIMEDLOCK, MUTEX_UNLOCK };
// Appelé sous la porte invitée. Verrou libre : trylock, sans relâcher la porte
// (il ne bloque jamais) ; sinon attente bloquante porte relâchée, comme avant.
static int mutex_operation(CPU *c, uint32_t address, int op) {
    Mutex *m = find_mutex(address);
    if (!m && address) m = static_mutex(c, address);
    if (!m) return EINVAL;
    int error;
    if (op == MUTEX_LOCK) {
        if (m->kind == 2 && m->owned && pthread_equal(m->owner, pthread_self())) return EDEADLK;
        if ((error = pthread_mutex_trylock(&m->native)) == EBUSY) {
            unsigned saved = rt_guest_suspend(); error = pthread_mutex_lock(&m->native); rt_guest_resume(saved);
        }
        if (!error) lock_acquired(m);
    } else if (op == MUTEX_TRYLOCK) { error = pthread_mutex_trylock(&m->native); if (!error) lock_acquired(m); }
    else if (op == MUTEX_TIMEDLOCK) { error = timed_lock(c, m, rt_arg32(c, 1)); if (!error) lock_acquired(m); }
    else { error = pthread_mutex_unlock(&m->native); if (!error && m->kind == 2) m->owned = 0; }
    return error;
}

static int create_cond(uint32_t address, Cond **out) {
    Cond *item = calloc(1, sizeof(*item));
    if (!item) return ENOMEM;
    int error = pthread_cond_init(&item->native, NULL);
    if (error) { free(item); return error; }
    item->address = address; item->next = conditions[bucket(address)]; conditions[bucket(address)] = item;
    *out = item;
    return 0;
}
// PTHREAD_COND_INITIALIZER glibc : 48 octets nuls, sans pthread_cond_init
// (std::condition_variable). pthread_cond_destroy laisse une marque non nulle
// pour qu'une condition détruite ne soit pas réadoptée.
#define COND_DESTROYED 44u
static int static_cond_bytes(CPU *c, uint32_t address) {
    const uint8_t *raw = rt_span(c, address, 48);
    for (unsigned i = 0; i < 48; ++i) if (raw[i]) return 0;
    return 1;
}
static Cond *find_or_adopt_cond(CPU *c, uint32_t address) {
    Cond *item = find_cond(address);
    if (!item && address && static_cond_bytes(c, address) && create_cond(address, &item)) item = NULL;
    return item;
}
static int cond_wake(CPU *c, uint32_t address, int broadcast) {
    Cond *item = find_or_adopt_cond(c, address);
    if (!item) return EINVAL;
    return broadcast ? pthread_cond_broadcast(&item->native) : pthread_cond_signal(&item->native);
}
static int cond_wait(CPU *c, int timed) {
    Cond *item = find_or_adopt_cond(c, rt_arg32(c, 0));
    Mutex *mutex = find_mutex(rt_arg32(c, 1));
    if (!item || !mutex) return EINVAL;
    struct timespec deadline = {0};
    if (timed && rt_read_timespec(c, rt_arg32(c, 2), &deadline)) return EINVAL;
    // The wait releases the mutex: an error-checking owner record must
    // not survive it, or another thread's lock would be misattributed.
    int held = mutex->kind == 2 && mutex->owned && pthread_equal(mutex->owner, pthread_self());
    if (held) mutex->owned = 0;
    unsigned saved = rt_guest_suspend();
    int error = timed ? pthread_cond_timedwait(&item->native, &mutex->native, &deadline)
                      : pthread_cond_wait(&item->native, &mutex->native);
    rt_guest_resume(saved);
    if (held && error != EPERM && error != EINVAL) lock_acquired(mutex);
    return error;
}
// getspecific : c->eax reçoit la valeur (EINVAL pour une clé invalide, comme
// avant) et renvoie -1 ; setspecific renvoie le code d'erreur.
static int key_specific(CPU *c, int set) {
    Key *key = find_key(rt_arg32(c, 0));
    if (set) return key ? pthread_setspecific(key->native, (void *)(uintptr_t)rt_arg32(c, 1)) : EINVAL;
    c->eax = key ? (uint32_t)(uintptr_t)pthread_getspecific(key->native) : (uint32_t)EINVAL;
    return -1;
}

static int pthread_objects(CPU *c, const char *name) {
    if (!strcmp(name, "__pthread_key_create")) name = "pthread_key_create";
    if (strncmp(name, "pthread_", 8)) return 0;
    uint32_t address = rt_arg32(c, 0);
    int error = 0;
    if (!strcmp(name, "pthread_key_create")) {
        rt_write_span(c, address, 4);   // avant de réserver une clé qui fuirait
        uint32_t id = 1; while (id <= KEYS_MAX && keys[id - 1].used) ++id;
        if (id > KEYS_MAX) error = EAGAIN;
        else {
            Key *key = &keys[id - 1];
            // Guest destructors are retained for the guest thread-exit path;
            // they must never be handed to Darwin as native function pointers.
            error = pthread_key_create(&key->native, NULL);
            if (!error) { key->used = 1; key->destructor = rt_arg32(c, 1); W32(address, id); }
        }
    } else if (!strcmp(name, "pthread_setspecific") || !strcmp(name, "pthread_getspecific") || !strcmp(name, "pthread_key_delete")) {
        Key *key = find_key(address);
        if (!strcmp(name, "pthread_setspecific")) error = key_specific(c, 1);
        else if (!strcmp(name, "pthread_getspecific")) { key_specific(c, 0); return 1; }
        else if (!key) error = EINVAL;
        else { error = pthread_key_delete(key->native); if (!error) key->used = 0; }
    } else if (!strcmp(name, "pthread_cond_init")) {
        if (rt_arg32(c, 1)) rt_trap(c, c->eip, "guest condition attributes not implemented");
        Cond *item = find_cond(address);
        if (!address || item) error = EINVAL;
        else if (!(error = create_cond(address, &item))) memset(rt_write_span(c, address, 48), 0, 48);
    } else if (!strcmp(name, "pthread_cond_signal") || !strcmp(name, "pthread_cond_broadcast")) {
        error = cond_wake(c, address, !strcmp(name, "pthread_cond_broadcast"));
    } else if (!strcmp(name, "pthread_cond_destroy")) {
        Cond **link = cond_link(address);
        if (!*link) {   // statique jamais utilisée : glibc renvoie 0
            if (address && static_cond_bytes(c, address)) W32(address + COND_DESTROYED, UINT32_MAX);
            else error = EINVAL;
        } else {
            Cond *item = *link; error = pthread_cond_destroy(&item->native);
            if (!error) { *link = item->next; W32(address + COND_DESTROYED, UINT32_MAX); free(item); }
        }
    } else if (!strcmp(name,"pthread_cond_wait") || !strcmp(name,"pthread_cond_timedwait")) {
        error = cond_wait(c, !strcmp(name, "pthread_cond_timedwait"));
    } else if (!strcmp(name, "pthread_once")) {
        // once_bridge écrit 2 une fois la routine terminée ; avant, le mot
        // invité reste à PTHREAD_ONCE_INIT (0) : 2 = déjà fait, sans registre.
        if (R32(address) == 2) { c->eax = 0; return 1; }
        Once *item = onces[bucket(address)]; while (item && item->address != address) item = item->next;
        if (!item) {
            if (R32(address)) rt_trap(c,c->eip,"unknown nonzero pthread_once control");
            item = calloc(1,sizeof(*item)); if (!item) { c->eax=ENOMEM; return 1; }
            pthread_once_t initial = PTHREAD_ONCE_INIT; item->native = initial;
            item->address=address; item->next=onces[bucket(address)]; onces[bucket(address)]=item;
        } else if (item->done && !R32(address)) {
            // Entrée terminée mais mot remis à 0 : mémoire réutilisée pour un
            // nouveau contrôle. Personne n'attend un once terminé : on réarme.
            pthread_once_t initial = PTHREAD_ONCE_INIT; item->native = initial; item->done = 0;
        }
        CPU *old_cpu=once_cpu; uint32_t old_function=once_function, old_address=once_address; Once *old_item=once_item;
        once_cpu=c; once_function=rt_arg32(c,1); once_address=address; once_item=item;
        unsigned saved=rt_guest_suspend();
        error=pthread_once(&item->native,once_bridge);
        rt_guest_resume(saved);
        once_cpu=old_cpu; once_function=old_function; once_address=old_address; once_item=old_item;
    } else if (!strcmp(name, "pthread_mutexattr_init")) {
        if (!address || find_attr(address)) error = EINVAL;
        else {
            Attr *a = calloc(1, sizeof(*a));
            if (!a) error = ENOMEM;
            else {
                error = pthread_mutexattr_init(&a->native);
                if (error) free(a);
                else { a->address = address; a->next = attributes; attributes = a; W32(address, 0); }
            }
        }
    } else if (!strcmp(name, "pthread_mutexattr_settype")) {
        Attr *a = find_attr(address);
        uint32_t type = rt_arg32(c, 1);
        if (type > 2) { c->eax = RT_EINVAL; return 1; }
        error = a ? pthread_mutexattr_settype(&a->native, native_type(type)) : EINVAL;
        if (!error) { a->kind = type; W32(address, type); }
    } else if (!strcmp(name, "pthread_mutexattr_destroy")) {
        Attr **link = &attributes;
        while (*link && (*link)->address != address) link = &(*link)->next;
        if (!*link) error = EINVAL;
        else {
            Attr *a = *link;
            error = pthread_mutexattr_destroy(&a->native);
            if (!error) { *link = a->next; W32(address, UINT32_MAX); free(a); }
        }
    } else if (!strcmp(name, "pthread_mutex_init")) {
        uint32_t attr_address = rt_arg32(c, 1);
        Attr *a = find_attr(attr_address);
        error = attr_address && !a ? EINVAL : create_mutex(address, a ? &a->native : NULL, a ? a->kind : 0);
        if (!error) { memset(rt_write_span(c, address, 24), 0, 24); W32(address + 12, a ? a->kind : 0); }
    } else if (!strcmp(name, "pthread_mutex_destroy")) {
        Mutex **link = &mutexes[bucket(address)];
        while (*link && (*link)->address != address) link = &(*link)->next;
        if (!*link) error = EINVAL;
        else {
            Mutex *m = *link;
            error = pthread_mutex_destroy(&m->native);
            if (!error) { *link = m->next; W32(address + 12, UINT32_MAX); free(m); }
        }
    } else if (!strcmp(name, "pthread_mutex_lock") || !strcmp(name, "pthread_mutex_trylock") ||
               !strcmp(name, "pthread_mutex_timedlock") || !strcmp(name, "pthread_mutex_unlock")) {
        int op = !strcmp(name, "pthread_mutex_lock") ? MUTEX_LOCK : !strcmp(name, "pthread_mutex_trylock") ? MUTEX_TRYLOCK
                 : !strcmp(name, "pthread_mutex_timedlock") ? MUTEX_TIMEDLOCK : MUTEX_UNLOCK;
        error = mutex_operation(c, address, op);
    } else return 0;
    c->eax = linux_error(error);
    return 1;
}

// Routes directes (rt_shim_route) des appels fréquents, sans recherche par nom.
enum { ROUTE_COND_SIGNAL = 16, ROUTE_COND_BROADCAST, ROUTE_COND_WAIT, ROUTE_COND_TIMEDWAIT,
       ROUTE_GETSPECIFIC, ROUTE_SETSPECIFIC, ROUTE_SELF };
static int pthread_handler(CPU *c, const char *name, void *data) {
    (void)name;
    int op = (int)(intptr_t)data, error;
    rt_guest_enter();
    switch (op) {
    case ROUTE_COND_SIGNAL: case ROUTE_COND_BROADCAST: error = cond_wake(c, rt_arg32(c, 0), op == ROUTE_COND_BROADCAST); break;
    case ROUTE_COND_WAIT: case ROUTE_COND_TIMEDWAIT: error = cond_wait(c, op == ROUTE_COND_TIMEDWAIT); break;
    case ROUTE_GETSPECIFIC: error = key_specific(c, 0); break;
    case ROUTE_SETSPECIFIC: error = key_specific(c, 1); break;
    case ROUTE_SELF: rt_thread_shim(c, "pthread_self"); error = -1; break;
    default: error = mutex_operation(c, rt_arg32(c, 0), op);
    }
    if (error >= 0) c->eax = linux_error(error);
    rt_guest_leave();
    return 1;
}
int rt_pthread_resolve(const char *name, RtShimRoute *route) {
    static const struct { const char *name; int op; } hot[] = {
        {"pthread_mutex_lock", MUTEX_LOCK}, {"pthread_mutex_trylock", MUTEX_TRYLOCK},
        {"pthread_mutex_unlock", MUTEX_UNLOCK}, {"pthread_cond_signal", ROUTE_COND_SIGNAL},
        {"pthread_cond_broadcast", ROUTE_COND_BROADCAST}, {"pthread_cond_wait", ROUTE_COND_WAIT},
        {"pthread_cond_timedwait", ROUTE_COND_TIMEDWAIT}, {"pthread_getspecific", ROUTE_GETSPECIFIC},
        {"pthread_setspecific", ROUTE_SETSPECIFIC}, {"pthread_self", ROUTE_SELF},
    };
    for (size_t i = 0; i < sizeof(hot) / sizeof(*hot); ++i)
        if (!strcmp(name, hot[i].name)) {
            route->handler = pthread_handler; route->data = (void *)(intptr_t)hot[i].op;
            return 1;
        }
    return 0;
}
int rt_pthread_shim(CPU *c,const char *name) {
    if(strcmp(name,"__pthread_key_create") && strncmp(name,"pthread_",8))return 0;
    rt_guest_enter();
    int handled=rt_thread_shim(c,name) || pthread_objects(c,name);
    rt_guest_leave();
    return handled;
}
void rt_thread_destructors(CPU *c) {
    for(unsigned pass=0;pass<4;++pass) {
        size_t count=0;for(Key *key=keys;key<keys+KEYS_MAX;++key)if(key->used && key->destructor && pthread_getspecific(key->native))++count;
        if(!count)return;
        struct Pending {uint32_t function,value;} *pending=calloc(count,sizeof(*pending));
        if(!pending)rt_trap(c,c->eip,"cannot snapshot TLS destructors");
        size_t index=0;
        for(Key *key=keys;key<keys+KEYS_MAX;++key){
            if(!key->used)continue;
            uint32_t value=(uint32_t)(uintptr_t)pthread_getspecific(key->native);
            if(key->destructor && value){pending[index++]=(struct Pending){key->destructor,value};pthread_setspecific(key->native,NULL);}
        }
        for(size_t i=0;i<index;++i)rt_invoke(c,pending[i].function,&pending[i].value,1);
        free(pending);
    }
}
void rt_pthreads_destroy(void) {
    rt_guest_enter();
    rt_threads_destroy();
    for (Key *key=keys;key<keys+KEYS_MAX;++key) if (key->used) { pthread_key_delete(key->native); key->used=0; }
    for (unsigned i = 0; i < BUCKETS; ++i) {
        while (conditions[i]) { Cond *next=conditions[i]->next; pthread_cond_destroy(&conditions[i]->native); free(conditions[i]); conditions[i]=next; }
        while (onces[i]) { Once *next=onces[i]->next; free(onces[i]); onces[i]=next; }
        while (mutexes[i]) { Mutex *next=mutexes[i]->next; pthread_mutex_destroy(&mutexes[i]->native); free(mutexes[i]); mutexes[i]=next; }
    }
    while (attributes) {
        Attr *next = attributes->next;
        pthread_mutexattr_destroy(&attributes->native);
        free(attributes); attributes = next;
    }
    rt_guest_leave();
}
