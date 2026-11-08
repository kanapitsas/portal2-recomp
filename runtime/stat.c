// i386 glibc stat/stat64 layouts, not Darwin's native struct stat.
// Layout: glibc sysdeps/unix/sysv/linux/x86/bits/struct_stat.h.
#include "rt.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

static int write_stat(CPU *c,uint32_t address,const struct stat *value,int wide) {
    if(!wide && (value->st_size>INT32_MAX || value->st_blocks>INT32_MAX || value->st_ino>UINT32_MAX)){errno=EOVERFLOW;return -1;}
    if(value->st_atimespec.tv_sec>INT32_MAX || value->st_atimespec.tv_sec<INT32_MIN ||
       value->st_mtimespec.tv_sec>INT32_MAX || value->st_mtimespec.tv_sec<INT32_MIN ||
       value->st_ctimespec.tv_sec>INT32_MAX || value->st_ctimespec.tv_sec<INT32_MIN){errno=EOVERFLOW;return -1;}
    memset(rt_write_span(c,address,wide?96:88),0,wide?96:88);
    W64(address,(uint64_t)value->st_dev);W32(address+12,(uint32_t)value->st_ino);
    W32(address+16,value->st_mode);W32(address+20,value->st_nlink);
    W32(address+24,value->st_uid);W32(address+28,value->st_gid);W64(address+32,(uint64_t)value->st_rdev);
    unsigned times;
    if(wide){W64(address+44,(uint64_t)value->st_size);W32(address+52,(uint32_t)value->st_blksize);W64(address+56,(uint64_t)value->st_blocks);W64(address+88,value->st_ino);times=64;}
    else{W32(address+44,(uint32_t)value->st_size);W32(address+48,(uint32_t)value->st_blksize);W32(address+52,(uint32_t)value->st_blocks);times=56;}
    W32(address+times,(uint32_t)value->st_atimespec.tv_sec);W32(address+times+4,(uint32_t)value->st_atimespec.tv_nsec);
    W32(address+times+8,(uint32_t)value->st_mtimespec.tv_sec);W32(address+times+12,(uint32_t)value->st_mtimespec.tv_nsec);
    W32(address+times+16,(uint32_t)value->st_ctimespec.tv_sec);W32(address+times+20,(uint32_t)value->st_ctimespec.tv_nsec);
    return 0;
}
int rt_stat_shim(CPU *c,const char *name) {
    const char *supported[]={"stat","stat64","lstat","lstat64","fstat","fstat64",
        "__xstat","__xstat64","__lxstat","__lxstat64","__fxstat","__fxstat64"};
    int recognized=0;
    for(unsigned i=0;i<sizeof(supported)/sizeof(*supported);++i)if(!strcmp(name,supported[i]))recognized=1;
    if(!recognized)return 0;
    int versioned=!strncmp(name,"__xstat",7)||!strncmp(name,"__lxstat",8)||!strncmp(name,"__fxstat",8);
    int fd_call=!strncmp(name,"__fxstat",8)||!strncmp(name,"fstat",5);
    int link_call=!strncmp(name,"__lxstat",8)||!strncmp(name,"lstat",5);
    unsigned arg=versioned?1:0;
    if(versioned && R32(c->esp+4)!=3){rt_set_errno(c,RT_EINVAL);c->eax=UINT32_MAX;return 1;}
    uint32_t target=R32(c->esp+4+arg*4),destination=R32(c->esp+8+arg*4);
    struct stat native;int result;
    if(fd_call)result=fstat((int32_t)target,&native);
    else{
        rt_string_length(c,target);const char *path=rt_read_path(GPTR(target));
        result=path?(link_call?lstat(path,&native):stat(path,&native)):-1;
    }
    static _Atomic int trace=-1;
    int tracing=atomic_load_explicit(&trace,memory_order_relaxed);
    if(tracing<0){tracing=getenv("P2_TRACE_STAT")!=NULL;atomic_store_explicit(&trace,tracing,memory_order_relaxed);}
    if(tracing&&!fd_call)fprintf(stderr,"[stat] %s %s -> %d\n",name,(const char *)GPTR(target),result);
    if(!result)result=write_stat(c,destination,&native,strstr(name,"64")!=NULL);
    if(result)rt_save_errno(c);
    c->eax=(uint32_t)result;return 1;
}
