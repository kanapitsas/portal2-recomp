// Pont OpenGL : libtogl obtient ses fonctions GL par SDL_GL_GetProcAddress,
// qui délègue à rt_gl_proc_address. Chaque fonction prise en charge reçoit
// une adresse invitée (rt_dynamic_import) ; l'appel invité arrive dans
// gl_shim, qui exécute le thunk de runtime/gl_generated.c (généré par
// tools/gen_gl.py) ou une fonction manuelle ci-dessous.
//
// Comme SDL sous macOS, une adresse est rendue pour tout symbole exporté par
// OpenGL.framework, même si le contexte courant (legacy 2.1) ne le prend pas
// en charge : l'appel répond alors GL_INVALID_OPERATION, comme sur un vrai
// Mac. Voir docs/gl-audit.md.
//
// Pointeurs invités : la mémoire invitée est projetée dans l'espace hôte
// (M + adresse), donc un pointeur invité validé devient GPTR(adresse) et
// reste valide après l'appel (tableaux de sommets clients, client storage).
// Seuls les pointeurs hôtes renvoyés par GL (glMapBuffer, glGetString,
// GLsync) demandent une copie ou un handle.
#include <stdio.h>
#include <mach/mach_time.h>
#include "rt.h"
#include <stdarg.h>
#include "host_gl.h"   // OpenGL.framework (macOS) ou gl4es (iOS)
#include <dlfcn.h>
#include <pthread.h>
#include <stdlib.h>

typedef int (*rt_gl_thunk)(CPU *c, void *fn);
typedef struct { const char *name; rt_gl_thunk thunk; } RtGlEntry;
extern const RtGlEntry rt_gl_entries[];
extern const unsigned rt_gl_entry_count;

#define GL_SYNC_KIND 0x474c5359u  // 'GLSY'
// ARB_map_buffer_range (gl3.h, absent de glext.h).
#define GL_MAP_READ_BIT 0x0001
#define GL_MAP_WRITE_BIT 0x0002
#define GL_MAP_INVALIDATE_RANGE_BIT 0x0004
#define GL_MAP_INVALIDATE_BUFFER_BIT 0x0008
#define GL_MAP_FLUSH_EXPLICIT_BIT 0x0010

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_once_t once = PTHREAD_ONCE_INIT;
static void *library;
static void **hosts;  // pointeur hôte par entrée, résolu à la demande

static void open_library(void) {
    library = rt_host_gl_open();
    hosts = calloc(rt_gl_entry_count, sizeof(*hosts));
}

static int compare_entry(const void *key, const void *entry) {
    return strcmp(key, ((const RtGlEntry *)entry)->name);
}
static const RtGlEntry *find_entry(const char *name) {
    if (!name || name[0] != 'g' || name[1] != 'l') return NULL;
    return bsearch(name, rt_gl_entries, rt_gl_entry_count, sizeof(*rt_gl_entries), compare_entry);
}
static void *host_function(const RtGlEntry *entry) {
    pthread_once(&once, open_library);
    if (!library || !hosts) return NULL;
    size_t index = (size_t)(entry - rt_gl_entries);
    void *fn = __atomic_load_n(&hosts[index], __ATOMIC_ACQUIRE);
    if (!fn) {
        fn = rt_host_gl_symbol(library, entry->name);
        __atomic_store_n(&hosts[index], fn, __ATOMIC_RELEASE);
    }
    return fn;
}

// ---- GLX minimal sur CGL ----
// togl ne se sert de GLX que pour un repli : quand une extension manque dans
// GL_EXTENSIONS, il la cherche dans glXQueryExtensionsString(glXGetCurrentDisplay(), 0)
// et exige ces deux fonctions. macOS n'a ni X11 ni GLX : le « display » est un
// handle opaque (jamais déréférençable) et la liste d'extensions GLX est vide,
// ce qui est exact. Toute autre fonction glX* reste absente.
#define GLX_DISPLAY_KIND 0x474c5844u  // 'GLXD'
static int is_glx(const char *name) {
    return name && (!strcmp(name, "glXGetCurrentDisplay") || !strcmp(name, "glXQueryExtensionsString"));
}
static uint32_t glx_display;
static uint32_t glx_empty;
static int glx_shim(CPU *c, const char *name) {
    pthread_mutex_lock(&lock);
    if (!glx_display) {
        static int marker;
        glx_display = rt_handle_new(&marker, GLX_DISPLAY_KIND);
        glx_empty = rt_alloc(1);
        if (glx_empty) W8(glx_empty, 0);
    }
    uint32_t display = glx_display, empty = glx_empty;
    pthread_mutex_unlock(&lock);
    if (!display || !empty) rt_trap(c, c->eip, "GLX : allocation impossible");
    if (!strcmp(name, "glXGetCurrentDisplay")) {
        c->eax = CGLGetCurrentContext() ? display : 0;  // NULL sans contexte courant, comme GLX
    } else {
        uint32_t dpy = R32(c->esp + 4);
        if (!rt_handle_get(dpy, GLX_DISPLAY_KIND)) rt_trap(c, c->eip, "glXQueryExtensionsString : display inconnu");
        c->eax = empty;
    }
    return 1;
}

uint32_t rt_gl_proc_address(const char *name) {
    if (is_glx(name)) return rt_dynamic_import(name);
    const RtGlEntry *entry = find_entry(name);
    return entry && host_function(entry) ? rt_dynamic_import(name) : 0;
}

static int gl_execute(CPU *c, const char *name, const RtGlEntry *entry, void *fn);
static int gl_shim(CPU *c, const char *name) {
    if (is_glx(name)) return glx_shim(c, name);
    const RtGlEntry *entry = find_entry(name);
    if (!entry) return 0;
    void *fn = host_function(entry);
    if (!fn) rt_trap(c, c->eip, "fonction GL absente de OpenGL.framework");
    // Sans contexte courant, le GL d'Apple déréférence un contexte nul ; la libGL
    // Linux, elle, aiguille vers une table « sans contexte » qui ne fait rien et
    // rend 0. togl en fait à l'arrêt (Cmd-Q) après destruction du contexte :
    // même comportement que Linux, signalé une fois.
    if (!CGLGetCurrentContext()) {
        static _Atomic int reported;
        if (!reported++) fprintf(stderr, "gl: %s sans contexte courant ignoré (comme la libGL Linux)\n", name);
        c->eax = 0; c->edx = 0;
        return 1;
    }
    return gl_execute(c, name, entry, fn);
}
// Résolution unique par adresse d'import : l'aiguillage rappelle ensuite
// directement gl_call avec l'entrée, sans comparaison GLX ni recherche
// dichotomique parmi ~180 noms à chaque appel (des millions par seconde).
static int stream_enabled(void);
// Rôle de chaque fonction GL pour le suivi des tampons, calculé une fois par
// entrée (pas de comparaison de noms à chaque appel).
enum { GL_KIND_OTHER = 1, GL_KIND_SUBDATA, GL_KIND_DATA, GL_KIND_MAP, GL_KIND_COPY_BUFFER, GL_KIND_PACK_READ };
static int stream_before(CPU *c, int kind);
static void stream_after(CPU *c, int kind);

