// libm, conversions, qsort/rand, fortify et printf (narrow/large) : appels
// simulés avec une pile invitée cdecl i386 dans 0x20000000-0x20020000.
#include "checks.h"
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

#define STACK 0x20010000u
#define DATA 0x20002000u   // zone de données des tests
#define OUT 0x20004000u    // tampon de sortie

typedef struct { CPU cpu; uint32_t at; } Call;
static Call *begin(Call *k) { rt_cpu_reset(&k->cpu); k->cpu.esp = STACK; k->at = STACK + 4; W32(STACK, 0xdeadbeef); return k; }
static void p32(Call *k, uint32_t v) { W32(k->at, v); k->at += 4; }
static void pf(Call *k, float v) { WF(k->at, v); k->at += 4; }
static void pd(Call *k, double v) { WD(k->at, v); k->at += 8; }
static void invoke(Call *k, const char *name) { assert(rt_shim(&k->cpu, name) == 1); }
static uint32_t put(uint32_t address, const char *s) { memcpy(GPTR(address), s, strlen(s) + 1); return address; }
static uint32_t put_wide(uint32_t address, const char *s) {
    size_t n = strlen(s);
    for (size_t i = 0; i <= n; ++i) W32(address + 4 * (uint32_t)i, (unsigned char)s[i]);
    return address;
}
static int wide_equals(uint32_t address, const char *s) {
    size_t n = strlen(s);
    for (size_t i = 0; i <= n; ++i) if (R32(address + 4 * (uint32_t)i) != (unsigned char)s[i]) return 0;
    return 1;
}
static uint32_t errno_address(void) { Call k; begin(&k); invoke(&k, "__errno_location"); return k.cpu.eax; }
static void clear_errno(void) { W32(errno_address(), 0); }
static uint32_t guest_errno(void) { return R32(errno_address()); }
static double st0(Call *k) { CPU *c = &k->cpu; return ST(0); }
static unsigned depth(Call *k) { return (8 - k->cpu.ftop) & 7; }  // éléments empilés depuis reset

// L'appel doit avorter comme __chk_fail : exécuté dans un processus fils.
static void expect_abort(void (*fn)(void)) {
    fflush(NULL);
    pid_t pid = fork();
    assert(pid >= 0);
    if (!pid) { freopen("/dev/null", "w", stderr); fn(); _exit(0); }
    int status = 0;
    assert(waitpid(pid, &status, 0) == pid);
    assert(WIFSIGNALED(status) && WTERMSIG(status) == SIGABRT);
}

// ---- rand / srand ----
RT_CHECK(rand_matches_glibc_type3) {
    static const uint32_t expected[] = {1804289383, 846930886, 1681692777, 1714636915, 1957747793,
                                        424238335, 719885386, 1649760492, 596516649, 1189641421};
    Call k;
    for (int seed = 0; seed < 2; ++seed) {  // srand(0) équivaut à srand(1)
        begin(&k); p32(&k, (uint32_t)seed); invoke(&k, "srand");
        for (size_t i = 0; i < sizeof(expected) / sizeof(*expected); ++i) {
            begin(&k); invoke(&k, "rand"); assert(k.cpu.eax == expected[i]);
        }
    }
    begin(&k); p32(&k, 2); invoke(&k, "srand");
    begin(&k); invoke(&k, "rand"); assert(k.cpu.eax != expected[0] && k.cpu.eax <= 0x7fffffff);
}

