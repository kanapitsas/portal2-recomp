// Pont FreeType + Fontconfig pour vguimatsurface.so (polices VGUI).
//
// Le code invité déréférence directement FT_Face (glyph, size, charmap,
// available_sizes, bbox...) et FcFontSet (nfont, fonts[i]). Ces structures sont
// recopiées en mémoire invitée au format public FreeType 2 / Fontconfig i386
// (FT_Long, FT_Pos, FT_Fixed et pointeurs sur 32 bits) et resynchronisées après
// chaque appel qui les modifie. Les objets opaques (FT_Library, FcConfig,
// FcPattern, FcObjectSet) sont des handles rt_handle_*.
//
// SHIMS: FT_Init_FreeType FT_Done_FreeType FT_New_Face FT_New_Memory_Face FT_Done_Face
// SHIMS: FT_Select_Charmap FT_Set_Pixel_Sizes FT_Request_Size FT_Select_Size
// SHIMS: FT_Get_Char_Index FT_Load_Char FT_Load_Glyph FT_Get_Kerning FT_Get_Postscript_Name FT_MulFix
// SHIMS: FcInit FcInitLoadConfigAndFonts FcConfigGetCurrent FcConfigSetCurrent FcConfigAppFontAddFile
// SHIMS: FcConfigSubstitute FcDefaultSubstitute FcFontList FcFontMatch
// SHIMS: FcFontSetDestroy FcObjectSetCreate FcObjectSetAdd FcObjectSetDestroy
// SHIMS: FcPatternCreate FcPatternAdd FcPatternDestroy FcPatternGetBool FcPatternGetString
#include "rt.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include <fontconfig/fontconfig.h>
#include <dlfcn.h>
#include <limits.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#define KIND_LIBRARY 0x46544c42u  // 'FTLB'
#define KIND_CONFIG  0x46434346u  // 'FCCF'
#define KIND_PATTERN 0x46435054u  // 'FCPT'
#define KIND_OBJSET  0x46434f53u  // 'FCOS'

// Disposition i386 (vérifiée par clang -target i386-linux-gnu sur les en-têtes
// FreeType 2.15 / Fontconfig 2.18 ; identique dans toute la série FreeType 2).
enum {
    FACE_SIZE = 0x84, SIZE_SIZE = 0x2c, SLOT_SIZE = 0xa0, CHARMAP_SIZE = 0x0c, BITMAP_SIZE_SIZE = 0x10,
    SIZE_OFFSET = FACE_SIZE, SLOT_OFFSET = SIZE_OFFSET + SIZE_SIZE, CHARMAPS_OFFSET = SLOT_OFFSET + SLOT_SIZE,
    // FT_FaceRec
    F_NUM_FACES = 0x00, F_FACE_INDEX = 0x04, F_FACE_FLAGS = 0x08, F_STYLE_FLAGS = 0x0c, F_NUM_GLYPHS = 0x10,
    F_FAMILY = 0x14, F_STYLE = 0x18, F_NUM_FIXED = 0x1c, F_SIZES = 0x20, F_NUM_CHARMAPS = 0x24,
    F_CHARMAPS = 0x28, F_BBOX = 0x34, F_UNITS_PER_EM = 0x44, F_ASCENDER = 0x46, F_DESCENDER = 0x48,
    F_HEIGHT = 0x4a, F_MAX_ADV_W = 0x4c, F_MAX_ADV_H = 0x4e, F_UNDERLINE_POS = 0x50,
    F_UNDERLINE_THICK = 0x52, F_GLYPH = 0x54, F_SIZE = 0x58, F_CHARMAP = 0x5c,
    // FT_SizeRec
    S_FACE = 0x00, S_METRICS = 0x0c,
    // FT_GlyphSlotRec
    G_LIBRARY = 0x00, G_FACE = 0x04, G_INDEX = 0x0c, G_METRICS = 0x18, G_LINEAR_HORI = 0x38,
    G_LINEAR_VERT = 0x3c, G_ADVANCE = 0x40, G_FORMAT = 0x48, G_BITMAP = 0x4c, G_BITMAP_LEFT = 0x64,
    G_BITMAP_TOP = 0x68, G_OUTLINE = 0x6c, G_NUM_SUBGLYPHS = 0x80, G_SUBGLYPHS = 0x84,
    G_CONTROL_DATA = 0x88, G_CONTROL_LEN = 0x8c, G_LSB_DELTA = 0x90, G_RSB_DELTA = 0x94,
};

typedef struct { uint32_t address, capacity; } GuestBuffer;

typedef struct Face {
    uint32_t guest, library;     // miroir FT_FaceRec et handle FT_Library invité
    FT_Face host;
    uint32_t charmap_records;     // FT_CharMapRec[num_charmaps] invités
    GuestBuffer block, bitmap, outline, control;
    uint32_t postscript;
    struct Face *next;
} Face;

typedef struct Pattern {
    FcPattern *host;
    const FcChar8 **sources; uint32_t *copies; uint32_t count;  // chaînes renvoyées
} Pattern;