// Diagnostic P2_GL_TIMING=1 : temps réel cumulé par fonction GL (et échange
// d'images, sdl_video.c), affiché toutes les 5 s, sans signaux.
static _Atomic uint64_t *gl_ticks, *gl_counts;
static uint64_t gl_timing_last, gl_timing_start;
void rt_gl_timing_add(const char *label, uint64_t ticks);
static void gl_timing_report(void) {
    static mach_timebase_info_data_t base;
    if (!base.denom) mach_timebase_info(&base);
    uint64_t now = mach_absolute_time();
    if (!gl_timing_start) { gl_timing_start = gl_timing_last = now; return; }
    if ((now - gl_timing_last) * base.numer / base.denom < 5000000000ull) return;
    gl_timing_last = now;
    double wall = (double)(now - gl_timing_start) * base.numer / base.denom / 1e9;
    unsigned order[16] = {0}; uint64_t best[16] = {0};
    for (unsigned i = 0; i < rt_gl_entry_count + 1; ++i) {
        uint64_t t = gl_ticks[i];
        for (int k = 0; k < 16; ++k) if (t > best[k]) {
            memmove(&best[k + 1], &best[k], (15 - k) * sizeof(*best));
            memmove(&order[k + 1], &order[k], (15 - k) * sizeof(*order));
            best[k] = t; order[k] = i; break;
        }
    }
    fprintf(stderr, "[gltime] sur %.0f s :", wall);
    for (int k = 0; k < 12 && best[k]; ++k) {
        unsigned i = order[k];
        double seconds = (double)best[k] * base.numer / base.denom / 1e9;
        fprintf(stderr, " %s %.1f%% (%llu)", i == rt_gl_entry_count ? "SwapWindow" : rt_gl_entries[i].name,
                100.0 * seconds / wall, (unsigned long long)gl_counts[i]);
    }
    fputc('\n', stderr);
}
static int gl_timing(void) {
    static int value = -1;
    if (value < 0) {
        value = getenv("P2_GL_TIMING") != NULL;
        if (value) { gl_ticks = calloc(rt_gl_entry_count + 1, 8); gl_counts = calloc(rt_gl_entry_count + 1, 8); }
    }
    return value && gl_ticks && gl_counts;
}
void rt_gl_timing_add(const char *label, uint64_t ticks) {   // échange d'images (sdl_video.c)
    (void)label;
    if (!gl_timing()) return;
    gl_ticks[rt_gl_entry_count] += ticks; ++gl_counts[rt_gl_entry_count];
    gl_timing_report();
}
static int gl_execute(CPU *c, const char *name, const RtGlEntry *entry, void *fn);
static int gl_call(CPU *c, const char *name, void *data) {
    const RtGlEntry *entry = data;
    void *fn = host_function(entry);
    if (!fn) rt_trap(c, c->eip, "fonction GL absente de OpenGL.framework");
    if (!CGLGetCurrentContext()) return gl_shim(c, name);   // chemin rare : même traitement que sans résolution
    return gl_execute(c, name, entry, fn);
}
// Seul chemin d'exécution d'un thunk GL, premier appel compris (avant que
// l'aiguillage ait mémorisé gl_call) : suivi des tampons et chronométrage.
// Diagnostic P2_BLIT_TRACE=<n> : décrit 300 copies glBlitFramebuffer à partir
// de la n-ième, avec le nombre de dessins faits dans la surface source (suivie
// à travers les FBO par ses attachements) depuis la copie précédente.
enum { TRACE_FBS = 4096 };
static uint32_t trace_attach[TRACE_FBS];        // fb -> objet en COLOR_ATTACHMENT0
static uint32_t trace_draw_fb, trace_read_fb;
static long trace_writes[2][TRACE_FBS];         // [renderbuffer|texture][nom] -> dessins
static long trace_seen_writes[2][TRACE_FBS];
static long blit_trace_start = -2;
// Vrais dessins (glDrawArrays*, glDrawElements*, glDrawRangeElements*), pas glDrawBuffer(s).
static int is_draw_call(const char *name) {
    return !strncmp(name, "glDrawArrays", 12) || !strncmp(name, "glDrawElements", 14) || !strncmp(name, "glDrawRangeElements", 19);
}
static long *trace_counter(uint32_t object) {
    uint32_t kind = object >> 31, name = object & 0x7fffffff;
    return name < TRACE_FBS ? &trace_writes[kind][name] : NULL;
}
static void trace_gl(CPU *c, const char *name) {
    if (blit_trace_start == -2) { const char *e = getenv("P2_BLIT_TRACE"); blit_trace_start = e ? atol(e) : -1; }
    if (blit_trace_start < 0) return;
    uint32_t a = c->esp + 4;
    if (!strncmp(name, "glBindFramebuffer", 17)) {
        uint32_t target = R32(a), fb = R32(a + 4);
        if (target != GL_READ_FRAMEBUFFER) trace_draw_fb = fb;
        if (target != GL_DRAW_FRAMEBUFFER) trace_read_fb = fb;
    } else if (!strncmp(name, "glFramebufferRenderbuffer", 25) || !strncmp(name, "glFramebufferTexture", 20)) {
        uint32_t target = R32(a), attachment = R32(a + 4);
        int texture = name[13] == 'T';
        uint32_t object = texture ? R32(a + 12) : R32(a + 12);   // (target, attachment, rbtarget|textarget, nom, ...)
        if (!strcmp(name, "glFramebufferTexture") || !strcmp(name, "glFramebufferTextureEXT")) object = R32(a + 8);
        uint32_t fb = target == GL_READ_FRAMEBUFFER ? trace_read_fb : trace_draw_fb;
        if (attachment == GL_COLOR_ATTACHMENT0 && fb < TRACE_FBS) trace_attach[fb] = object | (texture ? 0x80000000u : 0);
    } else if (is_draw_call(name) || !strcmp(name, "glClear")) {
        long *counter = trace_draw_fb < TRACE_FBS && trace_attach[trace_draw_fb] ? trace_counter(trace_attach[trace_draw_fb]) : NULL;
        if (counter) ++*counter;
    } else if (!strncmp(name, "glBlitFramebuffer", 17)) {
        static long seen;
        long index = seen++;
        uint32_t source = trace_read_fb < TRACE_FBS ? trace_attach[trace_read_fb] : 0;
        long *counter = source ? trace_counter(source) : NULL;
        long writes = counter ? *counter - trace_seen_writes[source >> 31][source & 0x7fffffff] : -1;
        if (counter) trace_seen_writes[source >> 31][source & 0x7fffffff] = *counter;
        if (index < blit_trace_start || index >= blit_trace_start + 300) return;
        uint32_t target = trace_draw_fb < TRACE_FBS ? trace_attach[trace_draw_fb] : 0;
        fprintf(stderr, "[blit] #%ld %s%u (%ld dessins depuis) -> fb %u %s%u src %d,%d-%d,%d dst %d,%d-%d,%d mask %#x filter %#x\n",
                index, source >> 31 ? "tex" : "rb", source & 0x7fffffff, writes, trace_draw_fb,
                target >> 31 ? "tex" : "rb", target & 0x7fffffff, (int)R32(a), (int)R32(a + 4), (int)R32(a + 8),
                (int)R32(a + 12), (int)R32(a + 16), (int)R32(a + 20), (int)R32(a + 24), (int)R32(a + 28), R32(a + 32), R32(a + 36));
        if (target && trace_counter(target)) ++*trace_counter(target);
    }
}
// Diagnostic P2_PASS_TRACE=<s> : décrit la première image rendue après s
// secondes, passe par passe (une passe = suite d'appels vers la même cible
// de dessin) : cible, zone de dessin, effacements, dessins, copies. Sert à
// repérer ce qui coûte au GPU (sur un GPU à tuiles, chaque changement de
// cible relit et réécrit les attachements en mémoire).
static int pass_state = -1;   // -1 inconnu, 0 inactif, 1 attente, 2 image en cours, 3 fini
static uint64_t pass_deadline;
static uint32_t pass_fb, pass_vw, pass_vh, pass_clears, pass_clear_mask;
static long pass_draws, pass_index;
static char *pass_log; static size_t pass_len, pass_cap;
static void pass_printf(const char *format, ...) __attribute__((format(printf, 1, 2)));
static void pass_printf(const char *format, ...) {
    if (pass_len + 512 > pass_cap) {
        size_t cap = pass_cap ? pass_cap * 2 : 1 << 16;
        char *grown = realloc(pass_log, cap);
        if (!grown) return;
        pass_log = grown; pass_cap = cap;
    }
    va_list list; va_start(list, format);
    int n = vsnprintf(pass_log + pass_len, pass_cap - pass_len, format, list);
    va_end(list);
    if (n > 0) pass_len += (size_t)n < pass_cap - pass_len ? (size_t)n : pass_cap - pass_len - 1;
}
static void pass_close(void) {
    if (pass_draws || pass_clears)
        pass_printf("passe %ld : fb %u, zone %ux%u, %ld dessins, %u effacements (masque %#x)\n",
                    pass_index++, pass_fb, pass_vw, pass_vh, pass_draws, pass_clears, pass_clear_mask);
    pass_draws = 0; pass_clears = 0; pass_clear_mask = 0;
}
// Appelé à chaque échange d'images (sdl_video.c).
void rt_gl_pass_frame(void) {
    if (pass_state == -1) {
        const char *e = getenv("P2_PASS_TRACE");
        pass_state = e && *e ? 1 : 0;
        mach_timebase_info_data_t base;
        mach_timebase_info(&base);
        if (pass_state) pass_deadline = mach_absolute_time() + (uint64_t)atol(e) * 1000000000ull * base.denom / base.numer;
    }
    if (pass_state == 1 && mach_absolute_time() >= pass_deadline) { pass_state = 2; pass_index = 0; pass_len = 0; }
    else if (pass_state == 2) {
        pass_close();
        fprintf(stderr, "[passes] image complète :\n%.*s[passes] fin\n", (int)pass_len, pass_log ? pass_log : "");
        pass_state = 3;
    }
}
static void pass_trace(CPU *c, const char *name) {
    uint32_t a = c->esp + 4;
    if (!strncmp(name, "glBindFramebuffer", 17)) {
        uint32_t target = R32(a), fb = R32(a + 4);
        if (target != GL_READ_FRAMEBUFFER && fb != pass_fb) { pass_close(); pass_fb = fb; }
    } else if (!strncmp(name, "glFramebufferTexture", 20) || !strncmp(name, "glFramebufferRenderbuffer", 25)) {
        pass_printf("  attache %s %#x -> objet %u\n", name + 13, R32(a + 4),
                    !strncmp(name, "glFramebufferTexture2D", 22) || name[13] == 'R' ? R32(a + 12) : R32(a + 8));
    } else if (!strcmp(name, "glViewport")) {
        pass_vw = R32(a + 8); pass_vh = R32(a + 12);
    } else if (!strcmp(name, "glClear")) {
        ++pass_clears; pass_clear_mask |= R32(a);
    } else if (is_draw_call(name)) {
        ++pass_draws;
    } else if (!strncmp(name, "glBlitFramebuffer", 17)) {
        pass_close();
        pass_printf("  copie blit src %d,%d-%d,%d -> dst %d,%d-%d,%d masque %#x filtre %#x\n", (int)R32(a), (int)R32(a + 4),
                    (int)R32(a + 8), (int)R32(a + 12), (int)R32(a + 16), (int)R32(a + 20), (int)R32(a + 24),
                    (int)R32(a + 28), R32(a + 32), R32(a + 36));
    } else if (!strncmp(name, "glCopyTex", 9)) {
        pass_close();
        pass_printf("  %s %ux%u\n", name, R32(a + 24), R32(a + 28));
    } else if (!strncmp(name, "glGenerateMipmap", 16) || !strcmp(name, "glReadPixels") || !strncmp(name, "glGetQueryObject", 16)
               || !strcmp(name, "glFinish") || !strcmp(name, "glFlush") || !strncmp(name, "glClientWaitSync", 16)) {
        pass_printf("  %s\n", name);
    }
}

