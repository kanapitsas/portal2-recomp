// Fontconfig minimal pour iOS (voir fontconfig/fontconfig.h) : un index des
// polices lu par FreeType au premier FcInit, puis correspondance par famille
// (insensible à la casse, avec quelques équivalents), graisse et inclinaison.
//
// Répertoires indexés : P2_FONT_DIRS (séparés par ':'), sinon les polices
// système. Chaque requête est journalisée une fois (P2_FONT_LOG=1 pour tout).
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE
#include "fontconfig/fontconfig.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include <dirent.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>

typedef struct { char *object; FcValue value; } Element;
struct _FcPattern { Element *items; int count; };
struct _FcConfig { int unused; };
struct _FcObjectSet { char **names; int count; };

typedef struct { char *family, *style, *file; int index, weight, slant; } Font;
static Font *fonts;
static int font_count;
static pthread_once_t once = PTHREAD_ONCE_INIT;
static struct _FcConfig config;
// Index (fonts, font_count) : grossi par FcConfigAppFontAddFile pendant que
// d'autres threads peuvent chercher une police.
static pthread_mutex_t index_lock = PTHREAD_MUTEX_INITIALIZER;

// ---------------------------------------------------------------- index
static int extension_ok(const char *name) {
    const char *dot = strrchr(name, '.');
    return dot && (!strcasecmp(dot, ".ttf") || !strcasecmp(dot, ".ttc") || !strcasecmp(dot, ".otf"));
}
static void add_font(FT_Library library, const char *path) {
    FT_Face face;
    if (FT_New_Face(library, path, -1, &face)) return;
    long faces = face->num_faces;
    FT_Done_Face(face);
    for (long i = 0; i < faces && i < 64; ++i) {
        if (FT_New_Face(library, path, i, &face)) continue;
        if (face->family_name && (face->face_flags & FT_FACE_FLAG_SCALABLE)) {
            Font *grown = realloc(fonts, (size_t)(font_count + 1) * sizeof(*fonts));
            if (grown) {
                fonts = grown;
                fonts[font_count++] = (Font){strdup(face->family_name), strdup(face->style_name ? face->style_name : "Regular"),
                    strdup(path), (int)i, face->style_flags & FT_STYLE_FLAG_BOLD ? FC_WEIGHT_BOLD : FC_WEIGHT_REGULAR,
                    face->style_flags & FT_STYLE_FLAG_ITALIC ? FC_SLANT_ITALIC : FC_SLANT_ROMAN};
            }
        }
        FT_Done_Face(face);
    }
}
static void scan(FT_Library library, const char *directory, int depth) {
    DIR *dir = opendir(directory);
    if (!dir) return;
    struct dirent *entry;
    while ((entry = readdir(dir))) {
        if (entry->d_name[0] == '.') continue;
        char path[2048];
        snprintf(path, sizeof(path), "%s/%s", directory, entry->d_name);
        struct stat st;
        if (stat(path, &st)) continue;
        if (S_ISDIR(st.st_mode) && depth < 4) scan(library, path, depth + 1);
        else if (S_ISREG(st.st_mode) && extension_ok(entry->d_name)) add_font(library, path);
    }
    closedir(dir);
}
static void build_index(void) {
    FT_Library library;
    if (FT_Init_FreeType(&library)) return;
    const char *dirs = getenv("P2_FONT_DIRS");
    char *copy = strdup(dirs && *dirs ? dirs : "/System/Library/Fonts");
    for (char *save = NULL, *d = strtok_r(copy, ":", &save); d; d = strtok_r(NULL, ":", &save)) scan(library, d, 0);
    free(copy);
    FT_Done_FreeType(library);
    fprintf(stderr, "[fontconfig-ios] %d faces indexées\n", font_count);
}

