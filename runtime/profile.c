// Profileur intégré (diagnostic) : P2_PROFILE=/chemin.txt active un minuteur
// SIGPROF (1 ms de CPU). Chaque tic relève le compteur de programme hôte ;
// un thread écrit toutes les 5 s l'histogramme par symbole hôte (fonctions
// générées fn_<adresse invitée>, shims, bibliothèques). Rien n'est actif
// sans la variable d'environnement.
#include "rt.h"
#include <TargetConditionals.h>
#include <dlfcn.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#define SLOTS 65536u
#define DEPTH 24
typedef struct { uintptr_t pc, caller, caller2; int main; uintptr_t chain[DEPTH]; } Key;
static Key keys[SLOTS];
static uint32_t used[SLOTS];   // 0 libre, 1 en cours, 2 prêt (accès __atomic)
static _Atomic uint32_t hits[SLOTS];
static _Atomic uint64_t total, dropped;
static const char *output;

static void tick(int signal, siginfo_t *info, void *context) {
    (void)signal; (void)info;
    Key key = {0};
    uintptr_t frame = 0;
#if defined(__aarch64__)
    ucontext_t *uc = context;
    key.pc = (uintptr_t)uc->uc_mcontext->__ss.__pc;
    key.caller = (uintptr_t)uc->uc_mcontext->__ss.__lr;
    uintptr_t fp = (uintptr_t)uc->uc_mcontext->__ss.__fp;
    frame = fp;
    // Deuxième appelant : cadre courant [fp] = fp parent, [fp+8] = retour.
    if (fp && !(fp & 7)) {
        uintptr_t parent = ((uintptr_t *)fp)[0];
        if (parent > fp && parent - fp < (1u << 24) && !(parent & 7)) key.caller2 = ((uintptr_t *)parent)[1];
        else key.caller2 = ((uintptr_t *)fp)[1];
    }
#else
    (void)context;
#endif
    // Chaîne de cadres (pointeur de cadre) pour retrouver le premier cadre de
    // l'exécutable (pont GL/libc, fonction générée) sous une bibliothèque.
    for (int d = 0; d < DEPTH && frame && !(frame & 7); ++d) {
        uintptr_t next = ((uintptr_t *)frame)[0];
        key.chain[d] = ((uintptr_t *)frame)[1];
        if (next <= frame || next - frame > (1u << 24)) break;
        frame = next;
    }
    key.main = pthread_main_np();
    ++total;
    uint32_t h = (uint32_t)(((key.pc >> 2) ^ (key.caller * 31) ^ (key.caller2 * 7) ^ (uintptr_t)key.main) * 2654435761u) & (SLOTS - 1);
    for (unsigned probe = 0; probe < 64; ++probe, h = (h + 1) & (SLOTS - 1)) {
        uint32_t state = __atomic_load_n(&used[h], __ATOMIC_ACQUIRE);
        if (state == 2 && !memcmp(&keys[h], &key, sizeof(key))) { ++hits[h]; return; }
        if (state == 0) {
            uint32_t expected = 0;
            if (__atomic_compare_exchange_n(&used[h], &expected, 1, 0, __ATOMIC_ACQUIRE, __ATOMIC_RELAXED)) {
                keys[h] = key; __atomic_store_n(&used[h], 2, __ATOMIC_RELEASE); ++hits[h]; return;
            }
        }
    }
    ++dropped;
}

