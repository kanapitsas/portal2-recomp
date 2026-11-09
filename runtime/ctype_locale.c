// ctype, locales et gettext glibc i386.
//
// Locales disponibles : "C" (alias "POSIX") et "C.UTF-8" (alias "C.utf8",
// et toute orthographe dont le jeu de caractères normalisé vaut "utf8"),
// comme sur un système glibc où aucune autre locale n'est installée. Toute
// autre locale demandée (y compris via setlocale(cat, "") et LANG/LC_*)
// échoue comme glibc : setlocale renvoie NULL, newlocale renvoie 0/ENOENT.
// Le jeu appelle setlocale(LC_CTYPE|LC_TIME|LC_COLLATE|LC_MONETARY, "")
// (engine.so, vguimatsurface.so) ; sans LANG/LC_* il reste donc en "C", et
// LC_ALL=C.UTF-8 donne au code invité un LC_CTYPE UTF-8.
//
// Tables __ctype_b/__ctype_tolower/__ctype_toupper : glibc les rend par thread
// (TLS). Les tables à un octet de "C" et "C.UTF-8" étant identiques (les
// octets >= 0x80 n'y sont jamais des caractères), un unique pointeur global
// stable est exact pour tous les threads et toutes les locales disponibles.
//
// SHIMS: __ctype_b_loc __ctype_tolower_loc __ctype_toupper_loc __ctype_get_mb_cur_max
// SHIMS: isalnum isalpha iscntrl isdigit isgraph islower isprint ispunct isspace isupper isxdigit isblank
// SHIMS: setlocale localeconv __newlocale __duplocale __freelocale __uselocale __nl_langinfo_l
// SHIMS: strcoll __strcoll_l __strxfrm_l __wctype_l __iswctype_l __towlower_l __towupper_l
// SHIMS: gettext dgettext dcgettext bindtextdomain bind_textdomain_codeset textdomain
#include "rt_locale.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <wctype.h>
#include <xlocale.h>


// --- Tables à un octet (bits glibc little-endian : _ISbit) -----------------
enum {
    IS_UPPER = 0x100, IS_LOWER = 0x200, IS_ALPHA = 0x400, IS_DIGIT = 0x800,
    IS_XDIGIT = 0x1000, IS_SPACE = 0x2000, IS_PRINT = 0x4000, IS_GRAPH = 0x8000,
    IS_BLANK = 0x1, IS_CNTRL = 0x2, IS_PUNCT = 0x4, IS_ALNUM = 0x8,
};
static uint16_t ascii_class(int ch) {
    if (ch < 0 || ch > 127) return 0;
    uint16_t m = 0;
    if (ch < 32 || ch == 127) m |= IS_CNTRL;
    if (ch == ' ' || ch == '\t') m |= IS_BLANK;
    if (ch == ' ' || (ch >= '\t' && ch <= '\r')) m |= IS_SPACE;
    if (ch >= 32 && ch < 127) m |= IS_PRINT;
    if (ch > 32 && ch < 127) m |= IS_GRAPH;
    if (ch >= 'A' && ch <= 'Z') m |= IS_UPPER | IS_ALPHA | IS_ALNUM;
    if (ch >= 'a' && ch <= 'z') m |= IS_LOWER | IS_ALPHA | IS_ALNUM;
    if (ch >= '0' && ch <= '9') m |= IS_DIGIT | IS_ALNUM | IS_XDIGIT;
    if ((ch >= 'A' && ch <= 'F') || (ch >= 'a' && ch <= 'f')) m |= IS_XDIGIT;
    if ((m & IS_GRAPH) && !(m & IS_ALNUM)) m |= IS_PUNCT;
    return m;
}
static int ascii_case(int ch, int upper) {
    if (upper && ch >= 'a' && ch <= 'z') return ch - 32;
    if (!upper && ch >= 'A' && ch <= 'Z') return ch + 32;
    return ch;
}
// Bloc invité : 3 pointeurs (b, tolower, toupper), puis les 3 tables de 384 entrées.
static uint32_t tables;
#define TABLE_POINTERS 0u
#define TABLE_B 16u
#define TABLE_LOWER (TABLE_B + 384u * 2u)
#define TABLE_UPPER (TABLE_LOWER + 384u * 4u)
#define TABLE_SIZE (TABLE_UPPER + 384u * 4u)
static uint32_t ctype_tables(CPU *c) {
    if (tables) return tables;
    uint32_t t = rt_alloc(TABLE_SIZE);
    if (!t) rt_trap(c, c->eip, "cannot allocate guest ctype tables");
    for (int i = -128; i < 256; ++i) {
        unsigned slot = (unsigned)(i + 128);
        W16(t + TABLE_B + 2 * slot, ascii_class(i));
        W32(t + TABLE_LOWER + 4 * slot, (uint32_t)ascii_case(i, 0));
        W32(t + TABLE_UPPER + 4 * slot, (uint32_t)ascii_case(i, 1));
    }
    W32(t + TABLE_POINTERS, t + TABLE_B + 256);
    W32(t + TABLE_POINTERS + 4, t + TABLE_LOWER + 512);
    W32(t + TABLE_POINTERS + 8, t + TABLE_UPPER + 512);
    return tables = t;
}

