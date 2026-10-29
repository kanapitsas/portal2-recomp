// Pont SDL2 natif : sous-systèmes, indices, fenêtres, modes d'affichage,
// contexte OpenGL, surfaces, curseurs et presse-papiers.
//
// Les modules Linux (launcher.so, engine.so, shaderapidx9.so, vgui2.so,
// vguimatsurface.so) sont liés à libSDL2-2.0.so.0 ; ils appellent SDL2 i386
// en cdecl. Les pointeurs SDL hôtes (64 bits) ne sont jamais tronqués :
// SDL_Window* → handle 'SDLW' (partagé avec le pont SDL entrées),
// SDL_GLContext → 'SDLG', SDL_Cursor* → 'SDLC', SDL_RWops* → 'SDLR'.
// SDL_Surface est lue directement par le code invité : on en recopie une
// version au format i386 (structure, SDL_PixelFormat, palette, pixels) en
// mémoire invitée, et on garde la surface hôte correspondante.
//
// SHIMS: SDL_Init SDL_InitSubSystem SDL_QuitSubSystem SDL_WasInit SDL_SetHint SDL_GetError SDL_free
// SHIMS: SDL_CreateWindow SDL_DestroyWindow SDL_ShowWindow SDL_HideWindow SDL_RaiseWindow
// SHIMS: SDL_SetWindowTitle SDL_SetWindowIcon SDL_SetWindowPosition SDL_SetWindowSize SDL_GetWindowSize
// SHIMS: SDL_SetWindowBordered SDL_SetWindowFullscreen SDL_SetWindowDisplayMode SDL_GetWindowDisplayMode
// SHIMS: SDL_GetWindowDisplayIndex SDL_GetWindowID SDL_GetWindowGrab SDL_SetWindowGrab
// SHIMS: SDL_SetWindowGammaRamp SDL_WarpMouseInWindow
// SHIMS: SDL_GetNumVideoDisplays SDL_GetDisplayBounds SDL_GetCurrentDisplayMode SDL_GetDesktopDisplayMode
// SHIMS: SDL_GetCurrentVideoDriver SDL_GetCurrentAudioDriver
// SHIMS: SDL_GL_LoadLibrary SDL_GL_UnloadLibrary SDL_GL_SetAttribute SDL_GL_CreateContext
// SHIMS: SDL_GL_DeleteContext SDL_GL_MakeCurrent SDL_GL_SetSwapInterval SDL_GL_SwapWindow SDL_GL_GetProcAddress
// SHIMS: SDL_LoadBMP_RW SDL_RWFromFile SDL_FreeSurface
// SHIMS: SDL_CreateColorCursor SDL_CreateSystemCursor SDL_SetCursor SDL_FreeCursor SDL_ShowCursor
// SHIMS: SDL_GetClipboardText SDL_SetClipboardText SDL_HasClipboardText
#include "rt.h"
#include "host_gl.h"   // OpenGL.framework (macOS) ou gl4es (iOS)
#include "frame_times.h"
#include <mach/mach_time.h>
void rt_gl_timing_add(const char *label, uint64_t ticks);
void rt_gl_pass_frame(void);
void fps_overlay_frame(int drawable_width, int drawable_height);
#include "sdl_common.h"
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

// Mise en page i386 de SDL2 (2.0.x à 2.32 : inchangée).
_Static_assert(SDL_WINDOW_OPENGL == 0x2 && SDL_WINDOW_FULLSCREEN_DESKTOP == 0x1001, "SDL2 window flags");
_Static_assert(SDL_INIT_VIDEO == 0x20 && SDL_GL_CONTEXT_PROFILE_MASK == 21, "SDL2 enums");
_Static_assert(SDL_WINDOW_VULKAN == 0x10000000 && SDL_WINDOW_METAL == 0x20000000, "SDL2 window flags (Vulkan, Metal)");

static uint32_t arg(CPU *c, unsigned i) { return R32(c->esp + 4 + 4 * i); }
static const char *guest_string(CPU *c, uint32_t address) {
    if (!address) return NULL;
    return rt_span(c, address, (uint64_t)rt_string_length(c, address) + 1);
}
static void *handle(CPU *c, uint32_t value, uint32_t kind, const char *what) {
    if (!value) return NULL;
    void *host = rt_handle_get(value, kind);
    if (!host) rt_trap(c, c->eip, what);
    return host;
}
#define WINDOW(i) ((SDL_Window *)handle(c, arg(c, i), RT_SDL_WINDOW, "SDL: invalid SDL_Window handle"))

static void require_main_thread(CPU *c, const char *what) {
    if (!pthread_main_np()) rt_trap(c, c->eip, what);
}

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

// Chaînes constantes renvoyées au code invité (SDL_GetError, pilotes) :
// une copie stable par contenu distinct, jamais libérée.
typedef struct ConstString { uint32_t address; struct ConstString *next; char text[]; } ConstString;
static ConstString *const_strings;
static uint32_t const_string(CPU *c, const char *text) {
    if (!text) return 0;
    pthread_mutex_lock(&lock);
    ConstString *s = const_strings;
    while (s && strcmp(s->text, text)) s = s->next;
    if (!s) {
        size_t length = strlen(text) + 1;
        s = malloc(sizeof(*s) + length);
        uint32_t address = s && length <= UINT32_MAX ? rt_alloc((uint32_t)length) : 0;
        if (!address) { free(s); pthread_mutex_unlock(&lock); rt_trap(c, c->eip, "SDL: guest string allocation failed"); }
        memcpy(s->text, text, length);
        memcpy(GPTR(address), text, length);
        s->address = address; s->next = const_strings; const_strings = s;
    }
    pthread_mutex_unlock(&lock);
    return s->address;
}

// Chaînes allouées sur le tas invité et rendues par SDL_free : celles du
// presse-papiers ici, et celles des événements du pont SDL entrées
// (SDL_DROPFILE, SDL_TEXTEDITING_EXT). Règle commune : toute adresse du tas
// invité passée à SDL_free retourne à rt_free ; une adresse inconnue piège.
static uint32_t heap_string(CPU *c, const char *text) {
    size_t length = strlen(text) + 1;
    uint32_t address = length <= UINT32_MAX ? rt_alloc((uint32_t)length) : 0;
    if (!address) rt_trap(c, c->eip, "SDL: guest string allocation failed");
    memcpy(GPTR(address), text, length);
    return address;
}
static int is_const_string(uint32_t address) {
    pthread_mutex_lock(&lock);
    ConstString *s = const_strings;
    while (s && s->address != address) s = s->next;
    pthread_mutex_unlock(&lock);
    return s != NULL;
}

