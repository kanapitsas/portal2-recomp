// Tapes au dos ou sur les côtés du téléphone tenu en paysage : un index qui
// frappe le dos près du bord gauche tire le portail bleu, près du bord droit
// l'orange.
//
// Repère de l'appareil (CoreMotion) : x le long du petit côté, y le long du
// grand côté (vers le haut en portrait), z sortant de l'écran. En paysage, y
// est horizontal pour le joueur et x vertical.
//
// Détection : un candidat commence quand l'accélération utilisateur dépasse
// accel_min et que sa variation d'un échantillon au suivant (secousse)
// dépasse jerk_min, ou (voie gyroscope) quand la rotation varie brusquement
// autour de x (gyro_jerk_min) avec un peu d'accélération (gyro_accel_min) ;
// il est alors retenu si la secousse atteint jerk_min, la rotation autour de x
// gyro_rot_min, et si le téléphone était immobile juste avant (gyro_calm_max). Calibration : à
// gauche, l'index tape près du bord et fait tourner le téléphone (0,4 à
// 0,7 rad/s) en l'accélérant peu (0,07 à 0,10 g). Il est jugé DELAY plus tard, une fois connus son pic, sa
// durée et les touchers qui l'entourent :
// - durée : temps passé au-dessus de max(accel_min, 35 % du pic) ; la main
//   fait vibrer le téléphone 50 à 100 ms après une tape (calibration) : la
//   limite par défaut est au-delà de la fenêtre de mesure, donc inactive ;
// - agitation : moyenne quadratique de |a| sur 300 ms avant le pic, prises en
//   remontant jusqu'à 600 ms hors des 250 ms qui suivent chacun des deux
//   candidats d'avant (une tape forte fait vibrer le téléphone jusque-là) ; un
//   pic au milieu d'un mouvement n'est pas une tape ; et dans les 50 ms d'avant,
//   vibration comprise, celle-ci doit être retombée sous 25 % du pic précédent
//   (mesuré : ~10 % à 200 ms ; une secousse reste vers 70 %) ;
// - téléphone à plat, écran vers le haut (gravité z ≈ -1) : posé sur une
//   table, on ne tape pas son dos ; le poser ou le reprendre secoue ;
// - gravité : sa variation dans les 200 ms qui précèdent (téléphone qu'on
//   tourne) ;
// - toucher à l'écran à moins de touch_guard (une tape sur l'écran secoue
//   aussi le téléphone) ; d'où le délai du jugement, touch_guard + 20 ms pour
//   que l'événement UIKit soit arrivé.
//
// Côté : une poussée F selon z appliquée au point r = (rx, ry, 0) donne un
// couple r × F = (ry·F, -rx·F, 0) : la rotation brève autour de x a le signe
// de ry, la position de la tape le long du grand côté. Le signe est pris sur
// l'écart maximal à la rotation moyenne d'avant le pic. Une tape sur la
// tranche d'un petit côté (poussée selon y) ne fait presque pas tourner :
// alors, si l'accélération est surtout selon y, son signe décide (repli
// « latéral »). CoreMotion rend l'accélération utilisateur avec le signe
// opposé à l'accélération physique (téléphone posé à plat : gravité
// (0, 0, -1)) ; si l'essai sur l'appareil dit le contraire, lateral_sign = -1.
// L'orientation de l'écran (paysage gauche ou droit) dit vers quel bord pointe
// y ; à défaut, le signe de la gravité selon x.
#include "back_tap.h"
#include "env_ios.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

#ifndef RT_BACKTAP_MIN_HISTORY
#define RT_BACKTAP_MIN_HISTORY 0.080
#endif
static const double kPre = 0.300, kPreGap = 0.015, kGravityLag = 0.200, kWindow = 0.040, kPreMax = 0.600, kTail = 0.050,
                    kDecay = 0.25, kRinging = 0.250, kMinHistory = RT_BACKTAP_MIN_HISTORY;

