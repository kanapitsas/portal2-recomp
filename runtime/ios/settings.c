// Réglages du jeu dans l'app Réglages d'iOS (ios/Settings.bundle) : au
// lancement, chaque réglage modifié par le joueur devient la variable P2_*
// que le runtime lit déjà. Une variable déjà présente (devicectl, simctl)
// prime : les bancs et diagnostics gardent la main. Un réglage jamais touché
// n'est pas enregistré par iOS : le défaut du code s'applique, et le
// DefaultValue du Settings.bundle doit lui correspondre
// (tests/checks_ios_settings.c vérifie que les clés concordent).
#include "settings.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// on/off : interrupteur (valeur lue « 1 » ou « 0 ») traduit en ces valeurs,
// NULL : variable laissée absente. on == NULL : liste de choix, valeur copiée,
// « auto » : variable laissée absente (défaut du code).
const rt_ios_setting rt_ios_settings[] = {
    {"P2ResolutionScale", "P2_IOS_RENDER_SCALE", NULL, NULL},   // native, 2, 1.5, 1
    {"P2MaxFPS", "P2_IOS_MAX_FPS", NULL, NULL},                 // 60, 120
    {"P2ShowFPS", "P2_SHOW_FPS", "1", "0"},
    {"P2Renderer", "P2_RENDERER", NULL, NULL},                 // opengl, d3d9 (src/main.c)
    {"P2TouchControls", "P2_TOUCH_CONTROLS", NULL, NULL},       // auto, 1, 0
    {"P2TouchLook", "P2_TOUCH_LOOK", NULL, NULL},               // vitesse de la caméra
    {"P2TouchAccel", "P2_TOUCH_ACCEL", NULL, NULL},             // accélération du trackpad (1 : aucune)
    {"P2BackTap", "P2_BACKTAP", "1", "0"},
    {"P2BackTapSensitivity", "P2_BACKTAP_SENSITIVITY", NULL, NULL},
    {"P2BackTapInvert", "P2_BACKTAP_INVERT", "1", "0"},
    {"P2PortalButtons", "P2_PORTAL_BUTTONS", NULL, NULL},       // auto, 1, 0
    {"P2Log", "P2_LOG_FILE", "Documents/game.log", NULL},
};
const int rt_ios_settings_count = (int)(sizeof(rt_ios_settings) / sizeof(*rt_ios_settings));

int rt_ios_settings_apply(rt_ios_settings_get get, void *ctx, char *summary, size_t size) {
    int applied = 0;
    size_t used = 0;
    if (summary && size) summary[0] = 0;
    for (int i = 0; i < rt_ios_settings_count; ++i) {
        const rt_ios_setting *s = &rt_ios_settings[i];
        char value[256];
        if (getenv(s->env) || !get(ctx, s->key, value, sizeof(value))) continue;
        const char *chosen = value;
        if (s->on) chosen = strcmp(value, "0") && strcmp(value, "false") ? s->on : s->off;
        else if (!value[0] || !strcmp(value, "auto")) chosen = NULL;
        if (!chosen) continue;
        setenv(s->env, chosen, 0);
        ++applied;
        if (summary && used < size) {
            int n = snprintf(summary + used, size - used, "%s%s=%s", used ? ", " : "", s->env, chosen);
            if (n > 0) used += (size_t)n < size - used ? (size_t)n : size - used - 1;
        }
    }
    return applied;
}

#if defined(__APPLE__)
#include <TargetConditionals.h>
#endif
#if TARGET_OS_IPHONE
#include <CoreFoundation/CoreFoundation.h>

// Préférences de l'app (NSUserDefaults) : booléen des interrupteurs, chaîne
// des listes, nombre au besoin.
static int preference(void *ctx, const char *key, char *out, size_t size) {
    (void)ctx;
    CFStringRef name = CFStringCreateWithCString(NULL, key, kCFStringEncodingUTF8);
    CFPropertyListRef v = CFPreferencesCopyAppValue(name, kCFPreferencesCurrentApplication);
    CFRelease(name);
    if (!v) return 0;
    int ok = 1;
    if (CFGetTypeID(v) == CFBooleanGetTypeID()) snprintf(out, size, "%d", CFBooleanGetValue((CFBooleanRef)v) ? 1 : 0);
    else if (CFGetTypeID(v) == CFStringGetTypeID()) ok = CFStringGetCString((CFStringRef)v, out, (CFIndex)size, kCFStringEncodingUTF8);
    else if (CFGetTypeID(v) == CFNumberGetTypeID()) {
        double d = 0;
        CFNumberGetValue((CFNumberRef)v, kCFNumberDoubleType, &d);
        snprintf(out, size, "%g", d);
    } else ok = 0;
    CFRelease(v);
    return ok;
}

int rt_ios_settings_load(char *summary, size_t size) { return rt_ios_settings_apply(preference, NULL, summary, size); }
#endif
