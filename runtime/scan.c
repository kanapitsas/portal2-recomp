// Famille scanf de la glibc (version ISO C99) : un seul moteur pour les
// entrées étroites (sscanf, fscanf sur un flux invité) et larges (swscanf,
// wchar_t Linux = 4 octets UTF-32). Il suit l'algorithme de vfscanf de la
// glibc : lecture caractère par caractère, un seul caractère remis (ungetc)
// après chaque champ, conversion des nombres par strto* sur le texte accepté,
// EOF seulement si l'entrée manque avant la première affectation. Locale « C » :
// passer d'une entrée large à une destination étroite (ou l'inverse) n'accepte
// que l'ASCII, sinon EILSEQ comme wcrtomb/mbrtowc de la glibc.
// Non géré (rt_trap) : arguments positionnels, drapeaux ' I m, long double.
// SHIMS: __isoc99_fscanf __isoc99_swscanf fscanf swscanf
#include "rt_locale.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

enum { SOURCE_NARROW, SOURCE_WIDE, SOURCE_FILE };
enum { NOT_READ = -2 };       // aucun caractère lu (largeur épuisée) : rien à remettre

typedef struct {
    CPU *c;
    int kind;
    uint32_t address;  // prochain caractère d'une chaîne invitée
    FILE *file;
    uint64_t consumed; // caractères consommés, pour %n
} Input;

static int32_t input_get(Input *in) {
    int32_t ch;
    if (in->kind == SOURCE_NARROW) {
        ch = R8(in->address);
        if (!ch) return EOF;
        ++in->address;
    } else if (in->kind == SOURCE_WIDE) {
        ch = (int32_t)R32(in->address);
        if (!ch) return EOF;
        in->address += 4;
    } else {
        ch = fgetc(in->file);
        if (ch == EOF) { if (ferror(in->file)) rt_save_errno(in->c); return EOF; }
    }
    ++in->consumed;
    return ch;
}
static void input_unget(Input *in, int32_t ch) {
    if (ch < 0) return;
    --in->consumed;
    if (in->kind == SOURCE_NARROW) --in->address;
    else if (in->kind == SOURCE_WIDE) in->address -= 4;
    else if (ungetc(ch, in->file) == EOF) rt_trap(in->c, in->c->eip, "fscanf ungetc failed");
}
// Caractère suivant si la largeur du champ le permet.
static int32_t input_more(Input *in, int width) { return width != 0 ? input_get(in) : NOT_READ; }
static void use_width(int *width) { if (*width > 0) --*width; }
static int is_space(int32_t ch) { return ch == ' ' || (ch >= '\t' && ch <= '\r'); }
// Blancs de l'entrée : isspace (table ASCII) ou iswspace de la locale.
static int input_space(const Input *in, int32_t ch) {
    if (in->kind != SOURCE_WIDE || ch < 0x80) return is_space(ch);
    return rt_wclass((uint32_t)ch, RT_WC_SPACE, rt_locale_ctype_utf8());
}
static int lower(int32_t ch) { return ch >= 'A' && ch <= 'Z' ? ch + 32 : ch; }
static int is_digit(int32_t ch, int base) {
    if (ch >= '0' && ch <= '9') return ch - '0' < base;
    return base == 16 && lower(ch) >= 'a' && lower(ch) <= 'f';
}

// Texte accepté pour un nombre, converti ensuite par strto*.
typedef struct { char text[512]; size_t length; } Number;
static void number_add(CPU *c, Number *n, int32_t ch) {
    if (n->length + 1 >= sizeof(n->text)) rt_trap(c, c->eip, "scanf numeric field too long");
    n->text[n->length++] = (char)ch; n->text[n->length] = 0;
}

typedef struct {   // directive %[...] : intervalles de codes, éventuellement niés
    int negate;
    size_t count;
    uint32_t low[128], high[128];
} Scanset;
static int scanset_has(const Scanset *set, int32_t ch) {
    for (size_t i = 0; i < set->count; ++i)
        if ((uint32_t)ch >= set->low[i] && (uint32_t)ch <= set->high[i]) return !set->negate;
    return set->negate;
}

