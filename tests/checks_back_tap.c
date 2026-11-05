// Tapes au dos du port iOS (runtime/ios/back_tap.c, C portable, testé ici
// sur l'hôte) : échantillons CoreMotion synthétiques, à 100 et 200 Hz.
#include "checks.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "../runtime/ios/back_tap.c"

// Une tape : poussée selon z (dos : vers l'utilisateur ; écran : à l'opposé)
// ou selon y (tranche), à la position y de signe ry dans le repère de
// l'appareil. CoreMotion rend l'opposé de l'accélération physique.
// ring : vibration de la main qui suit la tape (g ; 30 Hz, amortie en
// ring_tau, 100 ms par défaut), mesurée sur l'iPhone : le pic reste au-dessus
// de 35 % de son maximum 50 à 100 ms.
// rot_z : rotation autour de z (rad/s), dans le repère de l'écran (> 0 : vers la
// gauche) ; la main qui tape fait pivoter le téléphone dans son plan.
typedef struct { double t; int ry; int axis; float accel, rot, ring, ring_tau, rot_z; } Tap;
typedef struct {
    double rate, length;
    Tap taps[4]; int ntaps;
    double touches[4]; int ntouch;
    float gravity[3];
    double shake_from, shake_to;      // secousse de la main : 4 Hz, 1 g
    double turn_from;                 // téléphone qu'on tourne à 1 rad/s autour de y
    float wobble;                     // geste ample et lent : 2 Hz selon z (g)
    float noise;                      // amplitude du bruit d'accélération (g)
    int orient, active;
    double active_from;               // en menu jusque-là (0 : toujours la valeur active)
    rt_backtap_params params;
} Scene;
typedef struct { int accepted, reports, side[8]; const char *method[8], *reject[8]; int weak[8]; } Outcome;

static float half_sine(double x, double width) { return x < 0 || x > width ? 0 : (float)sin(M_PI * x / width); }

static Outcome play(const Scene *sc) {
    rt_backtap d; rt_backtap_init(&d, &sc->params);
    Outcome o; memset(&o, 0, sizeof(o));
    unsigned seed = 12345;
    int touched = 0;
    for (long i = 0; i < (long)(sc->length * sc->rate); ++i) {
        double t = 100 + i / sc->rate;   // horloge de l'appareil : secondes depuis le démarrage
        rt_backtap_sample s; memset(&s, 0, sizeof(s));
        s.t = t;
        for (int k = 0; k < 3; ++k) {
            seed = seed * 1103515245u + 12345u;
            s.a[k] = sc->noise * ((float)(seed >> 8 & 0xffff) / 32768.0f - 1);
            seed = seed * 1103515245u + 12345u;
            s.w[k] = 0.01f * ((float)(seed >> 8 & 0xffff) / 32768.0f - 1);
            s.g[k] = sc->gravity[k];
        }
        for (int j = 0; j < sc->ntaps; ++j) {
            const Tap *p = &sc->taps[j];
            double x = t - 100 - p->t;
            float push = p->accel * half_sine(x, 0.020) - 0.3f * p->accel * half_sine(x - 0.020, 0.020);
            double tau = p->ring_tau > 0 ? p->ring_tau : 0.100;
            if (x > 0.020) push += p->ring * (float)(exp(-(x - 0.020) / tau) * sin(2 * M_PI * 30 * (x - 0.020)));
            s.a[p->axis] -= push;
            float shape = x < 0 ? 0 : (float)(x < 0.005 ? x / 0.005 : exp(-(x - 0.005) / 0.030));
            s.w[0] += p->ry * p->rot * shape;
            s.w[2] += p->rot_z * shape;
        }
        double u = t - 100;
        if (u >= sc->shake_from && u < sc->shake_to) {
            s.a[1] += (float)sin(2 * M_PI * 4 * (u - sc->shake_from));
            s.w[2] += 2 * (float)cos(2 * M_PI * 4 * (u - sc->shake_from));
        }
        s.a[2] += sc->wobble * (float)sin(2 * M_PI * 2 * u);
        if (sc->turn_from > 0 && u >= sc->turn_from) {
            double a = u - sc->turn_from;
            s.g[0] = sc->gravity[0] * (float)cos(a) + sc->gravity[2] * (float)sin(a);
            s.g[2] = -sc->gravity[0] * (float)sin(a) + sc->gravity[2] * (float)cos(a);
            s.w[1] += 1;
        }
        // Toucher : livré par UIKit ~20 ms après son horodatage.
        while (touched < sc->ntouch && 100 + sc->touches[touched] + 0.020 <= t) rt_backtap_touch(&d, 100 + sc->touches[touched++]);
        rt_backtap_report r;
        int active = sc->active && u >= sc->active_from;
        if (rt_backtap_feed(&d, &s, sc->orient, active, &r)) {
            if (o.reports < 8) {
                o.side[o.reports] = r.side; o.method[o.reports] = r.method;
                o.reject[o.reports] = r.reject; o.weak[o.reports] = r.weak;
            }
            ++o.reports;
            if (!r.reject) ++o.accepted;
        }
    }
    return o;
}