// SDL_DisplayMode i386 : format, w, h, refresh_rate, driverdata (20 octets).
// driverdata est un pointeur hôte privé du pilote : jamais exposé (0 côté
// invité, NULL côté hôte ; SDL n'en a pas besoin pour un mode demandé).
static void put_mode(CPU *c, uint32_t address, const SDL_DisplayMode *mode) {
    rt_write_span(c, address, RT_SDL_DISPLAYMODE_SIZE);
    W32(address, mode->format); W32(address + 4, (uint32_t)mode->w);
    W32(address + 8, (uint32_t)mode->h); W32(address + 12, (uint32_t)mode->refresh_rate);
    W32(address + 16, 0);
}
static SDL_DisplayMode get_mode(CPU *c, uint32_t address) {
    rt_span(c, address, RT_SDL_DISPLAYMODE_SIZE);
    return (SDL_DisplayMode){R32(address), (int)R32(address + 4), (int)R32(address + 8), (int)R32(address + 12), NULL};
}
static int mode_result(CPU *c, int result, uint32_t address, const SDL_DisplayMode *mode) {
    if (!result) put_mode(c, address, mode);
    return result;
}

// --- Surfaces ---------------------------------------------------------------
// Bloc invité : SDL_Surface (60) | SDL_PixelFormat (44) | SDL_Palette (16)
// | couleurs (4 × n) | pixels (alignés sur 16).
enum { SURFACE_SIZE = 60, FORMAT_OFFSET = 64, FORMAT_SIZE = 44, PALETTE_OFFSET = 112, COLORS_OFFSET = 128 };
_Static_assert(FORMAT_OFFSET >= SURFACE_SIZE && PALETTE_OFFSET >= FORMAT_OFFSET + FORMAT_SIZE, "surface block");
typedef struct Surface { uint32_t guest, pixels, palette; SDL_Surface *host; struct Surface *next; } Surface;
static Surface *surfaces;

static Surface *find_surface(uint32_t guest) {
    pthread_mutex_lock(&lock);
    Surface *s = surfaces;
    while (s && s->guest != guest) s = s->next;
    pthread_mutex_unlock(&lock);
    return s;
}

static uint32_t export_surface(CPU *c, SDL_Surface *host) {
    if (!host) return 0;
    const SDL_PixelFormat *f = host->format;
    int colors = f->palette ? f->palette->ncolors : 0;
    if (host->w < 0 || host->h < 0 || host->pitch < 0 || colors < 0 || colors > 256) {
        SDL_FreeSurface(host); rt_trap(c, c->eip, "SDL: unsupported host surface geometry");
    }
    uint64_t pixels_offset = (COLORS_OFFSET + 4 * (uint64_t)colors + 15) & ~UINT64_C(15);
    uint64_t pixel_bytes = (uint64_t)host->pitch * (uint64_t)host->h;
    uint64_t total = pixels_offset + pixel_bytes;
    Surface *s = malloc(sizeof(*s));
    uint32_t base = s && total <= UINT32_MAX ? rt_alloc((uint32_t)total) : 0;
    if (!base) { free(s); SDL_FreeSurface(host); rt_trap(c, c->eip, "SDL: guest surface allocation failed"); }
    memset(GPTR(base), 0, (size_t)pixels_offset);
    uint32_t format = base + FORMAT_OFFSET, palette = colors ? base + PALETTE_OFFSET : 0;
    uint32_t pixels = base + (uint32_t)pixels_offset;
    W32(base, host->flags); W32(base + 4, format);
    W32(base + 8, (uint32_t)host->w); W32(base + 12, (uint32_t)host->h); W32(base + 16, (uint32_t)host->pitch);
    W32(base + 20, pixel_bytes ? pixels : 0);
    // userdata, locked, lock_data restent nuls ; map (pointeur interne) aussi.
    W32(base + 36, (uint32_t)host->clip_rect.x); W32(base + 40, (uint32_t)host->clip_rect.y);
    W32(base + 44, (uint32_t)host->clip_rect.w); W32(base + 48, (uint32_t)host->clip_rect.h);
    W32(base + 56, 1);  // refcount
    W32(format, f->format); W32(format + 4, palette);
    W8(format + 8, f->BitsPerPixel); W8(format + 9, f->BytesPerPixel);
    W32(format + 12, f->Rmask); W32(format + 16, f->Gmask); W32(format + 20, f->Bmask); W32(format + 24, f->Amask);
    const uint8_t loss_shift[8] = {f->Rloss, f->Gloss, f->Bloss, f->Aloss, f->Rshift, f->Gshift, f->Bshift, f->Ashift};
    memcpy(GPTR(format + 28), loss_shift, 8);
    W32(format + 36, 1);  // refcount ; next (liste interne) reste nul
    if (palette) {
        W32(palette, (uint32_t)colors); W32(palette + 4, base + COLORS_OFFSET);
        W32(palette + 8, f->palette->version); W32(palette + 12, 1);
        memcpy(GPTR(base + COLORS_OFFSET), f->palette->colors, 4 * (size_t)colors);
    }
    if (pixel_bytes) {
        int locked = SDL_MUSTLOCK(host) && !SDL_LockSurface(host);
        if (!host->pixels) rt_trap(c, c->eip, "SDL: host surface pixels unavailable");
        memcpy(GPTR(pixels), host->pixels, (size_t)pixel_bytes);
        if (locked) SDL_UnlockSurface(host);
    }
    s->guest = base; s->pixels = pixels; s->palette = palette; s->host = host;
    pthread_mutex_lock(&lock);
    s->next = surfaces; surfaces = s;
    pthread_mutex_unlock(&lock);
    return base;
}

// Avant de confier une surface à SDL hôte : recopier ce que le code invité a
// pu modifier (pixels, couleurs de palette). La géométrie doit être intacte.
SDL_Surface *rt_sdl_surface_sync(CPU *c, uint32_t guest) {
    if (!guest) return NULL;
    Surface *s = find_surface(guest);
    if (!s) rt_trap(c, c->eip, "SDL: surface not created by the SDL bridge");
    SDL_Surface *host = s->host;
    if ((int)R32(guest + 8) != host->w || (int)R32(guest + 12) != host->h || (int)R32(guest + 16) != host->pitch ||
        R32(guest + 20) != (host->pitch && host->h ? s->pixels : 0) || R32(guest + 4) != guest + FORMAT_OFFSET)
        rt_trap(c, c->eip, "SDL: guest changed surface geometry");
    uint64_t bytes = (uint64_t)host->pitch * (uint64_t)host->h;
    if (bytes) {
        int locked = SDL_MUSTLOCK(host) && !SDL_LockSurface(host);
        memcpy(host->pixels, rt_span(c, s->pixels, bytes), (size_t)bytes);
        if (locked) SDL_UnlockSurface(host);
    }
    if (s->palette) {
        SDL_Palette *p = host->format->palette;
        memcpy(p->colors, rt_span(c, guest + COLORS_OFFSET, 4 * (uint64_t)p->ncolors), 4 * (size_t)p->ncolors);
    }
    return host;
}

