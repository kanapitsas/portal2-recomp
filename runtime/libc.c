// Explicit guest handles and i386 layouts: no native FILE/DIR/timeval pointer
// or structure is ever cast to the guest ABI.
#include "posix.h"
#include <dirent.h>
#include <pthread.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/sysctl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

typedef struct Stream {
    uint32_t handle;
    FILE *file;
    int directory;         // flux de répertoire : liste fusionnée figée à l'ouverture
    RtDirEntry *entries;
    size_t entry_count, position;
    uint32_t dirent, dirent64;
    int standard;
    struct Stream *next, *prev;
} Stream;
static Stream *streams;
static uint32_t *free_handles; static size_t free_count, free_capacity;
// Index des flux ouverts par handle (adressage ouvert, suppression par décalage
// arrière). Les flux fermés sont retirés : la liste ne grandit plus avec
// chaque fopen (des milliers en jeu), et un handle fermé reste inconnu.
static Stream **stream_index; static uint32_t index_capacity, index_count;
static uint32_t index_slot(uint32_t handle) { return (handle >> 8) * 2654435761u; }
static void index_put(Stream *s) {
    if ((index_count + 1) * 2 > index_capacity) {
        uint32_t capacity = index_capacity ? index_capacity * 2 : 256;
        Stream **grown = calloc(capacity, sizeof(*grown));
        if (!grown) abort();
        for (uint32_t i = 0; i < index_capacity; ++i) if (stream_index[i]) {
            uint32_t h = index_slot(stream_index[i]->handle) & (capacity - 1);
            while (grown[h]) h = (h + 1) & (capacity - 1);
            grown[h] = stream_index[i];
        }
        free(stream_index); stream_index = grown; index_capacity = capacity;
    }
    uint32_t h = index_slot(s->handle) & (index_capacity - 1);
    while (stream_index[h]) h = (h + 1) & (index_capacity - 1);
    stream_index[h] = s; ++index_count;
}
static Stream *index_get(uint32_t handle) {
    if (!index_capacity) return NULL;
    for (uint32_t h = index_slot(handle) & (index_capacity - 1); stream_index[h]; h = (h + 1) & (index_capacity - 1))
        if (stream_index[h]->handle == handle) return stream_index[h];
    return NULL;
}
static void index_remove(uint32_t handle) {
    if (!index_capacity) return;
    uint32_t mask = index_capacity - 1, h = index_slot(handle) & mask;
    while (stream_index[h] && stream_index[h]->handle != handle) h = (h + 1) & mask;
    if (!stream_index[h]) return;
    stream_index[h] = NULL; --index_count;
    for (uint32_t j = (h + 1) & mask; stream_index[j]; j = (j + 1) & mask) {   // décalage arrière
        Stream *moved = stream_index[j]; stream_index[j] = NULL;
        uint32_t k = index_slot(moved->handle) & mask;
        while (stream_index[k]) k = (k + 1) & mask;
        stream_index[k] = moved;
    }
}
static void link_stream(Stream *s) {
    s->prev = NULL; s->next = streams; if (streams) streams->prev = s; streams = s; index_put(s);
}
// Retire un flux qui n'a plus ni FILE ni liste de répertoire.
static void release_if_unused(Stream *s) {
    if (s->file || s->directory || s->standard) return;
    if (s->prev) s->prev->next = s->next; else streams = s->next;
    if (s->next) s->next->prev = s->prev;
    index_remove(s->handle);
    if (free_count == free_capacity) {
        size_t capacity = free_capacity ? free_capacity * 2 : 256;
        uint32_t *grown = realloc(free_handles, capacity * sizeof(*grown));
        if (!grown) abort();
        free_handles = grown; free_capacity = capacity;
    }
    free_handles[free_count++] = s->handle;
    free(s);
}
static uint32_t next_handle = 0xd0000100u;
static _Thread_local uint32_t errno_address;
typedef struct Environment { char *name; uint32_t value; struct Environment *next; } Environment;
static Environment *environment;
static uint32_t environ_slots[4];
static size_t environ_slot_count;