typedef struct {
    CPU *c;
    uint32_t address;   // destination invitée
    int wide;           // destination wchar_t
    int input_wide;
    uint32_t written;
} Store;
// Range un caractère ; 0 si l'encodage échoue (EILSEQ). Entrée large vers
// destination étroite : wcrtomb de la locale LC_CTYPE (ASCII en "C", UTF-8 en
// "C.UTF-8"). Entrée étroite vers destination large : mbrtowc, ASCII en "C" ;
// le décodage UTF-8 octet par octet n'est pas implémenté (aucun appelant).
static int store_char(Store *s, int32_t ch) {
    if (s->wide == s->input_wide) {
        if (s->wide) W32(s->address + 4 * s->written, (uint32_t)ch);
        else W8(s->address + s->written, (uint32_t)ch);
        ++s->written;
    } else if (s->wide) {
        if ((uint32_t)ch > 0x7f) {
            if (rt_locale_ctype_utf8()) rt_trap(s->c, s->c->eip, "scanf UTF-8 to wide string is not supported");
            return 0;
        }
        W32(s->address + 4 * s->written++, (uint32_t)ch);
    } else {
        uint8_t bytes[4];
        uint32_t length = rt_wc_encode(rt_locale_ctype_utf8(), (uint32_t)ch, bytes);
        if (length > 4) return 0;
        for (uint32_t i = 0; i < length; ++i) W8(s->address + s->written++, bytes[i]);
    }
    return 1;
}
static void store_end(Store *s) {
    if (s->wide) W32(s->address + 4 * s->written, 0);
    else W8(s->address + s->written, 0);
}
void rt_store_integer(CPU *c, uint32_t address, unsigned size, uint64_t value) {
    rt_span(c, address, size);
    if (size == 1) W8(address, (uint32_t)value);
    else if (size == 2) W16(address, (uint32_t)value);
    else if (size == 4) W32(address, (uint32_t)value);
    else W64(address, value);
}

// strtol/strtoul de la glibc i386 (long = 32 bits) et strtoll/strtoull.
static uint64_t convert_integer(CPU *c, const char *text, int base, int is_signed, int wide64) {
    errno = 0;
    if (is_signed) {
        long long value = strtoll(text, NULL, base);
        if (!wide64 && value > INT32_MAX) { value = INT32_MAX; errno = ERANGE; }
        if (!wide64 && value < INT32_MIN) { value = INT32_MIN; errno = ERANGE; }
        if (errno) rt_set_errno(c, RT_ERANGE);
        return (uint64_t)value;
    }
    unsigned long long value = strtoull(text, NULL, base);
    if (!wide64) {
        const char *digits = text + (*text == '-' || *text == '+');
        unsigned long long magnitude = strtoull(digits, NULL, base);
        if (errno == ERANGE || magnitude > UINT32_MAX) { value = UINT32_MAX; errno = ERANGE; }
        else value = (uint32_t)value;
    }
    if (errno) rt_set_errno(c, RT_ERANGE);
    return value;
}