typedef struct FontSet {
    uint32_t guest; FcFontSet *host; uint32_t count; uint32_t *patterns;
    struct FontSet *next;
} FontSet;

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static Face *faces;
static FontSet *font_sets;

static uint32_t arg(CPU *c, unsigned i) { return R32(c->esp + 4 + 4 * i); }

static uint32_t guest_alloc(CPU *c, uint32_t size) {
    uint32_t address = rt_alloc(size);
    if (!address) rt_trap(c, c->eip, "guest heap exhausted (fonts bridge)");
    memset(GPTR(address), 0, size);
    return address;
}
static uint32_t buffer_fill(CPU *c, GuestBuffer *b, const void *data, uint64_t size) {
    if (!size) return 0;
    if (size > UINT32_MAX) rt_trap(c, c->eip, "font data too large for guest memory");
    if (b->capacity < size) {
        rt_free(b->address);
        b->address = guest_alloc(c, (uint32_t)size); b->capacity = (uint32_t)size;
    }
    memcpy(GPTR(b->address), data, (size_t)size);
    return b->address;
}
static void buffer_release(GuestBuffer *b) { rt_free(b->address); *b = (GuestBuffer){0}; }
static uint32_t guest_string(CPU *c, const char *s) {
    if (!s) return 0;
    size_t n = strlen(s) + 1;
    uint32_t a = guest_alloc(c, (uint32_t)n);
    memcpy(GPTR(a), s, n);
    return a;
}
static const char *host_string(CPU *c, uint32_t address) {
    if (!address) return NULL;
    rt_string_length(c, address);
    return GPTR(address);
}

// ---------------------------------------------------------------- FreeType

static FT_Library library_of(uint32_t handle) { return handle ? rt_handle_get(handle, KIND_LIBRARY) : NULL; }

static Face *find_face(CPU *c, uint32_t guest) {
    if (!guest) return NULL;
    pthread_mutex_lock(&lock);
    Face *f = faces;
    while (f && f->guest != guest) f = f->next;
    pthread_mutex_unlock(&lock);
    if (!f) rt_trap(c, c->eip, "FreeType call on an unknown FT_Face");
    return f;
}

static void sync_size(Face *f) {
    uint32_t s = f->guest + SIZE_OFFSET;
    FT_Size size = f->host->size;
    W32(f->guest + F_SIZE, size ? s : 0);
    memset(GPTR(s), 0, SIZE_SIZE);
    if (!size) return;
    const FT_Size_Metrics *m = &size->metrics;
    uint32_t a = s + S_METRICS;
    W32(s + S_FACE, f->guest);
    W16(a + 0, m->x_ppem); W16(a + 2, m->y_ppem);
    W32(a + 4, (uint32_t)m->x_scale); W32(a + 8, (uint32_t)m->y_scale);
    W32(a + 12, (uint32_t)m->ascender); W32(a + 16, (uint32_t)m->descender);
    W32(a + 20, (uint32_t)m->height); W32(a + 24, (uint32_t)m->max_advance);
}

static void sync_charmap(Face *f) {
    uint32_t value = 0;
    for (int i = 0; i < f->host->num_charmaps; ++i)
        if (f->host->charmaps[i] == f->host->charmap) value = f->charmap_records + CHARMAP_SIZE * (uint32_t)i;
    W32(f->guest + F_CHARMAP, value);
}