// errno Darwin et Linux coïncident de 1 à 34 (sauf EAGAIN/EDEADLK : 11/35 inversés,
// traités avant la table par rt_linux_errno/rt_darwin_errno).
static const struct { int darwin, linux_value; } errno_table[] = {
    {EINPROGRESS, 115}, {EALREADY, 114}, {ENOTSOCK, 88},
    {EDESTADDRREQ, 89}, {EMSGSIZE, 90}, {EPROTOTYPE, 91}, {ENOPROTOOPT, 92},
    {EPROTONOSUPPORT, 93}, {ESOCKTNOSUPPORT, 94}, {ENOTSUP, 95}, {EOPNOTSUPP, 95},
    {EPFNOSUPPORT, 96}, {EAFNOSUPPORT, 97}, {EADDRINUSE, 98}, {EADDRNOTAVAIL, 99},
    {ENETDOWN, 100}, {ENETUNREACH, 101}, {ENETRESET, 102}, {ECONNABORTED, 103},
    {ECONNRESET, 104}, {ENOBUFS, 105}, {EISCONN, 106}, {ENOTCONN, 107}, {ESHUTDOWN, 108},
    {ETOOMANYREFS, 109}, {ETIMEDOUT, 110}, {ECONNREFUSED, 111}, {ELOOP, 40},
    {ENAMETOOLONG, 36}, {EHOSTDOWN, 112}, {EHOSTUNREACH, 113}, {ENOTEMPTY, 39},
    {EUSERS, 87}, {EDQUOT, 122}, {ESTALE, 116}, {EREMOTE, 66}, {ENOLCK, 37}, {ENOSYS, 38},
    {EOVERFLOW, 75}, {ECANCELED, 125}, {EIDRM, 43}, {ENOMSG, 42}, {EILSEQ, 84},
    {ENOATTR, 61}, {EBADMSG, 74}, {EMULTIHOP, 72}, {ENODATA, 61}, {ENOLINK, 67},
    {ENOSR, 63}, {ENOSTR, 60}, {EPROTO, 71}, {ETIME, 62}, {ENOTRECOVERABLE, 131},
    {EOWNERDEAD, 130}, {EPROCLIM, 11}, {EBADEXEC, 8}, {EBADARCH, 8}, {EBADMACHO, 8},
    {ESHLIBVERS, 8}, {EFTYPE, 22}, {EAUTH, 13}, {ENEEDAUTH, 13}, {EPWROFF, 5}, {EDEVERR, 5},
    {EBADRPC, 5}, {ERPCMISMATCH, 5}, {EPROGUNAVAIL, 5}, {EPROGMISMATCH, 5}, {EPROCUNAVAIL, 5},
    {ENOPOLICY, 1}, {EQFULL, 105},
};
int rt_linux_errno(int value) {
    if (value == EDEADLK) return 35;
    if (value == EAGAIN) return 11;
    if (value > 0 && value <= 34) return value;
    for (size_t i = 0; i < sizeof(errno_table) / sizeof(*errno_table); ++i)
        if (errno_table[i].darwin == value) return errno_table[i].linux_value;
    return 5; // erreur Darwin sans équivalent Linux : EIO
}
int rt_darwin_errno(int value) {
    if (value == 11) return EAGAIN;
    if (value == 35) return EDEADLK;
    if (value > 0 && value <= 34) return value;
    for (size_t i = 0; i < sizeof(errno_table) / sizeof(*errno_table); ++i)
        if (errno_table[i].linux_value == value) return errno_table[i].darwin;
    return 0;
}
static uint32_t errno_slot(CPU *c) {
    if (!errno_address) {
        errno_address = rt_alloc(4);
        if (!errno_address) rt_trap(c, c->eip, "cannot allocate guest errno");
        W32(errno_address, 0);
    }
    return errno_address;
}
void rt_set_errno(CPU *c, int value) { W32(errno_slot(c), (uint32_t)value); }
void rt_save_errno(CPU *c) { rt_set_errno(c, rt_linux_errno(errno)); }
static int mode_writes(const char *mode) { return mode[0] == 'w' || mode[0] == 'a' || strchr(mode, '+') != NULL; }
const char *rt_stream_path(const char *path, const char *mode) {
    return mode_writes(mode) ? rt_write_path(path, mode[0] != 'w') : rt_read_path(path);
}
uint32_t rt_dirent_size(int wide, size_t name_length) {
    size_t header = wide ? 19 : 11, align = wide ? 7 : 3;
    return (uint32_t)((header + name_length + 1 + align) & ~align);
}
void rt_write_dirent(uint32_t address, int wide, const RtDirEntry *entry, uint64_t offset) {
    size_t len = strlen(entry->name);
    uint32_t header = wide ? 19 : 11, size = rt_dirent_size(wide, len);
    memset(GPTR(address), 0, size);
    if (wide) { W64(address, entry->ino); W64(address + 8, offset); }
    else { W32(address, (uint32_t)entry->ino); W32(address + 4, (uint32_t)offset); }
    W16(address + header - 3, size);
    W8(address + header - 1, entry->type);
    memcpy(GPTR(address + header), entry->name, len + 1);
}
static Stream *lookup(CPU *c, uint32_t handle, int directory) {
    Stream *s = index_get(handle);
    if (s && (directory ? s->directory : s->file != NULL)) return s;
    rt_trap(c, c->eip, directory ? "unknown guest DIR handle" : "unknown guest FILE handle");
}
void *rt_stream_file(CPU *c, uint32_t handle) { return lookup(c, handle, 0)->file; }
// Handles des flux libérés, réutilisés : la plage 0xd0000100-0xd0ffff00 (65 535
// handles, 0xd1000000 = flux standard) s'épuisait en deux minutes de jeu (le
// correcteur de casse ouvre ~400 dossiers par seconde) et le compteur
// atteignait le handle de stdin.
static Stream *new_stream_record(void) {
    uint32_t handle;
    if (free_count) handle = free_handles[--free_count];
    else {
        if (next_handle >= 0xd1000000u) { fputs("guest stream handles exhausted\n", stderr); abort(); }
        handle = next_handle; next_handle += 0x100;
    }
    Stream *s = calloc(1, sizeof(*s));
    if (!s) { free_handles[free_count++] = handle; return NULL; }
    s->handle = handle;
    link_stream(s);
    return s;
}
static uint32_t new_stream(FILE *file) {
    Stream *s = new_stream_record();
    if (!s) return 0;
    s->file = file;
    return s->handle;
}
uint32_t rt_stream_new(FILE *file) { return new_stream(file); }
FILE *rt_stream_take(CPU *c, uint32_t handle) {
    Stream *s = lookup(c, handle, 0);
    if (s->standard) rt_trap(c, c->eip, "standard stream cannot be detached");
    FILE *file = s->file; s->file = NULL;
    release_if_unused(s);
    return file;
}

