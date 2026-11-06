#include "checks.h"
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

#define STACK 0x20010000u
#define DATA 0x20014000u  // 0x20014000-0x20020000 : données de ces checks

static uint32_t next_data;
static uint32_t put(const void *bytes, uint32_t size) {
    uint32_t address = next_data; next_data += (size + 15) & ~15u;
    assert(next_data <= 0x20020000u);
    memcpy(GPTR(address), bytes, size);
    return address;
}
static uint32_t str(const char *s) { return put(s, (uint32_t)strlen(s) + 1); }
static uint32_t wstr(const char *s) {  // UTF-32 Linux depuis de l'ASCII
    uint32_t n = (uint32_t)strlen(s) + 1, address = put("", 4 * n);
    for (uint32_t i = 0; i < n; ++i) W32(address + 4 * i, (uint8_t)s[i]);
    return address;
}
static uint32_t call(const char *name, unsigned n, ...) {
    va_list list; va_start(list, n);
    for (unsigned i = 0; i < n; ++i) W32(STACK + 4 * i, va_arg(list, uint32_t));
    va_end(list);
    CPU c; rt_cpu_reset(&c); c.esp = STACK;
    rt_call(&c, rt_dynamic_import(name), RT_RETURN_SENTINEL);
    assert(c.esp == STACK);
    return c.eax;
}

RT_CHECK(libc_rest_string_functions) {
    next_data = DATA;
    uint32_t s = put("abc\0zzz", 7);
    assert(call("memchr", 3, s, 'c', 7) == s + 2);
    assert(call("memchr", 3, s, 'z', 7) == s + 4);        // memchr ne s'arrête pas au NUL
    assert(call("memchr", 3, s, 'q', 7) == 0);
    uint32_t ab = str("ab"), abc = str("abc"), abd = str("abd");
    assert(call("strncmp", 3, abc, abd, 2) == 0);
    assert((int32_t)call("strncmp", 3, abc, abd, 3) < 0);
    assert((int32_t)call("strncmp", 3, ab, abc, 5) < 0);   // NUL < 'c'
    assert(call("strncmp", 3, abc, abd, 0) == 0);
    assert(call("strspn", 2, str("aabxa"), str("ab")) == 3);
    assert(call("strcspn", 2, str("hello world"), str(" o")) == 4);
}

RT_CHECK(libc_rest_strtok_keeps_process_state) {
    next_data = DATA;
    uint32_t line = str("  alpha, beta,,gamma  "), delim = str(" ,");
    uint32_t t = call("strtok", 2, line, delim);
    assert(t == line + 2 && !strcmp(GPTR(t), "alpha"));
    t = call("strtok", 2, 0, delim); assert(!strcmp(GPTR(t), "beta"));
    t = call("strtok", 2, 0, delim); assert(!strcmp(GPTR(t), "gamma"));
    assert(call("strtok", 2, 0, delim) == 0);
    assert(call("strtok", 2, 0, delim) == 0);               // reste à la fin
    uint32_t save = put("\0\0\0\0", 4), other = str("x;y");
    t = call("strtok_r", 3, other, str(";"), save); assert(!strcmp(GPTR(t), "x"));
    t = call("strtok_r", 3, 0, str(";"), save); assert(!strcmp(GPTR(t), "y"));
    assert(call("strtok_r", 3, 0, str(";"), save) == 0);
}

RT_CHECK(libc_rest_strerror_r_is_gnu) {
    next_data = DATA;
    uint32_t buffer = put("################", 16);
    uint32_t m = call("strerror_r", 3, 2, buffer, 16);     // ENOENT Linux
    assert(m != buffer && !strcmp(GPTR(m), "No such file or directory"));
    assert(R8(buffer) == '#');                              // tampon ignoré
    assert(call("strerror_r", 3, 2, buffer, 16) == m);      // chaîne stable
    m = call("strerror_r", 3, 11, buffer, 16);              // EAGAIN Linux = 11 (35 sur Darwin)
    assert(!strcmp(GPTR(m), "Resource temporarily unavailable"));
    m = call("strerror_r", 3, 9999, buffer, 10);
    assert(m == buffer && !strcmp(GPTR(m), "Unknown e"));   // tronqué et terminé
}

RT_CHECK(libc_rest_realpath_chk_aborts_on_small_buffer) {
    next_data = DATA;
    uint32_t path = str("/"), out = put("", 4096);
    // Taille suffisante : délègue à realpath (vue fusionnée du runtime).
    pid_t pid = fork();
    if (!pid) {
        uint32_t r = call("__realpath_chk", 3, path, out, 4096);
        _exit(r == out ? 0 : 1);
    }
    int status; assert(waitpid(pid, &status, 0) == pid && WIFEXITED(status) && WEXITSTATUS(status) == 0);
    pid = fork();
    if (!pid) { freopen("/dev/null", "w", stderr); call("__realpath_chk", 3, path, out, 100); _exit(0); }
    assert(waitpid(pid, &status, 0) == pid && WIFSIGNALED(status) && WTERMSIG(status) == SIGABRT);
}

