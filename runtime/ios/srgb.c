// sRGB et bordures de togl sur OpenGL ES 3 d'iOS (voir essl3.c pour la
// moitié shader).
//
// togl suppose EXT_texture_sRGB_decode et GL_FRAMEBUFFER_SRGB : une texture
// créée en format sRGB est décodée à la lecture sauf si son paramètre
// TEXTURE_SRGB_DECODE vaut SKIP_DECODE, et la sortie d'un tracé est encodée si
// GL_FRAMEBUFFER_SRGB est actif et que la couleur 0 de la cible est sRGB.
// iOS n'a aucun des deux interrupteurs, et gl4es stocke les textures sRGB
// comme des textures ordinaires (octets non convertis). Ce module suit, entre
// togl et gl4es, l'état qui décide des deux conversions et, avant chaque tracé,
// le transmet aux uniformes ajoutés par essl3.c (p2_srgb_2d/cube/3d, et
// p2_srgb_write : sorties à encoder, selon attachements et glDrawBuffers) au
// programme courant. Différences restantes avec un vrai
// GPU : filtrage et mélange (blending) faits sur les valeurs encodées.
//
// Même mécanisme pour CLAMP_TO_BORDER, absent d'ES 3.0 : les axes S et T en
// bordure de chaque texture sont suivis et passés en masques par unité
// (p2_border_s, p2_border_t).
//
// P2_IOS_SRGB=0 désactive l'émulation (comparaison).
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <OpenGLES/ES3/gl.h>
#include "env_ios.h"

void *gl4es_GetProcAddress(const char *name);

int rt_ios_srgb_enabled(void) { return RT_IOS_ENV_FLAG("P2_IOS_SRGB", 1); }

#define FRAMEBUFFER_SRGB 0x8DB9
#define TEXTURE_SRGB_DECODE 0x8A48
#define SKIP_DECODE 0x8A4A
#define TEXTURE_3D 0x806F
#define UNITS 16

enum { TEX_SRGB = 1, TEX_SKIP_DECODE = 2, TEX_BORDER_S = 4, TEX_BORDER_T = 8 };
#define CLAMP_TO_BORDER 0x812D
enum { UNIFORMS = 6 };

// Tableaux indexés par nom d'objet, agrandis à la demande.
typedef struct { unsigned char *flags; size_t size; } ByteMap;
static unsigned char *byte_at(ByteMap *m, GLuint name) {
    if (name >= m->size) {
        size_t size = m->size ? m->size : 1024;
        while (size <= name) size *= 2;
        unsigned char *grown = realloc(m->flags, size);
        if (!grown) return NULL;
        memset(grown + m->size, 0, size - m->size);
        m->flags = grown; m->size = size;
    }
    return &m->flags[name];
}
static unsigned char byte_get(const ByteMap *m, GLuint name) { return name < m->size ? m->flags[name] : 0; }


