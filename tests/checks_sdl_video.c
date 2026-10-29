// Pont SDL2 vidéo : appels invités cdecl i386 simulés, pilote vidéo « dummy »
// (aucune fenêtre visible, presse-papiers interne de SDL, pas celui du Mac).
#include "checks.h"
#include "sdl_common.h"
#include "frame_times.h"
#include <limits.h>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <pthread.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#define STACK 0x20010000u
#define DATA 0x20014000u   // chaînes et structures passées par le code invité

static uint32_t call(const char *name, unsigned count, ...) {
    CPU c; rt_cpu_reset(&c); c.esp = STACK; c.eip = 0x1234;
    W32(c.esp, RT_RETURN_SENTINEL);
    va_list list; va_start(list, count);
    for (unsigned i = 0; i < count; ++i) W32(c.esp + 4 + 4 * i, va_arg(list, uint32_t));
    va_end(list);
    assert(rt_shim(&c, name) == 1);
    assert(c.esp == STACK);
    return c.eax;
}
static uint32_t put_string(uint32_t address, const char *text) {
    memcpy(GPTR(address), text, strlen(text) + 1);
    return address;
}
static const char *guest(uint32_t address) { assert(address); return (const char *)GPTR(address); }

// Démarre la vidéo SDL avec le pilote dummy, via les shims ; renvoie 0 si un
// autre test a déjà initialisé la vidéo avec un autre pilote.
static int start_dummy_video(void) {
    // Comme sous Linux, la variable d'environnement prime sur un indice normal.
    int environment = getenv("SDL_VIDEODRIVER") != NULL;
    uint32_t set = call("SDL_SetHint", 2, put_string(DATA, SDL_HINT_VIDEODRIVER), put_string(DATA + 64, "dummy"));
    assert(set == (environment ? SDL_FALSE : SDL_TRUE));
    if (!environment) assert(!strcmp(SDL_GetHint(SDL_HINT_VIDEODRIVER), "dummy"));
    assert(call("SDL_InitSubSystem", 1, SDL_INIT_VIDEO) == 0);
    assert(call("SDL_WasInit", 1, SDL_INIT_VIDEO) & SDL_INIT_VIDEO);
    return !strcmp(SDL_GetCurrentVideoDriver(), "dummy");
}
static void stop_video(void) {
    call("SDL_QuitSubSystem", 1, SDL_INIT_VIDEO);
    SDL_ResetHint(SDL_HINT_VIDEODRIVER);
}

RT_CHECK(sdl_video_displays_and_strings) {
    start_dummy_video();
    uint32_t driver = call("SDL_GetCurrentVideoDriver", 0);
    assert(!strcmp(guest(driver), SDL_GetCurrentVideoDriver()));
    assert(call("SDL_GetCurrentVideoDriver", 0) == driver);   // copie stable, pas de fuite
    int displays = (int)call("SDL_GetNumVideoDisplays", 0);
    assert(displays >= 1 && displays == SDL_GetNumVideoDisplays());

    SDL_Rect bounds; assert(!SDL_GetDisplayBounds(0, &bounds));
    memset(GPTR(DATA), 0xcc, 32);
    assert(call("SDL_GetDisplayBounds", 2, 0, DATA) == 0);
    assert((int)R32(DATA) == bounds.x && (int)R32(DATA + 4) == bounds.y);
    assert((int)R32(DATA + 8) == bounds.w && (int)R32(DATA + 12) == bounds.h && R32(DATA + 16) == 0xccccccccu);

    // SDL_DisplayMode i386 : 20 octets, driverdata jamais exposé.
    SDL_DisplayMode desktop; assert(!SDL_GetDesktopDisplayMode(0, &desktop));
    memset(GPTR(DATA), 0xcc, 32);
    assert(call("SDL_GetDesktopDisplayMode", 2, 0, DATA) == 0);
    assert(R32(DATA) == desktop.format && (int)R32(DATA + 4) == desktop.w && (int)R32(DATA + 8) == desktop.h);
    assert((int)R32(DATA + 12) == desktop.refresh_rate && R32(DATA + 16) == 0 && R32(DATA + 20) == 0xccccccccu);
    SDL_DisplayMode current; assert(!SDL_GetCurrentDisplayMode(0, &current));
    assert(call("SDL_GetCurrentDisplayMode", 2, 0, DATA) == 0 && (int)R32(DATA + 4) == current.w);

    // Erreur : rien n'est écrit, SDL_GetError renvoie la chaîne hôte.
    memset(GPTR(DATA), 0xcc, 32);
    assert((int32_t)call("SDL_GetCurrentDisplayMode", 2, 99, DATA) < 0 && R32(DATA) == 0xccccccccu);
    uint32_t error = call("SDL_GetError", 0);
    assert(*guest(error) && !strcmp(guest(error), SDL_GetError()));
    assert(call("SDL_GetError", 0) == error);
    stop_video();
}

