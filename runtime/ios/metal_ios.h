// Rendu Direct3D 9 natif sur iOS (runtime/ios/metal_ios.m).
#pragma once
#include <stddef.h>

// Appareil Metal utilisable par libd3d9metal (textures BC) ; why : raison ou description.
int rt_ios_metal_usable(char *why, size_t size);
// Clés MTLD3D_CONFIG propres à iOS (present.maxFps suivant P2_IOS_MAX_FPS,
// render.scale suivant P2_IOS_UPSCALE).
void rt_ios_metal_config(char *out, size_t size);
// Lancement : Direct3D 9 (Metal) par défaut ou sur P2_RENDERER=d3d9, si
// libd3d9metal est liée et que l'appareil convient : P2_RENDERER=d3d9,
// environnement préparé (cache, MTLD3D_CONFIG, compteur d'images), rend 1.
// Sinon (P2_RENDERER=opengl, bibliothèque absente, appareil inapte) :
// P2_RENDERER=opengl et rend 0.
int rt_ios_metal_prepare(void);
// Vue SDL_MetalView créée (thread principal) : échelle de rendu, fréquence d'affichage.
void rt_ios_metal_view_created(void *metal_view);