// Liste fusionnée d'un répertoire invité : copie utilisateur, dépôt Linux,
// copie du jeu. Un nom n'apparaît qu'une fois ; la première source gagne.
typedef struct { RtDirEntry *items; size_t count, capacity; uint32_t *table; size_t buckets; } Listing;
static uint32_t name_hash(const char *name) {
    uint32_t h = 2166136261u;
    while (*name) h = (h ^ (uint8_t)*name++) * 16777619u;
    return h;
}
static int listing_has(Listing *l, const char *name) {
    if (!l->buckets) return 0;
    for (size_t i = name_hash(name) & (l->buckets - 1);; i = (i + 1) & (l->buckets - 1)) {
        if (!l->table[i]) return 0;
        if (!strcmp(l->items[l->table[i] - 1].name, name)) return 1;
    }
}
static int listing_add(Listing *l, const struct dirent *entry) {
    if (l->count + 1 > l->buckets / 2) {
        size_t buckets = l->buckets ? l->buckets * 2 : 64;
        uint32_t *table = calloc(buckets, sizeof(*table));
        if (!table) return -1;
        for (size_t i = 0; i < l->count; ++i) {
            size_t j = name_hash(l->items[i].name) & (buckets - 1);
            while (table[j]) j = (j + 1) & (buckets - 1);
            table[j] = (uint32_t)i + 1;
        }
        free(l->table); l->table = table; l->buckets = buckets;
    }
    if (l->count == l->capacity) {
        size_t capacity = l->capacity ? l->capacity * 2 : 32;
        RtDirEntry *items = realloc(l->items, capacity * sizeof(*items));
        if (!items) return -1;
        l->items = items; l->capacity = capacity;
    }
    char *name = strdup(entry->d_name);
    if (!name) return -1;
    l->items[l->count] = (RtDirEntry){name, entry->d_ino, entry->d_type};
    size_t j = name_hash(name) & (l->buckets - 1);
    while (l->table[j]) j = (j + 1) & (l->buckets - 1);
    l->table[j] = (uint32_t)++l->count;
    return 0;
}
void rt_dir_entries_free(RtDirEntry *entries, size_t count) {
    for (size_t i = 0; i < count; ++i) free(entries[i].name);
    free(entries);
}
// Cache des listes de dossiers du jeu : le correcteur de casse de Source liste
// les dossiers composant par composant à chaque fichier absent (des dizaines
// de milliers de fois par minute). Invalidé à chaque écriture invitée
// (rt_paths_generation) ; les changements faits hors du jeu ne sont pas vus
// pendant la partie.
static int dir_entries_uncached(const char *path, RtDirEntry **entries, size_t *count);
typedef struct { char relative[PATH_MAX]; unsigned long generation; int error; RtDirEntry *entries; size_t count; } DirCache;
static DirCache dir_cache[64];
static unsigned dir_cache_next;
static pthread_mutex_t dir_cache_lock = PTHREAD_MUTEX_INITIALIZER;
static RtDirEntry *copy_entries(const RtDirEntry *from, size_t count) {
    RtDirEntry *to = calloc(count ? count : 1, sizeof(*to));
    for (size_t i = 0; to && i < count; ++i) {
        to[i] = from[i];
        if (!(to[i].name = strdup(from[i].name))) { rt_dir_entries_free(to, i); return NULL; }
    }
    return to;
}
int rt_dir_entries(const char *path, RtDirEntry **entries, size_t *count) {
    char relative[PATH_MAX];
    int inside = rt_path_relative(path, relative);
    if (inside < 0) return dir_entries_uncached(path, entries, count);
    if (!inside) {   // hors du jeu (parents de la racine...) : clé = chemin absolu
        if (rt_path_absolute(path, relative)) return dir_entries_uncached(path, entries, count);
        memmove(relative + 1, relative, strnlen(relative, PATH_MAX - 2) + 1);
        relative[0] = '\x01';   // ne peut pas coïncider avec un chemin relatif du jeu
    }
    unsigned long now = rt_paths_generation();
    pthread_mutex_lock(&dir_cache_lock);
    for (unsigned i = 0; i < 64; ++i) {
        DirCache *e = &dir_cache[i];
        if (e->generation != now || strcmp(e->relative, relative)) continue;
        int error = e->error;
        RtDirEntry *copy = error ? NULL : copy_entries(e->entries, e->count);
        size_t n = e->count;
        pthread_mutex_unlock(&dir_cache_lock);
        if (error) { errno = error; return -1; }
        if (!copy) { errno = ENOMEM; return -1; }
        *entries = copy; *count = n;
        return 0;
    }
    pthread_mutex_unlock(&dir_cache_lock);
    int result = dir_entries_uncached(path, entries, count), error = result ? errno : 0;
    if (result && error != ENOENT && error != ENOTDIR) return result;
    RtDirEntry *stored = result ? NULL : copy_entries(*entries, *count);
    if (!result && !stored) return result;
    pthread_mutex_lock(&dir_cache_lock);
    DirCache *e = &dir_cache[dir_cache_next++ % 64];
    if (e->entries) rt_dir_entries_free(e->entries, e->count);
    snprintf(e->relative, sizeof(e->relative), "%s", relative);
    e->generation = now; e->error = error; e->entries = stored; e->count = result ? 0 : *count;
    pthread_mutex_unlock(&dir_cache_lock);
    if (result) errno = error;
    return result;
}
static int dir_entries_uncached(const char *path, RtDirEntry **entries, size_t *count) {
    char relative[PATH_MAX], sources[3][PATH_MAX];
    int inside = rt_path_relative(path, relative), source_count = 0;
    if (inside < 0) return -1;
    if (inside) {
        // Même règle que rt_read_path : le répertoire doit être visible.
        const char *visible = rt_read_path(path); struct stat info;
        if (!visible) return -1;
        if (stat(visible, &info)) return -1;
        if (!S_ISDIR(info.st_mode)) { errno = ENOTDIR; return -1; }
        if (rt_path_overlay(relative, sources[0])) return -1;
        source_count = 1;
        char lower[2][PATH_MAX]; int lower_count = rt_path_lower(relative, lower);
        for (int i = 0; i < lower_count; ++i) strcpy(sources[source_count++], lower[i]);
    } else {
        if (strlen(path) >= PATH_MAX) { errno = ENAMETOOLONG; return -1; }
        strcpy(sources[0], path); source_count = 1;
    }
    Listing l = {0};
    int opened = 0;
    for (int i = 0; i < source_count; ++i) {
        DIR *dir = opendir(sources[i]);
        if (!dir) {
            if (!inside || (errno != ENOENT && errno != ENOTDIR)) goto fail;
            continue;
        }
        ++opened;
        struct dirent *entry;
        for (errno = 0; (entry = readdir(dir)); errno = 0) {
            if (listing_has(&l, entry->d_name)) continue;
            // Les sources originales (i > 0) respectent les suppressions ;
            // la copie utilisateur cache seulement le répertoire des marques.
            if (inside && (i ? rt_path_hidden_name(relative, entry->d_name)
                             : !*relative && !strcmp(entry->d_name, RT_WHITEOUTS))) continue;
            if (inside && rt_path_is_game_source(sources[i])) {
                char child[PATH_MAX];
                int length = snprintf(child, sizeof(child), "%s%s%s", relative, *relative ? "/" : "", entry->d_name);
                if (length < 0 || length >= (int)sizeof(child) || rt_path_windows_binary(child)) continue;
            }
            if (listing_add(&l, entry)) { closedir(dir); errno = ENOMEM; goto fail; }
        }
        int error = errno;
        closedir(dir);
        if (error) { errno = error; goto fail; }
    }
    if (!opened) { errno = ENOENT; goto fail; }
    free(l.table);
    *entries = l.items; *count = l.count;
    return 0;
fail:;
    int error = errno;
    free(l.table); rt_dir_entries_free(l.items, l.count);
    errno = error;
    return -1;
}
uint32_t rt_standard_stream(int index) {
    uint32_t handle = 0xd1000000u + (uint32_t)index * 0x100;
    if (index_get(handle)) return handle;
    Stream *s = calloc(1, sizeof(*s));
    if (!s) return 0;
    s->handle = handle; s->file = index == 0 ? stdin : index == 1 ? stdout : stderr;
    s->standard = 1; link_stream(s);
    return handle;
}
void rt_libc_destroy(void) {
    while (streams) {
        Stream *next = streams->next;
        if (streams->file && !streams->standard) fclose(streams->file);
        rt_dir_entries_free(streams->entries, streams->entry_count);
        free(streams); streams = next;
    }
    free(stream_index); stream_index = NULL; index_capacity = index_count = 0;
    free(free_handles); free_handles = NULL; free_count = free_capacity = 0;
    while (environment) { Environment *next = environment->next; free(environment->name); free(environment); environment = next; }
    next_handle = 0xd0000100u; errno_address = 0; environ_slot_count = 0;
}


// Diagnostic P2_TRACE_FILES=<texte> : ouvertures, suppressions et renommages
// invités dont le chemin contient ce texte (sans casse ; « 1 » : tous), avec
// le chemin hôte et le résultat, sur la sortie d'erreur.
int rt_trace_files_match(const char *filter, const char *path) {
    if (!filter || !*filter || !strcmp(filter, "0") || !path) return 0;
    return !strcmp(filter, "1") || strcasestr(path, filter) != NULL;
}
int rt_trace_files(const char *path) {
    static const char *filter; static int read;
    if (!read) { filter = getenv("P2_TRACE_FILES"); read = 1; }
    return rt_trace_files_match(filter, path);
}

