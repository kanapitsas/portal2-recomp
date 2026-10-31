// Pont SDL2 natif : événements, souris, clavier, manettes et retour haptique.
// Le code invité a été compilé pour SDL2 Linux i386 ; l'hôte est SDL2 ARM64
// (sdl2-compat). SDL_Event fait 56 octets des deux côtés et la plupart des
// événements ont la même disposition ; seuls ceux qui contiennent un pointeur
// ou un Uint64 diffèrent et sont traduits champ par champ (voir translate_*).
// SHIMS: SDL_PollEvent SDL_WaitEventTimeout SDL_PushEvent SDL_AddEventWatch SDL_DelEventWatch
// SHIMS: SDL_GetMouseState SDL_SetRelativeMouseMode SDL_SetModState
// SHIMS: SDL_NumJoysticks SDL_IsGameController SDL_GameControllerOpen SDL_GameControllerClose
// SHIMS: SDL_GameControllerGetJoystick SDL_JoystickOpen SDL_JoystickClose SDL_JoystickInstanceID
// SHIMS: SDL_JoystickNameForIndex SDL_JoystickGetDeviceGUID SDL_JoystickGetGUIDString
// SHIMS: SDL_HapticOpenFromJoystick SDL_HapticClose SDL_HapticRumbleInit SDL_HapticRumblePlay
#include "modules.h"
#include "sdl_common.h"
#include "touch_shaper.h"
#include <SDL2/SDL.h>
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE
#include "ios/sdl_ios.h"   // coordonnées en pixels pour le jeu
#endif
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define KIND_JOYSTICK 'SDLJ'
#define KIND_CONTROLLER 'SDLK'
#define KIND_HAPTIC 'SDLH'
#define GUEST_EVENT_SIZE 56u

// Disposition i386 des événements qui diffèrent de l'ARM64 (mesurée avec
// clang -target i386-linux-gnu sur les en-têtes SDL2 2.32).
enum {
    I386_USER_WINDOW = 8, I386_USER_CODE = 12, I386_USER_DATA1 = 16, I386_USER_DATA2 = 20,
    I386_DROP_FILE = 8, I386_DROP_WINDOW = 12,
    I386_EDITEXT_WINDOW = 8, I386_EDITEXT_TEXT = 12, I386_EDITEXT_START = 16, I386_EDITEXT_LENGTH = 20,
    I386_SENSOR_WHICH = 8, I386_SENSOR_DATA = 12, I386_SENSOR_TIMESTAMP_US = 36,
    I386_CSENSOR_WHICH = 8, I386_CSENSOR_SENSOR = 12, I386_CSENSOR_DATA = 16, I386_CSENSOR_TIMESTAMP_US = 28,
};
_Static_assert(sizeof(SDL_Event) == GUEST_EVENT_SIZE, "SDL_Event hôte de 56 octets");
_Static_assert(sizeof(SDL_Keysym) == 16 && offsetof(SDL_KeyboardEvent, keysym) == 16, "SDL_Keysym identique en i386");
_Static_assert(sizeof(SDL_JoystickGUID) == 16, "SDL_JoystickGUID de 16 octets");
// Événements sans pointeur ni champ 64 bits mal aligné : mêmes décalages en
// i386 et en ARM64 (les identifiants tactiles 64 bits tombent déjà sur 8).
_Static_assert(offsetof(SDL_TouchFingerEvent, fingerId) == 16 && offsetof(SDL_TouchFingerEvent, windowID) == 44, "doigt");
_Static_assert(offsetof(SDL_DollarGestureEvent, gestureId) == 16 && offsetof(SDL_DollarGestureEvent, y) == 36, "geste");
_Static_assert(offsetof(SDL_MouseWheelEvent, mouseY) == 40, "molette");

static uint32_t argument(CPU *c, unsigned i) { return R32(c->esp + 4 + 4 * i); }

