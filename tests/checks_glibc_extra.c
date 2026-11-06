// glibc imports first needed by Half-Life 2 (20th anniversary build): fortified
// and XSI variants, identities, fcntl, signal, valloc, dladdr. Expected values
// come from glibc's documented behaviour, not from the shims under test.
#include "checks.h"
#include <fcntl.h>
#include <stdarg.h>
#include <unistd.h>

#define STACK 0x20010000u
#define DATA 0x20014000u

static uint32_t next_data;
static uint32_t put(const void *bytes, uint32_t size) {
    uint32_t address = next_data; next_data += (size + 15) & ~15u;
    assert(next_data <= 0x20020000u);
    memcpy(GPTR(address), bytes, size);
    return address;
}
static uint32_t str(const char *s) { return put(s, (uint32_t)strlen(s) + 1); }
static uint32_t call(const char *name, unsigned n, ...) {
    va_list list; va_start(list, n);
    for (unsigned i = 0; i < n; ++i) W32(STACK + 4 * i, va_arg(list, uint32_t));
    va_end(list);
    CPU c; rt_cpu_reset(&c); c.esp = STACK;
    rt_call(&c, rt_dynamic_import(name), RT_RETURN_SENTINEL);
    assert(c.esp == STACK);
    return c.eax;
}

RT_CHECK(glibc_extra_strings) {
    next_data = DATA;
    uint32_t path = str("/usr/lib//");
    uint32_t base = call("__xpg_basename", 1, path);
    assert(!strcmp(GPTR(base), "lib") && base == path + 5 && R8(path + 8) == 0);   // '/' finaux retirés
    assert(!strcmp(GPTR(call("__xpg_basename", 1, str("///"))), "/"));
    assert(!strcmp(GPTR(call("__xpg_basename", 1, str(""))), "."));
    assert(!strcmp(GPTR(call("__xpg_basename", 1, 0)), "."));
    assert(!strcmp(GPTR(call("__xpg_basename", 1, str("name"))), "name"));

    uint32_t buffer = put("", 64), tiny = put("", 4);
    assert(call("__xpg_strerror_r", 3, 2, buffer, 64) == 0 && !strcmp(GPTR(buffer), "No such file or directory"));
    assert(call("__xpg_strerror_r", 3, 2, tiny, 4) == 34 && !strcmp(GPTR(tiny), "No "));   // ERANGE, tronqué
    assert(call("__xpg_strerror_r", 3, 9999, buffer, 64) == 22 && !strcmp(GPTR(buffer), "Unknown error 9999"));

    uint32_t text = str("abcdef");
    assert(call("__rawmemchr", 2, text, 'd') == text + 3);
    assert(call("__rawmemchr", 2, text, 0) == text + 6);

    uint32_t line = str("a,b"), separators = str(","), save = put("\0\0\0\0", 4);
    uint32_t first = call("__strtok_r", 3, line, separators, save);
    assert(first == line && !strcmp(GPTR(first), "a"));
    assert(!strcmp(GPTR(call("__strtok_r", 3, 0, separators, save)), "b"));
    assert(call("__strtok_r", 3, 0, separators, save) == 0);
}

RT_CHECK(glibc_extra_numbers_and_identity) {
    next_data = DATA;
    assert(call("__fdelt_chk", 1, 0) == 0 && call("__fdelt_chk", 1, 33) == 1 && call("__fdelt_chk", 1, 1023) == 31);
    // rand_r : glibc 2.x, graine 1 -> 476707713 puis 1186278907 (valeurs de référence).
    uint32_t seed = put("\1\0\0\0", 4);
    assert(call("rand_r", 1, seed) == 476707713u);
    assert(call("rand_r", 1, seed) == 1186278907u);
    assert(call("geteuid", 0) == (uint32_t)geteuid() && call("getgid", 0) == (uint32_t)getgid());
    assert(call("getegid", 0) == (uint32_t)getegid());
}

RT_CHECK(glibc_extra_fcntl_linux_values) {
    int fds[2]; assert(!pipe(fds));
    assert((call("fcntl", 2, fds[0], 3) & 3) == 0);                       // F_GETFL : O_RDONLY
    assert(call("fcntl", 3, fds[0], 4, 0x800) == 0);                      // F_SETFL O_NONBLOCK (Linux)
    assert(call("fcntl", 2, fds[0], 3) & 0x800);
    assert(fcntl(fds[0], F_GETFL) & O_NONBLOCK);                           // vu de l'hôte
    assert(call("fcntl", 3, fds[0], 2, 1) == 0 && call("fcntl", 2, fds[0], 1) == 1);   // FD_CLOEXEC
    int copy = (int)call("fcntl64", 3, fds[1], 1030, 100);                 // F_DUPFD_CLOEXEC
    assert(copy >= 100 && (fcntl(copy, F_GETFD) & FD_CLOEXEC));
    assert(call("fcntl", 3, fds[0], 6, 0) == UINT32_MAX);                 // F_SETLK : non traduit
    close(copy); close(fds[0]); close(fds[1]);
}

RT_CHECK(glibc_extra_signal_and_valloc) {
    assert(call("signal", 2, 13, 1) == 0);    // SIGPIPE -> SIG_IGN, ancien SIG_DFL
    assert(call("signal", 2, 13, 0) == 1);    // retour à SIG_DFL, ancien SIG_IGN
    assert(call("signal", 2, 9, 1) == UINT32_MAX);   // SIGKILL : SIG_ERR
    uint32_t small = call("valloc", 1, 100), large = call("valloc", 1, 0x12345);
    assert(small && !(small & 0xfff) && large && !(large & 0xfff) && small != large);
    assert(rt_heap_usable_size(small) >= 100 && rt_heap_usable_size(large) >= 0x12345);
    assert(!rt_free(small) && !rt_free(large));
    uint32_t again = call("valloc", 1, 0x1000);
    assert(again && !(again & 0xfff) && !rt_free(again));
}

RT_CHECK(glibc_extra_dladdr_outside_modules) {
    next_data = DATA;
    uint32_t info = put("", 16);
    assert(call("dladdr", 2, 0x1000u, info) == 0);       // sous tous les modules
    assert(call("backtrace", 2, info, 0) == 0);
    W32(STACK + 8, 0);
    assert(call("backtrace", 2, info, 4) == 1 && R32(info) == RT_RETURN_SENTINEL);
}