static ByteMap textures;          // TEX_* par texture
static ByteMap renderbuffers;     // TEX_SRGB par renderbuffer
// Par FBO : attachements couleur 0-3 (texture, ou renderbuffer | RENDERBUFFER_BIT)
// et sortie de shader -> attachement (glDrawBuffers, état propre à chaque FBO).
enum { OUTPUTS = 4 };
typedef struct { GLuint attach[OUTPUTS]; signed char draw[OUTPUTS]; unsigned char used; } FboState;
static FboState *fbos;
static size_t fbo_count;
#define RENDERBUFFER_BIT 0x80000000u
static FboState *fbo_state(GLuint fbo) {
    if (fbo >= fbo_count) {
        size_t size = fbo_count ? fbo_count : 64;
        while (size <= fbo) size *= 2;
        FboState *grown = realloc(fbos, size * sizeof(*grown));
        if (!grown) return NULL;
        memset(grown + fbo_count, 0, (size - fbo_count) * sizeof(*grown));
        fbos = grown; fbo_count = size;
    }
    FboState *f = &fbos[fbo];
    if (!f->used) { memset(f, 0, sizeof(*f)); f->draw[0] = 0; f->draw[1] = f->draw[2] = f->draw[3] = -1; f->used = 1; }
    return f;
}
// Un objet supprimé ne doit plus être vu attaché (son nom peut être réutilisé).
static void detach_everywhere(GLuint object) {
    for (size_t i = 0; i < fbo_count; ++i)
        for (int a = 0; a < OUTPUTS; ++a) if (fbos[i].attach[a] == object) fbos[i].attach[a] = 0;
}
static GLuint bound_renderbuffer;
static GLuint bound[UNITS][3];    // texture liée par unité : 2D, cube, 3D
static GLuint active_unit;
static GLuint draw_fbo, read_fbo;
static int framebuffer_srgb;
static GLuint current_program;
static unsigned char object_flags(GLuint object) {
    return object & RENDERBUFFER_BIT ? byte_get(&renderbuffers, object & ~RENDERBUFFER_BIT) : byte_get(&textures, object);
}
// Sorties du shader à encoder en sRGB pour le FBO de tracé (bit i : sortie i).
static GLint srgb_outputs(GLuint fbo) {
    if (!fbo || fbo >= fbo_count || !fbos[fbo].used) return 0;
    GLint mask = 0;
    for (int o = 0; o < OUTPUTS; ++o) {
        int a = fbos[fbo].draw[o];
        if (a >= 0 && (object_flags(fbos[fbo].attach[a]) & TEX_SRGB)) mask |= 1 << o;
    }
    return mask;
}

// Uniformes par programme : emplacements (-2 = à chercher) et dernières valeurs.
typedef struct { GLint loc[UNIFORMS]; GLint value[UNIFORMS]; } ProgramState;
static ProgramState *programs;
static size_t program_count;
static ProgramState *program_state(GLuint program) {
    if (program >= program_count) {
        size_t size = program_count ? program_count : 256;
        while (size <= program) size *= 2;
        ProgramState *grown = realloc(programs, size * sizeof(*grown));
        if (!grown) return NULL;
        for (size_t i = program_count; i < size; ++i)
            for (int k = 0; k < UNIFORMS; ++k) { grown[i].loc[k] = -2; grown[i].value[k] = -1; }
        programs = grown; program_count = size;
    }
    return &programs[program];
}

static int kind_of(GLenum target) {
    if (target == GL_TEXTURE_2D) return 0;
    if (target == GL_TEXTURE_CUBE_MAP || (target >= 0x8515 && target <= 0x851A)) return 1;
    if (target == TEXTURE_3D) return 2;
    return -1;
}
static int srgb_format(GLint internal) { return internal >= 0x8C40 && internal <= 0x8C4F; }

// ---- suivi, appelé par les enveloppes de gl_ios.m ----
void rt_ios_srgb_bind_texture(GLenum target, GLuint name) {
    int kind = kind_of(target);
    if (kind >= 0 && active_unit < UNITS) bound[active_unit][kind] = name;
}
void rt_ios_srgb_tex_image(GLenum target, GLint level, GLint internal) {
    int kind = kind_of(target);
    if (level != 0 || kind < 0 || active_unit >= UNITS) return;
    GLuint name = bound[active_unit][kind];
    unsigned char *f = name ? byte_at(&textures, name) : NULL;
    if (f) *f = (unsigned char)((*f & ~TEX_SRGB) | (srgb_format(internal) ? TEX_SRGB : 0));
}
void rt_ios_srgb_decode(GLenum target, GLint value) {
    int kind = kind_of(target);
    if (kind < 0 || active_unit >= UNITS) return;
    GLuint name = bound[active_unit][kind];
    unsigned char *f = name ? byte_at(&textures, name) : NULL;
    if (f) *f = (unsigned char)((*f & ~TEX_SKIP_DECODE) | (value == SKIP_DECODE ? TEX_SKIP_DECODE : 0));
}