static int plain_event(uint32_t type) {
    switch (type) {
    case SDL_QUIT: case SDL_APP_TERMINATING: case SDL_APP_LOWMEMORY:
    case SDL_APP_WILLENTERBACKGROUND: case SDL_APP_DIDENTERBACKGROUND:
    case SDL_APP_WILLENTERFOREGROUND: case SDL_APP_DIDENTERFOREGROUND: case SDL_LOCALECHANGED:
    case SDL_DISPLAYEVENT: case SDL_WINDOWEVENT:
    case SDL_KEYDOWN: case SDL_KEYUP: case SDL_TEXTEDITING: case SDL_TEXTINPUT: case SDL_KEYMAPCHANGED:
    case SDL_MOUSEMOTION: case SDL_MOUSEBUTTONDOWN: case SDL_MOUSEBUTTONUP: case SDL_MOUSEWHEEL:
    case SDL_JOYAXISMOTION: case SDL_JOYBALLMOTION: case SDL_JOYHATMOTION: case SDL_JOYBUTTONDOWN:
    case SDL_JOYBUTTONUP: case SDL_JOYDEVICEADDED: case SDL_JOYDEVICEREMOVED: case SDL_JOYBATTERYUPDATED:
    case SDL_CONTROLLERAXISMOTION: case SDL_CONTROLLERBUTTONDOWN: case SDL_CONTROLLERBUTTONUP:
    case SDL_CONTROLLERDEVICEADDED: case SDL_CONTROLLERDEVICEREMOVED: case SDL_CONTROLLERDEVICEREMAPPED:
    case SDL_CONTROLLERTOUCHPADDOWN: case SDL_CONTROLLERTOUCHPADMOTION: case SDL_CONTROLLERTOUCHPADUP:
    case SDL_CONTROLLERSTEAMHANDLEUPDATED:
    case SDL_FINGERDOWN: case SDL_FINGERUP: case SDL_FINGERMOTION:
    case SDL_DOLLARGESTURE: case SDL_DOLLARRECORD: case SDL_MULTIGESTURE:
    case SDL_CLIPBOARDUPDATE: case SDL_AUDIODEVICEADDED: case SDL_AUDIODEVICEREMOVED:
    case SDL_RENDER_TARGETS_RESET: case SDL_RENDER_DEVICE_RESET: case SDL_POLLSENTINEL:
        return 1;
    default: return 0;
    }
}
static int string_event(uint32_t type) {
    return type == SDL_DROPFILE || type == SDL_DROPTEXT || type == SDL_DROPBEGIN || type == SDL_DROPCOMPLETE;
}

// Types qu'un vrai SDL2 peut produire. sdl2-compat (SDL3 dessous) laisse
// passer vers les observateurs certains événements SDL3 sans équivalent SDL2
// (ex. 0x205, SDL_EVENT_WINDOW_MOVED) : un jeu SDL2 ne les verrait jamais,
// ils ne lui sont donc pas transmis (ni par observateur ni par PollEvent).
int rt_sdl_event_is_sdl2(uint32_t type);
void fps_overlay_toggle(void);
int rt_sdl_event_is_sdl2(uint32_t type) {
    return plain_event(type) || string_event(type) || (type >= SDL_USEREVENT && type <= SDL_LASTEVENT) ||
           type == SDL_TEXTEDITING_EXT || type == SDL_SENSORUPDATE || type == SDL_CONTROLLERSENSORUPDATE ||
           type == SDL_SYSWMEVENT;
}

// Copie d'une chaîne hôte en mémoire invitée (rt_alloc) ; 0 pour NULL.
static uint32_t guest_string(CPU *c, const char *text) {
    if (!text) return 0;
    size_t size = strlen(text) + 1;
    uint32_t address = size <= UINT32_MAX ? rt_alloc((uint32_t)size) : 0;
    if (!address) rt_trap(c, c->eip, "SDL event string: guest allocation failed");
    memcpy(GPTR(address), text, size);
    return address;
}

static void put32(uint8_t *out, unsigned offset, uint32_t value) { memcpy(out + offset, &value, 4); }
static uint32_t get32(const uint8_t *in, unsigned offset) { uint32_t v; memcpy(&v, in + offset, 4); return v; }

// Hôte → invité. Les chaînes (SDL_DROP*, SDL_TEXTEDITING_EXT) sont recopiées
// avec rt_alloc : l'invité les libère par SDL_free, qui doit donc appeler
// rt_free pour une adresse du tas invité. Si `release_host`, la chaîne hôte
// (propriété de l'application après SDL_PollEvent) est libérée ici.
void rt_sdl_event_to_guest(CPU *c, const SDL_Event *host, uint32_t guest, int release_host);
#define event_to_guest rt_sdl_event_to_guest
// Retina (sdl_video.c) : positions de souris dans la fenêtre et tailles de
// fenêtre converties de points en pixels du jeu, position de la fenêtre en
// pixels de l'écran. Les déplacements relatifs (visée) restent en points pour
// garder la même sensibilité.
static void scale_to_pixels(SDL_Event *e, int window_scale, int display_scale) {
    switch (e->type) {
    case SDL_MOUSEMOTION: e->motion.x *= window_scale; e->motion.y *= window_scale; break;
    case SDL_MOUSEBUTTONDOWN: case SDL_MOUSEBUTTONUP: e->button.x *= window_scale; e->button.y *= window_scale; break;
    case SDL_WINDOWEVENT:
        if (e->window.event == SDL_WINDOWEVENT_RESIZED || e->window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
            e->window.data1 *= window_scale; e->window.data2 *= window_scale;
        } else if (e->window.event == SDL_WINDOWEVENT_MOVED) {
            e->window.data1 *= display_scale; e->window.data2 *= display_scale;
        }
        break;
    }
}

