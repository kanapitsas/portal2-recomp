// Conversions numériques, qsort et rand de la glibc i386.
// `long` invité = 32 bits, `long long` = 64 bits (retour edx:eax) ; les
// flottants reviennent dans ST(0). Les variantes `_l` reçoivent un locale_t
// invité opaque, ignoré sans vérification (les tests passent des valeurs
// quelconques) : les seules locales que fournit ctype_locale.c, « C » et
// « C.UTF-8 », ont le même LC_NUMERIC (séparateur décimal « . », aucun groupement).
// SHIMS: strtoll strtoull strtold_l __strtod_internal __strtof_internal __strtol_internal
// SHIMS: __strtoll_internal __strtoul_internal __strtoull_internal __strtod_l __strtof_l
// SHIMS: qsort rand srand
#include "rt.h"
#include "modules.h"
#include <ctype.h>
#include <errno.h>
#include <float.h>
#include <limits.h>
#include <pthread.h>
#include <stdlib.h>

static uint32_t argument(CPU *c, unsigned i) { return R32(c->esp + 4 + 4 * i); }
static const char *guest_string(CPU *c, uint32_t address) { rt_string_length(c, address); return GPTR(address); }
static void store_end(uint32_t endptr, uint32_t start, const char *source, const char *end) {
    if (endptr) W32(endptr, start + (uint32_t)(end - source));
}
static int valid_base(int base) { return base == 0 || (base >= 2 && base <= 36); }
static int negative_sign(const char *s) {
    while (isspace((unsigned char)*s)) ++s;
    return *s == '-';
}

// strtol/strtoul/strtoll/strtoull : `bits` = 32 ou 64.
static void integer(CPU *c, int is_signed, int bits) {
    uint32_t start = argument(c, 0), endptr = argument(c, 1);
    int base = (int32_t)argument(c, 2);
    const char *source = guest_string(c, start);
    if (endptr) rt_write_span(c, endptr, 4);
    // glibc : base invalide → EINVAL, 0, *endptr non modifié.
    if (!valid_base(base)) { rt_set_errno(c, EINVAL); c->eax = c->edx = 0; return; }
    char *end; errno = 0;
    uint64_t result;
    int range = 0;
    if (is_signed) {
        long long value = strtoll(source, &end, base);
        range = errno == ERANGE;
        if (bits == 32 && value > INT32_MAX) { value = INT32_MAX; range = 1; }
        if (bits == 32 && value < INT32_MIN) { value = INT32_MIN; range = 1; }
        result = (uint64_t)value;
    } else {
        unsigned long long value = strtoull(source, &end, base);
        range = errno == ERANGE;
        if (bits == 32 && !range) {
            // Comme glibc : la magnitude doit tenir sur 32 bits, puis négation modulo 2^32.
            unsigned long long magnitude = negative_sign(source) ? 0ull - value : value;
            if (magnitude > UINT32_MAX) range = 1;
        }
        result = range ? (bits == 32 ? UINT32_MAX : UINT64_MAX) : value;
    }
    store_end(endptr, start, source, end);
    if (range) rt_set_errno(c, ERANGE);
    c->eax = (uint32_t)result;
    c->edx = bits == 64 ? (uint32_t)(result >> 32) : c->edx;
}

// strtod/strtof/strtold : résultat dans ST(0).
static void floating(CPU *c, int kind /* 0 float, 1 double, 2 long double */) {
    uint32_t start = argument(c, 0), endptr = argument(c, 1);
    const char *source = guest_string(c, start);
    if (endptr) rt_write_span(c, endptr, 4);
    char *end; errno = 0;
    double value;
    if (kind == 0) value = strtof(source, &end);
    else value = strtod(source, &end);
    int range = errno == ERANGE;
    if (kind == 2 && range) {
        // Le long double x87 (80 bits) couvre ~1e±4932 : un dépassement du double
        // n'en est pas un sous Linux. ST est modélisé en double : refuser plutôt
        // que renvoyer inf/0 et ERANGE à tort.
        rt_trap(c, c->eip, "strtold: value outside double range (x87 long double not modeled)");
    }
    store_end(endptr, start, source, end);
    if (range) rt_set_errno(c, ERANGE);
    rt_return_st0(c, value);
}