// ---------------------------------------------------------------- motifs
static const Element *element(const FcPattern *p, const char *object, int n) {
    for (int i = 0; p && i < p->count; ++i)
        if (!strcmp(p->items[i].object, object) && n-- == 0) return &p->items[i];
    return NULL;
}
FcPattern *FcPatternCreate(void) { return calloc(1, sizeof(FcPattern)); }
void FcPatternDestroy(FcPattern *p) {
    if (!p) return;
    for (int i = 0; i < p->count; ++i) {
        free(p->items[i].object);
        if (p->items[i].value.type == FcTypeString) free((void *)p->items[i].value.u.s);
        if (p->items[i].value.type == FcTypeMatrix) free((void *)p->items[i].value.u.m);
    }
    free(p->items);
    free(p);
}
FcBool FcPatternAdd(FcPattern *p, const char *object, FcValue value, FcBool append) {
    if (!p || !object) return FcFalse;
    if (value.type == FcTypeString) value.u.s = (const FcChar8 *)strdup(value.u.s ? (const char *)value.u.s : "");
    else if (value.type == FcTypeMatrix && value.u.m) {
        FcMatrix *m = malloc(sizeof(*m)); if (!m) return FcFalse; *m = *value.u.m; value.u.m = m;
    } else if (value.type != FcTypeInteger && value.type != FcTypeBool && value.type != FcTypeDouble &&
               value.type != FcTypeVoid) return FcFalse;
    Element *grown = realloc(p->items, (size_t)(p->count + 1) * sizeof(*grown));
    if (!grown) return FcFalse;
    p->items = grown;
    Element e = {strdup(object), value};
    if (append) p->items[p->count] = e;
    else { memmove(p->items + 1, p->items, (size_t)p->count * sizeof(*grown)); p->items[0] = e; }
    p->count++;
    return FcTrue;
}
static void add_string(FcPattern *p, const char *object, const char *s) {
    FcValue v = {.type = FcTypeString}; v.u.s = (const FcChar8 *)s; FcPatternAdd(p, object, v, FcTrue);
}
static void add_int(FcPattern *p, const char *object, int i) {
    FcValue v = {.type = FcTypeInteger}; v.u.i = i; FcPatternAdd(p, object, v, FcTrue);
}
static void add_bool(FcPattern *p, const char *object, FcBool b) {
    FcValue v = {.type = FcTypeBool}; v.u.b = b; FcPatternAdd(p, object, v, FcTrue);
}
FcResult FcPatternGetString(const FcPattern *p, const char *object, int n, FcChar8 **s) {
    const Element *e = element(p, object, n);
    if (!e) return FcResultNoMatch;
    if (e->value.type != FcTypeString) return FcResultTypeMismatch;
    *s = (FcChar8 *)e->value.u.s;
    return FcResultMatch;
}
FcResult FcPatternGetBool(const FcPattern *p, const char *object, int n, FcBool *b) {
    const Element *e = element(p, object, n);
    if (!e) return FcResultNoMatch;
    if (e->value.type != FcTypeBool) return FcResultTypeMismatch;
    *b = e->value.u.b;
    return FcResultMatch;
}
static int get_int(const FcPattern *p, const char *object, int fallback) {
    const Element *e = element(p, object, 0);
    if (!e) return fallback;
    if (e->value.type == FcTypeInteger) return e->value.u.i;
    if (e->value.type == FcTypeDouble) return (int)e->value.u.d;
    return fallback;
}
static const char *get_string(const FcPattern *p, const char *object) {
    const Element *e = element(p, object, 0);
    return e && e->value.type == FcTypeString ? (const char *)e->value.u.s : NULL;
}

// ---------------------------------------------------------------- configuration
FcBool FcInit(void) { pthread_once(&once, build_index); return FcTrue; }
FcConfig *FcConfigGetCurrent(void) { FcInit(); return &config; }
// Une seule configuration (Half-Life 2 en charge une « nouvelle » et la rend
// courante avant d'y ajouter ses polices) : référence et remplacement sans effet.
FcConfig *FcConfigReference(FcConfig *c) { FcInit(); return c ? c : &config; }
FcConfig *FcInitLoadConfigAndFonts(void) { FcInit(); return &config; }
FcBool FcConfigSetCurrent(FcConfig *c) { (void)c; FcInit(); return FcTrue; }
// Police du jeu (HL2 : resource/*.ttf, dont les icônes du HUD), ajoutée à
// l'index ; FcFalse si FreeType ne la lit pas.
FcBool FcConfigAppFontAddFile(FcConfig *c, const FcChar8 *file) {
    (void)c;
    FcInit();
    if (!file) return FcFalse;
    pthread_mutex_lock(&index_lock);
    // Le jeu ajoute ses polices à chaque rechargement du schéma : une fois suffit.
    for (int i = 0; i < font_count; ++i)
        if (!strcmp(fonts[i].file, (const char *)file)) { pthread_mutex_unlock(&index_lock); return FcTrue; }
    FT_Library library;
    int before = font_count;
    if (!FT_Init_FreeType(&library)) { add_font(library, (const char *)file); FT_Done_FreeType(library); }
    int added = font_count - before;
    pthread_mutex_unlock(&index_lock);
    fprintf(stderr, "[fontconfig-ios] police du jeu %s : %d face(s)\n", file, added);
    return added > 0;
}
FcBool FcConfigSubstitute(FcConfig *c, FcPattern *p, FcMatchKind kind) { (void)c; (void)p; (void)kind; return FcTrue; }
void FcDefaultSubstitute(FcPattern *p) { (void)p; }