// Nom d'une adresse, mémorisé : dladdr est lent sur le cache partagé d'iOS
// (une écriture du profil brut prenait plus d'une minute). Sur iOS, les
// symboles du cache partagé y sont masqués (« <redacted> ») : on écrit
// « bibliothèque+décalage », résolu sur le Mac par tools/ios_symbolize.py avec
// les symboles de l'appareil (Xcode, iOS DeviceSupport). Appelé par le seul
// thread d'écriture.
#define NAMES 65536u
static const char *symbol(uintptr_t pc) {
    static const char *names[NAMES]; static uintptr_t pcs[NAMES];
    unsigned h = (unsigned)((pc >> 2) * 2654435761u) & (NAMES - 1);
    unsigned probe = 0;
    for (; probe < 32; ++probe, h = (h + 1) & (NAMES - 1)) {
        if (names[h] && pcs[h] == pc) return names[h];
        if (!names[h]) break;
    }
    const char *name = "?";
    Dl_info where;
    if (pc && dladdr((void *)pc, &where)) {
        name = where.dli_sname ? where.dli_sname : where.dli_fname;
#if TARGET_OS_IPHONE
        if (!where.dli_sname || !strcmp(where.dli_sname, "<redacted>")) {
            const char *base = strrchr(where.dli_fname, '/'); base = base ? base + 1 : where.dli_fname;
            char text[256]; snprintf(text, sizeof(text), "%s+%#lx", base, (unsigned long)(pc - (uintptr_t)where.dli_fbase));
            char *copy = strdup(text); if (copy) name = copy;
        }
#endif
    }
    if (probe < 32) { names[h] = name; pcs[h] = pc; }
    return name;
}
typedef struct { const char *name; uint64_t count; } Entry;
static int by_count(const void *a, const void *b) {
    uint64_t x = ((const Entry *)a)->count, y = ((const Entry *)b)->count;
    return x < y ? 1 : x > y ? -1 : 0;
}

static void dump_section(FILE *out, int main_only, int with_callers) {
    Entry *entries = calloc(SLOTS, sizeof(*entries));
    char (*names)[512] = calloc(SLOTS, 512);
    if (!entries || !names) { free(entries); free(names); return; }
    size_t n = 0; uint64_t section = 0;
    for (uint32_t i = 0; i < SLOTS; ++i) {
        if (__atomic_load_n(&used[i], __ATOMIC_ACQUIRE) != 2 || !hits[i]) continue;
        if (main_only && !keys[i].main) continue;
        section += hits[i];
        char name[512];
        if (with_callers == 2) {
            // Feuille, puis premier cadre de l'exécutable principal dans la chaîne.
            const char *own = "?";
            Dl_info self, where;
            dladdr((void *)&dump_section, &self);
            for (int d = 0; d < DEPTH && keys[i].chain[d]; ++d)
                if (dladdr((void *)keys[i].chain[d], &where) && where.dli_fbase == self.dli_fbase && where.dli_sname) {
                    own = where.dli_sname; break;
                }
            snprintf(name, sizeof(name), "%s ... %s", symbol(keys[i].pc), own);
        } else if (with_callers) snprintf(name, sizeof(name), "%s <- %s <- %s", symbol(keys[i].pc), symbol(keys[i].caller), symbol(keys[i].caller2));
        else snprintf(name, sizeof(name), "%s", symbol(keys[i].pc));
        size_t k = 0;
        while (k < n && strcmp(names[k], name)) ++k;
        if (k == n) { snprintf(names[n], 512, "%s", name); entries[n++] = (Entry){names[k], 0}; }
        entries[k].count += hits[i];
    }
    qsort(entries, n, sizeof(*entries), by_count);
    fprintf(out, "== %s, %s : %llu échantillons\n", main_only ? "thread principal" : "tous threads",
            with_callers == 2 ? "feuille ... premier cadre de l'exécutable" : with_callers ? "avec appelants" : "par symbole", (unsigned long long)section);
    for (size_t k = 0; k < n && k < 60; ++k)
        fprintf(out, "%6.2f%% %s\n", 100.0 * (double)entries[k].count / (double)(section ? section : 1), entries[k].name);
    free(entries); free(names);
}

