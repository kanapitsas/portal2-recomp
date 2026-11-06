// Plafond d'images par seconde sur iOS (runtime/ios/power_ios.m).
#pragma once

// Plafond voulu : P2_IOS_MAX_FPS (30, 40, 60 par défaut, 120), ramené à 30
// au plus en mode économie d'énergie ou téléphone chaud (état thermique
// « sérieux » ou « critique ») si l'économie automatique est active
// (P2_IOS_POWER_SAVE, 1 par défaut). Lu au lancement par le rendu Metal
// (present.maxFps de mtld3d, fixé à l'initialisation) ; suivi en cours de
// partie par le limiteur du rendu OpenGL (gl_ios.m).
int rt_ios_max_fps(void);
// Suit les changements d'état thermique et du mode économie d'énergie (une
// fois, n'importe quel thread). metal : le changement ne s'appliquera qu'au
// prochain lancement (le journal le dit).
void rt_ios_power_watch(int metal);
