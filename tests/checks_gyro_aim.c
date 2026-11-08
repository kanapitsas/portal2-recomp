// Visée au gyroscope (runtime/ios/gyro_aim.c) : sens du lacet et du tangage
// dans les deux paysages et à plat, atténuation, trous, variables.
#include "checks.h"
#include <math.h>
#include <stdlib.h>
#include "../runtime/ios/gyro_aim.c"

static rt_gyro_params gyro_test_params(void) {
    rt_gyro_params p = {1000, 0.05};
    return p;
}
static void gyro_turn(int orient, const double g[3], const double w[3], double out[2]) {
    rt_gyro_params p = gyro_test_params();
    assert(rt_gyro_delta(&p, w, g, orient, 0.01, out));
}

RT_CHECK(gyro_yaw_and_pitch_follow_the_phone_in_both_landscapes) {
    double out[2];
    // Paysage gauche, téléphone debout : y de l'appareil vers la droite de
    // l'écran, haut de l'écran = -x, gravité vers +x.
    const double left_g[3] = {1, 0, 0};
    gyro_turn(1, left_g, (double[3]){-1, 0, 0}, out);   // on tourne sur soi vers la gauche
    assert(out[0] < 0 && fabs(out[1]) < 1e-9);
    assert(fabs(out[0] + 10) < 1e-9);                  // 1 rad/s pendant 10 ms à 1000 comptes/rad
    gyro_turn(1, left_g, (double[3]){0, 1, 0}, out);    // haut de l'écran vers soi : regarder en haut
    assert(out[1] < 0 && fabs(out[0]) < 1e-9);
    // Paysage droit : tout est retourné dans le repère de l'appareil.
    const double right_g[3] = {-1, 0, 0};
    gyro_turn(-1, right_g, (double[3]){1, 0, 0}, out);  // vers la gauche
    assert(out[0] < 0);
    gyro_turn(-1, right_g, (double[3]){0, -1, 0}, out); // haut de l'écran vers soi
    assert(out[1] < 0);
    gyro_turn(-1, right_g, (double[3]){0, 1, 0}, out);  // haut de l'écran vers l'avant : regarder en bas
    assert(out[1] > 0);
}

RT_CHECK(gyro_yaw_uses_the_real_vertical) {
    double out[2];
    // À plat écran vers le ciel : tourner sur la table, c'est tourner autour de z.
    gyro_turn(1, (double[3]){0, 0, -1}, (double[3]){0, 0, 1}, out);
    assert(out[0] < 0 && fabs(out[0] + 10) < 1e-9);
    // Incliné à 45° : seule la composante autour de la verticale compte.
    double s = sqrt(0.5);
    gyro_turn(1, (double[3]){s, 0, -s}, (double[3]){-s, 0, s}, out);
    assert(fabs(out[0] + 10) < 1e-9);
}

RT_CHECK(gyro_small_rotations_are_damped_and_gaps_ignored) {
    rt_gyro_params p = gyro_test_params();
    double out[2];
    const double g[3] = {1, 0, 0};
    // Au seuil : inchangé ; à la moitié : le quart ; à l'arrêt : rien.
    assert(rt_gyro_delta(&p, (double[3]){-0.05, 0, 0}, g, 1, 1, out) == 0);   // dt trop grand : trou
    assert(rt_gyro_delta(&p, (double[3]){-0.05, 0, 0}, g, 1, 0.1, out));
    assert(fabs(out[0] + 0.05 * 0.1 * 1000) < 1e-9);
    assert(rt_gyro_delta(&p, (double[3]){-0.025, 0, 0}, g, 1, 0.1, out));
    assert(fabs(out[0] + 0.025 * 0.5 * 0.1 * 1000) < 1e-9);
    assert(!rt_gyro_delta(&p, (double[3]){0, 0, 0}, g, 1, 0.01, out));
    // Orientation inconnue, gravité pas encore estimée, dt nul : rien.
    assert(!rt_gyro_delta(&p, (double[3]){-1, 0, 0}, g, 0, 0.01, out));
    assert(!rt_gyro_delta(&p, (double[3]){-1, 0, 0}, (double[3]){0, 0, 0}, 1, 0.01, out));
    assert(!rt_gyro_delta(&p, (double[3]){-1, 0, 0}, g, 1, 0, out));
}

RT_CHECK(gyro_settings_from_environment) {
    unsetenv("P2_GYRO"); unsetenv("P2_GYRO_SPEED"); unsetenv("P2_GYRO_TIGHTEN");
    assert(rt_gyro_enabled(1) && !rt_gyro_enabled(0));
    setenv("P2_GYRO", "0", 1);
    assert(!rt_gyro_enabled(1));
    setenv("P2_GYRO", "1", 1);
    assert(rt_gyro_enabled(0));
    rt_gyro_params p;
    rt_gyro_params_from_env(&p);
    assert(fabs(p.counts_per_rad - RT_GYRO_COUNTS_PER_RAD) < 1e-6 && fabs(p.tighten - 0.05) < 1e-12);
    setenv("P2_GYRO_SPEED", "2", 1); setenv("P2_GYRO_TIGHTEN", "0", 1);
    rt_gyro_params_from_env(&p);
    assert(fabs(p.counts_per_rad - 2 * RT_GYRO_COUNTS_PER_RAD) < 1e-6 && p.tighten == 0);
    unsetenv("P2_GYRO"); unsetenv("P2_GYRO_SPEED"); unsetenv("P2_GYRO_TIGHTEN");
}
