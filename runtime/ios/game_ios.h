// Jeu de ce build iOS : tools/build_ios.py écrit le profil (tools/games.py)
// dans la clé P2Game de l'Info.plist ; rien n'est deviné à l'exécution.
#pragma once

typedef enum { RT_IOS_LAYOUT_PORTAL2, RT_IOS_LAYOUT_HL2 } rt_ios_layout;

typedef struct {
    const char *name;       // valeur de P2Game : "portal2", "portal1", "hl2"
    const char *mod;        // -game par défaut
    const char *userdata;   // dossier des sauvegardes dans Documents
    const char *app_id;     // SteamAppId du stub Steam (runtime/ios/steam_stub.c)
    int back_taps;          // tapes au dos par défaut (P2_BACKTAP absente)
    rt_ios_layout layout;   // disposition des commandes tactiles (touch_layout.c)
    // ICvar de libvstdlib.so (game_state.c) : version et entrée de
    // FindCommandBase dans sa table virtuelle.
    const char *cvar_interface;
    unsigned find_command_base;
} rt_ios_game;

// Profil de ce nom ; Portal 2 pour NULL, "" ou un nom inconnu (builds sans P2Game).
const rt_ios_game *rt_ios_game_named(const char *name);
// Profil de l'app en cours (Info.plist, lu une fois) ; Portal 2 hors iOS.
const rt_ios_game *rt_ios_current_game(void);