static void free_surface(CPU *c, uint32_t guest) {
    if (!guest) return;
    pthread_mutex_lock(&lock);
    Surface **link = &surfaces;
    while (*link && (*link)->guest != guest) link = &(*link)->next;
    Surface *s = *link;
    if (s && R32(guest + 56) > 1) { W32(guest + 56, R32(guest + 56) - 1); pthread_mutex_unlock(&lock); return; }
    if (s) *link = s->next;
    pthread_mutex_unlock(&lock);
    if (!s) rt_trap(c, c->eip, "SDL_FreeSurface: surface not created by the SDL bridge");
    SDL_FreeSurface(s->host);
    rt_free(s->guest);
    free(s);
}

// --- Attributs GL -----------------------------------------------------------
static const char *gl_attribute_name(uint32_t attribute) {
    static const char *const names[] = {
        "RED_SIZE", "GREEN_SIZE", "BLUE_SIZE", "ALPHA_SIZE", "BUFFER_SIZE", "DOUBLEBUFFER", "DEPTH_SIZE",
        "STENCIL_SIZE", "ACCUM_RED_SIZE", "ACCUM_GREEN_SIZE", "ACCUM_BLUE_SIZE", "ACCUM_ALPHA_SIZE", "STEREO",
        "MULTISAMPLEBUFFERS", "MULTISAMPLESAMPLES", "ACCELERATED_VISUAL", "RETAINED_BACKING",
        "CONTEXT_MAJOR_VERSION", "CONTEXT_MINOR_VERSION", "CONTEXT_EGL", "CONTEXT_FLAGS", "CONTEXT_PROFILE_MASK",
        "SHARE_WITH_CURRENT_CONTEXT", "FRAMEBUFFER_SRGB_CAPABLE", "CONTEXT_RELEASE_BEHAVIOR",
        "CONTEXT_RESET_NOTIFICATION", "CONTEXT_NO_ERROR", "FLOATBUFFERS"};
    return attribute < sizeof(names) / sizeof(*names) ? names[attribute] : "?";
}

// Diagnostic hôte (sans effet sur le jeu) : P2_FRAME_LOG=1 journalise le
// nombre d'images et leur cadence ; P2_FRAME_DUMP=/chemin.ppm y écrit le
// tampon arrière toutes les 120 images, avant l'échange (fenêtre cachée).
static void drawable_size(SDL_Window *window, int *w, int *h);
// Compteur et cadence (P2_FRAME_LOG), communs à OpenGL (échange de tampons) et
// à Direct3D 9 (Present, voir runtime/ios/metal_ios.m) ; renvoie le numéro d'image.
uint64_t rt_frame_log_tick(void) {
    static uint64_t frames, last_ticks;
    static int log = -1;
    if (log < 0) log = getenv("P2_FRAME_LOG") != NULL;
    ++frames;
    if (log && frames % 60 == 0) {
        uint64_t now = SDL_GetTicks64();
        fprintf(stderr, "[diag] frame %llu, %.1f fps\n", (unsigned long long)frames,
                last_ticks ? 60000.0 / (double)(now - last_ticks + 1) : 0.0);
        last_ticks = now;
    }
    return frames;
}

static void diagnose_frame(SDL_Window *w) {
    static const char *dump; static int dump_read;
    if (!dump_read) { dump = getenv("P2_FRAME_DUMP"); dump_read = 1; }
    uint64_t frames = rt_frame_log_tick();
    if (!dump || frames % 120 != 1 || !w) return;
    int width = 0, height = 0;
    drawable_size(w, &width, &height);
    if (width <= 0 || height <= 0 || width > 8192 || height > 8192) return;
    unsigned char *pixels = malloc((size_t)width * (size_t)height * 3);
    if (!pixels) return;
    GLint pack_alignment, read_buffer, pack_buffer = 0;
    glGetIntegerv(GL_PACK_ALIGNMENT, &pack_alignment);
    glGetIntegerv(GL_READ_BUFFER, &read_buffer);
    glGetIntegerv(GL_PIXEL_PACK_BUFFER_BINDING, &pack_buffer);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadBuffer(GL_BACK);
    if (pack_buffer) glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    if (pack_buffer) glBindBuffer(GL_PIXEL_PACK_BUFFER, (GLuint)pack_buffer);
    glReadBuffer((GLenum)read_buffer);
    glPixelStorei(GL_PACK_ALIGNMENT, pack_alignment);
    FILE *file = fopen(dump, "wb");
    if (file) {
        fprintf(file, "P6\n%d %d\n255\n", width, height);
        for (int y = height - 1; y >= 0; --y) fwrite(pixels + (size_t)y * (size_t)width * 3, 1, (size_t)width * 3, file);
        fclose(file);
    }
    free(pixels);
    fprintf(stderr, "[diag] frame %llu dumped to %s (%dx%d)\n", (unsigned long long)frames, dump, width, height);
}

// Diagnostic P2_GPU_TIMING=1 : temps GPU par image (GL_TIME_ELAPSED autour de
// chaque image, résultats lus avec quelques images de retard), journalisé
// toutes les 120 images avec le temps CPU entre deux échanges.
static void gpu_timing_frame(void) {
#if TARGET_OS_IPHONE
    return;   // gl4es : pas de GL_TIME_ELAPSED (requêtes d'occlusion seulement)
#endif
    static int enabled = -1, active;
    static GLuint queries[8];
    static unsigned issued, read;
    static double gpu_ms, cpu_ms; static unsigned samples; static uint64_t last;
    if (enabled < 0) enabled = getenv("P2_GPU_TIMING") != NULL;
    if (!enabled) return;
    static mach_timebase_info_data_t base;
    if (!base.denom) { mach_timebase_info(&base); glGenQueries(8, queries); }
    if (active) { glEndQuery(GL_TIME_ELAPSED_EXT); ++issued; active = 0; }
    while (read < issued) {
        GLint ready = 0; glGetQueryObjectiv(queries[read % 8], GL_QUERY_RESULT_AVAILABLE, &ready);
        if (!ready && issued - read < 7) break;
        GLuint64EXT ns = 0;
#if !TARGET_OS_IPHONE
        glGetQueryObjectui64vEXT(queries[read % 8], GL_QUERY_RESULT, &ns);
#endif
        gpu_ms += (double)ns / 1e6; ++samples; ++read;
    }
    uint64_t now = mach_absolute_time();
    if (last) cpu_ms += (double)(now - last) * base.numer / base.denom / 1e6;
    last = now;
    if (samples >= 120) {
        fprintf(stderr, "[gpu] %.2f ms GPU par image, %.2f ms entre deux échanges\n", gpu_ms / samples, cpu_ms / samples);
        gpu_ms = cpu_ms = 0; samples = 0;
    }
    glBeginQuery(GL_TIME_ELAPSED_EXT, queries[issued % 8]); active = 1;
}

static int hidden_windows(void) {
    static int value = -1;
    if (value < 0) { const char *e = getenv("P2_HIDDEN_WINDOWS"); value = e && *e && strcmp(e, "0"); }
    return value;
}

