// GLSL ES 1.00 (sortie de gl4es) -> GLSL ES 3.00, pour un contexte OpenGL ES 3.
//
// gl4es traduit le GLSL 1.20 de togl en GLSL ES 1.00, qui n'a ni textures 3D
// (sampler3D sans précision possible) ni échantillonneurs d'ombre
// (shadow2D, shadow2DLod) : sur iOS, environ 1 400 shaders de Portal 2
// échouaient, et les surfaces s'affichaient sans leurs textures. GLSL ES 3.00
// a tout cela ; la réécriture est textuelle, par identifiants entiers :
// version, extensions devenues standard, attribute/varying -> in/out,
// gl_FragColor -> sortie déclarée, fonctions texture*D* -> texture*,
// shadow2D* -> fonctions d'appoint qui rendent le vec4 de GLSL 1.20.
//
// Émulation (emulate != 0, shaders de fragments), commandée par des uniformes
// que srgb.c règle avant chaque tracé. OpenGL ES 3.0 d'iOS n'a pas :
// - EXT_texture_sRGB_decode : décodage sRGB par unité et par type
//   d'échantillonneur (masques p2_srgb_2d, p2_srgb_cube, p2_srgb_3d) ;
// - EXT_sRGB_write_control : encodage des sorties (masque p2_srgb_write) ;
// - CLAMP_TO_BORDER (gl4es le change en CLAMP_TO_EDGE) : hors de [0,1], la
//   carte d'ombre et la texture projetée de la lumière étiraient leurs texels
//   de bord sur les murs. Bordure noire (seule couleur que togl demande) par
//   axe et par unité (p2_border_s, p2_border_t).
// Chaque appel « f( samplerN, ... ) » devient « p2_f( N, samplerN, ... ) ».
#include <ctype.h>
#include <stdio.h>
#include "env_ios.h"
#include <stdlib.h>
#include <string.h>

typedef struct { char *data; size_t length, capacity; } Buffer;
static int put(Buffer *b, const char *s, size_t n) {
    if (b->length + n + 1 > b->capacity) {
        size_t capacity = (b->length + n + 1) * 2;
        char *grown = realloc(b->data, capacity);
        if (!grown) return -1;
        b->data = grown; b->capacity = capacity;
    }
    memcpy(b->data + b->length, s, n);
    b->length += n; b->data[b->length] = 0;
    return 0;
}
static int puts_(Buffer *b, const char *s) { return put(b, s, strlen(s)); }

static int ident_char(int c) { return isalnum(c) || c == '_'; }

// Identifiants renommés (mot entier). Les deux tableaux dépendent du stade.
typedef struct { const char *from, *to; } Rename;
static const Rename common[] = {
    {"texture2D", "texture"}, {"texture2DProj", "textureProj"}, {"texture2DLod", "textureLod"},
    {"texture2DLodEXT", "textureLod"}, {"texture2DProjLod", "textureProjLod"}, {"texture2DProjLodEXT", "textureProjLod"},
    {"texture2DGradEXT", "textureGrad"}, {"textureCube", "texture"}, {"textureCubeLod", "textureLod"},
    {"textureCubeLodEXT", "textureLod"}, {"textureCubeGradEXT", "textureGrad"}, {"texture3D", "texture"},
    {"texture3DLod", "textureLod"}, {"texture3DProj", "textureProj"}, {"texture3DProjLod", "textureProjLod"},
    {"shadow2D", "gl4es_shadow2D"}, {"shadow2DEXT", "gl4es_shadow2D"}, {"shadow2DLod", "gl4es_shadow2DLod"},
    {"shadow2DProj", "gl4es_shadow2DProj"}, {"shadow2DProjEXT", "gl4es_shadow2DProj"},
    {"shadow2DProjLod", "gl4es_shadow2DProjLod"}, {"gl_FragDepthEXT", "gl_FragDepth"},
};
static const Rename vertex[] = {{"attribute", "in"}, {"varying", "out"}};
static const Rename fragment[] = {{"varying", "in"}, {"gl_FragColor", "gl4es_FragColor"},
                                  {"gl_FragData", "gl4es_FragData"}};

static const char *renamed(const char *word, size_t n, int is_fragment) {
    const Rename *tables[2] = {common, is_fragment ? fragment : vertex};
    size_t sizes[2] = {sizeof(common) / sizeof(*common),
                       is_fragment ? sizeof(fragment) / sizeof(*fragment) : sizeof(vertex) / sizeof(*vertex)};
    for (int t = 0; t < 2; ++t)
        for (size_t i = 0; i < sizes[t]; ++i)
            if (strlen(tables[t][i].from) == n && !strncmp(word, tables[t][i].from, n)) return tables[t][i].to;
    return NULL;
}