void rt_sdl_event_to_guest(CPU *c, const SDL_Event *host, uint32_t guest, int release_host) {
    uint8_t out[GUEST_EVENT_SIZE] = {0};
    uint32_t type = host->type;
    int window_scale = rt_sdl_window_scale(), display_scale = rt_sdl_display_scale();
    if (plain_event(type) && (window_scale != 1 || display_scale != 1)) {
        SDL_Event scaled = *host;
        scale_to_pixels(&scaled, window_scale, display_scale);
        memcpy(out, &scaled, GUEST_EVENT_SIZE);
    } else if (plain_event(type)) memcpy(out, host, GUEST_EVENT_SIZE);
    else if (type >= SDL_USEREVENT && type <= SDL_LASTEVENT) {
        uintptr_t d1 = (uintptr_t)host->user.data1, d2 = (uintptr_t)host->user.data2;
        if (d1 > UINT32_MAX || d2 > UINT32_MAX)
            rt_trap(c, c->eip, "SDL_USEREVENT carries a host pointer, not a guest address");
        put32(out, 0, type); put32(out, 4, host->user.timestamp);
        put32(out, I386_USER_WINDOW, host->user.windowID); put32(out, I386_USER_CODE, (uint32_t)host->user.code);
        put32(out, I386_USER_DATA1, (uint32_t)d1); put32(out, I386_USER_DATA2, (uint32_t)d2);
    } else if (string_event(type)) {
        put32(out, 0, type); put32(out, 4, host->drop.timestamp);
        put32(out, I386_DROP_FILE, guest_string(c, host->drop.file));
        put32(out, I386_DROP_WINDOW, host->drop.windowID);
        if (release_host) SDL_free(host->drop.file);
    } else if (type == SDL_TEXTEDITING_EXT) {
        put32(out, 0, type); put32(out, 4, host->editExt.timestamp);
        put32(out, I386_EDITEXT_WINDOW, host->editExt.windowID);
        put32(out, I386_EDITEXT_TEXT, guest_string(c, host->editExt.text));
        put32(out, I386_EDITEXT_START, (uint32_t)host->editExt.start);
        put32(out, I386_EDITEXT_LENGTH, (uint32_t)host->editExt.length);
        if (release_host) SDL_free(host->editExt.text);
    } else if (type == SDL_SENSORUPDATE) {
        put32(out, 0, type); put32(out, 4, host->sensor.timestamp);
        put32(out, I386_SENSOR_WHICH, (uint32_t)host->sensor.which);
        memcpy(out + I386_SENSOR_DATA, host->sensor.data, sizeof(host->sensor.data));
        memcpy(out + I386_SENSOR_TIMESTAMP_US, &host->sensor.timestamp_us, 8);
    } else if (type == SDL_CONTROLLERSENSORUPDATE) {
        put32(out, 0, type); put32(out, 4, host->csensor.timestamp);
        put32(out, I386_CSENSOR_WHICH, (uint32_t)host->csensor.which);
        put32(out, I386_CSENSOR_SENSOR, (uint32_t)host->csensor.sensor);
        memcpy(out + I386_CSENSOR_DATA, host->csensor.data, sizeof(host->csensor.data));
        memcpy(out + I386_CSENSOR_TIMESTAMP_US, &host->csensor.timestamp_us, 8);
    } else if (type == SDL_SYSWMEVENT) {
        // Le message natif est Cocoa, pas le SDL_SysWMmsg X11 attendu par l'invité.
        rt_trap(c, c->eip, "SDL_SYSWMEVENT cannot be translated to the Linux/X11 guest ABI");
    } else {
        char why[96];
        snprintf(why, sizeof(why), "unknown SDL event type %#x cannot be translated", type);
        rt_trap(c, c->eip, why);
    }
    memcpy(rt_write_span(c, guest, GUEST_EVENT_SIZE), out, GUEST_EVENT_SIZE);
}