// --- Classes larges --------------------------------------------------------
static const char *const class_names[] = {
    NULL, "alnum", "alpha", "blank", "cntrl", "digit", "graph", "lower", "print", "punct", "space", "upper", "xdigit",
};
static const uint16_t class_bits[] = {
    0, IS_ALNUM, IS_ALPHA, IS_BLANK, IS_CNTRL, IS_DIGIT, IS_GRAPH, IS_LOWER, IS_PRINT, IS_PUNCT, IS_SPACE, IS_UPPER, IS_XDIGIT,
};
// Hors ASCII, en C.UTF-8 seulement : tables Unicode de la locale hôte
// C.UTF-8 de macOS. Elles peuvent différer à la marge de la version Unicode
// de glibc ; en "C", glibc ne classe aucun caractère hors ASCII.
static locale_t host_utf8(void) {
    static locale_t host;
    if (!host) host = newlocale(LC_CTYPE_MASK, "C.UTF-8", NULL);
    if (!host) abort();
    return host;
}
int rt_wclass(uint32_t wc, int cls, int utf8) {
    if (cls < 1 || cls > 12) return 0;
    if (wc < 128) return (ascii_class((int)wc) & class_bits[cls]) != 0;
    // glibc (i18n) : digit et xdigit ne contiennent que les chiffres ASCII.
    if (!utf8 || cls == RT_WC_DIGIT || cls == RT_WC_XDIGIT || wc > 0x10ffff || (wc >= 0xd800 && wc <= 0xdfff)) return 0;
    return iswctype_l((wint_t)wc, wctype_l(class_names[cls], host_utf8()), host_utf8()) != 0;
}
uint32_t rt_wcase(uint32_t wc, int upper, int utf8) {
    if (wc < 128) return (uint32_t)ascii_case((int)wc, upper);
    if (!utf8 || wc > 0x10ffff || (wc >= 0xd800 && wc <= 0xdfff)) return wc;
    return (uint32_t)(upper ? towupper_l((wint_t)wc, host_utf8()) : towlower_l((wint_t)wc, host_utf8()));
}

// --- Chaînes invitées stables ---------------------------------------------
// Étroites : table internée commune (rt_intern_string). Larges (wchar_t UTF-32,
// nl_langinfo) : liste propre, le texte ASCII étant élargi à 4 octets.
typedef struct Interned { char *text; uint32_t address; struct Interned *next; } Interned;
static Interned *interned_wide;
static uint32_t intern(CPU *c, const char *text, int wide) {
    if (!wide) return rt_intern_string(c, text);
    for (Interned *s = interned_wide; s; s = s->next)
        if (!strcmp(s->text, text)) return s->address;
    size_t length = strlen(text) + 1;
    Interned *s = calloc(1, sizeof(*s));
    if (!s || !(s->text = strdup(text))) rt_trap(c, c->eip, "cannot intern locale string");
    s->address = rt_alloc((uint32_t)(length * 4));
    if (!s->address) rt_trap(c, c->eip, "cannot allocate guest locale string");
    for (size_t i = 0; i < length; ++i) W32(s->address + 4 * (uint32_t)i, (uint8_t)text[i]);
    s->next = interned_wide; interned_wide = s;
    return s->address;
}