// glibc msort_with_tmp : tri fusion descendant, stable. Le comparateur ne voit
// que des adresses du tableau d'origine (le tampon temporaire reste côté hôte),
// donc la suite d'appels est identique à celle de glibc.
typedef struct { CPU *c; uint32_t compare; uint32_t size; uint8_t *tmp; } Sort;
static int compare(Sort *s, uint32_t a, uint32_t b) {
    uint32_t args[2] = {a, b};
    return (int32_t)rt_invoke(s->c, s->compare, args, 2);
}
static void msort(Sort *s, uint32_t b, uint32_t n) {
    if (n <= 1) return;
    uint32_t n1 = n / 2, n2 = n - n1, size = s->size;
    uint32_t b1 = b, b2 = b + n1 * size;
    msort(s, b1, n1);
    msort(s, b2, n2);
    uint8_t *tmp = s->tmp;
    while (n1 > 0 && n2 > 0) {
        if (compare(s, b1, b2) <= 0) { memcpy(tmp, GPTR(b1), size); b1 += size; --n1; }
        else { memcpy(tmp, GPTR(b2), size); b2 += size; --n2; }
        tmp += size;
    }
    if (n1 > 0) memcpy(tmp, GPTR(b1), (size_t)n1 * size);
    memcpy(GPTR(b), s->tmp, (size_t)(n - n2) * size);
}
static void guest_qsort(CPU *c) {
    uint32_t base = argument(c, 0), count = argument(c, 1), size = argument(c, 2), function = argument(c, 3);
    uint64_t total = (uint64_t)count * size;
    if (total > UINT32_MAX) rt_trap(c, c->eip, "qsort: array size overflow");
    if (count <= 1 || !size) return;
    rt_write_span(c, base, total);
    Sort s = {c, function, size, malloc((size_t)total)};
    // glibc se replie sur un quicksort si la mémoire manque : ordre différent pour
    // les égaux, on préfère s'arrêter.
    if (!s.tmp) rt_trap(c, c->eip, "qsort: cannot allocate merge buffer");
    msort(&s, base, count);
    free(s.tmp);
}

// random_r TYPE_3 de glibc (x**31 + x**3 + 1), état par défaut = srandom(1).
static pthread_mutex_t random_lock = PTHREAD_MUTEX_INITIALIZER;
static int32_t random_state[31];
static int random_front, random_rear, random_seeded;
static int32_t random_next(void) {
    uint32_t value = (uint32_t)random_state[random_front] + (uint32_t)random_state[random_rear];
    random_state[random_front] = (int32_t)value;
    int32_t result = (int32_t)(value >> 1);
    if (++random_front >= 31) { random_front = 0; ++random_rear; }
    else if (++random_rear >= 31) random_rear = 0;
    return result;
}
static void random_seed(uint32_t seed) {
    if (!seed) seed = 1;
    random_state[0] = (int32_t)seed;
    int32_t word = (int32_t)seed;
    for (int i = 1; i < 31; ++i) {
        int32_t hi = word / 127773, lo = word % 127773;
        word = 16807 * lo - 2836 * hi;
        if (word < 0) word += 2147483647;
        random_state[i] = word;
    }
    random_front = 3; random_rear = 0;
    for (int i = 0; i < 310; ++i) random_next();
    random_seeded = 1;
}

int rt_numeric_shim(CPU *c, const char *name) {
    if (!strcmp(name, "strtoll") || !strcmp(name, "__strtoll_internal")) integer(c, 1, 64);
    else if (!strcmp(name, "strtoull") || !strcmp(name, "__strtoull_internal")) integer(c, 0, 64);
    // Le paramètre `group` des variantes _internal n'a pas d'effet en locale C.
    else if (!strcmp(name, "__strtol_internal")) integer(c, 1, 32);
    else if (!strcmp(name, "__strtoul_internal")) integer(c, 0, 32);
    else if (!strcmp(name, "__strtod_internal") || !strcmp(name, "__strtod_l")) floating(c, 1);
    else if (!strcmp(name, "__strtof_internal") || !strcmp(name, "__strtof_l")) floating(c, 0);
    else if (!strcmp(name, "strtold_l")) floating(c, 2);
    else if (!strcmp(name, "qsort")) guest_qsort(c);
    else if (!strcmp(name, "srand")) {
        pthread_mutex_lock(&random_lock); random_seed(argument(c, 0)); pthread_mutex_unlock(&random_lock);
    } else if (!strcmp(name, "rand")) {
        pthread_mutex_lock(&random_lock);
        if (!random_seeded) random_seed(1);
        c->eax = (uint32_t)random_next();
        pthread_mutex_unlock(&random_lock);
    } else return 0;
    return 1;
}
RT_SHIM_PROVIDER(rt_numeric_shim)
