// Jeu du build iOS (runtime/ios/game_ios.c) et dispositions des commandes
// tactiles (runtime/ios/touch_layout.c) : profil choisi par la clé P2Game,
// boutons dans la zone sûre, sans chevauchement, hors du coin du compteur
// d'images et du stick gauche, Portal 2 inchangé.
#include "checks.h"
#include <math.h>
#include <string.h>
#include "../runtime/ios/game_ios.c"
#include "../runtime/ios/touch_layout.c"

RT_CHECK(ios_game_profile_from_plist_key) {
    const rt_ios_game *hl2 = rt_ios_game_named("hl2"), *p2 = rt_ios_game_named("portal2");
    assert(!strcmp(hl2->mod, "hl2_complete") && !strcmp(hl2->app_id, "220"));
    assert(!strcmp(hl2->userdata, "hl2-userdata") && hl2->layout == RT_IOS_LAYOUT_HL2);
    assert(!hl2->back_taps && !strcmp(hl2->cvar_interface, "VEngineCvar004") && hl2->find_command_base == 10);
    // Portal 2 : dossier des installations existantes, tapes au dos, ICvar connu.
    assert(!strcmp(p2->userdata, "portal2-userdata") && !strcmp(p2->app_id, "620") && p2->back_taps);
    assert(!strcmp(p2->cvar_interface, "VEngineCvar007") && p2->find_command_base == 13);
    // Builds d'avant la clé P2Game, ou valeur inconnue : Portal 2.
    assert(rt_ios_game_named(NULL) == p2 && rt_ios_game_named("") == p2 && rt_ios_game_named("ep2") == p2);
    assert(rt_ios_current_game() == p2);   // hors iOS
}

// Zones sûres en paysage (points) : iPhone SE, iPhone 15 Pro Max, iPad 11".
static const struct { double x, y, w, h; } areas[] = {
    {0, 0, 667, 375}, {59, 0, 932 - 118, 430 - 21}, {0, 0, 1194, 834 - 20}};

RT_CHECK(ios_touch_layout_fits_the_screen) {
    for (int layout = RT_IOS_LAYOUT_PORTAL2; layout <= RT_IOS_LAYOUT_HL2; ++layout) {
        int n;
        const rt_touch_button *b = rt_touch_layout((rt_ios_layout)layout, &n);
        assert(n > 0 && n <= 16);   // MAX_BUTTONS de touch_controls.m
        for (size_t a = 0; a < sizeof(areas) / sizeof(*areas); ++a) {
            double x = areas[a].x, y = areas[a].y, w = areas[a].w, h = areas[a].h;
            // Coin du compteur d'images (fpsCorner de touch_controls.m, « OPENGL »/« 000 FPS ») :
            // un toucher y bascule le compteur avant tout bouton.
            double corner_w = x + 8 + (7 * 6 - 1) * 2 + 8 + 20, corner_h = y + 8 + 16 * 2 + 8 + 20;
            // Stick gauche au repos (restingStick: et stickRadius de touch_controls.m).
            double stick_x = x + w * .16, stick_y = y + h * .72, stick_r = fmin(66, h * .15);
            double look_x = x + w * rt_touch_look_rest((rt_ios_layout)layout);
            for (int i = 0; i < n; ++i) {
                double cx, cy, r = b[i].size / 2;
                rt_touch_place(&b[i], x, y, w, h, &cx, &cy);
                assert(cx - r >= x - .5 && cx + r <= x + w + .5 && cy - r >= y - .5 && cy + r <= y + h + .5);
                assert(!(cx < corner_w && cy < corner_h));
                if (layout == RT_IOS_LAYOUT_HL2) assert(!(cx - r < corner_w && cy - r < corner_h));   // pas même le bord
                assert(hypot(cx - stick_x, cy - stick_y) > stick_r + r);
                // HL2 : aucun bouton sur le stick de la caméra au repos (Portal 2 :
                // le saut et l'accroupi y touchent, disposition d'origine).
                if (layout == RT_IOS_LAYOUT_HL2) assert(hypot(cx - look_x, cy - stick_y) > stick_r + r);
                for (int j = 0; j < i; ++j) {
                    // Portal 2 sur iPhone SE : le portail bleu de droite touche la
                    // pause (disposition d'origine ; ses boutons ne s'affichent
                    // que sans tapes au dos).
                    if (layout == RT_IOS_LAYOUT_PORTAL2 && a == 0 && ((b[i].flags | b[j].flags) & RT_TOUCH_PORTAL)) continue;
                    double dx, dy;
                    rt_touch_place(&b[j], x, y, w, h, &dx, &dy);
                    assert(hypot(cx - dx, cy - dy) >= r + b[j].size / 2 + 4);
                }
            }
        }
    }
}