// Chaînes complètes des clés les plus fréquentes du thread principal.
static void dump_chains(FILE *out) {
    uint32_t best[8] = {0}; uint32_t counts[8] = {0};
    for (uint32_t i = 0; i < SLOTS; ++i) {
        if (__atomic_load_n(&used[i], __ATOMIC_ACQUIRE) != 2 || !keys[i].main) continue;
        uint32_t h = hits[i];
        for (int k = 0; k < 8; ++k) if (h > counts[k]) {
            memmove(&counts[k + 1], &counts[k], (7 - k) * sizeof(*counts));
            memmove(&best[k + 1], &best[k], (7 - k) * sizeof(*best));
            counts[k] = h; best[k] = i; break;
        }
    }
    fprintf(out, "== piles complètes du thread principal (8 plus fréquentes)\n");
    for (int k = 0; k < 8 && counts[k]; ++k) {
        const Key *key = &keys[best[k]];
        fprintf(out, "%u : %s", counts[k], symbol(key->pc));
        for (int d = 0; d < DEPTH && key->chain[d]; ++d) fprintf(out, " <- %s", symbol(key->chain[d]));
        fputc('\n', out);
    }
}

// Tous les échantillons, une ligne par clé : « coups, principal, feuille, pile… » séparés par des tabulations
// (P2_PROFILE_RAW=1, fichier <P2_PROFILE>.raw), pour une agrégation hors ligne
// (tools/ios_symbolize.py résout les « bibliothèque+décalage » d'iOS).
static void dump_raw(void) {
    char path[1100], temporary[1110];
    snprintf(path, sizeof(path), "%s.raw", output);
    snprintf(temporary, sizeof(temporary), "%s.tmp", path);
    FILE *out = fopen(temporary, "w");
    if (!out) return;
    for (uint32_t i = 0; i < SLOTS; ++i) {
        if (__atomic_load_n(&used[i], __ATOMIC_ACQUIRE) != 2 || !hits[i]) continue;
        const Key *key = &keys[i];
        fprintf(out, "%u\t%d\t%s", (unsigned)hits[i], key->main, symbol(key->pc));
        for (int d = 0; d < DEPTH && key->chain[d]; ++d) fprintf(out, "\t%s", symbol(key->chain[d]));
        fputc('\n', out);
    }
    fclose(out);
    rename(temporary, path);
}

// P2_PROFILE_ORDER=/chemin.txt : chaque adresse de l'exécutable vue dans les
// échantillons (feuille ou pile), « décalage propre inclusif » par ligne
// (décalage depuis le début de l'image, en hexadécimal), à résoudre hors ligne
// avec nm : sert à construire un fichier d'ordre de l'éditeur de liens.
static int wall_mode;
enum { ORDER_SLOTS = 1u << 20 };
typedef struct { uintptr_t pc; uint64_t self, inclusive; } OrderEntry;
static void dump_order(const char *path) {
    Dl_info self;
    if (!dladdr((void *)dump_order, &self)) return;
    uintptr_t base = (uintptr_t)self.dli_fbase;
    static OrderEntry *table;
    if (!table && !(table = malloc(ORDER_SLOTS * sizeof(*table)))) return;
    memset(table, 0, ORDER_SLOTS * sizeof(*table));
    for (uint32_t i = 0; i < SLOTS; ++i) {
        if (__atomic_load_n(&used[i], __ATOMIC_ACQUIRE) != 2) continue;
        uint32_t h = hits[i];
        for (int d = -1; d < DEPTH; ++d) {
            uintptr_t pc = d < 0 ? keys[i].pc : keys[i].chain[d];
            if (!pc) break;
            uint32_t s = (uint32_t)((pc >> 2) * 2654435761u) & (ORDER_SLOTS - 1);
            for (unsigned probe = 0; probe < 256 && table[s].pc && table[s].pc != pc; ++probe) s = (s + 1) & (ORDER_SLOTS - 1);
            if (table[s].pc && table[s].pc != pc) continue;
            table[s].pc = pc;
            if (d < 0) table[s].self += h;
            table[s].inclusive += h;
        }
    }
    char temporary[1100];
    snprintf(temporary, sizeof(temporary), "%s.tmp", path);
    FILE *out = fopen(temporary, "w");
    if (!out) return;
    for (uint32_t i = 0; i < ORDER_SLOTS; ++i)
        if (table[i].pc > base)
            fprintf(out, "%lx %llu %llu\n", (unsigned long)(table[i].pc - base), (unsigned long long)table[i].self,
                    (unsigned long long)table[i].inclusive);
    fclose(out);
    rename(temporary, path);
}