static Scene scene(double rate) {
    Scene sc; memset(&sc, 0, sizeof(sc));
    sc.rate = rate; sc.length = 2.5; sc.noise = 0.005f; sc.orient = 1; sc.active = 1;
    sc.gravity[0] = -0.8f; sc.gravity[2] = -0.6f;   // paysage, incliné en arrière
    sc.shake_from = sc.shake_to = -1;
    rt_backtap_defaults(&sc.params);
    return sc;
}
// Tape au dos du côté gauche (-1) ou droit (+1) de l'écran ; orient : +1 si
// l'axe y de l'appareil pointe vers la droite de l'écran.
static void back_tap(Scene *sc, double t, int screen_side) {
    sc->taps[sc->ntaps++] = (Tap){t, screen_side * sc->orient, 2, 0.3f, 0.4f, 0, 0, 0};
}
static const double kRates[] = {100, 200};

RT_CHECK(backtap_left_and_right_back_taps_pick_their_side) {
    for (int r = 0; r < 2; ++r)
        for (int orient = -1; orient <= 1; orient += 2)
            for (int side = -1; side <= 1; side += 2) {
                Scene sc = scene(kRates[r]); sc.orient = orient;
                back_tap(&sc, 1.0, side);
                Outcome o = play(&sc);
                assert(o.reports == 1 && o.accepted == 1);
                assert(o.side[0] == side && !strcmp(o.method[0], "rotation"));
            }
}

// Tape près d'un coin (26/10, téléphone retourné) : la rotation autour de x
// est faible et de signe trompeur, celle autour de z (la main qui tape fait
// pivoter le téléphone) donne le côté.
RT_CHECK(backtap_corner_tap_side_from_screen_plane_rotation) {
    for (int r = 0; r < 2; ++r)
        for (int orient = -1; orient <= 1; orient += 2)
            for (int side = -1; side <= 1; side += 2) {
                Scene sc = scene(kRates[r]); sc.orient = orient;
                back_tap(&sc, 1.0, side);
                sc.taps[0].rot = -0.12f;                 // signe opposé au côté
                sc.taps[0].rot_z = -0.2f * (float)side;  // gauche : z > 0
                Outcome o = play(&sc);
                assert(o.accepted == 1 && o.side[0] == side);
            }
}

// Orientation inconnue : déduite du signe de la gravité selon x.
RT_CHECK(backtap_orientation_from_gravity) {
    Scene sc = scene(100);
    back_tap(&sc, 1.0, -1);   // à gauche avec l'axe y vers la droite (orient = 1)
    sc.gravity[0] = 0.8f;     // axe y vers la droite de l'écran <=> gravité selon +x
    sc.orient = 0;
    Outcome o = play(&sc);
    assert(o.accepted == 1 && o.side[0] == -1);
    sc.gravity[0] = -0.8f;    // l'autre paysage : mêmes données, côté opposé
    o = play(&sc);
    assert(o.accepted == 1 && o.side[0] == 1);
    sc.gravity[0] = 0.1f;     // presque à plat : indécidable
    o = play(&sc);
    assert(o.accepted == 0 && !strcmp(o.reject[0], "orientation inconnue"));
}

