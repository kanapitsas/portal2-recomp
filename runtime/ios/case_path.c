// Chemins insensibles à la casse sur iOS. Le correcteur de casse de Source met
// les chemins en minuscules ; macOS (APFS insensible) les accepte, iOS non (le
// bac à sable compare la casse, et un vrai iPhone a un système de fichiers
// sensible à la casse). paths.c canonicalise déjà la racine (jeu, dépôt Linux,
// données utilisateur) ; ici chaque composant qui suit est remplacé par le nom
// réel de l'entrée du répertoire, trouvé sans tenir compte de la casse.
//
// Cache, par répertoire (chemin exact), d'une table des noms de ses entrées
// indexée sans tenir compte de la casse. Garanties :
// - un nom rendu par le cache existe au moment de la réponse (vérifié par
//   lstat) : un fichier supprimé puis recréé sous une autre casse ne renvoie
//   plus l'ancien nom ; la table du répertoire est alors reconstruite ;
// - un nom absent de la table fait relire le répertoire (fichier créé depuis
//   la mise en cache), au plus une fois par seconde, sauf après une écriture
//   du jeu (rt_paths_generation) ;
// - les chaînes rendues ne sont jamais libérées : des appelants en gardent des
//   pointeurs ; une relecture réutilise celles des noms toujours présents et
//   ne libère que le tableau d'index. Une relecture coûte O(entrées).
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE
#include <ctype.h>
#include <dirent.h>
#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <sys/stat.h>
#include <time.h>

typedef struct Directory {
    char *path;
    char **slots;          // adressage ouvert, clé : nom replié en minuscules ; NULL = case libre
    size_t capacity;       // puissance de deux, > 2 × count (la recherche s'arrête sur une case libre)
    size_t count;
    time_t refreshed;
    unsigned long generation;
    struct Directory *next;
} Directory;
unsigned long rt_paths_generation(void);   // paths.c : incrémenté à chaque écriture invitée
#define BUCKETS 4096u
static Directory *buckets[BUCKETS];
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

static unsigned hash(const char *s) { unsigned h = 2166136261u; while (*s) h = (h ^ (unsigned char)*s++) * 16777619u; return h; }
static unsigned fold_hash(const char *s) {
    unsigned h = 2166136261u;
    while (*s) h = (h ^ (unsigned char)tolower((unsigned char)*s++)) * 16777619u;   // même repli que strcasecmp
    return h;
}
// Case du nom (sans tenir compte de la casse) : celle qui le contient, sinon la première libre.
static char **slot(char **slots, size_t capacity, const char *name) {
    for (size_t i = fold_hash(name) & (capacity - 1);; i = (i + 1) & (capacity - 1))
        if (!slots[i] || !strcasecmp(slots[i], name)) return &slots[i];
}
static int grow(Directory *d) {
    size_t capacity = d->capacity ? d->capacity * 2 : 32;
    char **slots = calloc(capacity, sizeof(*slots));
    if (!slots) return 0;
    for (size_t i = 0; i < d->capacity; ++i) if (d->slots[i]) *slot(slots, capacity, d->slots[i]) = d->slots[i];
    free(d->slots);   // le tableau seulement
    d->slots = slots; d->capacity = capacity;
    return 1;
}
// 1 : inséré ; 0 : un nom de même casse repliée y est déjà (le premier lu reste) ; -1 : mémoire.
static int insert(Directory *d, char *name) {
    if ((d->count + 1) * 2 >= d->capacity && !grow(d)) return -1;
    char **s = slot(d->slots, d->capacity, name);
    if (*s) return 0;
    *s = name; d->count++;
    return 1;
}
// (Re)lit le répertoire dans une table neuve ; les chaînes des noms toujours
// présents sont réutilisées, les autres restent allouées (des appelants les
// tiennent). Rend 0 si le répertoire ne s'ouvre pas (table vide).
static int rebuild(Directory *d) {
    char **old = d->slots; size_t old_capacity = d->capacity;
    d->slots = NULL; d->capacity = d->count = 0;
    DIR *dir = opendir(d->path);
    if (dir) {
        struct dirent *entry;
        while ((entry = readdir(dir))) {
            char **previous = old ? slot(old, old_capacity, entry->d_name) : NULL;
            int reused = previous && *previous && !strcmp(*previous, entry->d_name);
            char *name = reused ? *previous : strdup(entry->d_name);
            if (!name) break;
            int status = insert(d, name);
            if (status <= 0 && !reused) free(name);
            if (status < 0) break;
        }
        closedir(dir);
    }
    free(old);
    return dir != NULL;
}

