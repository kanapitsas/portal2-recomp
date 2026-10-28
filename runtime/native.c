// Remplacements natifs de fonctions invitées chaudes (tools/lift.py,
// NATIVE_OVERRIDES). La fonction traduite reste disponible : P2_NATIVE_<NOM>=0
// la rétablit (comparaison A/B, repli si un remplacement pose problème).
#include "rt.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int rt_paths_case_insensitive(void);   // paths.c

int rt_native_enabled(const char *name, int *state) {
    int value = __atomic_load_n(state, __ATOMIC_RELAXED);
    if (value < 0) {
        char variable[64];
        snprintf(variable, sizeof(variable), "P2_NATIVE_%s", name);
        const char *e = getenv(variable);
        value = !(e && !strcmp(e, "0"));
        // Volume Mac sensible à la casse : le correcteur invité reste utile.
        if (value && !strcmp(name, "PATHMATCH") && !rt_paths_case_insensitive()) value = 0;
        __atomic_store_n(state, value, __ATOMIC_RELAXED);
    }
    return value;
}

// pathmatch(const char *in, char **out, bool allow_basename_mismatch,
//           char *buffer, size_t size) de filesystem_stdio (Valve, Linux).
// L'original met *out à NULL, et si access(in) réussit rend false sans rien
// changer ; sinon il cherche une variante de casse dossier par dossier.
// L'appelant utilise alors *out à la place de in. Ici, aucune variante n'est
// cherchée : utilisé seulement quand toutes les racines résolvent la casse
// (APFS insensible sur Mac, paths.c et case_path.c sur iOS). Résultat : *out
// reste NULL, retour false, comme l'original quand le chemin existe tel quel.
int rt_native_pathmatch(CPU *c, uint32_t entry) {
    (void)entry;
    uint32_t out = R32(c->esp + 8);
    if (out) W32(out, 0);
    c->eax = 0;
    rt_return(c);
    return 1;
}

// CVEngineServer::GetChangeAccessor(this, const edict_t *) d'engine.so :
//     sub esp,0x18 ; push [esp+0x20] (l'edict) ; call NUM_FOR_EDICT
//     mov edx,[sv.edictchangeinfo] ; add esp,0x1c ; lea eax,[edx+eax*4] ; ret
// avec NUM_FOR_EDICT : mov eax,[esp+4] ; sub eax,[sv.edicts] ; sar eax,4 ; ret.
// Quand on quitte en pleine partie, le moteur remet sv.edicts à zéro avant que
// les entités aient fini d'appeler NetworkStateChanged. L'index devient alors
// l'adresse de l'edict divisée par 16. Sous Linux le tas est sous 2 Go : le
// pointeur obtenu retombe dans de la mémoire projetée, sans effet visible. Ici
// le tas invité est au-dessus de 0x80000000 (heap.c) : le décalage arithmétique
// rend l'index négatif et l'accès tombe hors de toute projection (SIGBUS).
// Même calcul que l'original tant que sv.edicts est valide ; sinon, un
// accesseur factice que l'appelant peut lire et écrire sans conséquence.
// Les adresses des deux tableaux sont lues dans le code invité : tools/lift.py
// a vérifié les octets de GetChangeAccessor, ceux de NUM_FOR_EDICT le sont ici
// (sinon 0 : la fonction traduite s'exécute).
int rt_native_change_accessor(CPU *c, uint32_t entry) {
    static const uint8_t head[] = {0x8b, 0x44, 0x24, 0x04, 0x2b, 0x05}, tail[] = {0xc1, 0xf8, 0x04, 0xc3};
    static uint32_t scratch;
    uint32_t number_for_edict = entry + 12 + R32(entry + 8);   // call rel32 en entry+7
    if (!rt_guest_readable(number_for_edict) || !rt_guest_readable(number_for_edict + 13) ||
        memcmp(GPTR(number_for_edict), head, sizeof(head)) || memcmp(GPTR(number_for_edict + 10), tail, sizeof(tail)))
        return 0;
    // [esp+4] : this (CVEngineServer), [esp+8] : l'edict.
    uint32_t edict = R32(c->esp + 8), edicts = R32(R32(number_for_edict + 6)), infos = R32(R32(entry + 14));
    if (edicts) {
        c->eax = infos + 4u * (uint32_t)((int32_t)(edict - edicts) >> 4);
    } else {
        if (!scratch) scratch = rt_alloc(16);
        if (!scratch) return 0;
        memset(GPTR(scratch), 0, 16);
        c->eax = scratch;
    }
    rt_return(c);
    return 1;
}