// --- Modèle de locale ------------------------------------------------------
#define CATEGORIES 13
#define LC_ALL_INDEX 6
#define ALL_MASK (((1u << CATEGORIES) - 1) & ~(1u << LC_ALL_INDEX))
static const char *const category_names[CATEGORIES] = {
    "LC_CTYPE", "LC_NUMERIC", "LC_TIME", "LC_COLLATE", "LC_MONETARY", "LC_MESSAGES", "LC_ALL",
    "LC_PAPER", "LC_NAME", "LC_ADDRESS", "LC_TELEPHONE", "LC_MEASUREMENT", "LC_IDENTIFICATION",
};
// Noms hôtes internés une fois (jamais libérés : pointeurs comparables).
static const char *name_copy(const char *name) {
    static char **names; static size_t count;
    for (size_t i = 0; i < count; ++i) if (!strcmp(names[i], name)) return names[i];
    char **grown = realloc(names, (count + 1) * sizeof(*grown));
    char *copy = strdup(name);
    if (!grown || !copy) abort();
    names = grown; names[count] = copy;
    return names[count++];
}
// Nom canonique d'une locale disponible, NULL si absente.
static const char *available(const char *name) {
    if (!strcmp(name, "C") || !strcmp(name, "POSIX")) return name_copy("C");
    if (name[0] != 'C' || name[1] != '.') return NULL;
    char normalized[16]; size_t n = 0;
    for (const char *p = name + 2; *p; ++p) {
        if (*p == '@' || n + 1 >= sizeof(normalized)) return NULL;
        if (isalnum((unsigned char)*p)) normalized[n++] = (char)tolower((unsigned char)*p);
    }
    normalized[n] = 0;
    return !strcmp(normalized, "utf8") ? name_copy(name) : NULL;
}
static int name_utf8(const char *name) { return strcmp(name, "C") != 0; }
// Valeur de setlocale(cat, "") : LC_ALL, puis LC_<cat>, puis LANG, sinon "C".
static const char *from_environment(int category) {
    const char *value = getenv("LC_ALL");
    if (!value || !*value) value = getenv(category_names[category]);
    if (!value || !*value) value = getenv("LANG");
    if (!value || !*value) value = "C";
    return value;
}

static const char *global_names[CATEGORIES];
typedef struct Locale { uint32_t address; const char *names[CATEGORIES]; struct Locale *next; } Locale;
static Locale *locales;
static _Thread_local uint32_t thread_locale;   // 0 : locale globale
static void ensure_global(void) {
    if (!global_names[0]) for (int i = 0; i < CATEGORIES; ++i) global_names[i] = name_copy("C");
}
static Locale *find_locale(uint32_t address) {
    for (Locale *l = locales; l; l = l->next) if (l->address == address) return l;
    return NULL;
}
static const char *const *names_of(CPU *c, uint32_t locale) {
    ensure_global();
    if (locale == RT_LC_GLOBAL_LOCALE) return global_names;
    Locale *l = find_locale(locale);
    if (!l) rt_trap(c, c->eip, "unknown guest locale_t");
    return l->names;
}
int rt_locale_object_utf8(CPU *c, uint32_t locale) { return name_utf8(names_of(c, locale)[0]); }
int rt_locale_ctype_utf8(void) {
    ensure_global();
    Locale *l = thread_locale ? find_locale(thread_locale) : NULL;
    return name_utf8(l ? l->names[0] : global_names[0]);
}
// Nom composite glibc ("LC_CTYPE=…;LC_NUMERIC=…;…") ou nom unique.
static void composite(const char *const names[CATEGORIES], char *out, size_t size) {
    int uniform = 1;
    for (int i = 1; i < CATEGORIES; ++i) if (i != LC_ALL_INDEX && names[i] != names[0]) uniform = 0;
    if (uniform) { snprintf(out, size, "%s", names[0]); return; }
    out[0] = 0;
    for (int i = 0; i < CATEGORIES; ++i) {
        if (i == LC_ALL_INDEX) continue;
        size_t used = strlen(out);
        snprintf(out + used, size - used, "%s%s=%s", used ? ";" : "", category_names[i], names[i]);
    }
}
// Résout `request` pour les catégories de `mask` dans `out` ; 0 si une
// locale est absente ou si le nom composite est invalide.
static int resolve(const char *request, uint32_t mask, const char *out[CATEGORIES]) {
    if (strchr(request, '=')) {
        char *copy = strdup(request), *save = NULL;
        if (!copy) return 0;
        int ok = 1;
        for (char *item = strtok_r(copy, ";", &save); item && ok; item = strtok_r(NULL, ";", &save)) {
            char *equals = strchr(item, '=');
            int category = -1;
            if (equals) { *equals = 0; for (int i = 0; i < CATEGORIES; ++i) if (i != LC_ALL_INDEX && !strcmp(item, category_names[i])) category = i; }
            const char *name = category >= 0 ? available(equals + 1) : NULL;
            if (!name) ok = 0;
            else if (mask & (1u << category)) out[category] = name;
        }
        free(copy);
        return ok;
    }
    for (int i = 0; i < CATEGORIES; ++i) {
        if (!(mask & (1u << i))) continue;
        const char *name = available(*request ? request : from_environment(i));
        if (!name) return 0;
        out[i] = name;
    }
    return 1;
}