// Extensions de GLSL ES 1.00 devenues standard (ou inutiles) en 3.00.
static int dropped_extension(const char *line) {
    static const char *names[] = {"GL_EXT_shader_texture_lod", "GL_OES_standard_derivatives", "GL_EXT_frag_depth",
                                  "GL_OES_texture_3D", "GL_EXT_shadow_samplers", "GL_EXT_draw_buffers",
                                  "GL_EXT_shader_non_constant_global_initializers"};
    for (size_t i = 0; i < sizeof(names) / sizeof(*names); ++i) if (strstr(line, names[i])) return 1;
    return 0;
}

static int uses(const char *source, const char *word) {
    size_t n = strlen(word);
    for (const char *p = strstr(source, word); p; p = strstr(p + 1, word))
        if ((p == source || !ident_char((unsigned char)p[-1])) && !ident_char((unsigned char)p[n])) return 1;
    return 0;
}

// Type des échantillonneurs samplerN déclarés : 0 = 2D, 1 = cube, 2 = 3D,
// 3 = 2D d'ombre, -1 = autre ou absent.
enum { MAX_SAMPLERS = 32 };
static void sampler_types(const char *source, signed char types[MAX_SAMPLERS]) {
    memset(types, -1, MAX_SAMPLERS);
    static const char *names[] = {"sampler2D", "samplerCube", "sampler3D", "sampler2DShadow"};
    for (const char *p = strstr(source, "uniform"); p; p = strstr(p + 7, "uniform")) {
        if (p != source && ident_char((unsigned char)p[-1])) continue;
        const char *q = p + 7;
        while (*q == ' ' || *q == '\t') ++q;
        if (!strncmp(q, "lowp ", 5)) q += 5; else if (!strncmp(q, "mediump ", 8)) q += 8; else if (!strncmp(q, "highp ", 6)) q += 6;
        int type = -1; size_t n = 0;
        for (int t = 0; t < 4; ++t) {
            size_t m = strlen(names[t]);
            if (!strncmp(q, names[t], m) && !ident_char((unsigned char)q[m])) { type = t; n = m; }
        }
        if (type < 0) continue;
        q += n;
        while (*q == ' ' || *q == '\t') ++q;
        if (strncmp(q, "sampler", 7) || !isdigit((unsigned char)q[7])) continue;
        int unit = atoi(q + 7);
        if (unit >= 0 && unit < MAX_SAMPLERS) types[unit] = (signed char)type;
    }
}
// Appel de texture « nom ( samplerN » : unité N, ou -1.
static int texture_call_unit(const char *after_name) {
    const char *q = after_name;
    while (*q == ' ' || *q == '\t') ++q;
    if (*q++ != '(') return -1;
    while (*q == ' ' || *q == '\t') ++q;
    if (strncmp(q, "sampler", 7) || !isdigit((unsigned char)q[7])) return -1;
    const char *d = q + 7;
    while (isdigit((unsigned char)*d)) ++d;
    if (ident_char((unsigned char)*d)) return -1;
    return atoi(q + 7);
}
// Fonction d'échantillonnage enveloppée pour ce type d'échantillonneur ?
static int wrapped(const char *name, int type) {
    if (type == 3) return !strncmp(name, "gl4es_shadow2D", 14);
    return type >= 0 && (!strcmp(name, "texture") || !strcmp(name, "textureLod") || !strcmp(name, "textureProj") ||
                         !strcmp(name, "textureProjLod") || !strcmp(name, "textureGrad"));
}
static const char emulation_helpers[] =
    "uniform int p2_srgb_2d;\nuniform int p2_srgb_cube;\nuniform int p2_srgb_3d;\n"
    "uniform int p2_border_s;\nuniform int p2_border_t;\nuniform int p2_srgb_write;\n"
    "vec4 p2_decode(int mask, int u, vec4 c) {\n"
    "    if (((mask >> u) & 1) == 0) return c;\n"
    "    vec3 x = clamp(c.rgb, 0.0, 1.0);\n"
    "    return vec4(mix(pow((x + 0.055) / 1.055, vec3(2.4)), x / 12.92, vec3(lessThanEqual(x, vec3(0.04045)))), c.a);\n"
    "}\n"
    "vec3 p2_srgb_encode(vec3 c) {\n"
    "    vec3 x = clamp(c, 0.0, 1.0);\n"
    "    return mix(1.055 * pow(x, vec3(1.0 / 2.4)) - 0.055, x * 12.92, vec3(lessThan(x, vec3(0.0031308))));\n"
    "}\n"
    "bool p2_outside(int u, vec2 c) {\n"
    "    return (((p2_border_s >> u) & 1) != 0 && (c.x < 0.0 || c.x > 1.0)) ||\n"
    "           (((p2_border_t >> u) & 1) != 0 && (c.y < 0.0 || c.y > 1.0));\n"
    "}\n"
    "vec4 p2_texture(int u, sampler2D s, vec2 c) { vec4 t = texture(s, c); return p2_outside(u, c) ? vec4(0.0) : p2_decode(p2_srgb_2d, u, t); }\n"
    "vec4 p2_texture(int u, sampler2D s, vec2 c, float b) { vec4 t = texture(s, c, b); return p2_outside(u, c) ? vec4(0.0) : p2_decode(p2_srgb_2d, u, t); }\n"
    "vec4 p2_texture(int u, samplerCube s, vec3 c) { return p2_decode(p2_srgb_cube, u, texture(s, c)); }\n"
    "vec4 p2_texture(int u, samplerCube s, vec3 c, float b) { return p2_decode(p2_srgb_cube, u, texture(s, c, b)); }\n"
    "vec4 p2_texture(int u, sampler3D s, vec3 c) { return p2_decode(p2_srgb_3d, u, texture(s, c)); }\n"
    "vec4 p2_texture(int u, sampler3D s, vec3 c, float b) { return p2_decode(p2_srgb_3d, u, texture(s, c, b)); }\n"
    "vec4 p2_textureLod(int u, sampler2D s, vec2 c, float l) { vec4 t = textureLod(s, c, l); return p2_outside(u, c) ? vec4(0.0) : p2_decode(p2_srgb_2d, u, t); }\n"
    "vec4 p2_textureLod(int u, samplerCube s, vec3 c, float l) { return p2_decode(p2_srgb_cube, u, textureLod(s, c, l)); }\n"
    "vec4 p2_textureLod(int u, sampler3D s, vec3 c, float l) { return p2_decode(p2_srgb_3d, u, textureLod(s, c, l)); }\n"
    "vec4 p2_textureProj(int u, sampler2D s, vec3 c) { vec4 t = textureProj(s, c); return p2_outside(u, c.xy / c.z) ? vec4(0.0) : p2_decode(p2_srgb_2d, u, t); }\n"
    "vec4 p2_textureProj(int u, sampler2D s, vec4 c) { vec4 t = textureProj(s, c); return p2_outside(u, c.xy / c.w) ? vec4(0.0) : p2_decode(p2_srgb_2d, u, t); }\n"
    "vec4 p2_textureProj(int u, sampler3D s, vec4 c) { return p2_decode(p2_srgb_3d, u, textureProj(s, c)); }\n"
    "vec4 p2_textureProjLod(int u, sampler2D s, vec3 c, float l) { vec4 t = textureProjLod(s, c, l); return p2_outside(u, c.xy / c.z) ? vec4(0.0) : p2_decode(p2_srgb_2d, u, t); }\n"
    "vec4 p2_textureProjLod(int u, sampler2D s, vec4 c, float l) { vec4 t = textureProjLod(s, c, l); return p2_outside(u, c.xy / c.w) ? vec4(0.0) : p2_decode(p2_srgb_2d, u, t); }\n"
    "vec4 p2_textureGrad(int u, sampler2D s, vec2 c, vec2 x, vec2 y) { vec4 t = textureGrad(s, c, x, y); return p2_outside(u, c) ? vec4(0.0) : p2_decode(p2_srgb_2d, u, t); }\n"
    "vec4 p2_textureGrad(int u, samplerCube s, vec3 c, vec3 x, vec3 y) { return p2_decode(p2_srgb_cube, u, textureGrad(s, c, x, y)); }\n"
    "vec4 p2_textureGrad(int u, sampler3D s, vec3 c, vec3 x, vec3 y) { return p2_decode(p2_srgb_3d, u, textureGrad(s, c, x, y)); }\n";
