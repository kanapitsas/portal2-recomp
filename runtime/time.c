// Temps glibc i386 : time_t = int32, struct tm = 11 int + long tm_gmtoff +
// const char *tm_zone (44 octets, tm_zone = chaîne invitée stable).
//
// Les conversions de fuseau passent par la libc hôte (même base IANA, même
// variable TZ) sur des time_t élargis à 64 bits ; tout résultat hors de
// l'intervalle int32 donne EOVERFLOW comme glibc 32 bits. strftime/strptime
// sont réimplémentés pour la locale C de glibc (la seule disponible, voir
// ctype_locale.c), y compris les variantes larges et _l.
//
// gmtime et localtime partagent un tampon statique invité (glibc _tmbuf) ;
// ctime et asctime partagent le tampon statique d'asctime.
//
// SHIMS: time clock difftime mktime timegm gmtime gmtime_r localtime localtime_r
// SHIMS: ctime ctime_r asctime asctime_r strftime __strftime_l __wcsftime_l strptime tzset utime
// SHIMS: nanosleep sleep times
#include "rt_locale.h"
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <utime.h>


typedef struct { int32_t f[9]; int32_t gmtoff; uint32_t zone; } GTm;   // sec,min,hour,mday,mon,year,wday,yday,isdst
enum { SEC, MIN, HOUR, MDAY, MON, YEAR, WDAY, YDAY, ISDST };
#define TM_SIZE 44u
static void read_tm(CPU *c, uint32_t a, GTm *t) {
    rt_span(c, a, TM_SIZE);
    for (int i = 0; i < 9; ++i) t->f[i] = (int32_t)R32(a + 4 * (uint32_t)i);
    t->gmtoff = (int32_t)R32(a + 36); t->zone = R32(a + 40);
}
static void write_tm(CPU *c, uint32_t a, const GTm *t) {
    rt_write_span(c, a, TM_SIZE);
    for (int i = 0; i < 9; ++i) W32(a + 4 * (uint32_t)i, (uint32_t)t->f[i]);
    W32(a + 36, (uint32_t)t->gmtoff); W32(a + 40, t->zone);
}

// Noms de fuseau en mémoire invitée, internés (pointeurs stables, rt_intern_string).
static uint32_t zone_string(CPU *c, const char *name) { return rt_intern_string(c, name ? name : ""); }
static void from_host(CPU *c, const struct tm *h, GTm *t, int utc) {
    int32_t v[9] = {h->tm_sec, h->tm_min, h->tm_hour, h->tm_mday, h->tm_mon, h->tm_year, h->tm_wday, h->tm_yday, h->tm_isdst};
    memcpy(t->f, v, sizeof(v));
    t->gmtoff = utc ? 0 : (int32_t)h->tm_gmtoff;
    t->zone = zone_string(c, utc ? "GMT" : h->tm_zone);
}
static void to_host(const GTm *t, struct tm *h) {
    memset(h, 0, sizeof(*h));
    h->tm_sec = t->f[SEC]; h->tm_min = t->f[MIN]; h->tm_hour = t->f[HOUR]; h->tm_mday = t->f[MDAY];
    h->tm_mon = t->f[MON]; h->tm_year = t->f[YEAR]; h->tm_wday = t->f[WDAY]; h->tm_yday = t->f[YDAY];
    h->tm_isdst = t->f[ISDST]; h->tm_gmtoff = t->gmtoff;
}
// Conversion time_t -> struct tm (0 si impossible, errno Linux posé).
static int split(CPU *c, int32_t when, GTm *t, int utc) {
    time_t value = when; struct tm h;
    if (!(utc ? gmtime_r(&value, &h) : localtime_r(&value, &h))) { rt_set_errno(c, RT_EOVERFLOW); return 0; }
    from_host(c, &h, t, utc);
    return 1;
}
// mktime/timegm : renvoie 0 en cas d'échec (tm invité inchangé, comme glibc).
static int join(CPU *c, GTm *t, int utc, int32_t *result) {
    struct tm h; to_host(t, &h);
    errno = 0;
    time_t value = utc ? timegm(&h) : mktime(&h);
    if (value == (time_t)-1 && errno) { rt_save_errno(c); return 0; }
    if (value < INT32_MIN || value > INT32_MAX) { rt_set_errno(c, RT_EOVERFLOW); return 0; }
    from_host(c, &h, t, utc);
    *result = (int32_t)value;
    return 1;
}

