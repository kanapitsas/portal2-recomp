// Caractères larges et conversions multioctets glibc i386.
//
// wchar_t Linux = int32 UTF-32 (wint_t non signé, WEOF = 0xffffffff).
// Encodage multioctet selon le LC_CTYPE effectif du thread (rt_locale.h) :
// "C" = ASCII strict (ANSI_X3.4-1968 : tout octet >= 0x80 et tout wchar_t
// >= 0x80 donnent EILSEQ), "C.UTF-8" = UTF-8 RFC 3629 (4 octets maximum,
// sans surrogates ni sur-longueurs). Aucune fonction hôte dépendant de la
// locale n'est appelée sur les données invitées.
//
// mbstate_t i386 (8 octets) : {int __count; wint_t __value;}. Seules ces
// fonctions l'interprètent : __count = octets attendus | longueur totale << 4,
// __value = bits déjà décodés.
//
// SHIMS: wcslen wcscpy wcsncpy wcscat wcsncat wcscmp wcsncmp wcschr wcsrchr wcsstr
// SHIMS: wcstoll __wcsncpy_chk __wcscat_chk __wcstombs_chk
// SHIMS: wcscoll __wcscoll_l __wcsxfrm_l wcstol wcstod wcstof
// SHIMS: wmemchr wmemcmp wmemcpy wmemmove wmemset
// SHIMS: towlower towupper iswalnum iswalpha iswblank iswcntrl iswdigit iswgraph iswlower
// SHIMS: iswprint iswpunct iswspace iswupper iswxdigit
// SHIMS: wcstombs mbstowcs wcrtomb mbrtowc mbsrtowcs mbsnrtowcs wcsnrtombs wctob btowc
#include "rt_locale.h"
#include <errno.h>
#include <stdlib.h>
#include <xlocale.h>

#define EILSEQ_LINUX 84
#define BAD ((uint32_t)-1)
#define INCOMPLETE ((uint32_t)-2)

static uint32_t arg(CPU *c, unsigned i) { return R32(c->esp + 4 + 4 * i); }
static int32_t W(uint32_t a) { return (int32_t)R32(a); }

// Longueur d'une chaîne large invitée (accès mémoire vérifiés).
static uint32_t wlen(CPU *c, uint32_t a) {
    if (!a) rt_trap(c, c->eip, "null guest wide string");
    for (uint64_t n = 0;; ++n) {
        if (a + 4 * n + 3 > UINT32_MAX) rt_trap(c, c->eip, "unterminated guest wide string");
        if (!R32(a + 4 * (uint32_t)n)) return (uint32_t)n;
    }
}
static void *wspan(CPU *c, uint32_t a, uint64_t count, int write) {
    return write ? rt_write_span(c, a, count * 4) : rt_span(c, a, count * 4);
}
static int sign(int32_t a, int32_t b) { return a < b ? -1 : a > b; }

typedef struct { uint32_t count, value; } State;
static void load_state(uint32_t ps, State *s) { s->count = R32(ps); s->value = R32(ps + 4); }
static void store_state(uint32_t ps, const State *s) { W32(ps, s->count); W32(ps + 4, s->value); }