RT_CHECK(sdl_video_window_lifecycle) {
    int dummy = start_dummy_video();
    uint32_t window = call("SDL_CreateWindow", 6, put_string(DATA, "Portal 2"), SDL_WINDOWPOS_UNDEFINED,
                           SDL_WINDOWPOS_UNDEFINED, 320, 200, SDL_WINDOW_HIDDEN);
    assert(window >= RT_HANDLE_BASE);
    SDL_Window *host = rt_handle_get(window, RT_SDL_WINDOW);
    assert(host && !strcmp(SDL_GetWindowTitle(host), "Portal 2"));
    W32(DATA, 0); W32(DATA + 4, 0);
    call("SDL_GetWindowSize", 3, window, DATA, DATA + 4);
    assert(R32(DATA) == 320 && R32(DATA + 4) == 200);
    call("SDL_SetWindowSize", 3, window, 640, 480);
    W32(DATA, 0);
    call("SDL_GetWindowSize", 3, window, DATA, 0);      // pointeur nul accepté
    assert(R32(DATA) == 640);
    call("SDL_SetWindowTitle", 2, window, put_string(DATA + 64, "Aperture"));
    assert(!strcmp(SDL_GetWindowTitle(host), "Aperture"));
    assert(call("SDL_GetWindowID", 1, window) == SDL_GetWindowID(host));
    assert(call("SDL_GetWindowDisplayIndex", 1, window) == 0);
    call("SDL_SetWindowGrab", 2, window, 0);
    assert(call("SDL_GetWindowGrab", 1, window) == SDL_FALSE);
    memset(GPTR(DATA), 0xcc, 32);
    assert(call("SDL_GetWindowDisplayMode", 2, window, DATA) == 0);
    SDL_DisplayMode mode; assert(!SDL_GetWindowDisplayMode(host, &mode));
    assert((int)R32(DATA + 4) == mode.w && R32(DATA + 16) == 0 && R32(DATA + 20) == 0xccccccccu);
    // Mode demandé recopié depuis la structure invitée (driverdata ignoré).
    W32(DATA + 16, 0xdeadbeef);
    assert(call("SDL_SetWindowDisplayMode", 2, window, DATA) == 0);
    assert(call("SDL_SetWindowDisplayMode", 2, window, 0) == 0);
    call("SDL_ShowWindow", 1, window);
    assert(SDL_GetWindowFlags(host) & SDL_WINDOW_SHOWN);
    call("SDL_HideWindow", 1, window);
    assert(!(SDL_GetWindowFlags(host) & SDL_WINDOW_SHOWN));
    call("SDL_DestroyWindow", 1, window);
    assert(!rt_handle_get(window, RT_SDL_WINDOW));
    // Contexte GL : le pilote dummy n'a pas d'OpenGL (échec propre, erreur
    // transmise) ; avec SDL_VIDEODRIVER=cocoa, vrai contexte sur fenêtre cachée.
    window = call("SDL_CreateWindow", 6, put_string(DATA, "GL"), 0, 0, 64, 64, SDL_WINDOW_HIDDEN | SDL_WINDOW_OPENGL);
    if (!dummy) assert(window);
    if (window) {
        uint32_t context = call("SDL_GL_CreateContext", 1, window);
        if (dummy) assert(!context && *guest(call("SDL_GetError", 0)));
        else {
            assert(context && rt_handle_get(context, RT_SDL_GLCONTEXT) == SDL_GL_GetCurrentContext());
            assert(call("SDL_GL_MakeCurrent", 2, window, context) == 0);
            call("SDL_GL_SetSwapInterval", 1, 0);
            call("SDL_GL_SwapWindow", 1, window);
            assert(call("SDL_GL_MakeCurrent", 2, window, 0) == 0 && !SDL_GL_GetCurrentContext());
            call("SDL_GL_DeleteContext", 1, context);
            assert(!rt_handle_get(context, RT_SDL_GLCONTEXT));
        }
    }
    call("SDL_DestroyWindow", 1, window);
    assert(!rt_handle_get(window, RT_SDL_WINDOW));
    // Une fenêtre nulle passe à SDL, qui signale l'erreur comme sous Linux.
    assert((int32_t)call("SDL_GetWindowDisplayIndex", 1, 0) < 0);
    stop_video();
}