// Invité → hôte (SDL_PushEvent). Les adresses invitées de SDL_USEREVENT sont
// conservées telles quelles dans data1/data2 et retrouvées à la relecture.
void rt_sdl_event_to_host(CPU *c, uint32_t guest, SDL_Event *host);
#define event_to_host rt_sdl_event_to_host
void rt_sdl_event_to_host(CPU *c, uint32_t guest, SDL_Event *host) {
    const uint8_t *in = rt_span(c, guest, GUEST_EVENT_SIZE);
    uint32_t type = get32(in, 0);
    memset(host, 0, sizeof(*host));
    if (plain_event(type)) memcpy(host, in, GUEST_EVENT_SIZE);
    else if (type >= SDL_USEREVENT && type <= SDL_LASTEVENT) {
        host->user.type = type; host->user.timestamp = get32(in, 4);
        host->user.windowID = get32(in, I386_USER_WINDOW); host->user.code = (Sint32)get32(in, I386_USER_CODE);
        host->user.data1 = (void *)(uintptr_t)get32(in, I386_USER_DATA1);
        host->user.data2 = (void *)(uintptr_t)get32(in, I386_USER_DATA2);
    } else if (type == SDL_SENSORUPDATE) {
        host->sensor.type = type; host->sensor.timestamp = get32(in, 4);
        host->sensor.which = (Sint32)get32(in, I386_SENSOR_WHICH);
        memcpy(host->sensor.data, in + I386_SENSOR_DATA, sizeof(host->sensor.data));
        memcpy(&host->sensor.timestamp_us, in + I386_SENSOR_TIMESTAMP_US, 8);
    } else if (type == SDL_CONTROLLERSENSORUPDATE) {
        host->csensor.type = type; host->csensor.timestamp = get32(in, 4);
        host->csensor.which = (Sint32)get32(in, I386_CSENSOR_WHICH);
        host->csensor.sensor = (Sint32)get32(in, I386_CSENSOR_SENSOR);
        memcpy(host->csensor.data, in + I386_CSENSOR_DATA, sizeof(host->csensor.data));
        memcpy(&host->csensor.timestamp_us, in + I386_CSENSOR_TIMESTAMP_US, 8);
    } else {
        // DROP*/TEXTEDITING_EXT/SYSWM poussés par l'invité porteraient un
        // pointeur invité que SDL rendrait ensuite comme chaîne hôte à libérer.
        char why[96];
        snprintf(why, sizeof(why), "guest SDL_PushEvent of type %#x is not supported", type);
        rt_trap(c, c->eip, why);
    }
}

// ---- Surveillance d'événements (SDL_AddEventWatch) -----------------------
// SDL appelle les rappels de façon synchrone dans le thread qui pousse ou
// pompe l'événement, donc pendant un shim SDL invité : on réutilise le CPU de
// ce shim (rt_shim_cpu). Un rappel sur un thread hôte sans shim en cours
// (thread interne SDL) n'a ni pile ni TLS invitée : il est piégé.
typedef struct Watch { uint32_t function, userdata; struct Watch *next; } Watch;
static Watch *watches;

static int watch_trampoline(void *opaque, SDL_Event *event) {
    Watch *w = opaque;
    if (!rt_sdl_event_is_sdl2(event->type)) return 1;   // inconnu de SDL2 : invisible pour l'invité
    CPU *c = rt_shim_cpu();
    if (!c) {
        fprintf(stderr, "SDL event watch %#x called on a host thread outside any guest SDL call\n", w->function);
        abort();
    }
    uint32_t saved_esp = c->esp;
    uint32_t buffer = (c->esp - 128) & ~15u;   // copie i386 sur la pile invitée
    c->esp = buffer;
    event_to_guest(c, event, buffer, 0);
    uint32_t strings[2] = {0, 0};
    if (string_event(event->type)) strings[0] = R32(buffer + I386_DROP_FILE);
    if (event->type == SDL_TEXTEDITING_EXT) strings[1] = R32(buffer + I386_EDITEXT_TEXT);
    uint32_t saved_eax = c->eax, saved_ecx = c->ecx, saved_edx = c->edx;
    uint32_t args[2] = {w->userdata, buffer};
    uint32_t result = rt_invoke(c, w->function, args, 2);
    c->eax = saved_eax; c->ecx = saved_ecx; c->edx = saved_edx;
    c->esp = saved_esp;
    // La chaîne reste la propriété de SDL pour un observateur : copie temporaire.
    rt_free(strings[0]); rt_free(strings[1]);
    return (int)result;
}

// ---- Handles comptés (joysticks, contrôleurs, haptique) ------------------
// SDL compte les ouvertures d'un même périphérique et rend le même pointeur ;
// le handle invité est libéré quand la dernière ouverture invitée est fermée.
typedef struct Ref { void *host; uint32_t kind, handle, refs; struct Ref *next; } Ref;
static Ref *refs;