static void dump(void) {
    static int raw = -1;
    if (raw < 0) raw = getenv("P2_PROFILE_RAW") != NULL;
    if (raw) dump_raw();
    // Liste brute d'abord (linéaire) : en mode temps réel, les sections
    // ci-dessous peuvent prendre très longtemps (dédoublonnage quadratique).
    const char *order = getenv("P2_PROFILE_ORDER");
    if (order && *order) { dump_order(order); if (wall_mode) return; }
    char temporary[1100];
    snprintf(temporary, sizeof(temporary), "%s.tmp", output);
    FILE *out = fopen(temporary, "w");
    if (!out) return;
    fprintf(out, "samples %llu (dropped %llu)\n", (unsigned long long)total, (unsigned long long)dropped);
    dump_section(out, 1, 0);
    dump_section(out, 1, 1);
    dump_section(out, 1, 2);
    dump_chains(out);
    dump_section(out, 0, 0);
    dump_section(out, 0, 1);
    fclose(out);
    rename(temporary, output);
}

// P2_PROFILE_DELAY=<s> : l'échantillonnage ne commence qu'après s secondes
// (régime établi, sans le chargement de la carte).
static pthread_t main_thread;
static void *wall_sampler(void *unused);
static void start_sampling(void) {
    if (wall_mode) {
        pthread_t thread;
        pthread_create(&thread, NULL, wall_sampler, NULL);
        pthread_detach(thread);
    } else {
        struct itimerval timer = {{0, 1000}, {0, 1000}};
        setitimer(ITIMER_PROF, &timer, NULL);
    }
}

static void *writer(void *unused) {
    (void)unused;
    const char *delay = getenv("P2_PROFILE_DELAY");
    if (delay && atoi(delay) > 0) { sleep((unsigned)atoi(delay)); start_sampling(); }
    for (;;) { sleep(5); dump(); }
    return NULL;
}

// P2_PROFILE_WALL=1 : échantillonne le thread principal en temps réel (même
// bloqué) en lui envoyant SIGPROF toutes les millisecondes ; montre où il attend.
static void *wall_sampler(void *unused) {
    (void)unused;
    struct timespec period = {0, 1000000};
    for (;;) { nanosleep(&period, NULL); pthread_kill(main_thread, SIGPROF); }
    return NULL;
}

__attribute__((constructor)) static void start_profiler(void) {
    output = getenv("P2_PROFILE");
    if (!output || !*output) return;
#if TARGET_OS_IPHONE
    // iOS : chemin relatif au conteneur de l'app (HOME), comme P2_LOG_FILE.
    static char path[1024];
    if (*output != '/' && getenv("HOME")) { snprintf(path, sizeof(path), "%s/%s", getenv("HOME"), output); output = path; }
#endif
    struct sigaction action = {0};
    action.sa_sigaction = tick;
    action.sa_flags = SA_SIGINFO | SA_RESTART;
    sigemptyset(&action.sa_mask);
    sigaction(SIGPROF, &action, NULL);
    wall_mode = getenv("P2_PROFILE_WALL") != NULL;
    main_thread = pthread_self();   // constructeur : thread principal
    const char *delay = getenv("P2_PROFILE_DELAY");
    if (!delay || atoi(delay) <= 0) start_sampling();
    pthread_t thread;
    pthread_create(&thread, NULL, writer, NULL);
    pthread_detach(thread);
}

#ifdef RT_PGO_GENERATE
// Version --pgo-generate : les parties de diagnostic sont arrêtées au délai
// (SIGKILL), sans passer par exit où LLVM écrit son profil ; on l'écrit donc
// toutes les 20 s (fichier choisi par LLVM_PROFILE_FILE).
int __llvm_profile_write_file(void);
static void *pgo_writer(void *unused) {
    (void)unused;
    for (;;) { sleep(20); __llvm_profile_write_file(); }
    return NULL;
}
__attribute__((constructor)) static void start_pgo_writer(void) {
    pthread_t thread;
    if (!pthread_create(&thread, NULL, pgo_writer, NULL)) pthread_detach(thread);
}
#endif