RT_CHECK(sdl_video_gl_attributes_and_procs) {
    start_dummy_video();
    assert(call("SDL_GL_SetAttribute", 2, SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY) == 0);
    assert(call("SDL_GL_SetAttribute", 2, SDL_GL_DEPTH_SIZE, 24) == 0);
    int value = 0;
    assert(!SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &value) && value == SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
    SDL_GL_ResetAttributes();
    assert(call("SDL_GL_GetProcAddress", 1, put_string(DATA, "glNotARealFunctionAnywhere")) == 0);
    assert(strstr(guest(call("SDL_GetError", 0)), "glNotARealFunctionAnywhere"));
    stop_video();
}

// Écrit une BMP via SDL hôte : 3×2 en BGR24, ou 2×2 indexée 8 bits.
static void write_bmp(const char *path, int indexed) {
    SDL_Surface *s = SDL_CreateRGBSurfaceWithFormat(0, indexed ? 2 : 3, 2, indexed ? 8 : 24,
                                                    indexed ? SDL_PIXELFORMAT_INDEX8 : SDL_PIXELFORMAT_BGR24);
    assert(s);
    for (int y = 0; y < s->h; ++y)
        for (int x = 0; x < s->pitch; ++x) ((uint8_t *)s->pixels)[y * s->pitch + x] = (uint8_t)(y * 16 + x);
    if (indexed) {
        SDL_Color colors[4] = {{1, 2, 3, 255}, {4, 5, 6, 255}, {7, 8, 9, 255}, {10, 11, 12, 255}};
        assert(!SDL_SetPaletteColors(s->format->palette, colors, 0, 4));
    }
    assert(!SDL_SaveBMP(s, path));
    SDL_FreeSurface(s);
}

RT_CHECK(sdl_video_surfaces_have_i386_layout) {
    char path[] = "/tmp/rt-sdl-bmp-XXXXXX";
    int fd = mkstemp(path); assert(fd >= 0); close(fd);
    for (int indexed = 0; indexed < 2; ++indexed) {
        write_bmp(path, indexed);
        SDL_Surface *expected = SDL_LoadBMP(path); assert(expected);
        uint32_t rw = call("SDL_RWFromFile", 2, put_string(DATA, path), put_string(DATA + 256, "rb"));
        assert(rw && rt_handle_get(rw, RT_SDL_RWOPS));
        uint32_t s = call("SDL_LoadBMP_RW", 2, rw, 1);
        assert(s && !rt_handle_get(rw, RT_SDL_RWOPS));          // freesrc : RWops libéré
        assert(R32(s + 8) == (uint32_t)expected->w && R32(s + 12) == (uint32_t)expected->h);
        assert(R32(s + 16) == (uint32_t)expected->pitch && R32(s + 56) == 1);
        uint32_t format = R32(s + 4), pixels = R32(s + 20);
        assert(R32(format) == expected->format->format);
        assert(R8(format + 8) == expected->format->BitsPerPixel && R8(format + 9) == expected->format->BytesPerPixel);
        assert(R32(format + 12) == expected->format->Rmask && R8(format + 32) == expected->format->Rshift);
        assert(!memcmp(GPTR(pixels), expected->pixels, (size_t)(expected->pitch * expected->h)));
        assert((int)R32(s + 44) == expected->clip_rect.w && (int)R32(s + 48) == expected->clip_rect.h);
        uint32_t palette = R32(format + 4);
        if (indexed) {
            assert(palette && (int)R32(palette) == expected->format->palette->ncolors);
            uint32_t colors = R32(palette + 4);
            assert(R8(colors + 4) == 4 && R8(colors + 5) == 5 && R8(colors + 6) == 6);
            W8(colors + 4, 200);   // modification invitée, recopiée vers l'hôte
        } else assert(!palette && R8(format + 9) == 3);
        // Le code invité modifie un pixel : la surface hôte le reçoit avant usage.
        W8(pixels, 0xab);
        CPU c; rt_cpu_reset(&c);
        SDL_Surface *host = rt_sdl_surface_sync(&c, s);
        assert(host && ((uint8_t *)host->pixels)[0] == 0xab);
        if (indexed) assert(host->format->palette->colors[1].r == 200);
        // Compteur de références respecté : deux libérations nécessaires.
        W32(s + 56, 2);
        call("SDL_FreeSurface", 1, s);
        assert(R32(s + 56) == 1 && rt_sdl_surface_sync(&c, s) == host);
        call("SDL_FreeSurface", 1, s);
        call("SDL_FreeSurface", 1, 0);
        SDL_FreeSurface(expected);
    }
    unlink(path);
    // Fichier absent : NULL et message d'erreur SDL.
    assert(call("SDL_RWFromFile", 2, put_string(DATA, "/nonexistent/rt-sdl.bmp"), put_string(DATA + 256, "rb")) == 0);
    assert(*guest(call("SDL_GetError", 0)));
}