static Ref *ref_find(uint32_t handle, uint32_t kind) {
    for (Ref *r = refs; r; r = r->next) if (r->handle == handle && r->kind == kind) return r;
    return NULL;
}
static uint32_t ref_acquire(CPU *c, void *host, uint32_t kind, int counted) {
    if (!host) return 0;
    for (Ref *r = refs; r; r = r->next)
        if (r->host == host && r->kind == kind) { r->refs += (uint32_t)counted; return r->handle; }
    Ref *r = calloc(1, sizeof(*r));
    uint32_t handle = r ? rt_handle_new(host, kind) : 0;
    if (!handle) { free(r); rt_trap(c, c->eip, "SDL input handle table exhausted"); }
    *r = (Ref){host, kind, handle, (uint32_t)counted, refs};
    refs = r;
    return handle;
}
// 0 → NULL (SDL signale l'erreur) ; handle inconnu → piège.
static void *ref_host(CPU *c, uint32_t handle, uint32_t kind) {
    if (!handle) return NULL;
    void *host = rt_handle_get(handle, kind);
    if (!host) rt_trap(c, c->eip, "invalid SDL input handle");
    return host;
}
static void ref_drop(Ref *r) {
    Ref **link = &refs;
    while (*link != r) link = &(*link)->next;
    *link = r->next;
    rt_handle_free(r->handle, r->kind);
    free(r);
}
static void ref_release(uint32_t handle, uint32_t kind) {
    Ref *r = ref_find(handle, kind);
    if (r && r->refs && !--r->refs) ref_drop(r);
}

// Noms de périphériques : copies invitées internées, jamais libérées (SDL
// garantit seulement la validité jusqu'au prochain appel ; une copie stable
// est plus sûre et bornée par le nombre de noms distincts).
typedef struct Name { char *text; uint32_t guest; struct Name *next; } Name;
static Name *names;
static uint32_t intern_name(CPU *c, const char *text) {
    if (!text) return 0;
    for (Name *n = names; n; n = n->next) if (!strcmp(n->text, text)) return n->guest;
    Name *n = calloc(1, sizeof(*n));
    char *copy = strdup(text);
    if (!n || !copy) { free(n); free(copy); rt_trap(c, c->eip, "SDL name table allocation failed"); }
    *n = (Name){copy, guest_string(c, text), names};
    names = n;
    return n->guest;
}

// Diagnostic (P2_INPUT_LOG=1) : boutons de souris, touchers, manettes et mode
// relatif tels que le jeu les reçoit, horodatés ; rien quand il est éteint.
static int input_log = -1;
int rt_input_logging(void);
#define input_logging rt_input_logging
int rt_input_logging(void) {
    if (input_log < 0) input_log = getenv("P2_INPUT_LOG") != NULL;
    return input_log;
}
static void log_input_event(const SDL_Event *e) {
    if (!input_logging()) return;
    unsigned t = SDL_GetTicks();
    switch (e->type) {
    case SDL_MOUSEBUTTONDOWN: case SDL_MOUSEBUTTONUP:
        fprintf(stderr, "[input] %u mouse %s button %d at %d,%d (which %u, relative %d)\n", t,
                e->type == SDL_MOUSEBUTTONDOWN ? "down" : "up", e->button.button, e->button.x, e->button.y,
                e->button.which, SDL_GetRelativeMouseMode());
        break;
    case SDL_MOUSEMOTION: {
        static unsigned last;
        if (t - last >= 100) fprintf(stderr, "[input] %u mouse motion to %d,%d (which %u)\n", t, e->motion.x, e->motion.y, e->motion.which);
        last = t;
        break;
    }
    case SDL_FINGERDOWN: case SDL_FINGERUP:
        fprintf(stderr, "[input] %u finger %s at %.3f,%.3f\n", t, e->type == SDL_FINGERDOWN ? "down" : "up",
                e->tfinger.x, e->tfinger.y);
        break;
    case SDL_CONTROLLERBUTTONDOWN: case SDL_CONTROLLERBUTTONUP:
        fprintf(stderr, "[input] %u controller %d button %d %s\n", t, e->cbutton.which, e->cbutton.button,
                e->type == SDL_CONTROLLERBUTTONDOWN ? "down" : "up");
        break;
    case SDL_CONTROLLERAXISMOTION:
        if (e->caxis.value > 16000 || e->caxis.value < -16000)
            fprintf(stderr, "[input] %u controller %d axis %d %d\n", t, e->caxis.which, e->caxis.axis, e->caxis.value);
        break;
    case SDL_KEYDOWN:
        fprintf(stderr, "[input] %u key %s\n", t, SDL_GetScancodeName(e->key.keysym.scancode));
        break;
    }
}

