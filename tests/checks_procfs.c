#include "checks.h"
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sysctl.h>
#include <unistd.h>

#define STACK 0x20010000u
#define DATA 0x20016000u

static uint32_t call(const char *name, unsigned n, ...) {
    va_list list; va_start(list, n);
    for (unsigned i = 0; i < n; ++i) W32(STACK + 4 * i, va_arg(list, uint32_t));
    va_end(list);
    CPU c; rt_cpu_reset(&c); c.esp = STACK;
    rt_call(&c, rt_dynamic_import(name), RT_RETURN_SENTINEL);
    return c.eax;
}
static uint32_t gstr(uint32_t at, const char *s) { memcpy(GPTR(at), s, strlen(s) + 1); return at; }

// Lit un fichier invité par fopen/fread des shims, comme le jeu.
static size_t guest_read(const char *path, char *out, size_t size) {
    uint32_t file = call("fopen", 2, gstr(DATA, path), gstr(DATA + 0x100, "r"));
    if (!file) return 0;
    uint32_t got = call("fread", 4, DATA + 0x200, 1, (uint32_t)size - 1, file);
    memcpy(out, GPTR(DATA + 0x200), got); out[got] = 0;
    call("fclose", 1, file);
    return got;
}

RT_CHECK(procfs_meminfo_reports_host_memory) {
    char text[2048];
    assert(guest_read("/proc/meminfo", text, sizeof(text)) > 0);
    unsigned long long total = 0; size_t size = sizeof(total);
    assert(!sysctlbyname("hw.memsize", &total, &size, NULL, 0));
    unsigned long long kb = 0;
    assert(sscanf(text, "MemTotal: %llu kB", &kb) == 1 && kb == total / 1024);
    assert(strstr(text, "\nMemFree:") && strstr(text, "\nSwapTotal:"));
}

RT_CHECK(procfs_cpuinfo_matches_cpuid_profile) {
    char text[4096];
    assert(guest_read("/proc/cpuinfo", text, sizeof(text)) > 0);
    assert(strstr(text, "vendor_id\t: GenuineIntel\n") && strstr(text, "cpu MHz\t\t: 1000.000\n"));
    assert(strstr(text, "flags\t\t: fpu tsc cx8 cmov mmx fxsr sse sse2\n"));
    assert(!strstr(text, "processor\t: 1"));   // un seul processeur logique, comme CPUID
}

RT_CHECK(procfs_cmdline_and_unknown_entries) {
    rt_procfs_set_cmdline("portal2_linux\0-game\0portal2", 28);
    char path[64]; snprintf(path, sizeof(path), "/proc/%d/cmdline", (int)getpid());
    const char *host = rt_proc_path(path);
    assert(host);
    FILE *f = fopen(host, "rb"); char bytes[64]; size_t n = fread(bytes, 1, sizeof(bytes), f); fclose(f);
    assert(n == 28 && !memcmp(bytes, "portal2_linux\0-game\0portal2", 28));
    errno = 0;
    assert(!rt_proc_path("/proc/self/maps") && errno == ENOENT);
    assert(rt_is_proc_path("/proc") && rt_is_proc_path("/proc/x") && !rt_is_proc_path("/procfs"));
}

RT_CHECK(procfs_other_process_cmdline_is_real) {
    char path[64]; snprintf(path, sizeof(path), "/proc/%d/cmdline", (int)getppid());
    const char *host = rt_proc_path(path);
    assert(host);
    // Lecture complète : une ligne de commande parente longue ne doit pas être
    // tronquée avant son NUL final.
    FILE *f = fopen(host, "rb"); static char bytes[1 << 16]; size_t n = fread(bytes, 1, sizeof(bytes), f); fclose(f);
    assert(n > 0 && n < sizeof(bytes) && bytes[n - 1] == 0);   // arguments réels du parent, séparés par des NUL
}
