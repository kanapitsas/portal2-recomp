// Fonctions de chaînes glibc restantes, avec les adresses invitées comme
// résultats, et les variantes GNU de strerror_r et fortify de realpath.
// SHIMS: memchr strncmp strspn strcspn strtok strtok_r strerror_r __realpath_chk
#include "rt.h"
#include "posix.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

static uint32_t arg(CPU *c, unsigned i) { return R32(c->esp + 4 + 4 * i); }
static const char *text(CPU *c, uint32_t address) { rt_string_length(c, address); return GPTR(address); }

// Longueur lisible d'au plus n octets, NUL compris s'il est atteint (comme strncmp).
static uint32_t bounded(CPU *c, uint32_t address, uint32_t n) {
    uint32_t length = rt_bounded_strnlen(c, address, n);
    return length < n ? length + 1 : n;
}

static uint32_t token(CPU *c, uint32_t start, uint32_t delimiters, uint32_t *save) {
    const char *delim = text(c, delimiters);
    uint32_t s = start ? start : *save;
    if (!s) return 0;
    rt_string_length(c, s);
    s += (uint32_t)strspn(GPTR(s), delim);
    if (!R8(s)) { *save = s; return 0; }
    uint32_t end = s + (uint32_t)strcspn(GPTR(s), delim);
    if (R8(end)) { W8(end, 0); *save = end + 1; } else *save = end;
    return s;
}

// strerror_r GNU : message constant pour un errno connu (le tampon est
// ignoré), sinon « Unknown error N » écrit dans le tampon (tronqué).
// Les messages sont des chaînes invitées allouées une fois par errno et
// partagées avec strerror (libc.c) : auparavant strerror allouait à chaque appel.
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static uint32_t messages[256], unknown_message;   // adresses invitées, oubliées par rt_strings_extra_destroy
static uint32_t known_message(int code) {
    if (code <= 0 || code >= 256) return 0;
    int native = rt_darwin_errno(code);
    if (!native) return 0;
    pthread_mutex_lock(&lock);
    if (!messages[code]) {
        const char *message = strerror(native);
        size_t size = strlen(message) + 1;
        uint32_t copy = rt_alloc((uint32_t)size);
        if (copy) { memcpy(GPTR(copy), message, size); messages[code] = copy; }
    }
    uint32_t result = messages[code];
    pthread_mutex_unlock(&lock);
    return result;
}
uint32_t rt_strerror_message(int code) {
    uint32_t message = known_message(code);
    if (message) return message;
    // glibc : « Success » pour 0, sinon « Unknown error N » dans un tampon
    // statique (strerror n'est pas réentrant, comme dans glibc).
    pthread_mutex_lock(&lock);
    if (!unknown_message) unknown_message = rt_alloc(32);
    uint32_t unknown = unknown_message;
    if (unknown) {
        if (code) snprintf(GPTR(unknown), 32, "Unknown error %d", code);
        else strcpy(GPTR(unknown), "Success");
    }
    pthread_mutex_unlock(&lock);
    return unknown;
}

static uint32_t strtok_state;  // glibc : état de strtok global au processus
// Les adresses invitées gardées ici ne survivent pas à rt_destroy (tas remis
// à zéro, mémoire démappée) : appelé par rt_shims_destroy.
void rt_strings_extra_destroy(void) {
    pthread_mutex_lock(&lock);
    memset(messages, 0, sizeof(messages)); unknown_message = 0; strtok_state = 0;
    pthread_mutex_unlock(&lock);
}

static int strings_extra(CPU *c, const char *name) {
    if (!strcmp(name, "memchr")) {
        uint32_t s = arg(c, 0), n = arg(c, 2);
        const uint8_t *p = rt_span(c, s, n), *hit = memchr(p, (int)(uint8_t)arg(c, 1), n);
        c->eax = hit ? s + (uint32_t)(hit - p) : 0;
    } else if (!strcmp(name, "strncmp")) {
        uint32_t a = arg(c, 0), b = arg(c, 1), n = arg(c, 2);
        uint32_t la = bounded(c, a, n), lb = bounded(c, b, n);
        c->eax = (uint32_t)strncmp(GPTR(a), GPTR(b), la < lb ? la : lb);
    } else if (!strcmp(name, "strspn")) {
        c->eax = (uint32_t)strspn(text(c, arg(c, 0)), text(c, arg(c, 1)));
    } else if (!strcmp(name, "strcspn")) {
        c->eax = (uint32_t)strcspn(text(c, arg(c, 0)), text(c, arg(c, 1)));
    } else if (!strcmp(name, "strtok")) {
        c->eax = token(c, arg(c, 0), arg(c, 1), &strtok_state);
    } else if (!strcmp(name, "strtok_r")) {
        uint32_t save_address = arg(c, 2), save = R32(save_address);
        rt_write_span(c, save_address, 4);
        c->eax = token(c, arg(c, 0), arg(c, 1), &save);
        W32(save_address, save);
    } else if (!strcmp(name, "strerror_r")) {
        int code = (int32_t)arg(c, 0);
        uint32_t buffer = arg(c, 1), size = arg(c, 2), message = known_message(code);
        if (message) { c->eax = message; return 1; }
        char unknown[64];
        int length = snprintf(unknown, sizeof(unknown), "Unknown error %d", code);
        if (size) {
            uint32_t n = (uint32_t)length + 1 <= size ? (uint32_t)length + 1 : size;
            memcpy(rt_write_span(c, buffer, n), unknown, n - 1);
            W8(buffer + n - 1, 0);
        }
        c->eax = buffer;
    } else if (!strcmp(name, "__realpath_chk")) {
        // glibc : le tampon fourni doit pouvoir contenir PATH_MAX octets.
        if (arg(c, 1) && arg(c, 2) < 4096) rt_trap(c, c->eip, "*** buffer overflow detected ***: __realpath_chk");
        return rt_shim(c, "realpath");
    } else return 0;
    return 1;
}
RT_SHIM_PROVIDER(strings_extra)