// Opérations de rt_libc_shim : la recherche par nom (table triée, bsearch)
// n'a lieu qu'une fois par import, rt_libc_resolve mémorisant l'opération
// dans la route (route->data) pour les appels suivants.
enum {
    // Flux (FILE invité)
    OP_FOPEN, OP_FCLOSE, OP_FGETS, OP_FPUTS, OP_FPUTC, OP_FGETC, OP_FEOF, OP_FERROR, OP_CLEARERR,
    OP_FILENO, OP_REWIND, OP_FTELL, OP_FTELLO64, OP_FSEEK, OP_FSEEKO64, OP_FFLUSH, OP_FREAD, OP_FWRITE,
    // Descripteurs
    OP_OPEN, OP_OPEN_2, OP_CLOSE, OP_READ, OP_WRITE, OP_LSEEK, OP_LSEEK64, OP_ACCESS, OP_READLINK,
    // Répertoires
    OP_CHDIR, OP_GETCWD, OP_OPENDIR, OP_CLOSEDIR, OP_READDIR, OP_READDIR64,
    // Chaînes et caractères
    OP_STRCHR, OP_STRRCHR, OP_STRPBRK, OP_STRSTR, OP_STRCASESTR, OP_STRCASECMP, OP_STRNCASECMP,
    OP_STRDUP, OP_STRCAT, OP_STRNCAT, OP_STRTOD, OP_STRTOL, OP_STRTOUL, OP_STRERROR, OP_TOLOWER, OP_TOUPPER,
    // Système
    OP_ERRNO_LOCATION, OP_GETENV, OP_GETTIMEOFDAY, OP_SYSCALL, OP_CLOCK_GETTIME, OP_SYSCONF,
    OP_MMAP, OP_MMAP64, OP_MUNMAP, OP_MPROTECT, OP_MADVISE, OP_USLEEP, OP_GETPPID,
};
typedef struct { const char *name; int op; } LibcName;
// Triée par strcmp ('_' précède les lettres) ; vérifiée au démarrage.
static const LibcName libc_names[] = {
    {"__errno_location", OP_ERRNO_LOCATION}, {"__open64_2", OP_OPEN_2}, {"__open_2", OP_OPEN_2},
    {"access", OP_ACCESS}, {"chdir", OP_CHDIR}, {"clearerr", OP_CLEARERR}, {"clock_gettime", OP_CLOCK_GETTIME},
    {"close", OP_CLOSE}, {"closedir", OP_CLOSEDIR}, {"fclose", OP_FCLOSE}, {"feof", OP_FEOF}, {"ferror", OP_FERROR},
    {"fflush", OP_FFLUSH}, {"fgetc", OP_FGETC}, {"fgets", OP_FGETS}, {"fileno", OP_FILENO},
    {"fileno_unlocked", OP_FILENO}, {"fopen", OP_FOPEN}, {"fopen64", OP_FOPEN}, {"fputc", OP_FPUTC},
    {"fputs", OP_FPUTS}, {"fread", OP_FREAD}, {"fseek", OP_FSEEK}, {"fseeko", OP_FSEEK}, {"fseeko64", OP_FSEEKO64},
    {"ftell", OP_FTELL}, {"ftello", OP_FTELL}, {"ftello64", OP_FTELLO64}, {"fwrite", OP_FWRITE}, {"getc", OP_FGETC},
    {"getcwd", OP_GETCWD}, {"getenv", OP_GETENV}, {"getppid", OP_GETPPID}, {"gettimeofday", OP_GETTIMEOFDAY},
    {"lseek", OP_LSEEK}, {"lseek64", OP_LSEEK64}, {"madvise", OP_MADVISE}, {"mmap", OP_MMAP}, {"mmap64", OP_MMAP64},
    {"mprotect", OP_MPROTECT}, {"munmap", OP_MUNMAP}, {"open", OP_OPEN}, {"open64", OP_OPEN}, {"opendir", OP_OPENDIR},
    {"putc", OP_FPUTC}, {"read", OP_READ}, {"readdir", OP_READDIR}, {"readdir64", OP_READDIR64},
    {"readlink", OP_READLINK}, {"rewind", OP_REWIND}, {"strcasecmp", OP_STRCASECMP}, {"strcasestr", OP_STRCASESTR},
    {"strcat", OP_STRCAT}, {"strchr", OP_STRCHR}, {"strdup", OP_STRDUP}, {"strerror", OP_STRERROR},
    {"strncasecmp", OP_STRNCASECMP}, {"strncat", OP_STRNCAT}, {"strpbrk", OP_STRPBRK}, {"strrchr", OP_STRRCHR},
    {"strstr", OP_STRSTR}, {"strtod", OP_STRTOD}, {"strtol", OP_STRTOL}, {"strtoul", OP_STRTOUL},
    {"syscall", OP_SYSCALL}, {"sysconf", OP_SYSCONF}, {"tolower", OP_TOLOWER}, {"toupper", OP_TOUPPER},
    {"usleep", OP_USLEEP}, {"write", OP_WRITE},
};
enum { LIBC_NAME_COUNT = sizeof(libc_names) / sizeof(*libc_names) };
static int libc_name_compare(const void *key, const void *entry) { return strcmp(key, ((const LibcName *)entry)->name); }
static int libc_op(const char *name) {
    const LibcName *found = bsearch(name, libc_names, LIBC_NAME_COUNT, sizeof(*libc_names), libc_name_compare);
    return found ? found->op : -1;
}
__attribute__((constructor)) static void libc_names_check(void) {
    for (size_t i = 1; i < LIBC_NAME_COUNT; ++i)
        if (strcmp(libc_names[i - 1].name, libc_names[i].name) >= 0) { fputs("libc_names is not sorted\n", stderr); abort(); }
}