// --- Retina ------------------------------------------------------------------
// Le jeu raisonne en pixels : taille de fenêtre = taille du tampon arrière, et
// le menu Vidéo propose les résolutions de sa table qui tiennent dans l'écran
// annoncé. L'écran est donc annoncé en pixels (3024×1964 sur un MacBook Pro
// 14 pouces) et la fenêtre suit la résolution choisie :
// - si elle tient à l'écran en points, fenêtre de cette taille en points,
//   rendu à l'échelle 1 (comme avant) ;
// - sinon, fenêtre Retina : moitié de la taille en points, rendu natif.
// La propriété Retina de la vue Cocoa (wantsBestResolutionOpenGLSurface) est
// basculée à chaud ; tailles, positions et coordonnées de souris vues par le
// jeu sont converties. P2_RETINA=0 désactive tout (comportement d'origine).
#include <SDL2/SDL_syswm.h>
#include <objc/message.h>
static int display_scale_value, display_scale_forced, window_scale_value = 1;

static int retina_enabled(void) {
#if TARGET_OS_IPHONE
    return 0;   // iOS : échelle de rendu gérée par ios/sdl_ios.m
#endif
    const char *e = getenv("P2_RETINA");
    return !e || !*e || strcmp(e, "0");
}

// Échelle de l'écran principal, mesurée une fois sur une petite fenêtre cachée.
int rt_sdl_display_scale(void) {
    if (display_scale_value) return display_scale_value;
    if (!retina_enabled() || !SDL_WasInit(SDL_INIT_VIDEO) || !pthread_main_np()) return 1;
    int scale = 1;
    SDL_Window *probe = SDL_CreateWindow("", 0, 0, 64, 64, SDL_WINDOW_HIDDEN | SDL_WINDOW_ALLOW_HIGHDPI);
    if (probe) {
        int pixels = 0;
        SDL_GetWindowSizeInPixels(probe, &pixels, NULL);
        if (pixels >= 96) scale = (pixels + 32) / 64;
        SDL_DestroyWindow(probe);
    }
    display_scale_value = scale;
    fprintf(stderr, "[sdl] Retina: display scale %d\n", scale);
    return scale;
}
// Pixels par point de la fenêtre du jeu (1 ou l'échelle de l'écran).
int rt_sdl_window_scale(void) { return window_scale_value; }
// Tests : impose les échelles sans fenêtre de mesure ni vue Cocoa (0 = recalculer).
void rt_sdl_force_scales(int display, int window) {
    display_scale_value = display; display_scale_forced = display != 0; window_scale_value = window ? window : 1;
}

// Écran : pixels ↔ points.
static int screen_points(int pixels) { return pixels / rt_sdl_display_scale(); }
static int screen_pixels(int points) { return points * rt_sdl_display_scale(); }
static int position_to_points(int value) {
    return SDL_WINDOWPOS_ISUNDEFINED(value) || SDL_WINDOWPOS_ISCENTERED(value) ? value : screen_points(value);
}
static void scale_mode(SDL_DisplayMode *mode, int pixels) {
    mode->w = pixels ? screen_pixels(mode->w) : screen_points(mode->w);
    mode->h = pixels ? screen_pixels(mode->h) : screen_points(mode->h);
}

// --- Encoche -------------------------------------------------------------------
// Sur un écran à encoche (MacBook Pro et Air récents), une fenêtre plein écran
// SDL ne reçoit que la zone sous la barre des menus (1800×1130 points sur un
// 14 pouces réglé en 1800×1169), jamais la bande de l'encoche. Annoncer l'écran
// entier faisait choisir au jeu 3600×2338 pour une surface de 3600×2260 : image
// et souris divergeaient de 78 pixels en plein écran (boutons du menu cliquables
// plus bas que dessinés). L'écran est donc annoncé à la hauteur que reçoit
// vraiment la fenêtre : du bas de l'écran au bas de la barre des menus.
// Hauteur en points d'un écran de frame_h points dont la zone visible s'arrête
// visible_top points au-dessus de son bas, avec safe_top points d'encoche ;
// height est la hauteur demandée (inchangée si ce n'est pas l'écran entier).
int rt_sdl_notch_height(int height, int frame_h, int visible_top, double safe_top) {
    if (safe_top <= 0 || height != frame_h || visible_top <= 0 || visible_top >= frame_h) return height;
    return visible_top;
}

#if !TARGET_OS_IPHONE
// Mesurée à la première demande pour chaque écran, barre des menus visible, puis
// gardée : en plein écran la barre se masque et la zone visible gagne un point
// que la fenêtre ne reçoit pas. Sans encoche (écrans externes, Mac plus anciens),
// ou hors Cocoa (tests sur le pilote dummy) : rien ne change.
static int fullscreen_height(int display, int height) {
    static int cached[8][2];   // {hauteur de l'écran entier, hauteur annoncée}
    if (display_scale_forced || display < 0 || display >= 8 || !pthread_main_np()) return height;
    const char *driver = SDL_GetCurrentVideoDriver();
    if (!driver || strcmp(driver, "cocoa")) return height;
    if (cached[display][0]) return height == cached[display][0] ? cached[display][1] : height;
    SDL_Rect bounds;
    if (SDL_GetDisplayBounds(display, &bounds)) return height;
    typedef struct { double x, y, w, h; } Rect;
    typedef struct { double top, left, bottom, right; } Insets;
    id screens = ((id (*)(id, SEL))objc_msgSend)((id)objc_getClass("NSScreen"), sel_registerName("screens"));
    unsigned long count = screens ? ((unsigned long (*)(id, SEL))objc_msgSend)(screens, sel_registerName("count")) : 0;
    for (unsigned long i = 0; i < count; ++i) {
        id screen = ((id (*)(id, SEL, unsigned long))objc_msgSend)(screens, sel_registerName("objectAtIndex:"), i);
        Rect frame = ((Rect (*)(id, SEL))objc_msgSend)(screen, sel_registerName("frame"));
        if ((int)frame.x != bounds.x || (int)frame.w != bounds.w || (int)frame.h != bounds.h) continue;
        Rect visible = ((Rect (*)(id, SEL))objc_msgSend)(screen, sel_registerName("visibleFrame"));
        Insets safe = {0};
        if (((BOOL (*)(id, SEL, SEL))objc_msgSend)(screen, sel_registerName("respondsToSelector:"),
                                                   sel_registerName("safeAreaInsets")))
            safe = ((Insets (*)(id, SEL))objc_msgSend)(screen, sel_registerName("safeAreaInsets"));
        int top = (int)(visible.y + visible.h - frame.y);
        cached[display][0] = bounds.h;
        cached[display][1] = rt_sdl_notch_height(bounds.h, bounds.h, top, safe.top);
        if (cached[display][1] != bounds.h)
            fprintf(stderr, "[sdl] encoche : écran %d annoncé en %dx%d points (plein écran sous la barre des menus) au lieu de %dx%d\n",
                    display, bounds.w, cached[display][1], bounds.w, bounds.h);
        return height == cached[display][0] ? cached[display][1] : height;
    }
    return height;
}
#else
static int fullscreen_height(int display, int height) { (void)display; return height; }
#endif