// Décode au plus n octets invités à `a`. Renvoie 0 (NUL), le nombre d'octets
// consommés, INCOMPLETE (tous consommés, état mis à jour) ou BAD.
static uint32_t decode(int utf8, State *st, uint32_t a, uint32_t n, uint32_t *wc) {
    if (!n) return INCOMPLETE;
    if (!utf8) {
        uint8_t b = R8(a);
        if (b >= 0x80) return BAD;
        *wc = b; return b ? 1 : 0;
    }
    uint32_t need = st->count & 15, total = (st->count >> 4) & 15, v = st->value, i = 0;
    if (!need) {
        uint8_t b = R8(a); i = 1;
        if (b < 0x80) { *st = (State){0, 0}; *wc = b; return b ? 1 : 0; }
        if (b < 0xc2 || b > 0xf4) return BAD;
        total = b < 0xe0 ? 2 : b < 0xf0 ? 3 : 4;
        v = b & (total == 2 ? 0x1f : total == 3 ? 0x0f : 0x07);
        need = total - 1;
    }
    while (need && i < n) {
        uint8_t b = R8(a + i);
        if ((b & 0xc0) != 0x80) return BAD;
        if (total - need == 1) {   // deuxième octet : sur-longueur, surrogate, > U+10FFFF
            if ((total == 3 && v == 0 && b < 0xa0) || (total == 3 && v == 0xd && b >= 0xa0) ||
                (total == 4 && v == 0 && b < 0x90) || (total == 4 && v == 4 && b >= 0x90)) return BAD;
        }
        v = (v << 6) | (b & 0x3f); ++i; --need;
    }
    if (need) { st->count = need | total << 4; st->value = v; return INCOMPLETE; }
    *st = (State){0, 0}; *wc = v;
    return i;
}
// Encode wc dans out[4] ; renvoie la longueur ou BAD.
static uint32_t encode(int utf8, uint32_t wc, uint8_t out[4]) {
    if (wc < 0x80) { out[0] = (uint8_t)wc; return 1; }
    if (!utf8 || wc > 0x10ffff || (wc >= 0xd800 && wc <= 0xdfff)) return BAD;
    if (wc < 0x800) { out[0] = (uint8_t)(0xc0 | wc >> 6); out[1] = (uint8_t)(0x80 | (wc & 0x3f)); return 2; }
    if (wc < 0x10000) {
        out[0] = (uint8_t)(0xe0 | wc >> 12); out[1] = (uint8_t)(0x80 | ((wc >> 6) & 0x3f));
        out[2] = (uint8_t)(0x80 | (wc & 0x3f)); return 3;
    }
    out[0] = (uint8_t)(0xf0 | wc >> 18); out[1] = (uint8_t)(0x80 | ((wc >> 12) & 0x3f));
    out[2] = (uint8_t)(0x80 | ((wc >> 6) & 0x3f)); out[3] = (uint8_t)(0x80 | (wc & 0x3f)); return 4;
}
uint32_t rt_wc_encode(int utf8, uint32_t wc, uint8_t out[4]) { return encode(utf8, wc, out); }

// Multioctet -> large. `limit` = octets lisibles (au plus jusqu'au NUL inclus).
// *consumed : octets consommés ; *nul : NUL atteint. Renvoie le nombre de
// caractères (sans NUL) ou BAD (*consumed = début de la séquence invalide).
static uint32_t mb_to_wc(int utf8, State *st, uint32_t dst, uint32_t len, uint32_t src, uint32_t limit,
                         uint32_t *consumed, int *nul) {
    uint32_t count = 0, at = 0; *nul = 0;
    while (at < limit) {
        if (dst && count == len) break;
        uint32_t wc = 0, r = decode(utf8, st, src + at, limit - at, &wc);
        if (r == BAD) { *consumed = at; return BAD; }
        if (r == INCOMPLETE) { at = limit; break; }
        if (dst) W32(dst + 4 * count, wc);
        if (!r) { *nul = 1; ++at; break; }
        at += r; ++count;
    }
    *consumed = at;
    return count;
}
// Octets lisibles d'une chaîne : jusqu'au NUL inclus, au plus `maximum`.
static uint32_t readable(CPU *c, uint32_t s, uint32_t maximum) {
    if (!s) rt_trap(c, c->eip, "null guest multibyte string");
    uint32_t n = 0;
    while (n < maximum) { if (!R8(s + n++)) break; }
    return n;
}
// Large -> multioctet. `nwc` = caractères larges lisibles au plus.
static uint32_t wc_to_mb(int utf8, uint32_t dst, uint32_t len, uint32_t src, uint32_t nwc,
                         uint32_t *used, int *nul) {
    uint32_t written = 0, i = 0; *nul = 0;
    for (; i < nwc; ++i) {
        uint32_t wc = R32(src + 4 * i); uint8_t bytes[4];
        uint32_t k = encode(utf8, wc, bytes);
        if (k == BAD) { *used = i; return BAD; }
        if (dst && written + k > len) break;
        if (dst) for (uint32_t j = 0; j < k; ++j) W8(dst + written + j, bytes[j]);
        if (!wc) { *nul = 1; ++i; break; }
        written += k;
    }
    *used = i;
    return written;
}