// Fonctions d'appoint pour les échantillonneurs d'ombre : vec4 de GLSL 1.20,
// et bordure (profondeur 0 : comparaison perdue, résultat 0).
static const char shadow_helpers[] =
    "vec4 gl4es_shadow2D(sampler2DShadow s, vec3 c) { return vec4(texture(s, c)); }\n"
    "vec4 gl4es_shadow2DLod(sampler2DShadow s, vec3 c, float l) { return vec4(textureLod(s, c, l)); }\n"
    "vec4 gl4es_shadow2DProj(sampler2DShadow s, vec4 c) { return vec4(textureProj(s, c)); }\n"
    "vec4 gl4es_shadow2DProjLod(sampler2DShadow s, vec4 c, float l) { return vec4(textureProjLod(s, c, l)); }\n";
static const char shadow_emulation_helpers[] =
    "vec4 p2_gl4es_shadow2D(int u, sampler2DShadow s, vec3 c) { vec4 t = gl4es_shadow2D(s, c); return p2_outside(u, c.xy) ? vec4(0.0) : t; }\n"
    "vec4 p2_gl4es_shadow2DLod(int u, sampler2DShadow s, vec3 c, float l) { vec4 t = gl4es_shadow2DLod(s, c, l); return p2_outside(u, c.xy) ? vec4(0.0) : t; }\n"
    "vec4 p2_gl4es_shadow2DProj(int u, sampler2DShadow s, vec4 c) { vec4 t = gl4es_shadow2DProj(s, c); return p2_outside(u, c.xy / c.w) ? vec4(0.0) : t; }\n"
    "vec4 p2_gl4es_shadow2DProjLod(int u, sampler2DShadow s, vec4 c, float l) { vec4 t = gl4es_shadow2DProjLod(s, c, l); return p2_outside(u, c.xy / c.w) ? vec4(0.0) : t; }\n";

