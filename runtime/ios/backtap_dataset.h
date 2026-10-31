// Jeu de données des tapes au dos, étiqueté par ce que fait le joueur ensuite
// (étiquettes devinées, sans calibration). C portable : runtime/ios/backtap_dataset.c,
// testé sur l'hôte par tests/checks_backtap_dataset.c, nourri par
// touch_controls.m sur l'iPhone et par tools/backtap_replay.c sur le Mac.
//
// Un second détecteur, à porte d'entrée plus large que celui du jeu (quart des
// seuils au lieu de la moitié), voit aussi les tapes trop douces. Chacun de ses
// candidats devient un exemple : caractéristiques du bilan, fenêtre brute des
// capteurs, et une étiquette posée HORIZON secondes plus tard selon les tirs du
// détecteur du jeu autour de lui. Le détecteur du jeu n'est pas modifié.
#pragma once
#include <stdio.h>
#include "back_tap.h"

// Étiquettes devinées (colonne « label » du fichier).
typedef enum {
    RT_BTDS_QUIET,       // pas tiré, aucun tir à moins de HORIZON : probablement pas une tape
    RT_BTDS_AFTER_FIRE,  // pas tiré, juste après un tir : rebond de la main, probablement pas une tape
    RT_BTDS_MISSED,      // pas tiré, un tir suit de peu : probablement une tape manquée (côté du tir suivant)
    RT_BTDS_FIRED,       // tiré, rien ne le contredit : tape probable, côté retenu
    RT_BTDS_WRONG_SIDE,  // tiré, puis l'autre portail sans bouger la caméra : probablement le mauvais côté
    RT_BTDS_LABELS
} rt_btds_label;

enum { RT_BTDS_PENDING = 32, RT_BTDS_FIRES = 16, RT_BTDS_WINDOW = 96 };

typedef struct {
    rt_backtap_report r;
    double look;                       // caméra : comptes de souris cumulés à cet instant (NAN : inconnu)
    int fired;                         // côté tiré par le jeu (0 : rien)
    int nwin;
    rt_backtap_sample win[RT_BTDS_WINDOW];   // capteurs de t - 0,35 s au jugement (t + 0,10 s)
} rt_btds_candidate;

typedef struct { double t, look; int side; } rt_btds_fire;

typedef struct {
    FILE *out;                         // NULL : rien n'est écrit (comptes seulement)
    char tag[64];                      // étiquette de session (P2_BACKTAP_TAG ; virgules, guillemets et fins de ligne remplacés par « _ »)
    int write_error;                   // écriture du fichier en échec (disque plein...)
    double session;                    // identifiant de session (horloge au démarrage)
    rt_btds_candidate pending[RT_BTDS_PENDING];
    int npending;
    rt_btds_fire fires[RT_BTDS_FIRES];
    unsigned nfires;
    float accel_min, gyro_rot_min;     // seuils du détecteur du jeu (règle MISSED)
    struct { double t; float strength; } recent[RT_BTDS_PENDING];   // derniers candidats (règle MISSED)
    unsigned nrecent;
    long taps, corrections;            // exemples étiquetés tapes (avec ceux du fichier) : MISSED + FIRED + WRONG_SIDE, et corrections : MISSED + WRONG_SIDE
    long by_label[RT_BTDS_LABELS];     // exemples de cette session par étiquette
} rt_btds;

extern const char *const rt_btds_label_names[RT_BTDS_LABELS];

// Délais (s) des règles d'étiquetage, expliqués dans backtap_dataset.c.
#define RT_BTDS_HORIZON 1.5
#define RT_BTDS_MATCH 0.06

// p : seuils du détecteur du jeu ; out : fichier ouvert en ajout ; taps/corrections : comptes du fichier déjà là.
void rt_btds_init(rt_btds *ds, const rt_backtap_params *p, FILE *out, const char *tag, double session, long taps,
                  long corrections);
// Retire une dernière ligne incomplète (jeu tué pendant une écriture) ; rend 0 si tout va bien.
int rt_btds_repair_tail(const char *path);
// Compte les tapes et corrections étiquetées d'un fichier existant (NULL : 0).
void rt_btds_count_file(FILE *f, long *taps, long *corrections);
// Candidat jugé par le détecteur large d ; sa fenêtre est copiée de d->ring.
void rt_btds_candidate_add(rt_btds *ds, const rt_backtap *d, const rt_backtap_report *r, double look);
// Tir du détecteur du jeu (bilan accepté : r->t, r->side).
void rt_btds_fire_add(rt_btds *ds, double t, int side, double look);
// Étiquette et écrit les candidats plus vieux que HORIZON (now : horloge des capteurs).
void rt_btds_tick(rt_btds *ds, double now);
// Tout écrire (fin de session) : étiquettes avec ce qui est connu.
void rt_btds_flush(rt_btds *ds);
// Étiquette d'un candidat d'après les tirs connus (exposé pour les tests).
rt_btds_label rt_btds_label_of(const rt_btds *ds, const rt_btds_candidate *c, int *side);
// Ligne d'en-tête du fichier CSV (à écrire quand le fichier est neuf).
extern const char rt_btds_header[];
// Fraction des seuils qui ouvre un candidat du détecteur large (P2_BACKTAP_WIDE).
#define RT_BTDS_WIDE_ONSET 0.25f
