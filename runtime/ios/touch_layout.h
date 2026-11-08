// Disposition des boutons des commandes tactiles, par jeu (touch_controls.m
// les dessine et les traduit en touches ; tests/checks_touch_layout.c vérifie
// qu'ils tiennent à l'écran sans se chevaucher).
#pragma once
#include "game_ios.h"

typedef enum {
    RT_TOUCH_JUMP,          // espace
    RT_TOUCH_CROUCH,        // Ctrl gauche
    RT_TOUCH_USE,           // E
    RT_TOUCH_PAUSE,         // Échap
    RT_TOUCH_FIRE,          // clic gauche (portail bleu, tir)
    RT_TOUCH_ALT_FIRE,      // clic droit (portail orange, tir secondaire)
    RT_TOUCH_LOOK_MODE,     // stick ou trackpad pour la caméra
    RT_TOUCH_FLAG_ERROR,    // ✗ du jeu de données des tapes au dos
    RT_TOUCH_RELOAD,        // R
    RT_TOUCH_SPRINT,        // Maj gauche (+speed)
    RT_TOUCH_FLASHLIGHT,    // F (impulse 100)
    RT_TOUCH_WEAPON_PREV,   // molette vers le haut (invprev)
    RT_TOUCH_WEAPON_NEXT,   // molette vers le bas (invnext)
    RT_TOUCH_QUICKSAVE,     // F6 (save quick)
    RT_TOUCH_ACTION_COUNT
} rt_touch_action;

typedef enum { RT_TOUCH_INK, RT_TOUCH_BLUE, RT_TOUCH_ORANGE, RT_TOUCH_RED, RT_TOUCH_FLAG } rt_touch_color;

enum {
    RT_TOUCH_PORTAL = 1,    // bouton de portail : seulement sans tapes au dos (ou P2_PORTAL_BUTTONS=1)
    RT_TOUCH_DATASET = 2,   // seulement avec le jeu de données des tapes au dos
    RT_TOUCH_TOGGLE = 4,    // un appui enfonce, le suivant relâche (allumé tant qu'enfoncé)
    RT_TOUCH_AIM = 8,       // le doigt qui le tient tourne aussi la caméra en glissant
};

// Centre : coin de la zone sûre + fraction (ax, ay) de sa taille + décalage
// (ox, oy) en points ; size : diamètre en points.
typedef struct {
    rt_touch_action action;
    const char *symbol;     // SF Symbol (NULL : disque plein, portails)
    rt_touch_color color;
    float ax, ay, ox, oy, size;
    unsigned flags;
} rt_touch_button;

// Boutons de cette disposition ; *count reçoit leur nombre.
const rt_touch_button *rt_touch_layout(rt_ios_layout layout, int *count);
// Abscisse (fraction de la zone sûre) du stick de la caméra au repos.
double rt_touch_look_rest(rt_ios_layout layout);
// Centre d'un bouton dans la zone (x, y, w, h).
void rt_touch_place(const rt_touch_button *b, double x, double y, double w, double h, double *cx, double *cy);