// ---- qsort : comparateur invité (import dynamique) ----
static uint32_t sort_base, sort_count;
static int sort_calls, sort_outside;
static int sort_compare(CPU *c, const char *name) {
    if (strcmp(name, "test_qsort_compare")) return 0;
    uint32_t a = R32(c->esp + 4), b = R32(c->esp + 8);
    if (a < sort_base || b < sort_base || a >= sort_base + sort_count * 8 || b >= sort_base + sort_count * 8) ++sort_outside;
    ++sort_calls;
    int32_t ka = (int32_t)R32(a), kb = (int32_t)R32(b);
    c->eax = (uint32_t)(ka < kb ? -1 : ka > kb);
    return 1;
}
RT_SHIM_PROVIDER(sort_compare)
RT_CHECK(qsort_is_glibc_merge_sort) {
    // Éléments {clé, identifiant} : tri stable attendu (msort de glibc).
    static const int32_t keys[] = {5, 1, 3, 1, 5, 0, 3, 3, 2};
    sort_base = DATA; sort_count = sizeof(keys) / sizeof(*keys);
    for (uint32_t i = 0; i < sort_count; ++i) { W32(DATA + 8 * i, (uint32_t)keys[i]); W32(DATA + 8 * i + 4, i); }
    Call k; begin(&k);
    p32(&k, DATA); p32(&k, sort_count); p32(&k, 8); p32(&k, rt_dynamic_import("test_qsort_compare"));
    sort_calls = sort_outside = 0;
    invoke(&k, "qsort");
    static const uint32_t ids[] = {5, 1, 3, 8, 2, 6, 7, 0, 4};
    for (uint32_t i = 0; i < sort_count; ++i) {
        assert(R32(DATA + 8 * i + 4) == ids[i]);
        if (i) assert((int32_t)R32(DATA + 8 * i) >= (int32_t)R32(DATA + 8 * (i - 1)));
    }
    assert(!sort_outside && k.cpu.esp == STACK);
    // Nombre de comparaisons du msort descendant de glibc pour cette entrée
    // (obtenu en simulant msort_with_tmp en Python sur cette entrée).
    assert(sort_calls == 21);
    // n <= 1 : aucun appel au comparateur.
    sort_calls = 0; begin(&k);
    p32(&k, DATA); p32(&k, 1); p32(&k, 8); p32(&k, rt_dynamic_import("test_qsort_compare"));
    invoke(&k, "qsort"); assert(!sort_calls);
}

// ---- conversions ----
RT_CHECK(strto_integers_follow_i386_glibc) {
    Call k; uint32_t end = OUT;
    clear_errno(); begin(&k);
    p32(&k, put(DATA, "  -2147483649z")); p32(&k, end); p32(&k, 10); p32(&k, 0); invoke(&k, "__strtol_internal");
    assert(k.cpu.eax == 0x80000000u && guest_errno() == ERANGE && R32(end) == DATA + 13);
    clear_errno(); begin(&k);
    p32(&k, put(DATA, "-1")); p32(&k, end); p32(&k, 0); p32(&k, 0); invoke(&k, "__strtoul_internal");
    assert(k.cpu.eax == UINT32_MAX && guest_errno() == 0);
    begin(&k); p32(&k, put(DATA, "-4294967295")); p32(&k, 0); p32(&k, 10); p32(&k, 0); invoke(&k, "__strtoul_internal");
    assert(k.cpu.eax == 1 && guest_errno() == 0);
    begin(&k); p32(&k, put(DATA, "-4294967296")); p32(&k, 0); p32(&k, 10); p32(&k, 0); invoke(&k, "__strtoul_internal");
    assert(k.cpu.eax == UINT32_MAX && guest_errno() == ERANGE);
    clear_errno(); begin(&k);
    p32(&k, put(DATA, "18446744073709551615")); p32(&k, 0); p32(&k, 10); invoke(&k, "strtoull");
    assert(k.cpu.eax == UINT32_MAX && k.cpu.edx == UINT32_MAX && guest_errno() == 0);
    begin(&k); p32(&k, put(DATA, "-9223372036854775809")); p32(&k, 0); p32(&k, 10); invoke(&k, "strtoll");
    assert(k.cpu.eax == 0 && k.cpu.edx == 0x80000000u && guest_errno() == ERANGE);
    clear_errno(); begin(&k);
    p32(&k, put(DATA, "0x1fz")); p32(&k, end); p32(&k, 0); p32(&k, 0); invoke(&k, "__strtoll_internal");
    assert(k.cpu.eax == 0x1f && k.cpu.edx == 0 && R32(end) == DATA + 4);
    begin(&k); p32(&k, put(DATA, "0xg")); p32(&k, end); p32(&k, 16); p32(&k, 0); invoke(&k, "__strtoull_internal");
    assert(k.cpu.eax == 0 && R32(end) == DATA + 1);  // « 0 » consommé, pas « x »
    W32(end, 0x1234); begin(&k);
    p32(&k, put(DATA, "12")); p32(&k, end); p32(&k, 1); p32(&k, 0); invoke(&k, "__strtol_internal");
    assert(k.cpu.eax == 0 && guest_errno() == EINVAL && R32(end) == 0x1234);  // base invalide
}

