// Decode i386 va_list explicitly: Darwin ARM64 va_list and C long sizes differ.
// Famille printf de la glibc i386 : narrow (char) et large (wchar_t Linux =
// 4 octets UTF-32), plus les variantes _FORTIFY_SOURCE (__*printf_chk).
// SHIMS: sprintf vsprintf swprintf vswprintf
// SHIMS: __snprintf_chk __sprintf_chk __vsnprintf_chk __vsprintf_chk __fprintf_chk
// SHIMS: __printf_chk __vprintf_chk __vfprintf_chk __swprintf_chk __vswprintf_chk
#include "rt.h"
#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct { CPU *cpu; uint64_t cursor; } Arguments;
static uint32_t take32(Arguments *args) {
    if (args->cursor > UINT32_MAX - 3) rt_trap(args->cpu, args->cpu->eip, "variadic argument overflow");
    uint32_t value = R32((uint32_t)args->cursor); args->cursor += 4; return value;
}
static uint64_t take64(Arguments *args) {
    uint64_t lo = take32(args), hi = take32(args);
    return lo | (hi << 32);
}
static uint32_t argument(CPU *c, unsigned i) { return R32(c->esp + 4 + 4 * i); }

// Spécification de conversion déjà lue ; `native` est le format hôte
// équivalent (entiers toujours élargis en long long).
typedef struct {
    char flags[8];
    int width, precision;
    char length[3];
    char conversion;
    char native[64];
} Spec;

// Lit une spécification (après le '%') ; renvoie 0 en cas de largeur ou
// précision hors int (glibc : -1/EOVERFLOW), 1 sinon.
static int parse_spec(CPU *c, const char **cursor, Arguments *args, Spec *s) {
    const char *p = *cursor;
    memset(s, 0, sizeof(*s));
    size_t flag_count = 0;
    while (*p && strchr("-+ #0", *p)) {
        if (!strchr(s->flags, *p)) {
            if (flag_count >= sizeof(s->flags) - 2) rt_trap(c, c->eip, "format flags overflow");
            s->flags[flag_count++] = *p;
        }
        ++p;
    }
    s->precision = -1;
    if (*p == '*') {
        s->width = (int32_t)take32(args); ++p;
        if (s->width == INT_MIN) return 0;
        if (s->width < 0) {
            s->width = -s->width;
            if (!strchr(s->flags, '-')) s->flags[flag_count++] = '-';
        }
    } else while (isdigit((unsigned char)*p)) {
        if (s->width > (INT_MAX - (*p - '0')) / 10) return 0;
        s->width = s->width * 10 + *p++ - '0';
    }
    if (*p == '.') {
        ++p; s->precision = 0;
        if (*p == '*') { s->precision = (int32_t)take32(args); ++p; if (s->precision < 0) s->precision = -1; }
        else while (isdigit((unsigned char)*p)) {
            if (s->precision > (INT_MAX - (*p - '0')) / 10) return 0;
            s->precision = s->precision * 10 + *p++ - '0';
        }
    }
    if (*p && strchr("hljztL", *p)) {
        s->length[0] = *p++;
        if ((s->length[0] == 'h' || s->length[0] == 'l') && *p == s->length[0]) s->length[1] = *p++;
    }
    s->conversion = *p;
    if (!s->conversion) rt_trap(c, c->eip, "incomplete guest format");
    ++p;
    *cursor = p;
    char digits[24];
    if (s->conversion == 'p' && !strchr(s->flags, '#')) s->flags[flag_count++] = '#';  // glibc : %p = %#x
    snprintf(s->native, sizeof(s->native), "%%%s", s->flags);
    if (s->width) { snprintf(digits, sizeof(digits), "%d", s->width); strcat(s->native, digits); }
    if (s->precision >= 0) { snprintf(digits, sizeof(digits), ".%d", s->precision); strcat(s->native, digits); }
    char conversion = s->conversion;
    if (conversion == 'p') { strcat(s->native, "ll"); conversion = 'x'; }
    else if (strchr("diuoxX", conversion)) {
        if (s->length[0] == 'L') rt_trap(c, c->eip, "invalid integer format length");
        strcat(s->native, "ll");
    }
    size_t end = strlen(s->native); s->native[end] = conversion; s->native[end + 1] = 0;
    return 1;
}
static int is_integer(char conversion) { return strchr("diuoxX", conversion) != NULL; }
static int is_floating(const Spec *s) {
    return strchr("fFeEgGaA", s->conversion) && (!s->length[0] || !strcmp(s->length, "l"));
}
static uint64_t integer_value(Arguments *args, const Spec *s) {
    int wide = !strcmp(s->length, "ll") || !strcmp(s->length, "j");
    uint64_t value = wide ? take64(args) : take32(args);
    if (s->conversion == 'd' || s->conversion == 'i') {
        int64_t v = wide ? (int64_t)value : (int32_t)value;
        if (!strcmp(s->length, "h")) v = (int16_t)value;
        if (!strcmp(s->length, "hh")) v = (int8_t)value;
        return (uint64_t)v;
    }
    if (!strcmp(s->length, "h")) value = (uint16_t)value;
    if (!strcmp(s->length, "hh")) value = (uint8_t)value;
    return value;
}
static void store_count(CPU *c, uint32_t address, const Spec *s, uint64_t used) {
    unsigned size = !strcmp(s->length, "hh") ? 1 : !strcmp(s->length, "h") ? 2 :
                    (!strcmp(s->length, "ll") || !strcmp(s->length, "j")) ? 8 : 4;
    rt_store_integer(c, address, size, used);
}