void rt_ios_srgb_wrap_mode(GLenum target, GLenum name, GLint value) {
    int kind = kind_of(target);
    unsigned char bit = name == GL_TEXTURE_WRAP_S ? TEX_BORDER_S : name == GL_TEXTURE_WRAP_T ? TEX_BORDER_T : 0;
    if (!bit || kind < 0 || active_unit >= UNITS) return;
    GLuint texture = bound[active_unit][kind];
    unsigned char *f = texture ? byte_at(&textures, texture) : NULL;
    if (f) *f = (unsigned char)((*f & ~bit) | (value == CLAMP_TO_BORDER ? bit : 0));
}

// ---- enveloppes propres à ce module ----
static void (*next_active_texture)(GLenum);
static void (*next_delete_textures)(GLsizei, const GLuint *);
static void (*next_enable)(GLenum), (*next_disable)(GLenum);
static GLboolean (*next_is_enabled)(GLenum);
static void (*next_bind_framebuffer)(GLenum, GLuint);
static void (*next_framebuffer_texture_2d)(GLenum, GLenum, GLenum, GLuint, GLint);
static void (*next_framebuffer_texture_3d)(GLenum, GLenum, GLenum, GLuint, GLint, GLint);
static void (*next_draw_buffers)(GLsizei, const GLenum *);
static void (*next_framebuffer_renderbuffer)(GLenum, GLenum, GLenum, GLuint);
static void (*next_delete_framebuffers)(GLsizei, const GLuint *);
static void (*next_bind_renderbuffer)(GLenum, GLuint);
static void (*next_renderbuffer_storage)(GLenum, GLenum, GLsizei, GLsizei);
static void (*next_renderbuffer_storage_multisample)(GLenum, GLsizei, GLenum, GLsizei, GLsizei);
static void (*next_delete_renderbuffers)(GLsizei, const GLuint *);
static void (*next_use_program)(GLuint);
static void (*next_link_program)(GLuint);
static void (*next_compressed_tex_image_2d)(GLenum, GLint, GLenum, GLsizei, GLsizei, GLint, GLsizei, const void *);
static void (*next_draw_arrays)(GLenum, GLint, GLsizei);
static void (*next_draw_elements)(GLenum, GLsizei, GLenum, const void *);
static void (*next_draw_range_elements)(GLenum, GLuint, GLuint, GLsizei, GLenum, const void *);
static void (*next_draw_elements_base_vertex)(GLenum, GLsizei, GLenum, const void *, GLint);
static void (*next_draw_range_elements_base_vertex)(GLenum, GLuint, GLuint, GLsizei, GLenum, const void *, GLint);
static GLint (*g4_get_uniform_location)(GLuint, const GLchar *);
static void (*g4_uniform1i)(GLint, GLint);

