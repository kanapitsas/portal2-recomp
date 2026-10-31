// Jeu de données des tapes au dos à étiquettes devinées
// (runtime/ios/backtap_dataset.c, C portable, testé ici sur l'hôte).
#include "checks.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../runtime/ios/backtap_dataset.c"

static rt_backtap_params params(void) {
    rt_backtap_params p; memset(&p, 0, sizeof(p));
    p.accel_min = 0.10f; p.gyro_rot_min = 0.30f;
    return p;
}

// Candidat du détecteur large à l'instant t, d'amplitude peak (g), rejeté ou non.
static void candidate(rt_btds *ds, rt_backtap *d, double t, float peak, const char *reject, double look) {
    rt_backtap_report r; memset(&r, 0, sizeof(r));
    r.t = t; r.peak = peak; r.reject = reject; r.touch_dt = 1e9; r.orient = 1;
    rt_btds_candidate_add(ds, d, &r, look);
}

static rt_btds_label last_label(const rt_btds *ds, int *side) {
    return rt_btds_label_of(ds, &ds->pending[ds->npending - 1], side);
}

static rt_backtap ring;   // capteurs : 100 Hz de t = 99 à 101 s
static void fill_ring(void) {
    memset(&ring, 0, sizeof(ring));
    for (int i = 0; i < 200; ++i) {
        rt_backtap_sample *s = &ring.ring[ring.count++ % RT_BACKTAP_RING];
        memset(s, 0, sizeof(*s)); s->t = 99 + i * 0.01;
    }
}

// Tir puis l'autre portail 0,5 s plus tard, caméra immobile : mauvais côté ;
// caméra tournée entre les deux : deux tirs voulus.
RT_CHECK(backtap_dataset_wrong_side) {
    rt_backtap_params p = params(); rt_btds ds; fill_ring();
    for (int moved = 0; moved < 2; ++moved) {
        rt_btds_init(&ds, &p, NULL, "", 0, 0, 0);
        candidate(&ds, &ring, 100.00, 0.3f, NULL, 0);
        rt_btds_fire_add(&ds, 100.02, -1, 0);
        rt_btds_fire_add(&ds, 100.52, +1, moved ? 500 : 10);
        int side; rt_btds_label l = last_label(&ds, &side);
        if (moved) assert(l == RT_BTDS_FIRED && side == -1);
        else assert(l == RT_BTDS_WRONG_SIDE && side == +1);
    }
    // Même portail retiré : pas une correction de côté.
    rt_btds_init(&ds, &p, NULL, "", 0, 0, 0);
    candidate(&ds, &ring, 100.00, 0.3f, NULL, 0);
    rt_btds_fire_add(&ds, 100.00, -1, 0);
    rt_btds_fire_add(&ds, 100.50, -1, 0);
    int side; assert(last_label(&ds, &side) == RT_BTDS_FIRED && side == -1);
    // Caméra inconnue (rejeu d'un enregistrement brut) : la règle s'applique sans elle.
    rt_btds_init(&ds, &p, NULL, "", 0, 0, 0);
    candidate(&ds, &ring, 100.00, 0.3f, NULL, NAN);
    rt_btds_fire_add(&ds, 100.00, -1, NAN);
    rt_btds_fire_add(&ds, 100.50, +1, NAN);
    assert(last_label(&ds, &side) == RT_BTDS_WRONG_SIDE && side == +1);
}

// Candidat pas tiré suivi d'un tir : tape manquée s'il atteint la moitié des
// seuils et que le tir vient après un temps de réaction ; sinon calme.
RT_CHECK(backtap_dataset_missed_tap) {
    rt_backtap_params p = params(); rt_btds ds; fill_ring();
    struct { float peak; double dt; rt_btds_label want; } cases[] = {
        {0.06f, 0.50, RT_BTDS_MISSED},   // 0,6 × accel_min, tir 0,5 s plus tard
        {0.03f, 0.50, RT_BTDS_QUIET},    // frémissement de la main
        {0.06f, 0.10, RT_BTDS_QUIET},    // trop tôt : début du même geste
        {0.06f, 1.30, RT_BTDS_QUIET},    // trop tard
    };
    for (unsigned i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
        rt_btds_init(&ds, &p, NULL, "", 0, 0, 0);
        candidate(&ds, &ring, 100.0, cases[i].peak, "sous les seuils", 0);
        rt_btds_fire_add(&ds, 100.0 + cases[i].dt, +1, 0);
        int side; rt_btds_label l = last_label(&ds, &side);
        assert(l == cases[i].want);
        assert(side == (l == RT_BTDS_MISSED ? +1 : 0));
    }
    // Deux candidats avant le même tir : seul le plus fort est une tape manquée.
    rt_btds_init(&ds, &p, NULL, "", 0, 0, 0);
    candidate(&ds, &ring, 100.0, 0.06f, "sous les seuils", 0);
    candidate(&ds, &ring, 100.3, 0.08f, "sous les seuils", 0);
    rt_btds_fire_add(&ds, 100.8, +1, 0);
    int s0, s1;
    assert(rt_btds_label_of(&ds, &ds.pending[0], &s0) == RT_BTDS_QUIET);
    assert(rt_btds_label_of(&ds, &ds.pending[1], &s1) == RT_BTDS_MISSED && s1 == +1);
    // Un rebond fort juste après un tir ne masque pas la tape manquée qui suit.
    rt_btds_init(&ds, &p, NULL, "", 0, 0, 0);
    rt_btds_fire_add(&ds, 100.0, -1, 0);
    candidate(&ds, &ring, 100.2, 0.30f, "agitation avant le pic", 0);
    candidate(&ds, &ring, 100.5, 0.06f, "sous les seuils", 0);
    rt_btds_fire_add(&ds, 101.0, -1, 0);
    assert(rt_btds_label_of(&ds, &ds.pending[0], &s0) == RT_BTDS_AFTER_FIRE);
    assert(rt_btds_label_of(&ds, &ds.pending[1], &s1) == RT_BTDS_MISSED && s1 == -1);
    // Rotation forte sans accélération : la voie gyroscope compte aussi.
    rt_btds_init(&ds, &p, NULL, "", 0, 0, 0);
    candidate(&ds, &ring, 100.0, 0.02f, "sous les seuils", 0);
    ds.pending[0].r.rot[0] = -0.2f;
    rt_btds_fire_add(&ds, 100.5, -1, 0);
    int side; assert(last_label(&ds, &side) == RT_BTDS_MISSED && side == -1);
}

