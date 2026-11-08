// Variantes _FORTIFY_SOURCE de la glibc : même contrôle de taille de
// destination que glibc, puis l'opération normale. Un dépassement appelle
// __chk_fail, qui sous Linux affiche « buffer overflow detected » et avorte :
// ici, rt_trap avec le même message. Les variantes printf (__sprintf_chk...)
// vivent dans format.c, à côté du moteur de formatage.
// SHIMS: __memcpy_chk __memmove_chk __memset_chk __strcpy_chk __strncpy_chk
// SHIMS: __strcat_chk __strncat_chk __stack_chk_fail __fdelt_chk __stpcpy_chk __fread_chk
#include "rt.h"

static uint32_t argument(CPU *c, unsigned i) { return R32(c->esp + 4 + 4 * i); }
__attribute__((noreturn)) static void chk_fail(CPU *c) {
    rt_trap(c, c->eip, "*** buffer overflow detected ***: terminated (__chk_fail)");
}
#define bounded_length rt_bounded_strnlen

int rt_fortify_shim(CPU *c, const char *name) {
    if (!strcmp(name, "__stack_chk_fail"))
        rt_trap(c, c->eip, "*** stack smashing detected ***: terminated (__stack_chk_fail)");
    if (!strcmp(name, "__stpcpy_chk")) {   // (dest, src, destlen)
        if ((uint64_t)rt_string_length(c, argument(c, 1)) + 1 > argument(c, 2)) chk_fail(c);
        return rt_shim(c, "stpcpy");
    }
    if (!strcmp(name, "__fread_chk")) {    // (ptr, ptrlen, size, n, fp)
        uint32_t size = argument(c, 2), n = argument(c, 3), fp = argument(c, 4);
        if ((uint64_t)size * n > argument(c, 1)) chk_fail(c);
        // Les emplacements d'arguments appartiennent à l'appelé (cdecl) : on les réécrit
        // dans l'ordre de fread(ptr, size, n, fp).
        W32(c->esp + 8, size); W32(c->esp + 12, n); W32(c->esp + 16, fp);
        return rt_shim(c, "fread");
    }
    if (!strcmp(name, "__fdelt_chk")) {   // FD_SET/FD_ISSET fortifiés : 0 <= d < FD_SETSIZE (1024)
        int32_t d = (int32_t)argument(c, 0);
        if (d < 0 || d >= 1024) chk_fail(c);
        c->eax = (uint32_t)d / 32u;        // NFDBITS = 32 sur i386
        return 1;
    }
    if (!strcmp(name, "__memcpy_chk") || !strcmp(name, "__memmove_chk")) {
        uint32_t dst = argument(c, 0), src = argument(c, 1), n = argument(c, 2), dstlen = argument(c, 3);
        if (dstlen < n) chk_fail(c);
        void *to = rt_write_span(c, dst, n), *from = rt_span(c, src, n);
        memmove(to, from, n);  // même extension déterministe que le shim memcpy
        c->eax = dst;
    } else if (!strcmp(name, "__memset_chk")) {
        uint32_t dst = argument(c, 0), value = argument(c, 1), n = argument(c, 2), dstlen = argument(c, 3);
        if (dstlen < n) chk_fail(c);
        memset(rt_write_span(c, dst, n), (int)value, n);
        c->eax = dst;
    } else if (!strcmp(name, "__strcpy_chk")) {
        uint32_t dst = argument(c, 0), src = argument(c, 1), dstlen = argument(c, 2);
        uint32_t n = rt_string_length(c, src);
        if (n >= dstlen) chk_fail(c);
        memmove(rt_write_span(c, dst, (uint64_t)n + 1), GPTR(src), (size_t)n + 1);
        c->eax = dst;
    } else if (!strcmp(name, "__strncpy_chk")) {
        uint32_t dst = argument(c, 0), src = argument(c, 1), n = argument(c, 2), dstlen = argument(c, 3);
        if (dstlen < n) chk_fail(c);
        uint32_t length = bounded_length(c, src, n);
        uint8_t *to = rt_write_span(c, dst, n);
        memmove(to, GPTR(src), length);
        memset(to + length, 0, n - length);
        c->eax = dst;
    } else if (!strcmp(name, "__strcat_chk") || !strcmp(name, "__strncat_chk")) {
        int bounded = !strcmp(name, "__strncat_chk");
        uint32_t dst = argument(c, 0), src = argument(c, 1);
        uint32_t limit = bounded ? argument(c, 2) : UINT32_MAX, dstlen = argument(c, bounded ? 3 : 2);
        // glibc parcourt dest puis copie en décrémentant dstlen : échec dès que
        // strlen(dest) + copié + NUL dépasse dstlen.
        uint32_t existing = bounded_length(c, dst, dstlen);
        if (existing == dstlen) chk_fail(c);
        uint32_t count = bounded_length(c, src, limit);
        if ((uint64_t)existing + count + 1 > dstlen) chk_fail(c);
        uint8_t *to = rt_write_span(c, dst + existing, (uint64_t)count + 1);
        memmove(to, GPTR(src), count);
        to[count] = 0;
        c->eax = dst;
    } else return 0;
    return 1;
}
RT_SHIM_PROVIDER(rt_fortify_shim)