static void active_texture(GLenum unit) { active_unit = unit - GL_TEXTURE0; next_active_texture(unit); }
static void delete_textures(GLsizei n, const GLuint *names) {
    for (GLsizei i = 0; i < n; ++i) {
        if (names[i] < textures.size) textures.flags[names[i]] = 0;
        if (names[i]) detach_everywhere(names[i]);
        for (int u = 0; u < UNITS; ++u)
            for (int k = 0; k < 3; ++k) if (bound[u][k] == names[i]) bound[u][k] = 0;
    }
    next_delete_textures(n, names);
}
static unsigned clip_planes;   // diagnostic : plans de découpe actifs (bits)
static void enable(GLenum cap) {
    if (cap >= 0x3000 && cap < 0x3006) clip_planes |= 1u << (cap - 0x3000);
    if (cap == FRAMEBUFFER_SRGB) framebuffer_srgb = 1; else next_enable(cap);
}
static void disable(GLenum cap) {
    if (cap >= 0x3000 && cap < 0x3006) clip_planes &= ~(1u << (cap - 0x3000));
    if (cap == FRAMEBUFFER_SRGB) framebuffer_srgb = 0; else next_disable(cap);
}
static GLboolean is_enabled(GLenum cap) {
    return cap == FRAMEBUFFER_SRGB ? (GLboolean)framebuffer_srgb : next_is_enabled(cap);
}
static void bind_framebuffer(GLenum target, GLuint fbo) {
    if (target == GL_FRAMEBUFFER || target == GL_DRAW_FRAMEBUFFER) draw_fbo = fbo;
    if (target == GL_FRAMEBUFFER || target == GL_READ_FRAMEBUFFER) read_fbo = fbo;
    next_bind_framebuffer(target, fbo);
}
static GLuint target_fbo(GLenum target) { return target == GL_READ_FRAMEBUFFER ? read_fbo : draw_fbo; }
static void attach(GLenum target, GLenum attachment, GLuint object) {
    GLuint fbo = target_fbo(target);
    FboState *f = fbo && attachment >= GL_COLOR_ATTACHMENT0 && attachment < GL_COLOR_ATTACHMENT0 + OUTPUTS ? fbo_state(fbo) : NULL;
    if (f) f->attach[attachment - GL_COLOR_ATTACHMENT0] = object;
}
static void framebuffer_texture_2d(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level) {
    attach(target, attachment, texture);
    next_framebuffer_texture_2d(target, attachment, textarget, texture, level);
}
static void framebuffer_texture_3d(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint z) {
    attach(target, attachment, texture);
    next_framebuffer_texture_3d(target, attachment, textarget, texture, level, z);
}
static void framebuffer_renderbuffer(GLenum target, GLenum attachment, GLenum rbtarget, GLuint rb) {
    attach(target, attachment, rb ? rb | RENDERBUFFER_BIT : 0);
    next_framebuffer_renderbuffer(target, attachment, rbtarget, rb);
}
static void draw_buffers(GLsizei n, const GLenum *buffers) {
    FboState *f = draw_fbo ? fbo_state(draw_fbo) : NULL;
    for (int o = 0; f && o < OUTPUTS; ++o) {
        GLenum b = o < n ? buffers[o] : GL_NONE;
        f->draw[o] = b >= GL_COLOR_ATTACHMENT0 && b < GL_COLOR_ATTACHMENT0 + OUTPUTS ? (signed char)(b - GL_COLOR_ATTACHMENT0) : -1;
    }
    next_draw_buffers(n, buffers);
}
static void bind_renderbuffer(GLenum target, GLuint rb) { bound_renderbuffer = rb; next_bind_renderbuffer(target, rb); }
static void renderbuffer_format(GLenum internal) {
    unsigned char *f = bound_renderbuffer ? byte_at(&renderbuffers, bound_renderbuffer) : NULL;
    if (f) *f = srgb_format((GLint)internal) ? TEX_SRGB : 0;
}
static void renderbuffer_storage(GLenum target, GLenum internal, GLsizei w, GLsizei h) {
    renderbuffer_format(internal);
    next_renderbuffer_storage(target, internal, w, h);
}
static void renderbuffer_storage_multisample(GLenum target, GLsizei samples, GLenum internal, GLsizei w, GLsizei h) {
    static int logged;
    if (!logged++) fprintf(stderr, "[srgb] renderbuffer multi-échantillonné : %d échantillons, format %#x, %dx%d\n",
                           samples, internal, w, h);
    renderbuffer_format(internal);
    next_renderbuffer_storage_multisample(target, samples, internal, w, h);
}
static void delete_renderbuffers(GLsizei n, const GLuint *names) {
    for (GLsizei i = 0; i < n; ++i) {
        if (names[i] < renderbuffers.size) renderbuffers.flags[names[i]] = 0;
        if (names[i]) detach_everywhere(names[i] | RENDERBUFFER_BIT);
        if (bound_renderbuffer == names[i]) bound_renderbuffer = 0;
    }
    next_delete_renderbuffers(n, names);
}
static void delete_framebuffers(GLsizei n, const GLuint *names) {
    for (GLsizei i = 0; i < n; ++i) {
        if (names[i] < fbo_count) fbos[names[i]].used = 0;
        if (draw_fbo == names[i]) draw_fbo = 0;
        if (read_fbo == names[i]) read_fbo = 0;
    }
    next_delete_framebuffers(n, names);
}
static void use_program(GLuint program) { current_program = program; next_use_program(program); }
static void link_program(GLuint program) {
    next_link_program(program);
    ProgramState *s = program_state(program);
    if (s) for (int k = 0; k < UNIFORMS; ++k) { s->loc[k] = -2; s->value[k] = -1; }
}
static void compressed_tex_image_2d(GLenum t, GLint l, GLenum i, GLsizei w, GLsizei h, GLint b, GLsizei n, const void *d) {
    rt_ios_srgb_tex_image(t, l, (GLint)i);
    next_compressed_tex_image_2d(t, l, i, w, h, b, n, d);
}