// Extrait la partie ASCII initiale (après les espaces) pour les analyses numériques.
static char *narrow_number(CPU *c, uint32_t nptr, uint32_t *start) {
    int utf8 = rt_locale_ctype_utf8();
    uint32_t p = nptr;
    while (rt_wclass(R32(p), RT_WC_SPACE, utf8)) p += 4;
    uint32_t n = 0;
    while (1) { uint32_t wc = R32(p + 4 * n); if (!wc || wc >= 0x80) break; ++n; }
    char *text = malloc((size_t)n + 1);
    if (!text) rt_trap(c, c->eip, "cannot allocate number buffer");
    for (uint32_t i = 0; i < n; ++i) text[i] = (char)R32(p + 4 * i);
    text[n] = 0; *start = p;
    return text;
}
static locale_t host_c(void) {
    static locale_t c_locale;
    if (!c_locale) c_locale = newlocale(LC_ALL_MASK, "C", NULL);
    if (!c_locale) abort();
    return c_locale;
}

static int wide_class_shim(CPU *c, const char *name) {
    static const char *const names[] = {"iswalnum", "iswalpha", "iswblank", "iswcntrl", "iswdigit", "iswgraph",
                                        "iswlower", "iswprint", "iswpunct", "iswspace", "iswupper", "iswxdigit"};
    for (int i = 0; i < 12; ++i)
        if (!strcmp(name, names[i])) { c->eax = (uint32_t)rt_wclass(arg(c, 0), i + 1, rt_locale_ctype_utf8()); return 1; }
    return 0;
}