// Échelle d'une fenêtre de w×h pixels : 1 si elle tient dans la zone utile de
// l'écran (barre de titre comprise), l'échelle de l'écran sinon.
enum { TITLE_BAR_POINTS = 28 };
static int scale_for(int w, int h) {
    int scale = rt_sdl_display_scale();
    if (scale == 1) return 1;
    SDL_Rect usable;
    if (SDL_GetDisplayUsableBounds(0, &usable)) return scale;
    return w <= usable.w && h + TITLE_BAR_POINTS <= usable.h ? 1 : scale;
}

// Bascule la surface GL de la fenêtre en résolution native (Retina) ou non.
static void set_backing(SDL_Window *window, int scale) {
    if (scale == window_scale_value) return;
    window_scale_value = scale;
    fprintf(stderr, "[sdl] Retina: window scale %d\n", scale);
    SDL_SysWMinfo info;
    SDL_VERSION(&info.version);
#if TARGET_OS_IPHONE
    (void)window;
#else
    if (display_scale_forced || !SDL_GetWindowWMInfo(window, &info) || info.subsystem != SDL_SYSWM_COCOA) return;
    id view = ((id (*)(id, SEL))objc_msgSend)((id)info.info.cocoa.window, sel_registerName("contentView"));
    ((void (*)(id, SEL, BOOL))objc_msgSend)(view, sel_registerName("setWantsBestResolutionOpenGLSurface:"), scale > 1);
    id context = ((id (*)(id, SEL))objc_msgSend)((id)objc_getClass("NSOpenGLContext"), sel_registerName("currentContext"));
    if (context) ((void (*)(id, SEL))objc_msgSend)(context, sel_registerName("update"));
#endif
}

// Taille de la fenêtre en pixels du jeu (= surface GL), pour les diagnostics.
static void drawable_size(SDL_Window *window, int *w, int *h) {
    SDL_GetWindowSize(window, w, h);
    *w *= window_scale_value; *h *= window_scale_value;
}

// Journal de la géométrie réelle de la fenêtre (points, pixels de la surface,
// position, plein écran, zone sûre sous l'encoche), écrit à chaque changement :
// sert à diagnostiquer les décalages souris/image en plein écran.
static void log_geometry(SDL_Window *window, const char *why) {
    if (!window) return;
    int w = 0, h = 0, pw = 0, ph = 0, x = 0, y = 0;
    SDL_GetWindowSize(window, &w, &h);
    SDL_GL_GetDrawableSize(window, &pw, &ph);
    SDL_GetWindowPosition(window, &x, &y);
    static int last[5];
    uint32_t flags = SDL_GetWindowFlags(window);
    int now[5] = {w, h, pw, ph, (int)(flags & SDL_WINDOW_FULLSCREEN_DESKTOP)};
    if (!strcmp(why, "swap") && !memcmp(now, last, sizeof(now))) return;
    memcpy(last, now, sizeof(now));
    SDL_Rect bounds = {0}, usable = {0};
    SDL_GetDisplayBounds(0, &bounds);
    SDL_GetDisplayUsableBounds(0, &usable);
    double inset[4] = {0};
#if !TARGET_OS_IPHONE   // iOS : pas de fenêtre Cocoa (zone sûre gérée par ios/sdl_ios.m)
    SDL_SysWMinfo info;
    SDL_VERSION(&info.version);
    if (!display_scale_forced && SDL_GetWindowWMInfo(window, &info) && info.subsystem == SDL_SYSWM_COCOA) {
        id screen = ((id (*)(id, SEL))objc_msgSend)((id)info.info.cocoa.window, sel_registerName("screen"));
        typedef struct { double top, left, bottom, right; } Insets;
        if (screen && ((BOOL (*)(id, SEL, SEL))objc_msgSend)(screen, sel_registerName("respondsToSelector:"),
                                                             sel_registerName("safeAreaInsets"))) {
            Insets s = ((Insets (*)(id, SEL))objc_msgSend)(screen, sel_registerName("safeAreaInsets"));
            inset[0] = s.top; inset[1] = s.left; inset[2] = s.bottom; inset[3] = s.right;
        }
    }
#endif
    fprintf(stderr, "[sdl] géométrie (%s) : fenêtre %dx%d points en (%d,%d), surface %dx%d pixels, échelle %d, "
            "plein écran 0x%x, écran %dx%d, zone utile %dx%d+%d+%d, zone sûre haut %.0f bas %.0f\n",
            why, w, h, x, y, pw, ph, window_scale_value, flags & SDL_WINDOW_FULLSCREEN_DESKTOP,
            bounds.w, bounds.h, usable.w, usable.h, usable.x, usable.y, inset[0], inset[2]);
}

static void resize_window(SDL_Window *window, int w, int h) {
    int scale = scale_for(w, h);
    set_backing(window, scale);
    SDL_SetWindowSize(window, w / scale, h / scale);
    log_geometry(window, "taille demandée");
}

static SDL_Window *create_window(const char *title, int x, int y, int w, int h, uint32_t flags) {
    if (rt_sdl_display_scale() == 1) return SDL_CreateWindow(title, x, y, w, h, flags);
    int scale = scale_for(w, h);
    SDL_Window *window = SDL_CreateWindow(title, position_to_points(x), position_to_points(y), w / scale, h / scale,
                                          flags | SDL_WINDOW_ALLOW_HIGHDPI);
    if (!window) return NULL;
    window_scale_value = rt_sdl_display_scale();   // surface créée en Retina
    set_backing(window, scale);
    log_geometry(window, "création");
    return window;
}

// --- Metal (rendu Direct3D 9 natif, runtime/d3d9.c) ---------------------------
// Avec -vulkan, launcher.so crée sa fenêtre avec SDL_WINDOW_VULKAN et ne crée
// pas de contexte GL ; shaderapivk.so donne cette fenêtre (HWND) à Direct3D 9.
// Ici, pas de chargeur Vulkan : la fenêtre devient SDL_WINDOW_METAL et reçoit
// une SDL_MetalView (thread principal) dont la CAMetalLayer est remise à la
// bibliothèque Direct3D 9 native par rt_sdl_metal_layer.
#if TARGET_OS_IPHONE
#include "metal_ios.h"
#endif
typedef struct MetalView { SDL_Window *window; SDL_MetalView view; struct MetalView *next; } MetalView;
static MetalView *metal_views;

