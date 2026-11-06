// Pont FreeType/Fontconfig : appels au format cdecl i386 sur la pile invitée,
// miroirs FT_FaceRec/FT_GlyphSlotRec/FcFontSet comparés à FreeType hôte.
#include "checks.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include <fontconfig/fontconfig.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/stat.h>
#include <unistd.h>

#define STACK 0x20018000u
#define SCRATCH 0x20019000u   // variables de sortie invitées
#define STRINGS 0x2001a000u

static uint32_t call(const char *name, unsigned count, ...) {
    CPU c; rt_cpu_reset(&c);
    c.esp = STACK - 4 * (count + 1);
    W32(c.esp, RT_RETURN_SENTINEL);
    va_list ap; va_start(ap, count);
    for (unsigned i = 0; i < count; ++i) W32(c.esp + 4 + 4 * i, va_arg(ap, uint32_t));
    va_end(ap);
    assert(rt_shim(&c, name));
    return c.eax;
}
static uint32_t gstr(uint32_t at, const char *s) { memcpy(GPTR(at), s, strlen(s) + 1); return at; }

static const char *test_font(void) {
    static const char *candidates[] = {"/System/Library/Fonts/Supplemental/Arial.ttf",
                                       "/System/Library/Fonts/Supplemental/Verdana.ttf",
                                       "/System/Library/Fonts/Geneva.ttf"};
    for (unsigned i = 0; i < sizeof(candidates) / sizeof(*candidates); ++i) {
        FILE *f = fopen(candidates[i], "rb");
        if (f) { fclose(f); return candidates[i]; }
    }
    assert(!"no TrueType system font for the FreeType bridge checks");
    return NULL;
}

RT_CHECK(fonts_mulfix_is_32bit_i386) {
    assert(call("FT_MulFix", 2, 3u << 16, 0x8000u) == 0x18000u);
    assert(call("FT_MulFix", 2, 1u, 0x8000u) == 1u);                       // 0.5 arrondi vers le haut
    assert(call("FT_MulFix", 2, (uint32_t)-1, 0x8000u) == (uint32_t)-1);  // -0.5 loin de zéro
    assert(call("FT_MulFix", 2, (uint32_t)-3, 0x10000u) == (uint32_t)-3);
    assert(call("FT_MulFix", 2, 0x7fffffffu, 0x20000u) == 0xfffffffeu);  // FT_Long tronqué à 32 bits
}

static void compare_glyph(uint32_t face, FT_Face host) {
    uint32_t slot = R32(face + 0x54);
    assert(slot && R32(slot + 0x04) == face);
    FT_GlyphSlot g = host->glyph;
    assert(R32(slot + 0x0c) == g->glyph_index);
    assert((int32_t)R32(slot + 0x18) == g->metrics.width && (int32_t)R32(slot + 0x1c) == g->metrics.height);
    assert((int32_t)R32(slot + 0x20) == g->metrics.horiBearingX && (int32_t)R32(slot + 0x24) == g->metrics.horiBearingY);
    assert((int32_t)R32(slot + 0x28) == g->metrics.horiAdvance && (int32_t)R32(slot + 0x34) == g->metrics.vertAdvance);
    assert((int32_t)R32(slot + 0x38) == g->linearHoriAdvance);
    assert((int32_t)R32(slot + 0x40) == g->advance.x && (int32_t)R32(slot + 0x44) == g->advance.y);
    assert(R32(slot + 0x48) == g->format);
    assert(R32(slot + 0x4c) == g->bitmap.rows && R32(slot + 0x50) == g->bitmap.width);
    assert((int32_t)R32(slot + 0x54) == g->bitmap.pitch);
    assert(R16(slot + 0x5c) == g->bitmap.num_grays && R8(slot + 0x5e) == g->bitmap.pixel_mode);
    assert((int32_t)R32(slot + 0x64) == g->bitmap_left && (int32_t)R32(slot + 0x68) == g->bitmap_top);
    uint32_t bytes = g->bitmap.buffer ? g->bitmap.rows * (uint32_t)abs(g->bitmap.pitch) : 0;
    uint32_t buffer = R32(slot + 0x58);
    assert(bytes ? buffer && !memcmp(GPTR(buffer), g->bitmap.buffer, bytes) : !buffer);
    assert(R16(slot + 0x6e) == g->outline.n_points && (int16_t)R16(slot + 0x6c) == g->outline.n_contours);
    if (g->outline.n_points) {
        uint32_t points = R32(slot + 0x70), last = (uint32_t)g->outline.n_points - 1;
        assert((int32_t)R32(points + 8 * last) == g->outline.points[last].x);
        assert((int32_t)R32(points + 8 * last + 4) == g->outline.points[last].y);
        assert(!memcmp(GPTR(R32(slot + 0x74)), g->outline.tags, last + 1));
        assert(R16(R32(slot + 0x78)) == g->outline.contours[0]);
    }
    assert((int32_t)R32(slot + 0x90) == g->lsb_delta && (int32_t)R32(slot + 0x94) == g->rsb_delta);
}