// Familles Windows/Linux que Source demande souvent, et leur équivalent iOS.
static const char *alias(const char *family) {
    static const char *table[][2] = {
        {"Tahoma", "Verdana"}, {"Lucida Console", "Menlo"}, {"Courier", "Courier New"},
        {"Marlett", "Arial"}, {"DejaVu Sans", "Helvetica"}, {"Liberation Sans", "Arial"},
        {"sans-serif", "Helvetica"}, {"monospace", "Menlo"}, {"serif", "Times New Roman"},
    };
    for (size_t i = 0; i < sizeof(table) / sizeof(*table); ++i)
        if (!strcasecmp(family, table[i][0])) return table[i][1];
    return NULL;
}
static int family_score(const char *want, const char *have) {
    if (!want) return 1;
    if (!strcasecmp(want, have)) return 3;
    const char *a = alias(want);
    if (a && !strcasecmp(a, have)) return 2;
    return 0;
}
static FcPattern *font_pattern(const Font *f) {
    FcPattern *p = FcPatternCreate();
    if (!p) return NULL;
    add_string(p, FC_FAMILY, f->family); add_string(p, FC_STYLE, f->style); add_string(p, FC_FILE, f->file);
    add_int(p, FC_INDEX, f->index); add_int(p, FC_WEIGHT, f->weight); add_int(p, FC_SLANT, f->slant);
    add_bool(p, FC_SCALABLE, FcTrue);
    return p;
}
static void log_request(const char *what, const char *family, const char *answer) {
    static char seen[64][96];
    static int count;
    static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
    char key[96];
    snprintf(key, sizeof(key), "%s:%s", what, family ? family : "*");
    pthread_mutex_lock(&lock);
    int known = 0;
    for (int i = 0; i < count; ++i) known |= !strcmp(seen[i], key);
    if (!known && count < 64) strcpy(seen[count++], key);
    pthread_mutex_unlock(&lock);
    const char *all = getenv("P2_FONT_LOG");
    if (!known || (all && *all == '1'))
        fprintf(stderr, "[fontconfig-ios] %s(%s) -> %s\n", what, family ? family : "*", answer ? answer : "rien");
}

FcPattern *FcFontMatch(FcConfig *c, FcPattern *p, FcResult *result) {
    (void)c;
    FcInit();
    const char *family = get_string(p, FC_FAMILY);
    int bold = get_int(p, FC_WEIGHT, FC_WEIGHT_REGULAR) >= 150;
    int italic = get_int(p, FC_SLANT, FC_SLANT_ROMAN) > 0;
    const Font *best = NULL;
    int best_score = -1;
    pthread_mutex_lock(&index_lock);
    for (int pass = 0; pass < 2 && !best; ++pass) {
        const char *want = pass ? "Helvetica" : family;   // repli : une police par défaut
        for (int i = 0; i < font_count; ++i) {
            int f = family_score(want, fonts[i].family);
            if (!f) continue;
            int score = f * 100 + ((fonts[i].weight >= 150) == bold) * 10 + ((fonts[i].slant > 0) == italic) * 10;
            if (score > best_score) { best_score = score; best = &fonts[i]; }
        }
    }
    if (!best && font_count) best = &fonts[0];
    FcPattern *match = best ? font_pattern(best) : NULL;
    const char *file = best ? best->file : NULL;   // chaîne propre à la police : survit au verrou
    pthread_mutex_unlock(&index_lock);
    log_request("FcFontMatch", family, file);
    if (result) *result = match ? FcResultMatch : FcResultNoMatch;
    return match;
}

FcObjectSet *FcObjectSetCreate(void) { return calloc(1, sizeof(FcObjectSet)); }
FcBool FcObjectSetAdd(FcObjectSet *os, const char *object) {
    if (!os || !object) return FcFalse;
    char **grown = realloc(os->names, (size_t)(os->count + 1) * sizeof(*grown));
    if (!grown) return FcFalse;
    os->names = grown;
    os->names[os->count++] = strdup(object);
    return FcTrue;
}
void FcObjectSetDestroy(FcObjectSet *os) {
    if (!os) return;
    for (int i = 0; i < os->count; ++i) free(os->names[i]);
    free(os->names); free(os);
}

FcFontSet *FcFontList(FcConfig *c, FcPattern *p, FcObjectSet *os) {
    (void)c; (void)os;   // les motifs renvoyés portent toujours tous les attributs
    FcInit();
    const char *family = get_string(p, FC_FAMILY), *style = get_string(p, FC_STYLE);
    FcFontSet *set = calloc(1, sizeof(*set));
    if (!set) return NULL;
    pthread_mutex_lock(&index_lock);
    set->fonts = calloc((size_t)(font_count ? font_count : 1), sizeof(*set->fonts));
    if (!set->fonts) { pthread_mutex_unlock(&index_lock); free(set); return NULL; }
    for (int i = 0; i < font_count; ++i) {
        if (family && strcasecmp(family, fonts[i].family)) continue;
        if (style && strcasecmp(style, fonts[i].style)) continue;
        FcPattern *font = font_pattern(&fonts[i]);
        if (font) set->fonts[set->nfont++] = font;
    }
    set->sfont = font_count;
    pthread_mutex_unlock(&index_lock);
    char answer[32];
    snprintf(answer, sizeof(answer), "%d faces", set->nfont);
    log_request("FcFontList", family, answer);
    return set;
}
void FcFontSetDestroy(FcFontSet *s) {
    if (!s) return;
    for (int i = 0; i < s->nfont; ++i) FcPatternDestroy(s->fonts[i]);
    free(s->fonts); free(s);
}
#endif