RT_CHECK(libc_rest_swscanf_wide_formats) {
    next_data = DATA;
    uint32_t i = put("\0\0\0\0", 4), f = put("\0\0\0\0", 4), w = put("", 64), n = put("\0\0\0\0", 4);
    uint32_t r = call("__isoc99_swscanf", 6, wstr(" 42 2.5 mot"), wstr("%d %f %ls%n"), i, f, w, n);
    float value; memcpy(&value, GPTR(f), 4);
    assert(r == 3 && R32(i) == 42 && value == 2.5f && R32(n) == 11);
    assert(R32(w) == 'm' && R32(w + 8) == 't' && R32(w + 12) == 0);  // %ls : UTF-32
    assert(call("__isoc99_swscanf", 3, wstr("x"), wstr("%d"), i) == 0);
    assert(call("__isoc99_swscanf", 3, wstr(""), wstr("%d"), i) == UINT32_MAX);  // EOF
}

RT_CHECK(libc_rest_fscanf_on_guest_stream) {
    next_data = DATA;
    char path[] = "/tmp/p2-fscanf-XXXXXX";
    int fd = mkstemp(path); assert(fd >= 0);
    assert(write(fd, "7 eight 9", 9) == 9); close(fd);
    uint32_t stream = call("fopen", 2, str(path), str("r")); assert(stream);
    uint32_t a = put("\0\0\0\0", 4), word = put("", 16), b = put("\0\0\0\0", 4);
    assert(call("__isoc99_fscanf", 4, stream, str("%d %s"), a, word) == 2);
    assert(R32(a) == 7 && !strcmp(GPTR(word), "eight"));
    assert(call("__isoc99_fscanf", 3, stream, str("%d"), b) == 1 && R32(b) == 9);
    assert(call("__isoc99_fscanf", 3, stream, str("%d"), b) == UINT32_MAX);
    assert(call("fclose", 1, stream) == 0);
    unlink(path);
}

// Filtre de P2_TRACE_FILES : vide ou « 0 » éteint, « 1 » trace tout, sinon
// sous-chaîne du chemin invité, sans casse.
int rt_trace_files_match(const char *filter, const char *path);
RT_CHECK(libc_rest_trace_files_filter) {
    const char *path = "/game/portal2/SAVE/autosave.sav";
    assert(!rt_trace_files_match(NULL, path) && !rt_trace_files_match("", path) && !rt_trace_files_match("0", path));
    assert(rt_trace_files_match("1", path) && rt_trace_files_match("save/auto", path));
    assert(!rt_trace_files_match("maps", path) && !rt_trace_files_match("1", NULL));
}

// strlen/strcmp lisent par page : résultats de strcmp hôte, NUL de part et
// d'autre d'une frontière de page, et strcmp s'arrête au premier écart sans
// lire la suite (glibc), même si elle déborde sur une page interdite.
RT_CHECK(libc_rest_strlen_strcmp_by_page) {
    uint32_t m = rt_mmap(NULL, 0, 3 * 4096, 3, 0x22, -1, 0);
    assert(m != UINT32_MAX && !rt_mprotect(m + 2 * 4096, 4096, 0));
    uint8_t *h = GPTR(m);
    memset(h, 'x', 2 * 4096);
    h[4096 + 5] = 0;                                    // chaîne à cheval sur deux pages
    assert(call("strlen", 1, m + 4090) == 11);
    assert(call("strlen", 1, m + 4096 + 5) == 0);
    assert(call("strlen", 1, m) == 4096 + 5);
    static const char *pairs[][2] = {{"", ""}, {"a", ""}, {"", "a"}, {"abc", "abd"}, {"a\xff", "a"},
                                     {"a", "a\xff"}, {"\x80", "\x7f"}, {"same", "same"}, {"ab", "abc"}};
    for (unsigned i = 0; i < sizeof(pairs) / sizeof(*pairs); ++i)
        for (uint32_t shift = 0; shift < 3; ++shift) {   // la première chaîne finit sur la frontière
            uint32_t a = m + 4096 - (uint32_t)strlen(pairs[i][0]) - shift, b = m + 2 * 4096 - 64;
            memcpy(GPTR(a), pairs[i][0], strlen(pairs[i][0]) + 1);
            memcpy(GPTR(b), pairs[i][1], strlen(pairs[i][1]) + 1);
            assert((int32_t)call("strcmp", 2, a, b) == strcmp(pairs[i][0], pairs[i][1]));
            assert((int32_t)call("strcmp", 2, b, a) == strcmp(pairs[i][1], pairs[i][0]));
        }
    // Seconde chaîne non terminée avant la page interdite : l'écart vient avant.
    memset(h + 4096, 'b', 4096);
    uint32_t a = m + 4096 - 2; memcpy(GPTR(a), "a", 2);
    assert((int32_t)call("strcmp", 2, a, m + 4096) == 'a' - 'b');
    assert((int32_t)call("strcmp", 2, m + 4096, a) == 'b' - 'a');
    // Chaînes égales jusqu'à la page interdite : piège, comme strlen.
    pid_t pid = fork();
    if (!pid) { freopen("/dev/null", "w", stderr); call("strcmp", 2, m + 4096, m + 4096); _exit(0); }
    int status; assert(waitpid(pid, &status, 0) == pid && WIFSIGNALED(status) && WTERMSIG(status) == SIGABRT);
    pid = fork();
    if (!pid) { freopen("/dev/null", "w", stderr); call("strlen", 1, m + 4096); _exit(0); }
    assert(waitpid(pid, &status, 0) == pid && WIFSIGNALED(status) && WTERMSIG(status) == SIGABRT);
    assert(!rt_munmap(m, 3 * 4096));
}
