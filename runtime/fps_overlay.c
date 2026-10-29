// Compteur d'images côté hôte, activé/désactivé par F3 (fps_overlay_toggle,
// appelé par sdl_input.c) : rendu (« OpenGL » ou « Metal ») puis cadence.
// Le texte est découpé en rectangles ici ; OpenGL les efface au ciseau avant
// l'échange (ci-dessous), Direct3D 9 avec un Clear avant Present (d3d9.c).
// Le rendu du jeu n'est pas modifié. P2_FPS=1 l'active dès le lancement.
#include "rt.h"
#include "host_gl.h"   // OpenGL.framework (macOS) ou gl4es (iOS)
#if TARGET_OS_IPHONE
#undef glBindFramebufferEXT
#define glBindFramebufferEXT glBindFramebuffer   // gl4es : sans le nom EXT ; 0 = son tampon principal
#endif
#include <mach/mach_time.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fps_overlay.h"

static atomic_int enabled = -1;
static uint64_t last_ticks, frames;
static double shown_fps;

void fps_overlay_toggle(void) {
    if (atomic_load(&enabled) < 0) atomic_store(&enabled, 0);
    atomic_store(&enabled, !atomic_load(&enabled));
}

// Police 5x7 : chiffres, ponctuation et les lettres de « OpenGL », « Metal », « fps ».
static const char glyph_chars[] = "0123456789. fpsOenGLMtal";
static const uint8_t glyphs[][7] = {
    {0x0E,0x11,0x13,0x15,0x19,0x11,0x0E}, {0x04,0x0C,0x04,0x04,0x04,0x04,0x0E},
    {0x0E,0x11,0x01,0x02,0x04,0x08,0x1F}, {0x1F,0x02,0x04,0x02,0x01,0x11,0x0E},
    {0x02,0x06,0x0A,0x12,0x1F,0x02,0x02}, {0x1F,0x10,0x1E,0x01,0x01,0x11,0x0E},
    {0x06,0x08,0x10,0x1E,0x11,0x11,0x0E}, {0x1F,0x01,0x02,0x04,0x08,0x08,0x08},
    {0x0E,0x11,0x11,0x0E,0x11,0x11,0x0E}, {0x0E,0x11,0x11,0x0F,0x01,0x02,0x0C},
    {0x00,0x00,0x00,0x00,0x00,0x0C,0x0C}, {0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0x06,0x09,0x08,0x1C,0x08,0x08,0x08}, {0x00,0x00,0x1E,0x11,0x1E,0x10,0x10},
    {0x00,0x00,0x0F,0x10,0x0E,0x01,0x1E},
    {0x0E,0x11,0x11,0x11,0x11,0x11,0x0E},   // O
    {0x00,0x00,0x0E,0x11,0x1F,0x10,0x0E},   // e
    {0x00,0x00,0x16,0x19,0x11,0x11,0x11},   // n
    {0x0E,0x11,0x10,0x17,0x11,0x11,0x0F},   // G
    {0x10,0x10,0x10,0x10,0x10,0x10,0x1F},   // L
    {0x11,0x1B,0x15,0x15,0x11,0x11,0x11},   // M
    {0x08,0x08,0x1C,0x08,0x08,0x09,0x06},   // t
    {0x00,0x00,0x0E,0x01,0x0F,0x11,0x0F},   // a
    {0x0C,0x04,0x04,0x04,0x04,0x04,0x0E},   // l
};

// Compte une image ; vrai si le compteur est affiché. P2_FPS=1 l'affiche dès le lancement.
int fps_overlay_tick(void) {
    if (atomic_load(&enabled) < 0) {
        const char *e = getenv("P2_FPS");
        atomic_store(&enabled, e && *e && strcmp(e, "0"));
    }
    static mach_timebase_info_data_t base;
    if (!base.denom) mach_timebase_info(&base);
    uint64_t now = mach_absolute_time();
    ++frames;
    if (!last_ticks) last_ticks = now;
    double elapsed = (double)(now - last_ticks) * base.numer / base.denom / 1e9;
    if (elapsed >= 0.5) { shown_fps = (double)frames / elapsed; frames = 0; last_ticks = now; }
    return atomic_load(&enabled);
}

double fps_overlay_value(void) { return shown_fps; }

// Texte en rectangles (origine en haut à gauche, y vers le bas) : une rangée de
// pixels allumés contigus d'un glyphe = un rectangle. Commun aux deux rendus.
int fps_overlay_layout(const char *line1, const char *line2, int drawable_height, FpsRect *out, int max) {
    int scale = drawable_height >= 1200 ? 4 : 3, n = 0;
    const char *lines[2] = {line1, line2};
    for (int l = 0; l < 2; ++l) {
        int x = 12 * scale / 3, y = 12 + l * 9 * scale;
        for (const char *p = lines[l]; p && *p; ++p, x += 6 * scale) {
            const char *at = strchr(glyph_chars, *p);
            if (!at) continue;
            const uint8_t *g = glyphs[at - glyph_chars];
            for (int row = 0; row < 7; ++row)
                for (int col = 0; col < 5;) {
                    if (!(g[row] & (0x10 >> col))) { ++col; continue; }
                    int start = col;
                    while (col < 5 && (g[row] & (0x10 >> col))) ++col;
                    if (n < max) out[n++] = (FpsRect){x + start * scale, y + row * scale, (col - start) * scale, scale};
                }
        }
    }
    return n;
}

// Rectangles du compteur (rendu, puis cadence) ; après fps_overlay_tick.
int fps_overlay_text_rects(const char *renderer, int drawable_height, FpsRect *out, int max) {
    char fps[32];
    snprintf(fps, sizeof(fps), "%.0f fps", shown_fps);
    return fps_overlay_layout(renderer, fps, drawable_height, out, max);
}

// OpenGL : rectangles effacés au ciseau dans le framebuffer par défaut, juste
// avant l'échange, état GL sauvé puis restauré.
void fps_overlay_frame(int drawable_width, int drawable_height) {
    FpsRect rects[FPS_OVERLAY_MAX_RECTS];
    if (!fps_overlay_tick() || drawable_width <= 0 || drawable_height <= 0) return;
    int n = fps_overlay_text_rects("OpenGL", drawable_height, rects, FPS_OVERLAY_MAX_RECTS);
    GLint draw_framebuffer = 0;
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING_EXT, &draw_framebuffer);
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, 0);
    glDisable(GL_FRAMEBUFFER_SRGB_EXT);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glEnable(GL_SCISSOR_TEST);
    int shift = drawable_height >= 1200 ? 4 : 3;
    for (int pass = 0; pass < 2; ++pass) {   // ombre décalée, puis texte
        if (pass) glClearColor(1.0f, 0.85f, 0.1f, 1.0f); else glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        int d = pass ? 0 : shift / 3 + 1;
        for (int i = 0; i < n; ++i) {
            glScissor(rects[i].x + d, drawable_height - (rects[i].y + d) - rects[i].h, rects[i].w, rects[i].h);
            glClear(GL_COLOR_BUFFER_BIT);
        }
    }
    glPopAttrib();
    glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, (GLuint)draw_framebuffer);
}