// P2_IOS_SRGB_TRACE=1 : statistiques toutes les 2 000 tracés (build de débogage).
static void trace(const GLint *value, const ProgramState *s) {
    if (!RT_IOS_DEBUG_BUILD || !RT_IOS_ENV_FLAG("P2_IOS_SRGB_TRACE", 0)) return;
    static unsigned long draws, srgb_on, fbo_srgb, writes, reads, no_loc, borders, border_loc;
    ++draws; srgb_on += framebuffer_srgb != 0; reads += (value[0] | value[1] | value[2]) != 0;
    fbo_srgb += srgb_outputs(draw_fbo) != 0;
    writes += value[3] != 0; no_loc += s->loc[3] < 0;
    borders += (value[4] | value[5]) != 0; border_loc += (value[4] | value[5]) && s->loc[4] >= 0;
    if (draws % 2000 == 0)
        fprintf(stderr, "[srgb] tracés %lu : FRAMEBUFFER_SRGB %lu, cible sRGB %lu, encodage %lu, décodage %lu, "
                        "sans uniforme %lu, bordure %lu (dont shader %lu) (fbo %u couleur0 %u)\n", draws, srgb_on, fbo_srgb, writes, reads, no_loc, borders, border_loc,
                draw_fbo, draw_fbo < fbo_count ? fbos[draw_fbo].attach[0] : 0);
}