uint32_t rt_sdl_window_flags(uint32_t guest_flags) {
    if (!(guest_flags & SDL_WINDOW_VULKAN)) return guest_flags;
    return (guest_flags & ~(uint32_t)(SDL_WINDOW_VULKAN | SDL_WINDOW_OPENGL)) | SDL_WINDOW_METAL;
}

static MetalView *find_metal_view(SDL_Window *window) {
    pthread_mutex_lock(&lock);
    MetalView *v = metal_views;
    while (v && v->window != window) v = v->next;
    pthread_mutex_unlock(&lock);
    return v;
}
static MetalView *create_metal_view(SDL_Window *window) {
    if (!pthread_main_np() || !(SDL_GetWindowFlags(window) & SDL_WINDOW_METAL)) return NULL;
    SDL_MetalView view = SDL_Metal_CreateView(window);
    MetalView *v = view ? malloc(sizeof(*v)) : NULL;
    if (!v) {
        if (view) SDL_Metal_DestroyView(view);
        fprintf(stderr, "[sdl] SDL_Metal_CreateView failed: %s\n", SDL_GetError());
        return NULL;
    }
    v->window = window; v->view = view;
    pthread_mutex_lock(&lock);
    v->next = metal_views; metal_views = v;
    pthread_mutex_unlock(&lock);
#if TARGET_OS_IPHONE
    rt_ios_metal_view_created(view);   // échelle de rendu de l'app (ios/metal_ios.m)
#endif
    return v;
}
static void destroy_metal_view(SDL_Window *window) {
    pthread_mutex_lock(&lock);
    MetalView **link = &metal_views;
    while (*link && (*link)->window != window) link = &(*link)->next;
    MetalView *v = *link;
    if (v) *link = v->next;
    pthread_mutex_unlock(&lock);
    if (v) { SDL_Metal_DestroyView(v->view); free(v); }
}

// CAMetalLayer* de la fenêtre et taille de rendu du jeu en pixels (celle du
// tampon arrière qu'il choisit : points × échelle Retina de la fenêtre).
// NULL si la fenêtre n'est pas une fenêtre Metal du pont.
void *rt_sdl_metal_layer(void *window, uint32_t *width, uint32_t *height) {
    SDL_Window *w = window;
    MetalView *v = find_metal_view(w);
    if (!v) v = create_metal_view(w);
    if (!v) return NULL;
    int dw = 0, dh = 0;
    drawable_size(w, &dw, &dh);
    if (width) *width = (uint32_t)dw;
    if (height) *height = (uint32_t)dh;
    return SDL_Metal_GetLayer(v->view);
}

unsigned rt_sdl_game_modes(uint32_t (*modes)[3], unsigned capacity) {
    int temporary = 0;
    if (!SDL_WasInit(SDL_INIT_VIDEO)) {
        if (!pthread_main_np() || SDL_InitSubSystem(SDL_INIT_VIDEO)) return 0;
        temporary = 1;
    }
    unsigned count = 0;
    SDL_DisplayMode desktop;
    if (capacity && !SDL_GetDesktopDisplayMode(0, &desktop) && desktop.w > 0 && desktop.h > 0) {
        desktop.h = fullscreen_height(0, desktop.h);
        scale_mode(&desktop, 1);
        modes[count][0] = (uint32_t)desktop.w; modes[count][1] = (uint32_t)desktop.h;
        modes[count++][2] = (uint32_t)(desktop.refresh_rate > 0 ? desktop.refresh_rate : 0);
        for (int i = 0, n = SDL_GetNumDisplayModes(0); i < n && count < capacity; ++i) {
            SDL_DisplayMode mode;
            if (SDL_GetDisplayMode(0, i, &mode)) continue;
            scale_mode(&mode, 1);
            uint32_t w = (uint32_t)mode.w, h = (uint32_t)mode.h, hz = (uint32_t)(mode.refresh_rate > 0 ? mode.refresh_rate : 0);
            if (!w || !h || w > modes[0][0] || h > modes[0][1]) continue;
            unsigned j = 0;
            while (j < count && (modes[j][0] != w || modes[j][1] != h || modes[j][2] != hz)) ++j;
            if (j == count) { modes[count][0] = w; modes[count][1] = h; modes[count++][2] = hz; }
        }
    }
    if (temporary) SDL_QuitSubSystem(SDL_INIT_VIDEO);
    return count;
}