static void sync_glyph(CPU *c, Face *f) {
    uint32_t g = f->guest + SLOT_OFFSET;
    FT_GlyphSlot slot = f->host->glyph;
    W32(f->guest + F_GLYPH, slot ? g : 0);
    memset(GPTR(g), 0, SLOT_SIZE);
    if (!slot) return;
    if (slot->num_subglyphs) rt_trap(c, c->eip, "FT glyph subglyphs (FT_LOAD_NO_RECURSE) not bridged");
    if (slot->bitmap.palette) rt_trap(c, c->eip, "FT bitmap palette not bridged");
    W32(g + G_LIBRARY, f->library); W32(g + G_FACE, f->guest);
    W32(g + G_INDEX, slot->glyph_index);
    const FT_Glyph_Metrics *m = &slot->metrics;
    const FT_Pos metrics[8] = {m->width, m->height, m->horiBearingX, m->horiBearingY, m->horiAdvance,
                               m->vertBearingX, m->vertBearingY, m->vertAdvance};
    for (int i = 0; i < 8; ++i) W32(g + G_METRICS + 4 * i, (uint32_t)metrics[i]);
    W32(g + G_LINEAR_HORI, (uint32_t)slot->linearHoriAdvance);
    W32(g + G_LINEAR_VERT, (uint32_t)slot->linearVertAdvance);
    W32(g + G_ADVANCE, (uint32_t)slot->advance.x); W32(g + G_ADVANCE + 4, (uint32_t)slot->advance.y);
    W32(g + G_FORMAT, slot->format);
    const FT_Bitmap *b = &slot->bitmap;
    uint64_t bytes = b->buffer ? (uint64_t)b->rows * (uint64_t)(b->pitch < 0 ? -(int64_t)b->pitch : b->pitch) : 0;
    uint32_t bm = g + G_BITMAP;
    W32(bm + 0x00, b->rows); W32(bm + 0x04, b->width); W32(bm + 0x08, (uint32_t)b->pitch);
    W32(bm + 0x0c, buffer_fill(c, &f->bitmap, b->buffer, bytes));
    W16(bm + 0x10, b->num_grays); W8(bm + 0x12, b->pixel_mode); W8(bm + 0x13, b->palette_mode);
    W32(g + G_BITMAP_LEFT, (uint32_t)slot->bitmap_left); W32(g + G_BITMAP_TOP, (uint32_t)slot->bitmap_top);
    // FT_Outline : points (FT_Vector 2×32 bits), tags, contours (16 bits) dans un tampon.
    const FT_Outline *o = &slot->outline;
    uint32_t points = (uint32_t)o->n_points, contours = (uint32_t)o->n_contours;
    uint32_t ol = g + G_OUTLINE;
    W16(ol + 0, o->n_contours); W16(ol + 2, o->n_points); W32(ol + 0x10, (uint32_t)o->flags);
    if (points || contours) {
        uint32_t tags_at = points * 8, contours_at = (tags_at + points + 1) & ~1u;
        uint32_t total = contours_at + contours * 2;
        uint8_t *host = calloc(1, total);
        if (!host) rt_trap(c, c->eip, "out of host memory (FT outline)");
        for (uint32_t i = 0; i < points; ++i) {
            int32_t xy[2] = {(int32_t)o->points[i].x, (int32_t)o->points[i].y};
            memcpy(host + 8 * i, xy, 8);
        }
        if (points) memcpy(host + tags_at, o->tags, points);
        for (uint32_t i = 0; i < contours; ++i) { uint16_t v = (uint16_t)o->contours[i]; memcpy(host + contours_at + 2 * i, &v, 2); }
        uint32_t base = buffer_fill(c, &f->outline, host, total);
        free(host);
        W32(ol + 0x04, points ? base : 0); W32(ol + 0x08, points ? base + tags_at : 0);
        W32(ol + 0x0c, contours ? base + contours_at : 0);
    }
    // Instructions TrueType du glyphe (control_data), recopiées telles quelles.
    if (slot->control_data && slot->control_len > 0) {
        W32(g + G_CONTROL_DATA, buffer_fill(c, &f->control, slot->control_data, (uint64_t)slot->control_len));
        W32(g + G_CONTROL_LEN, (uint32_t)slot->control_len);
    }
    W32(g + G_LSB_DELTA, (uint32_t)slot->lsb_delta); W32(g + G_RSB_DELTA, (uint32_t)slot->rsb_delta);
    // `other` et `internal` sont privés au pilote : laissés à 0 (jamais lus ici).
}

static void sync_face(CPU *c, Face *f) {
    sync_size(f); sync_charmap(f); sync_glyph(c, f);
}

