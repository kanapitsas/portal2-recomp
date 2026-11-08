#pragma once
// Compteur d'images F3 (runtime/fps_overlay.c), commun à OpenGL et Direct3D 9 ;
// sur iOS, la surcouche tactile (runtime/ios/touch_controls.m) lit la même
// mesure (fps_overlay_value) et dessine avec la même police (fps_overlay_text).
typedef struct { int x, y, w, h; } FpsRect;   // pixels, origine en haut à gauche
enum { FPS_OVERLAY_MAX_RECTS = 512 };
void fps_overlay_toggle(void);
// Cadence mesurée (images par seconde sur la dernière demi-seconde), mise à
// jour par fps_overlay_tick ; lisible d'un autre thread.
double fps_overlay_value(void);
// Un texte en rectangles : origine (x, y), scale pixels par pixel de glyphe
// (police 5×7, avance de 6) ; une rangée de pixels allumés contigus = un
// rectangle. Chiffres, « . / », lettres (sans distinction de casse) ; rend
// le nombre de rectangles (écrits jusqu'à max).
int fps_overlay_text(const char *text, int x, int y, int scale, FpsRect *out, int max);
int fps_overlay_layout(const char *line1, const char *line2, int drawable_height, FpsRect *out, int max);
int fps_overlay_tick(void);   // compte une image ; vrai si le compteur est affiché
int fps_overlay_text_rects(const char *renderer, int drawable_height, FpsRect *out, int max);
void fps_overlay_frame(int drawable_width, int drawable_height);
