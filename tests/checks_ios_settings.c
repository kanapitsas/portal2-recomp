// Réglages iOS (runtime/ios/settings.c) : traduction en variables P2_*,
// priorité des variables déjà posées, concordance avec ios/Settings.bundle.
#include "checks.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../runtime/ios/settings.c"

typedef struct { const char *key, *value; } Pref;
static int fake_get(void *ctx, const char *key, char *out, size_t size) {
    for (const Pref *p = ctx; p->key; ++p)
        if (!strcmp(p->key, key)) { snprintf(out, size, "%s", p->value); return 1; }
    return 0;
}
static void clear_env(void) {
    for (int i = 0; i < rt_ios_settings_count; ++i) unsetenv(rt_ios_settings[i].env);
}

RT_CHECK(ios_settings_become_environment) {
    clear_env();
    Pref prefs[] = {{"P2ResolutionScale", "1.5"}, {"P2ShowFPS", "0"}, {"P2BackTap", "1"}, {"P2TouchControls", "auto"},
                    {"P2PortalButtons", "1"}, {"P2Log", "0"}, {"P2BackTapInvert", "1"}, {"P2Renderer", "d3d9"},
                    {"P2BackTapRecord", "1"}, {"P2Haptics", "0"}, {"P2MaxFPS", "40"}, {"P2PowerSave", "0"}, {"P2Upscale", "0.75"}, {NULL, NULL}};
    char summary[512];
    int n = rt_ios_settings_apply(fake_get, prefs, summary, sizeof(summary));
    assert(n == 11);
    assert(!strcmp(getenv("P2_IOS_UPSCALE"), "0.75"));
    assert(!strcmp(getenv("P2_IOS_MAX_FPS"), "40"));
    assert(!strcmp(getenv("P2_IOS_POWER_SAVE"), "0"));           // économie d'énergie automatique coupée
    assert(!strcmp(getenv("P2_HAPTICS"), "0"));                   // vibration au tir coupée
    assert(!strcmp(getenv("P2_BACKTAP_RAW"), "Documents/backtap/"));   // un fichier daté par lancement
    assert(!strcmp(getenv("P2_RENDERER"), "d3d9"));               // Rendu : Metal (src/main.c)
    assert(!strcmp(getenv("P2_IOS_RENDER_SCALE"), "1.5"));
    assert(!strcmp(getenv("P2_SHOW_FPS"), "0"));                 // interrupteur éteint
    assert(!strcmp(getenv("P2_BACKTAP"), "1"));
    assert(!strcmp(getenv("P2_BACKTAP_INVERT"), "1"));
    assert(!strcmp(getenv("P2_PORTAL_BUTTONS"), "1"));
    assert(!getenv("P2_TOUCH_CONTROLS"));                         // « auto » : défaut du code
    assert(!getenv("P2_LOG_FILE"));                               // journal éteint : aucune variable
    assert(!getenv("P2_TOUCH_LOOK"));                             // jamais touché
    assert(strstr(summary, "P2_IOS_RENDER_SCALE=1.5") && strstr(summary, "P2_SHOW_FPS=0"));
    clear_env();
}

// Une variable déjà posée (devicectl, bancs) prime sur le réglage.
RT_CHECK(ios_settings_yield_to_existing_environment) {
    clear_env();
    setenv("P2_IOS_RENDER_SCALE", "native", 1);
    Pref prefs[] = {{"P2ResolutionScale", "1"}, {"P2Log", "1"}, {NULL, NULL}};
    char summary[256];
    assert(rt_ios_settings_apply(fake_get, prefs, summary, sizeof(summary)) == 1);
    assert(!strcmp(getenv("P2_IOS_RENDER_SCALE"), "native"));
    assert(!strcmp(getenv("P2_LOG_FILE"), "Documents/game.log"));
    clear_env();
}

