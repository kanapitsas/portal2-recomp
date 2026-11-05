// Jeu de données des tapes au dos à étiquettes devinées (voir backtap_dataset.h).
//
// Règles d'étiquetage, d'après les tirs du détecteur du jeu autour d'un
// candidat du détecteur large (instants des débuts de pic) :
// - tiré : un tir à moins de MATCH (60 ms) du candidat. Si l'autre portail
//   suit de 0,2 à 1,2 s sans que la caméra ait tourné de plus de LOOK_STILL
//   comptes de souris (~4° à la sensibilité 3), le joueur corrige
//   probablement un mauvais côté (WRONG_SIDE, vrai côté = l'autre) ; sinon
//   FIRED. Caméra inconnue (rejeu d'un enregistrement brut) : pas de condition.
// - pas tiré : un tir moins de 0,4 s avant lui en fait un rebond de la main
//   (AFTER_FIRE) ; sinon, s'il atteint la moitié des seuils du jeu en
//   amplitude (accel_min ou gyro_rot_min) et qu'un tir le suit de 0,2 à 1,0 s,
//   c'est une tape manquée que le joueur refait plus fort (MISSED, côté de ce
//   tir) ; un tir n'explique qu'une tape manquée, la plus forte de cette
//   fenêtre (la plus ancienne à égalité) ; sinon un tir moins de 1,5 s avant
//   (AFTER_FIRE), et à défaut QUIET.
// Signalement du joueur (bouton ✗), prioritaire : un appui jusqu'à FLAG
// (2,5 s) après un tir, sans autre tir entre les deux, dit que ce tir était
// faux : mauvais côté si l'autre portail suit l'appui de moins de FOLLOW (2 s),
// fausse détection sinon. Un appui sans tir dans les FLAG secondes d'avant dit
// qu'une tape a été manquée : le candidat le plus fort de cette fenêtre (hors
// rebond et hors des 100 ms autour d'un toucher, l'appui lui-même secouant le
// téléphone), du côté du tir qui suit l'appui de moins de FOLLOW s'il y en a un.
// D'où HORIZON = 4,8 s : FLAG + FOLLOW, plus MATCH et le délai de jugement du
// détecteur du jeu (100 ms et plus), pour que le tir de fin de fenêtre soit connu.
// Plusieurs appuis : le premier qui s'applique compte.
//   Sans ce plancher, presque tout frémissement de la main (0,03-0,05 g) qui
//   précède une tape passait pour une tape manquée (rejeu des enregistrements
//   du 26/10 : 2,5 candidats par seconde au quart des seuils).
// Ces étiquettes sont des indices, pas une vérité : le fichier garde aussi les
// délais et la caméra jusqu'aux tirs voisins (colonnes prev_dt, next_dt,
// next_side, next_look) pour recalculer les étiquettes autrement hors ligne.
#include "backtap_dataset.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static const double kFlag = 2.5, kFollow = 2.0, kAfterFire = 1.5, kTouchNear = 0.1, kRebound = 0.4, kMissedMin = 0.2, kMissedMax = 1.0, kMissedStrength = 0.5, kWrongMin = 0.2, kWrongMax = 1.2, kLookStill = 60,
                    kWinBefore = 0.35;

const char *const rt_btds_label_names[RT_BTDS_LABELS] = {"quiet", "after_fire", "missed", "fired", "wrong_side",
                                                         "user_false", "user_wrong_side", "user_missed"};

static int is_user(rt_btds_label l) { return l >= RT_BTDS_USER_FALSE; }
static int is_correction(rt_btds_label l) { return l == RT_BTDS_MISSED || l == RT_BTDS_WRONG_SIDE || is_user(l); }
static int is_tap(rt_btds_label l) { return l != RT_BTDS_QUIET && l != RT_BTDS_AFTER_FIRE && l != RT_BTDS_USER_FALSE; }

// Amplitude rapportée aux seuils du jeu (1 : à la limite).
static float strength_of(const rt_btds *ds, const rt_backtap_report *r) {
    return fmaxf(r->peak / ds->accel_min, fabsf(r->rot[0]) / ds->gyro_rot_min);
}

void rt_btds_init(rt_btds *ds, const rt_backtap_params *p, FILE *out, const char *tag, double session, const long *counts) {
    memset(ds, 0, sizeof(*ds));
    ds->accel_min = p->accel_min; ds->gyro_rot_min = p->gyro_rot_min;
    ds->out = out; ds->session = session;
    snprintf(ds->tag, sizeof(ds->tag), "%s", tag ? tag : "");
    for (char *c = ds->tag; *c; ++c) if (*c == ',' || *c == '"' || *c == '\n' || *c == '\r') *c = '_';
    if (counts) { ds->taps = counts[0]; ds->corrections = counts[1]; ds->reported = counts[2]; }
}