// Valeurs par défaut : estimations à calibrer (docs/ios.md). Une tape d'index
// sur un téléphone tenu à deux mains : pic de 0,1 à 0,5 g, rotation de 0,1 à
// 1 rad/s (téléphone seul : ~0,7 rad/s pour un doigt de 10 g à 0,5 m/s, la
// main en absorbe une part). Au repos en jeu : bruit ~0,005 g et 0,01 rad/s,
// pouces sur l'écran quelques centièmes de g, variations lentes.
void rt_backtap_defaults(rt_backtap_params *p) {
    p->accel_min = 0.10f;     // 20 fois le bruit, sous une tape légère
    p->jerk_min = 6.0f;       // 0,06 g en 10 ms : un geste de la main monte 10 fois moins vite
    p->rot_min = 0.08f;       // 8 fois le bruit du gyroscope
    p->side_x_weight = 0.5f;  // 79/79 tapes étiquetées (x seul : 72/79)
    p->gyro_accel_min = 0.06f; // au-dessus du bruit en jeu (0,02-0,05 g en moyenne quadratique), secousse jerk_min exigée aussi
    p->gyro_rot_min = 0.30f;   // tapes à gauche manquées : 0,44 à 0,74 rad/s ; tapes retenues : 0,10 à 1,3
    p->gyro_jerk_min = 15.0f;  // 0,15 rad/s en 10 ms : le poignet tourne bien plus lentement
    p->gyro_calm_max = 0.03f;  // tapes légères retenues : 0,016-0,025 g ; tirs parasites en jeu : 0,037-0,054 g
    p->lateral_ratio = 0.7f;  // repli latéral seulement si y porte 70 % du pic
    p->lateral_sign = 1;
    p->invert = 0;
    p->z_sign = 0;            // à fixer après calibration : dos et écran poussent en sens opposés
    p->refractory = 0.25f;    // 4 tapes/s au plus ; couvre les rebonds de la main
    p->touch_guard = 0.08f;   // ±80 ms autour d'un début ou d'une fin de toucher
    p->max_width = 0.12f;     // inactive : tapes mesurées de 50 à 100 ms (vibration de la main), fenêtre de 100 ms
    p->flat_z = -0.95f;       // en jeu (calibration) : -0,75 à -0,86 ; posé sur la table : -1,00
    p->pre_rms_max = 0.05f;   // téléphone tenu immobile : ~0,01 g ; marche : 0,1 g et plus
    p->gravity_max = 0.12f;   // ~7° de rotation en 200 ms
    p->onset_factor = 0.5f;   // un candidat précoce cerne mieux le pic ; les faibles sont jugés « sous les seuils »
}

// Anciens noms, gardés pour tools/backtap_replay.c : les lectures sont dans env_ios.h.
float rt_backtap_env_float(const char *name, float def, float min) { return rt_ios_env_float(name, def, min); }
int rt_backtap_env_sign(const char *name, int def) { return rt_ios_env_sign(name, def); }
void rt_backtap_params_from_env(rt_backtap_params *p) {
    rt_backtap_defaults(p);
    // Sensibilité (Réglages : 0,75 / 1 / 1,33) : seuils de détection divisés
    // d'autant ; les variables par seuil, ci-dessous, priment.
    float k = rt_backtap_env_float("P2_BACKTAP_SENSITIVITY", 1, 0.1f);
    p->accel_min /= k; p->jerk_min /= k; p->gyro_accel_min /= k; p->gyro_rot_min /= k;
    p->accel_min = rt_backtap_env_float("P2_BACKTAP_ACCEL", p->accel_min, 0.001f);
    p->jerk_min = rt_backtap_env_float("P2_BACKTAP_JERK", p->jerk_min, 0);
    p->rot_min = rt_backtap_env_float("P2_BACKTAP_ROT", p->rot_min, 0.001f);
    p->side_x_weight = rt_backtap_env_float("P2_BACKTAP_SIDE_X", p->side_x_weight, 0);
    p->gyro_accel_min = rt_backtap_env_float("P2_BACKTAP_GYRO_ACCEL", p->gyro_accel_min, 0.001f);
    p->gyro_rot_min = rt_backtap_env_float("P2_BACKTAP_GYRO_ROT", p->gyro_rot_min, 0.001f);
    p->gyro_jerk_min = rt_backtap_env_float("P2_BACKTAP_GYRO_JERK", p->gyro_jerk_min, 0);
    p->gyro_calm_max = rt_backtap_env_float("P2_BACKTAP_GYRO_CALM", p->gyro_calm_max, 0);
    p->refractory = rt_backtap_env_float("P2_BACKTAP_REFRACTORY", p->refractory, 0);
    p->touch_guard = rt_backtap_env_float("P2_BACKTAP_TOUCH_GUARD", p->touch_guard, 0);
    p->max_width = rt_backtap_env_float("P2_BACKTAP_WIDTH", p->max_width, 0.001f);
    p->flat_z = rt_backtap_env_float("P2_BACKTAP_FLAT", p->flat_z, -2);
    p->pre_rms_max = rt_backtap_env_float("P2_BACKTAP_AGITATION", p->pre_rms_max, 0.001f);
    p->gravity_max = rt_backtap_env_float("P2_BACKTAP_GRAVITY", p->gravity_max, 0.001f);
    p->lateral_sign = rt_backtap_env_sign("P2_BACKTAP_LATERAL", p->lateral_sign);
    p->z_sign = rt_backtap_env_sign("P2_BACKTAP_ZSIGN", p->z_sign);
    p->invert = rt_backtap_env_sign("P2_BACKTAP_INVERT", 0) > 0;
}