RT_CHECK(sdl_video_cursors_and_clipboard) {
    if (!start_dummy_video()) { stop_video(); return; }   // jamais le vrai presse-papiers
    uint32_t cursor = call("SDL_CreateSystemCursor", 1, SDL_SYSTEM_CURSOR_ARROW);
    if (cursor) {
        assert(rt_handle_get(cursor, RT_SDL_CURSOR));
        call("SDL_SetCursor", 1, cursor);
        call("SDL_FreeCursor", 1, cursor);
        assert(!rt_handle_get(cursor, RT_SDL_CURSOR));
    }
    uint32_t shown = call("SDL_ShowCursor", 1, (uint32_t)SDL_QUERY);
    assert(shown == (uint32_t)SDL_ShowCursor(SDL_QUERY));

    assert(call("SDL_SetClipboardText", 1, put_string(DATA, "portal \xc3\xa9")) == 0);
    assert(call("SDL_HasClipboardText", 0) == SDL_TRUE);
    uint32_t text = call("SDL_GetClipboardText", 0);
    assert(!strcmp(guest(text), "portal \xc3\xa9"));
    call("SDL_free", 1, text);
    assert(rt_free(text) == -1);     // déjà rendu au tas invité par SDL_free
    call("SDL_free", 1, 0);
    stop_video();
}

// Un appel piégé (rt_trap → abort) est vérifié dans un processus fils.
static void *create_window_off_main(void *unused) {
    (void)unused;
    call("SDL_CreateWindow", 6, put_string(DATA, "x"), 0, 0, 10, 10, SDL_WINDOW_HIDDEN);
    return NULL;
}
static int traps(void (*body)(void), const char *message) {
    char path[] = "/tmp/rt-sdl-trap-XXXXXX";
    int fd = mkstemp(path); assert(fd >= 0);
    fflush(NULL);
    pid_t pid = fork(); assert(pid >= 0);
    if (!pid) { dup2(fd, 2); body(); _exit(0); }
    int status; assert(waitpid(pid, &status, 0) == pid);
    char log[4096] = {0};
    ssize_t n = pread(fd, log, sizeof(log) - 1, 0);
    close(fd); unlink(path);
    return n > 0 && WIFSIGNALED(status) && WTERMSIG(status) == SIGABRT && strstr(log, message);
}
static void off_main_thread(void) {
    pthread_t thread; assert(!pthread_create(&thread, NULL, create_window_off_main, NULL));
    pthread_join(thread, NULL);
}
static void stale_window(void) { call("SDL_ShowWindow", 1, RT_HANDLE_BASE + 0x7f0000); }
static void foreign_free(void) { call("SDL_free", 1, DATA); }
static void const_free(void) { call("SDL_free", 1, call("SDL_GetError", 0)); }

RT_CHECK(sdl_video_traps_instead_of_lying) {
    // macOS : fenêtres sur le thread principal seulement.
    assert(traps(off_main_thread, "SDL_CreateWindow must run on the macOS main thread"));
    assert(traps(stale_window, "invalid SDL_Window handle"));
    assert(traps(foreign_free, "SDL_free: block not allocated"));
    assert(traps(const_free, "SDL_free: block not allocated"));   // chaîne constante de SDL_GetError
    // Toute chaîne du tas invité (ex. SDL_DROPFILE du pont entrées) est libérable.
    uint32_t block = rt_alloc(8); assert(block);
    call("SDL_free", 1, block);
    assert(rt_free(block) == -1);
}

