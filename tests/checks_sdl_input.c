// Pont SDL2 d'entrée : traduction i386 de SDL_Event, rappels invités,
// retour de structure par valeur (`ret 4`) et handles de manettes, via des
// événements synthétiques et un joystick virtuel SDL (aucun matériel requis).
#include "touch_shaper.h"
#include "checks.h"
#include "sdl_common.h"
#include <SDL2/SDL.h>
#include <stdlib.h>

void rt_sdl_event_to_guest(CPU *c, const SDL_Event *host, uint32_t guest, int release_host);
void rt_sdl_event_to_host(CPU *c, uint32_t guest, SDL_Event *host);

#define STACK 0x20010000u
#define EVENT 0x20012000u
#define SCRATCH 0x20013000u

static uint32_t call(CPU *c, const char *name, const uint32_t *args, unsigned count) {
    c->esp = STACK;
    for (unsigned i = count; i; --i) PUSH(c, args[i - 1]);
    uint32_t base = c->esp;
    rt_call(c, rt_dynamic_import(name), RT_RETURN_SENTINEL);
    assert(c->esp == base);   // cdecl : l'appelant dépile
    return c->eax;
}

// Fonction « invitée » d'observation : atteinte par rt_invoke → rt_dispatch.
static uint8_t seen[56];
static uint32_t seen_userdata, seen_calls;
static int fake_watch(CPU *c, const char *name) {
    if (strcmp(name, "test_sdl_watch")) return 0;
    seen_userdata = R32(c->esp + 4);
    uint32_t event = R32(c->esp + 8);
    memcpy(seen, GPTR(event), 56);
    ++seen_calls;
    c->eax = 1;
    return 1;
}
RT_SHIM_PROVIDER(fake_watch)

// Simule un autre pont (ex. sdl_video) dont l'appel SDL hôte déclenche un
// événement : l'observateur invité doit s'exécuter sur le CPU de ce shim.
static int fake_host_push(CPU *c, const char *name) {
    if (strcmp(name, "test_sdl_host_push")) return 0;
    SDL_Event host = {0};
    host.user.type = SDL_USEREVENT; host.user.code = 11; host.user.data1 = (void *)(uintptr_t)0x2000cafe;
    c->eax = (uint32_t)SDL_PushEvent(&host);
    return 1;
}
RT_SHIM_PROVIDER(fake_host_push)

static void flush(void) { SDL_PumpEvents(); SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT); }
// SDL ≥ 2.24 termine une boucle `while (SDL_PollEvent)` une fois par pompage
// (SDL_POLLSENTINEL) : un 0 isolé est légitime, on relance une fois.
static uint32_t poll_guest(CPU *c) {
    uint32_t a[] = {EVENT};
    uint32_t r = call(c, "SDL_PollEvent", a, 1);
    return r ? r : call(c, "SDL_PollEvent", a, 1);
}

