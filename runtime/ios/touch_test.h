// Mode d'essai des commandes tactiles, sans toucher l'écran : simulateur
// seulement (runtime/ios/touch_test.m, compilé sous TARGET_OS_SIMULATOR ;
// ailleurs, ces fonctions rendent 0 et le mode n'existe pas).
//
// P2_TOUCH_TEST_MOVE="0.5,1" incline le stick de chaque valeur (négative : vers
// l'arrière) pendant P2_TOUCH_TEST_SECONDS (2 par défaut), après
// P2_TOUCH_TEST_DELAY secondes de partie (20 par défaut), trois secondes de
// repos entre deux ; direction P2_TOUCH_TEST_ANGLE (degrés, 0 vers l'avant, 90
// à droite). La surcouche n'est alors pas dessinée. Vitesse lue à l'écran avec
// « +cl_showpos 1 » ; avec « +sv_cheats 1 +host_framerate 0.0166667 », la
// physique est celle de 60 images/s malgré la lenteur du simulateur.
// P2_TOUCH_TEST_WASD=1 garde les touches W/A/S/D (témoin). Sur simulateur,
// SDL_JOYSTICK_MFI=0 écarte la manette MFi « Gamepad » qu'il expose et que le
// jeu prendrait avant la manette virtuelle.
// P2_TOUCH_TEST_CLICK="s,x,y" : clic gauche (points de la fenêtre SDL) s
// secondes après le premier affichage d'un menu, pour ouvrir un écran sans toucher.
#pragma once
#include <TargetConditionals.h>

#if TARGET_OS_SIMULATOR
// P2_TOUCH_TEST_MOVE posée : la surcouche n'est pas dessinée.
int rt_ios_touch_test_active(void);
// P2_TOUCH_TEST_WASD : pas de manette virtuelle, touches W/A/S/D.
int rt_ios_touch_test_wasd(void);
// Direction de l'inclinaison imposée, en radians (0 vers l'avant).
double rt_ios_touch_test_angle(void);
// Une étape par image : menu (interface du jeu visible), ready (manette prête
// ou refusée) ; *move reçoit l'inclinaison imposée (0 : aucune) ; rend 1 si
// elle a changé.
int rt_ios_touch_test_step(int menu, int ready, double *move);
// Clic d'essai, une étape par image : rend 0 (rien), 1 (survol de x, y), 2
// (appui), 3 (relâchement).
int rt_ios_touch_test_click(int menu, double *x, double *y);
#else
static inline int rt_ios_touch_test_active(void) { return 0; }
static inline int rt_ios_touch_test_wasd(void) { return 0; }
static inline double rt_ios_touch_test_angle(void) { return 0; }
static inline int rt_ios_touch_test_step(int menu, int ready, double *move) { (void)menu; (void)ready; *move = 0; return 0; }
static inline int rt_ios_touch_test_click(int menu, double *x, double *y) { (void)menu; (void)x; (void)y; return 0; }
#endif