// Lit le texte d'un flottant (décimal, hexadécimal, inf, nan) comme la glibc.
// Renvoie 0 si la séquence ne peut pas être un nombre (échec de conversion).
static int scan_float_text(Input *in, int width, Number *n, int32_t *last) {
    CPU *c = in->c;
    int32_t ch = input_get(in);  // la largeur vaut au moins 1 ici
    if (ch == '-' || ch == '+') { number_add(c, n, ch); use_width(&width); ch = input_more(in, width); }
    if (ch >= 0 && (lower(ch) == 'n' || lower(ch) == 'i')) {
        const char *word = lower(ch) == 'n' ? "nan" : "inf";
        for (size_t i = 0; word[i]; ++i) {
            if (i) ch = input_more(in, width);
            if (ch < 0 || lower(ch) != word[i]) { *last = ch; return 0; }
            number_add(c, n, ch); use_width(&width);
        }
        ch = input_more(in, width);
        if (word[0] == 'i' && ch >= 0 && lower(ch) == 'i') {  // « infinity » : tout ou échec
            static const char rest[] = "inity";
            for (size_t i = 0; rest[i]; ++i) {
                if (i) ch = input_more(in, width);
                if (ch < 0 || lower(ch) != rest[i]) { *last = ch; return 0; }
                number_add(c, n, ch); use_width(&width);
            }
            ch = NOT_READ;
        }
        *last = ch;
        return 1;
    }
    int hexadecimal = 0, got_digit = 0, got_dot = 0, got_exponent = 0;
    if (ch == '0') {
        number_add(c, n, ch); use_width(&width); got_digit = 1;
        ch = input_more(in, width);
        if (ch >= 0 && lower(ch) == 'x') {
            hexadecimal = 1; number_add(c, n, ch); use_width(&width);
            ch = input_more(in, width);
        }
    }
    char exponent = hexadecimal ? 'p' : 'e';
    while (ch >= 0) {
        if (is_digit(ch, 10) || (hexadecimal && !got_exponent && is_digit(ch, 16))) got_digit = 1;
        else if (got_exponent && lower(n->text[n->length - 1]) == exponent && (ch == '-' || ch == '+')) {}
        else if (got_digit && !got_exponent && lower(ch) == exponent) got_exponent = got_dot = 1;
        else if (!got_dot && ch == '.') got_dot = 1;
        else break;
        number_add(c, n, ch); use_width(&width);
        ch = input_more(in, width);
    }
    *last = ch;
    return 1;
}

static uint32_t next_argument(CPU *c, uint32_t *arguments) {
    (void)c;
    uint32_t value = R32(*arguments); *arguments += 4; return value;
}