void rt_backtap_init(rt_backtap *d, const rt_backtap_params *p) {
    memset(d, 0, sizeof(*d));
    d->p = *p;
    d->refractory_until = -1e9;
    d->last_candidates[0] = d->last_candidates[1] = -1e9;
    d->last_inactive = -1e9;
}

void rt_backtap_touch(rt_backtap *d, double t) {
    d->touches[d->ntouch++ % RT_BACKTAP_TOUCHES] = t;
}

static float norm3(const float *v) { return sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]); }
static double delay_of(const rt_backtap_params *p) { return fmax(0.100, p->touch_guard + 0.020); }
static const rt_backtap_sample *at(const rt_backtap *d, unsigned long i) { return &d->ring[i % RT_BACKTAP_RING]; }

static void judge(rt_backtap *d, int orient, int active, rt_backtap_report *r) {
    const rt_backtap_params *p = &d->p;
    double t0 = d->onset, delay = delay_of(p);
    unsigned long first = d->count > RT_BACKTAP_RING ? d->count - RT_BACKTAP_RING : 0, end = d->count;
    memset(r, 0, sizeof(*r));
    r->t = t0; r->touch_dt = 1e9;

    // Avant le pic : gravité (dernier échantillon avant t0 - 15 ms, et 200 ms
    // plus tôt), puis, en remontant jusqu'à 600 ms, 300 ms d'agitation et de
    // rotation moyenne hors de la vibration des deux candidats d'avant.
    double base[3] = {0, 0, 0}, sq = 0, span = 0, tail_sq = 0, prev_in = 0; int n = 0, n_tail = 0;
    const rt_backtap_sample *g_now = NULL, *g_then = NULL;
    unsigned long onset = end;   // indice du premier échantillon du pic
    for (unsigned long i = first; i < end; ++i) {
        const rt_backtap_sample *s = at(d, i);
        if (s->t >= t0) { onset = i; break; }
        if (s->t > t0 - kPreGap) continue;
        g_now = s;
        if (!g_then || s->t <= t0 - kPreGap - kGravityLag) g_then = s;
    }
    for (unsigned long i = onset; i-- > first;) {
        const rt_backtap_sample *s = at(d, i);
        if (s->t > t0 - kPreGap) continue;
        if (s->t < t0 - kPreMax || span >= kPre) break;
        if (s->t >= t0 - kPreGap - kTail) { tail_sq += (double)norm3(s->a) * norm3(s->a); ++n_tail; }
        int ringing = 0;   // rebonds d'un candidat d'avant
        for (int k = 0; k < 2; ++k) ringing |= s->t >= d->last_candidates[k] && s->t < d->last_candidates[k] + kRinging;
        if (ringing) continue;
        if (prev_in > 0) span += fmin(prev_in - s->t, 0.02);
        prev_in = s->t;
        float m = norm3(s->a);
        sq += (double)m * m;
        for (int k = 0; k < 3; ++k) base[k] += s->w[k];
        ++n;
    }
    if (n) for (int k = 0; k < 3; ++k) base[k] /= n;
    r->pre_rms = n ? (float)sqrt(sq / n) : 0;
    r->tail_rms = n_tail ? (float)sqrt(tail_sq / n_tail) : 0;
    if (g_now && g_then) {
        float dg[3] = {g_now->g[0] - g_then->g[0], g_now->g[1] - g_then->g[1], g_now->g[2] - g_then->g[2]};
        r->gravity = norm3(dg);
    }
    if (g_now) memcpy(r->grav, g_now->g, sizeof(r->grav));

    // Pic (40 ms) : amplitude, secousse, rotation ; la rotation peut précéder
    // l'accélération d'un échantillon.
    float onset_az = onset < end ? at(d, onset)->a[2] : 0;
    for (unsigned long i = onset > first ? onset - 1 : onset; i < end; ++i) {
        const rt_backtap_sample *s = at(d, i);
        if (s->t > t0 + kWindow) break;
        for (int k = 0; k < 3; ++k) {
            float v = s->w[k] - (float)base[k];
            if (fabsf(v) > fabsf(r->rot[k])) r->rot[k] = v;
        }
        if (i < onset) continue;
        float m = norm3(s->a);
        if (m > r->peak) { r->peak = m; memcpy(r->acc, s->a, sizeof(r->acc)); r->rise = (float)(s->t - t0); }
        const rt_backtap_sample *q = i > first ? at(d, i - 1) : NULL;
        if (q && s->t > q->t) {
            float da[3] = {s->a[0] - q->a[0], s->a[1] - q->a[1], s->a[2] - q->a[2]};
            float j = norm3(da) / (float)(s->t - q->t);
            if (j > r->jerk) r->jerk = j;
        }
    }
    // Durée : au-dessus de max(accel_min, 35 % du pic), jusqu'au jugement.
    float settle = fmaxf(p->accel_min, 0.35f * r->peak);
    double above_first = -1, above_last = -1;
    for (unsigned long i = onset; i < end; ++i) {
        const rt_backtap_sample *s = at(d, i);
        if (s->t > t0 + delay) break;
        if (norm3(s->a) >= settle) { if (above_first < 0) above_first = s->t; above_last = s->t; }
    }
    r->width = above_first < 0 ? 0 : (float)(above_last - above_first);

    for (unsigned i = 0; i < RT_BACKTAP_TOUCHES && i < d->ntouch; ++i) {
        double dt = d->touches[i] - t0;
        if (fabs(dt) < fabs(r->touch_dt)) r->touch_dt = dt;
    }

    // Orientation : écran d'abord, gravité à défaut.
    r->orient = orient;
    if (!r->orient && g_now && fabsf(g_now->g[0]) >= 0.3f) r->orient = g_now->g[0] > 0 ? 1 : -1;

    // Côté, dans le repère de l'appareil (+1 : vers +y), puis de l'écran.
    // Rotation autour de x (couple de la poussée) pondérée par side_x_weight,
    // et autour de z, rapportée à l'écran : la main qui tape fait pivoter le
    // téléphone dans son plan, vers la gauche (z > 0) ou la droite, quelle que
    // soit l'orientation. Sur 79 tapes étiquetées (26/10) : x seul 72, z seul
    // 79, 0,5 x + z 79 ; x seul se trompait sur les tapes près d'un coin.
    // Écarts maximaux : l'intégrale du premier lobe ou l'écart à la rotation
    // juste avant l'impact inversaient des séries entières.
    r->side_score = p->side_x_weight * r->rot[0] - (float)r->orient * r->rot[2];
    if (fabsf(r->side_score) >= p->rot_min) {
        r->side = r->side_score > 0 ? 1 : -1; r->method = "rotation";
    } else if (p->lateral_sign && r->peak > 0 && fabsf(r->acc[1]) >= p->lateral_ratio * r->peak) {
        // Poussée physique vers +y (a_y CoreMotion < 0) : tape venue du côté -y.
        r->side = (r->acc[1] > 0 ? 1 : -1) * p->lateral_sign; r->method = "latéral";
    }
    r->side *= r->orient * (p->invert ? -1 : 1);

    int by_accel = r->peak >= p->accel_min && r->jerk >= p->jerk_min;
    int by_gyro = r->peak >= p->gyro_accel_min && r->jerk >= p->jerk_min && fabsf(r->rot[0]) >= p->gyro_rot_min &&
                  r->tail_rms <= p->gyro_calm_max;
    r->weak = !by_accel && !by_gyro;
    r->gyro = !by_accel && by_gyro;
    if (r->weak) r->reject = "sous les seuils";
    // En menu jusqu'à touch_guard avant le pic : le clic qui reprend la partie
    // (toucher passé à SDL, inconnu ici) secoue aussi le téléphone.
    else if (!active || d->last_inactive >= t0 - p->touch_guard) r->reject = "menu";
    else if (fabs(r->touch_dt) <= p->touch_guard) r->reject = "toucher à l'écran";
    else if (n < 3 || span < kMinHistory) r->reject = "historique insuffisant";
    else if (r->pre_rms > p->pre_rms_max) r->reject = "agitation avant le pic";
    else if (d->last_candidates[0] + kRinging > t0 - kPreGap - kTail && r->tail_rms > fmaxf(p->pre_rms_max, kDecay * d->last_peak))
        r->reject = "agitation avant le pic";   // vibration de la tape d'avant qui ne retombe pas : secousse
    else if (r->gravity > p->gravity_max) r->reject = "gravité qui change";
    else if (g_now && g_now->g[2] < p->flat_z) r->reject = "téléphone à plat";
    else if (r->width > p->max_width) r->reject = "pic trop long";
    else if (p->z_sign && (onset_az > 0 ? 1 : -1) != p->z_sign) r->reject = "sens z";
    else if (!r->orient) r->reject = "orientation inconnue";
    else if (!r->side) r->reject = "côté indécis";
    if (r->reject) r->side = 0;
}

