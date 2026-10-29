// Rendu Direct3D 9 natif sur iOS (P2_RENDERER=d3d9, runtime/d3d9.c) : ce que
// l'app iOS fait autour de libd3d9metal, liée statiquement (tools/build_ios.py
// --d3d9metal).
// - Au lancement (ios/main_ios.c) : appareil Metal capable (textures BC, que
//   Source utilise partout et que mtld3d crée sans solution de repli), sinon
//   retour à OpenGL ; cache des nuanceurs dans Library/Caches ; cadence
//   plafonnée comme en OpenGL (P2_IOS_MAX_FPS, 60 par défaut).
// - À la création de la vue Metal (runtime/sdl_video.c) : échelle de rendu de
//   l'app (P2_IOS_RENDER_SCALE) au lieu de l'échelle native que SDL donne à la
//   couche, pour que mtld3d (taille du drawable = bounds × contentsScale)
//   présente une image de la taille du tampon arrière du jeu, sans
//   rééchantillonnage ; fréquence d'affichage demandée comme en OpenGL.
// - À chaque Present : compteur d'images (rt_ios_frames), lu par le compteur
//   affiché et par les tapes au dos (clic tenu au moins deux images).
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE
#import <Metal/Metal.h>
#import <UIKit/UIKit.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "d3d9_bridge.h"
#include "metal_ios.h"

extern _Atomic unsigned long rt_ios_frames;   // gl_ios.m
float rt_ios_render_scale(void);       // sdl_ios.m
void rt_ios_request_frame_rate(void);  // gl_ios.m

static void presented(void) { ++rt_ios_frames; }

int rt_ios_metal_usable(char *why, size_t size) {
    id<MTLDevice> device = MTLCreateSystemDefaultDevice();
    if (!device) { snprintf(why, size, "no Metal device"); return 0; }
    // Textures BC (DXT1/3/5) : Source n'a pas d'autre format pour ses textures
    // compressées, et mtld3d les crée telles quelles.
    BOOL bc = NO;
    if (@available(iOS 16.4, *)) bc = device.supportsBCTextureCompression;
    if (!bc) { snprintf(why, size, "%s: no BC texture compression", device.name.UTF8String); return 0; }
    snprintf(why, size, "%s, BC textures, %.0f MiB recommended working set", device.name.UTF8String,
             (double)device.recommendedMaxWorkingSetSize / (1024.0 * 1024.0));
    return 1;
}

// Clés MTLD3D_CONFIG propres à iOS, devant celles de l'utilisateur (la
// dernière gagne ; runtime/d3d9.c ajoute ensuite ses plafonds mémoire devant).
void rt_ios_metal_config(char *out, size_t size) {
    const char *e = getenv("P2_IOS_MAX_FPS");
    int fps = e && atoi(e) > 0 ? atoi(e) : 60;
    snprintf(out, size, "present.maxFps=%d", fps);
}

int rt_ios_metal_prepare(void) {
    const char *renderer = getenv("P2_RENDERER");
    if (!renderer || (strcmp(renderer, "d3d9") && strcmp(renderer, "metal"))) return 0;
    char why[256];
    if (!rt_ios_metal_usable(why, sizeof(why))) {
        fprintf(stderr, "[ios] renderer: Metal unusable (%s), staying on OpenGL\n", why);
        unsetenv("P2_RENDERER");
        return 0;
    }
    fprintf(stderr, "[ios] renderer: Metal requested (%s)\n", why);
    const char *home = getenv("HOME");
    if (!getenv("P2_D3D9_CACHE") && home) {
        char cache[1024];
        snprintf(cache, sizeof(cache), "%s/Library/Caches/d3d9metal", home);
        setenv("P2_D3D9_CACHE", cache, 0);   // runtime/d3d9.c crée le dossier
    }
    char ours[64];
    rt_ios_metal_config(ours, sizeof(ours));
    const char *given = getenv("MTLD3D_CONFIG");
    if (!given || !*given) setenv("MTLD3D_CONFIG", ours, 1);
    else {
        size_t length = strlen(ours) + 1 + strlen(given) + 1;
        char *joined = malloc(length);
        if (joined) {
            snprintf(joined, length, "%s;%s", ours, given);
            setenv("MTLD3D_CONFIG", joined, 1);
            free(joined);
        }
    }
    rt_d3d9_on_present = presented;
    return 1;
}

void rt_ios_metal_view_created(void *metal_view) {
    UIView *view = (__bridge UIView *)metal_view;
    CGFloat wanted = rt_ios_render_scale();
    // SDL : échelle native de l'écran si la fenêtre est haute résolution, 1 sinon.
    if (view && fabs(view.contentScaleFactor - wanted) > 0.01) {
        view.contentScaleFactor = wanted;   // contentsScale de la couche (et drawableSize par SDL)
        [view setNeedsLayout];
        [view layoutIfNeeded];
    }
    CAMetalLayer *layer = (CAMetalLayer *)view.layer;
    fprintf(stderr, "[ios] Metal view: %.0fx%.0f points, scale %.2f, drawable %.0fx%.0f\n", view.bounds.size.width,
            view.bounds.size.height, layer.contentsScale, layer.drawableSize.width, layer.drawableSize.height);
    rt_ios_request_frame_rate();
}
#endif
