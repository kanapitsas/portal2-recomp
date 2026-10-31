// FUTEX_WAIT/WAKE for original Linux allocator code. A waiter owns a native
// condition and is published before releasing the guest gate, avoiding lost
// wakes between the expected-value check and sleeping.
#include "rt.h"
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

typedef struct Waiter { pthread_cond_t condition; int awoken; struct Waiter *next; } Waiter;
typedef struct Futex { uint32_t address; pthread_mutex_t mutex; Waiter *waiters; struct Futex *next; } Futex;
static Futex *futexes;
int rt_futex(CPU *c,uint32_t address,uint32_t operation,uint32_t value,uint32_t timeout) {
    if(address&3){errno=EINVAL;return -1;}
    if(operation & ~0x81u)rt_trap(c,c->eip,"unimplemented futex operation");
    unsigned command=operation&0x7f;
    if(command==1 && (int32_t)value<0){errno=EINVAL;return -1;}
    rt_guest_enter();
    Futex *f=futexes;while(f && f->address!=address)f=f->next;
    if(!f){
        f=calloc(1,sizeof(*f));if(!f){rt_guest_leave();errno=ENOMEM;return -1;}
        if(pthread_mutex_init(&f->mutex,NULL)){free(f);rt_guest_leave();errno=ENOMEM;return -1;}
        f->address=address;f->next=futexes;futexes=f;
    }
    pthread_mutex_lock(&f->mutex);
    int result=0,error=0;
    if(command==1){
        for(Waiter *waiter=f->waiters;waiter && (uint32_t)result<value;waiter=waiter->next)
            if(!waiter->awoken){waiter->awoken=1;pthread_cond_signal(&waiter->condition);++result;}
    } else if(R32(address)!=value)error=EAGAIN;
    else {
        struct timespec deadline={0};
        if(timeout){
            int32_t seconds=(int32_t)R32(timeout),nanoseconds=(int32_t)R32(timeout+4);
            if(seconds<0 || nanoseconds<0 || nanoseconds>=1000000000)error=EINVAL;
            else {clock_gettime(CLOCK_REALTIME,&deadline);deadline.tv_sec+=seconds;deadline.tv_nsec+=nanoseconds;
                if(deadline.tv_nsec>=1000000000){++deadline.tv_sec;deadline.tv_nsec-=1000000000;}}
        }
        if(!error){
            Waiter waiter={0};error=pthread_cond_init(&waiter.condition,NULL);
            if(!error){
                waiter.next=f->waiters;f->waiters=&waiter;
                unsigned saved=rt_guest_suspend();
                while(!waiter.awoken && !error)
                    error=timeout?pthread_cond_timedwait(&waiter.condition,&f->mutex,&deadline):pthread_cond_wait(&waiter.condition,&f->mutex);
                // Réveil compté par FUTEX_WAKE pendant l'expiration : succès, sinon
                // ce réveil serait perdu pour les autres attendeurs.
                if(waiter.awoken)error=0;
                Waiter **link=&f->waiters;while(*link && *link!=&waiter)link=&(*link)->next;
                if(*link)*link=waiter.next;
                pthread_mutex_unlock(&f->mutex);
                rt_guest_resume(saved);
                pthread_cond_destroy(&waiter.condition);
                rt_guest_leave();
                if(error){errno=error;return -1;}return 0;
            }
        }
    }
    pthread_mutex_unlock(&f->mutex);rt_guest_leave();
    if(error){errno=error;return -1;}return result;
}
void rt_futex_destroy(void) {
    while(futexes){Futex *next=futexes->next;if(futexes->waiters)abort();pthread_mutex_destroy(&futexes->mutex);free(futexes);futexes=next;}
}