// struct __locale_struct i386 : __locales[13], __ctype_b, __ctype_tolower,
// __ctype_toupper, __names[13] = 116 octets. libstdc++ lit directement les
// trois pointeurs de tables ; __locales (données internes glibc) reste nul.
#define LOCALE_SIZE 116u
static void publish(CPU *c, Locale *l) {
    uint32_t t = ctype_tables(c), a = l->address;
    for (unsigned i = 0; i < CATEGORIES; ++i) W32(a + 4 * i, 0);
    W32(a + 52, R32(t + TABLE_POINTERS)); W32(a + 56, R32(t + TABLE_POINTERS + 4)); W32(a + 60, R32(t + TABLE_POINTERS + 8));
    char all[512]; composite(l->names, all, sizeof(all));
    for (unsigned i = 0; i < CATEGORIES; ++i)
        W32(a + 64 + 4 * i, intern(c, i == LC_ALL_INDEX ? all : l->names[i], 0));
}

// --- nl_langinfo -----------------------------------------------------------
// rt_day_names/rt_month_names : tables partagées avec asctime/strftime (time.c).
// Élément LC_TIME texte (index 0..49 étroits, 52..100 larges correspondants).
static const char *time_item(unsigned index, char *scratch) {
    if (index <= 6) { snprintf(scratch, 4, "%s", rt_day_names[index]); return scratch; }
    if (index <= 13) return rt_day_names[index - 7];
    if (index <= 25) { snprintf(scratch, 4, "%s", rt_month_names[index - 14]); return scratch; }
    if (index <= 37) return rt_month_names[index - 26];
    switch (index) {
    case 38: return "AM"; case 39: return "PM";
    case 40: return "%a %b %e %H:%M:%S %Y"; case 41: return "%m/%d/%y";
    case 42: return "%H:%M:%S"; case 43: return "%I:%M:%S %p";
    case 44: case 45: case 46: case 47: case 48: case 49: return "";   // ERA*, ALT_DIGITS
    }
    return NULL;
}
static int langinfo(CPU *c, uint32_t item, int utf8, uint32_t *result) {
    unsigned category = item >> 16, index = item & 0xffff;
    char scratch[8];
    const char *text = NULL; int wide = 0;
    if (category == 0) {
        if (index == 13) { *result = utf8 ? 6 : 1; return 1; }                // _NL_CTYPE_MB_CUR_MAX
        if (index == 14) text = utf8 ? "UTF-8" : "ANSI_X3.4-1968";             // CODESET
    } else if (category == 1) {
        if (index == 0) text = ".";                                           // RADIXCHAR
        else if (index == 1 || index == 2) text = "";                         // THOUSEP, GROUPING
        else if (index == 3) { *result = '.'; return 1; }                     // _NL_NUMERIC_DECIMAL_POINT_WC
        else if (index == 4) { *result = 0; return 1; }                       // _NL_NUMERIC_THOUSANDS_SEP_WC
    } else if (category == 2) {
        if (index <= 49) text = time_item(index, scratch);
        else if (index == 50) { *result = 0; return 1; }                      // _NL_TIME_ERA_NUM_ENTRIES
        else if (index >= 52 && index <= 95) { text = time_item(index - 52, scratch); wide = 1; }
        else if (index >= 96 && index <= 100) { text = ""; wide = 1; }       // _NL_WERA_*, _NL_WALT_DIGITS
    } else if (category == 4) {
        if (index <= 6) text = "";                                            // symboles, séparateurs, signes
        else if (index <= 14 || (index >= 16 && index <= 21)) text = "\377";  // valeurs char = CHAR_MAX codé '\377'
        else if (index == 15) text = "-";                                     // _NL_MONETARY_CRNCYSTR
        else if (index == 43 || index == 44) { *result = 0; return 1; }       // *_WC
    }
    if (!text) return 0;
    *result = intern(c, text, wide);
    return 1;
}