RT_CHECK(backtap_side_tap_uses_lateral_acceleration) {
    for (int r = 0; r < 2; ++r) {
        // Tape sur la tranche gauche : poussée physique vers la droite (+y si orient = 1).
        Scene sc = scene(kRates[r]);
        sc.taps[sc.ntaps++] = (Tap){1.0, 0, 1, 0.3f, 0, 0, 0, 0};
        Outcome o = play(&sc);
        assert(o.accepted == 1 && o.side[0] == -1 && !strcmp(o.method[0], "latéral"));
        sc.params.lateral_sign = 0;
        o = play(&sc);
        assert(o.accepted == 0 && !strcmp(o.reject[0], "côté indécis"));
    }
}

// Tapes réelles (calibration sur l'iPhone, 100 Hz) : la main fait vibrer le
// téléphone ; une limite de durée à 60 ms en rejetait plus de la moitié.
RT_CHECK(backtap_tap_with_hand_ringing_is_accepted) {
    for (int r = 0; r < 2; ++r)
        for (int side = -1; side <= 1; side += 2) {
            Scene sc = scene(kRates[r]);
            back_tap(&sc, 1.0, side);
            sc.taps[0].ring = 0.25f;
            Outcome o = play(&sc);
            assert(o.reports == 1 && o.accepted == 1 && o.side[0] == side);
        }
}

// Tape forte (calibration) : la vibration de la main dépasse la poussée, le
// maximum arrive 30 à 40 ms après le début ; la tape suivante, 0,45 s plus
// tard, tombe encore dans cette vibration (mesurée après les tapes de plus de
// 0,5 g : 0,07 g de moyenne quadratique à 200-250 ms, 0,04 g à 250-300 ms).
RT_CHECK(backtap_hard_taps_in_a_row_are_accepted) {
    for (int r = 0; r < 2; ++r) {
        Scene sc = scene(kRates[r]);
        back_tap(&sc, 1.0, -1); back_tap(&sc, 1.45, -1); back_tap(&sc, 1.9, 1);
        for (int i = 0; i < 3; ++i) { sc.taps[i].accel = 0.4f; sc.taps[i].ring = 0.8f; sc.taps[i].ring_tau = 0.085f; }
        Outcome o = play(&sc);
        assert(o.reports == 3 && o.accepted == 3);
        assert(o.side[0] == -1 && o.side[1] == -1 && o.side[2] == 1);
    }
}

// Frémissement (candidat faible) puis vraie tape 60 ms plus tard : la tape
// prend la place du candidat faible au lieu d'être couverte par lui.
RT_CHECK(backtap_tap_after_weak_blip_is_not_hidden) {
    for (int r = 0; r < 2; ++r)
        for (int side = -1; side <= 1; side += 2) {
            Scene sc = scene(kRates[r]);
            sc.taps[sc.ntaps++] = (Tap){1.0, 1, 2, 0.06f, 0, 0, 0, 0};   // sous accel_min, au-dessus de sa moitié
            back_tap(&sc, 1.06, side);
            Outcome o = play(&sc);
            assert(o.accepted == 1 && o.side[o.reports - 1] == side);
        }
}

// Tapes à 0,28 s d’écart : la vibration de la première est encore dans la
// fenêtre d'avant la seconde, mais elle est retombée (moins de 25 % du pic).
RT_CHECK(backtap_quick_taps_whose_ringing_decays_are_accepted) {
    for (int r = 0; r < 2; ++r) {
        Scene sc = scene(kRates[r]);
        back_tap(&sc, 1.0, 1); back_tap(&sc, 1.28, -1);
        for (int i = 0; i < 2; ++i) { sc.taps[i].ring = 0.25f; sc.taps[i].ring_tau = 0.06f; }
        Outcome o = play(&sc);
        assert(o.accepted == 2 && o.side[0] == 1 && o.side[1] == -1);
        for (int i = 0; i < 2; ++i) { sc.taps[i].ring = 0.6f; sc.taps[i].ring_tau = 1; }   // vibration qui ne retombe pas
        o = play(&sc);
        assert(o.accepted == 1 && o.side[0] == 1);   // la suite : agitation ou historique insuffisant
        for (int i = 1; i < o.reports && i < 8; ++i) assert(o.reject[i]);
    }
}

