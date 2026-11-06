// Passage en arrière-plan sur iOS (runtime/ios/lifecycle_ios.m) : le jeu
// cesse de rendre et de présenter tant que l'app n'est pas active.
#pragma once

// Observe les notifications d'UIApplication (thread principal, à la création
// de la fenêtre). P2_IOS_BACKGROUND_PAUSE=0 : rien n'est observé, rien ne s'arrête.
void rt_ios_lifecycle_init(void);
// 0 : app active ; 1 : inactive (centre de contrôle, appel, sélecteur d'apps) ;
// 2 : en arrière-plan.
int rt_ios_app_state(void);
// Thread principal, dans SDL_PollEvent (verrou invité tenu) : si l'app n'est
// plus active, attend qu'elle le redevienne, GPU vidé (OpenGL) et contexte
// OpenAL suspendu, en servant la boucle d'UIKit. Ailleurs ou app active : rien.
void rt_ios_pause_while_inactive(void);
