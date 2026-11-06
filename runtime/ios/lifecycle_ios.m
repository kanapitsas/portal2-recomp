// Arrière-plan sur iOS. Rien n'arrêtait le jeu quand l'app quittait l'écran :
// la boucle continuait de rendre et de présenter, or iOS tue une app qui fait
// travailler le GPU en OpenGL ES en arrière-plan, et refuse les soumissions
// Metal. Ici, dès que l'app n'est plus active (UIApplicationWillResignActive :
// centre de contrôle, appel, sélecteur d'apps, puis arrière-plan), le thread
// principal, qui est celui du jeu (SDL appelle main dessus et le jeu y rend,
// P2_GUEST_CPUS=1), s'arrête au prochain SDL_PollEvent :
// - OpenGL : glFinish (travail GPU en cours terminé avant l'arrière-plan) ;
//   Metal : rien à vider de ce côté, plus aucune image n'est soumise ;
// - OpenAL : contexte suspendu et détaché (motif d'Apple pour les
//   interruptions audio), rétabli au retour ; P2_IOS_BACKGROUND_AUDIO=0 n'y
//   touche pas ;
// - attente : la boucle d'UIKit est servie, verrou invité tenu (SDL y transmet
//   les événements d'app aux observateurs du jeu, code invité), puis relâché
//   pendant chaque pause de 20 ms pour laisser tourner les autres threads ;
// - reprise sur UIApplicationDidBecomeActive.
// Les appels OpenAL qui peuvent attendre le thread audio sont faits verrou
// invité relâché (CLAUDE.md). P2_IOS_BACKGROUND_PAUSE=0 coupe tout ce mécanisme.
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE
#import <UIKit/UIKit.h>
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include <stdatomic.h>
#include <stdio.h>
#include <unistd.h>
#include "env_ios.h"
#include "lifecycle_ios.h"

unsigned rt_guest_suspend(void);       // runtime/scheduler.c
void rt_guest_resume(unsigned saved);
void rt_ios_gl_finish(void);           // gl_ios.m

static _Atomic int app_state;   // 0 active, 1 inactive, 2 arrière-plan

int rt_ios_app_state(void) { return atomic_load_explicit(&app_state, memory_order_relaxed); }

void rt_ios_lifecycle_init(void) {
    static int done;
    if (done) return;
    done = 1;
    if (!rt_ios_env_flag("P2_IOS_BACKGROUND_PAUSE", 1)) {
        fprintf(stderr, "[ios] arrière-plan : pause désactivée (P2_IOS_BACKGROUND_PAUSE=0)\n");
        return;
    }
    NSNotificationCenter *center = NSNotificationCenter.defaultCenter;
    // Livrées sur le thread principal pendant que SDL (ou la pause) sert la boucle.
    [center addObserverForName:UIApplicationWillResignActiveNotification object:nil queue:nil
                    usingBlock:^(NSNotification *n) { (void)n; if (!atomic_load(&app_state)) atomic_store(&app_state, 1); }];
    [center addObserverForName:UIApplicationDidEnterBackgroundNotification object:nil queue:nil
                    usingBlock:^(NSNotification *n) { (void)n; atomic_store(&app_state, 2); }];
    [center addObserverForName:UIApplicationDidBecomeActiveNotification object:nil queue:nil
                    usingBlock:^(NSNotification *n) { (void)n; atomic_store(&app_state, 0); }];
}

void rt_ios_pause_while_inactive(void) {
    static int pausing;
    if (!atomic_load(&app_state) || pausing || ![NSThread isMainThread]) return;
    pausing = 1;
    CFTimeInterval start = CACurrentMediaTime();
    fprintf(stderr, "[ios] app inactive : jeu en pause\n");
    rt_ios_gl_finish();
    unsigned saved = rt_guest_suspend();
    ALCcontext *audio = rt_ios_env_flag("P2_IOS_BACKGROUND_AUDIO", 1) ? alcGetCurrentContext() : NULL;
    if (audio) { alcSuspendContext(audio); alcMakeContextCurrent(NULL); }
    rt_guest_resume(saved);
    int deepest = 1;
    while (atomic_load(&app_state)) {
        while (CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0, true) == kCFRunLoopRunHandledSource) {}
        int state = atomic_load(&app_state);
        if (!state) break;
        if (state > deepest) { deepest = state; fprintf(stderr, "[ios] app en arrière-plan\n"); }
        saved = rt_guest_suspend();
        usleep(20000);
        rt_guest_resume(saved);
    }
    saved = rt_guest_suspend();
    if (audio) { alcMakeContextCurrent(audio); alcProcessContext(audio); }
    rt_guest_resume(saved);
    fprintf(stderr, "[ios] app active : reprise après %.1f s%s\n", CACurrentMediaTime() - start,
            audio ? ", contexte OpenAL rétabli" : "");
    pausing = 0;
}
#endif
