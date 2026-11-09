// Profils de jeu du build iOS (voir game_ios.h). La table suit tools/games.py
// (mod, appid) ; tests/checks_touch_layout.c et tests/test_games.py vérifient
// la concordance.
#include "game_ios.h"
#include <TargetConditionals.h>
#include <stddef.h>
#include <string.h>
#if TARGET_OS_IPHONE
#include <CoreFoundation/CoreFoundation.h>
#endif

static const rt_ios_game games[] = {
    // Le dossier « portal2-userdata » reste celui des installations existantes.
    // ICvar : IAppSystem à 8 entrées, puis AllocateDLLIdentifier, Register/
    // UnregisterConCommand, UnregisterConCommands, GetCommandLineValue.
    {"portal2", "portal2", "portal2-userdata", "620", 1, RT_IOS_LAYOUT_PORTAL2, "VEngineCvar007", 13},
    // Tapes au dos coupées : elles ne donnent qu'un clic bref, alors que les
    // armes automatiques de HL2 se tirent en tenant le bouton, et un tir
    // involontaire coûte des munitions ou réveille les ennemis. P2_BACKTAP=1
    // les rend (gauche : tir, droite : tir secondaire).
    // ICvar de Source 2013 : IAppSystem à 5 entrées (Connect, Disconnect,
    // QueryInterface, Init, Shutdown), donc FindCommandBase en 10 ; vérifié
    // dans libvstdlib.so du dépôt 224 (table dont l'entrée 2 compare le nom à
    // « VEngineCvar004 », 5 incrémente un compteur, 10 et 11 parcourent la
    // liste des commandes par GetCommands, 16 et 17).
    {"hl2", "hl2_complete", "hl2-userdata", "220", 0, RT_IOS_LAYOUT_HL2, "VEngineCvar004", 10},
};

const rt_ios_game *rt_ios_game_named(const char *name) {
    for (size_t i = 0; name && i < sizeof(games) / sizeof(*games); ++i)
        if (!strcmp(games[i].name, name)) return &games[i];
    return &games[0];
}

const rt_ios_game *rt_ios_current_game(void) {
    static const rt_ios_game *current;
    if (current) return current;
#if TARGET_OS_IPHONE
    char name[32] = "";
    CFBundleRef bundle = CFBundleGetMainBundle();
    CFTypeRef value = bundle ? CFBundleGetValueForInfoDictionaryKey(bundle, CFSTR("P2Game")) : NULL;
    if (value && CFGetTypeID(value) == CFStringGetTypeID())
        CFStringGetCString((CFStringRef)value, name, sizeof(name), kCFStringEncodingUTF8);
    current = rt_ios_game_named(name);
#else
    current = rt_ios_game_named(NULL);
#endif
    return current;
}
