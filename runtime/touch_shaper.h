#pragma once
#include <SDL2/SDL.h>
#include <stdint.h>
// Clics des touchers iOS retenus derrière leur déplacement (sdl_input.c).
enum { TOUCH_CLICK_DELAY_MS = 40, TOUCH_HELD_MAX = 16 };
typedef struct { SDL_Event event; uint32_t release; } HeldEvent;
typedef struct { HeldEvent items[TOUCH_HELD_MAX]; unsigned count; } TouchShaper;
int rt_touch_shaper_hold(TouchShaper *t, const SDL_Event *e, uint32_t now);
int rt_touch_shaper_pop(TouchShaper *t, uint32_t now, SDL_Event *out);
