// Drapeaux d'environnement des fichiers runtime/ios (P2_IOS_GL_TRACE, P2_IOS_SRGB…).
#pragma once
#include <stdlib.h>

// 1 si la variable vaut « 1… », 0 si elle vaut « 0… », def si elle est absente,
// vide ou autre.
static inline int rt_ios_env_flag(const char *name, int def) {
    const char *e = getenv(name);
    return !e || !*e ? def : *e == '1' ? 1 : *e == '0' ? 0 : def;
}
// Même chose, lue une fois par point d'appel (chemins chauds : un getenv par
// tracé coûterait) ; expression GNU, comme typeof ailleurs dans ces fichiers.
#define RT_IOS_ENV_FLAG(name, def) ({ static int rt_ios_env_cached = -1; \
    if (rt_ios_env_cached < 0) rt_ios_env_cached = rt_ios_env_flag(name, def); rt_ios_env_cached; })
