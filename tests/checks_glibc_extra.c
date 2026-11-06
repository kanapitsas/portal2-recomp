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
    assert(call("fcntl", 3, fds[0], 9, 0) == UINT32_MAX);                 // F_SETOWN : non traduit
    char lock_path[] = "/tmp/glibc-extra-lockXXXXXX"; int lockfd = mkstemp(lock_path); assert(lockfd >= 0);
    next_data = DATA;
    uint32_t lock = put("\1\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0", 16);       // F_WRLCK Linux, tout le fichier
    assert(call("fcntl", 3, lockfd, 6, lock) == 0);                        // F_SETLK
    struct flock seen = {.l_type = F_WRLCK, .l_whence = SEEK_SET};
    assert(!fcntl(lockfd, F_GETLK, &seen) && seen.l_type == F_UNLCK);      // le verrou est à nous
    W16(lock, 2);                                                          // F_UNLCK Linux
    assert(call("fcntl", 3, lockfd, 6, lock) == 0);
    W16(lock, 7);
    assert(call("fcntl", 3, lockfd, 6, lock) == UINT32_MAX);              // type invalide
    close(lockfd); unlink(lock_path);
    close(copy); close(fds[0]); close(fds[1]);
}

RT_CHECK(glibc_extra_signal_and_valloc) {
    assert(call("signal", 2, 13, 1) == 0);    // SIGPIPE -> SIG_IGN, ancien SIG_DFL
    assert(call("signal", 2, 13, 0) == 1);    // retour à SIG_DFL, ancien SIG_IGN
    assert(call("signal", 2, 9, 1) == UINT32_MAX);   // SIGKILL : SIG_ERR
    assert(call("signal", 2, 4, 0x12345678u) == 0);  // SIGILL des sondes Crypto++ : enregistré
    assert(call("signal", 2, 4, 0) == 0x12345678u);
    assert(call("signal", 2, 11, 0x12345678u) == 0 && call("signal", 2, 11, 0) == 0x12345678u);   // vaudio_miles
    uint32_t small = call("valloc", 1, 100), large = call("valloc", 1, 0x12345);
    assert(small && !(small & 0xfff) && large && !(large & 0xfff) && small != large);
    assert(rt_heap_usable_size(small) >= 100 && rt_heap_usable_size(large) >= 0x12345);
    assert(!rt_free(small) && !rt_free(large));
    uint32_t m16 = call("memalign", 2, 16, 40), m256 = call("memalign", 2, 256, 40);
    assert(m16 && !(m16 & 15) && m256 && !(m256 & 255) && !rt_free(m16) && !rt_free(m256));
    assert(call("memalign", 2, 24, 40) == 0);                                 // pas une puissance de 2
    next_data = DATA; uint32_t out = put("\0\0\0\0", 4);
    assert(call("posix_memalign", 3, out, 64, 100) == 0 && R32(out) && !(R32(out) & 63) && !rt_free(R32(out)));
    assert(call("posix_memalign", 3, out, 2, 100) == 22);                     // < sizeof(void *)
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

static double st0(CPU *c) { return c->st[c->ftop & 7]; }
RT_CHECK(glibc_extra_math_and_wide) {
    next_data = DATA;
    W32(STACK, 0); WD(STACK, 2.25);
    CPU c; rt_cpu_reset(&c); c.esp = STACK; rt_call(&c, rt_dynamic_import("sqrt"), RT_RETURN_SENTINEL);
    assert(st0(&c) == 1.5);
    WF(STACK, 3.0f); rt_cpu_reset(&c); c.esp = STACK; rt_call(&c, rt_dynamic_import("exp2f"), RT_RETURN_SENTINEL);
    assert(st0(&c) == 8.0);
    WF(STACK, -1.0f); WF(STACK + 4, 2.0f);
    rt_cpu_reset(&c); c.esp = STACK; rt_call(&c, rt_dynamic_import("fmaxf"), RT_RETURN_SENTINEL); assert(st0(&c) == 2.0);
    rt_cpu_reset(&c); c.esp = STACK; rt_call(&c, rt_dynamic_import("fminf"), RT_RETURN_SENTINEL); assert(st0(&c) == -1.0);

    uint32_t number = put("1\0\0\0" "2\0\0\0" "3\0\0\0" "4\0\0\0" "5\0\0\0" "6\0\0\0" "7\0\0\0" "8\0\0\0" "9\0\0\0" "0\0\0\0" "1\0\0\0" "2\0\0\0" "\0\0\0\0", 52);
    W32(STACK, number); W32(STACK + 4, 0); W32(STACK + 8, 10);
    rt_cpu_reset(&c); c.esp = STACK; rt_call(&c, rt_dynamic_import("wcstoll"), RT_RETURN_SENTINEL);
    assert(((uint64_t)c.edx << 32 | c.eax) == UINT64_C(123456789012));   // au-delà de 32 bits

    uint32_t dst = put("", 64), src = put("a\0\0\0b\0\0\0\0\0\0\0", 12);
    assert(call("__wcsncpy_chk", 4, dst, src, 4, 16) == dst && R32(dst) == 'a' && R32(dst + 4) == 'b' && R32(dst + 12) == 0);
    assert(call("__wcscat_chk", 3, dst, src, 16) == dst && R32(dst + 8) == 'a' && R32(dst + 16) == 0);
    uint32_t narrow = put("", 8);
    assert(call("__wcstombs_chk", 4, narrow, src, 8, 8) == 2 && !strcmp(GPTR(narrow), "ab"));
    uint32_t text = put("", 32), format = put("%\0\0\0d\0\0\0\0\0\0\0", 12);
    W32(STACK, text); W32(STACK + 4, 8); W32(STACK + 8, 1); W32(STACK + 12, 8); W32(STACK + 16, format); W32(STACK + 20, 42);
    rt_cpu_reset(&c); c.esp = STACK; rt_call(&c, rt_dynamic_import("__swprintf_chk"), RT_RETURN_SENTINEL);
    assert(c.eax == 2 && R32(text) == '4' && R32(text + 4) == '2' && R32(text + 8) == 0);
    uint32_t copy = put("", 8);
    assert(call("__stpcpy_chk", 3, copy, str("abc"), 8) == copy + 3 && !strcmp(GPTR(copy), "abc"));
}

RT_CHECK(glibc_extra_time_terminal_threads) {
    next_data = DATA;
    uint32_t request = put("\0\0\0\0\x40\x42\x0f\0", 8);       // 0 s, 1 ms
    assert(call("nanosleep", 2, request, 0) == 0);
    W32(request + 4, 1000000000u);
    assert(call("nanosleep", 2, request, 0) == UINT32_MAX);    // tv_nsec hors bornes : EINVAL
    assert(call("sleep", 1, 0) == 0);
    uint32_t tms = put("", 16);
    uint32_t ticks = call("times", 1, tms);
    assert(ticks > 0 && R32(tms + 4) == 0);
    int fds[2]; assert(!pipe(fds));
    assert(call("isatty", 1, fds[0]) == 0);
    assert(call("tcgetattr", 2, fds[0], put("", 64)) == UINT32_MAX);   // ENOTTY sur un tube
    close(fds[0]); close(fds[1]);
    assert(!strcmp(GPTR(call("ctermid", 1, 0)), "/dev/tty"));
    assert(call("pthread_setname_np", 2, 0x12345678u, str("a-name-longer-than-15")) == 34);   // ERANGE
    assert(call("signal", 2, 13, 1) == 0 && call("raise", 1, 13) == 0);   // SIGPIPE ignoré : raise sans effet
    assert(call("signal", 2, 13, 0) == 1);
}