RT_CHECK(fonts_freetype_face_mirror_matches_host) {
    const char *path = test_font();
    FT_Library hl; assert(!FT_Init_FreeType(&hl));
    FT_Face host; assert(!FT_New_Face(hl, path, 0, &host));

    assert(call("FT_Init_FreeType", 1, SCRATCH) == 0);
    uint32_t lib = R32(SCRATCH);
    assert(lib >= RT_HANDLE_BASE);
    W32(SCRATCH + 4, 0xdeadbeef);
    assert(call("FT_New_Face", 4, lib, gstr(STRINGS, "/nonexistent/font.ttf"), 0, SCRATCH + 4) == FT_Err_Cannot_Open_Resource);
    assert(R32(SCRATCH + 4) == 0);
    assert(call("FT_New_Face", 4, lib, gstr(STRINGS, path), 0, SCRATCH + 4) == 0);
    uint32_t face = R32(SCRATCH + 4);
    assert(face && face < RT_HANDLE_BASE);

    // FT_FaceRec i386
    assert((int32_t)R32(face + 0x00) == host->num_faces && (int32_t)R32(face + 0x10) == host->num_glyphs);
    assert(R32(face + 0x08) == (uint32_t)host->face_flags && (R32(face + 0x08) & FT_FACE_FLAG_SCALABLE));
    assert(!strcmp(GPTR(R32(face + 0x14)), host->family_name) && !strcmp(GPTR(R32(face + 0x18)), host->style_name));
    assert(R32(face + 0x1c) == (uint32_t)host->num_fixed_sizes);
    assert((int32_t)R32(face + 0x24) == host->num_charmaps && host->num_charmaps > 0);
    assert((int32_t)R32(face + 0x38) == host->bbox.yMin && (int32_t)R32(face + 0x40) == host->bbox.yMax);
    assert(R16(face + 0x44) == host->units_per_EM && (int16_t)R16(face + 0x46) == host->ascender);
    assert((int16_t)R16(face + 0x48) == host->descender && (int16_t)R16(face + 0x4a) == host->height);
    uint32_t charmaps = R32(face + 0x28), active = R32(face + 0x5c);
    int found = 0;
    for (int i = 0; i < host->num_charmaps; ++i) {
        uint32_t m = R32(charmaps + 4 * (uint32_t)i);
        assert(R32(m) == face && R32(m + 4) == host->charmaps[i]->encoding);
        assert(R16(m + 8) == host->charmaps[i]->platform_id && R16(m + 10) == host->charmaps[i]->encoding_id);
        if (host->charmaps[i] == host->charmap) { assert(m == active); found = 1; }
    }
    assert(found && R32(active + 4) == FT_ENCODING_UNICODE);
    uint32_t size = R32(face + 0x58);
    assert(size && R32(size) == face);

    // Taille en pixels puis requête de taille (FT_Size_RequestRec, 20 octets).
    assert(call("FT_Set_Pixel_Sizes", 3, face, 0, 32) == 0);
    assert(!FT_Set_Pixel_Sizes(host, 0, 32));
    assert(R16(size + 0x0e) == 32 && (int32_t)R32(size + 0x14) == host->size->metrics.y_scale);
    assert((int32_t)R32(size + 0x18) == host->size->metrics.ascender && (int32_t)R32(size + 0x1c) == host->size->metrics.descender);
    assert((int32_t)R32(size + 0x24) == host->size->metrics.max_advance && R32(size + 0x18) > 0);
    W32(SCRATCH + 16, FT_SIZE_REQUEST_TYPE_NOMINAL); W32(SCRATCH + 20, 0); W32(SCRATCH + 24, 20 << 6);
    W32(SCRATCH + 28, 0); W32(SCRATCH + 32, 0);
    assert(call("FT_Request_Size", 2, face, SCRATCH + 16) == 0);
    assert(R16(size + 0x0e) == 20);
    assert(call("FT_Request_Size", 2, face, 0) == FT_Err_Invalid_Argument);
    assert(call("FT_Select_Size", 2, face, 0) == (uint32_t)FT_Select_Size(host, 0));
    assert(call("FT_Set_Pixel_Sizes", 3, face, 0, 32) == 0);

    // Rendu d'un glyphe : bitmap recopié en mémoire invitée.
    uint32_t index = call("FT_Get_Char_Index", 2, face, 'A');
    assert(index && index == FT_Get_Char_Index(host, 'A'));
    assert(call("FT_Get_Char_Index", 2, face, 0x10ffff) == 0);
    assert(call("FT_Load_Char", 3, face, 'A', FT_LOAD_RENDER) == 0);
    assert(!FT_Load_Char(host, 'A', FT_LOAD_RENDER));
    compare_glyph(face, host);
    uint32_t slot = R32(face + 0x54), rows = R32(slot + 0x4c), width = R32(slot + 0x50);
    assert(rows > 10 && width > 10 && R8(slot + 0x5e) == FT_PIXEL_MODE_GRAY && R32(slot + 0x48) == FT_GLYPH_FORMAT_BITMAP);
    uint32_t ink = 0;
    for (uint32_t i = 0; i < rows * width; ++i) ink += R8(R32(slot + 0x58) + i) != 0;
    assert(ink > 20);
    uint32_t o = call("FT_Get_Char_Index", 2, face, 'o');
    assert(call("FT_Load_Glyph", 3, face, o, FT_LOAD_NO_BITMAP) == 0);
    assert(!FT_Load_Glyph(host, o, FT_LOAD_NO_BITMAP));
    compare_glyph(face, host);
    assert(R32(slot + 0x48) == FT_GLYPH_FORMAT_OUTLINE && R16(slot + 0x6e) > 0);
    assert(call("FT_Load_Glyph", 3, face, 0xfffffu, FT_LOAD_RENDER) == (uint32_t)FT_Load_Glyph(host, 0xfffff, FT_LOAD_RENDER));

    // Crénage (FT_Vector i386 écrit par pointeur), même résultat que l'hôte.
    uint32_t a = FT_Get_Char_Index(host, 'A'), v = FT_Get_Char_Index(host, 'V');
    FT_Vector k; FT_Error kerror = FT_Get_Kerning(host, a, v, FT_KERNING_DEFAULT, &k);
    W32(SCRATCH + 8, 0x55555555); W32(SCRATCH + 12, 0x55555555);
    assert(call("FT_Get_Kerning", 5, face, a, v, FT_KERNING_DEFAULT, SCRATCH + 8) == (uint32_t)kerror);
    assert((int32_t)R32(SCRATCH + 8) == k.x && (int32_t)R32(SCRATCH + 12) == k.y);
    assert(!strstr(path, "Arial") || (k.x < 0 && (R32(face + 0x08) & FT_FACE_FLAG_KERNING)));
    assert(call("FT_Get_Kerning", 5, face, a, v, FT_KERNING_DEFAULT, 0) == FT_Err_Invalid_Argument);

    // Nom PostScript : chaîne invitée stable.
    uint32_t ps = call("FT_Get_Postscript_Name", 1, face);
    assert(ps && !strcmp(GPTR(ps), FT_Get_Postscript_Name(host)) && call("FT_Get_Postscript_Name", 1, face) == ps);
    assert(call("FT_Get_Postscript_Name", 1, 0) == 0);

    // Changement de table de caractères (Apple Roman, demandé par le jeu).
    FT_Error selected = FT_Select_Charmap(host, FT_ENCODING_APPLE_ROMAN);
    assert(call("FT_Select_Charmap", 2, face, FT_ENCODING_APPLE_ROMAN) == (uint32_t)selected);
    if (!selected) assert(R32(R32(face + 0x5c) + 4) == FT_ENCODING_APPLE_ROMAN);

    // Face en mémoire : le tampon invité est utilisé sans copie.
    FILE *f = fopen(path, "rb"); assert(f);
    fseek(f, 0, SEEK_END); long length = ftell(f); fseek(f, 0, SEEK_SET);
    uint32_t data = rt_alloc((uint32_t)length); assert(data);
    assert(fread(GPTR(data), 1, (size_t)length, f) == (size_t)length); fclose(f);
    assert(call("FT_New_Memory_Face", 5, lib, data, (uint32_t)length, 0, SCRATCH + 4) == 0);
    uint32_t memory_face = R32(SCRATCH + 4);
    assert(memory_face != face && (int32_t)R32(memory_face + 0x10) == host->num_glyphs);
    assert(call("FT_Set_Pixel_Sizes", 3, memory_face, 0, 32) == 0);
    assert(call("FT_Load_Char", 3, memory_face, 'A', FT_LOAD_RENDER) == 0);
    FT_Select_Charmap(host, FT_ENCODING_UNICODE);
    assert(!FT_Load_Char(host, 'A', FT_LOAD_RENDER));
    compare_glyph(memory_face, host);
    FT_Face truncated = NULL;
    FT_Error truncated_error = FT_New_Memory_Face(hl, GPTR(data), 16, 0, &truncated);
    W32(SCRATCH + 4, 0x1234);
    assert(truncated_error && call("FT_New_Memory_Face", 5, lib, data, 16, 0, SCRATCH + 4) == (uint32_t)truncated_error);
    assert(R32(SCRATCH + 4) == 0);

    assert(call("FT_Done_Face", 1, face) == 0);
    assert(call("FT_Done_Face", 1, 0) == FT_Err_Invalid_Face_Handle);
    // FT_Done_FreeType libère aussi les faces restantes (memory_face).
    assert(call("FT_Done_FreeType", 1, lib) == 0);
    assert(!rt_handle_get(lib, 0x46544c42u));
    rt_free(data);
    FT_Done_Face(host); FT_Done_FreeType(hl);
}

