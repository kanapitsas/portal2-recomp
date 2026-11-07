// Dispositions des commandes tactiles (voir touch_layout.h).
#include "touch_layout.h"
#include <stddef.h>

// Tailles de touch_controls.m : bouton 54 pt, portail 60 pt, petits boutons
// des coins aux trois quarts (40,5 pt), à 0,45 bouton du bord.
#define B 54.0f
#define SMALL (B * .75f)
#define EDGE (B * .45f)

// Portal 2 : la disposition d'origine, inchangée.
static const rt_touch_button portal2[] = {
    // Portails, en miroir sous le compteur d'images : bleu à l'extérieur, orange à l'intérieur.
    {RT_TOUCH_FIRE, NULL, RT_TOUCH_BLUE, .06f, .19f, 0, 0, 60, RT_TOUCH_PORTAL},
    {RT_TOUCH_ALT_FIRE, NULL, RT_TOUCH_ORANGE, .06f, .19f, 60 * 1.15f, 0, 60, RT_TOUCH_PORTAL},
    {RT_TOUCH_FIRE, NULL, RT_TOUCH_BLUE, .94f, .19f, 0, 0, 60, RT_TOUCH_PORTAL},
    {RT_TOUCH_ALT_FIRE, NULL, RT_TOUCH_ORANGE, .94f, .19f, -60 * 1.15f, 0, 60, RT_TOUCH_PORTAL},
    {RT_TOUCH_JUMP, "arrow.up", RT_TOUCH_INK, .94f, .50f, 0, 0, B, 0},
    {RT_TOUCH_CROUCH, "arrow.down", RT_TOUCH_INK, .80f, .40f, 0, 0, B, 0},
    {RT_TOUCH_USE, "hand.point.up.left.fill", RT_TOUCH_INK, .94f, .80f, 0, 0, B, 0},
    {RT_TOUCH_PAUSE, "pause.fill", RT_TOUCH_INK, 1, 0, -EDGE, EDGE, SMALL, 0},
    // Signaler une erreur des tapes au dos (jeu de données), à gauche de la pause.
    {RT_TOUCH_FLAG_ERROR, "xmark", RT_TOUCH_FLAG, 1, 0, -B * 1.35f, EDGE, SMALL, RT_TOUCH_DATASET},
    // Mode de la caméra, dans le coin en bas à gauche (icône selon le mode).
    {RT_TOUCH_LOOK_MODE, "r.joystick.fill", RT_TOUCH_INK, 0, 1, EDGE, -EDGE, SMALL, 0},
};

// Half-Life 2 : sept commandes de jeu de la taille des boutons, réparties
// entre les deux pouces, le reste en petits boutons dans la barre du haut.
// - Pouce droit, en arc autour du tir (le plus gros, coin bas droit) : tir
//   secondaire, rechargement, saut, accroupi, utiliser. Les deux tirs suivent
//   le doigt : glisser en tirant vise, comme sur le trackpad.
// - Pouce gauche : le stick, et le sprint juste au-dessus (bascule : un appui
//   lance la course, elle s'arrête au relâchement du stick ou au second appui).
// - Accroupi en bascule aussi : un pouce ne peut pas tenir Ctrl tout en
//   visant ou en marchant (conduits, saut accroupi : sauter pendant qu'il est allumé).
// - Haut, petits : armes précédente et suivante au centre, sous la liste des
//   armes que le jeu y affiche (le tir choisit) ; lampe, sauvegarde rapide et
//   pause à droite ; mode de la caméra en haut à gauche (en bas, il cacherait
//   la santé du HUD).
// Décalages à partir du bord droit : la grappe garde sa forme sur tous les
// écrans (de l'iPhone SE à l'iPad).
static const rt_touch_button hl2[] = {
    {RT_TOUCH_FIRE, "scope", RT_TOUCH_RED, 1, .64f, -62, 0, 72, RT_TOUCH_AIM},
    {RT_TOUCH_ALT_FIRE, "burst.fill", RT_TOUCH_INK, 1, .60f, -150, 0, 50, RT_TOUCH_AIM},
    {RT_TOUCH_RELOAD, "arrow.clockwise", RT_TOUCH_INK, 1, .36f, -110, 0, 46, 0},
    {RT_TOUCH_JUMP, "arrow.up", RT_TOUCH_INK, 1, .36f, -34, 0, B, 0},
    {RT_TOUCH_CROUCH, "arrow.down", RT_TOUCH_INK, 1, .88f, -62, 0, 50, RT_TOUCH_TOGGLE},
    {RT_TOUCH_USE, "hand.point.up.left.fill", RT_TOUCH_INK, 1, .86f, -140, 0, 50, 0},
    {RT_TOUCH_SPRINT, "figure.run", RT_TOUCH_INK, .07f, .42f, 0, 0, 50, RT_TOUCH_TOGGLE},
    {RT_TOUCH_WEAPON_PREV, "chevron.left", RT_TOUCH_INK, .5f, 0, -30, EDGE, SMALL, 0},
    {RT_TOUCH_WEAPON_NEXT, "chevron.right", RT_TOUCH_INK, .5f, 0, 30, EDGE, SMALL, 0},
    {RT_TOUCH_FLASHLIGHT, "flashlight.on.fill", RT_TOUCH_INK, 1, 0, -B * 2.25f, EDGE, SMALL, 0},
    {RT_TOUCH_QUICKSAVE, "tray.and.arrow.down.fill", RT_TOUCH_INK, 1, 0, -B * 1.35f, EDGE, SMALL, 0},
    {RT_TOUCH_PAUSE, "pause.fill", RT_TOUCH_INK, 1, 0, -EDGE, EDGE, SMALL, 0},
    // En haut à gauche, après le compteur d'images : le coin bas gauche
    // cacherait la santé du HUD.
    {RT_TOUCH_LOOK_MODE, "r.joystick.fill", RT_TOUCH_INK, 0, 0, 170, EDGE, SMALL, 0},
};

const rt_touch_button *rt_touch_layout(rt_ios_layout layout, int *count) {
    if (layout == RT_IOS_LAYOUT_HL2) { *count = (int)(sizeof(hl2) / sizeof(*hl2)); return hl2; }
    *count = (int)(sizeof(portal2) / sizeof(*portal2));
    return portal2;
}

double rt_touch_look_rest(rt_ios_layout layout) { return layout == RT_IOS_LAYOUT_HL2 ? .64 : .80; }

void rt_touch_place(const rt_touch_button *b, double x, double y, double w, double h, double *cx, double *cy) {
    *cx = x + w * b->ax + b->ox;
    *cy = y + h * b->ay + b->oy;
}