static int sdl_video(CPU *c, const char *name) {
    if (strncmp(name, "SDL_", 4)) return 0;
    const char *n = name + 4;
    uint32_t r = 0;
    if (!strcmp(n, "Init") || !strcmp(n, "InitSubSystem")) {
        uint32_t flags = arg(c, 0);
        if (flags & SDL_INIT_VIDEO) require_main_thread(c, "SDL video must be initialized on the macOS main thread");
        r = (uint32_t)(!strcmp(n, "Init") ? SDL_Init(flags) : SDL_InitSubSystem(flags));
    } else if (!strcmp(n, "QuitSubSystem")) SDL_QuitSubSystem(arg(c, 0));
    else if (!strcmp(n, "WasInit")) r = SDL_WasInit(arg(c, 0));
    else if (!strcmp(n, "SetHint")) r = (uint32_t)SDL_SetHint(guest_string(c, arg(c, 0)), guest_string(c, arg(c, 1)));
    else if (!strcmp(n, "GetError")) r = const_string(c, SDL_GetError());
    else if (!strcmp(n, "free")) {
        uint32_t address = arg(c, 0);
        if (address && (is_const_string(address) || rt_free(address)))
            rt_trap(c, c->eip, "SDL_free: block not allocated by the SDL bridge");
    }
    // Fenêtres
    else if (!strcmp(n, "CreateWindow")) {
        require_main_thread(c, "SDL_CreateWindow must run on the macOS main thread");
        // Mode diagnostic P2_HIDDEN_WINDOWS=1 : la fenêtre existe (contexte GL
        // réel) mais n'est jamais montrée ; le jeu n'en voit aucune différence.
        uint32_t flags = rt_sdl_window_flags(arg(c, 5)) | (hidden_windows() ? SDL_WINDOW_HIDDEN : 0);
        SDL_Window *w = create_window(guest_string(c, arg(c, 0)), (int)arg(c, 1), (int)arg(c, 2),
                                      (int)arg(c, 3), (int)arg(c, 4), flags);
        if (w && !(r = rt_handle_new(w, RT_SDL_WINDOW))) { SDL_DestroyWindow(w); rt_trap(c, c->eip, "SDL: handle table full"); }
        if (w && (flags & SDL_WINDOW_METAL)) create_metal_view(w);   // thread principal : vue prête pour Direct3D 9
    } else if (!strcmp(n, "DestroyWindow")) {
        SDL_Window *w = WINDOW(0);
        if (w) { rt_handle_free(arg(c, 0), RT_SDL_WINDOW); destroy_metal_view(w); }
        SDL_DestroyWindow(w);
    }
    else if (!strcmp(n, "ShowWindow")) { SDL_Window *w = WINDOW(0); if (!hidden_windows()) SDL_ShowWindow(w); }
    else if (!strcmp(n, "HideWindow")) SDL_HideWindow(WINDOW(0));
    else if (!strcmp(n, "RaiseWindow")) { SDL_Window *w = WINDOW(0); if (!hidden_windows()) SDL_RaiseWindow(w); }
    else if (!strcmp(n, "SetWindowTitle")) SDL_SetWindowTitle(WINDOW(0), guest_string(c, arg(c, 1)));
    else if (!strcmp(n, "SetWindowIcon")) {
        SDL_Window *w = WINDOW(0);
        SDL_SetWindowIcon(w, rt_sdl_surface_sync(c, arg(c, 1)));
    }
    else if (!strcmp(n, "SetWindowPosition"))
        SDL_SetWindowPosition(WINDOW(0), position_to_points((int)arg(c, 1)), position_to_points((int)arg(c, 2)));
    else if (!strcmp(n, "SetWindowSize")) resize_window(WINDOW(0), (int)arg(c, 1), (int)arg(c, 2));
    else if (!strcmp(n, "GetWindowSize")) {
        int w = 0, h = 0; uint32_t pw = arg(c, 1), ph = arg(c, 2);
        if (pw) rt_write_span(c, pw, 4);
        if (ph) rt_write_span(c, ph, 4);
        SDL_GetWindowSize(WINDOW(0), pw ? &w : NULL, ph ? &h : NULL);
        if (pw) W32(pw, (uint32_t)(w * window_scale_value));
        if (ph) W32(ph, (uint32_t)(h * window_scale_value));
    }
    else if (!strcmp(n, "SetWindowBordered")) SDL_SetWindowBordered(WINDOW(0), arg(c, 1) ? SDL_TRUE : SDL_FALSE);
    else if (!strcmp(n, "SetWindowFullscreen")) {
        SDL_Window *w = WINDOW(0);
        r = (uint32_t)SDL_SetWindowFullscreen(w, arg(c, 1));
        fprintf(stderr, "[sdl] SetWindowFullscreen(0x%x) = %d\n", arg(c, 1), (int)r);
        log_geometry(w, "plein écran");
    }
    else if (!strcmp(n, "SetWindowDisplayMode")) {
        SDL_Window *w = WINDOW(0); uint32_t address = arg(c, 1);
        SDL_DisplayMode mode;
        if (address) { mode = get_mode(c, address); scale_mode(&mode, 0); }
        r = (uint32_t)SDL_SetWindowDisplayMode(w, address ? &mode : NULL);
    } else if (!strcmp(n, "GetWindowDisplayMode")) {
        SDL_Window *w = WINDOW(0); uint32_t address = arg(c, 1);
        SDL_DisplayMode mode = {0};
        if (address) rt_write_span(c, address, RT_SDL_DISPLAYMODE_SIZE);
        int result = SDL_GetWindowDisplayMode(w, address ? &mode : NULL);
        if (!result) mode.h = fullscreen_height(SDL_GetWindowDisplayIndex(w), mode.h);
        scale_mode(&mode, 1);
        r = (uint32_t)mode_result(c, result, address, &mode);
    }
    else if (!strcmp(n, "GetWindowDisplayIndex")) r = (uint32_t)SDL_GetWindowDisplayIndex(WINDOW(0));
    else if (!strcmp(n, "GetWindowID")) r = SDL_GetWindowID(WINDOW(0));
    else if (!strcmp(n, "GetWindowGrab")) r = (uint32_t)SDL_GetWindowGrab(WINDOW(0));
    else if (!strcmp(n, "SetWindowGrab")) SDL_SetWindowGrab(WINDOW(0), arg(c, 1) ? SDL_TRUE : SDL_FALSE);
    else if (!strcmp(n, "SetWindowGammaRamp")) {
        SDL_Window *w = WINDOW(0);
        const Uint16 *ramp[3];
        for (unsigned i = 0; i < 3; ++i) ramp[i] = arg(c, 1 + i) ? rt_span(c, arg(c, 1 + i), 512) : NULL;
        r = (uint32_t)SDL_SetWindowGammaRamp(w, ramp[0], ramp[1], ramp[2]);
    } else if (!strcmp(n, "WarpMouseInWindow")) SDL_WarpMouseInWindow(WINDOW(0), (int)arg(c, 1) / window_scale_value, (int)arg(c, 2) / window_scale_value);
    // Écrans
    else if (!strcmp(n, "GetNumVideoDisplays")) r = (uint32_t)SDL_GetNumVideoDisplays();
    else if (!strcmp(n, "GetDisplayBounds")) {
        uint32_t address = arg(c, 1);
        SDL_Rect rect;
        if (!address) r = (uint32_t)SDL_GetDisplayBounds((int)arg(c, 0), NULL);
        else {
            rt_write_span(c, address, RT_SDL_RECT_SIZE);
            if (!(r = (uint32_t)SDL_GetDisplayBounds((int)arg(c, 0), &rect))) {
                rect.h = fullscreen_height((int)arg(c, 0), rect.h);
                W32(address, (uint32_t)screen_pixels(rect.x)); W32(address + 4, (uint32_t)screen_pixels(rect.y));
                W32(address + 8, (uint32_t)screen_pixels(rect.w)); W32(address + 12, (uint32_t)screen_pixels(rect.h));
            }
        }
    } else if (!strcmp(n, "GetCurrentDisplayMode") || !strcmp(n, "GetDesktopDisplayMode")) {
        uint32_t address = arg(c, 1);
        SDL_DisplayMode mode = {0};
        if (address) rt_write_span(c, address, RT_SDL_DISPLAYMODE_SIZE);
        int index = (int)arg(c, 0);
        int result = n[3] == 'C' ? SDL_GetCurrentDisplayMode(index, address ? &mode : NULL)
                                 : SDL_GetDesktopDisplayMode(index, address ? &mode : NULL);
        if (!result) mode.h = fullscreen_height(index, mode.h);
        scale_mode(&mode, 1);
        r = (uint32_t)mode_result(c, result, address, &mode);
    }
    else if (!strcmp(n, "GetCurrentVideoDriver")) r = const_string(c, SDL_GetCurrentVideoDriver());
    else if (!strcmp(n, "GetCurrentAudioDriver")) r = const_string(c, SDL_GetCurrentAudioDriver());
    // OpenGL
    else if (!strcmp(n, "GL_LoadLibrary")) {
        // Un chemin invité désignerait une bibliothèque GL Linux, inutilisable ici.
        if (arg(c, 0)) rt_trap(c, c->eip, "SDL_GL_LoadLibrary with an explicit Linux library path");
        r = (uint32_t)SDL_GL_LoadLibrary(NULL);
    }
    else if (!strcmp(n, "GL_UnloadLibrary")) SDL_GL_UnloadLibrary();
    else if (!strcmp(n, "GL_SetAttribute")) {
        uint32_t attribute = arg(c, 0), value = arg(c, 1);
        r = (uint32_t)SDL_GL_SetAttribute((SDL_GLattr)attribute, (int)value);
        fprintf(stderr, "[sdl] SDL_GL_SetAttribute(%s, %#x) = %d\n", gl_attribute_name(attribute), value, (int)r);
    } else if (!strcmp(n, "GL_CreateContext")) {
        require_main_thread(c, "SDL_GL_CreateContext must run on the macOS main thread");
        SDL_GLContext context = SDL_GL_CreateContext(WINDOW(0));
        if (context && !(r = rt_handle_new(context, RT_SDL_GLCONTEXT))) {
            SDL_GL_DeleteContext(context); rt_trap(c, c->eip, "SDL: handle table full");
        }
        if (!context) fprintf(stderr, "[sdl] SDL_GL_CreateContext failed: %s\n", SDL_GetError());
    } else if (!strcmp(n, "GL_DeleteContext")) {
        SDL_GLContext context = handle(c, arg(c, 0), RT_SDL_GLCONTEXT, "SDL: invalid SDL_GLContext handle");
        if (context) { rt_handle_free(arg(c, 0), RT_SDL_GLCONTEXT); SDL_GL_DeleteContext(context); }
    } else if (!strcmp(n, "GL_MakeCurrent")) {
        SDL_Window *w = WINDOW(0);
        r = (uint32_t)SDL_GL_MakeCurrent(w, handle(c, arg(c, 1), RT_SDL_GLCONTEXT, "SDL: invalid SDL_GLContext handle"));
    }
    else if (!strcmp(n, "GL_SetSwapInterval")) {
        // Fenêtre cachée (diagnostic) : jamais rafraîchie par l'écran, la
        // synchronisation verticale de SDL3/Cocoa y attend indéfiniment.
        int interval = hidden_windows() ? 0 : (int)arg(c, 0);
        r = (uint32_t)SDL_GL_SetSwapInterval(interval);
    }
    else if (!strcmp(n, "GL_SwapWindow")) {
        uint64_t frame_start = rt_frame_times_enabled() ? rt_frame_times_now() : 0;   // P2_FRAME_TIMES
        SDL_Window *w = WINDOW(0);
        if (w) { int dw = 0, dh = 0; drawable_size(w, &dw, &dh); fps_overlay_frame(dw, dh); }
        static unsigned swaps;
        if (w && ++swaps % 30 == 0) log_geometry(w, "swap");   // transitions plein écran asynchrones
        diagnose_frame(w);
        gpu_timing_frame();
        unsigned saved = rt_guest_suspend();   // la synchronisation verticale peut bloquer
        uint64_t start = mach_absolute_time();
        SDL_GL_SwapWindow(w);
        rt_gl_timing_add("SwapWindow", mach_absolute_time() - start);
        rt_gl_pass_frame();
        rt_guest_resume(saved);
        if (frame_start) rt_frame_times_record('g', frame_start);
    } else if (!strcmp(n, "GL_GetProcAddress")) {
        const char *proc = guest_string(c, arg(c, 0));
        r = proc ? rt_gl_proc_address(proc) : 0;
        if (!r) SDL_SetError("SDL bridge: OpenGL function %s is not bridged", proc ? proc : "(null)");
    }
    // Fichiers et surfaces
    else if (!strcmp(n, "RWFromFile")) {
        const char *path = guest_string(c, arg(c, 0)), *mode = guest_string(c, arg(c, 1));
        if (!path || !mode) { SDL_SetError("Parameter '%s' is invalid", path ? "mode" : "file"); r = 0; }
        else {
            int writing = mode[0] == 'w' || mode[0] == 'a' || strchr(mode, '+');
            const char *resolved = writing ? rt_write_path(path, mode[0] != 'w') : rt_read_path(path);
            SDL_RWops *rw = resolved ? SDL_RWFromFile(resolved, mode) : NULL;
            if (!resolved) SDL_SetError("Couldn't resolve %s", path);
            if (rw && !(r = rt_handle_new(rw, RT_SDL_RWOPS))) { SDL_RWclose(rw); rt_trap(c, c->eip, "SDL: handle table full"); }
        }
    } else if (!strcmp(n, "LoadBMP_RW")) {
        uint32_t source = arg(c, 0), free_source = arg(c, 1);
        SDL_RWops *rw = handle(c, source, RT_SDL_RWOPS, "SDL: invalid SDL_RWops handle");
        if (rw && free_source) rt_handle_free(source, RT_SDL_RWOPS);
        r = export_surface(c, SDL_LoadBMP_RW(rw, (int)free_source));
    }
    else if (!strcmp(n, "FreeSurface")) free_surface(c, arg(c, 0));
    // Curseurs
    else if (!strcmp(n, "CreateColorCursor") || !strcmp(n, "CreateSystemCursor")) {
        SDL_Cursor *cursor = n[6] == 'C' ? SDL_CreateColorCursor(rt_sdl_surface_sync(c, arg(c, 0)), (int)arg(c, 1), (int)arg(c, 2))
                                         : SDL_CreateSystemCursor((SDL_SystemCursor)arg(c, 0));
        if (cursor && !(r = rt_handle_new(cursor, RT_SDL_CURSOR))) { SDL_FreeCursor(cursor); rt_trap(c, c->eip, "SDL: handle table full"); }
    } else if (!strcmp(n, "SetCursor")) SDL_SetCursor(handle(c, arg(c, 0), RT_SDL_CURSOR, "SDL: invalid SDL_Cursor handle"));
    else if (!strcmp(n, "FreeCursor")) {
        SDL_Cursor *cursor = handle(c, arg(c, 0), RT_SDL_CURSOR, "SDL: invalid SDL_Cursor handle");
        if (cursor) { rt_handle_free(arg(c, 0), RT_SDL_CURSOR); SDL_FreeCursor(cursor); }
    }
    else if (!strcmp(n, "ShowCursor")) r = (uint32_t)SDL_ShowCursor((int)arg(c, 0));
    // Presse-papiers
    else if (!strcmp(n, "GetClipboardText")) {
        char *text = SDL_GetClipboardText();
        r = heap_string(c, text ? text : "");
        SDL_free(text);
    }
    else if (!strcmp(n, "SetClipboardText")) r = (uint32_t)SDL_SetClipboardText(guest_string(c, arg(c, 0)));
    else if (!strcmp(n, "HasClipboardText")) r = (uint32_t)SDL_HasClipboardText();
    else return 0;
    c->eax = r;
    static int trace = -1;
    if (trace < 0) trace = getenv("RT_SDL_TRACE") != NULL;
    if (trace) fprintf(stderr, "[sdl] %s(%#x, %#x, %#x) = %#x\n", name, arg(c, 0), arg(c, 1), arg(c, 2), r);
    return 1;
}
RT_SHIM_PROVIDER(sdl_video)