// `fortify` > 0 : appel __*printf_chk avec flag > 0 (_FORTIFY_SOURCE=2).
static int guest_format(CPU *c, uint32_t destination, uint32_t capacity,
                        uint32_t format, uint32_t argv, FILE *stream, int fortify) {
    if (!stream) rt_write_span(c, destination, capacity);
    rt_string_length(c, format);
    const char *p = GPTR(format);
    Arguments args = {c, argv};
    uint64_t used = 0;
    #define OUTPUT_VALUE(value) (stream ? fprintf(stream, s.native, (value)) : snprintf(out, available, s.native, (value)))
    while (*p) {
        if (*p != '%') {
            if (stream) { if (fputc((unsigned char)*p, stream) == EOF) return -1; }
            else if (used + 1 < capacity) W8(destination + (uint32_t)used, (uint8_t)*p);
            ++used; ++p; continue;
        }
        ++p;
        if (*p == '%') {
            if (stream) { if (fputc('%', stream) == EOF) return -1; }
            else if (used + 1 < capacity) W8(destination + (uint32_t)used, '%');
            ++used; ++p; continue;
        }
        Spec s;
        if (!parse_spec(c, &p, &args, &s)) return -1;
        size_t available = used < capacity ? (size_t)(capacity - used) : 0;
        char *out = available ? GPTR(destination + (uint32_t)used) : NULL;
        int count;
        if (is_integer(s.conversion)) {
            uint64_t value = integer_value(&args, &s);
            if (s.conversion == 'd' || s.conversion == 'i') count = OUTPUT_VALUE((long long)value);
            else count = OUTPUT_VALUE((unsigned long long)value);
        } else if (s.conversion == 's' && !s.length[0]) {
            uint32_t address = take32(&args);
            if (!address) {
                // glibc : "(null)" si la précision le permet entièrement, sinon "".
                const char *text = s.precision < 0 || s.precision >= 6 ? "(null)" : "";
                count = OUTPUT_VALUE(text);
            } else {
            if (s.precision < 0) rt_string_length(c, address);
            else rt_bounded_strnlen(c, address, (uint32_t)s.precision);
            count = OUTPUT_VALUE((char *)GPTR(address));
            }
        } else if (s.conversion == 'c' && !s.length[0]) {
            count = OUTPUT_VALUE((int)take32(&args));
        } else if (s.conversion == 'p' && !s.length[0]) {
            uint32_t pointer = take32(&args);
            if (pointer) count = OUTPUT_VALUE((unsigned long long)pointer);
            else {
                // glibc : "(nil)" avec la largeur et '-' ; la précision ne tronque pas.
                if (s.width) snprintf(s.native, sizeof(s.native), "%%%s%ds", strchr(s.flags, '-') ? "-" : "", s.width);
                else strcpy(s.native, "%s");
                count = OUTPUT_VALUE("(nil)");
            }
        } else if (is_floating(&s)) {
            uint64_t bits = take64(&args); double value; memcpy(&value, &bits, 8);
            count = OUTPUT_VALUE(value);
        } else if (s.conversion == 'n' && s.length[0] != 'L') {
            if (fortify > 0)
                rt_trap(c, c->eip, "%n under _FORTIFY_SOURCE=2 (writable-format check not modeled)");
            store_count(c, take32(&args), &s, used);
            count = 0;
        } else if ((s.conversion == 's' && !strcmp(s.length, "l")) || (s.conversion == 'S' && !s.length[0])) {
            // %ls : glibc convertit par wcrtomb dans la locale C, que seuls les
            // caractères ASCII traversent ; sinon -1 (EILSEQ). Précision en octets.
            uint32_t address = take32(&args);
            s.native[strlen(s.native) - 1] = 's';
            if (!address) count = OUTPUT_VALUE(s.precision < 0 || s.precision >= 6 ? "(null)" : "");
            else {
                size_t limit = s.precision >= 0 ? (size_t)s.precision : SIZE_MAX, n = 0, capacity = 64;
                char *text = malloc(capacity);
                if (!text) rt_trap(c, c->eip, "printf %ls: out of host memory");
                for (uint32_t at = address; n < limit; at += 4) {
                    rt_span(c, at, 4);
                    uint32_t unit = R32(at);
                    if (!unit) break;
                    if (unit > 0x7f) { free(text); return -1; }
                    if (n + 1 >= capacity) {
                        char *grown = realloc(text, capacity *= 2);
                        if (!grown) { free(text); rt_trap(c, c->eip, "printf %ls: out of host memory"); }
                        text = grown;
                    }
                    text[n++] = (char)unit;
                }
                text[n] = 0;
                count = OUTPUT_VALUE(text);
                free(text);
            }
        } else if ((s.conversion == 'c' && !strcmp(s.length, "l")) || (s.conversion == 'C' && !s.length[0])) {
            uint32_t unit = take32(&args);   // %lc : même règle (locale C)
            if (unit > 0x7f) return -1;
            s.native[strlen(s.native) - 1] = 'c';
            count = OUTPUT_VALUE((int)unit);
        } else {
            static _Thread_local char message[96];
            snprintf(message, sizeof(message), "unsupported guest format conversion %%%s%c", s.length, s.conversion);
            rt_trap(c, c->eip, message);
        }
        if (count < 0) return -1;
        used += (unsigned)count;
        if (used > INT_MAX) return -1;
    }
    #undef OUTPUT_VALUE
    if (!stream && capacity) W8(destination + (uint32_t)(used < capacity ? used : capacity - 1), 0);
    return used <= INT_MAX ? (int)used : -1;
}

