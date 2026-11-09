// Mode d'essai des commandes tactiles (simulateur) : voir touch_test.h.
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE && TARGET_OS_SIMULATOR
#include <SDL.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "env_ios.h"
#include "touch_test.h"

static const char *plan(void) {
    static const char *value; static int parsed;
    if (!parsed) { parsed = 1; value = getenv("P2_TOUCH_TEST_MOVE"); if (value && !*value) value = NULL; }
    return value;
}
int rt_ios_touch_test_active(void) { return plan() != NULL; }
int rt_ios_touch_test_wasd(void) { return getenv("P2_TOUCH_TEST_WASD") != NULL; }
double rt_ios_touch_test_angle(void) {
    static double angle = -1;
    if (angle < 0) angle = rt_ios_env_double("P2_TOUCH_TEST_ANGLE", 0, -360) * M_PI / 180;
    return angle;
}

int rt_ios_touch_test_click(int menu, double *x, double *y) {
    static double at = -1, cx, cy; static Uint64 when; static int step;
    if (at == -1) { const char *e = getenv("P2_TOUCH_TEST_CLICK");
        if (!e || sscanf(e, "%lf,%lf,%lf", &at, &cx, &cy) != 3 || at <= 0) at = -2; }
    if (at <= 0 || !menu) return 0;
    Uint64 now = SDL_GetTicks64();
    if (!when) { when = now + (Uint64)(at * 1000); return 0; }
    if (now < when) return 0;
    *x = cx; *y = cy;
    int result = step + 1;   // survol, puis appui et relâchement sur des images distinctes
    if (step == 2) at = -2;
    fprintf(stderr, "[ios] essai clic %d à (%.0f, %.0f), %llu ms\n", step, cx, cy, (unsigned long long)now);
    ++step; when = now + 300;
    return result;
}

int rt_ios_touch_test_step(int menu, int ready, double *move) {
    static double seconds, delay, current; static int parsed, phase; static Uint64 next;
    *move = current;
    const char *p = plan();
    if (!p) return 0;
    if (!parsed) {
        parsed = 1;
        seconds = rt_ios_env_double("P2_TOUCH_TEST_SECONDS", 2, 1e-9);
        delay = rt_ios_env_double("P2_TOUCH_TEST_DELAY", 20, 0);
    }
    Uint64 now = SDL_GetTicks64();
    if (menu || !ready) { next = 0; return 0; }   // en partie, manette prête
    if (!next) { next = now + (Uint64)(delay * 1000); phase = 0; return 0; }
    if (now < next) return 0;
    // Étapes paires : début d'inclinaison ; impaires : fin et repos.
    const char *at = p;
    for (int i = 0; i < phase / 2 && at; ++i) { at = strchr(at, ','); if (at) ++at; }
    if (!at || !*at) { if (current) { current = 0; *move = 0; return 1; } return 0; }
    int start = phase % 2 == 0;
    current = start ? atof(at) : 0;
    fprintf(stderr, "[ios] essai stick : %s inclinaison %.2f à %llu ms\n", start ? "début" : "fin", atof(at),
            (unsigned long long)now);
    next = now + (Uint64)((start ? seconds : 3.0) * 1000);
    ++phase;
    *move = current;
    return 1;
}
#endif