RT_CHECK(strto_floats_return_in_st0) {
    Call k; uint32_t end = OUT;
    clear_errno(); begin(&k);
    p32(&k, put(DATA, "2.5x")); p32(&k, end); p32(&k, 0x12345678); invoke(&k, "__strtod_l");
    assert(depth(&k) == 1 && st0(&k) == 2.5 && R32(end) == DATA + 3 && guest_errno() == 0);
    begin(&k); p32(&k, put(DATA, "0.1")); p32(&k, end); p32(&k, 0); invoke(&k, "__strtof_internal");
    assert(st0(&k) == (double)0.1f);
    begin(&k); p32(&k, put(DATA, "1e40")); p32(&k, 0); p32(&k, 0); invoke(&k, "__strtof_l");
    assert(isinf(st0(&k)) && guest_errno() == ERANGE);
    clear_errno(); begin(&k);
    p32(&k, put(DATA, "-0x1.8p1")); p32(&k, 0); p32(&k, 0); invoke(&k, "__strtod_internal");
    assert(st0(&k) == -3.0 && guest_errno() == 0);
    begin(&k); p32(&k, put(DATA, "0.1")); p32(&k, end); p32(&k, 0); invoke(&k, "strtold_l");
    assert(st0(&k) == 0.1 && R32(end) == DATA + 3);
}
static void strtold_out_of_double_range(void) {
    Call k; begin(&k); p32(&k, put(DATA, "1e400")); p32(&k, 0); p32(&k, 0); invoke(&k, "strtold_l");
}
RT_CHECK(strtold_refuses_values_beyond_double) { expect_abort(strtold_out_of_double_range); }

// ---- libm ----
RT_CHECK(libm_i386_abi_and_errno) {
    Call k;
    begin(&k); pf(&k, 0.5f); invoke(&k, "sinf");
    assert(depth(&k) == 1 && st0(&k) == (double)sinf(0.5f));
    begin(&k); pd(&k, 1.0); pd(&k, -1.0); invoke(&k, "atan2");   // atan2(y, x)
    assert(st0(&k) == atan2(1.0, -1.0));
    begin(&k); pf(&k, 1.0f); pf(&k, -1.0f); invoke(&k, "atan2f");
    assert(st0(&k) == (double)atan2f(1.0f, -1.0f));
    begin(&k); pd(&k, 7.5); pd(&k, 2.0); invoke(&k, "fmod"); assert(st0(&k) == 1.5);
    begin(&k); pf(&k, 2.0f); pf(&k, 10.0f); invoke(&k, "powf"); assert(st0(&k) == 1024.0);
    begin(&k); pd(&k, 3.0); p32(&k, (uint32_t)-2); invoke(&k, "ldexp"); assert(st0(&k) == 0.75);
    begin(&k); pd(&k, 48.0); p32(&k, OUT); invoke(&k, "frexp");
    assert(st0(&k) == 0.75 && R32(OUT) == 6);
    begin(&k); pd(&k, -3.25); p32(&k, OUT); invoke(&k, "modf");
    assert(st0(&k) == -0.25 && RD(OUT) == -3.0);
    begin(&k); pd(&k, 0.5); p32(&k, OUT); p32(&k, OUT + 8); invoke(&k, "sincos");
    assert(depth(&k) == 0 && RD(OUT) == sin(0.5) && RD(OUT + 8) == cos(0.5));
    begin(&k); pf(&k, 0.5f); p32(&k, OUT); p32(&k, OUT + 4); invoke(&k, "sincosf");
    assert(depth(&k) == 0 && RF(OUT) == sinf(0.5f) && RF(OUT + 4) == cosf(0.5f));
    begin(&k); pd(&k, -2.7); invoke(&k, "trunc"); assert(st0(&k) == -2.0);
    begin(&k); pf(&k, 27.0f); invoke(&k, "cbrtf"); assert(st0(&k) == 3.0);
    // errno façon glibc
    clear_errno(); begin(&k); pd(&k, 0.0); invoke(&k, "log"); assert(isinf(st0(&k)) && guest_errno() == ERANGE);
    clear_errno(); begin(&k); pf(&k, -1.0f); invoke(&k, "sqrtf"); assert(isnan(st0(&k)) && guest_errno() == EDOM);
    clear_errno(); begin(&k); pd(&k, -1000.0); invoke(&k, "exp"); assert(st0(&k) == 0 && guest_errno() == ERANGE);
    clear_errno(); begin(&k); pd(&k, 1.0); pd(&k, 0.0); invoke(&k, "fmod"); assert(isnan(st0(&k)) && guest_errno() == EDOM);
    clear_errno(); begin(&k); pd(&k, 10.0); pd(&k, 400.0); invoke(&k, "__pow_finite"); assert(isinf(st0(&k)) && guest_errno() == 0);
    begin(&k); pd(&k, 10.0); pd(&k, 400.0); invoke(&k, "pow"); assert(guest_errno() == ERANGE);
    clear_errno(); begin(&k); pd(&k, NAN); invoke(&k, "sin"); assert(isnan(st0(&k)) && guest_errno() == 0);
    begin(&k); pd(&k, -INFINITY); invoke(&k, "exp"); assert(st0(&k) == 0 && guest_errno() == 0);
}