// Construit le miroir complet d'une face hôte nouvellement ouverte.
static Face *mirror_face(CPU *c, FT_Face host, uint32_t library) {
    uint32_t charmaps = (uint32_t)(host->num_charmaps > 0 ? host->num_charmaps : 0);
    uint32_t fixed = (uint32_t)(host->num_fixed_sizes > 0 && host->available_sizes ? host->num_fixed_sizes : 0);
    size_t family = host->family_name ? strlen(host->family_name) + 1 : 0;
    size_t style = host->style_name ? strlen(host->style_name) + 1 : 0;
    uint64_t total = CHARMAPS_OFFSET + (uint64_t)charmaps * (CHARMAP_SIZE + 4) + (uint64_t)fixed * BITMAP_SIZE_SIZE + family + style;
    if (total > 0x1000000) rt_trap(c, c->eip, "FT_Face mirror too large");
    Face *f = calloc(1, sizeof(*f));
    if (!f) rt_trap(c, c->eip, "out of host memory (FT_Face)");
    f->host = host; f->library = library;
    f->block.capacity = (uint32_t)total; f->block.address = guest_alloc(c, (uint32_t)total);
    uint32_t g = f->guest = f->block.address;
    f->charmap_records = g + CHARMAPS_OFFSET;
    uint32_t pointers = f->charmap_records + charmaps * CHARMAP_SIZE;
    uint32_t sizes = pointers + charmaps * 4;
    uint32_t strings = sizes + fixed * BITMAP_SIZE_SIZE;

    W32(g + F_NUM_FACES, (uint32_t)host->num_faces); W32(g + F_FACE_INDEX, (uint32_t)host->face_index);
    W32(g + F_FACE_FLAGS, (uint32_t)host->face_flags); W32(g + F_STYLE_FLAGS, (uint32_t)host->style_flags);
    W32(g + F_NUM_GLYPHS, (uint32_t)host->num_glyphs);
    if (family) { memcpy(GPTR(strings), host->family_name, family); W32(g + F_FAMILY, strings); }
    if (style) { memcpy(GPTR(strings + family), host->style_name, style); W32(g + F_STYLE, strings + (uint32_t)family); }
    W32(g + F_NUM_FIXED, (uint32_t)host->num_fixed_sizes);
    W32(g + F_SIZES, fixed ? sizes : 0);
    for (uint32_t i = 0; i < fixed; ++i) {
        const FT_Bitmap_Size *s = &host->available_sizes[i];
        uint32_t a = sizes + i * BITMAP_SIZE_SIZE;
        W16(a, (uint16_t)s->height); W16(a + 2, (uint16_t)s->width);
        W32(a + 4, (uint32_t)s->size); W32(a + 8, (uint32_t)s->x_ppem); W32(a + 12, (uint32_t)s->y_ppem);
    }
    W32(g + F_NUM_CHARMAPS, (uint32_t)host->num_charmaps);
    W32(g + F_CHARMAPS, charmaps ? pointers : 0);
    for (uint32_t i = 0; i < charmaps; ++i) {
        const FT_CharMapRec *m = host->charmaps[i];
        uint32_t a = f->charmap_records + i * CHARMAP_SIZE;
        W32(a, g); W32(a + 4, m->encoding); W16(a + 8, m->platform_id); W16(a + 10, m->encoding_id);
        W32(pointers + 4 * i, a);
    }
    const FT_Pos bbox[4] = {host->bbox.xMin, host->bbox.yMin, host->bbox.xMax, host->bbox.yMax};
    for (int i = 0; i < 4; ++i) W32(g + F_BBOX + 4 * i, (uint32_t)bbox[i]);
    W16(g + F_UNITS_PER_EM, host->units_per_EM);
    W16(g + F_ASCENDER, (uint16_t)host->ascender); W16(g + F_DESCENDER, (uint16_t)host->descender);
    W16(g + F_HEIGHT, (uint16_t)host->height);
    W16(g + F_MAX_ADV_W, (uint16_t)host->max_advance_width); W16(g + F_MAX_ADV_H, (uint16_t)host->max_advance_height);
    W16(g + F_UNDERLINE_POS, (uint16_t)host->underline_position);
    W16(g + F_UNDERLINE_THICK, (uint16_t)host->underline_thickness);
    // generic, driver, memory, stream, sizes_list, autohint, extensions, internal :
    // privés à FreeType, laissés à 0.
    sync_face(c, f);
    pthread_mutex_lock(&lock);
    f->next = faces; faces = f;
    pthread_mutex_unlock(&lock);
    return f;
}

static void release_face(Face *f) {
    buffer_release(&f->block); buffer_release(&f->bitmap); buffer_release(&f->outline);
    buffer_release(&f->control); rt_free(f->postscript);
    free(f);
}

static Face *unlink_face(uint32_t guest) {
    pthread_mutex_lock(&lock);
    Face **p = &faces;
    while (*p && (*p)->guest != guest) p = &(*p)->next;
    Face *f = *p;
    if (f) *p = f->next;
    pthread_mutex_unlock(&lock);
    return f;
}

// Fin commune de FT_New_Face / FT_New_Memory_Face (sémantique FT_Open_Face).
static void open_result(CPU *c, FT_Error error, FT_Face host, uint32_t library, uint32_t aface) {
    if (aface) W32(aface, !error && host ? mirror_face(c, host, library)->guest : 0);
    c->eax = (uint32_t)error;
}

