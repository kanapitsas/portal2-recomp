// i386 pthread_t is a guest TCB address; Darwin pthread_t never crosses the ABI.
// SHIMS: pthread_detach pthread_kill pthread_yield pthread_setaffinity_np pthread_timedjoin_np pthread_tryjoin_np sched_yield
// SHIMS: pthread_setname_np
// pthread_cancel is deliberately not declared: it traps with an explicit reason
// (asynchronous cancellation of AOT code cannot be honoured). Only weak
// references exist in the game (libstdc++'s __gthread_active_p probe).
#include "modules.h"
#include <errno.h>
#include <pthread.h>
#include <sched.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/sysctl.h>
#include <time.h>

typedef struct ThreadAttr {
    uint32_t address,size;
    struct ThreadAttr *next;
} ThreadAttr;
typedef struct Thread {
    CPU cpu;
    pthread_t native;
    uint32_t handle,entry,argument,stack,length,result;
    int finished,joining,joined,detached,foreign;
    struct Thread *next;
} Thread;
static ThreadAttr *attributes;
static Thread *threads;
static uint32_t next_tid=2,main_handle;
static pthread_t main_native;
static _Thread_local Thread *current;
// Completion of a worker is published under join_lock so that timed and
// non-blocking joins can wait without polling. Lock order: gate, then join_lock.
static pthread_mutex_t join_lock=PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t join_cond=PTHREAD_COND_INITIALIZER;
static pthread_key_t foreign_key;
static ThreadAttr *find_attr(uint32_t address){for(ThreadAttr *a=attributes;a;a=a->next)if(a->address==address)return a;return NULL;}
static Thread *find_thread(uint32_t handle){for(Thread *t=threads;t;t=t->next)if(t->handle==handle)return t;return NULL;}
static void unlink_thread(Thread *t){Thread **link=&threads;while(*link && *link!=t)link=&(*link)->next;if(*link)*link=t->next;}
// Gate held. Frees a joined, detached or foreign record once its guest code is over.
static void release(Thread *t){unlink_thread(t);if(t->stack)rt_munmap(t->stack,t->length);rt_tls_release(t->cpu.tid);free(t);}
static void finish(Thread *t){
    pthread_mutex_lock(&join_lock);t->finished=1;pthread_cond_broadcast(&join_cond);pthread_mutex_unlock(&join_lock);
    if(t->detached)release(t);
}
static void *thread_entry(void *opaque) {
    Thread *t=opaque;current=t;
    rt_guest_enter();
    t->result=rt_invoke(&t->cpu,t->entry,&t->argument,1);
    rt_thread_destructors(&t->cpu);
    finish(t);current=NULL;
    rt_guest_leave();
    return NULL;
}
// Allocate a guest stack with a guard below and above. Gate held. Guards are
// whole host pages (16 KiB on Apple silicon): under RT_FAST_MEMORY a host page
// carries the union of its guest pages' rights, so a 4 KiB guard sharing a
// host page with the stack would not fault. The stack itself keeps the size
// the guest asked for (rounded to 4 KiB) and starts on a host page.
static int map_stack(CPU *c,Thread *t,uint32_t size){
    uint32_t page=(uint32_t)rt_host_page_size();
    size=(size+4095)&~4095u;
    t->length=((page+size+page-1)&~(page-1))+page;   // garde basse, pile, garde haute d'au moins une page hôte entière
    t->stack=rt_mmap(c,0,t->length,3,0x22,-1,0);
    if(t->stack==UINT32_MAX){t->stack=0;return -1;}
    if(rt_mprotect(t->stack,page,0) || rt_mprotect(t->stack+page+size,t->length-page-size,0))rt_trap(c,c->eip,"guest thread stack protection failed");
    t->cpu.esp=t->stack+page+size;
    return 0;
}
static int linux_signal_valid(uint32_t signal){return signal<=64;}
// Gate held; waits for t to finish (deadline NULL = no wait), then reaps it:
// the record, its stack, TCB and TLS blocks are released (a later join or
// detach of the same handle gets ESRCH, as after a detached thread's end).
static int join(Thread *t,uint32_t output,const struct timespec *deadline,int nonblocking){
    if(t==current)return RT_EDEADLK;
    if(t->joined || t->joining || t->detached || t->foreign)return EINVAL;
    if(!t->finished && nonblocking)return RT_EBUSY;
    t->joining=1;
    unsigned saved;int error=0;
    if(deadline){
        pthread_mutex_lock(&join_lock);saved=rt_guest_suspend();
        while(!t->finished && !error)error=pthread_cond_timedwait(&join_cond,&join_lock,deadline);
        if(t->finished)error=0;
        pthread_mutex_unlock(&join_lock);rt_guest_resume(saved);
    }
    if(!error){saved=rt_guest_suspend();error=pthread_join(t->native,NULL);rt_guest_resume(saved);}
    t->joining=0;
    if(!error){t->joined=1;if(output)W32(output,t->result);release(t);}
    return error==ETIMEDOUT?RT_ETIMEDOUT:error;
}
int rt_thread_shim(CPU *c,const char *name) {
    if(!c->tid)c->tid=1;
    if(c->tid==1 && !main_handle){main_handle=rt_thread_pointer(c);main_native=pthread_self();}
    uint32_t address=rt_arg32(c,0);int error=0;
    if(!strcmp(name,"pthread_self"))c->eax=rt_thread_pointer(c);
    else if(!strcmp(name,"pthread_equal"))c->eax=address==rt_arg32(c,1);
    else if(!strcmp(name,"pthread_attr_init")) {
        if(!address || find_attr(address))error=EINVAL;
        else {ThreadAttr *a=calloc(1,sizeof(*a));if(!a)error=ENOMEM;
            else {a->address=address;a->size=8*1024*1024;a->next=attributes;attributes=a;memset(rt_write_span(c,address,36),0,36);}}
        c->eax=error;
    } else if(!strcmp(name,"pthread_attr_setstacksize") || !strcmp(name,"pthread_attr_destroy")) {
        ThreadAttr **link=&attributes;while(*link && (*link)->address!=address)link=&(*link)->next;
        if(!*link)error=EINVAL;
        else if(!strcmp(name,"pthread_attr_destroy")){ThreadAttr *a=*link;*link=a->next;free(a);}
        else {uint32_t size=rt_arg32(c,1);if(size<16384 || size>256*1024*1024)error=EINVAL;else (*link)->size=size;}
        c->eax=error;
    } else if(!strcmp(name,"pthread_create")) {
        uint32_t attr_address=rt_arg32(c,1);ThreadAttr *attr=find_attr(attr_address);
        if(attr_address && !attr){c->eax=EINVAL;return 1;}
        Thread *t=calloc(1,sizeof(*t));if(!t){c->eax=ENOMEM;return 1;}
        rt_cpu_reset(&t->cpu);t->cpu.tid=next_tid++;
        t->cpu.fcw=c->fcw;t->cpu.mxcsr=c->mxcsr;
        if(map_stack(c,t,attr?attr->size:8*1024*1024)){free(t);c->eax=RT_EAGAIN;return 1;}
        t->handle=rt_thread_pointer(&t->cpu);
        t->entry=rt_arg32(c,2);t->argument=rt_arg32(c,3);
        pthread_attr_t native_attr;error=pthread_attr_init(&native_attr);
        if(!error){
            error=pthread_attr_setstacksize(&native_attr,16*1024*1024);
            // Threads du jeu (mixage audio, travaux) : même qualité de service que le thread principal.
            if(!error)error=pthread_attr_set_qos_class_np(&native_attr,QOS_CLASS_USER_INTERACTIVE,0);
            if(!error)error=pthread_create(&t->native,&native_attr,thread_entry,t);
            pthread_attr_destroy(&native_attr);
        }
        if(error){rt_munmap(t->stack,t->length);rt_tls_release(t->cpu.tid);free(t);}   // TCB alloué par rt_thread_pointer
        else {t->next=threads;threads=t;W32(address,t->handle);}
        c->eax=error==EAGAIN?RT_EAGAIN:(uint32_t)error;
    } else if(!strcmp(name,"pthread_join") || !strcmp(name,"pthread_tryjoin_np") || !strcmp(name,"pthread_timedjoin_np")) {
        Thread *t=find_thread(address);uint32_t output=rt_arg32(c,1);
        if(!t){c->eax=RT_ESRCH;return 1;}
        struct timespec deadline={0};int timed=!strcmp(name,"pthread_timedjoin_np");
        if(timed && !t->finished && rt_read_timespec(c,rt_arg32(c,2),&deadline)){c->eax=EINVAL;return 1;}
        c->eax=(uint32_t)join(t,output,timed?&deadline:NULL,!strcmp(name,"pthread_tryjoin_np"));
    } else if(!strcmp(name,"pthread_detach")) {
        Thread *t=find_thread(address);
        if(!t)error=address==main_handle?0:RT_ESRCH;
        else if(t->joined || t->joining || t->detached || t->foreign)error=EINVAL;
        else {
            error=pthread_detach(t->native);
            if(!error){t->detached=1;if(t->finished)release(t);}
        }
        c->eax=(uint32_t)error;
    } else if(!strcmp(name,"pthread_kill")) {
        Thread *t=find_thread(address);uint32_t signal=rt_arg32(c,1);
        int alive=address==main_handle || (t && !t->finished && !t->joined);
        if(!alive)c->eax=RT_ESRCH;
        else if(!linux_signal_valid(signal))c->eax=EINVAL;
        else if(signal)rt_trap(c,c->eip,signal==9?"pthread_kill(SIGKILL) terminates the whole Linux process; not emulated":"guest signal delivery to a thread is not implemented");
        else c->eax=0;
    } else if(!strcmp(name,"pthread_cancel")) {
        rt_trap(c,c->eip,"pthread_cancel: asynchronous cancellation of recompiled code is not supported");
    } else if(!strcmp(name,"pthread_yield")) {
        unsigned saved=rt_guest_suspend();sched_yield();rt_guest_resume(saved);c->eax=0;
    } else if(!strcmp(name,"pthread_setname_np")) {
        // Linux : 15 octets au plus (ERANGE). Darwin ne nomme que le thread courant ; le nom
        // d'un autre thread ne sert qu'aux outils de débogage et n'est pas appliqué.
        uint32_t length=rt_string_length(c,rt_arg32(c,1));
        if(length>15){c->eax=RT_ERANGE;return 1;}
        if(address==rt_thread_pointer(c))pthread_setname_np((const char *)GPTR(rt_arg32(c,1)));
        c->eax=0;
    } else if(!strcmp(name,"pthread_setaffinity_np")) {
        // Darwin has no CPU pinning on Apple silicon. The mask is validated as
        // Linux would (ESRCH, EINVAL for no online CPU), then left as a hint.
        Thread *t=find_thread(address);uint32_t size=rt_arg32(c,1);
        if(!(address==main_handle || (t && !t->joined && !t->finished))){c->eax=RT_ESRCH;return 1;}
        const uint8_t *mask=rt_span(c,rt_arg32(c,2),size);
        int cpus=1;size_t length=sizeof(cpus);
        if(sysctlbyname("hw.logicalcpu",&cpus,&length,NULL,0) || cpus<1)cpus=1;
        int any=0;for(uint32_t bit=0;bit<(uint32_t)cpus && bit/8<size;++bit)if(mask[bit/8]>>(bit%8)&1)any=1;
        c->eax=any?0:EINVAL;
    } else if(!strcmp(name,"pthread_getschedparam") || !strcmp(name,"pthread_setschedparam")) {
        Thread *t=find_thread(address);pthread_t native;
        if(t && !t->joined && !t->foreign)native=t->native;
        else if(address==main_handle)native=main_native;
        else {c->eax=RT_ESRCH;return 1;}
        struct sched_param param;int policy;
        if(!strcmp(name,"pthread_getschedparam")){
            error=pthread_getschedparam(native,&policy,&param);
            if(!error){
                uint32_t guest_policy=policy==SCHED_FIFO?1:policy==SCHED_RR?2:0,priority=(uint32_t)param.sched_priority;
                uint8_t *policy_out=rt_write_span(c,rt_arg32(c,1),4),*param_out=rt_write_span(c,rt_arg32(c,2),4);
                memcpy(policy_out,&guest_policy,4);memcpy(param_out,&priority,4);
            }
        } else {
            uint32_t guest_policy=rt_arg32(c,1);
            if(guest_policy>2){c->eax=EINVAL;return 1;}
            policy=guest_policy==1?SCHED_FIFO:guest_policy==2?SCHED_RR:SCHED_OTHER;
            memset(&param,0,sizeof(param));int32_t priority;memcpy(&priority,rt_span(c,rt_arg32(c,2),4),4);param.sched_priority=priority;
            error=pthread_setschedparam(native,policy,&param);
        }
        c->eax=(uint32_t)error;
    } else if(!strcmp(name,"pthread_exit")) {
        if(!current)rt_trap(c,c->eip,"pthread_exit outside worker thread");
        if(current->foreign)rt_trap(c,c->eip,"pthread_exit on a foreign host thread (would end a library thread)");
        Thread *t=current;t->result=address;rt_thread_destructors(c);current=NULL;finish(t);
        rt_guest_suspend();pthread_exit(NULL);
    } else return 0;
    return 1;
}
// sched_yield is not a pthread_* name, so it needs its own provider.
static int sched_shim(CPU *c,const char *name){
    if(strcmp(name,"sched_yield"))return 0;
    unsigned saved=rt_guest_suspend();sched_yield();rt_guest_resume(saved);c->eax=0;
    return 1;
}
RT_SHIM_PROVIDER(sched_shim)