// sprintf : taille de destination inconnue. Une première passe mesure la
// sortie, la seconde écrit exactement used + 1 octets (les %n écrivent deux
// fois la même valeur). `limit` (__sprintf_chk) : taille réelle de l'objet.
static int unbounded_format(CPU *c, uint32_t destination, uint32_t format, uint32_t argv,
                            int fortify, uint64_t limit) {
    int length = guest_format(c, destination, 0, format, argv, NULL, fortify);
    if (length < 0) return -1;
    if ((uint64_t)length + 1 > limit)
        rt_trap(c, c->eip, "*** buffer overflow detected ***: terminated (__sprintf_chk)");
    return guest_format(c, destination, (uint32_t)length + 1, format, argv, NULL, fortify);
}

// ---- Variante large (wchar_t = UTF-32 sur 4 octets) ----

typedef struct { CPU *cpu; uint32_t *data; size_t length, capacity; } Wide;
static void wide_put(Wide *w, uint32_t unit) {
    if (w->length == w->capacity) {
        size_t capacity = w->capacity ? w->capacity * 2 : 256;
        uint32_t *grown = realloc(w->data, capacity * sizeof(*grown));
        if (!grown) rt_trap(w->cpu, w->cpu->eip, "swprintf: out of host memory");
        w->data = grown; w->capacity = capacity;
    }
    w->data[w->length++] = unit;
}
static void wide_pad(Wide *w, int count) { for (int i = 0; i < count; ++i) wide_put(w, ' '); }
// Chaîne déjà convertie en unités larges, avec largeur, précision et '-'.
static void wide_field(Wide *w, const Spec *s, const uint32_t *units, size_t count) {
    if (s->precision >= 0 && count > (size_t)s->precision) count = (size_t)s->precision;
    int pad = s->width > (int)count ? s->width - (int)count : 0;
    int left = strchr(s->flags, '-') != NULL;
    if (!left) wide_pad(w, pad);
    for (size_t i = 0; i < count; ++i) wide_put(w, units[i]);
    if (left) wide_pad(w, pad);
}
// Locale C de glibc : seuls les octets ASCII se convertissent (mbrtowc/btowc),
// le reste dépend de setlocale, que le runtime ne fournit pas.
static uint32_t ascii_unit(CPU *c, uint32_t byte) {
    if (byte > 0x7f) rt_trap(c, c->eip, "wide printf: non-ASCII multibyte input (locale not modeled)");
    return byte;
}