RT_CHECK(sdl_input_events_translate_to_i386) {
    assert(!SDL_InitSubSystem(SDL_INIT_EVENTS));
    CPU cpu, *c = &cpu; rt_cpu_reset(c);
    flush();

    // SDL_USEREVENT poussé par l'invité : data1/data2 (adresses invitées) aux
    // décalages i386 16/20, conservés à l'aller-retour ; l'observateur voit
    // la même copie i386 avec son userdata.
    uint32_t watch[] = {rt_dynamic_import("test_sdl_watch"), 0x2000beef};
    call(c, "SDL_AddEventWatch", watch, 2);
    uint8_t *ev = GPTR(EVENT);
    memset(ev, 0, 56);
    uint32_t user = SDL_USEREVENT + 3;
    memcpy(ev, &user, 4);
    W32(EVENT + 8, 7); W32(EVENT + 12, (uint32_t)-42); W32(EVENT + 16, 0x2000abcd); W32(EVENT + 20, 0x80001234);
    uint32_t pa[] = {EVENT};
    assert(call(c, "SDL_PushEvent", pa, 1) == 1);
    assert(seen_calls == 1 && seen_userdata == 0x2000beef);
    uint32_t v; memcpy(&v, seen + 16, 4); assert(v == 0x2000abcd);
    memcpy(&v, seen + 20, 4); assert(v == 0x80001234);
    memset(ev, 0xcc, 56);
    assert(poll_guest(c) == 1);
    assert(R32(EVENT) == user && R32(EVENT + 8) == 7 && R32(EVENT + 12) == (uint32_t)-42);
    assert(R32(EVENT + 16) == 0x2000abcd && R32(EVENT + 20) == 0x80001234 && R32(EVENT + 24) == 0);

    // Après SDL_DelEventWatch, l'observateur n'est plus appelé.
    call(c, "SDL_DelEventWatch", watch, 2);
    memcpy(ev, &user, 4);
    W32(EVENT + 4, 1);   // horodatage périmé : SDL2 le remplace par SDL_GetTicks()
    SDL_Delay(2);
    assert(call(c, "SDL_PushEvent", pa, 1) == 1);
    assert(seen_calls == 1 && R32(EVENT + 4) > 1);
    assert(poll_guest(c) == 1 && R32(EVENT) == user && R32(EVENT + 4) > 1);
    uint32_t none[] = {0};
    assert(call(c, "SDL_PollEvent", none, 1) == 0 && call(c, "SDL_PollEvent", none, 1) == 0);   // NULL, file vide

    // Événement levé par un autre pont pendant son propre shim : l'observateur
    // invité s'exécute sur le CPU de ce shim (rt_shim_cpu) et sa pile.
    call(c, "SDL_AddEventWatch", watch, 2);
    seen_calls = 0;
    assert(call(c, "test_sdl_host_push", none, 0) == 1);
    memcpy(&v, seen + 16, 4);
    assert(seen_calls == 1 && v == 0x2000cafe && seen[12] == 11);
    call(c, "SDL_DelEventWatch", watch, 2);
    assert(poll_guest(c) == 1 && R32(EVENT + 16) == 0x2000cafe);

    // SDL_DROPFILE (sdl2-compat plante si l'application en pousse un : on
    // traduit directement) : chaîne recopiée par rt_alloc, pointeur à +8,
    // windowID à +12, chaîne hôte libérée comme après SDL_PollEvent.
    SDL_Event host = {0};
    host.drop.type = SDL_DROPFILE; host.drop.file = SDL_strdup("/tmp/chell.dem"); host.drop.windowID = 5;
    rt_sdl_event_to_guest(c, &host, EVENT, 1);
    assert(R32(EVENT) == SDL_DROPFILE && R32(EVENT + 12) == 5);
    uint32_t file = R32(EVENT + 8);
    assert(file >= 0x80000000u && !strcmp((const char *)GPTR(file), "/tmp/chell.dem"));
    assert(!rt_free(file));   // ce que fera SDL_free côté invité
    host = (SDL_Event){0}; host.drop.type = SDL_DROPBEGIN; host.drop.windowID = 3;
    rt_sdl_event_to_guest(c, &host, EVENT, 1);
    assert(R32(EVENT) == SDL_DROPBEGIN && R32(EVENT + 8) == 0 && R32(EVENT + 12) == 3);

    // SDL_CONTROLLERSENSORUPDATE : timestamp_us (Uint64) à +28 en i386, +32 en
    // ARM64. sdl2-compat perd `which` et timestamp_us d'un événement capteur
    // poussé par l'application : traduction testée directement.
    host = (SDL_Event){0};
    host.csensor.type = SDL_CONTROLLERSENSORUPDATE; host.csensor.timestamp = 77; host.csensor.which = 3;
    host.csensor.sensor = 2; host.csensor.data[0] = 1.5f; host.csensor.data[2] = -2.0f;
    host.csensor.timestamp_us = 0x1122334455667788ull;
    memset(ev, 0xcc, 56);
    rt_sdl_event_to_guest(c, &host, EVENT, 1);
    assert(R32(EVENT) == SDL_CONTROLLERSENSORUPDATE && R32(EVENT + 4) == 77);
    assert(R32(EVENT + 8) == 3 && R32(EVENT + 12) == 2 && RF(EVENT + 16) == 1.5f && RF(EVENT + 24) == -2.0f);
    assert(R64(EVENT + 28) == 0x1122334455667788ull && R32(EVENT + 36) == 0);
    SDL_Event back;
    rt_sdl_event_to_host(c, EVENT, &back);
    assert(!memcmp(&back.csensor, &host.csensor, sizeof(back.csensor)));
    // SDL_SENSORUPDATE : data[6] à +12, timestamp_us à +36 en i386 (+40 en ARM64).
    memset(ev, 0, 56);
    W32(EVENT, SDL_SENSORUPDATE); W32(EVENT + 8, 9); WF(EVENT + 12, 0.25f); WF(EVENT + 32, 4.0f);
    W64(EVENT + 36, 0x0102030405060708ull);
    rt_sdl_event_to_host(c, EVENT, &back);
    assert(back.sensor.which == 9 && back.sensor.data[0] == 0.25f && back.sensor.data[5] == 4.0f);
    assert(back.sensor.timestamp_us == 0x0102030405060708ull);
    memset(ev, 0xcc, 56);
    rt_sdl_event_to_guest(c, &back, EVENT, 1);
    assert(R32(EVENT + 8) == 9 && RF(EVENT + 32) == 4.0f && R64(EVENT + 36) == 0x0102030405060708ull && R32(EVENT + 44) == 0);
    // Par la vraie file SDL, les données du capteur survivent.
    assert(call(c, "SDL_PushEvent", pa, 1) == 1);
    memset(ev, 0, 56);
    assert(poll_guest(c) == 1 && R32(EVENT) == SDL_SENSORUPDATE && RF(EVENT + 12) == 0.25f && RF(EVENT + 32) == 4.0f);

    // SDL_KEYDOWN : disposition identique, SDL_Keysym à +16 (scancode, sym, mod à +24).
    host = (SDL_Event){0};
    host.key.type = SDL_KEYDOWN; host.key.windowID = 2; host.key.state = SDL_PRESSED; host.key.repeat = 1;
    host.key.keysym.scancode = SDL_SCANCODE_E; host.key.keysym.sym = SDLK_e; host.key.keysym.mod = KMOD_LSHIFT;
    assert(SDL_PushEvent(&host) == 1);
    assert(poll_guest(c) == 1 && R32(EVENT) == SDL_KEYDOWN && R32(EVENT + 8) == 2);
    assert(R8(EVENT + 12) == SDL_PRESSED && R8(EVENT + 13) == 1);
    assert(R32(EVENT + 16) == SDL_SCANCODE_E && R32(EVENT + 20) == SDLK_e && R16(EVENT + 24) == KMOD_LSHIFT);

    // SDL_WaitEventTimeout : délai écoulé sans événement → 0 ; événement → 1.
    flush();
    uint32_t wa[] = {EVENT, 20};
    uint64_t start = SDL_GetTicks64();
    assert(call(c, "SDL_WaitEventTimeout", wa, 2) == 0 && SDL_GetTicks64() - start >= 20);
    host = (SDL_Event){0}; host.type = SDL_QUIT;
    SDL_PushEvent(&host);
    assert(call(c, "SDL_WaitEventTimeout", wa, 2) == 1 && R32(EVENT) == SDL_QUIT);

    // SDL_SetModState / SDL_GetMouseState (pointeurs invités facultatifs).
    uint32_t mod[] = {KMOD_NUM | KMOD_RCTRL};
    call(c, "SDL_SetModState", mod, 1);
    assert(SDL_GetModState() == (KMOD_NUM | KMOD_RCTRL));
    mod[0] = KMOD_NONE; call(c, "SDL_SetModState", mod, 1);
    W32(SCRATCH, 0xdead); W32(SCRATCH + 4, 0xdead);
    int hx, hy; uint32_t buttons = SDL_GetMouseState(&hx, &hy);
    uint32_t ma[] = {SCRATCH, SCRATCH + 4};
    assert(call(c, "SDL_GetMouseState", ma, 2) == buttons);
    assert(R32(SCRATCH) == (uint32_t)hx && R32(SCRATCH + 4) == (uint32_t)hy);
    uint32_t mn[] = {0, 0};
    assert(call(c, "SDL_GetMouseState", mn, 2) == buttons);
    flush();
    SDL_QuitSubSystem(SDL_INIT_EVENTS);
}

