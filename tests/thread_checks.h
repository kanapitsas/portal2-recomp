#include <time.h>
static uint32_t thread_api(CPU *c,const char *name,const uint32_t *args,unsigned count) {
    for(unsigned i=0;i<count;++i)W32(c->esp+4+i*4,args[i]);
    assert(rt_pthread_shim(c,name));return c->eax;
}
static void wait_counter(uint32_t address,uint32_t expected) {
    struct timespec start,now;clock_gettime(CLOCK_MONOTONIC,&start);
    while(R32(address)!=expected){
        unsigned saved=rt_guest_suspend();usleep(1000);rt_guest_resume(saved);
        clock_gettime(CLOCK_MONOTONIC,&now);assert(now.tv_sec-start.tv_sec<5);
    }
}
static void check_threads(void) {
    CPU c;rt_cpu_reset(&c);c.esp=0x20018000;c.mxcsr=0x9fc0;
    rt_guest_enter();
    memset(GPTR(0x2000b000),0,0x200);
    uint32_t args[4]={0x2000b000,0};assert(!thread_api(&c,"pthread_mutex_init",args,2));
    args[0]=0x2000b020;assert(!thread_api(&c,"pthread_cond_init",args,2));
    args[0]=0x2000b088;args[1]=0x26000;assert(!thread_api(&c,"pthread_key_create",args,2));
    assert(rt_libc_shim(&c,"__errno_location"));uint32_t main_errno=c.eax;W32(main_errno,0xabba);
    args[0]=0x2000b0c0;assert(!thread_api(&c,"pthread_attr_init",args,1));
    args[1]=65536;assert(!thread_api(&c,"pthread_attr_setstacksize",args,2));
    for(unsigned i=0;i<2;++i){
        args[0]=0x2000b0a0+i*4;args[1]=0x2000b0c0;args[2]=0x25000;args[3]=0x2000c000+i*0x100;
        assert(!thread_api(&c,"pthread_create",args,4));
    }
    wait_counter(0x2000b080,2); // both native threads reached their AOT spin loop
    W32(0x2000b084,1);
    wait_counter(0x2000b08c,2); // both reached the condition wait
    args[0]=0x2000b000;assert(!thread_api(&c,"pthread_mutex_lock",args,1));
    W32(0x2000b084,2);
    args[0]=0x2000b020;assert(!thread_api(&c,"pthread_cond_broadcast",args,1));
    args[0]=0x2000b000;assert(!thread_api(&c,"pthread_mutex_unlock",args,1));
    for(unsigned i=0;i<2;++i){
        args[0]=R32(0x2000b0a0+i*4);args[1]=0x2000b0b0;
        assert(!thread_api(&c,"pthread_join",args,2));
        assert(R32(args[1])==0x2000c000+i*0x100);
        assert(R32(0x2000c004+i*0x100)==args[0]);
        uint32_t error=R32(0x2000c008+i*0x100);assert(error!=main_errno && R32(error)==0x2000c000+i*0x100);
        assert(R32(0x2000c010+i*0x100)==0x9fc0);
        assert(thread_api(&c,"pthread_join",args,2)==3);   // ESRCH : fiche libérée par la jonction
    }
    assert(R32(0x2000b0a0)!=R32(0x2000b0a4));
    assert(R32(0x2000c008)!=R32(0x2000c108));
    assert(R32(main_errno)==0xabba && R32(0x2000b094)==2);
    assert(R32(0x2000b098)==2 && R32(0x2000b09c)==1);
    args[0]=0x2000b000;assert(!thread_api(&c,"pthread_mutex_lock",args,1));
    W32(0x2000b0e8,0);W32(0x2000b0ec,0);
    args[0]=0x2000b020;args[1]=0x2000b000;args[2]=0x2000b0e8;
    assert(thread_api(&c,"pthread_cond_timedwait",args,3)==110);
    args[0]=0x2000b000;assert(!thread_api(&c,"pthread_mutex_unlock",args,1));
    assert(!thread_api(&c,"pthread_mutex_destroy",args,1));
    args[0]=0x2000b020;assert(!thread_api(&c,"pthread_cond_destroy",args,1));
    args[0]=R32(0x2000b088);assert(!thread_api(&c,"pthread_key_delete",args,1));
    args[0]=0x2000b0c0;assert(!thread_api(&c,"pthread_attr_destroy",args,1));
    args[0]=0x2000b0a8;args[1]=0;args[2]=0x28000;args[3]=0;
    assert(!thread_api(&c,"pthread_create",args,4));
    wait_counter(0x2000b104,1);
    int awakened=0;
    struct timespec start,now;clock_gettime(CLOCK_MONOTONIC,&start);
    while(!awakened){
        awakened=rt_futex(&c,0x2000b100,129,1,0);assert(awakened>=0);
        if(!awakened){unsigned saved=rt_guest_suspend();usleep(1000);rt_guest_resume(saved);}
        clock_gettime(CLOCK_MONOTONIC,&now);assert(now.tv_sec-start.tv_sec<5);
    }
    assert(awakened==1);
    args[0]=R32(0x2000b0a8);args[1]=0x2000b0b0;
    assert(!thread_api(&c,"pthread_join",args,2) && !R32(args[1]));
    assert(rt_futex(&c,0x2000b100,128,1,0)==-1 && errno==EAGAIN);
    assert(rt_futex(&c,0x2000b100,128,0,0x2000b0e8)==-1 && errno==ETIMEDOUT);
    assert(rt_futex(&c,0x2000b100,129,1,0)==0);
    assert(rt_futex(&c,0x2000b101,128,0,0)==-1 && errno==EINVAL);
    assert(rt_futex(&c,0x2000b100,129,UINT32_MAX,0)==-1 && errno==EINVAL);
    rt_guest_leave();
}