static int wchar_shim(CPU *c, const char *name) {
    static State mbrtowc_state, mbsrtowcs_state, mbsnrtowcs_state;
    uint32_t a = 0, b = 0, n = 0;
    if (!strncmp(name, "wcs", 3) || !strncmp(name, "wmem", 4) || !strncmp(name, "__wcs", 5)) { a = arg(c, 0); b = arg(c, 1); }
    if (!strcmp(name, "wcslen")) c->eax = wlen(c, a);
    else if (!strcmp(name, "wcscpy")) {
        n = wlen(c, b) + 1;
        memmove(wspan(c, a, n, 1), GPTR(b), (size_t)n * 4); c->eax = a;
    } else if (!strcmp(name, "wcsncpy")) {
        n = arg(c, 2); wspan(c, a, n, 1);
        uint32_t i = 0;
        for (; i < n; ++i) { uint32_t wc = R32(b + 4 * i); W32(a + 4 * i, wc); if (!wc) break; }
        for (; i < n; ++i) W32(a + 4 * i, 0);
        c->eax = a;
    } else if (!strcmp(name, "wcscat") || !strcmp(name, "wcsncat")) {
        uint32_t limit = name[3] == 'n' ? arg(c, 2) : UINT32_MAX, end = wlen(c, a), count = 0;
        while (count < limit && R32(b + 4 * count)) ++count;
        wspan(c, a + 4 * end, (uint64_t)count + 1, 1);
        memmove(GPTR(a + 4 * end), GPTR(b), (size_t)count * 4);
        W32(a + 4 * (end + count), 0); c->eax = a;
    } else if (!strcmp(name, "wcscmp") || !strcmp(name, "wcscoll") || !strcmp(name, "__wcscoll_l") || !strcmp(name, "wcsncmp")) {
        // wchar_t est signé sur i386 ; C/C.UTF-8 : collation = ordre des points de code.
        if (name[0] == '_') rt_locale_object_utf8(c, arg(c, 2));
        uint32_t limit = name[3] == 'n' ? arg(c, 2) : UINT32_MAX;
        int r = 0;
        for (uint32_t i = 0; i < limit && !r; ++i) {
            int32_t x = W(a + 4 * i), y = W(b + 4 * i);
            r = sign(x, y);
            if (!x) break;
        }
        c->eax = (uint32_t)r;
    } else if (!strcmp(name, "__wcsxfrm_l")) {
        n = arg(c, 2); rt_locale_object_utf8(c, arg(c, 3));
        uint32_t length = wlen(c, b);
        if (n) { uint32_t count = length + 1 < n ? length + 1 : n; memmove(wspan(c, a, count, 1), GPTR(b), (size_t)count * 4); }
        c->eax = length;
    } else if (!strcmp(name, "wcschr") || !strcmp(name, "wcsrchr")) {
        int32_t wanted = (int32_t)b; c->eax = 0;
        for (uint32_t p = a;; p += 4) {
            int32_t wc = W(p);
            if (wc == wanted) { c->eax = p; if (name[3] == 'c' || !wc) break; }
            if (!wc) break;
        }
    } else if (!strcmp(name, "wcsstr")) {
        uint32_t hl = wlen(c, a), nl = wlen(c, b); c->eax = 0;
        for (uint32_t i = 0; i + nl <= hl; ++i)
            if (!memcmp(GPTR(a + 4 * i), GPTR(b), (size_t)nl * 4)) { c->eax = a + 4 * i; break; }
    } else if (!strcmp(name, "wmemchr")) {
        n = arg(c, 2); wspan(c, a, n, 0); c->eax = 0;
        for (uint32_t i = 0; i < n; ++i) if (R32(a + 4 * i) == b) { c->eax = a + 4 * i; break; }
    } else if (!strcmp(name, "wmemcmp")) {
        n = arg(c, 2); wspan(c, a, n, 0); wspan(c, b, n, 0);
        int r = 0;
        for (uint32_t i = 0; i < n && !r; ++i) r = sign(W(a + 4 * i), W(b + 4 * i));
        c->eax = (uint32_t)r;
    } else if (!strcmp(name, "wmemcpy") || !strcmp(name, "wmemmove")) {
        n = arg(c, 2);
        void *d = wspan(c, a, n, 1), *s = wspan(c, b, n, 0);
        memmove(d, s, (size_t)n * 4); c->eax = a;
    } else if (!strcmp(name, "wmemset")) {
        n = arg(c, 2); wspan(c, a, n, 1);
        for (uint32_t i = 0; i < n; ++i) W32(a + 4 * i, b);
        c->eax = a;
    } else if (!strcmp(name, "towlower") || !strcmp(name, "towupper")) {
        c->eax = rt_wcase(arg(c, 0), name[3] == 'u', rt_locale_ctype_utf8());
    } else if (!strcmp(name, "wcstol")) {
        a = arg(c, 0); b = arg(c, 1); int base = (int32_t)arg(c, 2);
        if (base == 1 || base < 0 || base > 36) { rt_set_errno(c, 22); c->eax = 0; return 1; }
        uint32_t start; char *text = narrow_number(c, a, &start), *end;
        errno = 0;
        long long value = strtoll_l(text, &end, base, host_c());
        if (value > INT32_MAX) { value = INT32_MAX; errno = ERANGE; }
        if (value < INT32_MIN) { value = INT32_MIN; errno = ERANGE; }
        if (errno) rt_save_errno(c);
        if (b) W32(b, end == text ? a : start + 4 * (uint32_t)(end - text));
        free(text); c->eax = (uint32_t)(int32_t)value;
    } else if (!strcmp(name, "wcstoll")) {
        a = arg(c, 0); b = arg(c, 1); int base = (int32_t)arg(c, 2);
        if (base == 1 || base < 0 || base > 36) { rt_set_errno(c, 22); c->eax = c->edx = 0; return 1; }
        uint32_t start; char *text = narrow_number(c, a, &start), *end;
        errno = 0;
        long long value = strtoll_l(text, &end, base, host_c());
        if (errno) rt_save_errno(c);
        if (b) W32(b, end == text ? a : start + 4 * (uint32_t)(end - text));
        free(text);
        c->eax = (uint32_t)(uint64_t)value; c->edx = (uint32_t)((uint64_t)value >> 32);
    } else if (!strcmp(name, "__wcsncpy_chk") || !strcmp(name, "__wcscat_chk") || !strcmp(name, "__wcstombs_chk")) {
        // _FORTIFY_SOURCE : taille de destination en wchar_t (en octets pour wcstombs), puis
        // l'opération normale ; les arguments de tête ont la même place.
        uint32_t room = arg(c, !strcmp(name, "__wcscat_chk") ? 2 : 3), need;
        if (!strcmp(name, "__wcsncpy_chk")) need = arg(c, 2);
        else if (!strcmp(name, "__wcstombs_chk")) need = arg(c, 0) ? arg(c, 2) : 0;
        else { uint32_t end = wlen(c, arg(c, 0)), add = wlen(c, arg(c, 1)); need = end + add + 1; }
        if (room < need) rt_trap(c, c->eip, "*** buffer overflow detected ***: terminated (__chk_fail)");
        return rt_shim(c, !strcmp(name, "__wcsncpy_chk") ? "wcsncpy" : !strcmp(name, "__wcscat_chk") ? "wcscat" : "wcstombs");
    } else if (!strcmp(name, "wcstod") || !strcmp(name, "wcstof")) {
        a = arg(c, 0); b = arg(c, 1);
        uint32_t start; char *text = narrow_number(c, a, &start), *end;
        errno = 0;
        double value = name[5] == 'f' ? (double)strtof_l(text, &end, host_c()) : strtod_l(text, &end, host_c());
        if (errno) rt_save_errno(c);
        if (b) W32(b, end == text ? a : start + 4 * (uint32_t)(end - text));
        free(text); rt_return_st0(c, value);
    } else if (!strcmp(name, "wctob")) {
        uint32_t wc = arg(c, 0); c->eax = wc < 0x80 ? wc : UINT32_MAX;
    } else if (!strcmp(name, "btowc")) {
        int32_t ch = (int32_t)arg(c, 0); c->eax = ch >= 0 && ch < 0x80 ? (uint32_t)ch : UINT32_MAX;
    } else if (!strcmp(name, "mbrtowc")) {
        uint32_t pwc = arg(c, 0), s = arg(c, 1), ps = arg(c, 3); n = arg(c, 2);
        State st = mbrtowc_state; if (ps) load_state(ps, &st);
        uint32_t wc = 0, r;
        if (!s) { pwc = 0; st = (State){0, 0}; r = 0; }
        else r = decode(rt_locale_ctype_utf8(), &st, s, n, &wc);
        if (r == BAD) rt_set_errno(c, EILSEQ_LINUX);
        else if (r != INCOMPLETE && pwc) W32(pwc, wc);
        if (ps) store_state(ps, &st); else mbrtowc_state = st;
        c->eax = r;
    } else if (!strcmp(name, "wcrtomb")) {
        uint32_t s = arg(c, 0), wc = arg(c, 1), ps = arg(c, 2);
        if (ps) store_state(ps, &(State){0, 0});
        uint8_t bytes[4];
        uint32_t k = encode(rt_locale_ctype_utf8(), s ? wc : 0, bytes);
        if (k == BAD) rt_set_errno(c, EILSEQ_LINUX);
        else if (s) memcpy(rt_write_span(c, s, k), bytes, k);
        c->eax = k;
    } else if (!strcmp(name, "mbstowcs")) {
        uint32_t dst = arg(c, 0), src = arg(c, 1); n = arg(c, 2);
        State st = {0, 0}; uint32_t used; int nul;
        c->eax = mb_to_wc(rt_locale_ctype_utf8(), &st, dst, n, src, readable(c, src, UINT32_MAX), &used, &nul);
        if (c->eax == BAD) rt_set_errno(c, EILSEQ_LINUX);
    } else if (!strcmp(name, "mbsrtowcs") || !strcmp(name, "mbsnrtowcs")) {
        int bounded = name[3] == 'n';
        uint32_t dst = arg(c, 0), srcp = arg(c, 1), nms = bounded ? arg(c, 2) : UINT32_MAX;
        uint32_t len = arg(c, bounded ? 3 : 2), ps = arg(c, bounded ? 4 : 3);
        State *internal = bounded ? &mbsnrtowcs_state : &mbsrtowcs_state, st = *internal;
        if (ps) load_state(ps, &st);
        uint32_t src = R32(srcp), used; int nul;
        uint32_t r = mb_to_wc(rt_locale_ctype_utf8(), &st, dst, len, src, readable(c, src, nms), &used, &nul);
        if (r == BAD) rt_set_errno(c, EILSEQ_LINUX);
        if (dst) W32(srcp, nul ? 0 : src + used);
        if (ps) store_state(ps, &st); else *internal = st;
        c->eax = r;
    } else if (!strcmp(name, "wcstombs") || !strcmp(name, "wcsnrtombs")) {
        int bounded = name[3] == 'n';
        uint32_t dst = arg(c, 0), srcp = bounded ? arg(c, 1) : 0, src = bounded ? R32(srcp) : arg(c, 1);
        uint32_t nwc = bounded ? arg(c, 2) : UINT32_MAX, len = arg(c, bounded ? 3 : 2), used; int nul;
        if (bounded && arg(c, 4)) store_state(arg(c, 4), &(State){0, 0});
        if (!src) rt_trap(c, c->eip, "null guest wide string");
        uint32_t r = wc_to_mb(rt_locale_ctype_utf8(), dst, len, src, nwc, &used, &nul);
        if (r == BAD) rt_set_errno(c, EILSEQ_LINUX);
        if (bounded && dst) W32(srcp, nul ? 0 : src + 4 * used);
        c->eax = r;
    } else return wide_class_shim(c, name);
    return 1;
}
RT_SHIM_PROVIDER(wchar_shim)