static int freetype_shim(CPU *c, const char *name) {
    if (!strcmp(name, "FT_Init_FreeType")) {
        uint32_t out = arg(c, 0);
        if (out) rt_write_span(c, out, 4);
        FT_Library library = NULL;
        FT_Error error = FT_Init_FreeType(out ? &library : NULL);
        if (!error) {
            uint32_t handle = rt_handle_new(library, KIND_LIBRARY);
            if (!handle) { FT_Done_FreeType(library); rt_trap(c, c->eip, "handle table full (FT_Library)"); }
            W32(out, handle);
        }
        c->eax = (uint32_t)error;
    } else if (!strcmp(name, "FT_Done_FreeType")) {
        uint32_t handle = arg(c, 0);
        FT_Library library = library_of(handle);
        if (library) {  // FT_Done_FreeType détruit aussi les faces de la bibliothèque
            pthread_mutex_lock(&lock);
            Face **p = &faces, *dead = NULL;
            while (*p) {
                if ((*p)->library == handle) { Face *f = *p; *p = f->next; f->next = dead; dead = f; }
                else p = &(*p)->next;
            }
            pthread_mutex_unlock(&lock);
            c->eax = (uint32_t)FT_Done_FreeType(library);
            while (dead) { Face *next = dead->next; release_face(dead); dead = next; }
            rt_handle_free(handle, KIND_LIBRARY);
        } else c->eax = (uint32_t)FT_Done_FreeType(NULL);
    } else if (!strcmp(name, "FT_New_Face")) {
        uint32_t handle = arg(c, 0), path = arg(c, 1), aface = arg(c, 3);
        FT_Long index = (int32_t)arg(c, 2);
        if (aface) rt_write_span(c, aface, 4);
        const char *guest_path = host_string(c, path);
        const char *host_path = guest_path ? rt_read_path(guest_path) : NULL;
        FT_Face face = NULL; FT_Error error;
        if (guest_path && !host_path) error = FT_Err_Cannot_Open_Resource;
        else error = FT_New_Face(library_of(handle), host_path, index, aface ? &face : NULL);
        open_result(c, error, face, handle, aface);
    } else if (!strcmp(name, "FT_New_Memory_Face")) {
        uint32_t handle = arg(c, 0), base = arg(c, 1), size = arg(c, 2), aface = arg(c, 4);
        FT_Long index = (int32_t)arg(c, 3);
        if (aface) rt_write_span(c, aface, 4);
        // FreeType ne copie pas : le tampon invité doit vivre autant que la face,
        // exactement comme sous Linux. La mémoire invitée ne bouge jamais.
        const FT_Byte *data = base ? rt_span(c, base, size) : NULL;
        FT_Face face = NULL;
        FT_Error error = FT_New_Memory_Face(library_of(handle), data, (FT_Long)size, index, aface ? &face : NULL);
        open_result(c, error, face, handle, aface);
    } else if (!strcmp(name, "FT_Done_Face")) {
        uint32_t guest = arg(c, 0);
        if (!guest) { c->eax = (uint32_t)FT_Done_Face(NULL); return 1; }
        Face *f = unlink_face(guest);
        if (!f) rt_trap(c, c->eip, "FT_Done_Face on an unknown FT_Face");
        c->eax = (uint32_t)FT_Done_Face(f->host);
        release_face(f);
    } else if (!strcmp(name, "FT_Select_Charmap")) {
        Face *f = find_face(c, arg(c, 0));
        c->eax = (uint32_t)FT_Select_Charmap(f ? f->host : NULL, (FT_Encoding)arg(c, 1));
        if (f) sync_charmap(f);
    } else if (!strcmp(name, "FT_Set_Pixel_Sizes")) {
        Face *f = find_face(c, arg(c, 0));
        c->eax = (uint32_t)FT_Set_Pixel_Sizes(f ? f->host : NULL, arg(c, 1), arg(c, 2));
        if (f) sync_size(f);
    } else if (!strcmp(name, "FT_Request_Size")) {
        Face *f = find_face(c, arg(c, 0));
        uint32_t req = arg(c, 1);
        FT_Size_RequestRec request = {0};
        if (req) {
            rt_span(c, req, 20);
            request.type = (FT_Size_Request_Type)R32(req);
            request.width = (int32_t)R32(req + 4); request.height = (int32_t)R32(req + 8);
            request.horiResolution = R32(req + 12); request.vertResolution = R32(req + 16);
        }
        c->eax = (uint32_t)FT_Request_Size(f ? f->host : NULL, req ? &request : NULL);
        if (f) sync_size(f);
    } else if (!strcmp(name, "FT_Select_Size")) {
        Face *f = find_face(c, arg(c, 0));
        c->eax = (uint32_t)FT_Select_Size(f ? f->host : NULL, (FT_Int)(int32_t)arg(c, 1));
        if (f) sync_size(f);
    } else if (!strcmp(name, "FT_Get_Char_Index")) {
        Face *f = find_face(c, arg(c, 0));
        c->eax = FT_Get_Char_Index(f ? f->host : NULL, arg(c, 1));
    } else if (!strcmp(name, "FT_Load_Char") || !strcmp(name, "FT_Load_Glyph")) {
        Face *f = find_face(c, arg(c, 0));
        FT_Int32 flags = (FT_Int32)arg(c, 2);
        FT_Face host = f ? f->host : NULL;
        c->eax = (uint32_t)(name[8] == 'C' ? FT_Load_Char(host, arg(c, 1), flags) : FT_Load_Glyph(host, arg(c, 1), flags));
        if (f) sync_face(c, f);  // le chargement peut aussi changer de taille (bitmaps couleur)
    } else if (!strcmp(name, "FT_Get_Kerning")) {
        Face *f = find_face(c, arg(c, 0));
        uint32_t out = arg(c, 4);
        FT_Vector kerning = {0, 0};
        if (out) { rt_write_span(c, out, 8); kerning.x = (int32_t)R32(out); kerning.y = (int32_t)R32(out + 4); }
        c->eax = (uint32_t)FT_Get_Kerning(f ? f->host : NULL, arg(c, 1), arg(c, 2), arg(c, 3), out ? &kerning : NULL);
        if (out) { W32(out, (uint32_t)kerning.x); W32(out + 4, (uint32_t)kerning.y); }
    } else if (!strcmp(name, "FT_Get_Postscript_Name")) {
        Face *f = find_face(c, arg(c, 0));
        const char *ps = FT_Get_Postscript_Name(f ? f->host : NULL);
        if (!ps) c->eax = 0;
        else {  // FreeType met le nom en cache dans la face : même durée de vie ici
            if (!f->postscript || strcmp(GPTR(f->postscript), ps)) { rt_free(f->postscript); f->postscript = guest_string(c, ps); }
            c->eax = f->postscript;
        }
    } else if (!strcmp(name, "FT_MulFix")) {
        // FT_Long vaut 32 bits sur i386 : même calcul que FT_MulFix_i386
        // (arrondi au plus proche, demi loin de zéro), résultat tronqué à 32 bits.
        int64_t ab = (int64_t)(int32_t)arg(c, 0) * (int32_t)arg(c, 1);
        c->eax = (uint32_t)(uint64_t)((ab + 0x8000 - (ab < 0)) >> 16);
    } else return 0;
    return 1;
}