int rt_btds_repair_tail(const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) return 0;   // fichier neuf
    long size = fseek(f, 0, SEEK_END) ? -1 : ftell(f), keep = size;
    char buf[4096];
    // Remonte jusqu'à la dernière fin de ligne.
    while (keep > 0) {
        long from = keep > (long)sizeof(buf) ? keep - (long)sizeof(buf) : 0, n = keep - from;
        if (fseek(f, from, SEEK_SET) || fread(buf, 1, (size_t)n, f) != (size_t)n) { fclose(f); return -1; }
        long i = n;
        while (i > 0 && buf[i - 1] != '\n') --i;
        if (i > 0) { keep = from + i; break; }
        keep = from;
    }
    fclose(f);
    if (size < 0) return -1;
    return keep == size ? 0 : truncate(path, keep);
}

void rt_btds_count_file(FILE *f, long *counts) {
    counts[0] = counts[1] = counts[2] = 0;
    if (!f) return;
    char buf[512];
    int start = 1;   // début de ligne (les lignes sont plus longues que buf)
    while (fgets(buf, sizeof(buf), f)) {
        size_t len = strlen(buf);
        if (start && buf[0] != '#' && strncmp(buf, "session,", 8)) {
            // session,tag,t,label,... : quatrième champ
            const char *p = buf; int field = 0;
            while (*p && field < 3) if (*p++ == ',') ++field;
            if (field == 3) {
                const char *e = strchr(p, ',');
                size_t n = e ? (size_t)(e - p) : 0;
                for (int l = 0; l < RT_BTDS_LABELS; ++l) {
                    const char *name = rt_btds_label_names[l];
                    if (n != strlen(name) || strncmp(p, name, n)) continue;
                    counts[0] += is_tap(l); counts[1] += is_correction(l); counts[2] += is_user(l);
                }
            }
        }
        start = len > 0 && buf[len - 1] == '\n';
    }
}

void rt_btds_candidate_add(rt_btds *ds, const rt_backtap *d, const rt_backtap_report *r, double look) {
    if (r->reject && !strcmp(r->reject, "menu")) return;   // hors partie : rien à apprendre
    if (ds->npending == RT_BTDS_PENDING) {                 // ne devrait pas arriver (32 en 1,5 s)
        rt_btds_tick(ds, 1e300);
    }
    unsigned slot = ds->nrecent++ % RT_BTDS_PENDING;
    ds->recent[slot].t = r->t; ds->recent[slot].strength = strength_of(ds, r);
    rt_btds_candidate *c = &ds->pending[ds->npending++];
    c->r = *r; c->look = look; c->fired = 0; c->nwin = 0;
    unsigned long first = d->count > RT_BACKTAP_RING ? d->count - RT_BACKTAP_RING : 0;
    for (unsigned long i = first; i < d->count; ++i) {
        const rt_backtap_sample *s = &d->ring[i % RT_BACKTAP_RING];
        if (s->t < r->t - kWinBefore) continue;
        if (c->nwin == RT_BTDS_WINDOW) break;   // fréquence plus haute que prévue : l'avant du pic compte le plus
        c->win[c->nwin++] = *s;
    }
}

void rt_btds_flag_add(rt_btds *ds, double t) { ds->flags[ds->nflags++ % RT_BTDS_FLAGS] = t; }
void rt_btds_touch_add(rt_btds *ds, double t) { ds->touches[ds->ntouches++ % RT_BTDS_FLAGS] = t; }

// Un toucher connu (du bilan ou arrivé depuis) à moins de kTouchNear de t ?
static int near_touch(const rt_btds *ds, double t) {
    unsigned n = ds->ntouches < RT_BTDS_FLAGS ? ds->ntouches : RT_BTDS_FLAGS;
    for (unsigned i = 0; i < n; ++i) if (fabs(ds->touches[i] - t) <= kTouchNear) return 1;
    return 0;
}

void rt_btds_fire_add(rt_btds *ds, double t, int side, double look) {
    rt_btds_fire *f = &ds->fires[ds->nfires++ % RT_BTDS_FIRES];
    f->t = t; f->side = side; f->look = look;
}

// Tirs connus : celui du candidat, le dernier avant, le premier après.
static const rt_btds_fire *fire_near(const rt_btds *ds, double t, int dir) {
    const rt_btds_fire *best = NULL;
    unsigned n = ds->nfires < RT_BTDS_FIRES ? ds->nfires : RT_BTDS_FIRES;
    for (unsigned i = 0; i < n; ++i) {
        const rt_btds_fire *f = &ds->fires[i];
        double dt = f->t - t;
        if (dir == 0 && fabs(dt) <= RT_BTDS_MATCH && (!best || fabs(dt) < fabs(best->t - t))) best = f;
        if (dir < 0 && dt < -RT_BTDS_MATCH && (!best || f->t > best->t)) best = f;
        if (dir > 0 && dt > RT_BTDS_MATCH && (!best || f->t < best->t)) best = f;
    }
    return best;
}