static int gl_execute(CPU *c, const char *name, const RtGlEntry *entry, void *fn) {
    static uint8_t *kinds;
    if (__builtin_expect(!kinds, 0)) kinds = calloc(rt_gl_entry_count, 1);
    size_t index = (size_t)(entry - rt_gl_entries);
    int kind = kinds ? kinds[index] : 0;
    if (__builtin_expect(!kind, 0)) {
        kind = !strcmp(name, "glBufferSubData") || !strcmp(name, "glBufferSubDataARB") ? GL_KIND_SUBDATA
             : !strcmp(name, "glBufferData") || !strcmp(name, "glBufferDataARB") ? GL_KIND_DATA
             : !strncmp(name, "glMapBuffer", 11) ? GL_KIND_MAP
             : !strcmp(name, "glCopyBufferSubData") ? GL_KIND_COPY_BUFFER
             : !strcmp(name, "glReadPixels") || !strcmp(name, "glGetTexImage") || !strncmp(name, "glGetCompressedTexImage", 23)
                   ? GL_KIND_PACK_READ : GL_KIND_OTHER;
        if (kinds) kinds[index] = (uint8_t)kind;
    }
    if (kind != GL_KIND_OTHER && stream_enabled() && stream_before(c, kind)) return 1;
    if (__builtin_expect(blit_trace_start != -1, 0)) trace_gl(c, name);
    if (__builtin_expect(pass_state == 2, 0)) pass_trace(c, name);
    int handled;
    if (__builtin_expect(!gl_timing(), 1)) handled = entry->thunk(c, fn);
    else {
        uint64_t start = mach_absolute_time();
        handled = entry->thunk(c, fn);
        gl_ticks[index] += mach_absolute_time() - start; ++gl_counts[index];
    }
    if (kind == GL_KIND_DATA && stream_enabled()) stream_after(c, kind);
    return handled;
}
static int gl_resolve(const char *name, RtShimRoute *route) {
    if (is_glx(name)) return 0;
    const RtGlEntry *entry = find_entry(name);
    if (!entry) return 0;
    route->handler = gl_call;
    route->data = (void *)entry;
    return 1;
}
RT_SHIM_PROVIDER_RESOLVER(gl_shim, gl_resolve)

// ---- Arguments i386 cdecl ----
uint32_t rt_gl_u32(CPU *c, unsigned slot) { return R32(c->esp + 4 + 4 * slot); }
float rt_gl_f32(CPU *c, unsigned slot) { return RF(c->esp + 4 + 4 * slot); }
double rt_gl_f64(CPU *c, unsigned slot) { return RD(c->esp + 4 + 4 * slot); }
uint64_t rt_gl_u64(CPU *c, unsigned slot) { return R64(c->esp + 4 + 4 * slot); }
uint64_t rt_gl_n(int64_t n) { return n < 0 ? 0 : (uint64_t)n; }
void *rt_gl_handle_arg(uint32_t handle) { return (void *)(uintptr_t)handle; }
uint32_t rt_gl_handle_ret(CPU *c, void *handle) {
    // GLhandleARB vaut void * sur macOS, unsigned int sur Linux.
    if ((uintptr_t)handle > UINT32_MAX) rt_trap(c, c->eip, "GLhandleARB hôte hors 32 bits");
    return (uint32_t)(uintptr_t)handle;
}
uint32_t rt_gl_iptr_ret(CPU *c, intptr_t value) {
    if (value < INT32_MIN || value > INT32_MAX) rt_trap(c, c->eip, "GLintptr hôte hors 32 bits");
    return (uint32_t)(int32_t)value;
}

