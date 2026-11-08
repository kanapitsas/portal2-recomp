// Visée au gyroscope (iPhone) : vitesse de rotation de l'appareil -> mouvement
// de souris relatif, ajouté à celui du stick ou du trackpad tactile.
#pragma once

typedef struct {
    double counts_per_rad;   // comptes de souris par radian tourné (vitesse)
    double tighten;          // rad/s : sous ce seuil, la rotation est atténuée (tremblement de la main)
} rt_gyro_params;

// Comptes de souris par radian à « Normal » : 1,5 fois le mouvement réel à la
// sensibilité 3 du jeu (0,066° par compte, comme le stick tactile).
#define RT_GYRO_COUNTS_PER_RAD (1.5 * 57.29578 / 0.066)

// Paramètres par défaut, P2_GYRO_SPEED (multiplicateur) et
// P2_GYRO_TIGHTEN (rad/s) appliqués.
void rt_gyro_params_from_env(rt_gyro_params *p);

// Activée ? P2_GYRO=1 ou 0, sinon le défaut du jeu (on_by_default).
int rt_gyro_enabled(int on_by_default);

// Un échantillon de CoreMotion (repère de l'appareil) : vitesse de rotation
// w (rad/s), gravité g (en g, vers le sol), orient = +1 si l'axe y de l'appareil
// pointe vers la droite de l'écran (paysage gauche), -1 vers la gauche
// (paysage droit), 0 inconnu (rien). dt en secondes. Écrit dans out le
// mouvement de souris (x : droite, y : bas) ; renvoie 0 sans mouvement.
// Lacet autour de la verticale réelle (« player space ») : tourner sur soi
// tourne la caméra quelle que soit l'inclinaison du téléphone. Tangage autour
// de l'axe horizontal de l'écran : haut de l'écran vers soi, la caméra monte.
int rt_gyro_delta(const rt_gyro_params *p, const double w[3], const double g[3], int orient, double dt,
                  double out[2]);