static int wide_format(CPU *c, uint32_t format, uint32_t argv, Wide *w) {
    Arguments args = {c, argv};
    uint32_t at = format;
    for (;;) {
        rt_span(c, at, 4);
        uint32_t unit = R32(at); at += 4;
        if (!unit) break;
        if (unit != '%') { wide_put(w, unit); continue; }
        // Recopie la spécification (ASCII) pour l'analyseur commun.
        char spec[64]; size_t n = 0;
        for (;;) {
            rt_span(c, at, 4);
            uint32_t u = R32(at);
            if (!u || u > 0x7f) rt_trap(c, c->eip, "incomplete guest wide format");
            if (n + 1 >= sizeof(spec)) rt_trap(c, c->eip, "guest wide format specification too long");
            spec[n++] = (char)u; at += 4;
            if (!strchr("-+ #0123456789.*hljztL", (int)u)) break;
        }
        spec[n] = 0;
        if (spec[0] == '%' && n == 1) { wide_put(w, '%'); continue; }
        const char *p = spec;
        Spec s;
        if (!parse_spec(c, &p, &args, &s)) return -1;
        char narrow[512];
        int count = -1;
        if (is_integer(s.conversion)) {
            uint64_t value = integer_value(&args, &s);
            count = s.conversion == 'd' || s.conversion == 'i'
                ? snprintf(narrow, sizeof(narrow), s.native, (long long)value)
                : snprintf(narrow, sizeof(narrow), s.native, (unsigned long long)value);
        } else if (is_floating(&s)) {
            uint64_t bits = take64(&args); double value; memcpy(&value, &bits, 8);
            count = snprintf(narrow, sizeof(narrow), s.native, value);
        } else if (s.conversion == 'p' && !s.length[0]) {
            uint32_t pointer = take32(&args);
            if (pointer) count = snprintf(narrow, sizeof(narrow), s.native, (unsigned long long)pointer);
            else { static const uint32_t nil[] = {'(', 'n', 'i', 'l', ')'}; Spec t = s; t.precision = -1; wide_field(w, &t, nil, 5); continue; }
        } else if (s.conversion == 's' || s.conversion == 'S') {
            int wide = s.conversion == 'S' || !strcmp(s.length, "l");
            if (!wide && s.length[0]) rt_trap(c, c->eip, "unsupported guest wide format length");
            uint32_t address = take32(&args);
            size_t limit = s.precision >= 0 ? (size_t)s.precision : SIZE_MAX, length = 0;
            uint32_t *units = NULL; size_t capacity = 0;
            if (!address) {
                // glibc écrit "(null)" (si la précision le permet entièrement).
                static const uint32_t null[] = {'(', 'n', 'u', 'l', 'l', ')'};
                if (s.precision < 0 || s.precision >= 6) wide_field(w, &s, null, 6);
                else { Spec t = s; t.precision = 0; wide_field(w, &t, null, 0); }
                continue;
            }
            for (;;) {
                if (length >= limit) break;
                uint32_t value;
                if (wide) { rt_span(c, address + (uint32_t)length * 4, 4); value = R32(address + (uint32_t)length * 4); }
                else { rt_span(c, address + (uint32_t)length, 1); value = R8(address + (uint32_t)length); }
                if (!value) break;
                if (!wide) value = ascii_unit(c, value);
                if (length == capacity) {
                    capacity = capacity ? capacity * 2 : 64;
                    uint32_t *grown = realloc(units, capacity * sizeof(*grown));
                    if (!grown) { free(units); rt_trap(c, c->eip, "swprintf: out of host memory"); }
                    units = grown;
                }
                units[length++] = value;
            }
            wide_field(w, &s, units, length);
            free(units);
            continue;
        } else if (s.conversion == 'c' || s.conversion == 'C') {
            int wide = s.conversion == 'C' || !strcmp(s.length, "l");
            if (!wide && s.length[0]) rt_trap(c, c->eip, "unsupported guest wide format length");
            uint32_t value = take32(&args);
            if (!wide) value = ascii_unit(c, value & 0xff);
            Spec t = s; t.precision = -1;
            wide_field(w, &t, &value, 1);
            continue;
        } else if (s.conversion == 'n' && s.length[0] != 'L') {
            store_count(c, take32(&args), &s, w->length);
            continue;
        } else rt_trap(c, c->eip, "unsupported guest wide format conversion");
        if (count < 0) return -1;
        if ((size_t)count >= sizeof(narrow)) rt_trap(c, c->eip, "wide printf: numeric field too wide");
        for (int i = 0; i < count; ++i) wide_put(w, (unsigned char)narrow[i]);
        if (w->length > INT_MAX) return -1;
    }
    return 0;
}