// ---- Pointeurs invités ----
void *rt_gl_in(CPU *c, uint32_t ptr, uint64_t size, int nullable) {
    if (!ptr) {
        if (nullable || !size) return NULL;
        rt_trap(c, c->eip, "pointeur GL nul");
    }
    return rt_span(c, ptr, size);
}
void *rt_gl_out(CPU *c, uint32_t ptr, uint64_t size, int nullable) {
    if (!ptr) {
        if (nullable || !size) return NULL;
        rt_trap(c, c->eip, "pointeur GL de sortie nul");
    }
    return rt_write_span(c, ptr, size);
}
const char *rt_gl_str(CPU *c, uint32_t ptr) { rt_string_length(c, ptr); return GPTR(ptr); }

static GLint integer(GLenum pname) { GLint v = 0; glGetIntegerv(pname, &v); return v; }

// Pointeur de tableau de sommets : décalage dans le VBO lié, sinon tableau
// client en mémoire invitée (lu par GL au moment du tracé, taille inconnue ici).
const void *rt_gl_array(CPU *c, uint32_t ptr) {
    (void)c;
    if (integer(GL_ARRAY_BUFFER_BINDING)) return (const void *)(uintptr_t)ptr;
    return ptr ? GPTR(ptr) : NULL;
}

const void *rt_gl_indices(CPU *c, uint32_t ptr, int32_t count, uint32_t type) {
    if (integer(GL_ELEMENT_ARRAY_BUFFER_BINDING)) return (const void *)(uintptr_t)ptr;
    uint64_t unit = type == GL_UNSIGNED_BYTE ? 1 : type == GL_UNSIGNED_SHORT ? 2 : type == GL_UNSIGNED_INT ? 4 : 0;
    if (!unit) return ptr ? GPTR(ptr) : NULL;  // type invalide : GL refuse sans lire
    return rt_gl_in(c, ptr, rt_gl_n(count) * unit, 0);
}

// Octets par pixel d'un couple format/type (0 : inconnu du pont).
static uint64_t pixel_bytes(uint32_t format, uint32_t type) {
    switch (type) {
    case GL_UNSIGNED_BYTE_3_3_2: case GL_UNSIGNED_BYTE_2_3_3_REV: return 1;
    case GL_UNSIGNED_SHORT_5_6_5: case GL_UNSIGNED_SHORT_5_6_5_REV:
    case GL_UNSIGNED_SHORT_4_4_4_4: case GL_UNSIGNED_SHORT_4_4_4_4_REV:
    case GL_UNSIGNED_SHORT_5_5_5_1: case GL_UNSIGNED_SHORT_1_5_5_5_REV:
    case GL_UNSIGNED_SHORT_8_8_APPLE: case GL_UNSIGNED_SHORT_8_8_REV_APPLE: return 2;
    case GL_UNSIGNED_INT_8_8_8_8: case GL_UNSIGNED_INT_8_8_8_8_REV:
    case GL_UNSIGNED_INT_10_10_10_2: case GL_UNSIGNED_INT_2_10_10_10_REV:
    case GL_UNSIGNED_INT_24_8_EXT: case GL_UNSIGNED_INT_10F_11F_11F_REV_EXT:
    case GL_UNSIGNED_INT_5_9_9_9_REV_EXT: return 4;
    case GL_FLOAT_32_UNSIGNED_INT_24_8_REV: return 8;
    }
    uint64_t size;
    switch (type) {
    case GL_UNSIGNED_BYTE: case GL_BYTE: size = 1; break;
    case GL_UNSIGNED_SHORT: case GL_SHORT: case GL_HALF_FLOAT_ARB: size = 2; break;
    case GL_UNSIGNED_INT: case GL_INT: case GL_FLOAT: size = 4; break;
    default: return 0;
    }
    switch (format) {
    case GL_RED: case GL_GREEN: case GL_BLUE: case GL_ALPHA: case GL_LUMINANCE:
    case GL_DEPTH_COMPONENT: case GL_STENCIL_INDEX: case GL_COLOR_INDEX: return size;
    case GL_LUMINANCE_ALPHA: case GL_RG: return 2 * size;
    case GL_RGB: case GL_BGR: return 3 * size;
    case GL_RGBA: case GL_BGRA: case GL_ABGR_EXT: return 4 * size;
    }
    return 0;
}

typedef struct { GLenum row_length, skip_rows, skip_pixels, alignment, image_height, skip_images, row_bytes, binding; } PixelStore;
static const PixelStore unpack_store = {GL_UNPACK_ROW_LENGTH, GL_UNPACK_SKIP_ROWS, GL_UNPACK_SKIP_PIXELS, GL_UNPACK_ALIGNMENT,
                                        GL_UNPACK_IMAGE_HEIGHT, GL_UNPACK_SKIP_IMAGES, GL_UNPACK_ROW_BYTES_APPLE, GL_PIXEL_UNPACK_BUFFER_BINDING};
static const PixelStore pack_store = {GL_PACK_ROW_LENGTH, GL_PACK_SKIP_ROWS, GL_PACK_SKIP_PIXELS, GL_PACK_ALIGNMENT,
                                      GL_PACK_IMAGE_HEIGHT, GL_PACK_SKIP_IMAGES, GL_PACK_ROW_BYTES_APPLE, GL_PIXEL_PACK_BUFFER_BINDING};

// Taille lue/écrite par GL pour une image w×h×d (d = 0 : image 2D, sans
// SKIP_IMAGES ni IMAGE_HEIGHT), selon l'état de stockage des pixels.
static uint64_t image_bytes(CPU *c, const PixelStore *s, int32_t w, int32_t h, int32_t d, uint32_t format, uint32_t type) {
    int is3d = d != 0;
    if (!is3d) d = 1;
    if (w <= 0 || h <= 0 || d <= 0) return 0;
    uint64_t pixel = pixel_bytes(format, type);
    if (!pixel) rt_trap(c, c->eip, "format/type de pixels GL non pris en charge par le pont");
    // État de stockage et dimensions viennent de l'invité : arithmétique vérifiée,
    // un débordement rendrait la validation du pointeur inopérante.
    #define CHECKED(op, a, b, out) do { if (__builtin_##op##_overflow((a), (b), (out))) \
        rt_trap(c, c->eip, "taille d'image GL hors limites"); } while (0)
    uint64_t row_length = integer(s->row_length) > 0 ? (uint64_t)integer(s->row_length) : (uint64_t)w;
    uint64_t alignment = integer(s->alignment) > 0 ? (uint64_t)integer(s->alignment) : 1;
    uint64_t row, image, start = 0, part, total;
    CHECKED(mul, row_length, pixel, &row);
    row = (row + alignment - 1) / alignment * alignment;
    if (integer(s->row_bytes) > 0) row = (uint64_t)integer(s->row_bytes);  // APPLE_row_bytes
    uint64_t rows = integer(s->image_height) > 0 && is3d ? (uint64_t)integer(s->image_height) : (uint64_t)h;
    CHECKED(mul, rows, row, &image);
    uint64_t skip_rows = integer(s->skip_rows) > 0 ? (uint64_t)integer(s->skip_rows) : 0;
    uint64_t skip_pixels = integer(s->skip_pixels) > 0 ? (uint64_t)integer(s->skip_pixels) : 0;
    uint64_t skip_images = is3d && integer(s->skip_images) > 0 ? (uint64_t)integer(s->skip_images) : 0;
    CHECKED(mul, skip_rows, row, &part); CHECKED(add, start, part, &start);
    CHECKED(mul, skip_pixels, pixel, &part); CHECKED(add, start, part, &start);
    CHECKED(mul, skip_images, image, &part); CHECKED(add, start, part, &start);
    CHECKED(mul, (uint64_t)(d - 1), image, &part); CHECKED(add, start, part, &total);
    CHECKED(mul, (uint64_t)(h - 1), row, &part); CHECKED(add, total, part, &total);
    CHECKED(mul, (uint64_t)w, pixel, &part); CHECKED(add, total, part, &total);
    #undef CHECKED
    return total;
}