// NULL si le source n'est pas du GLSL ES 1.00 (déjà 3.00, ou autre version).
// Attributs de togl « in vec4 vN; » : gl4es leur passe les couleurs D3DCOLOR
// (GL_BGRA) telles quelles depuis le tampon, et le nuanceur les remet dans
// l'ordre selon le bit N de p2_bgra (voir gl4es_p2_bgra_mask, gl_ios.m).
// Chaque usage de vN passe par p2_vN() : un #define après la déclaration,
// la fonction définie avant lui lisant l'attribut réel.
static char *bgra_attributes(char *shader) {
    Buffer out = {0};
    int declared = 0;
    for (const char *p = shader; *p;) {
        const char *end = strchr(p, '\n'); size_t n = end ? (size_t)(end - p) + 1 : strlen(p);
        unsigned index; char tail[8] = {0}; int used = 0;
        int attribute = !strncmp(p, "in vec4 v", 9) && sscanf(p + 9, "%u%7[; \t]%n", &index, tail, &used) == 2 &&
                        strchr(tail, ';') && index < 32;
        if (attribute && !declared) { if (puts_(&out, "uniform int p2_bgra;\n")) goto fail; declared = 1; }
        if (put(&out, p, n)) goto fail;
        if (attribute) {
            char helper[200];
            snprintf(helper, sizeof(helper), "%svec4 p2_v%u() { return ((p2_bgra >> %u) & 1) != 0 ? v%u.zyxw : v%u; }\n#define v%u p2_v%u()\n",
                     end ? "" : "\n", index, index, index, index, index, index);
            if (puts_(&out, helper)) goto fail;
        }
        p += n;
    }
    if (!declared) { free(out.data); return shader; }
    free(shader);
    return out.data;
fail:
    free(out.data);
    return shader;
}

// Nombre d'occurrences de « word » en mot entier.
static int occurrences(const char *source, const char *word) {
    size_t n = strlen(word);
    int count = 0;
    for (const char *p = strstr(source, word); p; p = strstr(p + 1, word))
        count += (p == source || !ident_char((unsigned char)p[-1])) && !ident_char((unsigned char)p[n]);
    return count;
}