static void sync_uniforms(void) {
    if (!current_program) return;
    ProgramState *s = program_state(current_program);
    if (!s) return;
    static const char *names[UNIFORMS] = {"p2_srgb_2d", "p2_srgb_cube", "p2_srgb_3d", "p2_srgb_write", "p2_border_s",
                                          "p2_border_t"};
    if (s->loc[0] == -2)
        for (int k = 0; k < UNIFORMS; ++k) s->loc[k] = g4_get_uniform_location(current_program, names[k]);
    GLint value[UNIFORMS] = {0};
    for (int u = 0; u < UNITS; ++u) {
        for (int k = 0; k < 3; ++k) {
            unsigned char f = byte_get(&textures, bound[u][k]);
            if ((f & (TEX_SRGB | TEX_SKIP_DECODE)) == TEX_SRGB) value[k] |= 1 << u;
        }
        unsigned char f = byte_get(&textures, bound[u][0]);   // bordures : textures 2D
        if (f & TEX_BORDER_S) value[4] |= 1 << u;
        if (f & TEX_BORDER_T) value[5] |= 1 << u;
    }
    if (framebuffer_srgb) value[3] = srgb_outputs(draw_fbo);
    trace(value, s);
    for (int k = 0; k < UNIFORMS; ++k) {
        if (s->loc[k] < 0 || s->value[k] == value[k]) continue;
        g4_uniform1i(s->loc[k], value[k]);
        s->value[k] = value[k];
    }
}
// Diagnostic par bissection : P2_IOS_SKIP_DRAWS=a-b saute, à chaque image,
// les tracés numérotés [a, b) ; P2_IOS_DRAW_LOG=n journalise ceux de l'image n.
// Build de débogage seulement : ailleurs draw_skipped rend 0 et disparaît du
// chemin de chaque tracé.
static unsigned draw_index, frame_index;
void rt_ios_draws_frame_end(void) { draw_index = 0; ++frame_index; }
static int draw_skipped(const char *fn, GLenum mode, GLsizei count, GLuint start, GLuint end, GLint base) {
    if (!RT_IOS_DEBUG_BUILD) return 0;
    static int init; static long skip_a = -1, skip_b = -1, log_frame = -1;
    if (!init) {
        init = 1;
        const char *e = getenv("P2_IOS_SKIP_DRAWS");
        if (e && *e) { skip_a = strtol(e, (char **)&e, 10); skip_b = *e == '-' ? strtol(e + 1, NULL, 10) : skip_a + 1; }
        e = getenv("P2_IOS_DRAW_LOG");
        if (e && *e) log_frame = atol(e);
    }
    unsigned index = draw_index++;
    if ((long)frame_index == log_frame) {
        fprintf(stderr, "[draw] %u %s mode %u n %d [%u,%u] base %d prog %u fbo %u srgb %d clip %#x\n", index, fn, mode, count,
                start, end, base, current_program, draw_fbo, framebuffer_srgb, clip_planes);
        static void (*attrib_iv)(GLuint, GLenum, GLint *);
        static void (*attrib_pointer)(GLuint, GLenum, void **);
        if (!attrib_iv) {
            attrib_iv = gl4es_GetProcAddress("glGetVertexAttribiv");
            attrib_pointer = gl4es_GetProcAddress("glGetVertexAttribPointerv");
        }
        static GLuint described[64]; static int n_described;
        int seen = 0;
        for (int i = 0; i < n_described; ++i) seen |= described[i] == current_program;
        if (!seen && n_described < 64) {
            described[n_described++] = current_program;
            void (*program_iv)(GLuint, GLenum, GLint *) = gl4es_GetProcAddress("glGetProgramiv");
            void (*active_attrib)(GLuint, GLuint, GLsizei, GLsizei *, GLint *, GLenum *, GLchar *) =
                gl4es_GetProcAddress("glGetActiveAttrib");
            GLint (*attrib_location)(GLuint, const GLchar *) = gl4es_GetProcAddress("glGetAttribLocation");
            const char *dump = getenv("P2_IOS_PROGRAM_DUMP");   // dossier : sources des programmes journalisés
            if (dump && *dump) {
                void (*attached)(GLuint, GLsizei, GLsizei *, GLuint *) = gl4es_GetProcAddress("glGetAttachedShaders");
                void (*source)(GLuint, GLsizei, GLsizei *, GLchar *) = gl4es_GetProcAddress("glGetShaderSource");
                GLuint shaders[4]; GLsizei n = 0; attached(current_program, 4, &n, shaders);
                for (GLsizei i = 0; i < n; ++i) {
                    static char text[262144]; GLsizei length = 0; source(shaders[i], sizeof(text), &length, text);
                    char path[1024]; snprintf(path, sizeof(path), "%s/prog%u_%d.glsl", dump, current_program, (int)i);
                    FILE *f = fopen(path, "w"); if (f) { fwrite(text, 1, (size_t)length, f); fclose(f); }
                }
            }
            GLint count = 0; program_iv(current_program, GL_ACTIVE_ATTRIBUTES, &count);
            for (GLint i = 0; i < count; ++i) {
                char name[64]; GLint size = 0; GLenum type = 0;
                active_attrib(current_program, (GLuint)i, sizeof(name), NULL, &size, &type, name);
                fprintf(stderr, "[draw]    programme %u entrée %s type %#x emplacement %d\n", current_program, name, type,
                        attrib_location(current_program, name));
            }
        }
        for (GLuint a = 0; a < 16 && attrib_iv; ++a) {
            GLint on = 0, size = 0, type = 0, stride = 0, norm = 0, buffer = 0; void *ptr = NULL;
            attrib_iv(a, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &on);
            if (!on) continue;
            attrib_iv(a, GL_VERTEX_ATTRIB_ARRAY_SIZE, &size); attrib_iv(a, GL_VERTEX_ATTRIB_ARRAY_TYPE, &type);
            attrib_iv(a, GL_VERTEX_ATTRIB_ARRAY_STRIDE, &stride); attrib_iv(a, GL_VERTEX_ATTRIB_ARRAY_NORMALIZED, &norm);
            attrib_iv(a, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, &buffer);
            attrib_pointer(a, GL_VERTEX_ATTRIB_ARRAY_POINTER, &ptr);
            fprintf(stderr, "[draw]    attr %u : %d x %#x norm %d stride %d buffer %d offset %p\n", a, size, type, norm, stride,
                    buffer, ptr);
        }
    }
    static char programs[256] = ",";
    static int programs_init;
    if (!programs_init) {   // P2_IOS_SKIP_PROGRAMS=45,802 : programmes à sauter
        programs_init = 1;
        const char *e = getenv("P2_IOS_SKIP_PROGRAMS");
        if (e && *e) snprintf(programs, sizeof(programs), ",%s,", e);
    }
    if (programs[1]) {
        char key[16]; snprintf(key, sizeof(key), ",%u,", current_program);
        if (strstr(programs, key)) return 1;
    }
    return (long)index >= skip_a && (long)index < skip_b;
}
static void draw_arrays(GLenum m, GLint f, GLsizei c) {
    if (draw_skipped("arrays", m, c, (GLuint)f, 0, 0)) return;
    sync_uniforms(); next_draw_arrays(m, f, c);
}
static void draw_elements(GLenum m, GLsizei c, GLenum t, const void *i) {
    if (draw_skipped("elements", m, c, 0, 0, 0)) return;
    sync_uniforms(); next_draw_elements(m, c, t, i);
}
static void draw_range_elements(GLenum m, GLuint s, GLuint e, GLsizei c, GLenum t, const void *i) {
    if (draw_skipped("range", m, c, s, e, 0)) return;
    sync_uniforms(); next_draw_range_elements(m, s, e, c, t, i);
}
static void draw_elements_base_vertex(GLenum m, GLsizei c, GLenum t, const void *i, GLint b) {
    if (draw_skipped("elements_base", m, c, 0, 0, b)) return;
    sync_uniforms(); next_draw_elements_base_vertex(m, c, t, i, b);
}
static void draw_range_elements_base_vertex(GLenum m, GLuint s, GLuint e, GLsizei c, GLenum t, const void *i, GLint b) {
    if (draw_skipped("range_base", m, c, s, e, b)) return;
    sync_uniforms(); next_draw_range_elements_base_vertex(m, s, e, c, t, i, b);
}

