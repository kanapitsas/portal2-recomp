// Types partagés par les ponts SDL2 (vidéo, entrées, audio).
#pragma once
// Identifiants des handles rt_handle_* pour les pointeurs SDL hôtes.
// 'SDLW' est aussi utilisé par le pont des entrées pour retrouver une fenêtre
// (rt_handle_find(fenetre_hote, RT_SDL_WINDOW)) : ne pas le changer.
#define RT_SDL_WINDOW    0x53444c57u  /* 'SDLW' */
#define RT_SDL_GLCONTEXT 0x53444c47u  /* 'SDLG' */
#define RT_SDL_CURSOR    0x53444c43u  /* 'SDLC' */
#define RT_SDL_RWOPS     0x53444c52u  /* 'SDLR' */
// Tailles des structures SDL2 i386 lues/écrites en mémoire invitée.
#define RT_SDL_RECT_SIZE 16u         /* x, y, w, h */
#define RT_SDL_DISPLAYMODE_SIZE 20u  /* format, w, h, refresh_rate, driverdata */
#define RT_SDL_SURFACE_SIZE 60u
#define RT_SDL_PIXELFORMAT_SIZE 44u
// Surface hôte associée à une SDL_Surface invitée créée par le pont vidéo,
// après recopie des pixels/couleurs modifiés par le code invité (rt_trap si
// la surface est inconnue ou si sa géométrie a changé). NULL pour 0.
#include <stdint.h>
struct CPU; struct SDL_Surface;
struct SDL_Surface *rt_sdl_surface_sync(struct CPU *c, uint32_t guest);
// Retina (P2_RETINA=0 pour désactiver) : pixels par point de l'écran, et de
// la fenêtre du jeu (1 si la résolution choisie tient à l'écran en points).
int rt_sdl_display_scale(void);
int rt_sdl_window_scale(void);
void rt_sdl_force_scales(int display, int window);
// Encoche : hauteur d'écran annoncée au jeu (celle que reçoit une fenêtre plein écran).
int rt_sdl_notch_height(int height, int frame_h, int visible_top, double safe_top);
// Fenêtres Metal (rendu Direct3D 9 natif) : drapeaux SDL invités → hôtes
// (SDL_WINDOW_VULKAN devient SDL_WINDOW_METAL), et CAMetalLayer* d'une
// SDL_Window* hôte avec la taille de rendu en pixels (NULL si non Metal).
uint32_t rt_sdl_window_flags(uint32_t guest_flags);
void *rt_sdl_metal_layer(void *window, uint32_t *width, uint32_t *height);
void *rt_sdl_metal_window(void);           // fenêtre Metal créée en dernier, ou NULL
// Modes de l'écran 0 tels que le jeu les voit par SDL_GetDesktopDisplayMode
// (pixels, hauteur sous l'encoche) : {largeur, hauteur, Hz}, celui du bureau
// d'abord, puis les modes SDL plus petits sans doublon. Démarre la vidéo SDL
// le temps de la lecture si elle ne l'est pas (thread principal seulement).
// Rend le nombre de modes écrits (0 si indisponible).
unsigned rt_sdl_game_modes(uint32_t (*modes)[3], unsigned capacity);