// Moteur commun. format : codes déjà lus (octets ou wchar_t). Renvoie le
// nombre d'affectations, ou EOF si l'entrée manque avant la première.
static int scan(Input *in, const uint32_t *f, uint32_t arguments) {
    CPU *c = in->c;
    int done = 0, skip_space = 0, input_wide = in->kind == SOURCE_WIDE;
    int32_t ch;
#define INPUT_ERROR() return done ? done : EOF
#define CONVERSION_ERROR() return done
#define ENCODING_ERROR() do { rt_set_errno(c, RT_EILSEQ); return done; } while (0)
    while (*f) {
        uint32_t fc = *f++;
        if (fc != '%') {
            if (is_space((int32_t)fc)) { skip_space = 1; continue; }
            ch = input_get(in);
            if (ch == EOF) INPUT_ERROR();
            if (skip_space) {
                while (input_space(in, ch)) if ((ch = input_get(in)) == EOF) INPUT_ERROR();
                skip_space = 0;
            }
            if ((uint32_t)ch != fc) { input_unget(in, ch); CONVERSION_ERROR(); }
            continue;
        }
        int suppress = 0, width = 0, digits = 0;
        if (*f == '*') { suppress = 1; ++f; }
        while (*f >= '0' && *f <= '9') {
            if (width > 100000000) rt_trap(c, c->eip, "scanf field width overflow");
            width = width * 10 + (int)(*f++ - '0'); digits = 1;
        }
        if (digits && *f == '$') rt_trap(c, c->eip, "positional scanf arguments are not supported");
        if (*f == '\'' || *f == 'I' || *f == 'm')
            rt_trap(c, c->eip, "unsupported scanf flag");
        if (!width) width = -1;
        unsigned size = 4; int is_long = 0, long_double = 0;
        switch (*f) {
        case 'h': ++f; size = 2; if (*f == 'h') { ++f; size = 1; } break;
        case 'l': ++f; is_long = 1; if (*f == 'l') { ++f; size = 8; is_long = 0; } break;
        case 'q': case 'L': case 'j': ++f; size = 8; long_double = 1; break;
        case 'z': case 't': ++f; break;
        }
        uint32_t conversion = *f++;
        if (!conversion) rt_trap(c, c->eip, "incomplete guest scanf conversion");
        if (conversion == 'S' || conversion == 'C') { conversion = lower((int32_t)conversion); is_long = 1; }
        if (skip_space || (conversion != '[' && conversion != 'c' && conversion != 'n')) {
            do ch = input_get(in); while (input_space(in, ch));
            input_unget(in, ch);
            skip_space = 0;
        }
        switch (conversion) {
        case '%':
            ch = input_get(in);
            if (ch == EOF) INPUT_ERROR();
            if (ch != '%') { input_unget(in, ch); CONVERSION_ERROR(); }
            break;
        case 'n':
            if (!suppress) rt_store_integer(c, next_argument(c, &arguments), size, in->consumed);
            break;
        case 'd': case 'i': case 'u': case 'o': case 'x': case 'X': {
            int base = conversion == 'd' || conversion == 'u' ? 10 : conversion == 'i' ? 0 :
                       conversion == 'o' ? 8 : 16;
            Number n = {.length = 0};
            ch = input_get(in);
            if (ch == EOF) INPUT_ERROR();
            if (ch == '-' || ch == '+') { number_add(c, &n, ch); use_width(&width); ch = input_more(in, width); }
            if (ch == '0') {
                use_width(&width); number_add(c, &n, ch);
                ch = input_more(in, width);
                if (ch >= 0 && lower(ch) == 'x') {
                    if (!base) base = 16;
                    if (base == 16) { use_width(&width); ch = input_more(in, width); }
                } else if (!base) base = 8;
            }
            if (!base) base = 10;
            while (ch >= 0 && is_digit(ch, base)) {
                number_add(c, &n, ch); use_width(&width);
                ch = input_more(in, width);
            }
            input_unget(in, ch);
            if (!n.length || (n.length == 1 && (n.text[0] == '-' || n.text[0] == '+'))) CONVERSION_ERROR();
            if (!suppress) {
                int is_signed = conversion == 'd' || conversion == 'i';
                rt_store_integer(c, next_argument(c, &arguments), size,
                              convert_integer(c, n.text, base, is_signed, size == 8));
                ++done;
            }
            break;
        }
        case 'a': case 'A': case 'e': case 'E': case 'f': case 'F': case 'g': case 'G': {
            if (long_double || size != 4) rt_trap(c, c->eip, "unsupported scanf floating length");
            Number n = {.length = 0};
            if ((ch = input_get(in)) == EOF) INPUT_ERROR();
            input_unget(in, ch);
            int32_t last;
            int ok = scan_float_text(in, width, &n, &last);
            input_unget(in, last);
            if (!ok) CONVERSION_ERROR();
            char *end; errno = 0;
            double value = is_long ? strtod(n.text, &end) : (double)strtof(n.text, &end);
            if (end == n.text) CONVERSION_ERROR();
            if (errno == ERANGE) rt_set_errno(c, RT_ERANGE);
            if (!suppress) {
                uint32_t address = next_argument(c, &arguments);
                if (is_long) { rt_span(c, address, 8); WD(address, value); }
                else { rt_span(c, address, 4); WF(address, (float)value); }
                ++done;
            }
            break;
        }
        case 's': case 'c': case '[': {
            if (size != 4) rt_trap(c, c->eip, "unsupported scanf string length");
            Scanset set = {0};
            if (conversion == '[') {
                if (*f == '^') { set.negate = 1; ++f; }
                const uint32_t *first = f;
                while (*f && (*f != ']' || f == first)) {
                    if (set.count == sizeof(set.low) / sizeof(*set.low)) rt_trap(c, c->eip, "scanf scanset overflow");
                    uint32_t low = *f++, high = low;
                    if (*f == '-' && f[1] && f[1] != ']' && f[1] >= low) { high = f[1]; f += 2; }
                    set.low[set.count] = low; set.high[set.count++] = high;
                }
                if (*f != ']') rt_trap(c, c->eip, "unterminated scanf scanset");
                ++f;
            }
            Store out = {c, suppress ? 0 : next_argument(c, &arguments), is_long, input_wide, 0};
            if ((ch = input_get(in)) == EOF) INPUT_ERROR();
            if (conversion == 'c') {
                if (width < 0) width = 1;
                for (;;) {
                    if (!suppress && !store_char(&out, ch)) ENCODING_ERROR();
                    if (--width <= 0 || (ch = input_get(in)) == EOF) break;
                }
            } else {
                uint32_t matched = 0;
                for (;;) {
                    if (conversion == 's' ? input_space(in, ch) : !scanset_has(&set, ch)) { input_unget(in, ch); break; }
                    if (!suppress && !store_char(&out, ch)) ENCODING_ERROR();
                    ++matched;
                    if ((width > 0 && --width == 0) || (ch = input_get(in)) == EOF) break;
                }
                if (!matched) CONVERSION_ERROR();
                if (!suppress) store_end(&out);
            }
            if (!suppress) ++done;
            break;
        }
        default:
            rt_trap(c, c->eip, "unsupported guest scanf conversion");
        }
    }
    if (skip_space) {
        do ch = input_get(in); while (input_space(in, ch));
        input_unget(in, ch);
    }
    return done;
#undef INPUT_ERROR
#undef CONVERSION_ERROR
#undef ENCODING_ERROR
}