// --- Flux invités (FILE) ----------------------------------------------------
static void libc_stream(CPU *c, int op) {
    uint32_t a, b, n;
    switch (op) {
    case OP_FOPEN: {
        const char *path = rt_guest_cstr(c, rt_arg32(c, 0)), *mode = rt_guest_cstr(c, rt_arg32(c, 1));
        FILE *file;
        {   // /proc/cpuinfo et les autres fichiers /proc viennent de procfs.c (rt_read_path)
            // Lecture d'un fichier que le cache sait absent : ENOENT sans fopen
            // hôte (le moteur sonde chaque dossier de recherche, ~110 fois/s).
            if (!mode_writes(mode) && rt_access_cached(path, F_OK) == -1 && errno == ENOENT) {
                file = NULL;
                if (rt_trace_files(path)) { int kept = errno; fprintf(stderr, "[fichiers] fopen %s \"%s\" : absent (cache)\n", path, mode); errno = kept; }
            } else {
                const char *resolved=rt_stream_path(path,mode);
                file=resolved?fopen(resolved,mode):NULL;
                if (rt_trace_files(path)) { int kept = errno; fprintf(stderr, "[fichiers] fopen %s \"%s\" -> %s : %s\n", path, mode, resolved ? resolved : "(null)", file ? "ok" : strerror(errno)); errno = kept; }
            }
        }
        if (!file) { rt_save_errno(c); c->eax = 0; }
        else {
            c->eax = new_stream(file);
            if (!c->eax) { fclose(file); errno = ENOMEM; rt_save_errno(c); }
        }
        break;
    }
    case OP_FCLOSE: {
        Stream *s = lookup(c, rt_arg32(c, 0), 0);
        // Flux standard : glibc ferme stderr pour de bon, mais le runtime garde
        // le FILE hôte (un fputs(stderr) ultérieur du jeu doit rester possible).
        int result = s->standard ? fflush(s->file) : fclose(s->file);
        if (!s->standard) s->file = NULL;
        if (result) rt_save_errno(c);
        release_if_unused(s);
        c->eax = (uint32_t)result;
        break;
    }
    case OP_FGETS: {
        a = rt_arg32(c, 0); n = rt_arg32(c, 1);
        Stream *s = lookup(c, rt_arg32(c, 2), 0);
        rt_write_span(c, a, n);
        char *result = fgets(GPTR(a), (int)n, s->file);
        if (!result && ferror(s->file)) rt_save_errno(c);
        c->eax = result ? a : 0;
        break;
    }
    case OP_FPUTS: {
        const char *value = rt_guest_cstr(c, rt_arg32(c, 0));
        Stream *s = lookup(c, rt_arg32(c, 1), 0);
        int result = fputs(value, s->file); if (result == EOF) rt_save_errno(c);
        c->eax = (uint32_t)result;
        break;
    }
    case OP_FPUTC: {
        Stream *s = lookup(c, rt_arg32(c, 1), 0);
        int result = fputc((int)rt_arg32(c, 0), s->file); if (result == EOF) rt_save_errno(c);
        c->eax = (uint32_t)result;
        break;
    }
    case OP_FGETC: {
        Stream *s = lookup(c, rt_arg32(c, 0), 0);
        int result = fgetc(s->file); if (result == EOF && ferror(s->file)) rt_save_errno(c);
        c->eax = (uint32_t)result;
        break;
    }
    case OP_FEOF: case OP_FERROR: case OP_CLEARERR: case OP_FILENO: case OP_REWIND: {
        Stream *s=lookup(c,rt_arg32(c,0),0);
        if(op==OP_FEOF)c->eax=(uint32_t)feof(s->file);
        else if(op==OP_FERROR)c->eax=(uint32_t)ferror(s->file);
        else if(op==OP_CLEARERR)clearerr(s->file);
        else if(op==OP_REWIND)rewind(s->file);
        else {int result=fileno(s->file);if(result<0)rt_save_errno(c);c->eax=(uint32_t)result;}
        break;
    }
    case OP_FTELL: case OP_FTELLO64: {
        off_t result=ftello(lookup(c,rt_arg32(c,0),0)->file);
        int wide=op==OP_FTELLO64;
        if(!wide && result>INT32_MAX){result=-1;errno=EOVERFLOW;}
        if(result<0)rt_save_errno(c);
        c->eax=(uint32_t)result;if(wide)c->edx=(uint32_t)((uint64_t)result>>32);
        break;
    }
    case OP_FSEEK: case OP_FSEEKO64: {
        Stream *s=lookup(c,rt_arg32(c,0),0);
        int wide=op==OP_FSEEKO64, whence=(int)rt_arg32(c,wide?3:2);
        int64_t offset=wide?(int64_t)(((uint64_t)rt_arg32(c,2)<<32)|rt_arg32(c,1)):(int32_t)rt_arg32(c,1);
        int result=fseeko(s->file,offset,whence);if(result)rt_save_errno(c);c->eax=(uint32_t)result;
        break;
    }
    case OP_FFLUSH: {
        a = rt_arg32(c, 0);
        int result = fflush(a ? lookup(c, a, 0)->file : NULL); if (result) rt_save_errno(c);
        c->eax = (uint32_t)result;
        break;
    }
    default: {   // OP_FREAD, OP_FWRITE
        a = rt_arg32(c, 0); b = rt_arg32(c, 1); n = rt_arg32(c, 2);
        Stream *s = lookup(c, rt_arg32(c, 3), 0);
        void *buffer = op == OP_FREAD ? rt_write_span(c, a, (uint64_t)b * n) : rt_span(c, a, (uint64_t)b * n);
        // La porte invitée reste tenue pendant l'E/S disque (ici, read/write, stat.c) : la
        // relâcher exigerait de figer le tampon invité contre munmap/mprotect concurrents.
        size_t count = op == OP_FREAD ? fread(buffer, b, n, s->file) : fwrite(buffer, b, n, s->file);
        if (count < n && ferror(s->file)) rt_save_errno(c);
        c->eax = (uint32_t)count;
    }
    }
}