// ---- fortify ----
RT_CHECK(fortify_copies_within_bounds) {
    Call k;
    put(DATA, "abcdef");
    begin(&k); p32(&k, OUT); p32(&k, DATA); p32(&k, 7); p32(&k, 7); invoke(&k, "__memcpy_chk");
    assert(k.cpu.eax == OUT && !strcmp((char *)GPTR(OUT), "abcdef"));
    begin(&k); p32(&k, OUT); p32(&k, 'z'); p32(&k, 3); p32(&k, 3); invoke(&k, "__memset_chk");
    assert(!memcmp(GPTR(OUT), "zzzdef", 7));
    begin(&k); p32(&k, OUT); p32(&k, DATA); p32(&k, 7); invoke(&k, "__strcpy_chk");
    assert(!strcmp((char *)GPTR(OUT), "abcdef"));
    begin(&k); p32(&k, OUT); p32(&k, put(DATA, "xy")); p32(&k, 9); invoke(&k, "__strcat_chk");
    assert(!strcmp((char *)GPTR(OUT), "abcdefxy"));
    begin(&k); p32(&k, OUT); p32(&k, put(DATA, "123")); p32(&k, 1); p32(&k, 10); invoke(&k, "__strncat_chk");
    assert(!strcmp((char *)GPTR(OUT), "abcdefxy1"));
    memset(GPTR(OUT), 'q', 8);
    begin(&k); p32(&k, OUT); p32(&k, put(DATA, "ab")); p32(&k, 5); p32(&k, 5); invoke(&k, "__strncpy_chk");
    assert(!memcmp(GPTR(OUT), "ab\0\0\0qqq", 8));
    begin(&k); p32(&k, OUT); p32(&k, DATA); p32(&k, 2); p32(&k, 2); invoke(&k, "__memmove_chk");
    assert(!memcmp(GPTR(OUT), "ab", 2));
}
static void memcpy_overflow(void) { Call k; begin(&k); p32(&k, OUT); p32(&k, DATA); p32(&k, 8); p32(&k, 7); invoke(&k, "__memcpy_chk"); }
static void strcpy_overflow(void) { Call k; begin(&k); p32(&k, OUT); p32(&k, put(DATA, "abc")); p32(&k, 3); invoke(&k, "__strcpy_chk"); }
static void strcat_overflow(void) {
    put(OUT, "ab"); Call k; begin(&k); p32(&k, OUT); p32(&k, put(DATA, "cd")); p32(&k, 4); invoke(&k, "__strcat_chk");
}
static void stack_smash(void) { Call k; begin(&k); invoke(&k, "__stack_chk_fail"); }
static void sprintf_overflow(void) {
    Call k; begin(&k); p32(&k, OUT); p32(&k, 1); p32(&k, 4); p32(&k, put(DATA, "%d")); p32(&k, 1234); invoke(&k, "__sprintf_chk");
}
static void snprintf_bad_size(void) {
    Call k; begin(&k); p32(&k, OUT); p32(&k, 16); p32(&k, 1); p32(&k, 8); p32(&k, put(DATA, "x")); invoke(&k, "__snprintf_chk");
}
RT_CHECK(fortify_overflows_abort) {
    expect_abort(memcpy_overflow); expect_abort(strcpy_overflow); expect_abort(strcat_overflow);
    expect_abort(stack_smash); expect_abort(sprintf_overflow); expect_abort(snprintf_bad_size);
}