// Variables importées `timezone` et `daylight` (copies invitées, rafraîchies par tzset).
static uint32_t timezone_address, daylight_address;
static void refresh_zone(void) {
    tzset();
    extern long timezone; extern int daylight;
    if (timezone_address) W32(timezone_address, (uint32_t)(int32_t)timezone);
    if (daylight_address) W32(daylight_address, (uint32_t)daylight);
}
int rt_time_data_import(const char *name, uint32_t address) {
    if (!strcmp(name, "timezone")) timezone_address = address;
    else if (!strcmp(name, "daylight")) daylight_address = address;
    else return -1;
    refresh_zone();
    return 0;
}

const char *const rt_day_names[7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
const char *const rt_month_names[12] = {"January", "February", "March", "April", "May", "June", "July",
                                        "August", "September", "October", "November", "December"};
static int leap(int64_t year) { return year % 4 == 0 && (year % 100 != 0 || year % 400 == 0); }

// asctime glibc : "%.3s %.3s%3d %.2d:%.2d:%.2d %d\n".
static uint32_t asctime_into(CPU *c, const GTm *t, uint32_t out, uint32_t size) {
    if (t->f[YEAR] > INT_MAX - 1900) { rt_set_errno(c, RT_EOVERFLOW); return 0; }
    char text[128];
    int n = snprintf(text, sizeof(text), "%.3s %.3s%3d %.2d:%.2d:%.2d %d\n",
                     t->f[WDAY] < 0 || t->f[WDAY] >= 7 ? "???" : rt_day_names[t->f[WDAY]],
                     t->f[MON] < 0 || t->f[MON] >= 12 ? "???" : rt_month_names[t->f[MON]],
                     t->f[MDAY], t->f[HOUR], t->f[MIN], t->f[SEC], 1900 + t->f[YEAR]);
    uint32_t count = (uint32_t)n + 1 < size ? (uint32_t)n + 1 : size;
    memcpy(rt_write_span(c, out, count), text, count);
    W8(out + count - 1, 0);
    if ((uint32_t)n >= size) { rt_set_errno(c, RT_EOVERFLOW); return 0; }
    return out;
}

// --- strftime --------------------------------------------------------------
typedef struct { uint32_t *v; size_t len, cap; } Buf;
static void put(Buf *b, uint32_t u) {
    if (b->len == b->cap) {
        size_t cap = b->cap ? b->cap * 2 : 64;
        uint32_t *grown = realloc(b->v, cap * sizeof(*grown));
        if (!grown) abort();
        b->v = grown; b->cap = cap;
    }
    b->v[b->len++] = u;
}
typedef struct { CPU *c; uint32_t address; int wide; const char *host; } Fmt;
static uint32_t unit(const Fmt *f, size_t i) {
    if (f->host) return (uint8_t)f->host[i];
    return f->wide ? R32(f->address + 4 * (uint32_t)i) : R8(f->address + (uint32_t)i);
}
static uint32_t recase(uint32_t u, int upper, int lower) {
    if (upper && u >= 'a' && u <= 'z') return u - 32;
    if (lower && u >= 'A' && u <= 'Z') return u + 32;
    return u;
}
static void put_units(Buf *b, const uint32_t *s, size_t n, int width, int pad, int upper, int lower) {
    for (int k = (int)n; k < width; ++k) put(b, pad == '0' ? '0' : ' ');
    for (size_t i = 0; i < n; ++i) put(b, recase(s[i], upper, lower));
}
static void put_text(Buf *b, const char *s, int width, int pad, int upper, int lower) {
    uint32_t tmp[64]; size_t n = 0;
    while (s[n] && n < 64) { tmp[n] = (uint8_t)s[n]; ++n; }
    put_units(b, tmp, n, width, pad, upper, lower);
}
// Nombre glibc : complété à `digits` chiffres (zéros, ou espaces si le
// remplissage effectif est '_', rien si '-'), puis à `width` avec des espaces,
// ou des zéros si l'indicateur '0' est explicite. `fallback` = remplissage par
// défaut de la conversion ('0', ou '_' pour %e %k %l).
static void put_number(Buf *b, int64_t value, int digits, int fallback, int pad, int width, int plus) {
    char text[32];
    int n = snprintf(text, sizeof(text), "%lld", (long long)(value < 0 ? -value : value));
    int mode = pad ? pad : fallback;
    char sign = value < 0 ? '-' : plus ? '+' : 0;
    int body = n + (sign != 0), zeros = 0, spaces = 0;
    if (mode != '-' && digits > body) { if (mode == '_') spaces = digits - body; else zeros = digits - body; }
    int total = body + zeros + spaces;
    if (width > total) { if (pad == '0') zeros += width - total; else spaces += width - total; }
    for (int k = 0; k < spaces; ++k) put(b, ' ');
    if (sign) put(b, (uint8_t)sign);
    for (int k = 0; k < zeros; ++k) put(b, '0');
    for (int i = 0; i < n; ++i) put(b, (uint8_t)text[i]);
}
static int iso_week_days(int yday, int wday) {
    int big = (366 / 7 + 2) * 7;
    return yday - (yday - wday + 4 + big) % 7 + 4 - 1;
}
static void format(CPU *c, const Fmt *f, const GTm *t, Buf *out) {
    for (size_t i = 0;;) {
        uint32_t u = unit(f, i);
        if (!u) return;
        if (u != '%') { put(out, u); ++i; continue; }
        size_t start = i++;
        int pad = 0, upper = 0, swap = 0, width = 0;
        for (;; ++i) {
            u = unit(f, i);
            if (u == '_' || u == '-' || u == '0') pad = (int)u;
            else if (u == '^') upper = 1;
            else if (u == '#') swap = 1;
            else break;
        }
        while ((u = unit(f, i)) >= '0' && u <= '9') { if (width < 100000) width = width * 10 + (int)(u - '0'); ++i; }
        if (u == 'E' || u == 'O') u = unit(f, ++i);
        if (!u) { for (size_t k = start; k < i; ++k) put(out, unit(f, k)); return; }
        ++i;
        const char *sub = NULL, *text = NULL;
        int lower = 0;
        int32_t hour12 = t->f[HOUR] % 12 == 0 ? 12 : t->f[HOUR] % 12;
        int64_t year = (int64_t)t->f[YEAR] + 1900;
        switch (u) {
        case '%': put_text(out, "%", width, pad, 0, 0); break;
        case 'n': put_text(out, "\n", width, pad, 0, 0); break;
        case 't': put_text(out, "\t", width, pad, 0, 0); break;
        case 'a': case 'A': {
            char abbreviated[4];
            const char *name = t->f[WDAY] < 0 || t->f[WDAY] > 6 ? "?" : rt_day_names[t->f[WDAY]];
            if (u == 'a' && name[1]) { snprintf(abbreviated, sizeof(abbreviated), "%s", name); name = abbreviated; }
            put_text(out, name, width, pad, upper || swap, 0); break;
        }
        case 'b': case 'h': case 'B': {
            char abbreviated[4];
            const char *name = t->f[MON] < 0 || t->f[MON] > 11 ? "?" : rt_month_names[t->f[MON]];
            if (u != 'B' && name[1]) { snprintf(abbreviated, sizeof(abbreviated), "%s", name); name = abbreviated; }
            put_text(out, name, width, pad, upper || swap, 0); break;
        }
        case 'p': case 'P':
            text = t->f[HOUR] > 11 ? "PM" : "AM";
            lower = u == 'P' || swap;
            put_text(out, text, width, pad, upper && !lower, lower); break;
        case 'Z': {
            char zone[64] = "";
            if (t->zone) {
                uint32_t n = rt_string_length(c, t->zone);
                snprintf(zone, sizeof(zone), "%.*s", (int)n, (const char *)GPTR(t->zone));
            }
            put_text(out, zone, width, pad, upper, swap); break;
        }
        case 'z': {
            if (t->f[ISDST] < 0) break;
            int32_t diff = t->gmtoff / 60;
            int64_t value = (int64_t)(diff / 60) * 100 + diff % 60;
            put_number(out, value, 5, '0', pad, width, 1);
            break;
        }
        case 'c': sub = "%a %b %e %H:%M:%S %Y"; break;
        case 'D': case 'x': sub = "%m/%d/%y"; break;
        case 'F': sub = "%Y-%m-%d"; break;
        case 'r': sub = "%I:%M:%S %p"; break;
        case 'R': sub = "%H:%M"; break;
        case 'T': case 'X': sub = "%H:%M:%S"; break;
        case 'C': {
            int64_t century = year / 100 - (year % 100 < 0);
            put_number(out, century, 2, '0', pad, width, 0); break;
        }
        case 'd': put_number(out, t->f[MDAY], 2, '0', pad, width, 0); break;
        case 'e': put_number(out, t->f[MDAY], 2, '_', pad, width, 0); break;
        case 'H': put_number(out, t->f[HOUR], 2, '0', pad, width, 0); break;
        case 'k': put_number(out, t->f[HOUR], 2, '_', pad, width, 0); break;
        case 'I': put_number(out, hour12, 2, '0', pad, width, 0); break;
        case 'l': put_number(out, hour12, 2, '_', pad, width, 0); break;
        case 'j': put_number(out, (int64_t)t->f[YDAY] + 1, 3, '0', pad, width, 0); break;
        case 'm': put_number(out, (int64_t)t->f[MON] + 1, 2, '0', pad, width, 0); break;
        case 'M': put_number(out, t->f[MIN], 2, '0', pad, width, 0); break;
        case 'S': put_number(out, t->f[SEC], 2, '0', pad, width, 0); break;
        case 'u': put_number(out, (t->f[WDAY] - 1 + 7) % 7 + 1, 1, '0', pad, width, 0); break;
        case 'w': put_number(out, t->f[WDAY], 1, '0', pad, width, 0); break;
        case 'U': put_number(out, (t->f[YDAY] - t->f[WDAY] + 7) / 7, 2, '0', pad, width, 0); break;
        case 'W': put_number(out, (t->f[YDAY] - (t->f[WDAY] - 1 + 7) % 7 + 7) / 7, 2, '0', pad, width, 0); break;
        case 'y': put_number(out, (year % 100 + 100) % 100, 2, '0', pad, width, 0); break;
        case 'Y': put_number(out, year, 1, '0', pad, width, 0); break;
        case 'G': case 'g': case 'V': {
            int64_t iso_year = year;
            int days = iso_week_days(t->f[YDAY], t->f[WDAY]);
            if (days < 0) {
                --iso_year;
                days = iso_week_days(t->f[YDAY] + 365 + leap(iso_year), t->f[WDAY]);
            } else {
                int d = iso_week_days(t->f[YDAY] - (365 + leap(iso_year)), t->f[WDAY]);
                if (d >= 0) { ++iso_year; days = d; }
            }
            if (u == 'g') put_number(out, (iso_year % 100 + 100) % 100, 2, '0', pad, width, 0);
            else if (u == 'G') put_number(out, iso_year, 1, '0', pad, width, 0);
            else put_number(out, days / 7 + 1, 2, '0', pad, width, 0);
            break;
        }
        case 's': {
            GTm copy = *t; int32_t value = 0;
            struct tm h; to_host(&copy, &h);
            time_t when = mktime(&h);
            value = (int32_t)when;
            put_number(out, value, 1, '0', pad, width, 0); break;
        }
        default:   // conversion inconnue : glibc recopie la séquence telle quelle
            for (size_t k = start; k < i; ++k) put(out, unit(f, k));
        }
        if (sub) {
            Fmt nested = {c, 0, 0, sub};
            Buf tmp = {0};
            format(c, &nested, t, &tmp);
            put_units(out, tmp.v, tmp.len, width, pad, upper, 0);
            free(tmp.v);
        }
    }
}
uint32_t rt_strftime(CPU *c, uint32_t out, uint32_t max, uint32_t fmt, uint32_t tm, int wide) {
    GTm t; read_tm(c, tm, &t);
    Fmt f = {c, fmt, wide, NULL};
    Buf b = {0};
    format(c, &f, &t, &b);
    uint32_t result = 0;
    if (b.len < max) {
        for (size_t i = 0; i < b.len; ++i) {
            if (wide) W32(out + 4 * (uint32_t)i, b.v[i]); else W8(out + (uint32_t)i, b.v[i]);
        }
        if (wide) W32(out + 4 * (uint32_t)b.len, 0); else W8(out + (uint32_t)b.len, 0);
        result = (uint32_t)b.len;
    }
    free(b.v);
    return result;
}

// --- strptime (locale C de glibc) -------------------------------------------
typedef struct {
    int have_I, is_pm, century, want_century, want_xday, have_wday, have_yday, have_mon, have_mday,
        have_uweek, have_wweek, week_no;
} Parse;
static const int month_yday[2][13] = {
    {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365},
    {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366},
};
static int space(uint8_t ch) { return ch == ' ' || (ch >= '\t' && ch <= '\r'); }
static int lower_ascii(int ch) { return ch >= 'A' && ch <= 'Z' ? ch + 32 : ch; }
static int match_name(uint32_t *rp, const char *name) {
    size_t n = strlen(name);
    for (size_t i = 0; i < n; ++i) if (lower_ascii(R8(*rp + (uint32_t)i)) != lower_ascii((uint8_t)name[i])) return 0;
    *rp += (uint32_t)n;
    return 1;
}
static int number(uint32_t *rp, int from, int to, int digits, int *value) {
    while (space(R8(*rp))) ++*rp;
    uint8_t ch = R8(*rp);
    if (ch < '0' || ch > '9') return 0;
    int v = 0;
    do { v = v * 10 + (R8((*rp)++) - '0'); }
    while (--digits > 0 && v * 10 <= to && (ch = R8(*rp)) >= '0' && ch <= '9');
    if (v < from || v > to) return 0;
    *value = v;
    return 1;
}
static void day_of_week(GTm *t) {
    int corr_year = 1900 + t->f[YEAR] - (t->f[MON] < 2);
    int wday = (-473 + (365 * (t->f[YEAR] - 70)) + (corr_year / 4) - ((corr_year / 4) / 25) + ((corr_year / 4) % 25 < 0) +
                (((corr_year / 4) / 25) / 4) + month_yday[0][t->f[MON]] + t->f[MDAY] - 1);
    t->f[WDAY] = ((wday % 7) + 7) % 7;
}
static void day_of_year(GTm *t) {
    t->f[YDAY] = month_yday[leap(1900 + (int64_t)t->f[YEAR])][t->f[MON]] + (t->f[MDAY] - 1);
}
static int parse(CPU *c, uint32_t *rp, const char *fmt, GTm *t, Parse *s) {
    while (*fmt) {
        if (space((uint8_t)*fmt)) { while (space(R8(*rp))) ++*rp; ++fmt; continue; }
        if (*fmt != '%') { if (R8(*rp) != (uint8_t)*fmt) return 0; ++*rp; ++fmt; continue; }
        ++fmt;
        while (*fmt == '-' || *fmt == '_' || *fmt == '0' || *fmt == '^' || *fmt == '#') ++fmt;
        while (*fmt >= '0' && *fmt <= '9') ++fmt;
        if (*fmt == 'E' || *fmt == 'O') ++fmt;
        int v = 0;
        switch (*fmt++) {
        case '%': if (R8(*rp) != '%') return 0; ++*rp; break;
        case 'a': case 'A': {
            int found = -1;
            for (int d = 0; d < 7 && found < 0; ++d) {
                char abbreviated[4]; snprintf(abbreviated, sizeof(abbreviated), "%s", rt_day_names[d]);
                if (match_name(rp, rt_day_names[d]) || match_name(rp, abbreviated)) found = d;
            }
            if (found < 0) return 0;
            t->f[WDAY] = found; s->have_wday = 1; break;
        }
        case 'b': case 'B': case 'h': {
            int found = -1;
            for (int m = 0; m < 12 && found < 0; ++m) {
                char abbreviated[4]; snprintf(abbreviated, sizeof(abbreviated), "%s", rt_month_names[m]);
                if (match_name(rp, rt_month_names[m]) || match_name(rp, abbreviated)) found = m;
            }
            if (found < 0) return 0;
            t->f[MON] = found; s->have_mon = 1; s->want_xday = 1; break;
        }
        case 'c': if (!parse(c, rp, "%a %b %e %H:%M:%S %Y", t, s)) return 0; s->want_xday = 1; break;
        case 'C': if (!number(rp, 0, 99, 2, &v)) return 0; s->century = v; s->want_xday = 1; break;
        case 'd': case 'e': if (!number(rp, 1, 31, 2, &v)) return 0; t->f[MDAY] = v; s->have_mday = 1; s->want_xday = 1; break;
        case 'F': if (!parse(c, rp, "%Y-%m-%d", t, s)) return 0; s->want_xday = 1; break;
        case 'x': case 'D': if (!parse(c, rp, "%m/%d/%y", t, s)) return 0; s->want_xday = 1; break;
        case 'k': case 'H': if (!number(rp, 0, 23, 2, &v)) return 0; t->f[HOUR] = v; s->have_I = 0; break;
        case 'l': case 'I': if (!number(rp, 1, 12, 2, &v)) return 0; t->f[HOUR] = v % 12; s->have_I = 1; break;
        case 'j': if (!number(rp, 1, 366, 3, &v)) return 0; t->f[YDAY] = v - 1; s->have_yday = 1; break;
        case 'm': if (!number(rp, 1, 12, 2, &v)) return 0; t->f[MON] = v - 1; s->have_mon = 1; s->want_xday = 1; break;
        case 'M': if (!number(rp, 0, 59, 2, &v)) return 0; t->f[MIN] = v; break;
        case 'n': case 't': while (space(R8(*rp))) ++*rp; break;
        case 'p':
            if (match_name(rp, "AM")) s->is_pm = 0;
            else if (match_name(rp, "PM")) s->is_pm = 1;
            else return 0;
            break;
        case 'r': if (!parse(c, rp, "%I:%M:%S %p", t, s)) return 0; break;
        case 'R': if (!parse(c, rp, "%H:%M", t, s)) return 0; break;
        case 's': {
            int negative = R8(*rp) == '-';
            if (negative) ++*rp;
            uint8_t ch = R8(*rp);
            if (ch < '0' || ch > '9') return 0;
            int64_t secs = 0;
            while ((ch = R8(*rp)) >= '0' && ch <= '9') {
                secs = secs * 10 + (ch - '0');
                if (secs > (int64_t)INT32_MAX + 1) return 0;
                ++*rp;
            }
            if (negative) secs = -secs;
            if (secs < INT32_MIN || secs > INT32_MAX) return 0;
            GTm split_tm = *t;
            if (!split(c, (int32_t)secs, &split_tm, 0)) return 0;
            *t = split_tm;
            break;
        }
        case 'S': if (!number(rp, 0, 61, 2, &v)) return 0; t->f[SEC] = v; break;
        case 'X': case 'T': if (!parse(c, rp, "%H:%M:%S", t, s)) return 0; break;
        case 'u': if (!number(rp, 1, 7, 1, &v)) return 0; t->f[WDAY] = v % 7; s->have_wday = 1; break;
        case 'g': if (!number(rp, 0, 99, 2, &v)) return 0; break;   // ne détermine aucune date
        case 'G': {
            uint8_t ch = R8(*rp);
            if (ch < '0' || ch > '9') return 0;
            do ++*rp; while ((ch = R8(*rp)) >= '0' && ch <= '9');
            break;
        }
        case 'U': if (!number(rp, 0, 53, 2, &v)) return 0; s->week_no = v; s->have_uweek = 1; s->have_wweek = 0; break;
        case 'W': if (!number(rp, 0, 53, 2, &v)) return 0; s->week_no = v; s->have_wweek = 1; s->have_uweek = 0; break;
        case 'V': if (!number(rp, 0, 53, 2, &v)) return 0; break;
        case 'w': if (!number(rp, 0, 6, 1, &v)) return 0; t->f[WDAY] = v; s->have_wday = 1; break;
        case 'y':
            if (!number(rp, 0, 99, 2, &v)) return 0;
            t->f[YEAR] = v >= 69 ? v : v + 100; s->want_century = 1; s->want_xday = 1; break;
        case 'Y':
            if (!number(rp, 0, 9999, 4, &v)) return 0;
            t->f[YEAR] = v - 1900; s->want_century = 0; s->want_xday = 1; break;
        case 'Z':   // lu, sans conversion
            while (space(R8(*rp))) ++*rp;
            while (R8(*rp) && !space(R8(*rp))) ++*rp;
            break;
        case 'z': {
            while (space(R8(*rp))) ++*rp;
            if (R8(*rp) == 'Z') { ++*rp; t->gmtoff = 0; break; }
            uint8_t ch = R8(*rp);
            if (ch != '+' && ch != '-') return 0;
            int negative = ch == '-', n = 0; ++*rp; v = 0;
            while (n < 4 && (ch = R8(*rp)) >= '0' && ch <= '9') {
                v = v * 10 + (ch - '0'); ++*rp; ++n;
                if (R8(*rp) == ':' && n == 2 && R8(*rp + 1) >= '0' && R8(*rp + 1) <= '9') ++*rp;
            }
            if (n == 2) v *= 100;
            else if (n != 4 || v % 100 >= 60) return 0;
            t->gmtoff = (v / 100) * 3600 + (v % 100) * 60;
            if (negative) t->gmtoff = -t->gmtoff;
            break;
        }
        default: return 0;
        }
    }
    return 1;
}
static uint32_t strptime_guest(CPU *c, uint32_t input, const char *fmt, uint32_t tm) {
    GTm t; read_tm(c, tm, &t);
    Parse s = {0}; s.century = -1;
    uint32_t rp = input;
    if (!parse(c, &rp, fmt, &t, &s)) return 0;
    if (s.have_I && s.is_pm) t.f[HOUR] += 12;
    if (s.century != -1) {
        if (s.want_century) t.f[YEAR] = t.f[YEAR] % 100 + (s.century - 19) * 100;
        else t.f[YEAR] = (s.century - 19) * 100;
    }
    if (s.want_xday && !s.have_wday) {
        if (!(s.have_mon && s.have_mday) && s.have_yday) {
            int l = leap(1900 + (int64_t)t.f[YEAR]), m = 0;
            while (m < 12 && month_yday[l][m] <= t.f[YDAY]) ++m;
            if (!s.have_mon) t.f[MON] = m - 1;
            if (!s.have_mday) t.f[MDAY] = t.f[YDAY] - month_yday[l][m - 1] + 1;
            s.have_mon = s.have_mday = 1;
        }
        if (t.f[YEAR] >= -1900 && (uint32_t)t.f[MON] <= 11) day_of_week(&t);
    }
    if (s.want_xday && !s.have_yday && (s.have_mon || (uint32_t)t.f[MON] <= 11) && (uint32_t)t.f[MON] <= 11) day_of_year(&t);
    if ((s.have_uweek || s.have_wweek) && s.have_wday) {
        int save_wday = t.f[WDAY], save_mday = t.f[MDAY], save_mon = t.f[MON];
        int offset = s.have_uweek ? 0 : 1;
        t.f[MDAY] = 1; t.f[MON] = 0;
        day_of_week(&t);
        if (s.have_mday) t.f[MDAY] = save_mday;
        if (s.have_mon) t.f[MON] = save_mon;
        if (!s.have_yday)
            t.f[YDAY] = ((7 - (t.f[WDAY] - offset)) % 7 + (s.week_no - 1) * 7 + (save_wday - offset + 7) % 7);
        if (!s.have_mday || !s.have_mon) {
            int l = leap(1900 + (int64_t)t.f[YEAR]), m = 0;
            while (m < 12 && month_yday[l][m] <= t.f[YDAY]) ++m;
            if (m == 0) m = 1;
            if (!s.have_mon) t.f[MON] = m - 1;
            if (!s.have_mday) t.f[MDAY] = t.f[YDAY] - month_yday[l][m - 1] + 1;
        }
        t.f[WDAY] = save_wday;
    }
    write_tm(c, tm, &t);
    return rp;
}

static int time_shim(CPU *c, const char *name) {
    if (!strcmp(name, "nanosleep")) {   // timespec i386 : {int32 tv_sec, int32 tv_nsec}
        uint32_t request = rt_arg32(c, 0), remain = rt_arg32(c, 1);
        int32_t sec = (int32_t)R32(request), nsec = (int32_t)R32(request + 4);
        if (sec < 0 || nsec < 0 || nsec > 999999999) { rt_set_errno(c, RT_EINVAL); c->eax = UINT32_MAX; return 1; }
        struct timespec delay = {sec, nsec}, left = {0, 0};
        unsigned saved = rt_guest_suspend();
        int result = nanosleep(&delay, &left), error = errno;
        rt_guest_resume(saved);
        if (result) { errno = error; rt_save_errno(c); if (remain) { W32(remain, (uint32_t)left.tv_sec); W32(remain + 4, (uint32_t)left.tv_nsec); } }
        c->eax = (uint32_t)result;
        return 1;
    }
    if (!strcmp(name, "sleep")) {       // renvoie les secondes non dormies
        unsigned saved = rt_guest_suspend();
        unsigned left = sleep(rt_arg32(c, 0));
        rt_guest_resume(saved);
        c->eax = left;
        return 1;
    }
    if (!strcmp(name, "times")) {
        // struct tms i386 : 4 clock_t de 32 bits, en tops de sysconf(_SC_CLK_TCK) = 100 sous Linux.
        uint32_t out = rt_arg32(c, 0);
        struct timespec user = {0, 0}, now;
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &user);
        clock_gettime(CLOCK_MONOTONIC, &now);
        if (out) {
            rt_write_span(c, out, 16);
            W32(out, (uint32_t)(user.tv_sec * 100 + user.tv_nsec / 10000000));   // utilisateur + système confondus
            W32(out + 4, 0); W32(out + 8, 0); W32(out + 12, 0);
        }
        c->eax = (uint32_t)(now.tv_sec * 100 + now.tv_nsec / 10000000);
        return 1;
    }
    static uint32_t tmbuf, ascbuf;
    if (!strcmp(name, "time")) {
        time_t now = time(NULL); uint32_t out = rt_arg32(c, 0);
        if (now > INT32_MAX) { rt_set_errno(c, RT_EOVERFLOW); c->eax = UINT32_MAX; return 1; }
        if (out) W32(out, (uint32_t)now);
        c->eax = (uint32_t)now;
    } else if (!strcmp(name, "clock")) {
        struct timespec ts;
        if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts)) { c->eax = UINT32_MAX; return 1; }
        c->eax = (uint32_t)((uint64_t)ts.tv_sec * 1000000u + (uint64_t)ts.tv_nsec / 1000u);   // CLOCKS_PER_SEC
    } else if (!strcmp(name, "difftime")) {
        rt_return_st0(c, (double)(int32_t)rt_arg32(c, 0) - (double)(int32_t)rt_arg32(c, 1));
    } else if (!strcmp(name, "mktime") || !strcmp(name, "timegm")) {
        uint32_t address = rt_arg32(c, 0); GTm t; int32_t result;
        read_tm(c, address, &t);
        if (name[0] == 'm') refresh_zone();
        if (join(c, &t, name[0] == 't', &result)) { write_tm(c, address, &t); c->eax = (uint32_t)result; }
        else c->eax = UINT32_MAX;
    } else if (!strcmp(name, "gmtime") || !strcmp(name, "localtime") || !strcmp(name, "gmtime_r") || !strcmp(name, "localtime_r")) {
        int utc = name[0] == 'g', reentrant = name[strlen(name) - 1] == 'r';
        uint32_t out = reentrant ? rt_arg32(c, 1) : tmbuf;
        if (!out && !(out = tmbuf = rt_alloc(TM_SIZE))) rt_trap(c, c->eip, "cannot allocate guest tm buffer");
        if (!utc && !reentrant) refresh_zone();
        GTm t;
        c->eax = split(c, (int32_t)R32(rt_arg32(c, 0)), &t, utc) ? (write_tm(c, out, &t), out) : 0;
    } else if (!strcmp(name, "asctime") || !strcmp(name, "asctime_r") || !strcmp(name, "ctime") || !strcmp(name, "ctime_r")) {
        int reentrant = name[strlen(name) - 1] == 'r';
        if (!reentrant && !ascbuf && !(ascbuf = rt_alloc(114))) rt_trap(c, c->eip, "cannot allocate guest asctime buffer");
        uint32_t out = reentrant ? rt_arg32(c, 1) : ascbuf, size = reentrant ? 26 : 114;
        GTm t;
        if (name[0] == 'a') {
            if (!rt_arg32(c, 0)) { rt_set_errno(c, RT_EINVAL); c->eax = 0; return 1; }
            read_tm(c, rt_arg32(c, 0), &t);
        } else {
            // ctime = asctime(localtime(t)) : le tampon tm statique est aussi écrit.
            if (!reentrant) refresh_zone();
            if (!split(c, (int32_t)R32(rt_arg32(c, 0)), &t, 0)) { c->eax = 0; return 1; }
            if (!reentrant) {
                if (!tmbuf && !(tmbuf = rt_alloc(TM_SIZE))) rt_trap(c, c->eip, "cannot allocate guest tm buffer");
                write_tm(c, tmbuf, &t);
            }
        }
        c->eax = asctime_into(c, &t, out, size);
    } else if (!strcmp(name, "strftime") || !strcmp(name, "__strftime_l") || !strcmp(name, "__wcsftime_l")) {
        if (name[0] == '_') rt_locale_object_utf8(c, rt_arg32(c, 4));
        c->eax = rt_strftime(c, rt_arg32(c, 0), rt_arg32(c, 1), rt_arg32(c, 2), rt_arg32(c, 3), name[2] == 'w');
    } else if (!strcmp(name, "strptime")) {
        uint32_t input = rt_arg32(c, 0), fmt = rt_arg32(c, 1);
        rt_string_length(c, input); rt_string_length(c, fmt);
        c->eax = strptime_guest(c, input, GPTR(fmt), rt_arg32(c, 2));
    } else if (!strcmp(name, "tzset")) {
        refresh_zone();
    } else if (!strcmp(name, "utime")) {
        uint32_t path = rt_arg32(c, 0), times = rt_arg32(c, 1);
        rt_string_length(c, path);
        // Métadonnées modifiées : jamais sur la copie originale du jeu.
        const char *resolved = rt_write_path(GPTR(path), 1);
        struct utimbuf value, *pointer = NULL;
        if (times) { value.actime = (int32_t)R32(times); value.modtime = (int32_t)R32(times + 4); pointer = &value; }
        int result = resolved ? utime(resolved, pointer) : -1;
        if (result) rt_save_errno(c);
        c->eax = (uint32_t)result;
    } else return 0;
    return 1;
}
RT_SHIM_PROVIDER(time_shim)