// ---------------------------------------------------------------- Fontconfig

// Fontconfig déréférence le motif sans tester NULL (sauf FcPatternDestroy) :
// un motif nul est une faute du code invité, signalée plutôt que reproduite.
static Pattern *pattern_of(CPU *c, uint32_t handle) {
    if (!handle) rt_trap(c, c->eip, "Fontconfig call on a null FcPattern");
    Pattern *p = rt_handle_get(handle, KIND_PATTERN);
    if (!p) rt_trap(c, c->eip, "Fontconfig call on an unknown FcPattern");
    return p;
}
static FcPattern *host_pattern(CPU *c, uint32_t handle) { return pattern_of(c, handle)->host; }

static FcConfig *config_of(CPU *c, uint32_t handle) {
    if (!handle) return NULL;
    FcConfig *config = rt_handle_get(handle, KIND_CONFIG);
    if (!config) rt_trap(c, c->eip, "Fontconfig call on an unknown FcConfig");
    return config;
}

// Handle invité d'une FcConfig : un seul par configuration, qui garde une
// référence Fontconfig. FcConfigSetCurrent détruit l'ancienne configuration
// courante (FcConfigGetCurrent n'en donne pas de référence) : sans celle-ci,
// un handle déjà rendu à l'invité pointerait sur de la mémoire libérée.
static uint32_t config_handle(CPU *c, FcConfig *config) {
    if (!config) return 0;
    uint32_t handle = rt_handle_find(config, KIND_CONFIG);
    if (handle) return handle;
    if (!(handle = rt_handle_new(config, KIND_CONFIG))) rt_trap(c, c->eip, "handle table full (FcConfig)");
    FcConfigReference(config);
    return handle;
}

static uint32_t wrap_pattern(CPU *c, FcPattern *host) {
    if (!host) return 0;
    Pattern *p = calloc(1, sizeof(*p));
    if (!p) rt_trap(c, c->eip, "out of host memory (FcPattern)");
    p->host = host;
    uint32_t handle = rt_handle_new(p, KIND_PATTERN);
    if (!handle) rt_trap(c, c->eip, "handle table full (FcPattern)");
    return handle;
}

// Libère l'enveloppe ; `destroy` décide si la référence hôte est rendue.
static void drop_pattern(uint32_t handle, int destroy) {
    Pattern *p = rt_handle_free(handle, KIND_PATTERN);
    if (!p) return;
    if (destroy) FcPatternDestroy(p->host);
    for (uint32_t i = 0; i < p->count; ++i) rt_free(p->copies[i]);
    free(p->sources); free(p->copies); free(p);
}

// Les chaînes de FcPatternGet* pointent dans le motif : une copie invitée par
// chaîne, libérée avec le motif.
static uint32_t pattern_string(CPU *c, Pattern *p, const FcChar8 *s) {
    for (uint32_t i = 0; i < p->count; ++i) if (p->sources[i] == s) return p->copies[i];
    const FcChar8 **sources = realloc(p->sources, (p->count + 1) * sizeof(*sources));
    if (sources) p->sources = sources;
    uint32_t *copies = realloc(p->copies, (p->count + 1) * sizeof(*copies));
    if (copies) p->copies = copies;
    if (!sources || !copies) rt_trap(c, c->eip, "out of host memory (FcPattern strings)");
    p->sources[p->count] = s;
    p->copies[p->count] = guest_string(c, (const char *)s);
    return p->copies[p->count++];
}