// --- Descripteurs de fichiers -----------------------------------------------
static void libc_fd(CPU *c, int op) {
    uint32_t a, n;
    switch (op) {
    case OP_OPEN: case OP_OPEN_2: {
        const char *path = rt_guest_cstr(c, rt_arg32(c, 0)); uint32_t flags = rt_arg32(c, 1);
        int native = (int)(flags & 3);
        if (flags & 0x40) native |= O_CREAT;
        if (flags & 0x80) native |= O_EXCL;
        if (flags & 0x100) native |= O_NOCTTY;
        if (flags & 0x200) native |= O_TRUNC;
        if (flags & 0x400) native |= O_APPEND;
        if (flags & 0x800) native |= O_NONBLOCK;
        // Valeurs i386 (asm-generic/fcntl.h) : O_DSYNC 0x1000, O_LARGEFILE 0x8000
        // (sans effet : off_t hôte 64 bits), __O_SYNC 0x100000 (O_SYNC = 0x101000).
        // Hors liste : FASYNC 0x2000, O_DIRECT 0x4000, O_NOATIME 0x40000, O_PATH
        // 0x200000, __O_TMPFILE 0x400000 (ENOTSUP, le jeu ne les utilise pas).
        if (flags & 0x1000) native |= O_DSYNC;
        if (flags & 0x100000) native |= O_SYNC;
        if (flags & 0x10000) native |= O_DIRECTORY;
        if (flags & 0x20000) native |= O_NOFOLLOW;
        if (flags & 0x80000) native |= O_CLOEXEC;
        uint32_t supported = 3u|0x40|0x80|0x100|0x200|0x400|0x800|0x1000|0x8000|0x10000|0x20000|0x80000|0x100000;
        if (flags & ~supported) { errno = ENOTSUP; rt_save_errno(c); c->eax = UINT32_MAX; return; }
        if (!(flags & (3u | 0x40)) && rt_access_cached(path, F_OK) == -1 && errno == ENOENT) {
            rt_save_errno(c); c->eax = UINT32_MAX; return;   // lecture d'un absent connu : ENOENT direct
        }
        if ((flags & 0x40) && op == OP_OPEN_2) rt_trap(c,c->eip,"invalid fortified open flags");
        mode_t mode = flags & 0x40 ? (mode_t)rt_arg32(c, 2) : 0;
        if((flags & 0xc0)==0xc0) {
            const char *existing=rt_read_path(path);
            if(!existing || !access(existing,F_OK)) {
                if(existing)errno=EEXIST;
                rt_save_errno(c);c->eax=UINT32_MAX;return;
            }
        }
        const char *resolved=(flags & (3|0x40|0x200|0x400))?rt_write_path(path,!(flags & (0x80|0x200))):rt_read_path(path);
        int result = resolved?open(resolved, native, mode):-1; if (result < 0) rt_save_errno(c);
        if (rt_trace_files(path)) { int kept = errno; fprintf(stderr, "[fichiers] open %s %#x -> %s : %s\n", path, flags, resolved ? resolved : "(null)", result >= 0 ? "ok" : strerror(errno)); errno = kept; }
        c->eax = (uint32_t)result;
        break;
    }
    case OP_CLOSE: {
        int result = close((int32_t)rt_arg32(c, 0)); if (result) rt_save_errno(c);
        c->eax = (uint32_t)result;
        break;
    }
    case OP_READ: case OP_WRITE: {
        int fd = (int32_t)rt_arg32(c, 0); a=rt_arg32(c,1); n=rt_arg32(c,2);
        if (n > 0x7ffff000u) n=0x7ffff000u;
        ssize_t result;
        if (op==OP_READ) result=read(fd,rt_write_span(c,a,n),n);
        else result=write(fd,rt_span(c,a,n),n);
        if (result < 0) rt_save_errno(c);
        c->eax=(uint32_t)result;
        break;
    }
    case OP_LSEEK: case OP_LSEEK64: {
        int fd=(int32_t)rt_arg32(c,0); int64_t offset=(int32_t)rt_arg32(c,1); int whence=(int)rt_arg32(c,2);
        int wide=op==OP_LSEEK64;
        if (wide) { offset=(int64_t)(((uint64_t)rt_arg32(c,2)<<32)|rt_arg32(c,1)); whence=(int)rt_arg32(c,3); }
        off_t result=lseek(fd,(off_t)offset,whence);
        if (!wide && result > INT32_MAX) { result=-1; errno=EOVERFLOW; }
        if (result < 0) rt_save_errno(c);
        c->eax=(uint32_t)result; if (wide) c->edx=(uint32_t)((uint64_t)result>>32);
        break;
    }
    case OP_ACCESS: {
        const char *path = rt_guest_cstr(c, rt_arg32(c, 0));
        int result;
        if ((result = rt_access_cached(path, (int)rt_arg32(c, 1))) == -2) {
            const char *resolved=rt_read_path(path); result=resolved?access(resolved,(int)rt_arg32(c,1)):-1;
        }
        if (result) rt_save_errno(c);
        c->eax = (uint32_t)result;
        break;
    }
    default: {   // OP_READLINK
        const char *path=rt_guest_cstr(c,rt_arg32(c,0)); a=rt_arg32(c,1); n=rt_arg32(c,2);
        ssize_t result;
        char own_exe[48]; snprintf(own_exe,sizeof(own_exe),"/proc/%d/exe",(int)getpid());
        if(!strcmp(path,"/proc/self/exe") || !strcmp(path,own_exe)) {
            const char *program=rt_program_path(); size_t count=strlen(program);if(count>n)count=n;
            memcpy(rt_write_span(c,a,count),program,count);result=(ssize_t)count;
        } else {const char *resolved=rt_read_path(path);result=resolved?readlink(resolved,rt_write_span(c,a,n),n):-1;}
        if(result<0)rt_save_errno(c);
        c->eax=(uint32_t)result;
    }
    }
}

// --- Répertoires ------------------------------------------------------------
static void libc_dir(CPU *c, int op) {
    uint32_t a, n;
    switch (op) {
    case OP_CHDIR: {
        const char *resolved=rt_read_path(rt_guest_cstr(c,rt_arg32(c,0)));
        int result=resolved?chdir(resolved):-1;
        if(result)rt_save_errno(c);
        c->eax=(uint32_t)result;
        break;
    }
    case OP_GETCWD: {
        a=rt_arg32(c,0); n=rt_arg32(c,1);
        char *result=getcwd(NULL,0);
        if(!result){rt_save_errno(c);c->eax=0;}
        else {
            size_t length=strlen(result)+1;
            // glibc : tampon fourni de taille 0 → EINVAL ; trop petit → ERANGE
            // (aussi pour un tampon alloué de taille demandée, sans l'allouer).
            int error=a && !n?EINVAL:n && length>n?ERANGE:0;
            if(!error && !a){ a=rt_alloc(n?n:(uint32_t)length); if(!a)error=ENOMEM; }
            if(error){errno=error;rt_save_errno(c);c->eax=0;}
            else {memcpy(rt_write_span(c,a,length),result,length);c->eax=a;}
            free(result);
        }
        break;
    }
    case OP_OPENDIR: {
        RtDirEntry *entries; size_t count;
        if (rt_dir_entries(rt_guest_cstr(c, rt_arg32(c, 0)), &entries, &count)) { rt_save_errno(c); c->eax = 0; return; }
        Stream *s = new_stream_record();
        if (!s) { rt_dir_entries_free(entries, count); errno = ENOMEM; rt_save_errno(c); c->eax = 0; return; }
        s->directory = 1; s->entries = entries; s->entry_count = count;
        c->eax = s->handle;
        break;
    }
    case OP_CLOSEDIR: {
        Stream *s = lookup(c, rt_arg32(c, 0), 1);
        rt_dir_entries_free(s->entries, s->entry_count);
        s->entries = NULL; s->entry_count = 0; s->directory = 0;
        if (s->dirent) rt_free(s->dirent);
        if (s->dirent64) rt_free(s->dirent64);
        s->dirent = s->dirent64 = 0;
        c->eax = 0;
        release_if_unused(s);
        break;
    }
    default: {   // OP_READDIR, OP_READDIR64
        // Tampon invité par flux, de la taille d'un dirent à nom de 255 octets
        // (268 ou 280 octets, comme sizeof(struct dirent) glibc i386).
        Stream *s = lookup(c, rt_arg32(c, 0), 1);
        int wide = op == OP_READDIR64;
        c->eax = 0;
        if (s->position >= s->entry_count) return; // fin : errno inchangé
        RtDirEntry *entry = &s->entries[s->position];
        if (strlen(entry->name) > 255) { errno = ENAMETOOLONG; rt_save_errno(c); return; }
        if (!wide && entry->ino > UINT32_MAX) { ++s->position; errno = EOVERFLOW; rt_save_errno(c); return; }
        uint32_t *slot = wide ? &s->dirent64 : &s->dirent;
        if (!*slot) *slot = rt_alloc(rt_dirent_size(wide, 255));
        if (!*slot) { errno = ENOMEM; rt_save_errno(c); return; }
        ++s->position;
        rt_write_dirent(*slot, wide, entry, s->position);
        c->eax = *slot;
    }
    }
}