// ---- printf ----
RT_CHECK(sprintf_family_glibc_output) {
    Call k;
    begin(&k);
    p32(&k, OUT); p32(&k, put(DATA, "%d-%s-%p-%p-%5.1f-%s|%-7p|")); p32(&k, (uint32_t)-42); p32(&k, put(DATA + 0x100, "ab"));
    p32(&k, 0); p32(&k, 0x1234); pd(&k, 2.25); p32(&k, 0); p32(&k, 0);
    invoke(&k, "sprintf");
    const char *expected = "-42-ab-(nil)-0x1234-  2.2-(null)|(nil)  |";
    assert(!strcmp((char *)GPTR(OUT), expected) && k.cpu.eax == strlen(expected));
    // vsprintf : va_list = pointeur vers les arguments i386.
    W32(DATA + 0x200, 7); W32(DATA + 0x204, 0xffffffff); W32(DATA + 0x208, 0);  // %lld = -1 (lo, hi)
    W32(DATA + 0x208, 0xffffffff);
    memset(GPTR(OUT), 'q', 32);
    begin(&k); p32(&k, OUT); p32(&k, put(DATA, "%u/%lld")); p32(&k, DATA + 0x200); invoke(&k, "vsprintf");
    assert(!strcmp((char *)GPTR(OUT), "7/-1") && k.cpu.eax == 4 && R8(OUT + 5) == 'q');  // n'écrit que len+1
    begin(&k); p32(&k, OUT); p32(&k, 1); p32(&k, 8); p32(&k, put(DATA, "%x!")); p32(&k, 0xbeef); invoke(&k, "__sprintf_chk");
    assert(!strcmp((char *)GPTR(OUT), "beef!") && k.cpu.eax == 5);
    begin(&k); p32(&k, OUT); p32(&k, 4); p32(&k, 1); p32(&k, 8); p32(&k, put(DATA, "%s")); p32(&k, put(DATA + 0x100, "hello"));
    invoke(&k, "__snprintf_chk");
    assert(!strcmp((char *)GPTR(OUT), "hel") && k.cpu.eax == 5);
    W32(DATA + 0x200, 99);
    begin(&k); p32(&k, OUT); p32(&k, 0); p32(&k, 0xffffffff); p32(&k, put(DATA, "<%03d>")); p32(&k, DATA + 0x200); invoke(&k, "__vsprintf_chk");
    assert(!strcmp((char *)GPTR(OUT), "<099>"));
}

// %ls et %lc dans printf étroit (texte localisé de Source, plantage en jeu sur
// iPhone) : locale C de glibc, ASCII seulement, précision en octets.
RT_CHECK(sprintf_wide_arguments_c_locale) {
    Call k;
    put_wide(DATA + 0x100, "portal");
    begin(&k);
    p32(&k, OUT); p32(&k, put(DATA, "[%ls|%-8.3ls|%S|%lc%C]")); p32(&k, DATA + 0x100); p32(&k, DATA + 0x100); p32(&k, 0);
    p32(&k, 'x'); p32(&k, 'y');
    invoke(&k, "sprintf");
    const char *expected = "[portal|por     |(null)|xy]";
    assert(!strcmp((char *)GPTR(OUT), expected) && k.cpu.eax == strlen(expected));
    W32(DATA + 0x200, 0xE9); W32(DATA + 0x204, 0);   // « é » : non convertible dans la locale C
    begin(&k); p32(&k, OUT); p32(&k, put(DATA, "%ls")); p32(&k, DATA + 0x200); invoke(&k, "sprintf");
    assert(k.cpu.eax == UINT32_MAX);
}

