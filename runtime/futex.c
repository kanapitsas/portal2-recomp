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
    // FUTEX_WAKE sans attendeur connu : rien à réveiller, pas d'entrée créée.
    if(!f && command==1){rt_guest_leave();return 0;}
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
        struct timespec deadline={0},relative;
        if(timeout){
            if(rt_read_timespec(c,timeout,&relative) || relative.tv_sec<0)error=EINVAL;
            // FUTEX_WAIT relatif : horloge monotone sous Linux (un réglage de
            // l'heure ne l'allonge ni ne l'écourte), d'où l'échéance monotone
            // et pthread_cond_timedwait_relative_np sur le temps restant.
            else {clock_gettime(CLOCK_MONOTONIC,&deadline);deadline.tv_sec+=relative.tv_sec;deadline.tv_nsec+=relative.tv_nsec;
                if(deadline.tv_nsec>=1000000000){++deadline.tv_sec;deadline.tv_nsec-=1000000000;}}
        }
        if(!error){
            Waiter waiter={0};error=pthread_cond_init(&waiter.condition,NULL);
            if(!error){
                waiter.next=f->waiters;f->waiters=&waiter;
                unsigned saved=rt_guest_suspend();
                while(!waiter.awoken && !error){
                    if(!timeout){error=pthread_cond_wait(&waiter.condition,&f->mutex);continue;}
                    struct timespec now,left;clock_gettime(CLOCK_MONOTONIC,&now);
                    left.tv_sec=deadline.tv_sec-now.tv_sec;left.tv_nsec=deadline.tv_nsec-now.tv_nsec;
                    if(left.tv_nsec<0){--left.tv_sec;left.tv_nsec+=1000000000;}
                    if(left.tv_sec<0 || (left.tv_sec==0 && left.tv_nsec==0))error=ETIMEDOUT;
                    else error=pthread_cond_timedwait_relative_np(&waiter.condition,&f->mutex,&left);
                }
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
size_t rt_futex_count(void) {   // pour les tests
    rt_guest_enter();size_t count=0;for(Futex *f=futexes;f;f=f->next)++count;rt_guest_leave();return count;
}
void rt_futex_destroy(void) {
    while(futexes){Futex *next=futexes->next;if(futexes->waiters)abort();pthread_mutex_destroy(&futexes->mutex);free(futexes);futexes=next;}
}