// Retina (échelle d'écran imposée à 2) : écran annoncé en pixels ; une
// résolution qui tient à l'écran en points garde une fenêtre à l'échelle 1,
// une plus grande donne une fenêtre de moitié en points.
RT_CHECK(sdl_video_retina_window_follows_resolution) {
    start_dummy_video();
    rt_sdl_force_scales(2, 1);
    SDL_Rect usable; assert(!SDL_GetDisplayUsableBounds(0, &usable));
    int big_w = usable.w * 2, big_h = usable.h * 2;   // ne tient pas en points
    uint32_t window = call("SDL_CreateWindow", 6, put_string(DATA, "Retina"), SDL_WINDOWPOS_CENTERED,
                           SDL_WINDOWPOS_CENTERED, big_w, big_h, SDL_WINDOW_HIDDEN);
    SDL_Window *host = rt_handle_get(window, RT_SDL_WINDOW);
    int w = 0, h = 0; SDL_GetWindowSize(host, &w, &h);
    assert(w == usable.w && h == usable.h && rt_sdl_window_scale() == 2);
    call("SDL_GetWindowSize", 3, window, DATA, DATA + 4);
    assert((int)R32(DATA) == big_w && (int)R32(DATA + 4) == big_h);
    // Petite résolution : échelle 1, fenêtre de la même taille en points.
    call("SDL_SetWindowSize", 3, window, usable.w / 2, usable.h / 2);
    SDL_GetWindowSize(host, &w, &h);
    assert(w == usable.w / 2 && h == usable.h / 2 && rt_sdl_window_scale() == 1);
    call("SDL_GetWindowSize", 3, window, DATA, DATA + 4);
    assert((int)R32(DATA) == usable.w / 2 && (int)R32(DATA + 4) == usable.h / 2);
    call("SDL_SetWindowSize", 3, window, big_w, big_h);
    assert(rt_sdl_window_scale() == 2);
    SDL_Rect bounds; assert(!SDL_GetDisplayBounds(0, &bounds));
    assert(call("SDL_GetDisplayBounds", 2, 0, DATA) == 0);
    assert((int)R32(DATA + 8) == 2 * bounds.w && (int)R32(DATA + 12) == 2 * bounds.h);
    SDL_DisplayMode desktop; assert(!SDL_GetDesktopDisplayMode(0, &desktop));
    assert(call("SDL_GetDesktopDisplayMode", 2, 0, DATA) == 0);
    assert((int)R32(DATA + 4) == 2 * desktop.w && (int)R32(DATA + 8) == 2 * desktop.h);
    call("SDL_DestroyWindow", 1, window);
    rt_sdl_force_scales(0, 0);
    stop_video();
}

RT_CHECK(sdl_video_notch_height) {
    // MacBook Pro 14 pouces réglé en 1800×1169 points : encoche de 38 points,
    // barre des menus jusqu'à 1130 points du bas ; plein écran = 1130.
    assert(rt_sdl_notch_height(1169, 1169, 1130, 38.0) == 1130);
    // Sans encoche (écran externe, Mac plus ancien) : l'écran entier.
    assert(rt_sdl_notch_height(1169, 1169, 1130, 0.0) == 1169);
    // Autre hauteur que l'écran entier (mode non natif) : inchangée.
    assert(rt_sdl_notch_height(900, 1169, 1130, 38.0) == 900);
    // Zone visible absente ou incohérente : l'écran entier.
    assert(rt_sdl_notch_height(1169, 1169, 0, 38.0) == 1169);
    assert(rt_sdl_notch_height(1169, 1169, 1169, 38.0) == 1169);
}

// P2_FRAME_TIMES : SDL_GL_SwapWindow écrit une ligne « g » par échange (même
// sans fenêtre : l'appel est daté avant que SDL ne le refuse), rien fermé.
RT_CHECK(sdl_video_swap_writes_frame_times) {
    char path[PATH_MAX];
    snprintf(path, sizeof(path), "%s/p2-gl-frame-times-%d.log", getenv("TMPDIR") ? getenv("TMPDIR") : "/tmp", (int)getpid());
    assert(rt_frame_times_open(path));
    uint64_t before = rt_frame_times_count();
    for (int i = 0; i < 3; ++i) call("SDL_GL_SwapWindow", 1, 0);
    assert(rt_frame_times_count() == before + 3);
    assert(!rt_frame_times_open(NULL));
    call("SDL_GL_SwapWindow", 1, 0);
    assert(rt_frame_times_count() == before + 3);
    FILE *file = fopen(path, "r");
    assert(file);
    char line[256]; unsigned lines = 0;
    unsigned long long start, present, cpu, last = 0;
    assert(fgets(line, sizeof(line), file) && !strncmp(line, "# p2-frame-times v1 ", 20));
    while (fgets(line, sizeof(line), file)) {
        assert(sscanf(line, "g %llu %llu %llu", &start, &present, &cpu) == 3 && start > last);
        last = start; ++lines;
    }
    fclose(file); unlink(path);
    assert(lines == 3);
}

uint64_t rt_frame_log_tick(void);
// Le compteur d'images est partagé : Present (Direct3D 9) l'avance comme l'échange GL.
RT_CHECK(sdl_video_frame_log_tick_is_shared_and_monotonic) {
    uint64_t a = rt_frame_log_tick(), b = rt_frame_log_tick();
    assert(b == a + 1);
}
