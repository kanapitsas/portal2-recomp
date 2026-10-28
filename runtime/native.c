// Remplacements natifs de fonctions invitées chaudes (tools/lift.py,
// NATIVE_OVERRIDES). La fonction traduite reste disponible : P2_NATIVE_<NOM>=0
// la rétablit (comparaison A/B, repli si un remplacement pose problème).
#include "rt.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int rt_native_enabled(const char *name, int *state) {
    int value = __atomic_load_n(state, __ATOMIC_RELAXED);
    if (value < 0) {
        char variable[64];
        snprintf(variable, sizeof(variable), "P2_NATIVE_%s", name);
        const char *e = getenv(variable);
        value = !(e && !strcmp(e, "0"));
        __atomic_store_n(state, value, __ATOMIC_RELAXED);
    }
    return value;
}

// pathmatch(const char *in, char **out, bool allow_basename_mismatch,
//           char *buffer, size_t size) de filesystem_stdio (Valve, Linux).
// L'original met *out à NULL, et si access(in) réussit rend false sans rien
// changer ; sinon il cherche une variante de casse dossier par dossier.
// L'appelant utilise alors *out à la place de in. Ici, aucune variante n'est
// cherchée : APFS (Mac) est insensible à la casse, et sur iOS paths.c et
// case_path.c résolvent la casse de chaque chemin du jeu. Résultat : *out
// reste NULL, retour false, comme l'original quand le chemin existe tel quel.
void rt_native_pathmatch(CPU *c) {
    uint32_t out = R32(c->esp + 8);
    if (out) W32(out, 0);
    c->eax = 0;
    rt_return(c);
}
