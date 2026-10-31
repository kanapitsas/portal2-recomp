// Rejoue un enregistrement brut des tapes au dos (P2_BACKTAP_RAW sur l'iPhone)
// dans le détecteur de runtime/ios/back_tap.c, pour régler les seuils sur de
// vraies données. Mêmes variables d'environnement que le jeu (P2_BACKTAP_ACCEL,
// P2_BACKTAP_AGITATION, ...) ; P2_BACKTAP_LOG=1 montre aussi les candidats faibles.
// P2_BACKTAP_DATASET=<fichier.csv> écrit le jeu de données à étiquettes
// devinées (runtime/ios/backtap_dataset.c) avec le détecteur large
// (P2_BACKTAP_WIDE, quart des seuils par défaut), comme le jeu ; sans
// caméra dans l'enregistrement, la règle « mauvais côté » ne la regarde pas.
//
//     cc -O2 -o build/backtap-replay tools/backtap_replay.c -lm
//     build/backtap-replay backtap.raw
#include "../runtime/ios/back_tap.c"
#include "../runtime/ios/backtap_dataset.c"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

int main(int argc, char **argv) {
    if (argc != 2) { fprintf(stderr, "usage : %s <enregistrement>\n", argv[0]); return 2; }
    FILE *f = fopen(argv[1], "r");
    if (!f) { perror(argv[1]); return 1; }
    rt_backtap_params p;
    rt_backtap_params_from_env(&p);   // mêmes variables et bornes que touch_controls.m
    int log = getenv("P2_BACKTAP_LOG") && *getenv("P2_BACKTAP_LOG") != '0';
    static rt_backtap d, wide;
    rt_backtap_init(&d, &p);
    rt_backtap_params pw = p;
    pw.onset_factor = env("P2_BACKTAP_WIDE", RT_BTDS_WIDE_ONSET);
    rt_backtap_init(&wide, &pw);
    static rt_btds ds;
    const char *dataset = getenv("P2_BACKTAP_DATASET");
    struct stat in_st, out_st;   // ne jamais tronquer l'enregistrement lu
    if (dataset && *dataset && !fstat(fileno(f), &in_st) && !stat(dataset, &out_st) && in_st.st_dev == out_st.st_dev &&
        in_st.st_ino == out_st.st_ino) {
        fprintf(stderr, "%s : c'est l'enregistrement lu\n", dataset); return 1;
    }
    FILE *out = dataset && *dataset ? fopen(dataset, "w") : NULL;
    if (dataset && *dataset && !out) { perror(dataset); return 1; }
    if (out) fputs(rt_btds_header, out);
    rt_btds_init(&ds, &p, out, getenv("P2_BACKTAP_TAG"), 0, 0, 0);
    char line[512];
    double origin = 0, played = 0, last_t = 0;
    int taps[2] = {0, 0}, rejects = 0;
    while (fgets(line, sizeof(line), f)) {
        rt_backtap_sample s; int orient, active; double t;
        if (line[0] == 't' && sscanf(line, "t %lf", &t) == 1) { rt_backtap_touch(&d, t); rt_backtap_touch(&wide, t); continue; }
        if (line[0] != 'm' || sscanf(line, "m %lf %f %f %f %f %f %f %f %f %f %d %d", &s.t, &s.a[0], &s.a[1], &s.a[2], &s.w[0],
                                     &s.w[1], &s.w[2], &s.g[0], &s.g[1], &s.g[2], &orient, &active) != 12) continue;
        if (!origin) { origin = s.t; ds.session = s.t; }
        if (active && last_t && s.t - last_t < 0.1) played += s.t - last_t;
        last_t = s.t;
        rt_backtap_report r, rw;
        if (rt_backtap_feed(&wide, &s, orient, active, &rw)) rt_btds_candidate_add(&ds, &wide, &rw, NAN);
        int judged = rt_backtap_feed(&d, &s, orient, active, &r);
        if (judged && !r.reject) rt_btds_fire_add(&ds, r.t, r.side, NAN);
        rt_btds_tick(&ds, s.t);
        if (!judged || (r.weak && !log)) continue;
        if (!r.reject) ++taps[r.side > 0];
        else ++rejects;
        char touch[16]; if (fabs(r.touch_dt) < 10) snprintf(touch, sizeof(touch), "%+.0fms", r.touch_dt * 1000); else snprintf(touch, sizeof(touch), "aucun");
        printf("%8.2f s  pic %.2f g  secousse %3.0f  rot x %+.2f z %+.2f  o %+d  montée %2.0f ms  avant %.3f  fin %.3f  grav z %+.2f  toucher %s  -> %s\n",
               r.t - origin, r.peak, r.jerk, r.rot[0], r.rot[2], r.orient, r.rise * 1000, r.pre_rms, r.tail_rms, r.grav[2],
               touch,
               r.reject ? r.reject : r.side < 0 ? (r.gyro ? "GAUCHE (gyro)" : "GAUCHE") : (r.gyro ? "DROITE (gyro)" : "DROITE"));
    }
    printf("%d gauche, %d droite, %d rejets\n", taps[0], taps[1], rejects);
    rt_btds_flush(&ds);
    if (out) fclose(out);
    long all = 0; for (int i = 0; i < RT_BTDS_LABELS; ++i) all += ds.by_label[i];
    printf("jeu de données : %ld exemples en %.0f s de jeu, %ld tapes dont %ld corrections", all, played, ds.taps, ds.corrections);
    for (int i = 0; i < RT_BTDS_LABELS; ++i) printf(", %s %ld", rt_btds_label_names[i], ds.by_label[i]);
    printf("\n");
    return 0;
}