RT_CHECK(fonts_fontconfig_match_and_list) {
    assert(call("FcInit", 0) == 1);
    uint32_t config = call("FcConfigGetCurrent", 0);
    assert(config >= RT_HANDLE_BASE && call("FcConfigGetCurrent", 0) == config);

    // FcPatternAdd(p, objet, FcValue sur 12 octets, append)
    uint32_t p = call("FcPatternCreate", 0);
    assert(p >= RT_HANDLE_BASE);
    assert(call("FcPatternAdd", 6, p, gstr(STRINGS, "family"), FcTypeString, gstr(STRINGS + 64, "Arial"), 0, 0) == 1);
    assert(call("FcPatternAdd", 6, p, gstr(STRINGS + 128, "weight"), FcTypeInteger, 80, 0, 0) == 1);
    assert(call("FcPatternAdd", 6, p, gstr(STRINGS + 192, "size"), FcTypeDouble, 0, 0x40280000u, 0) == 1);  // 12.0
    assert(call("FcConfigSubstitute", 3, 0, p, FcMatchPattern) == 1);
    call("FcDefaultSubstitute", 1, p);
    W32(SCRATCH, 0x77);
    uint32_t match = call("FcFontMatch", 3, 0, p, SCRATCH);
    assert(match && R32(SCRATCH) == FcResultMatch);

    // Même requête directement sur l'hôte : même fichier.
    FcPattern *hp = FcPatternBuild(NULL, FC_FAMILY, FcTypeString, "Arial", FC_WEIGHT, FcTypeInteger, 80,
                                   FC_SIZE, FcTypeDouble, 12.0, (char *)NULL);
    FcConfigSubstitute(NULL, hp, FcMatchPattern); FcDefaultSubstitute(hp);
    FcResult hr; FcPattern *hm = FcFontMatch(NULL, hp, &hr); assert(hm);
    FcChar8 *hfile; assert(FcPatternGetString(hm, FC_FILE, 0, &hfile) == FcResultMatch);

    assert(call("FcPatternGetString", 4, match, gstr(STRINGS, "file"), 0, SCRATCH + 4) == FcResultMatch);
    uint32_t file = R32(SCRATCH + 4);
    assert(file && !strcmp(GPTR(file), (const char *)hfile));
    assert(call("FcPatternGetString", 4, match, STRINGS, 0, SCRATCH + 8) == FcResultMatch && R32(SCRATCH + 8) == file);
    assert(call("FcPatternGetString", 4, match, STRINGS, 7, SCRATCH + 8) == FcResultNoId);
    assert(call("FcPatternGetString", 4, match, gstr(STRINGS + 64, "nonexistentobject"), 0, SCRATCH + 8) == FcResultNoMatch);
    assert(call("FcPatternGetBool", 4, match, gstr(STRINGS + 128, "scalable"), 0, SCRATCH + 12) == FcResultMatch);
    FcBool hb; assert(FcPatternGetBool(hm, FC_SCALABLE, 0, &hb) == FcResultMatch && R32(SCRATCH + 12) == (uint32_t)hb);
    assert(call("FcPatternGetBool", 4, match, gstr(STRINGS + 192, "family"), 0, SCRATCH + 12) == FcResultTypeMismatch);

    // Le fichier renvoyé est ouvrable par FT_New_Face via le chemin invité.
    assert(call("FT_Init_FreeType", 1, SCRATCH + 16) == 0);
    uint32_t lib = R32(SCRATCH + 16);
    assert(call("FT_New_Face", 4, lib, file, 0, SCRATCH + 20) == 0 && R32(SCRATCH + 20));
    assert(call("FT_Done_FreeType", 1, lib) == 0);
    call("FcPatternDestroy", 1, match);
    call("FcPatternDestroy", 1, p);
    call("FcPatternDestroy", 1, 0);
    assert(!rt_handle_get(p, 0x46435054u));
    FcPatternDestroy(hm); FcPatternDestroy(hp);

    // FcFontList : FcFontSet i386 lu directement (nfont, sfont, fonts[i]).
    uint32_t empty = call("FcPatternCreate", 0), os = call("FcObjectSetCreate", 0);
    assert(os >= RT_HANDLE_BASE);
    const char *objects[] = {"file", "fullname", "family", "scalable"};
    for (int i = 0; i < 4; ++i) assert(call("FcObjectSetAdd", 2, os, gstr(STRINGS + 64 * (uint32_t)i, objects[i])) == 1);
    uint32_t set = call("FcFontList", 3, config, empty, os);
    FcPattern *he = FcPatternCreate();
    FcObjectSet *hos = FcObjectSetBuild(FC_FILE, FC_FULLNAME, FC_FAMILY, FC_SCALABLE, (char *)NULL);
    FcFontSet *hset = FcFontList(NULL, he, hos);
    assert(set && set < RT_HANDLE_BASE && hset);
    uint32_t nfont = R32(set);
    assert((int)nfont == hset->nfont && nfont > 10 && R32(set + 8));
    int with_file = 0;
    for (uint32_t i = 0; i < nfont; ++i) {
        uint32_t font = R32(R32(set + 8) + 4 * i);
        assert(font >= RT_HANDLE_BASE);
        if (call("FcPatternGetString", 4, font, STRINGS, 0, SCRATCH) == FcResultMatch) {
            ++with_file;
            FcChar8 *expected; assert(FcPatternGetString(hset->fonts[i], FC_FILE, 0, &expected) == FcResultMatch);
            assert(!strcmp(GPTR(R32(SCRATCH)), (const char *)expected));
        }
    }
    assert(with_file == (int)nfont);
    uint32_t first = R32(R32(set + 8));
    call("FcFontSetDestroy", 1, set);
    assert(!rt_handle_get(first, 0x46435054u));
    call("FcObjectSetDestroy", 1, os);
    call("FcPatternDestroy", 1, empty);
    FcFontSetDestroy(hset); FcObjectSetDestroy(hos); FcPatternDestroy(he);
}