RT_CHECK(sdl_input_joysticks_and_struct_return) {
    assert(!SDL_InitSubSystem(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC));
    CPU cpu, *c = &cpu; rt_cpu_reset(c);
    int index = SDL_JoystickAttachVirtual(SDL_JOYSTICK_TYPE_GAMECONTROLLER, 6, 15, 1);
    assert(index >= 0);
    uint32_t none[] = {0};
    assert((int)call(c, "SDL_NumJoysticks", none, 0) == SDL_NumJoysticks());
    uint32_t ia[] = {(uint32_t)index};
    assert(call(c, "SDL_IsGameController", ia, 1) == (SDL_IsGameController(index) ? 1u : 0u));
    uint32_t name = call(c, "SDL_JoystickNameForIndex", ia, 1);
    assert(name && !strcmp((const char *)GPTR(name), SDL_JoystickNameForIndex(index)));
    assert(call(c, "SDL_JoystickNameForIndex", ia, 1) == name);   // copie internée
    uint32_t bad[] = {999};
    assert(call(c, "SDL_JoystickNameForIndex", bad, 1) == 0 && call(c, "SDL_JoystickOpen", bad, 1) == 0);

    // Retour de SDL_JoystickGUID par valeur : pointeur caché, eax = pointeur,
    // l'appelé dépile 4 octets (comme le site d'appel d'inputsystem.so).
    c->esp = STACK;
    PUSH(c, (uint32_t)index); PUSH(c, SCRATCH + 1);   // adresse impaire comme sur la pile réelle
    uint32_t before = c->esp;
    rt_call(c, rt_dynamic_import("SDL_JoystickGetDeviceGUID"), RT_RETURN_SENTINEL);
    assert(c->esp == before + 4 && c->eax == SCRATCH + 1);
    SDL_JoystickGUID expected = SDL_JoystickGetDeviceGUID(index);
    assert(!memcmp(GPTR(SCRATCH + 1), expected.data, 16));
    // SDL_JoystickGetGUIDString(guid par valeur, psz, cb) : 32 chiffres hexa.
    c->esp = STACK;
    PUSH(c, 33); PUSH(c, SCRATCH + 64);
    c->esp -= 16; memcpy(GPTR(c->esp), expected.data, 16);
    before = c->esp;
    rt_call(c, rt_dynamic_import("SDL_JoystickGetGUIDString"), RT_RETURN_SENTINEL);
    assert(c->esp == before);
    char text[33]; SDL_JoystickGetGUIDString(expected, text, sizeof(text));
    assert(strlen(text) == 32 && !strcmp((const char *)GPTR(SCRATCH + 64), text));
    c->esp = STACK;   // tampon court : troncature SDL avec terminateur
    PUSH(c, 5); PUSH(c, SCRATCH + 128); c->esp -= 16; memcpy(GPTR(c->esp), expected.data, 16);
    W32(SCRATCH + 132, 0x41414141);
    rt_call(c, rt_dynamic_import("SDL_JoystickGetGUIDString"), RT_RETURN_SENTINEL);
    assert(!strncmp((const char *)GPTR(SCRATCH + 128), text, 4) && R8(SCRATCH + 132) == 0 && R8(SCRATCH + 133) == 0x41);

    // Handles comptés : deux ouvertures → même handle, vivant jusqu'à la
    // dernière fermeture ; le joystick d'un contrôleur est ce même handle.
    uint32_t j1 = call(c, "SDL_JoystickOpen", ia, 1), j2 = call(c, "SDL_JoystickOpen", ia, 1);
    assert(j1 >= RT_HANDLE_BASE && j1 == j2);
    SDL_Joystick *hj = rt_handle_get(j1, 'SDLJ');
    assert(hj);
    uint32_t ja[] = {j1};
    assert((int32_t)call(c, "SDL_JoystickInstanceID", ja, 1) == SDL_JoystickInstanceID(hj));
    assert((int32_t)call(c, "SDL_JoystickInstanceID", none, 1) == -1);   // NULL : erreur SDL
    uint32_t k = call(c, "SDL_GameControllerOpen", ia, 1);
    assert(k >= RT_HANDLE_BASE && rt_handle_get(k, 'SDLK'));
    uint32_t ka[] = {k};
    assert(call(c, "SDL_GameControllerGetJoystick", ka, 1) == j1);
    // Pas de moteur de vibration sur un joystick virtuel : NULL comme SDL.
    uint32_t h = call(c, "SDL_HapticOpenFromJoystick", ja, 1);
    if (h) {
        uint32_t ha[] = {h, 0, 100};
        float strength = 0.5f; memcpy(&ha[1], &strength, 4);
        (void)call(c, "SDL_HapticRumbleInit", ha, 1);
        (void)call(c, "SDL_HapticRumblePlay", ha, 3);
        call(c, "SDL_HapticClose", ha, 1);
        assert(!rt_handle_get(h, 'SDLH'));
    }
    call(c, "SDL_GameControllerClose", ka, 1);
    assert(!rt_handle_get(k, 'SDLK') && rt_handle_get(j1, 'SDLJ') == hj);
    call(c, "SDL_JoystickClose", ja, 1);
    assert(rt_handle_get(j1, 'SDLJ') == hj);
    call(c, "SDL_JoystickClose", ja, 1);
    assert(!rt_handle_get(j1, 'SDLJ'));

    // Contrôleur seul : son joystick emprunté disparaît avec lui.
    k = call(c, "SDL_GameControllerOpen", ia, 1); ka[0] = k;
    uint32_t borrowed = call(c, "SDL_GameControllerGetJoystick", ka, 1);
    assert(borrowed && rt_handle_get(borrowed, 'SDLJ'));
    call(c, "SDL_GameControllerClose", ka, 1);
    assert(!rt_handle_get(borrowed, 'SDLJ') && !rt_handle_get(k, 'SDLK'));

    SDL_JoystickDetachVirtual(index);
    SDL_QuitSubSystem(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC);
}