const void *rt_gl_unpack(CPU *c, uint32_t ptr, int32_t w, int32_t h, int32_t d, uint32_t format, uint32_t type, int nullable) {
    if (integer(GL_PIXEL_UNPACK_BUFFER_BINDING)) return (const void *)(uintptr_t)ptr;
    if (!ptr && nullable) return NULL;
    return rt_gl_in(c, ptr, image_bytes(c, &unpack_store, w, h, d, format, type), 0);
}
const void *rt_gl_unpack_compressed(CPU *c, uint32_t ptr, int32_t size) {
    if (integer(GL_PIXEL_UNPACK_BUFFER_BINDING)) return (const void *)(uintptr_t)ptr;
    return rt_gl_in(c, ptr, rt_gl_n(size), 1);
}
static GLint level_parameter(uint32_t target, int32_t level, GLenum pname) {
    GLint v = 0; glGetTexLevelParameteriv(target, level, pname, &v); return v;
}
void *rt_gl_pack_tex(CPU *c, uint32_t ptr, uint32_t target, int32_t level, uint32_t format, uint32_t type) {
    if (integer(GL_PIXEL_PACK_BUFFER_BINDING)) return (void *)(uintptr_t)ptr;
    int is3d = target == GL_TEXTURE_3D || target == GL_TEXTURE_2D_ARRAY_EXT || target == GL_TEXTURE_1D_ARRAY_EXT;
    int32_t w = level_parameter(target, level, GL_TEXTURE_WIDTH);
    int32_t h = level_parameter(target, level, GL_TEXTURE_HEIGHT);
    int32_t d = is3d ? level_parameter(target, level, GL_TEXTURE_DEPTH) : 0;
    if (!w) return ptr ? GPTR(ptr) : NULL;  // cible ou niveau invalide : GL refuse sans écrire
    return rt_gl_out(c, ptr, image_bytes(c, &pack_store, w, h ? h : 1, d, format, type), 0);
}
void *rt_gl_pack_compressed(CPU *c, uint32_t ptr, uint32_t target, int32_t level) {
    if (integer(GL_PIXEL_PACK_BUFFER_BINDING)) return (void *)(uintptr_t)ptr;
    return rt_gl_out(c, ptr, rt_gl_n(level_parameter(target, level, GL_TEXTURE_COMPRESSED_IMAGE_SIZE)), 0);
}

// Nombre de valeurs écrites par glGet*v (GL 2.1 et extensions de togl).
// Limite : un pname à plusieurs valeurs absent de cette table est validé
// pour une seule valeur.
uint32_t rt_gl_get_count(uint32_t pname) {
    switch (pname) {
    case GL_MODELVIEW_MATRIX: case GL_PROJECTION_MATRIX: case GL_TEXTURE_MATRIX: case GL_COLOR_MATRIX:
    case GL_TRANSPOSE_MODELVIEW_MATRIX: case GL_TRANSPOSE_PROJECTION_MATRIX:
    case GL_TRANSPOSE_TEXTURE_MATRIX: case GL_TRANSPOSE_COLOR_MATRIX:
    case GL_CURRENT_MATRIX_ARB: case GL_TRANSPOSE_CURRENT_MATRIX_ARB: return 16;
    case GL_VIEWPORT: case GL_SCISSOR_BOX: case GL_COLOR_CLEAR_VALUE: case GL_COLOR_WRITEMASK:
    case GL_BLEND_COLOR: case GL_CURRENT_COLOR: case GL_CURRENT_TEXTURE_COORDS:
    case GL_CURRENT_RASTER_POSITION: case GL_CURRENT_RASTER_COLOR: case GL_CURRENT_RASTER_SECONDARY_COLOR:
    case GL_CURRENT_RASTER_TEXTURE_COORDS: case GL_CURRENT_SECONDARY_COLOR: case GL_FOG_COLOR:
    case GL_LIGHT_MODEL_AMBIENT: case GL_ACCUM_CLEAR_VALUE: case GL_MAP2_GRID_DOMAIN: return 4;
    case GL_CURRENT_NORMAL: case GL_POINT_DISTANCE_ATTENUATION: return 3;
    case GL_DEPTH_RANGE: case GL_MAX_VIEWPORT_DIMS: case GL_POLYGON_MODE: case GL_LINE_WIDTH_RANGE:
    case GL_POINT_SIZE_RANGE: case GL_ALIASED_LINE_WIDTH_RANGE: case GL_ALIASED_POINT_SIZE_RANGE:
    case GL_MAP1_GRID_DOMAIN: case GL_MAP2_GRID_SEGMENTS: return 2;
    case GL_COMPRESSED_TEXTURE_FORMATS: {
        GLint n = integer(GL_NUM_COMPRESSED_TEXTURE_FORMATS);
        return n > 0 ? (uint32_t)n : 0;
    }
    }
    return 1;
}
uint32_t rt_gl_texparam_count(uint32_t pname) { return pname == GL_TEXTURE_BORDER_COLOR ? 4 : 1; }

// ---- glGetString : copie stable en mémoire invitée ----
typedef struct GlString { uint32_t name, guest; char *text; struct GlString *next; } GlString;
static GlString *strings;

int rt_gl_manual_glGetString(CPU *c, void *fn) {
    uint32_t name = rt_gl_u32(c, 0);
    const char *text = (const char *)((const GLubyte *(*)(GLenum))fn)(name);
    c->eax = 0;
    if (!text) return 1;
    pthread_mutex_lock(&lock);
    GlString *s = strings;
    while (s && (s->name != name || strcmp(s->text, text))) s = s->next;
    if (!s) {
        size_t length = strlen(text);
        s = calloc(1, sizeof(*s));
        if (s) s->text = strdup(text);
        if (s && s->text && length < UINT32_MAX) s->guest = rt_alloc((uint32_t)length + 1);
        if (!s || !s->text || !s->guest) { pthread_mutex_unlock(&lock); rt_trap(c, c->eip, "glGetString : mémoire invitée épuisée"); }
        memcpy(GPTR(s->guest), text, length + 1);
        s->name = name; s->next = strings; strings = s;
    }
    c->eax = s->guest;  // jamais libéré : le pointeur reste valide comme sous Linux
    pthread_mutex_unlock(&lock);
    return 1;
}

// ---- Tampons mappés : miroir invité ----
// Le pointeur hôte de glMapBuffer ne peut pas être donné à l'invité. Chaque
// tampon reçoit un miroir en mémoire invitée (réutilisé d'un mappage à
// l'autre) : contenu recopié au mappage, renvoyé vers l'hôte à chaque
// glFlushMappedBufferRange(APPLE) et, sauf vidage explicite, au démappage.
// Limites : coût d'une copie complète par mappage ; écritures invitées non
// visibles par GL avant flush/démap (togl n'en dépend pas : APPLE_flush_buffer_range).
typedef struct Mirror {
    CGLShareGroupObj share; uint32_t name;
    uint32_t guest, capacity;
    void *host; uint64_t length;
    int mapped, writable, explicit_flush;
    int flush_on_unmap;  // état APPLE_flush_buffer_range du tampon (vrai par défaut)
    struct Mirror *next;
} Mirror;
static Mirror *mirrors;