// Sortie de nuanceur de sommets déclarée mais jamais écrite : le GLSL de
// bureau la laisse indéfinie, l'éditeur de liens d'OpenGL ES d'Apple refuse le
// programme si le nuanceur de fragments la lit (« Input of fragment shader
// '_gl4es_FrontColor' not written by vertex shader »). Cas de
// bufferclearobeystencil_vs20 sans couleur (gl4es y déclare tout de même
// _gl4es_FrontColor), apparié à un nuanceur de fragments qui lit gl_Color :
// togl ne trace jamais ce programme, et l'effacement de la profondeur dans le
// pochoir d'un portail (BufferClearObeyStencil) disparaissait, d'où une vue à
// travers le portail rejetée entière par le test de profondeur. Chaque sortie
// scalaire ou vecteur sans autre occurrence que sa déclaration reçoit une
// valeur au début de main : blanc opaque pour les couleurs (couleur courante
// par défaut d'OpenGL), zéro sinon.
static char *initialize_unwritten_outputs(char *shader) {
    Buffer init = {0};
    for (const char *p = shader; *p;) {
        const char *end = strchr(p, '\n'); size_t n = end ? (size_t)(end - p) + 1 : strlen(p);
        char line[256], word[5][64];
        size_t m = n < sizeof(line) - 1 ? n : sizeof(line) - 1;
        memcpy(line, p, m); line[m] = 0;
        p += n;
        int words = sscanf(line, " %63s %63s %63s %63s %63s", word[0], word[1], word[2], word[3], word[4]), w = 0;
        // [centroid|flat|smooth] out [précision] type nom;
        if (words > 0 && (!strcmp(word[0], "centroid") || !strcmp(word[0], "flat") || !strcmp(word[0], "smooth"))) ++w;
        if (words < w + 3 || strcmp(word[w], "out")) continue;
        ++w;
        if (!strcmp(word[w], "lowp") || !strcmp(word[w], "mediump") || !strcmp(word[w], "highp")) ++w;
        if (words < w + 2) continue;
        const char *type = word[w];
        char *name = word[w + 1];
        size_t length = strcspn(name, " \t;");
        if (name[length] != ';' || strchr(line, '[')) continue;   // tableaux exclus
        name[length] = 0;
        int components = !strcmp(type, "float") ? 1 : !strcmp(type, "vec2") ? 2 : !strcmp(type, "vec3") ? 3
                       : !strcmp(type, "vec4") ? 4 : 0;
        if (!components || occurrences(shader, name) != 1) continue;
        const char *value = strstr(name, "Color") && components == 4 ? "vec4(1.0)" : components == 1 ? "0.0"
                          : components == 2 ? "vec2(0.0)" : components == 3 ? "vec3(0.0)" : "vec4(0.0)";
        char assignment[128];
        snprintf(assignment, sizeof(assignment), "\n%s = %s;", name, value);
        if (puts_(&init, assignment)) { free(init.data); return shader; }
    }
    if (!init.data) return shader;
    char *entry = !strncmp(shader, "void main", 9) ? shader : strstr(shader, "\nvoid main");
    char *brace = entry ? strchr(entry, '{') : NULL;
    Buffer out = {0};
    if (!brace || put(&out, shader, (size_t)(brace + 1 - shader)) || puts_(&out, init.data) || puts_(&out, brace + 1)) {
        free(init.data); free(out.data); return shader;
    }
    free(init.data); free(shader);
    return out.data;
}