int rt_sdl_event_is_sdl2(uint32_t type);
RT_CHECK(sdl_input_hides_sdl3_only_events) {
    assert(rt_sdl_event_is_sdl2(SDL_WINDOWEVENT) && rt_sdl_event_is_sdl2(SDL_KEYDOWN));
    assert(rt_sdl_event_is_sdl2(SDL_USEREVENT + 7) && rt_sdl_event_is_sdl2(SDL_DROPFILE));
    assert(!rt_sdl_event_is_sdl2(0x205) && !rt_sdl_event_is_sdl2(0x202));  // événements fenêtre SDL3
}

// Retina : positions dans la fenêtre et tailles de fenêtre en pixels (×2),
// déplacements relatifs inchangés (même sensibilité de visée).
RT_CHECK(sdl_input_retina_scales_absolute_coordinates) {
    CPU cpu, *c = &cpu; rt_cpu_reset(c);
    rt_sdl_force_scales(2, 2);
    SDL_Event host = {0};
    host.motion.type = SDL_MOUSEMOTION; host.motion.x = 10; host.motion.y = 20; host.motion.xrel = 3; host.motion.yrel = -4;
    rt_sdl_event_to_guest(c, &host, EVENT, 0);
    assert(R32(EVENT + 20) == 20 && R32(EVENT + 24) == 40 && R32(EVENT + 28) == 3 && R32(EVENT + 32) == (uint32_t)-4);
    memset(&host, 0, sizeof(host));
    host.button.type = SDL_MOUSEBUTTONDOWN; host.button.button = 1; host.button.x = 7; host.button.y = 9;
    rt_sdl_event_to_guest(c, &host, EVENT, 0);
    assert(((uint8_t *)GPTR(EVENT))[16] == 1 && R32(EVENT + 20) == 14 && R32(EVENT + 24) == 18);
    memset(&host, 0, sizeof(host));
    host.window.type = SDL_WINDOWEVENT; host.window.event = SDL_WINDOWEVENT_SIZE_CHANGED;
    host.window.data1 = 1280; host.window.data2 = 720;
    rt_sdl_event_to_guest(c, &host, EVENT, 0);
    assert(R32(EVENT + 16) == 2560 && R32(EVENT + 20) == 1440);
    host.window.event = SDL_WINDOWEVENT_MOVED; host.window.data1 = 100; host.window.data2 = 50;
    rt_sdl_force_scales(2, 1);   // fenêtre à l'échelle 1 : position à l'écran toujours en pixels
    rt_sdl_event_to_guest(c, &host, EVENT, 0);
    assert(R32(EVENT + 16) == 200 && R32(EVENT + 20) == 100);
    host.window.event = SDL_WINDOWEVENT_FOCUS_GAINED; host.window.data1 = 5;
    rt_sdl_event_to_guest(c, &host, EVENT, 0);
    assert(R32(EVENT + 16) == 5);
    rt_sdl_force_scales(0, 0);
    memset(&host, 0, sizeof(host));
    host.motion.type = SDL_MOUSEMOTION; host.motion.x = 10;
    rt_sdl_event_to_guest(c, &host, EVENT, 0);
    assert(R32(EVENT + 20) == 10);
}