RT_CHECK(swprintf_utf32_and_truncation) {
    Call k;
    put_wide(DATA + 0x100, "wide");
    begin(&k);
    p32(&k, OUT); p32(&k, 64); p32(&k, put_wide(DATA, "%ls|%s|%5d|%c|%-3.2S|%lc%n")); p32(&k, DATA + 0x100); p32(&k, put(DATA + 0x200, "nar"));
    p32(&k, 42); p32(&k, 'x'); p32(&k, DATA + 0x100); p32(&k, 0x1F600); p32(&k, DATA + 0x300);
    invoke(&k, "swprintf");
    // "wide|nar|   42|x|wi |" + U+1F600
    assert(k.cpu.eax == 22 && R32(DATA + 0x300) == 22);
    for (int i = 0; i < 21; ++i) assert(R32(OUT + 4 * i) == (unsigned char)"wide|nar|   42|x|wi |"[i]);
    assert(R32(OUT + 4 * 21) == 0x1F600 && R32(OUT + 4 * 22) == 0);
    // Caractère non ASCII littéral dans le format : recopié tel quel.
    W32(DATA, 0xE9); W32(DATA + 4, '%'); W32(DATA + 8, 'd'); W32(DATA + 12, 0);
    begin(&k); p32(&k, OUT); p32(&k, 8); p32(&k, DATA); p32(&k, 5); invoke(&k, "swprintf");
    assert(k.cpu.eax == 2 && R32(OUT) == 0xE9 && R32(OUT + 4) == '5' && R32(OUT + 8) == 0);
    // Dépassement : -1, maxlen-1 caractères écrits, pas de terminateur.
    for (int i = 0; i < 8; ++i) W32(OUT + 4 * i, 0x55);
    W32(DATA + 0x200, (uint32_t)put(DATA + 0x300, "hello"));
    begin(&k); p32(&k, OUT); p32(&k, 4); p32(&k, put_wide(DATA, "%s")); p32(&k, DATA + 0x200); invoke(&k, "vswprintf");
    assert(k.cpu.eax == UINT32_MAX);
    assert(R32(OUT) == 'h' && R32(OUT + 4) == 'e' && R32(OUT + 8) == 'l' && R32(OUT + 12) == 0x55);
    // Exactement maxlen-1 : tient, terminé.
    begin(&k); p32(&k, OUT); p32(&k, 6); p32(&k, put_wide(DATA, "%s")); p32(&k, DATA + 0x200); invoke(&k, "vswprintf");
    assert(k.cpu.eax == 5 && wide_equals(OUT, "hello"));
    begin(&k); p32(&k, OUT); p32(&k, 0); p32(&k, put_wide(DATA, "x")); invoke(&k, "swprintf");
    assert(k.cpu.eax == UINT32_MAX);
    begin(&k); p32(&k, OUT); p32(&k, 16); p32(&k, put_wide(DATA, "%p %%|%.3s|")); p32(&k, 0); p32(&k, 0); invoke(&k, "swprintf");
    assert(wide_equals(OUT, "(nil) %||") && k.cpu.eax == 9);
}

// DIV/IDIV 8 et 16 bits ; DX:AX = 0x80000000 / -1 doit piéger (#DE), sans UB.
static int narrow_div_traps(uint32_t eax, uint32_t edx, uint32_t divisor, int width, int is_signed) {
    fflush(NULL);
    pid_t pid = fork();
    assert(pid >= 0);
    if (!pid) {
        int devnull = open("/dev/null", 1);
        if (devnull >= 0) dup2(devnull, 2);
        CPU c; rt_cpu_reset(&c); c.eax = eax; c.edx = edx;
        rt_div_narrow(&c, divisor, width, is_signed, 0);
        _exit(0);
    }
    int status = 0;
    assert(waitpid(pid, &status, 0) == pid);
    return WIFSIGNALED(status);
}

RT_CHECK(numeric_narrow_division) {
    CPU c; rt_cpu_reset(&c);
    c.eax = 0xabcd0064u; rt_div_narrow(&c, 7, 8, 0, 0);                     // 100 / 7
    assert(c.eax == 0xabcd020eu);
    c.eax = 0xabcdff9cu; rt_div_narrow(&c, 7, 8, 1, 0);                     // -100 / 7
    assert(c.eax == 0xabcdfef2u);
    c.eax = 0x1111e240u; c.edx = 0x22220001u; rt_div_narrow(&c, 1000, 16, 0, 0);   // 123456 / 1000
    assert(c.eax == 0x1111007bu && c.edx == 0x222201c8u);
    c.eax = 0x11111dc0u; c.edx = 0x2222fffeu; rt_div_narrow(&c, 1000, 16, 1, 0);   // -123456 / 1000
    assert(c.eax == 0x1111ff85u && c.edx == 0x2222fe38u);
    assert(narrow_div_traps(0, 0x8000, 0xffff, 16, 1));                     // INT32_MIN / -1
    assert(narrow_div_traps(0x8000, 0xffff, 0xffff, 16, 1));                // -32768 / -1
    assert(narrow_div_traps(0xff80, 0, 0xff, 8, 1));                        // -128 / -1
    assert(narrow_div_traps(0, 1, 1, 16, 0));                               // quotient > 0xffff
    assert(!narrow_div_traps(0x0004, 0, 0xfffe, 16, 1));                    // 4 / -2 : correct
}