// Touchers iOS (souris SDL_TOUCH_MOUSEID) : SDL envoie le déplacement et le
// clic d'un toucher au même instant, mais les menus de Source ne savent quel
// élément est sous le curseur qu'à l'image suivante ; le premier toucher ne
// faisait que survoler. Les boutons d'un toucher sont donc retenus au moins
// TOUCH_CLICK_DELAY_MS après leur arrivée (ordre conservé), le déplacement
// passant tout de suite. Une vraie souris n'est jamais retardée.
static int is_touch_button(const SDL_Event *e) {
    return (e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP) && e->button.which == SDL_TOUCH_MOUSEID;
}
// Vrai si l'événement est retenu (rendu plus tard par touch_shaper_pop).
int rt_touch_shaper_hold(TouchShaper *t, const SDL_Event *e, uint32_t now) {
    if (!is_touch_button(e) || t->count == TOUCH_HELD_MAX) return 0;
    uint32_t release = now + TOUCH_CLICK_DELAY_MS;
    if (t->count && (int32_t)(t->items[t->count - 1].release - release) > 0) release = t->items[t->count - 1].release;
    t->items[t->count++] = (HeldEvent){*e, release};
    return 1;
}
int rt_touch_shaper_pop(TouchShaper *t, uint32_t now, SDL_Event *out) {
    if (!t->count || (int32_t)(now - t->items[0].release) < 0) return 0;
    *out = t->items[0].event;
    memmove(t->items, t->items + 1, --t->count * sizeof(*t->items));
    return 1;
}
static TouchShaper touch_shaper;

// Déplacement de souris synthétique (aller-retour d'un pixel, souris réelle et
// non tactile) demandé par runtime/ios/game_state.c à l'ouverture d'un menu.
static _Atomic int nudge_steps;
void rt_input_nudge_mouse(void) { nudge_steps = 2; }
static int nudge_event(SDL_Event *e) {
    int step = nudge_steps;
    if (step <= 0) return 0;
    nudge_steps = step - 1;
    int x = 0, y = 0;
    SDL_GetMouseState(&x, &y);
    SDL_Window *w = SDL_GetMouseFocus();
    memset(e, 0, sizeof(*e));
    e->type = SDL_MOUSEMOTION;
    e->motion.timestamp = SDL_GetTicks();
    e->motion.windowID = w ? SDL_GetWindowID(w) : 0;
    e->motion.which = 0;
    e->motion.x = x + (step == 2); e->motion.y = y;
    e->motion.xrel = step == 2 ? 1 : -1;
    return 1;
}

static void poll_into(CPU *c, uint32_t guest, int *got) {
    SDL_Event event;
    if (!guest) {
        // Simple test de présence : écarter d'abord les événements inconnus de SDL2.
        SDL_PumpEvents();
        while ((*got = SDL_PeepEvents(&event, 1, SDL_PEEKEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT)) == 1 &&
               !rt_sdl_event_is_sdl2(event.type))
            SDL_PeepEvents(&event, 1, SDL_GETEVENT, event.type, event.type);
        if (*got < 0) *got = 0;
        return;
    }
    // Diagnostic (fenêtre cachée seulement) : P2_FAKE_FOCUS=1 annonce une fois au
    // jeu que sa fenêtre a le focus, pour mesurer sans le sommeil de 50 ms que
    // le moteur s'impose sans focus. Sans effet en fenêtre visible.
    static int fake_focus = -1;
    if (fake_focus < 0) {
        const char *hidden = getenv("P2_HIDDEN_WINDOWS"), *wanted = getenv("P2_FAKE_FOCUS");
        fake_focus = hidden && *hidden && strcmp(hidden, "0") && wanted && *wanted && strcmp(wanted, "0");
    }
    // Fenêtre du rendu : contexte GL courant, ou vue Metal (Direct3D 9, sans contexte GL).
    SDL_Window *window = fake_focus != 1 ? NULL : SDL_GL_GetCurrentWindow();
    if (fake_focus == 1 && !window) window = rt_sdl_metal_window();
    if (window) {
        fake_focus = 2;
        memset(&event, 0, sizeof(event));
        event.type = SDL_WINDOWEVENT;
        event.window.timestamp = SDL_GetTicks();
        event.window.windowID = SDL_GetWindowID(window);
        event.window.event = SDL_WINDOWEVENT_FOCUS_GAINED;
        event_to_guest(c, &event, guest, 0);
        *got = 1;
        return;
    }
    uint32_t now = SDL_GetTicks();
    if (!(*got = nudge_event(&event)) && !(*got = rt_touch_shaper_pop(&touch_shaper, now, &event)))
        while ((*got = SDL_PollEvent(&event)) &&
               (!rt_sdl_event_is_sdl2(event.type) || rt_touch_shaper_hold(&touch_shaper, &event, now))) {}
    // F3 : compteur d'images hôte (fps_overlay.c). La touche reste transmise au
    // jeu, qui ne l'associe à rien par défaut.
    if (*got && event.type == SDL_KEYDOWN && !event.key.repeat && event.key.keysym.scancode == SDL_SCANCODE_F3)
        fps_overlay_toggle();
    if (*got) { log_input_event(&event); event_to_guest(c, &event, guest, 1); }
}