// Foreign host threads (CoreAudio/SDL audio threads...) calling guest code.
static void foreign_release(Thread *t){
    rt_guest_enter();
    current=t;rt_thread_destructors(&t->cpu);current=NULL;
    release(t);
    rt_guest_leave();
}
// Runs at host thread exit if the bridge never detached. _Thread_local data may
// already be reset here; the guest gate and errno simply start from zero.
// Darwin clears TSD slots in ascending key order, even slots without a native
// destructor: creating this key before main makes it run while the guest
// pthread_key values of the thread are still readable.
static void foreign_exit(void *opaque){foreign_release(opaque);}
__attribute__((constructor)) static void foreign_key_create(void){if(pthread_key_create(&foreign_key,foreign_exit))abort();}
CPU *rt_foreign_thread_attach(void){
    if(current)return &current->cpu;
    rt_guest_enter();
    Thread *t=calloc(1,sizeof(*t));
    if(!t){fputs("cannot attach foreign thread\n",stderr);abort();}
    rt_cpu_reset(&t->cpu);t->cpu.tid=next_tid++;t->foreign=1;t->native=pthread_self();
    if(map_stack(&t->cpu,t,1024*1024)){fputs("cannot map foreign thread guest stack\n",stderr);abort();}
    t->handle=rt_thread_pointer(&t->cpu);
    t->next=threads;threads=t;current=t;
    rt_guest_leave();
    if(pthread_setspecific(foreign_key,t))abort();
    return &t->cpu;
}
void rt_foreign_thread_detach(void){
    Thread *t=current;
    if(!t || !t->foreign)return;
    pthread_setspecific(foreign_key,NULL);
    foreign_release(t);
}
void rt_threads_destroy(void) {
    // Refuse to unmap a CPU or TLS while a native worker can still access it.
    for(Thread *t=threads;t;t=t->next){
        if(t->foreign){fputs("cannot destroy runtime with an attached foreign thread\n",stderr);abort();}
        if(t->joined)continue;
        if(!t->finished){fputs("cannot destroy runtime with a live guest worker\n",stderr);abort();}
        unsigned saved=rt_guest_suspend();pthread_join(t->native,NULL);rt_guest_resume(saved);
    }
    while(threads){Thread *next=threads->next;free(threads);threads=next;}
    while(attributes){ThreadAttr *next=attributes->next;free(attributes);attributes=next;}
    next_tid=2;main_handle=0;current=NULL;
}