// --- gettext sans catalogue ------------------------------------------------
typedef struct Domain { char *name; uint32_t directory, codeset; struct Domain *next; } Domain;
static Domain *domains;
static uint32_t current_domain;
static Domain *domain(CPU *c, const char *name) {
    for (Domain *d = domains; d; d = d->next) if (!strcmp(d->name, name)) return d;
    Domain *d = calloc(1, sizeof(*d));
    if (!d || !(d->name = strdup(name))) rt_trap(c, c->eip, "cannot allocate gettext domain");
    d->next = domains; domains = d;
    return d;
}
static uint32_t guest_copy(CPU *c, uint32_t source) { return rt_guest_strdup_or_trap(c, rt_guest_cstr(c, source)); }

static int classify_shim(CPU *c, const char *name) {
    static const char *const names[] = {"isalnum", "isalpha", "isblank", "iscntrl", "isdigit", "isgraph",
                                        "islower", "isprint", "ispunct", "isspace", "isupper", "isxdigit"};
    static const uint16_t bits[] = {IS_ALNUM, IS_ALPHA, IS_BLANK, IS_CNTRL, IS_DIGIT, IS_GRAPH,
                                    IS_LOWER, IS_PRINT, IS_PUNCT, IS_SPACE, IS_UPPER, IS_XDIGIT};
    for (unsigned i = 0; i < sizeof(names) / sizeof(*names); ++i) {
        if (strcmp(name, names[i])) continue;
        int32_t value = (int32_t)rt_arg32(c, 0);
        // Hors [-128, 255], glibc indexe hors de sa table : comportement indéfini.
        if (value < -128 || value > 255) rt_trap(c, c->eip, "ctype argument out of range");
        c->eax = ascii_class(value) & bits[i];
        return 1;
    }
    return 0;
}