static const rt_btds_fire *fire_near(const rt_btds *ds, double t, int dir);

// Un autre candidat de [lo, hi] qui pourrait lui aussi être la tape manquée
// (pas tiré, pas un rebond, au moins floor, loin d'un toucher si far_touch)
// l'emporte-t-il sur celui-ci ?
static int outranked(const rt_btds *ds, double t, float strength, double lo, double hi, double floor, int far_touch) {
    unsigned n = ds->nrecent < RT_BTDS_PENDING ? ds->nrecent : RT_BTDS_PENDING;
    for (unsigned i = 0; i < n; ++i) {
        double u = ds->recent[i].t; float k = ds->recent[i].strength;
        if (u == t || u < lo || u > hi || k < floor || (far_touch && near_touch(ds, u))) continue;
        const rt_btds_fire *prev = fire_near(ds, u, -1);
        if (fire_near(ds, u, 0) || (prev && u - prev->t < kRebound)) continue;
        if (k > strength || (k == strength && u < t)) return 1;
    }
    return 0;
}

// Appuis sur ✗ dans [from, from + kFlag], dans l'ordre : *flag à 0 pour le
// premier, rend 0 quand il n'y en a plus.
static int next_flag(const rt_btds *ds, double from, double *flag) {
    double best = 0, after = *flag;
    unsigned n = ds->nflags < RT_BTDS_FLAGS ? ds->nflags : RT_BTDS_FLAGS;
    for (unsigned i = 0; i < n; ++i) {
        double f = ds->flags[i];
        if (f >= from && f <= from + kFlag && (!after || f > after) && (!best || f < best)) best = f;
    }
    *flag = best;
    return best != 0;
}
// Dernier tir à t ou avant, premier tir après t (à moins de kFollow).
static const rt_btds_fire *fire_before(const rt_btds *ds, double t) {
    const rt_btds_fire *best = NULL;
    unsigned n = ds->nfires < RT_BTDS_FIRES ? ds->nfires : RT_BTDS_FIRES;
    for (unsigned i = 0; i < n; ++i)
        if (ds->fires[i].t <= t && (!best || ds->fires[i].t > best->t)) best = &ds->fires[i];
    return best;
}
static const rt_btds_fire *fire_following(const rt_btds *ds, double t) {
    const rt_btds_fire *best = NULL;
    unsigned n = ds->nfires < RT_BTDS_FIRES ? ds->nfires : RT_BTDS_FIRES;
    for (unsigned i = 0; i < n; ++i)
        if (ds->fires[i].t > t && ds->fires[i].t <= t + kFollow && (!best || ds->fires[i].t < best->t)) best = &ds->fires[i];
    return best;
}

static double look_between(double a, double b) { return isnan(a) || isnan(b) ? NAN : fabs(b - a); }

rt_btds_label rt_btds_label_of(const rt_btds *ds, const rt_btds_candidate *c, int *side) {
    double t = c->r.t;
    const rt_btds_fire *own = fire_near(ds, t, 0), *prev = fire_near(ds, t, -1), *next = fire_near(ds, t, 1);
    float strength = strength_of(ds, &c->r);
    // Signalement du joueur.
    double flag = 0;
    if (own) {
        while (next_flag(ds, own->t, &flag)) {
            if (fire_before(ds, flag) != own) continue;   // un autre tir entre les deux : l'appui le vise
            const rt_btds_fire *re = fire_following(ds, flag);
            if (re && re->side == -own->side) { *side = re->side; return RT_BTDS_USER_WRONG_SIDE; }
            *side = 0; return RT_BTDS_USER_FALSE;
        }
    } else if (fabs(c->r.touch_dt) > kTouchNear && !near_touch(ds, t) && !(prev && t - prev->t < kRebound)) {
        // Appui après le candidat, sans tir dans les kFlag secondes d'avant.
        while (next_flag(ds, t, &flag)) {
            const rt_btds_fire *last = fire_before(ds, flag);
            if ((last && flag - last->t <= kFlag) || outranked(ds, t, strength, flag - kFlag, flag, 0, 1)) continue;
            const rt_btds_fire *re = fire_following(ds, flag);
            *side = re ? re->side : 0; return RT_BTDS_USER_MISSED;
        }
    }
    if (own) {
        const rt_btds_fire *after = fire_near(ds, own->t, 1);
        double dt = after ? after->t - own->t : 0, look = after ? look_between(own->look, after->look) : NAN;
        if (after && after->side == -own->side && dt >= kWrongMin && dt <= kWrongMax && !(look > kLookStill)) {
            *side = after->side; return RT_BTDS_WRONG_SIDE;
        }
        *side = own->side; return RT_BTDS_FIRED;
    }
    *side = 0;
    if (prev && t - prev->t < kRebound) return RT_BTDS_AFTER_FIRE;
    if (next && next->t - t >= kMissedMin && next->t - t <= kMissedMax && strength >= kMissedStrength &&
        !outranked(ds, t, strength, next->t - kMissedMax, next->t - kMissedMin, kMissedStrength, 0)) {
        *side = next->side; return RT_BTDS_MISSED;
    }
    if (prev && t - prev->t < kAfterFire) return RT_BTDS_AFTER_FIRE;
    return RT_BTDS_QUIET;
}

