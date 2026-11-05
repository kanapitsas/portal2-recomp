// Rejoue un enregistrement brut des tapes au dos (P2_BACKTAP_RAW sur l'iPhone)
// dans le détecteur de runtime/ios/back_tap.c, pour régler les seuils sur de
// vraies données. Mêmes variables d'environnement que le jeu (P2_BACKTAP_ACCEL,
// P2_BACKTAP_AGITATION, ...) ; P2_BACKTAP_LOG=1 montre aussi les candidats faibles.
//
//     cc -O2 -o build/backtap-replay tools/backtap_replay.c -lm
//     build/backtap-replay backtap.raw
#include "../runtime/ios/back_tap.c"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    if (argc != 2) { fprintf(stderr, "usage : %s <enregistrement>\n", argv[0]); return 2; }
    FILE *f = fopen(argv[1], "r");
    if (!f) { perror(argv[1]); return 1; }
    rt_backtap_params p;
    rt_backtap_params_from_env(&p);   // mêmes variables et bornes que touch_controls.m
    int log = getenv("P2_BACKTAP_LOG") && *getenv("P2_BACKTAP_LOG") != '0';
    static rt_backtap d;
    rt_backtap_init(&d, &p);
    char line[512];
    double origin = 0;
    int taps[2] = {0, 0}, rejects = 0;
    while (fgets(line, sizeof(line), f)) {
        rt_backtap_sample s; int orient, active; double t;
        if (line[0] == 't' && sscanf(line, "t %lf", &t) == 1) { rt_backtap_touch(&d, t); continue; }
        if (line[0] != 'm' || sscanf(line, "m %lf %f %f %f %f %f %f %f %f %f %d %d", &s.t, &s.a[0], &s.a[1], &s.a[2], &s.w[0],
                                     &s.w[1], &s.w[2], &s.g[0], &s.g[1], &s.g[2], &orient, &active) != 12) continue;
        if (!origin) origin = s.t;
        rt_backtap_report r;
        if (!rt_backtap_feed(&d, &s, orient, active, &r) || (r.weak && !log)) continue;
        if (!r.reject) ++taps[r.side > 0];
        else ++rejects;
        char touch[16]; if (fabs(r.touch_dt) < 10) snprintf(touch, sizeof(touch), "%+.0fms", r.touch_dt * 1000); else snprintf(touch, sizeof(touch), "aucun");
        printf("%8.2f s  pic %.2f g  secousse %3.0f  rot x %+.2f z %+.2f  o %+d  montée %2.0f ms  avant %.3f  fin %.3f  grav z %+.2f  toucher %s  -> %s\n",
               r.t - origin, r.peak, r.jerk, r.rot[0], r.rot[2], r.orient, r.rise * 1000, r.pre_rms, r.tail_rms, r.grav[2],
               touch,
               r.reject ? r.reject : r.side < 0 ? (r.gyro ? "GAUCHE (gyro)" : "GAUCHE") : (r.gyro ? "DROITE (gyro)" : "DROITE"));
    }
    printf("%d gauche, %d droite, %d rejets\n", taps[0], taps[1], rejects);
    return 0;
}