static GLenum binding_of(uint32_t target) {
    switch (target) {
    case GL_ARRAY_BUFFER: return GL_ARRAY_BUFFER_BINDING;
    case GL_ELEMENT_ARRAY_BUFFER: return GL_ELEMENT_ARRAY_BUFFER_BINDING;
    case GL_PIXEL_PACK_BUFFER: return GL_PIXEL_PACK_BUFFER_BINDING;
    case GL_PIXEL_UNPACK_BUFFER: return GL_PIXEL_UNPACK_BUFFER_BINDING;
    case GL_UNIFORM_BUFFER_EXT: return GL_UNIFORM_BUFFER_BINDING_EXT;
    case GL_TRANSFORM_FEEDBACK_BUFFER_EXT: return GL_TRANSFORM_FEEDBACK_BUFFER_BINDING_EXT;
    case 0x8F36: case 0x8F37: return target;  // COPY_READ/WRITE_BUFFER (GL 3.1)
    }
    return 0;
}
static CGLShareGroupObj current_share(void) { return CGLGetShareGroup(CGLGetCurrentContext()); }
static Mirror *find_mirror(CGLShareGroupObj share, uint32_t name, int create) {
    for (Mirror *m = mirrors; m; m = m->next) if (m->share == share && m->name == name) return m;
    if (!create) return NULL;
    Mirror *m = calloc(1, sizeof(*m));
    if (!m) return NULL;
    m->share = share; m->name = name; m->flush_on_unmap = 1; m->next = mirrors; mirrors = m;
    return m;
}
static uint32_t bound_buffer(uint32_t target) {
    GLenum binding = binding_of(target);
    return binding ? (uint32_t)integer(binding) : 0;
}
static int host_still_mapped(uint32_t target) {
    GLint mapped = 0; glGetBufferParameteriv(target, GL_BUFFER_MAPPED, &mapped); return mapped;
}

// Mappage hôte réussi de [offset, offset+length) : miroir invité.
static uint32_t mirror_map(CPU *c, uint32_t target, void *host, uint64_t length, int copy_in, int writable, int explicit_flush) {
    uint32_t name = bound_buffer(target);
    Mirror *m = name ? find_mirror(current_share(), name, 1) : NULL;
    if (!m || length > UINT32_MAX) { glUnmapBufferARB(target); rt_trap(c, c->eip, "glMapBuffer : tampon hôte non suivi par le pont"); }
    if (m->capacity < length || !m->guest) {
        rt_free(m->guest);
        m->guest = rt_alloc((uint32_t)(length ? length : 1)); m->capacity = m->guest ? (uint32_t)length : 0;
        if (!m->guest) { glUnmapBufferARB(target); rt_trap(c, c->eip, "glMapBuffer : mémoire invitée épuisée"); }
    }
    if (copy_in && length) memcpy(GPTR(m->guest), host, length);
    m->host = host; m->length = length; m->mapped = 1;
    m->writable = writable; m->explicit_flush = explicit_flush;
    return m->guest;
}
static Mirror *mapped_mirror(uint32_t target) {
    uint32_t name = bound_buffer(target);
    Mirror *m = name ? find_mirror(current_share(), name, 0) : NULL;
    if (m && m->mapped && !host_still_mapped(target)) m->mapped = 0;  // démappé implicitement (glBufferData…)
    return m && m->mapped ? m : NULL;
}

int rt_gl_manual_glMapBufferARB(CPU *c, void *fn) {
    uint32_t target = rt_gl_u32(c, 0), access = rt_gl_u32(c, 1);
    void *host = ((void *(*)(GLenum, GLenum))fn)(target, access);
    c->eax = 0;
    if (!host) return 1;
    GLint size = 0; glGetBufferParameteriv(target, GL_BUFFER_SIZE, &size);
    Mirror *known = find_mirror(current_share(), bound_buffer(target), 0);
    int explicit_flush = known && !known->flush_on_unmap;
    c->eax = mirror_map(c, target, host, (uint64_t)(size > 0 ? size : 0), 1, access != GL_READ_ONLY, explicit_flush);
    return 1;
}

int rt_gl_manual_glMapBufferRange(CPU *c, void *fn) {
    uint32_t target = rt_gl_u32(c, 0), access = rt_gl_u32(c, 3);
    intptr_t offset = (int32_t)rt_gl_u32(c, 1), length = (int32_t)rt_gl_u32(c, 2);
    void *host = ((void *(*)(GLenum, intptr_t, intptr_t, GLbitfield))fn)(target, offset, length, access);
    c->eax = 0;
    if (!host) return 1;
    int invalidate = (access & (GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT)) && !(access & GL_MAP_READ_BIT);
    c->eax = mirror_map(c, target, host, (uint64_t)(length > 0 ? length : 0), !invalidate,
                        (access & GL_MAP_WRITE_BIT) != 0, (access & GL_MAP_FLUSH_EXPLICIT_BIT) != 0);
    return 1;
}

int rt_gl_manual_glUnmapBuffer(CPU *c, void *fn) {
    uint32_t target = rt_gl_u32(c, 0);
    Mirror *m = mapped_mirror(target);
    if (m) {
        if (m->writable && !m->explicit_flush && m->length) memcpy(m->host, GPTR(m->guest), m->length);
        m->mapped = 0; m->host = NULL;
    }
    c->eax = (uint8_t)((GLboolean (*)(GLenum))fn)(target);
    return 1;
}

static int flush_range(CPU *c, void *fn) {
    uint32_t target = rt_gl_u32(c, 0);
    intptr_t offset = (int32_t)rt_gl_u32(c, 1), size = (int32_t)rt_gl_u32(c, 2);
    Mirror *m = mapped_mirror(target);
    if (m && m->writable && offset >= 0 && size >= 0 && (uint64_t)offset + (uint64_t)size <= m->length)
        memcpy((char *)m->host + offset, (char *)GPTR(m->guest) + offset, (size_t)size);
    ((void (*)(GLenum, intptr_t, intptr_t))fn)(target, offset, size);
    return 1;
}
void rt_gl_stream_serialized(uint32_t name, int serialized);
void rt_gl_stream_forget(uint32_t name);
int rt_gl_manual_glBufferParameteriAPPLE(CPU *c, void *fn) {
    uint32_t target = rt_gl_u32(c, 0), pname = rt_gl_u32(c, 1);
    int32_t value = (int32_t)rt_gl_u32(c, 2);
    ((void (*)(GLenum, GLenum, GLint))fn)(target, pname, value);
    uint32_t name = bound_buffer(target);
    // Seul un appel valide modifie l'état : relire la valeur retenue par GL.
    if (name && pname == GL_BUFFER_SERIALIZED_MODIFY_APPLE) {
        GLint current = GL_TRUE; glGetBufferParameteriv(target, GL_BUFFER_SERIALIZED_MODIFY_APPLE, &current);
        rt_gl_stream_serialized(name, current != GL_FALSE);
    }
    if (name && pname == GL_BUFFER_FLUSHING_UNMAP_APPLE) {
        Mirror *m = find_mirror(current_share(), name, 1);
        GLint current = GL_TRUE; glGetBufferParameteriv(target, GL_BUFFER_FLUSHING_UNMAP_APPLE, &current);
        if (m) m->flush_on_unmap = current != GL_FALSE;
    }
    return 1;
}
int rt_gl_manual_glFlushMappedBufferRangeAPPLE(CPU *c, void *fn) { return flush_range(c, fn); }
int rt_gl_manual_glFlushMappedBufferRange(CPU *c, void *fn) { return flush_range(c, fn); }

int rt_gl_manual_glDeleteBuffersARB(CPU *c, void *fn) {
    int32_t n = (int32_t)rt_gl_u32(c, 0);
    uint32_t list = rt_gl_u32(c, 1);
    const GLuint *names = rt_gl_in(c, list, rt_gl_n(n) * 4, 0);
    CGLShareGroupObj share = current_share();
    for (int32_t i = 0; i < n; ++i) {
        uint32_t name = R32(list + 4 * (uint32_t)i);
        rt_gl_stream_forget(name);
        for (Mirror **p = &mirrors; *p; p = &(*p)->next) {
            if ((*p)->share != share || (*p)->name != name || !name) continue;
            Mirror *dead = *p; *p = dead->next;
            rt_free(dead->guest); free(dead);  // GL démappe implicitement un tampon supprimé
            break;
        }
    }
    ((void (*)(GLsizei, const GLuint *))fn)(n, names);
    return 1;
}