// Posé sur la table, écran vers le haut : le poser ou le reprendre secoue.
RT_CHECK(backtap_phone_lying_flat_is_ignored) {
    Scene sc = scene(100);
    sc.gravity[0] = 0; sc.gravity[2] = -1;
    back_tap(&sc, 1.0, 1);
    Outcome o = play(&sc);
    assert(o.reports == 1 && o.accepted == 0 && !strcmp(o.reject[0], "téléphone à plat"));
    sc.gravity[2] = 1;   // écran vers le bas, joueur allongé : accepté
    o = play(&sc);
    assert(o.accepted == 1);
}

// Secousse : seul son premier à-coup, depuis le repos, peut passer pour une
// tape (limite connue : l'écarter retarderait chaque tir) ; la suite non.
RT_CHECK(backtap_shake_is_not_a_tap) {
    for (int r = 0; r < 2; ++r) {
        Scene sc = scene(kRates[r]);
        sc.shake_from = 0.8; sc.shake_to = 1.8;
        Outcome o = play(&sc);
        assert(o.reports > 1 && o.accepted <= 1);
        for (int i = 1; i < o.reports && i < 8; ++i) assert(o.reject[i]);
        sc.shake_from = 0.4;   // tape au milieu de la secousse
        back_tap(&sc, 1.0, 1);
        o = play(&sc);
        assert(o.accepted <= 1);   // le premier à-coup au plus, pas la tape
        for (int i = 1; i < o.reports && i < 8; ++i) assert(o.reject[i]);
    }
}

RT_CHECK(backtap_screen_touch_rejects_nearby_peak) {
    const double offsets[] = {-0.06, 0.0, 0.01, 0.07};
    for (int r = 0; r < 2; ++r)
        for (int i = 0; i < 4; ++i) {
            Scene sc = scene(kRates[r]);
            sc.taps[sc.ntaps++] = (Tap){1.0, 1, 2, -0.3f, 0.4f, 0, 0, 0};   // écran : poussée vers l'arrière
            sc.touches[sc.ntouch++] = 1.0 + offsets[i];
            Outcome o = play(&sc);
            assert(o.reports == 1 && o.accepted == 0 && !strcmp(o.reject[0], "toucher à l'écran"));
        }
    // Toucher lointain (doigt posé sur un stick bien avant) : tape acceptée.
    Scene sc = scene(100);
    back_tap(&sc, 1.0, 1);
    sc.touches[sc.ntouch++] = 0.5; sc.touches[sc.ntouch++] = 1.2;
    Outcome o = play(&sc);
    assert(o.accepted == 1 && o.side[0] == 1);
}

// Sens z exigé (après calibration) : le dos et l'écran poussent en sens opposés.
RT_CHECK(backtap_z_sign_separates_back_from_screen) {
    Scene sc = scene(200);
    sc.params.z_sign = -1;   // dos : poussée vers l'utilisateur, a_z CoreMotion < 0
    back_tap(&sc, 1.0, -1);
    Outcome o = play(&sc);
    assert(o.accepted == 1);
    sc.taps[0].accel = -0.3f;
    o = play(&sc);
    assert(o.accepted == 0 && !strcmp(o.reject[0], "sens z"));
}

RT_CHECK(backtap_nothing_in_menus) {
    Scene sc = scene(100);
    sc.active = 0;
    back_tap(&sc, 1.0, 1);
    Outcome o = play(&sc);
    assert(o.accepted == 0 && !strcmp(o.reject[0], "menu"));
    // Clic qui reprend la partie : pic en menu, jugé une fois en partie.
    const double resume[] = {1.03, 0.95};
    for (int i = 0; i < 2; ++i) {
        sc.active = 1; sc.active_from = resume[i];
        o = play(&sc);
        assert(o.reports == 1 && o.accepted == 0 && !strcmp(o.reject[0], "menu"));
    }
    sc.active_from = 0.85;   // partie reprise bien avant la tape
    o = play(&sc);
    assert(o.accepted == 1);
}

