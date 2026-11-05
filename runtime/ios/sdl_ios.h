// SDL sur iOS : le jeu doit voir l'écran en pixels réels, pas en points, sinon
// il rend en 874×402 sur un écran 2622×1206 (image pixellisée). Ces macros,
// incluses par les ponts sdl_video.c (via host_gl.h) et sdl_input.c,
// remplacent les appels SDL qui exposent des tailles ou des coordonnées :
// fenêtre haute résolution, tailles et modes multipliés par l'échelle,
// coordonnées de souris et événements de fenêtre convertis.
// Simulateur : rendu en points par défaut (P2_IOS_RENDER_SCALE=native pour
// l'écran entier) ; appareil : échelle 2 par défaut (moitié de la résolution
// native sur un écran ×3 ; P2_IOS_RENDER_SCALE=1 pour les points, =native pour
// les pixels réels).
#pragma once
#include <SDL.h>

float rt_ios_render_scale(void);
// Donne à une UIView (vue GL de SDL, SDL_MetalView) l'échelle de rendu voulue,
// si elle diffère, et fait recalculer sa disposition (SDL recrée alors ses
// tampons à cette taille). Thread principal.
void rt_ios_view_set_scale(void *uiview, float wanted);
SDL_Window *rt_ios_create_window(const char *title, int x, int y, int w, int h, Uint32 flags);
void rt_ios_get_window_size(SDL_Window *window, int *w, int *h);
void rt_ios_set_window_size(SDL_Window *window, int w, int h);
int rt_ios_set_window_display_mode(SDL_Window *window, const SDL_DisplayMode *mode);
int rt_ios_get_window_display_mode(SDL_Window *window, SDL_DisplayMode *mode);
int rt_ios_get_current_display_mode(int display, SDL_DisplayMode *mode);
int rt_ios_get_desktop_display_mode(int display, SDL_DisplayMode *mode);
int rt_ios_get_display_bounds(int display, SDL_Rect *rect);
Uint32 rt_ios_get_mouse_state(int *x, int *y);
void rt_ios_warp_mouse_in_window(SDL_Window *window, int x, int y);
int rt_ios_poll_event(SDL_Event *event);

#define SDL_CreateWindow rt_ios_create_window
#define SDL_GetWindowSize rt_ios_get_window_size
#define SDL_SetWindowSize rt_ios_set_window_size
#define SDL_SetWindowDisplayMode rt_ios_set_window_display_mode
#define SDL_GetWindowDisplayMode rt_ios_get_window_display_mode
#define SDL_GetCurrentDisplayMode rt_ios_get_current_display_mode
#define SDL_GetDesktopDisplayMode rt_ios_get_desktop_display_mode
#define SDL_GetDisplayBounds rt_ios_get_display_bounds
#define SDL_GetMouseState rt_ios_get_mouse_state
#define SDL_WarpMouseInWindow rt_ios_warp_mouse_in_window
#define SDL_PollEvent rt_ios_poll_event