// ---- Sources de shaders : tableau de char * invités ----
int rt_gl_manual_glShaderSourceARB(CPU *c, void *fn) {
    void *shader = rt_gl_handle_arg(rt_gl_u32(c, 0));
    int32_t count = (int32_t)rt_gl_u32(c, 1);
    typedef void (*F)(void *, GLsizei, const char *const *, const GLint *);
    if (count <= 0) { ((F)fn)(shader, count, NULL, NULL); return 1; }  // GL valide count
    uint32_t list = rt_gl_u32(c, 2), length_list = rt_gl_u32(c, 3);
    rt_gl_in(c, list, (uint64_t)count * 4, 0);
    const GLint *lengths = rt_gl_in(c, length_list, (uint64_t)count * 4, 1);
    const char **host = calloc((size_t)count, sizeof(*host));
    if (!host) rt_trap(c, c->eip, "glShaderSource : mémoire hôte épuisée");
    for (int32_t i = 0; i < count; ++i) {
        uint32_t text = R32(list + 4 * (uint32_t)i);
        int32_t length = lengths ? (int32_t)R32(length_list + 4 * (uint32_t)i) : -1;
        host[i] = length >= 0 ? rt_gl_in(c, text, (uint64_t)length, 0) : rt_gl_str(c, text);
    }
    ((F)fn)(shader, count, host, lengths);
    free(host);
    return 1;
}

// ---- GLsync : pointeur hôte → handle ----
static void *sync_arg(CPU *c, uint32_t handle) {
    if (!handle) return NULL;  // GL signale lui-même l'objet invalide
    void *sync = rt_handle_get(handle, GL_SYNC_KIND);
    if (!sync) rt_trap(c, c->eip, "GLsync invalide");
    return sync;
}
int rt_gl_manual_glFenceSync(CPU *c, void *fn) {
    void *sync = ((void *(*)(GLenum, GLbitfield))fn)(rt_gl_u32(c, 0), rt_gl_u32(c, 1));
    c->eax = sync ? rt_handle_new(sync, GL_SYNC_KIND) : 0;
    if (sync && !c->eax) { glDeleteSync(sync); rt_trap(c, c->eip, "table de handles pleine (GLsync)"); }
    return 1;
}
int rt_gl_manual_glClientWaitSync(CPU *c, void *fn) {
    void *sync = sync_arg(c, rt_gl_u32(c, 0));
    GLbitfield flags = rt_gl_u32(c, 1);
    uint64_t timeout = rt_gl_u64(c, 2);
    unsigned saved = rt_guest_suspend();  // attente bloquante : laisser tourner les autres threads
    GLenum result = ((GLenum (*)(void *, GLbitfield, uint64_t))fn)(sync, flags, timeout);
    rt_guest_resume(saved);
    c->eax = result;
    return 1;
}
int rt_gl_manual_glWaitSync(CPU *c, void *fn) {
    ((void (*)(void *, GLbitfield, uint64_t))fn)(sync_arg(c, rt_gl_u32(c, 0)), rt_gl_u32(c, 1), rt_gl_u64(c, 2));
    return 1;
}
int rt_gl_manual_glDeleteSync(CPU *c, void *fn) {
    uint32_t handle = rt_gl_u32(c, 0);
    void *sync = sync_arg(c, handle);
    if (sync) rt_handle_free(handle, GL_SYNC_KIND);
    ((void (*)(void *))fn)(sync);
    return 1;
}
int rt_gl_manual_glGetSynciv(CPU *c, void *fn) {
    void *sync = sync_arg(c, rt_gl_u32(c, 0));
    uint32_t pname = rt_gl_u32(c, 1);
    int32_t size = (int32_t)rt_gl_u32(c, 2);
    GLsizei *length = rt_gl_out(c, rt_gl_u32(c, 3), 4, 1);
    GLint *values = rt_gl_out(c, rt_gl_u32(c, 4), rt_gl_n(size) * 4, 0);
    ((void (*)(void *, GLenum, GLsizei, GLsizei *, GLint *))fn)(sync, pname, size, length, values);
    return 1;
}

// APPLE_texture_range : le pointeur rendu est celui fourni par l'invité.
int rt_gl_manual_glGetTexParameterPointervAPPLE(CPU *c, void *fn) {
    uint32_t target = rt_gl_u32(c, 0), pname = rt_gl_u32(c, 1), out = rt_gl_u32(c, 2);
    rt_gl_out(c, out, 4, 0);
    void *value = NULL;
    ((void (*)(GLenum, GLenum, void **))fn)(target, pname, &value);
    uintptr_t p = (uintptr_t)value, base = (uintptr_t)M;
    if (p && (p < base || p - base > UINT32_MAX)) rt_trap(c, c->eip, "pointeur GL hôte hors de la mémoire invitée");
    W32(out, p ? (uint32_t)(p - base) : 0);
    return 1;
}