// Chaque réglage de la page iOS est lu par le code, et inversement
// (P2TouchTrackpad est lu directement par touch_controls.m).
RT_CHECK(ios_settings_match_settings_bundle) {
    FILE *f = fopen("ios/Settings.bundle/Root.plist", "r");
    assert(f);
    static char plist[65536];
    size_t n = fread(plist, 1, sizeof(plist) - 1, f);
    fclose(f);
    plist[n] = 0;
    for (int i = 0; i < rt_ios_settings_count; ++i) {
        char needle[128];
        snprintf(needle, sizeof(needle), "<string>%s</string>", rt_ios_settings[i].key);
        assert(strstr(plist, needle));
    }
    int keys = 0;
    for (const char *p = plist; (p = strstr(p, "<key>Key</key>")); ++p) {
        const char *v = strstr(p, "<string>") + 8, *e = strstr(v, "</string>");
        char key[64];
        snprintf(key, sizeof(key), "%.*s", (int)(e - v), v);
        int known = !strcmp(key, "P2TouchTrackpad");
        for (int i = 0; i < rt_ios_settings_count; ++i) known |= !strcmp(key, rt_ios_settings[i].key);
        assert(known);
        ++keys;
    }
    assert(keys == rt_ios_settings_count + 1);
}

// Choix d'une liste de la page Réglages : la valeur par défaut est celle que le
// code applique quand le réglage n'a jamais été touché, et les valeurs sont
// celles que le code sait lire.
static char settings_plist[65536];
static const char *settings_entry(const char *key, size_t *length) {
    if (!settings_plist[0]) {
        FILE *f = fopen("ios/Settings.bundle/Root.plist", "r");
        assert(f);
        settings_plist[fread(settings_plist, 1, sizeof(settings_plist) - 1, f)] = 0;
        fclose(f);
    }
    char needle[128];
    snprintf(needle, sizeof(needle), "<key>Key</key>\n\t\t\t<string>%s</string>", key);
    const char *at = strstr(settings_plist, needle);
    assert(at);
    const char *start = at;
    while (start > settings_plist && strncmp(start, "<dict>", 6)) --start;
    const char *end = strstr(at, "</dict>");
    assert(end);
    *length = (size_t)(end - start);
    return start;
}
static void assert_choices(const char *key, const char *def, const char *const *values) {
    size_t length;
    const char *entry = settings_entry(key, &length);
    char text[8192], needle[128];
    assert(length < sizeof(text));
    memcpy(text, entry, length); text[length] = 0;
    snprintf(needle, sizeof(needle), "<key>DefaultValue</key>\n\t\t\t<string>%s</string>", def);
    assert(strstr(text, needle));
    const char *list = strstr(text, "<key>Values</key>");
    assert(list);
    int n = 0;
    for (const char *p = list; (p = strstr(p, "<string>")); ++p) ++n;
    for (int i = 0; values[i]; ++i) {
        snprintf(needle, sizeof(needle), "<string>%s</string>", values[i]);
        assert(strstr(list, needle));
        --n;
    }
    assert(n == 0);   // ni plus ni moins
}
RT_CHECK(ios_settings_choices_match_code) {
    // P2_IOS_MAX_FPS : 60 par défaut (power_ios.m), 40 demande l'écran à 120 Hz (gl_ios.m).
    assert_choices("P2MaxFPS", "60", (const char *const[]){"30", "40", "60", "120", NULL});
    assert_choices("P2ResolutionScale", "2", (const char *const[]){"native", "2", "1.5", "1", NULL});
    assert_choices("P2Upscale", "1", (const char *const[]){"1", "0.75", "0.5", NULL});
    // P2_GYRO_SPEED : multiplicateur, 1 par défaut (gyro_aim.c).
    assert_choices("P2GyroSpeed", "1", (const char *const[]){"0.5", "0.75", "1", "1.5", "2", NULL});
}

// Agrandissement MetalFX : render.scale seulement pour une fraction de ]0, 1[.
RT_CHECK(ios_metal_keys_add_render_scale) {
    char keys[128];
    double scale = 0;
    rt_ios_metal_keys(keys, sizeof(keys), 60, "0.75", &scale);
    assert(!strcmp(keys, "present.maxFps=60;render.scale=0.75") && scale == 0.75);
    rt_ios_metal_keys(keys, sizeof(keys), 30, "0.5", &scale);
    assert(!strcmp(keys, "present.maxFps=30;render.scale=0.5"));
    const char *none[] = {"1", "", "0", "-0.5", "1.5", "abc", "0.75x"};
    for (size_t i = 0; i < sizeof(none) / sizeof(*none); ++i) {
        rt_ios_metal_keys(keys, sizeof(keys), 120, none[i], &scale);
        assert(!strcmp(keys, "present.maxFps=120") && scale == 1);
    }
    rt_ios_metal_keys(keys, sizeof(keys), 60, NULL, NULL);
    assert(!strcmp(keys, "present.maxFps=60"));
}