// Juste après un tir : rebond de la main, même si un autre tir suit.
RT_CHECK(backtap_dataset_after_fire) {
    rt_backtap_params p = params(); rt_btds ds; fill_ring();
    rt_btds_init(&ds, &p, NULL, "", 0, 0, 0);
    rt_btds_fire_add(&ds, 99.8, +1, 0);
    candidate(&ds, &ring, 100.0, 0.08f, "agitation avant le pic", 0);
    rt_btds_fire_add(&ds, 100.5, +1, 0);
    int side; assert(last_label(&ds, &side) == RT_BTDS_AFTER_FIRE && side == 0);
    // En menu : rien n'est gardé.
    rt_btds_init(&ds, &p, NULL, "", 0, 0, 0);
    candidate(&ds, &ring, 100.0, 0.3f, "menu", 0);
    assert(ds.npending == 0);
}

// Écriture : étiquette posée après HORIZON, fenêtre des capteurs, comptes
// relus d'un fichier (les lignes dépassent le tampon de lecture).
RT_CHECK(backtap_dataset_file_roundtrip) {
    rt_backtap_params p = params(); rt_btds ds; fill_ring();
    FILE *f = tmpfile();
    assert(f);
    fputs(rt_btds_header, f);
    rt_btds_init(&ds, &p, f, "es,sai", 1, 0, 0);   // virgule remplacée : colonnes intactes
    candidate(&ds, &ring, 100.00, 0.3f, NULL, 0);            // tiré puis corrigé : wrong_side
    rt_btds_fire_add(&ds, 100.00, -1, 0);
    rt_btds_fire_add(&ds, 100.50, +1, 0);
    candidate(&ds, &ring, 100.50, 0.3f, NULL, 0);            // le second tir : fired
    // Fenêtre : dès 0,35 s avant le pic, plafonnée (ici les capteurs vont jusqu'à 101 s).
    assert(ds.pending[0].nwin == RT_BTDS_WINDOW && fabs(ds.pending[0].win[0].t - 99.65) < 0.011);
    rt_btds_tick(&ds, 101.0);                                // trop tôt : rien d'écrit
    assert(ds.npending == 2 && ds.taps == 0);
    rt_btds_tick(&ds, 101.6);
    assert(ds.npending == 1 && ds.taps == 1 && ds.corrections == 1);
    rt_btds_flush(&ds);
    assert(ds.npending == 0 && ds.taps == 2 && ds.corrections == 1);
    assert(ds.by_label[RT_BTDS_WRONG_SIDE] == 1 && ds.by_label[RT_BTDS_FIRED] == 1);
    rewind(f);
    long taps, fixes; rt_btds_count_file(f, &taps, &fixes);
    assert(taps == 2 && fixes == 1);
    // Colonnes : autant que l'en-tête, fenêtre comprise.
    rewind(f);
    static char line[16384];
    int header_cols = 0, cols = 0;
    assert(fgets(line, sizeof(line), f));
    for (char *c = line; *c; ++c) header_cols += *c == ',';
    assert(fgets(line, sizeof(line), f));
    for (char *c = line; *c; ++c) cols += *c == ',';
    assert(cols == header_cols && strstr(line, ",es_sai,100.0000,wrong_side,1,-1,"));
    fclose(f);
}

// Jeu tué pendant une écriture : la ligne incomplète est retirée, le reste gardé.
RT_CHECK(backtap_dataset_repairs_partial_last_line) {
    char path[] = "/tmp/p2-backtap-dataset-XXXXXX";
    int fd = mkstemp(path);
    assert(fd >= 0);
    close(fd);
    FILE *f = fopen(path, "w");
    fputs("session,tag\n1,a,100,fired\n1,a,101,mis", f);
    fclose(f);
    assert(rt_btds_repair_tail(path) == 0);
    f = fopen(path, "r");
    char buf[128]; size_t n = fread(buf, 1, sizeof(buf) - 1, f); buf[n] = 0;
    fclose(f);
    assert(!strcmp(buf, "session,tag\n1,a,100,fired\n"));
    assert(rt_btds_repair_tail(path) == 0);   // déjà propre : inchangé
    f = fopen(path, "w"); fputs("sans fin de ligne", f); fclose(f);
    assert(rt_btds_repair_tail(path) == 0);
    f = fopen(path, "r"); assert(fgetc(f) == EOF); fclose(f);
    unlink(path);
    assert(rt_btds_repair_tail(path) == 0);   // absent : fichier neuf
}