// Enveloppe `fn` (ce que togl aurait reçu pour `name`) si ce module suit cette
// fonction ; sinon rend `fn`.
void *rt_ios_srgb_wrap(const char *name, void *fn) {
    if (!fn || !rt_ios_srgb_enabled()) return fn;
    static const struct { const char *name; void **next; void *wrapper; } table[] = {
        {"glActiveTexture", (void **)&next_active_texture, (void *)active_texture},
        {"glActiveTextureARB", (void **)&next_active_texture, (void *)active_texture},
        {"glDeleteTextures", (void **)&next_delete_textures, (void *)delete_textures},
        {"glDeleteTexturesEXT", (void **)&next_delete_textures, (void *)delete_textures},
        {"glEnable", (void **)&next_enable, (void *)enable},
        {"glDisable", (void **)&next_disable, (void *)disable},
        {"glIsEnabled", (void **)&next_is_enabled, (void *)is_enabled},
        {"glBindFramebuffer", (void **)&next_bind_framebuffer, (void *)bind_framebuffer},
        {"glBindFramebufferEXT", (void **)&next_bind_framebuffer, (void *)bind_framebuffer},
        {"glFramebufferTexture2D", (void **)&next_framebuffer_texture_2d, (void *)framebuffer_texture_2d},
        {"glFramebufferTexture2DEXT", (void **)&next_framebuffer_texture_2d, (void *)framebuffer_texture_2d},
        {"glFramebufferTexture3D", (void **)&next_framebuffer_texture_3d, (void *)framebuffer_texture_3d},
        {"glFramebufferTexture3DEXT", (void **)&next_framebuffer_texture_3d, (void *)framebuffer_texture_3d},
        {"glDrawBuffers", (void **)&next_draw_buffers, (void *)draw_buffers},
        {"glDrawBuffersARB", (void **)&next_draw_buffers, (void *)draw_buffers},
        {"glFramebufferRenderbuffer", (void **)&next_framebuffer_renderbuffer, (void *)framebuffer_renderbuffer},
        {"glFramebufferRenderbufferEXT", (void **)&next_framebuffer_renderbuffer, (void *)framebuffer_renderbuffer},
        {"glDeleteFramebuffers", (void **)&next_delete_framebuffers, (void *)delete_framebuffers},
        {"glDeleteFramebuffersEXT", (void **)&next_delete_framebuffers, (void *)delete_framebuffers},
        {"glBindRenderbuffer", (void **)&next_bind_renderbuffer, (void *)bind_renderbuffer},
        {"glBindRenderbufferEXT", (void **)&next_bind_renderbuffer, (void *)bind_renderbuffer},
        {"glRenderbufferStorage", (void **)&next_renderbuffer_storage, (void *)renderbuffer_storage},
        {"glRenderbufferStorageEXT", (void **)&next_renderbuffer_storage, (void *)renderbuffer_storage},
        {"glRenderbufferStorageMultisample", (void **)&next_renderbuffer_storage_multisample,
         (void *)renderbuffer_storage_multisample},
        {"glRenderbufferStorageMultisampleEXT", (void **)&next_renderbuffer_storage_multisample,
         (void *)renderbuffer_storage_multisample},
        {"glDeleteRenderbuffers", (void **)&next_delete_renderbuffers, (void *)delete_renderbuffers},
        {"glDeleteRenderbuffersEXT", (void **)&next_delete_renderbuffers, (void *)delete_renderbuffers},
        {"glUseProgram", (void **)&next_use_program, (void *)use_program},
        {"glUseProgramObjectARB", (void **)&next_use_program, (void *)use_program},
        {"glLinkProgram", (void **)&next_link_program, (void *)link_program},
        {"glLinkProgramARB", (void **)&next_link_program, (void *)link_program},
        {"glCompressedTexImage2D", (void **)&next_compressed_tex_image_2d, (void *)compressed_tex_image_2d},
        {"glCompressedTexImage2DARB", (void **)&next_compressed_tex_image_2d, (void *)compressed_tex_image_2d},
        {"glDrawArrays", (void **)&next_draw_arrays, (void *)draw_arrays},
        {"glDrawElements", (void **)&next_draw_elements, (void *)draw_elements},
        {"glDrawRangeElements", (void **)&next_draw_range_elements, (void *)draw_range_elements},
        {"glDrawRangeElementsEXT", (void **)&next_draw_range_elements, (void *)draw_range_elements},
        {"glDrawElementsBaseVertex", (void **)&next_draw_elements_base_vertex, (void *)draw_elements_base_vertex},
        {"glDrawRangeElementsBaseVertex", (void **)&next_draw_range_elements_base_vertex,
         (void *)draw_range_elements_base_vertex},
    };
    for (size_t i = 0; i < sizeof(table) / sizeof(*table); ++i) {
        if (strcmp(name, table[i].name)) continue;
        if (!g4_get_uniform_location) {
            g4_get_uniform_location = gl4es_GetProcAddress("glGetUniformLocation");
            g4_uniform1i = gl4es_GetProcAddress("glUniform1i");
        }
        if (!g4_get_uniform_location || !g4_uniform1i) return fn;
        *table[i].next = fn;
        return table[i].wrapper;
    }
    return fn;
}
