// Native threads share one serialized guest CPU. AOT backward branches and
// calls yield this gate; blocking native operations release it entirely.
// This keeps ordinary guest loads/stores ordered on ARM and protects registries.
#include "rt.h"
#include <pthread.h>
#include <sched.h>
#include <mach/mach_time.h>
#include <stdio.h>
#include <stdatomic.h>
#include <stdlib.h>

static pthread_mutex_t gate=PTHREAD_MUTEX_INITIALIZER;
static _Thread_local unsigned depth, ticks;
// Threads bloqués sur gate : céder seulement s'il y en a. Exporté pour que le
// code généré puisse tester sans appel (déclaration dans rt.h, branche fast-path).
_Atomic unsigned rt_guest_waiting;
// Diagnostic P2_GATE_TIMING=1 : temps d'attente du verrou par le thread principal.
static uint64_t main_wait_ticks, main_waits, report_last, report_start;
static void gate_report(uint64_t now) {
    static mach_timebase_info_data_t base;
    if(!base.denom)mach_timebase_info(&base);
    if(!report_start){report_start=report_last=now;return;}
    if((now-report_last)*base.numer/base.denom<5000000000ull)return;
    report_last=now;
    double wall=(double)(now-report_start)*base.numer/base.denom/1e9;
    double waited=(double)main_wait_ticks*base.numer/base.denom/1e9;
    fprintf(stderr,"[gate] thread principal : %.1f%% du temps à attendre le verrou (%llu attentes sur %.0f s)\n",
            100.0*waited/wall,(unsigned long long)main_waits,wall);
}
static void acquire(void) {
    if(!pthread_mutex_trylock(&gate))return;
    // Lu avant la prise du verrou : atomique, plusieurs threads peuvent y passer en même temps.
    static _Atomic int timing=-1;
    int enabled=atomic_load_explicit(&timing,memory_order_relaxed);
    if(enabled<0){enabled=getenv("P2_GATE_TIMING")!=NULL;atomic_store_explicit(&timing,enabled,memory_order_relaxed);}
    int measure=enabled && pthread_main_np();
    uint64_t start=measure?mach_absolute_time():0;
    ++rt_guest_waiting;
    if(pthread_mutex_lock(&gate))abort();
    --rt_guest_waiting;
    if(measure){uint64_t now=mach_absolute_time();main_wait_ticks+=now-start;++main_waits;gate_report(now);}
}
void rt_guest_enter(void) {
    if(!depth)acquire();
    ++depth;
    rt_guest_poll();
}
void rt_guest_leave(void) {
    if(!depth)abort();
    if(!--depth && pthread_mutex_unlock(&gate))abort();
}
unsigned rt_guest_suspend(void) {
    unsigned saved=depth;depth=0;
    if(saved && pthread_mutex_unlock(&gate))abort();
    return saved;
}
void rt_guest_resume(unsigned saved) {
    if(depth)abort();
    if(saved)acquire();
    depth=saved;
}
void rt_guest_poll(void) {
    // Test global d'abord : les _Thread_local coûtent un appel _tlv_get_addr sur macOS.
    if(!atomic_load_explicit(&rt_guest_waiting,memory_order_relaxed))return;
    if(!depth || (++ticks & 1023))return;
    unsigned saved=rt_guest_suspend();
    sched_yield();
    rt_guest_resume(saved);
}