// Toucher iOS : le déplacement passe, le clic est retenu une image (le menu
// sait alors ce qui est sous le doigt), l'ordre bas/haut est gardé ; une vraie
// souris n'est jamais retenue.
RT_CHECK(sdl_input_touch_clicks_wait_one_frame) {
    TouchShaper t = {0};
    SDL_Event motion = {.type = SDL_MOUSEMOTION}, down = {.type = SDL_MOUSEBUTTONDOWN}, up = {.type = SDL_MOUSEBUTTONUP}, out;
    motion.motion.which = down.button.which = up.button.which = SDL_TOUCH_MOUSEID;
    assert(!rt_touch_shaper_hold(&t, &motion, 1000));
    assert(rt_touch_shaper_hold(&t, &down, 1000) && rt_touch_shaper_hold(&t, &up, 1010));
    assert(!rt_touch_shaper_pop(&t, 1000 + TOUCH_CLICK_DELAY_MS - 1, &out));
    assert(rt_touch_shaper_pop(&t, 1000 + TOUCH_CLICK_DELAY_MS, &out) && out.type == SDL_MOUSEBUTTONDOWN);
    assert(!rt_touch_shaper_pop(&t, 1000 + TOUCH_CLICK_DELAY_MS, &out) || out.type == SDL_MOUSEBUTTONUP);
    assert(rt_touch_shaper_pop(&t, 1010 + TOUCH_CLICK_DELAY_MS, &out) || t.count == 0);
    assert(t.count == 0);
    SDL_Event mouse = {.type = SDL_MOUSEBUTTONDOWN};
    mouse.button.which = 0;
    assert(!rt_touch_shaper_hold(&t, &mouse, 2000));
}
