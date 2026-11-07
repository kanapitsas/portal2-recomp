// Réglages iOS (app Réglages) traduits en variables P2_* : runtime/ios/settings.c.
#pragma once
#include <stddef.h>

typedef struct { const char *key, *env, *on, *off; } rt_ios_setting;
extern const rt_ios_setting rt_ios_settings[];
extern const int rt_ios_settings_count;

// Lit la valeur d'un réglage (« 1 »/« 0 » pour un interrupteur) ; 0 si absent.
typedef int (*rt_ios_settings_get)(void *ctx, const char *key, char *out, size_t size);
// Pose les variables des réglages présents, sauf celles déjà définies ; rend
// leur nombre et, dans summary, la liste « VAR=valeur, ... ».
int rt_ios_settings_apply(rt_ios_settings_get get, void *ctx, char *summary, size_t size);
// Sur iOS : depuis les préférences de l'app.
int rt_ios_settings_load(char *summary, size_t size);
// Taille de rendu (pixels, paysage) : écran en points × échelle de rendu (sdl_ios.m).
void rt_ios_render_size(int *w, int *h);
// Clés MTLD3D_CONFIG propres à iOS : present.maxFps, et render.scale si
// upscale (P2_IOS_UPSCALE) est dans ]0, 1[ ; *scale reçoit l'échelle retenue (1 : aucune).
int rt_ios_metal_keys(char *out, size_t size, int max_fps, const char *upscale, double *scale);
