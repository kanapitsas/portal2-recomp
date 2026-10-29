#pragma once
// Compteur d'images F3 (runtime/fps_overlay.c), commun à OpenGL et Direct3D 9.
typedef struct { int x, y, w, h; } FpsRect;   // pixels, origine en haut à gauche
enum { FPS_OVERLAY_MAX_RECTS = 512 };
void fps_overlay_toggle(void);
double fps_overlay_value(void);
int fps_overlay_layout(const char *line1, const char *line2, int drawable_height, FpsRect *out, int max);
int fps_overlay_tick(void);   // compte une image ; vrai si le compteur est affiché
int fps_overlay_text_rects(const char *renderer, int drawable_height, FpsRect *out, int max);
void fps_overlay_frame(int drawable_width, int drawable_height);