RT_CHECK(ios_touch_layout_hl2_has_every_command) {
    int n, seen[RT_TOUCH_ACTION_COUNT] = {0};
    const rt_touch_button *b = rt_touch_layout(RT_IOS_LAYOUT_HL2, &n);
    for (int i = 0; i < n; ++i) {
        ++seen[b[i].action];
        assert(b[i].symbol && !(b[i].flags & (RT_TOUCH_PORTAL | RT_TOUCH_DATASET)));   // tout est toujours affiché
    }
    static const rt_touch_action wanted[] = {
        RT_TOUCH_FIRE, RT_TOUCH_ALT_FIRE, RT_TOUCH_RELOAD, RT_TOUCH_USE, RT_TOUCH_JUMP, RT_TOUCH_CROUCH,
        RT_TOUCH_SPRINT, RT_TOUCH_FLASHLIGHT, RT_TOUCH_WEAPON_PREV, RT_TOUCH_WEAPON_NEXT, RT_TOUCH_PAUSE,
        RT_TOUCH_QUICKSAVE, RT_TOUCH_LOOK_MODE};
    for (size_t i = 0; i < sizeof(wanted) / sizeof(*wanted); ++i) assert(seen[wanted[i]] == 1);
    assert(!seen[RT_TOUCH_FLAG_ERROR]);
    // Sprint et accroupi en bascule ; les tirs visent en glissant.
    for (int i = 0; i < n; ++i) {
        int toggle = b[i].action == RT_TOUCH_SPRINT || b[i].action == RT_TOUCH_CROUCH;
        int aim = b[i].action == RT_TOUCH_FIRE || b[i].action == RT_TOUCH_ALT_FIRE;
        assert(!!(b[i].flags & RT_TOUCH_TOGGLE) == toggle && !!(b[i].flags & RT_TOUCH_AIM) == aim);
    }
}

// Portal 2 : mêmes centres que les formules d'avant les dispositions par jeu.
RT_CHECK(ios_touch_layout_portal2_unchanged) {
    int n;
    const rt_touch_button *b = rt_touch_layout(RT_IOS_LAYOUT_PORTAL2, &n);
    double x = 59, y = 0, w = 814, h = 409, k = 54, top = y + h * .19, gap = 60 * 1.15;
    const struct { rt_touch_action action; double cx, cy, size; unsigned flags; } old[] = {
        {RT_TOUCH_FIRE, x + w * .06, top, 60, RT_TOUCH_PORTAL}, {RT_TOUCH_ALT_FIRE, x + w * .06 + gap, top, 60, RT_TOUCH_PORTAL},
        {RT_TOUCH_FIRE, x + w * .94, top, 60, RT_TOUCH_PORTAL}, {RT_TOUCH_ALT_FIRE, x + w * .94 - gap, top, 60, RT_TOUCH_PORTAL},
        {RT_TOUCH_JUMP, x + w * .94, y + h * .50, k, 0}, {RT_TOUCH_CROUCH, x + w * .80, y + h * .40, k, 0},
        {RT_TOUCH_USE, x + w * .94, y + h * .80, k, 0}, {RT_TOUCH_PAUSE, x + w - k * .45, y + k * .45, k * .75, 0},
        {RT_TOUCH_FLAG_ERROR, x + w - k * 1.35, y + k * .45, k * .75, RT_TOUCH_DATASET},
        {RT_TOUCH_LOOK_MODE, x + k * .45, y + h - k * .45, k * .75, 0}};
    assert(n == (int)(sizeof(old) / sizeof(*old)));
    for (int i = 0; i < n; ++i) {
        double cx, cy;
        rt_touch_place(&b[i], x, y, w, h, &cx, &cy);
        assert(b[i].action == old[i].action && b[i].flags == old[i].flags);
        assert(fabs(cx - old[i].cx) < .01 && fabs(cy - old[i].cy) < .01 && fabs(b[i].size - old[i].size) < .01);
    }
}