int rt_backtap_feed(rt_backtap *d, const rt_backtap_sample *s, int orient, int active, rt_backtap_report *r) {
    d->ring[d->count++ % RT_BACKTAP_RING] = *s;
    if (!active) d->last_inactive = s->t;
    if (d->count < 2) return 0;
    // Début d'un candidat dès onset_factor des seuils (il cerne mieux le pic) ;
    // un candidat faible cède la place au premier échantillon qui atteint les
    // seuils entiers avant son jugement : une tape qui suit de 40 à 100 ms un
    // frémissement n'est pas perdue.
    if (!d->onset || !d->onset_strong) {
        const rt_backtap_sample *prev = at(d, d->count - 2);
        double dt = s->t - prev->t;
        if (s->t >= d->refractory_until && dt > 0 && dt <= 0.1) {
            float da[3] = {s->a[0] - prev->a[0], s->a[1] - prev->a[1], s->a[2] - prev->a[2]};
            float m = norm3(s->a), jerk = norm3(da) / (float)dt, wjerk = fabsf(s->w[0] - prev->w[0]) / (float)dt;
            const rt_backtap_params *p = &d->p;
            int strong = (m >= p->accel_min && jerk >= p->jerk_min) || (m >= p->gyro_accel_min && wjerk >= p->gyro_jerk_min);
            float f = p->onset_factor;
            if (strong && (!d->onset || s->t > d->onset + kWindow)) { d->onset = s->t; d->onset_strong = 1; }
            else if (strong) d->onset_strong = 1;   // dans la fenêtre du pic : le candidat le couvre déjà
            else if (!d->onset && ((m >= p->accel_min * f && jerk >= p->jerk_min * f) ||
                                   (m >= p->gyro_accel_min * f && wjerk >= p->gyro_jerk_min * f)))
                d->onset = s->t;
        }
        if (!d->onset) return 0;
    }
    if (s->t < d->onset + delay_of(&d->p)) return 0;
    judge(d, orient, active, r);
    // Même rejeté (faute d'historique par exemple), un candidat fort fait
    // vibrer le téléphone : sa vibration est exclue de l'agitation du suivant.
    if (!r->weak) {
        d->refractory_until = d->onset + d->p.refractory;
        d->last_candidates[1] = d->last_candidates[0]; d->last_candidates[0] = d->onset; d->last_peak = r->peak;
    }
    d->onset = 0; d->onset_strong = 0;
    return 1;
}