// --- Chaînes et caractères --------------------------------------------------
static void libc_string(CPU *c, int op) {
    uint32_t a, b;
    switch (op) {
    case OP_STRCHR: case OP_STRRCHR: case OP_STRPBRK: case OP_STRSTR: case OP_STRCASESTR: {
        a = rt_arg32(c, 0); const char *source = rt_guest_cstr(c, a); const char *result;
        if (op == OP_STRCHR) result = strchr(source, (int)rt_arg32(c, 1));
        else if (op == OP_STRRCHR) result = strrchr(source, (int)rt_arg32(c, 1));
        else {
            const char *other = rt_guest_cstr(c, rt_arg32(c, 1));
            result = op == OP_STRPBRK ? strpbrk(source, other) : op == OP_STRSTR ? strstr(source, other) : strcasestr(source, other);
        }
        c->eax = result ? a + (uint32_t)(result - source) : 0;
        break;
    }
    case OP_STRCASECMP: case OP_STRNCASECMP: {
        const char *lhs = rt_guest_cstr(c, rt_arg32(c, 0)), *rhs = rt_guest_cstr(c, rt_arg32(c, 1));
        c->eax = (uint32_t)(op == OP_STRCASECMP ? strcasecmp(lhs, rhs) : strncasecmp(lhs, rhs, rt_arg32(c, 2)));
        break;
    }
    case OP_STRDUP:
        c->eax = rt_guest_strdup(rt_guest_cstr(c, rt_arg32(c, 0)));
        if (!c->eax) { errno = ENOMEM; rt_save_errno(c); }
        break;
    case OP_STRCAT: case OP_STRNCAT: {
        a = rt_arg32(c, 0); b = rt_arg32(c, 1);
        uint32_t destination_length = rt_string_length(c, a);
        uint32_t count = rt_bounded_strnlen(c, b, op == OP_STRCAT ? UINT32_MAX : rt_arg32(c, 2));
        rt_write_span(c, a, (uint64_t)destination_length + count + 1);
        memmove(GPTR(a + destination_length), GPTR(b), count);
        W8(a + destination_length + count, 0); c->eax = a;
        break;
    }
    case OP_STRTOD: {
        a = rt_arg32(c, 0); b = rt_arg32(c, 1);
        const char *source = rt_guest_cstr(c, a); char *end;
        errno = 0; double value = strtod(source, &end);
        if (b) { uint32_t at = a + (uint32_t)(end - source); memcpy(rt_write_span(c, b, 4), &at, 4); }
        if (errno) rt_save_errno(c);
        fpush(c, value);
        break;
    }
    case OP_STRTOL: case OP_STRTOUL: {
        a = rt_arg32(c, 0); b = rt_arg32(c, 1);
        const char *source = rt_guest_cstr(c, a); char *end;
        int base = (int32_t)rt_arg32(c, 2);
        if (base && (base < 2 || base > 36)) { errno = EINVAL; rt_save_errno(c); c->eax = 0; return; }
        errno = 0;
        if (op == OP_STRTOL) {
            long value = strtol(source, &end, base);
            if (value > INT32_MAX) { value = INT32_MAX; errno = ERANGE; }
            if (value < INT32_MIN) { value = INT32_MIN; errno = ERANGE; }
            c->eax = (uint32_t)value;
        } else {
            unsigned long value = strtoul(source, &end, base);
            const char *sign = source; while (isspace((unsigned char)*sign)) ++sign;
            unsigned long magnitude = *sign == '-' ? 0ul - value : value;
            if (errno == ERANGE || magnitude > UINT32_MAX) { c->eax = UINT32_MAX; errno = ERANGE; }
            else c->eax = (uint32_t)value;
        }
        if (b) { rt_write_span(c, b, 4); W32(b, a + (uint32_t)(end - source)); }
        if (errno) rt_save_errno(c);
        break;
    }
    case OP_STRERROR:
        c->eax = rt_strerror_message((int32_t)rt_arg32(c, 0));
        break;
    default: {   // OP_TOLOWER, OP_TOUPPER
        int value = (int32_t)rt_arg32(c, 0);
        if (value < -1 || value > 255) rt_trap(c, c->eip, "invalid ctype input");
        c->eax = (uint32_t)(op == OP_TOLOWER ? tolower(value) : toupper(value));
    }
    }
}