RT_CHECK(backtap_refractory_period) {
    for (int r = 0; r < 2; ++r) {
        Scene sc = scene(kRates[r]);
        back_tap(&sc, 1.0, -1); back_tap(&sc, 1.15, 1);   // rebond ou deuxième tape trop proche
        Outcome o = play(&sc);
        assert(o.reports == 1 && o.accepted == 1 && o.side[0] == -1);
        sc.taps[1].t = 1.4;
        o = play(&sc);
        assert(o.accepted == 2 && o.side[0] == -1 && o.side[1] == 1);
        // Période courte : la tape d'avant ne compte pas dans l'agitation.
        sc.params.refractory = 0.1f; sc.taps[1].t = 1.15;
        o = play(&sc);
        assert(o.accepted == 2 && o.side[0] == -1 && o.side[1] == 1);
    }
}

RT_CHECK(backtap_motion_rejects) {
    Scene sc = scene(100);
    sc.turn_from = 0.6;   // téléphone qu'on tourne
    back_tap(&sc, 1.0, 1);
    Outcome o = play(&sc);
    assert(o.accepted == 0 && !strcmp(o.reject[0], "gravité qui change"));
    for (int r = 0; r < 2; ++r) {
        sc = scene(kRates[r]);
        sc.wobble = 0.12f;    // geste lent : rien à lui seul (au plus des candidats faibles), mais la tape est rejetée
        o = play(&sc);
        assert(o.accepted == 0);
        for (int i = 0; i < o.reports && i < 8; ++i) assert(o.weak[i]);
        back_tap(&sc, 1.0, 1);
        o = play(&sc);
        assert(o.reports >= 1 && o.accepted == 0);   // agitation, ou couverte par un candidat faible du geste
        for (int i = 0; i < o.reports && i < 8; ++i)
            assert(o.weak[i] || !strcmp(o.reject[i], "agitation avant le pic"));
    }
    sc = scene(100);
    sc.noise = 0.12f;     // marche : agitation continue
    back_tap(&sc, 1.0, 1);
    o = play(&sc);
    assert(o.accepted == 0);
    for (int i = 0; i < o.reports && i < 8; ++i) assert(o.reject[i]);
}

// Candidats sous les seuils : signalés (journal), sans période réfractaire.
RT_CHECK(backtap_weak_candidates_only_logged) {
    Scene sc = scene(200);
    sc.taps[sc.ntaps++] = (Tap){1.0, 1, 2, 0.04f, 0.4f, 0, 0, 0};
    sc.params.onset_factor = 1;
    Outcome o = play(&sc);
    assert(o.reports == 0);
    sc.params.onset_factor = 0.5f;
    back_tap(&sc, 1.12, -1);
    o = play(&sc);
    assert(o.reports == 2 && o.weak[0] && !strcmp(o.reject[0], "sous les seuils"));
    assert(o.accepted == 1 && o.side[1] == -1);
}

// Tape légère à gauche (calibration) : 0,07 à 0,10 g seulement, mais le
// téléphone tourne de 0,4 à 0,7 rad/s : retenue par la voie gyroscope.
RT_CHECK(backtap_light_tap_found_by_gyroscope) {
    for (int r = 0; r < 2; ++r)
        for (int side = -1; side <= 1; side += 2) {
            Scene sc = scene(kRates[r]);
            back_tap(&sc, 1.0, side);
            sc.taps[0].accel = 0.07f; sc.taps[0].rot = 0.6f;
            Outcome o = play(&sc);
            assert(o.reports == 1 && o.accepted == 1 && o.side[0] == side);
            sc.taps[0].rot = 0.15f;   // sans rotation franche : trop faible
            o = play(&sc);
            assert(o.accepted == 0);
            // Candidat ouvert par la rotation seule : voie accélération hors d'atteinte.
            sc.taps[0].rot = 0.6f; sc.params.accel_min = 1;
            o = play(&sc);
            assert(o.accepted == 1 && o.side[0] == side);
            // Téléphone qui bouge un peu (pouces sur l'écran, 0,04 g) : la voie
            // gyroscope se tait (tirs parasites du 26/10), une tape franche passe.
            sc.params.accel_min = 0.10f; sc.noise = 0.04f;
            o = play(&sc);
            assert(o.accepted == 0);
            sc.taps[0].accel = 0.3f;
            o = play(&sc);
            assert(o.accepted == 1);
            for (int i = 0; i < o.reports && i < 8; ++i) if (!o.reject[i]) assert(o.side[i] == side);
        }
}