// glibc _IO_vswprintf : maxlen == 0 → -1 ; au plus maxlen-1 caractères ;
// si la sortie ne tient pas, -1 et la chaîne n'est pas terminée.
static int guest_swprintf(CPU *c, uint32_t destination, uint32_t maxlen, uint32_t format, uint32_t argv) {
    if (!maxlen) return -1;
    rt_write_span(c, destination, (uint64_t)maxlen * 4);
    Wide w = {c, NULL, 0, 0};
    int status = wide_format(c, format, argv, &w);
    int result;
    if (status < 0) result = -1;
    else if (w.length > (size_t)maxlen - 1) {
        for (uint32_t i = 0; i < maxlen - 1; ++i) W32(destination + 4 * i, w.data[i]);
        result = -1;
    } else {
        for (size_t i = 0; i < w.length; ++i) W32(destination + 4 * (uint32_t)i, w.data[i]);
        W32(destination + 4 * (uint32_t)w.length, 0);
        result = (int)w.length;
    }
    free(w.data);
    return result;
}

int rt_format_shim(CPU *c, const char *name) {
    if (!strcmp(name, "snprintf") || !strcmp(name, "vsnprintf")) {
        uint32_t argv = !strcmp(name, "vsnprintf") ? argument(c, 3) : c->esp + 16;
        c->eax = (uint32_t)guest_format(c, argument(c, 0), argument(c, 1), argument(c, 2), argv, NULL, 0);
        return 1;
    }
    if (!strcmp(name, "sprintf") || !strcmp(name, "vsprintf")) {
        uint32_t argv = !strcmp(name, "vsprintf") ? argument(c, 2) : c->esp + 12;
        c->eax = (uint32_t)unbounded_format(c, argument(c, 0), argument(c, 1), argv, 0, UINT64_MAX);
        return 1;
    }
    if (!strcmp(name, "printf") || !strcmp(name, "vprintf")) {
        uint32_t argv = !strcmp(name, "vprintf") ? argument(c, 1) : c->esp + 8;
        c->eax = (uint32_t)guest_format(c, 0, 0, argument(c, 0), argv, stdout, 0);
        return 1;
    }
    if (!strcmp(name, "fprintf") || !strcmp(name, "vfprintf")) {
        uint32_t argv = !strcmp(name, "vfprintf") ? argument(c, 2) : c->esp + 12;
        FILE *stream = rt_stream_file(c, argument(c, 0));
        c->eax = (uint32_t)guest_format(c, 0, 0, argument(c, 1), argv, stream, 0);
        return 1;
    }
    if (!strcmp(name, "swprintf") || !strcmp(name, "vswprintf")) {
        uint32_t argv = !strcmp(name, "vswprintf") ? argument(c, 3) : c->esp + 16;
        c->eax = (uint32_t)guest_swprintf(c, argument(c, 0), argument(c, 1), argument(c, 2), argv);
        return 1;
    }
    // __snprintf_chk(s, maxlen, flag, slen, fmt, ...) : slen < maxlen → __chk_fail.
    if (!strcmp(name, "__snprintf_chk") || !strcmp(name, "__vsnprintf_chk")) {
        uint32_t argv = !strcmp(name, "__vsnprintf_chk") ? argument(c, 5) : c->esp + 24;
        if (argument(c, 3) < argument(c, 1))
            rt_trap(c, c->eip, "*** buffer overflow detected ***: terminated (__snprintf_chk)");
        c->eax = (uint32_t)guest_format(c, argument(c, 0), argument(c, 1), argument(c, 4), argv, NULL,
                                        (int32_t)argument(c, 2));
        return 1;
    }
    // __sprintf_chk(s, flag, slen, fmt, ...) : slen == 0 ou sortie + NUL > slen → __chk_fail.
    if (!strcmp(name, "__sprintf_chk") || !strcmp(name, "__vsprintf_chk")) {
        uint32_t argv = !strcmp(name, "__vsprintf_chk") ? argument(c, 4) : c->esp + 20;
        uint32_t slen = argument(c, 2);
        if (!slen) rt_trap(c, c->eip, "*** buffer overflow detected ***: terminated (__sprintf_chk)");
        c->eax = (uint32_t)unbounded_format(c, argument(c, 0), argument(c, 3), argv,
                                            (int32_t)argument(c, 1), slen);
        return 1;
    }
    // __swprintf_chk(s, n, flag, slen, fmt, ...) : n > slen (en wchar_t) → __chk_fail.
    if (!strcmp(name, "__swprintf_chk") || !strcmp(name, "__vswprintf_chk")) {
        uint32_t argv = !strcmp(name, "__vswprintf_chk") ? argument(c, 5) : c->esp + 24;
        if (argument(c, 1) > argument(c, 3))
            rt_trap(c, c->eip, "*** buffer overflow detected ***: terminated (__swprintf_chk)");
        c->eax = (uint32_t)guest_swprintf(c, argument(c, 0), argument(c, 1), argument(c, 4), argv);
        return 1;
    }
    // __printf_chk(flag, fmt, ...), __vprintf_chk(flag, fmt, ap), __vfprintf_chk(fp, flag, fmt, ap).
    if (!strcmp(name, "__printf_chk") || !strcmp(name, "__vprintf_chk")) {
        uint32_t argv = !strcmp(name, "__vprintf_chk") ? argument(c, 2) : c->esp + 12;
        c->eax = (uint32_t)guest_format(c, 0, 0, argument(c, 1), argv, stdout, (int32_t)argument(c, 0));
        return 1;
    }
    if (!strcmp(name, "__vfprintf_chk")) {
        FILE *stream = rt_stream_file(c, argument(c, 0));
        c->eax = (uint32_t)guest_format(c, 0, 0, argument(c, 2), argument(c, 3), stream, (int32_t)argument(c, 1));
        return 1;
    }
    if (!strcmp(name, "__fprintf_chk")) {
        FILE *stream = rt_stream_file(c, argument(c, 0));
        c->eax = (uint32_t)guest_format(c, 0, 0, argument(c, 2), c->esp + 16, stream, (int32_t)argument(c, 1));
        return 1;
    }
    return 0;
}