static int sdl_input_shim(CPU *c, const char *name) {
    if (strncmp(name, "SDL_", 4)) return 0;
    const char *n = name + 4;
    if (!strcmp(n, "PollEvent")) {
        uint32_t guest = argument(c, 0);
        if (guest) rt_write_span(c, guest, GUEST_EVENT_SIZE);
        int got; poll_into(c, guest, &got);
        c->eax = (uint32_t)got;
    } else if (!strcmp(n, "WaitEventTimeout")) {
        // Sondage + SDL_Delay(1) verrou invité relâché : les appels SDL restent
        // sérialisés par le verrou d'exécution, ce qui évite qu'un observateur
        // attende ce verrou pendant qu'un autre thread invité attend SDL.
        uint32_t guest = argument(c, 0);
        int32_t timeout = (int32_t)argument(c, 1);
        if (timeout && getenv("P2_TRACE_WAIT"))
            fprintf(stderr, "[wait] SDL_WaitEventTimeout(%#x, %d) from %#x\n", guest, timeout, R32(c->esp));
        if (guest) rt_write_span(c, guest, GUEST_EVENT_SIZE);
        uint64_t deadline = SDL_GetTicks64() + (uint64_t)(timeout > 0 ? timeout : 0);
        int got;
        for (;;) {
            poll_into(c, guest, &got);
            if (got || timeout == 0 || (timeout > 0 && SDL_GetTicks64() >= deadline)) break;
            unsigned saved = rt_guest_suspend();
            SDL_Delay(1);
            rt_guest_resume(saved);
        }
        c->eax = (uint32_t)got;
    } else if (!strcmp(n, "PushEvent")) {
        // SDL2 écrase l'horodatage de l'événement de l'appelant par
        // SDL_GetTicks() ; sdl2-compat conserve un horodatage non nul et ne
        // touche pas l'appelant (vérifié sur l'hôte) : 0 lui fait poser l'heure.
        uint32_t guest = argument(c, 0);
        SDL_Event event;
        event_to_host(c, guest, &event);
        W32(guest + 4, SDL_GetTicks());
        event.common.timestamp = 0;
        c->eax = (uint32_t)SDL_PushEvent(&event);
    } else if (!strcmp(n, "AddEventWatch")) {
        Watch *w = calloc(1, sizeof(*w));
        if (!w) rt_trap(c, c->eip, "SDL_AddEventWatch allocation failed");
        *w = (Watch){argument(c, 0), argument(c, 1), watches};
        watches = w;
        SDL_AddEventWatch(watch_trampoline, w);
    } else if (!strcmp(n, "DelEventWatch")) {
        uint32_t function = argument(c, 0), userdata = argument(c, 1);
        for (Watch **link = &watches; *link; link = &(*link)->next)
            if ((*link)->function == function && (*link)->userdata == userdata) {
                Watch *w = *link; *link = w->next;
                SDL_DelEventWatch(watch_trampoline, w);
                free(w);
                break;
            }
    } else if (!strcmp(n, "GetMouseState")) {
        uint32_t px = argument(c, 0), py = argument(c, 1);
        int x, y;
        c->eax = SDL_GetMouseState(&x, &y);
        if (input_logging()) {
            static int lx = -1, ly = -1; static uint32_t lb;
            if (x != lx || y != ly || c->eax != lb)
                fprintf(stderr, "[input] %u GetMouseState %d,%d buttons %u\n", SDL_GetTicks(), x, y, c->eax);
            lx = x; ly = y; lb = c->eax;
        }
        if (px) W32(px, (uint32_t)(x * rt_sdl_window_scale()));
        if (py) W32(py, (uint32_t)(y * rt_sdl_window_scale()));
    } else if (!strcmp(n, "SetRelativeMouseMode")) {
        c->eax = (uint32_t)SDL_SetRelativeMouseMode(argument(c, 0) ? SDL_TRUE : SDL_FALSE);
        static int last_relative = -1;
        if (input_logging() && (int)argument(c, 0) != last_relative)
            fprintf(stderr, "[input] %u relative mouse mode %u\n", SDL_GetTicks(), argument(c, 0));
        last_relative = (int)argument(c, 0);
    } else if (!strcmp(n, "SetModState")) {
        SDL_SetModState((SDL_Keymod)argument(c, 0));
    } else if (!strcmp(n, "NumJoysticks")) {
        c->eax = (uint32_t)SDL_NumJoysticks();
    } else if (!strcmp(n, "IsGameController")) {
        c->eax = SDL_IsGameController((int)argument(c, 0)) ? 1 : 0;
    } else if (!strcmp(n, "JoystickNameForIndex")) {
        c->eax = intern_name(c, SDL_JoystickNameForIndex((int)argument(c, 0)));
    } else if (!strcmp(n, "JoystickOpen")) {
        c->eax = ref_acquire(c, SDL_JoystickOpen((int)argument(c, 0)), KIND_JOYSTICK, 1);
    } else if (!strcmp(n, "JoystickClose")) {
        uint32_t handle = argument(c, 0);
        SDL_JoystickClose(ref_host(c, handle, KIND_JOYSTICK));
        ref_release(handle, KIND_JOYSTICK);
    } else if (!strcmp(n, "JoystickInstanceID")) {
        c->eax = (uint32_t)SDL_JoystickInstanceID(ref_host(c, argument(c, 0), KIND_JOYSTICK));
    } else if (!strcmp(n, "GameControllerOpen")) {
        c->eax = ref_acquire(c, SDL_GameControllerOpen((int)argument(c, 0)), KIND_CONTROLLER, 1);
    } else if (!strcmp(n, "GameControllerClose")) {
        uint32_t handle = argument(c, 0);
        SDL_GameController *controller = ref_host(c, handle, KIND_CONTROLLER);
        Ref *r = ref_find(handle, KIND_CONTROLLER);
        int last = r && r->refs == 1;
        SDL_Joystick *joystick = last ? SDL_GameControllerGetJoystick(controller) : NULL;
        SDL_GameControllerClose(controller);
        ref_release(handle, KIND_CONTROLLER);
        // Joystick seulement emprunté via ce contrôleur (jamais ouvert par l'invité).
        if (joystick) for (Ref *j = refs; j; j = j->next)
            if (j->kind == KIND_JOYSTICK && j->host == joystick && !j->refs) { ref_drop(j); break; }
    } else if (!strcmp(n, "GameControllerGetJoystick")) {
        SDL_Joystick *joystick = SDL_GameControllerGetJoystick(ref_host(c, argument(c, 0), KIND_CONTROLLER));
        c->eax = ref_acquire(c, joystick, KIND_JOYSTICK, 0);
    } else if (!strcmp(n, "JoystickGetDeviceGUID")) {
        // Retour de structure i386 : pointeur caché en premier argument, rendu
        // dans eax, et l'appelé dépile ce pointeur (`ret 4`). rt_return ne
        // dépilera que l'adresse de retour : on la remonte d'un mot.
        uint32_t result = argument(c, 0);
        SDL_JoystickGUID guid = SDL_JoystickGetDeviceGUID((int)argument(c, 1));
        memcpy(rt_write_span(c, result, sizeof(guid)), guid.data, sizeof(guid));
        uint32_t return_address = R32(c->esp);
        W32(c->esp + 4, return_address);
        c->esp += 4;
        c->eax = result;
    } else if (!strcmp(n, "JoystickGetGUIDString")) {
        // (SDL_JoystickGUID par valeur sur 16 octets, char *psz, int cbGUID)
        SDL_JoystickGUID guid;
        memcpy(guid.data, rt_span(c, c->esp + 4, 16), 16);
        uint32_t out = argument(c, 4);
        int32_t size = (int32_t)argument(c, 5);
        if (out && size > 0) {
            char text[64];
            SDL_JoystickGetGUIDString(guid, text, size < (int32_t)sizeof(text) ? size : (int)sizeof(text));
            size_t length = strlen(text) + 1;
            memcpy(rt_write_span(c, out, length), text, length);
        } else SDL_JoystickGetGUIDString(guid, NULL, 0);   // même erreur SDL
    } else if (!strcmp(n, "HapticOpenFromJoystick")) {
        c->eax = ref_acquire(c, SDL_HapticOpenFromJoystick(ref_host(c, argument(c, 0), KIND_JOYSTICK)), KIND_HAPTIC, 1);
    } else if (!strcmp(n, "HapticClose")) {
        uint32_t handle = argument(c, 0);
        SDL_HapticClose(ref_host(c, handle, KIND_HAPTIC));
        ref_release(handle, KIND_HAPTIC);
    } else if (!strcmp(n, "HapticRumbleInit")) {
        c->eax = (uint32_t)SDL_HapticRumbleInit(ref_host(c, argument(c, 0), KIND_HAPTIC));
    } else if (!strcmp(n, "HapticRumblePlay")) {
        float strength = RF(c->esp + 8);
        c->eax = (uint32_t)SDL_HapticRumblePlay(ref_host(c, argument(c, 0), KIND_HAPTIC), strength, argument(c, 2));
    } else return 0;
    return 1;
}
RT_SHIM_PROVIDER(sdl_input_shim)