// Seuils lus dans l'environnement (touch_controls.m et tools/backtap_replay.c) :
// sensibilité, variables par seuil et leurs bornes basses.
RT_CHECK(backtap_params_from_env_bounds) {
    static const char *const names[] = {"P2_BACKTAP_SENSITIVITY", "P2_BACKTAP_ACCEL", "P2_BACKTAP_JERK", "P2_BACKTAP_ROT",
        "P2_BACKTAP_GYRO_ROT", "P2_BACKTAP_FLAT", "P2_BACKTAP_LATERAL", "P2_BACKTAP_ZSIGN", "P2_BACKTAP_INVERT",
        "P2_BACKTAP_TOUCH_GUARD"};
    for (size_t i = 0; i < sizeof(names) / sizeof(*names); ++i) unsetenv(names[i]);
    rt_backtap_params def, p;
    rt_backtap_defaults(&def);
    rt_backtap_params_from_env(&p);
    assert(!memcmp(&p, &def, sizeof(p)));   // rien de défini : valeurs par défaut

    setenv("P2_BACKTAP_SENSITIVITY", "2", 1);
    rt_backtap_params_from_env(&p);
    assert(fabsf(p.accel_min - def.accel_min / 2) < 1e-6f && fabsf(p.jerk_min - def.jerk_min / 2) < 1e-6f);
    assert(fabsf(p.gyro_accel_min - def.gyro_accel_min / 2) < 1e-6f && fabsf(p.gyro_rot_min - def.gyro_rot_min / 2) < 1e-6f);
    assert(p.rot_min == def.rot_min);   // la sensibilité ne touche pas le score de côté
    setenv("P2_BACKTAP_SENSITIVITY", "0.05", 1);   // sous 0,1 : ignorée
    rt_backtap_params_from_env(&p);
    assert(p.accel_min == def.accel_min && p.jerk_min == def.jerk_min);
    setenv("P2_BACKTAP_SENSITIVITY", "0.1", 1);    // la borne est acceptée
    rt_backtap_params_from_env(&p);
    assert(fabsf(p.accel_min - def.accel_min / 0.1f) < 1e-5f);
    unsetenv("P2_BACKTAP_SENSITIVITY");

    // Une variable par seuil prime sur la sensibilité ; sous sa borne, ignorée.
    setenv("P2_BACKTAP_SENSITIVITY", "2", 1);
    setenv("P2_BACKTAP_ACCEL", "0.3", 1);
    setenv("P2_BACKTAP_JERK", "0", 1);        // borne 0 : acceptée
    setenv("P2_BACKTAP_ROT", "0", 1);         // borne 0,001 : ignorée
    setenv("P2_BACKTAP_GYRO_ROT", "-1", 1);   // négatif : ignoré
    setenv("P2_BACKTAP_FLAT", "-3", 1);       // sous -2 : ignoré
    setenv("P2_BACKTAP_TOUCH_GUARD", "", 1);  // vide : ignoré
    rt_backtap_params_from_env(&p);
    assert(fabsf(p.accel_min - 0.3f) < 1e-6f && p.jerk_min == 0);
    assert(p.rot_min == def.rot_min && fabsf(p.gyro_rot_min - def.gyro_rot_min / 2) < 1e-6f);
    assert(p.flat_z == def.flat_z && p.touch_guard == def.touch_guard);

    // Signes : > 0, < 0 ou 0 ; absent : valeur par défaut.
    setenv("P2_BACKTAP_LATERAL", "-3", 1);
    setenv("P2_BACKTAP_ZSIGN", "2", 1);
    setenv("P2_BACKTAP_INVERT", "1", 1);
    rt_backtap_params_from_env(&p);
    assert(p.lateral_sign == -1 && p.z_sign == 1 && p.invert == 1);
    setenv("P2_BACKTAP_LATERAL", "0", 1);
    setenv("P2_BACKTAP_INVERT", "-1", 1);
    rt_backtap_params_from_env(&p);
    assert(p.lateral_sign == 0 && p.invert == 0);
    for (size_t i = 0; i < sizeof(names) / sizeof(*names); ++i) unsetenv(names[i]);
    rt_backtap_params_from_env(&p);
    assert(!memcmp(&p, &def, sizeof(p)));
}