static int locale_shim(CPU *c, const char *name) {
    if (!strcmp(name, "__ctype_b_loc")) c->eax = ctype_tables(c) + TABLE_POINTERS;
    else if (!strcmp(name, "__ctype_tolower_loc")) c->eax = ctype_tables(c) + TABLE_POINTERS + 4;
    else if (!strcmp(name, "__ctype_toupper_loc")) c->eax = ctype_tables(c) + TABLE_POINTERS + 8;
    else if (!strcmp(name, "__ctype_get_mb_cur_max")) c->eax = rt_locale_ctype_utf8() ? 6 : 1;
    else if (!strcmp(name, "setlocale")) {
        int category = (int32_t)rt_arg32(c, 0); uint32_t request = rt_arg32(c, 1);
        ensure_global();
        if (category < 0 || category >= CATEGORIES) { rt_set_errno(c, RT_EINVAL); c->eax = 0; return 1; }
        if (request) {
            const char *wanted[CATEGORIES]; memcpy(wanted, global_names, sizeof(wanted));
            uint32_t mask = category == LC_ALL_INDEX ? ALL_MASK : 1u << category;
            if (category != LC_ALL_INDEX && strchr(rt_guest_cstr(c, request), '=')) { rt_set_errno(c, RT_EINVAL); c->eax = 0; return 1; }
            if (!resolve(rt_guest_cstr(c, request), mask, wanted)) { rt_set_errno(c, RT_ENOENT); c->eax = 0; return 1; }
            memcpy(global_names, wanted, sizeof(wanted));
        }
        char all[512];
        if (category == LC_ALL_INDEX) composite(global_names, all, sizeof(all));
        else snprintf(all, sizeof(all), "%s", global_names[category]);
        c->eax = intern(c, all, 0);
    } else if (!strcmp(name, "localeconv")) {
        // struct lconv i386 : 10 pointeurs puis 14 char ; locale C : "." et "",
        // champs numériques CHAR_MAX (127).
        static uint32_t lconv;
        if (!lconv && !(lconv = rt_alloc(56))) rt_trap(c, c->eip, "cannot allocate guest lconv");
        memset(GPTR(lconv), 0, 56);
        W32(lconv, intern(c, ".", 0));
        for (unsigned i = 1; i < 10; ++i) W32(lconv + 4 * i, intern(c, "", 0));
        for (unsigned i = 40; i < 54; ++i) W8(lconv + i, 127);
        c->eax = lconv;
    } else if (!strcmp(name, "__newlocale")) {
        uint32_t mask = rt_arg32(c, 0), request = rt_arg32(c, 1), base = rt_arg32(c, 2);
        if (mask == 1u << LC_ALL_INDEX) mask = ALL_MASK;
        if ((mask & ~ALL_MASK) || !request) { rt_set_errno(c, RT_EINVAL); c->eax = 0; return 1; }
        Locale *b = base ? find_locale(base) : NULL;
        if (base && !b) rt_trap(c, c->eip, "newlocale on unknown base locale_t");
        const char *wanted[CATEGORIES];
        for (int i = 0; i < CATEGORIES; ++i) wanted[i] = b ? b->names[i] : name_copy("C");
        if (!resolve(rt_guest_cstr(c, request), mask, wanted)) { rt_set_errno(c, RT_ENOENT); c->eax = 0; return 1; }
        if (!b) {
            b = calloc(1, sizeof(*b));
            if (!b || !(b->address = rt_alloc(LOCALE_SIZE))) { free(b); rt_set_errno(c, RT_ENOMEM); c->eax = 0; return 1; }
            b->next = locales; locales = b;
        }
        memcpy(b->names, wanted, sizeof(wanted));
        publish(c, b); c->eax = b->address;
    } else if (!strcmp(name, "__duplocale")) {
        const char *const *source = names_of(c, rt_arg32(c, 0));
        Locale *l = calloc(1, sizeof(*l));
        if (!l || !(l->address = rt_alloc(LOCALE_SIZE))) { free(l); rt_set_errno(c, RT_ENOMEM); c->eax = 0; return 1; }
        memcpy(l->names, source, sizeof(l->names));
        l->next = locales; locales = l;
        publish(c, l); c->eax = l->address;
    } else if (!strcmp(name, "__freelocale")) {
        uint32_t address = rt_arg32(c, 0);
        Locale **link = &locales;
        while (*link && (*link)->address != address) link = &(*link)->next;
        if (!*link || address == thread_locale) rt_trap(c, c->eip, "freelocale on unknown or active locale_t");
        Locale *l = *link; *link = l->next;
        rt_free(l->address); free(l);
    } else if (!strcmp(name, "__uselocale")) {
        uint32_t next = rt_arg32(c, 0);
        c->eax = thread_locale ? thread_locale : RT_LC_GLOBAL_LOCALE;
        if (next == RT_LC_GLOBAL_LOCALE) thread_locale = 0;
        else if (next) { if (!find_locale(next)) rt_trap(c, c->eip, "uselocale on unknown locale_t"); thread_locale = next; }
    } else if (!strcmp(name, "__nl_langinfo_l")) {
        uint32_t item = rt_arg32(c, 0);
        if (!langinfo(c, item, rt_locale_object_utf8(c, rt_arg32(c, 1)), &c->eax)) {
            fprintf(stderr, "unimplemented nl_langinfo item %#x\n", item);
            rt_trap(c, c->eip, "unimplemented nl_langinfo item");
        }
    } else if (!strcmp(name, "strcoll") || !strcmp(name, "__strcoll_l")) {
        // C et C.UTF-8 : ordre des octets (= ordre des points de code en UTF-8).
        if (name[0] == '_') names_of(c, rt_arg32(c, 2));
        int r = strcmp(rt_guest_cstr(c, rt_arg32(c, 0)), rt_guest_cstr(c, rt_arg32(c, 1)));
        c->eax = (uint32_t)(r < 0 ? -1 : r > 0);
    } else if (!strcmp(name, "__strxfrm_l")) {
        uint32_t dest = rt_arg32(c, 0), source = rt_arg32(c, 1), n = rt_arg32(c, 2);
        names_of(c, rt_arg32(c, 3));
        uint32_t length = rt_string_length(c, source);
        if (n) {
            uint32_t count = length + 1 < n ? length + 1 : n;
            memmove(rt_write_span(c, dest, count), GPTR(source), count);
        }
        c->eax = length;
    } else if (!strcmp(name, "__wctype_l")) {
        const char *property = rt_guest_cstr(c, rt_arg32(c, 0));
        names_of(c, rt_arg32(c, 1));
        c->eax = 0;   // descripteur opaque : 1..12, 0 = propriété inconnue
        for (uint32_t i = 1; i <= 12; ++i) if (!strcmp(property, class_names[i])) c->eax = i;
    } else if (!strcmp(name, "__iswctype_l")) {
        uint32_t wc = rt_arg32(c, 0), descriptor = rt_arg32(c, 1);
        int utf8 = rt_locale_object_utf8(c, rt_arg32(c, 2));
        if (descriptor > 12) rt_trap(c, c->eip, "invalid wctype_t descriptor");
        c->eax = descriptor ? (uint32_t)rt_wclass(wc, (int)descriptor, utf8) : 0;
    } else if (!strcmp(name, "__towlower_l") || !strcmp(name, "__towupper_l")) {
        c->eax = rt_wcase(rt_arg32(c, 0), name[5] == 'u', rt_locale_object_utf8(c, rt_arg32(c, 1)));
    } else if (!strcmp(name, "gettext") || !strcmp(name, "dgettext") || !strcmp(name, "dcgettext")) {
        // Aucun catalogue : glibc renvoie le msgid (et préserve errno).
        uint32_t msgid = rt_arg32(c, name[0] == 'g' ? 0 : 1);
        if (msgid) rt_string_length(c, msgid);
        c->eax = msgid;
    } else if (!strcmp(name, "bindtextdomain") || !strcmp(name, "bind_textdomain_codeset")) {
        uint32_t domain_name = rt_arg32(c, 0), value = rt_arg32(c, 1);
        if (!domain_name || !*rt_guest_cstr(c, domain_name)) { c->eax = 0; return 1; }
        Domain *d = domain(c, rt_guest_cstr(c, domain_name));
        int directory = name[4] == 't';
        uint32_t *slot = directory ? &d->directory : &d->codeset;
        if (value) { if (*slot) rt_free(*slot); *slot = guest_copy(c, value); }
        c->eax = *slot;
        if (!c->eax && directory) c->eax = intern(c, "/usr/share/locale", 0);
    } else if (!strcmp(name, "textdomain")) {
        uint32_t value = rt_arg32(c, 0);
        if (!current_domain) current_domain = intern(c, "messages", 0);
        if (value) current_domain = *rt_guest_cstr(c, value) ? guest_copy(c, value) : intern(c, "messages", 0);
        c->eax = current_domain;
    } else return classify_shim(c, name);
    return 1;
}
RT_SHIM_PROVIDER(locale_shim)