static Directory *listing(const char *path) {
    unsigned b = hash(path) % BUCKETS;
    for (Directory *d = buckets[b]; d; d = d->next) if (!strcmp(d->path, path)) return d;
    Directory *d = calloc(1, sizeof(*d));
    if (!d) return NULL;
    if (!(d->path = strdup(path))) { free(d); return NULL; }
    d->refreshed = time(NULL); d->generation = rt_paths_generation();
    if (!rebuild(d)) { free(d->slots); free(d->path); free(d); return NULL; }   // répertoire absent : rien de gardé
    d->next = buckets[b]; buckets[b] = d;
    return d;
}

static const char *find(const Directory *d, const char *name) {
    return d && d->count ? *slot(d->slots, d->capacity, name) : NULL;
}
// Relit un répertoire déjà en cache. Au plus une relecture par seconde (le
// moteur sonde beaucoup d'absents), sauf après une écriture du jeu : un fichier
// qu'il vient de créer (fichier de transition .HL1 relu aussitôt) doit être
// trouvé tout de suite ; force : nom du cache qui n'existe plus.
static void refresh(Directory *d, int force) {
    time_t now = time(NULL);
    unsigned long generation = rt_paths_generation();
    if (!force && now == d->refreshed && generation == d->generation) return;
    d->refreshed = now; d->generation = generation;
    rebuild(d);
}
static int exists(const char *directory, const char *name) {
    char path[PATH_MAX];
    struct stat info;
    return snprintf(path, sizeof(path), "%s/%s", directory, name) < (int)sizeof(path) && !lstat(path, &info);
}
static const char *match(const char *directory, const char *name) {
    pthread_mutex_lock(&lock);
    Directory *d = listing(directory);
    const char *found = find(d, name);
    if (d && found && !exists(directory, found)) { refresh(d, 1); found = find(d, name); }   // nom périmé
    else if (d && !found) { refresh(d, 0); found = find(d, name); }
    pthread_mutex_unlock(&lock);
    return found;   // les noms du cache ne sont jamais libérés
}

// canonical : chemin absolu dont les `trusted` premiers octets sont exacts.
const char *rt_ios_case_path(const char *canonical, size_t trusted) {
    static _Thread_local char ring[4][PATH_MAX];
    static _Thread_local unsigned next;
    char *out = ring[next++ & 3];
    struct stat info;
    size_t length = strlen(canonical);
    // Le résultat survit à l'appel : jamais le tampon (souvent sur la pile) de l'appelant.
    if (length >= PATH_MAX) { errno = ENAMETOOLONG; return NULL; }
    if (!lstat(canonical, &info) || trusted > length) { memcpy(out, canonical, length + 1); return out; }
    memcpy(out, canonical, trusted); out[trusted] = 0;
    const char *rest = canonical + trusted;
    int exact = 1;
    while (*rest) {
        while (*rest == '/') ++rest;
        if (!*rest) break;
        const char *end = strchr(rest, '/'); if (!end) end = rest + strlen(rest);
        char part[NAME_MAX + 1];
        size_t n = (size_t)(end - rest);
        if (n > NAME_MAX) { memcpy(out, canonical, length + 1); return out; }
        memcpy(part, rest, n); part[n] = 0;
        size_t used = strlen(out);
        if (used + 1 + n >= PATH_MAX) { memcpy(out, canonical, length + 1); return out; }
        const char *real = NULL;
        if (exact) {
            out[used] = '/'; strcpy(out + used + 1, part);
            if (lstat(out, &info)) { out[used] = 0; real = match(used ? out : "/", part); }
            else { rest = end; continue; }
        }
        if (!real) exact = 0;   // introuvable : la suite est gardée telle quelle
        out[used] = '/'; strcpy(out + used + 1, real ? real : part);
        rest = end;
    }
    return out;
}
#endif