// Polices à tailles fixes et couleur : chemin GetCharRGBA du jeu (FT_Select_Size
// puis FT_LOAD_COLOR, bitmap BGRA) et tableau available_sizes.
RT_CHECK(fonts_freetype_fixed_sizes_and_color) {
    const char *path = "/System/Library/Fonts/Apple Color Emoji.ttc";
    FILE *probe = fopen(path, "rb");
    if (!probe) { printf("skip: %s absent\n", path); return; }
    fclose(probe);
    FT_Library hl; assert(!FT_Init_FreeType(&hl));
    FT_Face host; assert(!FT_New_Face(hl, path, 0, &host));
    assert(call("FT_Init_FreeType", 1, SCRATCH) == 0);
    uint32_t lib = R32(SCRATCH);
    assert(call("FT_New_Face", 4, lib, gstr(STRINGS, path), 0, SCRATCH + 4) == 0);
    uint32_t face = R32(SCRATCH + 4);
    assert((int32_t)R32(face) == host->num_faces && (R32(face + 0x08) & FT_FACE_FLAG_COLOR));
    int strikes = host->num_fixed_sizes;
    assert(strikes > 0 && (int32_t)R32(face + 0x1c) == strikes);
    uint32_t sizes = R32(face + 0x20);
    for (int i = 0; i < strikes; ++i) {
        uint32_t a = sizes + 16 * (uint32_t)i;
        assert((int16_t)R16(a) == host->available_sizes[i].height && (int16_t)R16(a + 2) == host->available_sizes[i].width);
        assert((int32_t)R32(a + 4) == host->available_sizes[i].size && (int32_t)R32(a + 12) == host->available_sizes[i].y_ppem);
    }
    assert(call("FT_Select_Size", 2, face, 0) == 0 && !FT_Select_Size(host, 0));
    uint32_t size = R32(face + 0x58);
    assert(R16(size + 0x0e) == host->size->metrics.y_ppem && (int32_t)R32(size + 0x18) == host->size->metrics.ascender);
    uint32_t smile = FT_Get_Char_Index(host, 0x1f600);
    assert(smile && call("FT_Get_Char_Index", 2, face, 0x1f600) == smile);
    assert(call("FT_Load_Glyph", 3, face, smile, FT_LOAD_RENDER | FT_LOAD_COLOR) == 0);
    assert(!FT_Load_Glyph(host, smile, FT_LOAD_RENDER | FT_LOAD_COLOR));
    compare_glyph(face, host);
    uint32_t slot = R32(face + 0x54);
    assert(R8(slot + 0x5e) == FT_PIXEL_MODE_BGRA && (int32_t)R32(slot + 0x54) == 4 * (int32_t)R32(slot + 0x50));
    assert(call("FT_Done_FreeType", 1, lib) == 0);
    FT_Done_Face(host); FT_Done_FreeType(hl);
}