// Le bundle précompilé de setup.sh compile Fontconfig avec un préfixe neutre
// (/opt/p2-host-deps) : sa configuration est dans <lib>/../etc/fonts. Sans
// FONTCONFIG_PATH (que le lanceur de l'app fixe), on la cherche là ; avec
// Homebrew, ce dossier n'existe pas et le chemin compilé sert.
static void fontconfig_locate(void) {
    if (getenv("FONTCONFIG_PATH") || getenv("FONTCONFIG_FILE")) return;
    Dl_info info;
    if (!dladdr((const void *)FcInit, &info) || !info.dli_fname) return;
    char path[PATH_MAX];
    const char *slash = strrchr(info.dli_fname, '/');
    if (!slash) return;
    int n = snprintf(path, sizeof(path), "%.*s/../etc/fonts/fonts.conf", (int)(slash - info.dli_fname), info.dli_fname);
    if (n <= 0 || (size_t)n >= sizeof(path) || access(path, R_OK)) return;
    path[n - (int)strlen("/fonts.conf")] = 0;
    setenv("FONTCONFIG_PATH", path, 0);
}

static int fontconfig_shim(CPU *c, const char *name) {
    static pthread_once_t located = PTHREAD_ONCE_INIT;
    pthread_once(&located, fontconfig_locate);
    if (!strcmp(name, "FcInit")) {
        c->eax = (uint32_t)FcInit();
    } else if (!strcmp(name, "FcConfigGetCurrent")) {
        c->eax = config_handle(c, FcConfigGetCurrent());
    } else if (!strcmp(name, "FcInitLoadConfigAndFonts")) {
        // Nouvelle configuration : sa référence initiale revient à l'invité,
        // config_handle en prend une pour la table. NULL si Fontconfig échoue.
        c->eax = config_handle(c, FcInitLoadConfigAndFonts());
    } else if (!strcmp(name, "FcConfigSetCurrent")) {
        // FcConfigSetCurrent(NULL) recharge la configuration par défaut (2.13+).
        c->eax = (uint32_t)FcConfigSetCurrent(config_of(c, arg(c, 0)));
    } else if (!strcmp(name, "FcConfigAppFontAddFile")) {
        // Chemin invité (dossier du jeu) traduit comme FT_New_Face ; FcFalse si
        // le fichier est introuvable ou n'est pas une police lisible.
        FcConfig *config = config_of(c, arg(c, 0));
        const char *guest_path = host_string(c, arg(c, 1));
        const char *host_path = guest_path ? rt_read_path(guest_path) : NULL;
        c->eax = host_path ? (uint32_t)FcConfigAppFontAddFile(config, (const FcChar8 *)host_path) : FcFalse;
    } else if (!strcmp(name, "FcPatternCreate")) {
        c->eax = wrap_pattern(c, FcPatternCreate());
    } else if (!strcmp(name, "FcPatternDestroy")) {
        uint32_t handle = arg(c, 0);
        if (handle) { pattern_of(c, handle); drop_pattern(handle, 1); }
    } else if (!strcmp(name, "FcPatternAdd")) {
        // FcPatternAdd(FcPattern*, const char*, FcValue, FcBool) : FcValue i386 de
        // 12 octets passé par valeur (type, puis union alignée sur 4).
        FcPattern *p = host_pattern(c, arg(c, 0));
        const char *object = host_string(c, arg(c, 1));
        FcValue value = {0};
        value.type = (FcType)(int32_t)arg(c, 2);
        switch (value.type) {
        case FcTypeVoid: break;
        case FcTypeInteger: value.u.i = (int32_t)arg(c, 3); break;
        case FcTypeBool: value.u.b = (FcBool)(int32_t)arg(c, 3); break;
        case FcTypeDouble: { uint64_t bits = (uint64_t)arg(c, 3) | (uint64_t)arg(c, 4) << 32; memcpy(&value.u.d, &bits, 8); break; }
        case FcTypeString: value.u.s = (const FcChar8 *)host_string(c, arg(c, 3)); break;  // copiée par FcValueSave
        case FcTypeMatrix: {
            uint32_t m = arg(c, 3); FcMatrix matrix;
            if (m) { rt_span(c, m, 32); matrix.xx = RD(m); matrix.xy = RD(m + 8); matrix.yx = RD(m + 16); matrix.yy = RD(m + 24); }
            value.u.m = m ? &matrix : NULL;
            c->eax = (uint32_t)FcPatternAdd(p, object, value, (FcBool)arg(c, 5));
            return 1;
        }
        default: rt_trap(c, c->eip, "FcPatternAdd value type not bridged (charset/langset/range/FT_Face)");
        }
        c->eax = (uint32_t)FcPatternAdd(p, object, value, (FcBool)arg(c, 5));
    } else if (!strcmp(name, "FcPatternGetString")) {
        Pattern *p = pattern_of(c, arg(c, 0));
        uint32_t out = arg(c, 3);
        FcChar8 *s = NULL;
        rt_write_span(c, out, 4);
        FcResult result = FcPatternGetString(p->host, host_string(c, arg(c, 1)), (int)(int32_t)arg(c, 2), &s);
        if (result == FcResultMatch) W32(out, s ? pattern_string(c, p, s) : 0);
        c->eax = (uint32_t)result;
    } else if (!strcmp(name, "FcPatternGetBool")) {
        FcPattern *p = host_pattern(c, arg(c, 0));
        uint32_t out = arg(c, 3);
        FcBool b = 0;
        rt_write_span(c, out, 4);
        FcResult result = FcPatternGetBool(p, host_string(c, arg(c, 1)), (int)(int32_t)arg(c, 2), &b);
        if (result == FcResultMatch) W32(out, (uint32_t)b);
        c->eax = (uint32_t)result;
    } else if (!strcmp(name, "FcConfigSubstitute")) {
        c->eax = (uint32_t)FcConfigSubstitute(config_of(c, arg(c, 0)), host_pattern(c, arg(c, 1)), (FcMatchKind)arg(c, 2));
    } else if (!strcmp(name, "FcDefaultSubstitute")) {
        FcDefaultSubstitute(host_pattern(c, arg(c, 0)));
    } else if (!strcmp(name, "FcFontMatch")) {
        uint32_t out = arg(c, 2);
        rt_write_span(c, out, 4);
        FcResult result = FcResultNoMatch;
        FcPattern *match = FcFontMatch(config_of(c, arg(c, 0)), host_pattern(c, arg(c, 1)), &result);
        W32(out, (uint32_t)result);
        c->eax = wrap_pattern(c, match);
    } else if (!strcmp(name, "FcObjectSetCreate")) {
        FcObjectSet *os = FcObjectSetCreate();
        c->eax = os ? rt_handle_new(os, KIND_OBJSET) : 0;
        if (os && !c->eax) rt_trap(c, c->eip, "handle table full (FcObjectSet)");
    } else if (!strcmp(name, "FcObjectSetAdd")) {
        FcObjectSet *os = rt_handle_get(arg(c, 0), KIND_OBJSET);
        if (!os) rt_trap(c, c->eip, "FcObjectSetAdd on an unknown FcObjectSet");
        c->eax = (uint32_t)FcObjectSetAdd(os, host_string(c, arg(c, 1)));
    } else if (!strcmp(name, "FcObjectSetDestroy")) {
        uint32_t handle = arg(c, 0);
        FcObjectSet *os = handle ? rt_handle_free(handle, KIND_OBJSET) : NULL;
        if (handle && !os) rt_trap(c, c->eip, "FcObjectSetDestroy on an unknown FcObjectSet");
        if (os) FcObjectSetDestroy(os);
    } else if (!strcmp(name, "FcFontList")) {
        uint32_t os_handle = arg(c, 2);
        FcObjectSet *os = os_handle ? rt_handle_get(os_handle, KIND_OBJSET) : NULL;
        if (os_handle && !os) rt_trap(c, c->eip, "FcFontList on an unknown FcObjectSet");
        FcFontSet *set = FcFontList(config_of(c, arg(c, 0)), host_pattern(c, arg(c, 1)), os);
        if (!set) { c->eax = 0; return 1; }
        // FcFontSet i386 : { int nfont; int sfont; FcPattern **fonts; }.
        // Les motifs restent possédés par l'ensemble (FcFontSetDestroy).
        uint32_t count = set->nfont > 0 ? (uint32_t)set->nfont : 0;
        FontSet *fs = calloc(1, sizeof(*fs));
        uint32_t *patterns = calloc(count ? count : 1, sizeof(*patterns));
        if (!fs || !patterns) rt_trap(c, c->eip, "out of host memory (FcFontSet)");
        fs->host = set; fs->count = count; fs->patterns = patterns;
        fs->guest = guest_alloc(c, 12 + 4 * count);
        W32(fs->guest, count); W32(fs->guest + 4, count); W32(fs->guest + 8, count ? fs->guest + 12 : 0);
        for (uint32_t i = 0; i < count; ++i) W32(fs->guest + 12 + 4 * i, patterns[i] = wrap_pattern(c, set->fonts[i]));
        pthread_mutex_lock(&lock);
        fs->next = font_sets; font_sets = fs;
        pthread_mutex_unlock(&lock);
        c->eax = fs->guest;
    } else if (!strcmp(name, "FcFontSetDestroy")) {
        uint32_t guest = arg(c, 0);
        pthread_mutex_lock(&lock);
        FontSet **p = &font_sets;
        while (*p && (*p)->guest != guest) p = &(*p)->next;
        FontSet *fs = *p;
        if (fs) *p = fs->next;
        pthread_mutex_unlock(&lock);
        if (!fs) rt_trap(c, c->eip, "FcFontSetDestroy on an unknown FcFontSet");
        for (uint32_t i = 0; i < fs->count; ++i) drop_pattern(fs->patterns[i], 0);
        FcFontSetDestroy(fs->host);
        rt_free(fs->guest); free(fs->patterns); free(fs);
    } else return 0;
    return 1;
}

static int fonts_shim(CPU *c, const char *name) {
    if (name[0] == 'F' && name[1] == 'T' && name[2] == '_') return freetype_shim(c, name);
    if (name[0] == 'F' && name[1] == 'c') return fontconfig_shim(c, name);
    return 0;
}
RT_SHIM_PROVIDER(fonts_shim)