static void write_candidate(rt_btds *ds, const rt_btds_candidate *c) {
    int side; rt_btds_label label = rt_btds_label_of(ds, c, &side);
    const rt_btds_fire *own = fire_near(ds, c->r.t, 0), *prev = fire_near(ds, c->r.t, -1), *next = fire_near(ds, c->r.t, 1);
    ++ds->by_label[label];
    ds->taps += is_tap(label); ds->corrections += is_correction(label); ds->reported += is_user(label);
    FILE *o = ds->out;
    if (!o) return;
    const rt_backtap_report *r = &c->r;
    // Après un tir, le voisin utile est le tir suivant celui-ci.
    const rt_btds_fire *after = own ? fire_near(ds, own->t, 1) : next;
    double base_t = own ? own->t : r->t, base_look = own ? own->look : c->look;
    fprintf(o, "%.3f,%s,%.4f,%s,%d,%d,%s,%d,%d,%s", ds->session, ds->tag, r->t, rt_btds_label_names[label], side,
            own ? own->side : 0, r->reject ? r->reject : "", r->weak, r->gyro, r->method ? r->method : "");
    fprintf(o, ",%.4f,%.2f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.3f,%d",
            r->peak, r->jerk, r->acc[0], r->acc[1], r->acc[2], r->rot[0], r->rot[1], r->rot[2], r->side_score, r->width,
            r->rise, r->grav[0], r->grav[1], r->grav[2], r->pre_rms, r->tail_rms, r->gravity,
            fabs(r->touch_dt) < 10 ? r->touch_dt : 9.999, r->orient);
    if (prev) fprintf(o, ",%.3f", r->t - prev->t); else fputs(",", o);
    if (after) {
        double look = look_between(base_look, after->look);
        fprintf(o, ",%.3f,%d", after->t - base_t, after->side);
        if (isnan(look)) fputs(",", o); else fprintf(o, ",%.0f", look);
    } else fputs(",,,", o);
    // Fenêtre : dt (ms depuis le début du pic) a(3) w(3) g(3), échantillons séparés par « ; ».
    fputc(',', o);
    for (int i = 0; i < c->nwin; ++i) {
        const rt_backtap_sample *s = &c->win[i];
        fprintf(o, "%s%.0f %.3f %.3f %.3f %.3f %.3f %.3f %.2f %.2f %.2f", i ? ";" : "", (s->t - r->t) * 1000, s->a[0],
                s->a[1], s->a[2], s->w[0], s->w[1], s->w[2], s->g[0], s->g[1], s->g[2]);
    }
    fputc('\n', o);
    if (ferror(o) && !ds->write_error) ds->write_error = 1;
}

void rt_btds_tick(rt_btds *ds, double now) {
    int kept = 0;
    for (int i = 0; i < ds->npending; ++i) {
        if (ds->pending[i].r.t + RT_BTDS_HORIZON <= now) write_candidate(ds, &ds->pending[i]);
        else ds->pending[kept++] = ds->pending[i];
    }
    ds->npending = kept;
}

void rt_btds_flush(rt_btds *ds) {
    rt_btds_tick(ds, 1e300);
    if (ds->out) fflush(ds->out);
}

int rt_btds_wanted(const char *dataset, const char *raw, int *custom) {
    *custom = dataset && *dataset && strcmp(dataset, "0") && strcmp(dataset, "1");
    if (dataset && !strcmp(dataset, "0")) return 0;
    return (dataset && *dataset) || (raw && *raw);
}

// En-tête du fichier (écrit par l'appelant quand le fichier est neuf).
const char rt_btds_header[] =
    "session,tag,t,label,label_side,fired,reject,weak,gyro,method,peak,jerk,acc_x,acc_y,acc_z,rot_x,rot_y,rot_z,"
    "side_score,width,rise,grav_x,grav_y,grav_z,pre_rms,tail_rms,gravity,touch_dt,orient,prev_dt,next_dt,next_side,"
    "next_look,window\n";