// Appel par l'adresse dynamique, comme un import invité indirect.
static uint32_t dcall(const char *name, unsigned count, ...) {
    CPU c; rt_cpu_reset(&c);
    c.esp = STACK - 4 * count;
    va_list ap; va_start(ap, count);
    for (unsigned i = 0; i < count; ++i) W32(c.esp + 4 * i, va_arg(ap, uint32_t));
    va_end(ap);
    uint32_t base = c.esp;
    rt_call(&c, rt_dynamic_import(name), RT_RETURN_SENTINEL);
    assert(c.esp == base);
    return c.eax;
}

// FcInitLoadConfigAndFonts / FcConfigSetCurrent / FcConfigAppFontAddFile : la
// police ajoutée n'existe que dans le dépôt Linux, le chemin invité (dossier du
// jeu) n'aboutit que par la traduction rt_read_path.
RT_CHECK(fonts_fontconfig_app_font_and_current_config) {
    assert(call("FcInit", 0) == 1);
    uint32_t previous = dcall("FcConfigGetCurrent", 0);
    uint32_t config = dcall("FcInitLoadConfigAndFonts", 0);
    assert(config >= RT_HANDLE_BASE && config != previous);
    FcConfig *host = rt_handle_get(config, 0x46434346u);
    assert(host && host != FcConfigGetCurrent());

    char root[PATH_MAX], game[PATH_MAX], depot[PATH_MAX], user[PATH_MAX], path[PATH_MAX + 32];
    snprintf(root, sizeof(root), "/tmp/portal2-fonts-XXXXXX"); assert(mkdtemp(root));
    snprintf(game, sizeof(game), "%s/game", root); assert(!mkdir(game, 0755));
    snprintf(depot, sizeof(depot), "%s/linux", root); assert(!mkdir(depot, 0755));
    snprintf(user, sizeof(user), "%s/user", root);
    snprintf(path, sizeof(path), "%s/resource", depot); assert(!mkdir(path, 0755));
    snprintf(path, sizeof(path), "cp '%s' '%s/resource/check.ttf'", test_font(), depot); assert(!system(path));
    assert(!rt_paths_init(game, depot, user));

    snprintf(path, sizeof(path), "%s/resource/check.ttf", game);
    assert(access(path, F_OK));   // absent du dossier du jeu côté hôte
    assert(dcall("FcConfigAppFontAddFile", 2, config, gstr(STRINGS, path)) == FcTrue);
    FcFontSet *app = FcConfigGetFonts(host, FcSetApplication);
    assert(app && app->nfont >= 1);
    FcChar8 *file; assert(FcPatternGetString(app->fonts[app->nfont - 1], FC_FILE, 0, &file) == FcResultMatch);
    char expected[PATH_MAX], got[PATH_MAX];
    snprintf(path, sizeof(path), "%s/resource/check.ttf", depot);
    assert(realpath(path, expected) && realpath((const char *)file, got) && !strcmp(expected, got));
    snprintf(path, sizeof(path), "%s/resource/missing.ttf", game);
    assert(dcall("FcConfigAppFontAddFile", 2, config, gstr(STRINGS, path)) == FcFalse);
    snprintf(path, sizeof(path), "%s/resource", depot);
    FILE *f = fopen(strcat(path, "/notafont.ttf"), "w"); assert(f); fputs("not a font", f); fclose(f);
    snprintf(path, sizeof(path), "%s/resource/notafont.ttf", game);
    assert(dcall("FcConfigAppFontAddFile", 2, config, gstr(STRINGS, path)) == FcFalse);

    // Configuration courante changée puis rétablie : l'ancien handle reste
    // valide (référence gardée par la table) alors que Fontconfig l'a quittée.
    assert(dcall("FcConfigSetCurrent", 1, config) == FcTrue);
    assert(FcConfigGetCurrent() == host && dcall("FcConfigGetCurrent", 0) == config);
    assert(dcall("FcConfigSetCurrent", 1, previous) == FcTrue);
    assert(dcall("FcConfigGetCurrent", 0) == previous && FcConfigGetCurrent() == rt_handle_get(previous, 0x46434346u));

    snprintf(path, sizeof(path), "rm -rf '%s'", root); assert(!system(path));
}