char *rt_essl1_to_essl3(const char *source, int is_fragment, int emulate) {
    const char *p = source;
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') ++p;
    int versioned = !strncmp(p, "#version", 8);
    if (versioned) {
        const char *v = p + 8;
        while (*v == ' ' || *v == '\t') ++v;
        if (strncmp(v, "100", 3)) return NULL;
        p = strchr(p, '\n'); p = p ? p + 1 : p + strlen(p);
    } else p = source;
    Buffer out = {0};
    if (puts_(&out, "#version 300 es\n")) return NULL;
    // Directives de tête (#extension...) recopiées en premier, sauf celles
    // devenues standard ; les précisions des types ajoutés viennent ensuite.
    while (*p == '#' || *p == '\n' || *p == '\r' || *p == ' ' || *p == '\t') {
        const char *end = strchr(p, '\n'); size_t n = end ? (size_t)(end - p) + 1 : strlen(p);
        int extension = !strncmp(p, "#extension", 10);
        if (!extension) break;
        char line[256]; size_t m = n < sizeof(line) - 1 ? n : sizeof(line) - 1;
        memcpy(line, p, m); line[m] = 0;
        if (!dropped_extension(line) && put(&out, p, n)) goto fail;
        p += n;
    }
    if (puts_(&out, "precision highp float;\nprecision highp int;\nprecision highp sampler3D;\n"
                    "precision highp sampler2DShadow;\nprecision highp samplerCubeShadow;\n"))
        goto fail;
    int shadow = uses(source, "shadow2D") || uses(source, "shadow2DEXT") || uses(source, "shadow2DLod") ||
                 uses(source, "shadow2DProj") || uses(source, "shadow2DProjEXT") || uses(source, "shadow2DProjLod");
    int frag_color = is_fragment && uses(source, "gl_FragColor");
    int frag_data = is_fragment && uses(source, "gl_FragData");
    // Les deux à la fois (shaders de pipeline fixe de gl4es autour d'un shader
    // de togl) : une seule sortie, gl_FragColor devient gl4es_FragData[0].
    int color_as_data = frag_color && frag_data;
    if (color_as_data) frag_color = 0;
    // Sorties déclarées en tête : gl4es peut les écrire dans une fonction
    // placée avant main (_gl4es_main de ses shaders de pipeline fixe).
    if (frag_color && puts_(&out, "out vec4 gl4es_FragColor;\n")) goto fail;
    if (frag_data && puts_(&out, "layout(location = 0) out vec4 gl4es_FragData[4];\n")) goto fail;
    emulate = emulate && is_fragment && (frag_color || frag_data);
    if (shadow && puts_(&out, shadow_helpers)) goto fail;
    signed char types[MAX_SAMPLERS];
    if (emulate) {
        sampler_types(source, types);
        if (puts_(&out, emulation_helpers) || (shadow && puts_(&out, shadow_emulation_helpers))) goto fail;
    }
    int inserted = 0;
    // Corps : identifiants renommés ; main renommé p2_main si émulation.
    while (*p) {
        if (!inserted && !strncmp(p, "void main", 9) && (p == source || p[-1] == '\n')) {
            inserted = 1;
            if (emulate) { if (puts_(&out, "void p2_main")) goto fail; p += 9; continue; }
        }
        if (ident_char((unsigned char)*p) && !isdigit((unsigned char)*p)) {
            const char *start = p;
            while (ident_char((unsigned char)*p)) ++p;
            const char *to = renamed(start, (size_t)(p - start), is_fragment);
            if (color_as_data && to && !strcmp(to, "gl4es_FragColor")) to = "gl4es_FragData[0]";
            int unit = emulate && to ? texture_call_unit(p) : -1;
            if (unit >= 0 && unit < MAX_SAMPLERS && wrapped(to, types[unit])) {
                char call[96];   // « f ( samplerN » -> « p2_f(N, samplerN »
                snprintf(call, sizeof(call), "p2_%s(%d, ", to, unit);
                if (puts_(&out, call)) goto fail;
                while (*p != '(') ++p;
                ++p;
                continue;
            }
            if (to ? puts_(&out, to) : put(&out, start, (size_t)(p - start))) goto fail;
        } else if (p[0] == '/' && p[1] == '/') {   // commentaires recopiés tels quels
            const char *end = strchr(p, '\n'); size_t n = end ? (size_t)(end - p) : strlen(p);
            if (put(&out, p, n)) goto fail;
            p += n;
        } else if (p[0] == '/' && p[1] == '*') {
            const char *end = strstr(p + 2, "*/"); size_t n = end ? (size_t)(end - p) + 2 : strlen(p);
            if (put(&out, p, n)) goto fail;
            p += n;
        } else if (isdigit((unsigned char)*p)) {   // nombres (1.0e5, 2u...) recopiés d'un bloc
            const char *start = p;
            while (ident_char((unsigned char)*p) || *p == '.') ++p;
            if (put(&out, start, (size_t)(p - start))) goto fail;
        } else {
            if (put(&out, p, 1)) goto fail;
            ++p;
        }
    }
    if (!inserted) goto fail;   // pas de main : laisser le source tel quel
    if (emulate) {
        // Sorties encodées selon le masque p2_srgb_write (bit i : sortie i).
        if (puts_(&out, "\nvoid main() {\n    p2_main();\n")) goto fail;
        for (int i = 0; i < (frag_color ? 1 : 4); ++i) {
            char line[200], name[32];
            snprintf(name, sizeof(name), frag_color ? "%s" : "%s[%d]", frag_color ? "gl4es_FragColor" : "gl4es_FragData", i);
            snprintf(line, sizeof(line), "    if (((p2_srgb_write >> %d) & 1) != 0) %s.rgb = p2_srgb_encode(%s.rgb);\n", i, name, name);
            if (puts_(&out, line)) goto fail;
        }
        if (puts_(&out, "}\n")) goto fail;
    }
    if (!is_fragment) out.data = initialize_unwritten_outputs(out.data);
    if (!is_fragment && rt_ios_env_flag("P2_IOS_BGRA", 1)) return bgra_attributes(out.data);
    return out.data;
fail:
    free(out.data);
    return NULL;
}