// --- Système ----------------------------------------------------------------
static void libc_system(CPU *c, int op) {
    uint32_t a, b, n;
    switch (op) {
    case OP_ERRNO_LOCATION: c->eax = errno_slot(c); break;
    case OP_GETENV: {
        const char *key = rt_guest_cstr(c, rt_arg32(c, 0));
        const char *value = getenv(key);
        c->eax = 0;
        if (value) {
            Environment *item = environment;
            while (item && strcmp(item->name, key)) item = item->next;
            if (!item) {
                item = calloc(1, sizeof(*item));
                if (!item) return;
                item->name = strdup(key);
                if (!item->name) { free(item); return; }
                item->next = environment; environment = item;
            }
            if (!item->value || strcmp(GPTR(item->value), value)) {
                uint32_t copy = rt_guest_strdup(value);
                if (copy) item->value = copy;
            }
            c->eax = item->value;
        }
        break;
    }
    case OP_GETTIMEOFDAY: {
        a = rt_arg32(c, 0); b = rt_arg32(c, 1);
        struct timeval tv; struct timezone tz;
        int result = gettimeofday(&tv, b ? &tz : NULL);
        if (!result && (tv.tv_sec > INT32_MAX || tv.tv_sec < INT32_MIN)) { errno = EOVERFLOW; result = -1; }
        if (!result && a) { rt_write_span(c, a, 8); W32(a, (uint32_t)tv.tv_sec); W32(a + 4, (uint32_t)tv.tv_usec); }
        if (!result && b) { rt_write_span(c, b, 8); W32(b, (uint32_t)tz.tz_minuteswest); W32(b + 4, (uint32_t)tz.tz_dsttime); }
        if (result) rt_save_errno(c);
        c->eax = (uint32_t)result;
        break;
    }
    case OP_SYSCALL: {
        uint32_t number = rt_arg32(c, 0);
        if (number == 355) { // i386 SYS_getrandom
            a = rt_arg32(c, 1); n = rt_arg32(c, 2); b = rt_arg32(c, 3);
            if (b & ~7u) { errno = EINVAL; rt_save_errno(c); c->eax = UINT32_MAX; }
            else { arc4random_buf(rt_write_span(c, a, n), n); c->eax = n; }
        } else if (number == 224) {
            c->eax = !c->tid || c->tid==1 ? (uint32_t)getpid() : 0x40000000u+c->tid;
        } else if (number == 240) {
            int result=rt_futex(c,rt_arg32(c,1),rt_arg32(c,2),rt_arg32(c,3),rt_arg32(c,4));
            if(result<0)rt_save_errno(c);
            c->eax=(uint32_t)result;
        } else {
            fprintf(stderr, "unimplemented i386 syscall %u\n", number);
            rt_trap(c, c->eip, "unimplemented i386 syscall");
        }
        break;
    }
    case OP_CLOCK_GETTIME: {
        int id = (int32_t)rt_arg32(c, 0); b = rt_arg32(c, 1);
        clockid_t native;
        switch (id) {
        case 0: case 5: native = CLOCK_REALTIME; break;
        case 1: case 6: case 7: native = CLOCK_MONOTONIC; break;
        case 2: native = CLOCK_PROCESS_CPUTIME_ID; break;
        case 3: native = CLOCK_THREAD_CPUTIME_ID; break;
        case 4: native = CLOCK_MONOTONIC_RAW; break;
        default: errno = EINVAL; rt_save_errno(c); c->eax = UINT32_MAX; return;
        }
        struct timespec value;
        int result = clock_gettime(native, &value);
        if (!result && (value.tv_sec > INT32_MAX || value.tv_sec < INT32_MIN)) { result = -1; errno = EOVERFLOW; }
        if (!result) { rt_write_span(c, b, 8); W32(b, (uint32_t)value.tv_sec); W32(b + 4, (uint32_t)value.tv_nsec); }
        else rt_save_errno(c);
        c->eax = (uint32_t)result;
        break;
    }
    case OP_SYSCONF: {
        int key = (int32_t)rt_arg32(c, 0); long result;
        switch (key) {
        case 2: result = sysconf(_SC_CLK_TCK); break;
        case 4: result = sysconf(_SC_OPEN_MAX); break;
        case 30: result = 4096; break; // virtual i386/Linux page size
        case 83: case 84: result = (long)rt_guest_cpu_count(); break; // même topologie que CPUID et /proc
        case 85: case 86: {
            uint64_t memory = 0; size_t size = sizeof(memory);
            if (sysctlbyname("hw.memsize", &memory, &size, NULL, 0)) result = -1;
            else result = (long)((memory < UINT64_C(2147483648) ? memory : UINT64_C(2147483648)) / 4096);
            break;
        }
        default: errno = EINVAL; result = -1;
        }
        if (result == -1) rt_save_errno(c);
        c->eax = (uint32_t)result;
        break;
    }
    case OP_MMAP: case OP_MMAP64: {
        uint64_t offset = rt_arg32(c, 5);
        if (op == OP_MMAP64) offset |= (uint64_t)rt_arg32(c, 6) << 32;
        c->eax = rt_mmap(c, rt_arg32(c, 0), rt_arg32(c, 1), (int)rt_arg32(c, 2), rt_arg32(c, 3), (int32_t)rt_arg32(c, 4), offset);
        if (c->eax == UINT32_MAX) {
            rt_save_errno(c);
            if (getenv("P2_TRACE_MMAP"))
                fprintf(stderr, "[mmap] échec hint=%#x len=%#x prot=%u flags=%#x fd=%d errno=%d (mappé %u Mio)\n",
                        rt_arg32(c, 0), rt_arg32(c, 1), rt_arg32(c, 2), rt_arg32(c, 3), (int32_t)rt_arg32(c, 4), errno,
                        rt_vm_mapped_bytes() >> 20);
        }
        break;
    }
    case OP_MUNMAP: case OP_MPROTECT: case OP_MADVISE: {
        a = rt_arg32(c, 0); b = rt_arg32(c, 1); int result;
        if (op == OP_MUNMAP) result = rt_munmap(a, b);
        else if (op == OP_MPROTECT) result = rt_mprotect(a, b, (int)rt_arg32(c, 2));
        else result = rt_madvise(a, b, (int)rt_arg32(c, 2));
        if (result) rt_save_errno(c);
        c->eax = (uint32_t)result;
        break;
    }
    case OP_USLEEP: {
        uint32_t usec = rt_arg32(c, 0);
        struct timespec delay = {usec / 1000000u, (long)(usec % 1000000u) * 1000};
        int result = RT_BLOCKING(nanosleep(&delay, NULL));
        if (result) rt_save_errno(c);
        c->eax = (uint32_t)result;
        break;
    }
    default:   // OP_GETPPID
        c->eax = (uint32_t)getppid();
    }
}

static void libc_run(CPU *c, int op) {
    if (op <= OP_FWRITE) libc_stream(c, op);
    else if (op <= OP_READLINK) libc_fd(c, op);
    else if (op <= OP_READDIR64) libc_dir(c, op);
    else if (op <= OP_TOUPPER) libc_string(c, op);
    else libc_system(c, op);
}
// Route directe (rt_shim_route) : l'opération est dans route->data.
static int libc_handler(CPU *c, const char *name, void *data) { (void)name; libc_run(c, (int)(intptr_t)data); return 1; }
int rt_libc_resolve(const char *name, RtShimRoute *route) {
    int op = libc_op(name);
    if (op < 0) return 0;
    route->handler = libc_handler; route->data = (void *)(intptr_t)op;
    return 1;
}
int rt_libc_shim(CPU *c, const char *name) {
    int op = libc_op(name);
    if (op < 0) return 0;
    libc_run(c, op);
    return 1;
}

// Objets invités `environ` : reconstruits après chaque setenv, comme glibc
// qui modifie le tableau visible par le programme. Les anciennes chaînes ne
// sont pas libérées (glibc ne libère pas non plus les valeurs remplacées).
int rt_environ_sync(void) {
    if (!environ_slot_count) return 0;
    extern char **environ;
    size_t count = 0; while (environ[count]) ++count;
    uint32_t array = rt_alloc((uint32_t)((count + 1) * 4));
    if (!array) return -1;
    for (size_t i = 0; i < count; ++i) {
        uint32_t copy = rt_guest_strdup(environ[i]);
        if (!copy) return -1;
        W32(array + (uint32_t)i * 4, copy);
    }
    W32(array + (uint32_t)count * 4, 0);
    for (size_t i = 0; i < environ_slot_count; ++i) W32(environ_slots[i], array);
    return 0;
}
int rt_init_import_data(const char *name, uint32_t address, uint32_t size) {
    if (rt_map(address, size)) return -1;
    if (!strcmp(name, "stdin") || !strcmp(name, "stdout") || !strcmp(name, "stderr")) {
        int index = !strcmp(name, "stdin") ? 0 : !strcmp(name, "stdout") ? 1 : 2;
        uint32_t handle = rt_standard_stream(index);
        if (!handle) return -1;
        memset(GPTR(address), 0, size); W32(address, handle);
        return 0;
    }
    if (!strcmp(name, "environ") || !strcmp(name, "__environ") || !strcmp(name, "_environ")) {
        if (environ_slot_count == sizeof(environ_slots) / sizeof(*environ_slots)) return -1;
        environ_slots[environ_slot_count++] = address;
        return rt_environ_sync();
    }
    if (!strcmp(name, "timezone") || !strcmp(name, "daylight"))
        return rt_time_data_import(name, address);   // time.c : rafraîchi par tzset
    fprintf(stderr, "unimplemented imported object: %s\n", name);
    return -1;
}