// Format invité recopié en codes hôtes (octets non signés ou wchar_t).
static uint32_t *read_format(CPU *c, uint32_t address, int wide) {
    size_t length = 0;
    if (wide) while (R32(address + 4 * (uint32_t)length)) ++length;
    else length = rt_string_length(c, address);
    uint32_t *format = calloc(length + 2, sizeof(*format));  // + garde après un '%' final
    if (!format) rt_trap(c, c->eip, "cannot allocate scanf format");
    for (size_t i = 0; i < length; ++i)
        format[i] = wide ? R32(address + 4 * (uint32_t)i) : R8(address + (uint32_t)i);
    return format;
}
static int run(Input *in, uint32_t format_address, int wide_format, uint32_t arguments) {
    uint32_t *format = read_format(in->c, format_address, wide_format);
    int result = scan(in, format, arguments);
    free(format);
    return result;
}

int rt_scan_shim(CPU *c, const char *name) {
    int kind;
    if (!strcmp(name, "__isoc99_sscanf") || !strcmp(name, "sscanf")) kind = SOURCE_NARROW;
    // fscanf/swscanf sans __isoc99_ : même syntaxe sauf le %a GNU (allocation), absent des
    // formats du jeu ; un tel format serait mal lu, d'où le piège ci-dessous.
    else if (!strcmp(name, "__isoc99_swscanf") || !strcmp(name, "swscanf")) kind = SOURCE_WIDE;
    else if (!strcmp(name, "__isoc99_fscanf") || !strcmp(name, "fscanf")) kind = SOURCE_FILE;
    else return 0;
    uint32_t first = R32(c->esp + 4), format = R32(c->esp + 8), arguments = c->esp + 12;
    if (strncmp(name, "__isoc99_", 9) && kind != SOURCE_NARROW) {
        int wide_format = kind == SOURCE_WIDE;
        for (uint32_t at = format;; at += wide_format ? 4 : 1) {
            uint32_t ch = wide_format ? R32(at) : R8(at);
            if (!ch) break;
            uint32_t next = wide_format ? R32(at + 4) : R8(at + 1);
            if (ch == '%' && (next == 'a')) rt_trap(c, c->eip, "GNU %a scanf allocation is not implemented");
        }
    }
    Input in = {.c = c, .kind = kind, .address = first};
    if (kind == SOURCE_FILE) in.file = rt_stream_file(c, first);
    int wide = kind == SOURCE_WIDE;
    c->eax = (uint32_t)run(&in, format, wide, arguments);
    return 1;
}