// ---- Écritures en flux sans synchronisation (actif par défaut ; P2_GL_STREAM_UNSYNC=0 pour désactiver) ----
// togl remplit ses tampons dynamiques par ajouts croissants (glBufferSubData) et
// les réalloue (glBufferData) une fois pleins ; le pilote d'Apple synchronise
// chaque ajout avec le GPU (~26 µs). Une écriture entièrement au-delà de tout
// ce qui a été écrit dans le stockage courant ne peut être lue par aucune
// commande en attente : elle passe par une projection non sérialisée
// (APPLE_flush_buffer_range, comme le portage Mac de Valve). Mesuré : 218 -> 274
// fps en jeu (--release, 1280x720). L'état est suivi par (share group, nom) ;
// tout chevauchement, projection, écriture possible du GPU (lecture de pixels
// vers un tampon, copie), échec de glBufferData ou état inconnu rend le
// stockage « entièrement écrit » : chemin synchronisé jusqu'à la réallocation.
typedef struct { CGLShareGroupObj share; uint32_t name; uint32_t size, written_end; int known, serialized, used; } Stream;
static Stream *streams; static uint32_t stream_capacity, stream_used;
static uint64_t stream_unsync_writes;
uint64_t rt_gl_stream_unsync_writes(void) { return stream_unsync_writes; }   // tests
static Stream *stream_state(uint32_t name, int create);
int rt_gl_stream_tracked(uint32_t name) { Stream *st = stream_state(name, 0); return st != NULL; }   // tests   // used : occupés + supprimés
static uint32_t stream_hash(CGLShareGroupObj share, uint32_t name) {
    return (uint32_t)(((uintptr_t)share >> 4) * 0x9e3779b1u) ^ (name * 2654435761u);
}
static Stream *stream_state(uint32_t name, int create) {
    if (!name) return NULL;
    CGLShareGroupObj share = current_share();
    if (create && (stream_used + 1) * 2 > stream_capacity) {
        uint32_t capacity = stream_capacity ? stream_capacity * 2 : 4096;
        Stream *grown = calloc(capacity, sizeof(*grown));
        if (!grown) return NULL;
        uint32_t live = 0;
        for (uint32_t i = 0; i < stream_capacity; ++i) if (streams[i].used == 1) {
            uint32_t h = stream_hash(streams[i].share, streams[i].name) & (capacity - 1);
            while (grown[h].used) h = (h + 1) & (capacity - 1);
            grown[h] = streams[i]; ++live;
        }
        free(streams); streams = grown; stream_capacity = capacity; stream_used = live;
    }
    if (!stream_capacity) return NULL;
    Stream *tombstone = NULL;
    for (uint32_t i = 0, h = stream_hash(share, name) & (stream_capacity - 1); i < stream_capacity; ++i, h = (h + 1) & (stream_capacity - 1)) {
        Stream *e = &streams[h];
        if (e->used == 1 && e->share == share && e->name == name) return e;
        if (e->used == 2) { if (!tombstone) tombstone = e; continue; }   // supprimé
        if (!e->used) {
            if (!create) return NULL;
            if (!tombstone) { tombstone = e; ++stream_used; }
            *tombstone = (Stream){share, name, 0, 0, 0, 1, 1};
            return tombstone;
        }
    }
    return NULL;
}
static int stream_enabled(void) {
    static int value = -1;
    if (value < 0) { const char *e = getenv("P2_GL_STREAM_UNSYNC"); value = !e || strcmp(e, "0"); }   // actif par défaut
    return value;
}
void rt_gl_stream_serialized(uint32_t name, int serialized) {
    if (!stream_enabled()) return;
    Stream *st = stream_state(name, 1); if (st) st->serialized = serialized;
}
void rt_gl_stream_forget(uint32_t name) {   // glDeleteBuffers : le nom pourra être réutilisé
    if (!stream_enabled()) return;
    Stream *st = stream_state(name, 0); if (st) st->used = 2;
}
static void stream_unknown(uint32_t target) {   // stockage considéré entièrement écrit
    Stream *st = stream_state(bound_buffer(target), 0);
    if (st) st->written_end = st->size;
}
// Renvoie 1 si l'écriture a été faite sans synchronisation.
static int stream_subdata(CPU *c, uint32_t target, uint32_t offset, uint32_t size, uint32_t data) {
    if (target != GL_ARRAY_BUFFER && target != GL_ELEMENT_ARRAY_BUFFER) return 0;
#if TARGET_OS_IPHONE
    // gl4es copie dans son double en mémoire puis appelle glBufferSubData d'ES,
    // que ios/gl_ios.m fait passer par une projection non synchronisée.
    {
        uint32_t name = bound_buffer(target);
        Stream *st = stream_state(name, 0);
        if (!st || !st->known || !size || !data || offset < st->written_end || (uint64_t)offset + size > st->size) return 0;
        Mirror *m = find_mirror(current_share(), name, 0);
        if (m && m->mapped) return 0;
        rt_ios_gl_unsync_next = 1;
        glBufferSubData(target, (GLintptr)offset, (GLsizeiptr)size, rt_span(c, data, size));
        rt_ios_gl_unsync_next = 0;
        st->written_end = offset + size;
        ++stream_unsync_writes;
        return 1;
    }
#else
    uint32_t name = bound_buffer(target);
    Stream *st = stream_state(name, 0);
    if (!st || !st->known || !size || !data || offset < st->written_end || (uint64_t)offset + size > st->size) return 0;
    Mirror *m = find_mirror(current_share(), name, 0);
    if (m && m->mapped) return 0;                              // projection invitée en cours
    const void *source = rt_span(c, data, size);
    int was_serialized = st->serialized, flush_unmap = m ? m->flush_on_unmap : 1;
    if (was_serialized) glBufferParameteriAPPLE(target, GL_BUFFER_SERIALIZED_MODIFY_APPLE, GL_FALSE);
    if (flush_unmap) glBufferParameteriAPPLE(target, GL_BUFFER_FLUSHING_UNMAP_APPLE, GL_FALSE);
    uint8_t *host = glMapBuffer(target, GL_WRITE_ONLY);
    GLboolean intact = GL_FALSE;
    if (host) {
        memcpy(host + offset, source, size);
        glFlushMappedBufferRangeAPPLE(target, (GLintptr)offset, (GLsizeiptr)size);
        intact = glUnmapBuffer(target);
    }
    if (flush_unmap) glBufferParameteriAPPLE(target, GL_BUFFER_FLUSHING_UNMAP_APPLE, GL_TRUE);
    if (was_serialized) glBufferParameteriAPPLE(target, GL_BUFFER_SERIALIZED_MODIFY_APPLE, GL_TRUE);
    // GL_FALSE au démappage : contenu du stockage perdu. Le glBufferSubData normal
    // réécrit la plage, et le stockage n'est plus suivi jusqu'à sa réallocation.
    if (host && !intact) st->known = 0;
    if (!host || !intact) return 0;
    st->written_end = offset + size;
    ++stream_unsync_writes;
    return 1;
#endif
}
static int valid_usage(uint32_t usage) {
    switch (usage) {
    case GL_STREAM_DRAW: case GL_STREAM_READ: case GL_STREAM_COPY:
    case GL_STATIC_DRAW: case GL_STATIC_READ: case GL_STATIC_COPY:
    case GL_DYNAMIC_DRAW: case GL_DYNAMIC_READ: case GL_DYNAMIC_COPY: return 1;
    }
    return 0;
}
// Avant glBufferData : toutes ses erreurs déterministes sont connues d'avance
// (cible et tampon lié, taille, usage, tampon projeté). On ne lit jamais
// glGetError, pour laisser au jeu la sémantique exacte d'Apple : seul un
// GL_OUT_OF_MEMORY d'un stockage de même taille échapperait (au pire une image
// fausse, l'écriture restant dans les bornes du stockage existant).
static int buffer_data_ok_before;
static void stream_buffer_data_before(uint32_t target, uint32_t size, uint32_t usage) {
    uint32_t name = bound_buffer(target);
    Mirror *m = name ? find_mirror(current_share(), name, 0) : NULL;
    GLint mapped = GL_FALSE;
    if (name) glGetBufferParameteriv(target, GL_BUFFER_MAPPED, &mapped);
    buffer_data_ok_before = name && (int32_t)size >= 0 && valid_usage(usage) && !mapped && !(m && m->mapped);
}
// Avant le thunk : détourne les ajouts, marque les écritures possibles.
static int stream_before(CPU *c, int kind) {
    uint32_t a = c->esp + 4;
    if (kind == GL_KIND_SUBDATA) {
        uint32_t target = R32(a), offset = R32(a + 4), size = R32(a + 8), data = R32(a + 12);
        if (stream_subdata(c, target, offset, size, data)) return 1;
        Stream *st = stream_state(bound_buffer(target), 0);
        if (st) { uint64_t end = (uint64_t)offset + size; if (end > st->written_end) st->written_end = end > st->size ? st->size : (uint32_t)end; }
    } else if (kind == GL_KIND_DATA) {
        stream_buffer_data_before(R32(a), R32(a + 4), R32(a + 12));
    } else if (kind == GL_KIND_MAP) {
        stream_unknown(R32(a));
    } else if (kind == GL_KIND_COPY_BUFFER) {
        stream_unknown(R32(a + 4));                            // cible d'écriture
    } else if (kind == GL_KIND_PACK_READ) {
        stream_unknown(GL_PIXEL_PACK_BUFFER);                  // le GPU peut écrire dans le tampon de pixels lié
    }
    return 0;
}
// Après le thunk : glBufferData n'est pris en compte que s'il a réussi.
static void stream_after(CPU *c, int kind) {
    if (kind != GL_KIND_DATA) return;
    uint32_t a = c->esp + 4, target = R32(a), size = R32(a + 4), data = R32(a + 8);
    Stream *st = stream_state(bound_buffer(target), 1);
    if (!st) return;
    GLint actual = -1; glGetBufferParameteriv(target, GL_BUFFER_SIZE, &actual);
    if (!buffer_data_ok_before || actual < 0 || (uint32_t)actual != size) { st->known = 0; return; }
    st->size = size; st->written_end = data ? size : 0; st->known = 1;
}
