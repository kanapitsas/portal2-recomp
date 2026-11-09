// Variables d'environnement des fichiers runtime/ios (P2_IOS_GL_TRACE,
// P2_BACKTAP_HOLD, P2_IOS_MAX_FPS…) : drapeau, nombre, entier, signe. En-tête
// seul, sans dépendance : aussi compilé sur le Mac (tests, tools/backtap_replay.c).
#pragma once
#include <stdlib.h>

// Diagnostics GL de gl_ios.m et srgb.c (P2_IOS_GL_TRACE, P2_IOS_SHADER_DUMP,
// P2_IOS_DEBUG_UV, P2_IOS_PROBE_FRAMES, P2_IOS_SKIP_DRAWS, P2_IOS_DRAW_LOG,
// P2_IOS_SRGB_TRACE…) : seulement dans un build de débogage (tools/build_ios.py
// --debug, ou P2_IOS_GL_DEBUG=1 dans l'environnement du build, qui définit
// RT_IOS_GL_DEBUG). Ailleurs ces fonctions rendent 0 et le compilateur retire
// leur code du chemin de chaque tracé.
#ifdef RT_IOS_GL_DEBUG
#define RT_IOS_DEBUG_BUILD 1
#else
#define RT_IOS_DEBUG_BUILD 0
#endif

// Nombre : la valeur de la variable si elle est présente, non vide et >= min,
// sinon def.
static inline double rt_ios_env_double(const char *name, double def, double min) {
    const char *e = getenv(name);
    double value = e && *e ? atof(e) : 0;
    return e && *e && value >= min ? value : def;
}
// Même chose en simple précision : la borne est comparée en float (« 0.1 »
// atteint ainsi la borne 0,1f).
static inline float rt_ios_env_float(const char *name, float def, float min) {
    const char *e = getenv(name);
    float value = e && *e ? (float)atof(e) : 0;
    return e && *e && value >= min ? value : def;
}
// Entier : la valeur si elle est présente et >= min, sinon def.
static inline int rt_ios_env_int(const char *name, int def, int min) {
    const char *e = getenv(name);
    int value = e && *e ? atoi(e) : 0;
    return e && *e && value >= min ? value : def;
}
// Signe : +1, -1 ou 0 d'après la variable ; def si elle est absente ou vide.
static inline int rt_ios_env_sign(const char *name, int def) {
    const char *e = getenv(name);
    return !e || !*e ? def : atoi(e) > 0 ? 1 : atoi(e) < 0 ? -1 : 0;
}

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
