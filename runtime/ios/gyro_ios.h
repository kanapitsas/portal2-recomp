// Visée au gyroscope sur iOS (runtime/ios/gyro_ios.m) : capteurs CoreMotion
// intégrés par gyro_aim.c sur leur file, vidés à chaque image par la surcouche
// tactile (touch_controls.m). En-tête C : rien d'UIKit.
#pragma once

// Démarre (thread principal) si la visée est voulue (rt_gyro_enabled, selon le
// jeu) et qu'un capteur existe ; sinon rt_ios_gyro_available rend 0.
void rt_ios_gyro_start(int on_by_default);
int rt_ios_gyro_available(void);
// Capteurs en marche ou arrêtés (thread principal).
void rt_ios_gyro_run(int on);
// Orientation de l'écran : +1, -1 ou 0 (comme les tapes au dos).
void rt_ios_gyro_set_orient(int orient);
// Mouvement accumulé depuis le dernier appel, en comptes de souris (remis à zéro).
void rt_ios_gyro_take(double out[2]);
