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
typedef struct Key { uint32_t id, destructor; pthread_key_t native; struct Key *next; } Key;
typedef struct Cond { uint32_t address; pthread_cond_t native; struct Cond *next; } Cond;
typedef struct Once { uint32_t address; pthread_once_t native; struct Once *next; } Once;
static Key *keys; static uint32_t next_key = 1;
static Cond *conditions;
static Once *onces;
static _Thread_local CPU *once_cpu;
static _Thread_local uint32_t once_function, once_address;
static void once_bridge(void) {
    rt_guest_enter();
    rt_invoke(once_cpu, once_function, NULL, 0);
    W32(once_address, 2);
    rt_guest_leave();
}
static Mutex *mutexes;
static Attr *attributes;

static uint32_t argument(CPU *c, unsigned i) { return R32(c->esp + 4 + i * 4); }
static int linux_error(int error) {
    if (error == EAGAIN) return 11;
    if (error == EDEADLK) return 35;
    if (error == ENOTSUP) return 95;
    if (error == ETIMEDOUT) return 110;
    return error; // EINVAL, EBUSY, ENOMEM, EPERM agree on these platforms
}
static Attr *find_attr(uint32_t address) {
    for (Attr *a = attributes; a; a = a->next) if (a->address == address) return a;
    return NULL;
}
static Mutex *find_mutex(uint32_t address) {
    for (Mutex *m = mutexes; m; m = m->next) if (m->address == address) return m;
    return NULL;
}
static int create_mutex(uint32_t address, const pthread_mutexattr_t *attr, uint32_t kind) {
    if (!address || find_mutex(address)) return EINVAL;
    Mutex *m = calloc(1, sizeof(*m));
    if (!m) return ENOMEM;
    int error = pthread_mutex_init(&m->native, attr);
    if (error) { free(m); return error; }
    m->address = address; m->kind = kind; m->next = mutexes; mutexes = m;
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
    struct timespec deadline = {(int32_t)R32(pointer), (int32_t)R32(pointer + 4)}, remaining;
    (void)c;
    if (deadline.tv_nsec < 0 || deadline.tv_nsec >= 1000000000) return EINVAL;
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

static int pthread_objects(CPU *c, const char *name) {
    if (!strcmp(name, "__pthread_key_create")) name = "pthread_key_create";
    if (strncmp(name, "pthread_", 8)) return 0;
    uint32_t address = argument(c, 0);
    int error = 0;
    if (!strcmp(name, "pthread_key_create")) {
        Key *key = calloc(1, sizeof(*key));
        if (!key) error = ENOMEM;
        else {
            // Guest destructors are retained for the guest thread-exit path;
            // they must never be handed to Darwin as native function pointers.
            error = pthread_key_create(&key->native, NULL);
            if (error) free(key);
            else { key->id = next_key++; key->destructor = argument(c, 1); key->next = keys; keys = key; W32(address, key->id); }
        }
    } else if (!strcmp(name, "pthread_setspecific") || !strcmp(name, "pthread_getspecific") || !strcmp(name, "pthread_key_delete")) {
        Key **link = &keys; while (*link && (*link)->id != address) link = &(*link)->next;
        if (!*link) error = EINVAL;
        else if (!strcmp(name, "pthread_setspecific")) error = pthread_setspecific((*link)->native, (void *)(uintptr_t)argument(c, 1));
        else if (!strcmp(name, "pthread_getspecific")) { c->eax = (uint32_t)(uintptr_t)pthread_getspecific((*link)->native); return 1; }
        else { Key *key = *link; error = pthread_key_delete(key->native); if (!error) { *link = key->next; free(key); } }
    } else if (!strcmp(name, "pthread_cond_init")) {
        if (argument(c, 1)) rt_trap(c, c->eip, "guest condition attributes not implemented");
        Cond *item = conditions; while (item && item->address != address) item = item->next;
        if (!address || item) error = EINVAL;
        else {
            item = calloc(1, sizeof(*item));
            if (!item) error = ENOMEM;
            else {
                error = pthread_cond_init(&item->native, NULL);
                if (error) free(item);
                else { item->address = address; item->next = conditions; conditions = item; memset(rt_write_span(c,address,48),0,48); }
            }
        }
    } else if (!strcmp(name, "pthread_cond_destroy") || !strcmp(name, "pthread_cond_signal") || !strcmp(name, "pthread_cond_broadcast")) {
        Cond **link = &conditions; while (*link && (*link)->address != address) link = &(*link)->next;
        if (!*link) error = EINVAL;
        else if (!strcmp(name, "pthread_cond_destroy")) {
            Cond *item = *link; error = pthread_cond_destroy(&item->native);
            if (!error) { *link = item->next; free(item); }
        } else if (!strcmp(name,"pthread_cond_signal")) error = pthread_cond_signal(&(*link)->native);
        else error = pthread_cond_broadcast(&(*link)->native);
    } else if (!strcmp(name,"pthread_cond_wait") || !strcmp(name,"pthread_cond_timedwait")) {
        Cond *item=conditions;while(item && item->address!=address)item=item->next;
        Mutex *mutex=find_mutex(argument(c,1));
        if(!item || !mutex)error=EINVAL;
        else {
            struct timespec deadline={0};int timed=!strcmp(name,"pthread_cond_timedwait");
            if(timed){uint32_t pointer=argument(c,2);deadline.tv_sec=(int32_t)R32(pointer);deadline.tv_nsec=(int32_t)R32(pointer+4);}
            if(timed && (deadline.tv_nsec<0 || deadline.tv_nsec>=1000000000))error=EINVAL;
            else {
                // The wait releases the mutex: an error-checking owner record must
                // not survive it, or another thread's lock would be misattributed.
                int held=mutex->kind==2 && mutex->owned && pthread_equal(mutex->owner,pthread_self());
                if(held)mutex->owned=0;
                unsigned saved=rt_guest_suspend();
                error=timed?pthread_cond_timedwait(&item->native,&mutex->native,&deadline):pthread_cond_wait(&item->native,&mutex->native);
                rt_guest_resume(saved);
                if(held && error!=EPERM && error!=EINVAL)lock_acquired(mutex);
            }
        }
    } else if (!strcmp(name, "pthread_once")) {
        Once *item = onces; while (item && item->address != address) item = item->next;
        if (!item) {
            if (R32(address)) rt_trap(c,c->eip,"unknown nonzero pthread_once control");
            item = calloc(1,sizeof(*item)); if (!item) { c->eax=ENOMEM; return 1; }
            pthread_once_t initial = PTHREAD_ONCE_INIT; item->native = initial;
            item->address=address; item->next=onces; onces=item;
        }
        CPU *old_cpu=once_cpu; uint32_t old_function=once_function, old_address=once_address;
        once_cpu=c; once_function=argument(c,1); once_address=address;
        unsigned saved=rt_guest_suspend();
        error=pthread_once(&item->native,once_bridge);
        rt_guest_resume(saved);
        once_cpu=old_cpu; once_function=old_function; once_address=old_address;
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
        uint32_t type = argument(c, 1);
        if (type > 2) { c->eax = 22; return 1; }
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
        uint32_t attr_address = argument(c, 1);
        Attr *a = find_attr(attr_address);
        error = attr_address && !a ? EINVAL : create_mutex(address, a ? &a->native : NULL, a ? a->kind : 0);
        if (!error) { memset(rt_write_span(c, address, 24), 0, 24); W32(address + 12, a ? a->kind : 0); }
    } else if (!strcmp(name, "pthread_mutex_destroy")) {
        Mutex **link = &mutexes;
        while (*link && (*link)->address != address) link = &(*link)->next;
        if (!*link) error = EINVAL;
        else {
            Mutex *m = *link;
            error = pthread_mutex_destroy(&m->native);
            if (!error) { *link = m->next; W32(address + 12, UINT32_MAX); free(m); }
        }
    } else if (!strcmp(name, "pthread_mutex_lock") || !strcmp(name, "pthread_mutex_trylock") ||
               !strcmp(name, "pthread_mutex_timedlock") || !strcmp(name, "pthread_mutex_unlock")) {
        Mutex *m = find_mutex(address);
        if (!m && address) m = static_mutex(c, address);
        if (!m) error = EINVAL;
        else if (!strcmp(name, "pthread_mutex_lock")) {
            if (m->kind == 2 && m->owned && pthread_equal(m->owner, pthread_self())) error = EDEADLK;
            else { unsigned saved=rt_guest_suspend(); error=pthread_mutex_lock(&m->native); rt_guest_resume(saved); }
            if (!error) lock_acquired(m);
        }
        else if (!strcmp(name, "pthread_mutex_trylock")) { error = pthread_mutex_trylock(&m->native); if (!error) lock_acquired(m); }
        else if (!strcmp(name, "pthread_mutex_timedlock")) { error = timed_lock(c, m, argument(c, 1)); if (!error) lock_acquired(m); }
        else { error = pthread_mutex_unlock(&m->native); if (!error && m->kind == 2) m->owned = 0; }
    } else return 0;
    c->eax = (uint32_t)linux_error(error);
    return 1;
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
        size_t count=0;for(Key *key=keys;key;key=key->next)if(key->destructor && pthread_getspecific(key->native))++count;
        if(!count)return;
        struct Pending {uint32_t function,value;} *pending=calloc(count,sizeof(*pending));
        if(!pending)rt_trap(c,c->eip,"cannot snapshot TLS destructors");
        size_t index=0;
        for(Key *key=keys;key;key=key->next){
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
    while (keys) { Key *next=keys->next; pthread_key_delete(keys->native); free(keys); keys=next; }
    while (conditions) { Cond *next=conditions->next; pthread_cond_destroy(&conditions->native); free(conditions); conditions=next; }
    while (onces) { Once *next=onces->next; free(onces); onces=next; }
    next_key=1;
    while (mutexes) {
        Mutex *next = mutexes->next;
        pthread_mutex_destroy(&mutexes->native);
        free(mutexes); mutexes = next;
    }
    while (attributes) {
        Attr *next = attributes->next;
        pthread_mutexattr_destroy(&attributes->native);
        free(attributes); attributes = next;
    }
    rt_guest_leave();
}
