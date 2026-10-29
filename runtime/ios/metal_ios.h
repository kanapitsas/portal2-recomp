// Rendu Direct3D 9 natif sur iOS (runtime/ios/metal_ios.m).
#pragma once
#include <stddef.h>

// Appareil Metal utilisable par libd3d9metal (textures BC) ; why : raison ou description.
int rt_ios_metal_usable(char *why, size_t size);
// Clés MTLD3D_CONFIG propres à iOS (present.maxFps suivant P2_IOS_MAX_FPS).
void rt_ios_metal_config(char *out, size_t size);
// Lancement : si P2_RENDERER demande Direct3D 9 et que l'appareil convient,
// prépare l'environnement (cache, MTLD3D_CONFIG, compteur d'images) et rend 1 ;
// sinon retire P2_RENDERER (OpenGL) et rend 0.
int rt_ios_metal_prepare(void);
// Vue SDL_MetalView créée (thread principal) : échelle de rendu, fréquence d'affichage.
void rt_ios_metal_view_created(void *metal_view);
