// État du jeu pour les commandes tactiles : interface du jeu (menus, pause,
// écrans de chargement) affichée ou non.
//
// Aucun appel SDL ne le dit sur iOS : le moteur ne capture jamais la souris et
// appelle SDL_ShowCursor(1) à chaque image, en partie comme au menu. On demande
// donc au moteur : IEngineVGui (« VEngineVGui001 », obtenu par CreateInterface
// d'engine.so), méthode IsGameUIVisible, 4e entrée de sa table virtuelle (deux
// destructeurs, GetPanel, IsGameUIVisible ; vérifié dans le code traduit :
// m_pGameUI ? m_pGameUI->IsGameUIActive() : false).
//
// Il règle aussi, pour la manette virtuelle du stick tactile (touch_controls.m),
// la courbe de réponse du déplacement au joystick (joy_response_move) : voir
// plus bas.
//
// Appelé depuis SDL_PollEvent (sdl_ios.m), sur le thread principal du jeu,
// verrou invité tenu, au plus dix fois par seconde.
#include <TargetConditionals.h>
#if TARGET_OS_IPHONE
#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "modules.h"
#include "rt.h"

static uint32_t engine_vgui;      // objet IEngineVGui invité (0 : pas encore)
static int ui_visible = 1;        // menus au démarrage
static uint64_t next_query, next_lookup;

void rt_input_nudge_mouse(void);   // runtime/sdl_input.c
int rt_ios_game_ui_visible(void) { return ui_visible; }

// ---- Courbe de réponse du déplacement au joystick ------------------------
// La configuration Xbox du jeu (cfg/joy_configuration.cfg, config.cfg) règle
// joy_response_move à 5 : inclinaison réduite de moitié jusqu'à 60 %, entière
// au-delà, d'où un trou entre 30 et 60 % de la vitesse de marche. Mesuré dans
// le simulateur : vitesse = 175 × courbe((axe - 0,15) / 0,85), 0,15 étant
// joy_forwardthreshold/joy_sidethreshold. Pour le stick tactile, le jeu passe
// en réponse linéaire (0) tant que la manette virtuelle est là, et retrouve sa
// valeur quand elle part (vraie manette branchée).
//
// Variables lues et écrites par ICvar (« VEngineCvar007 », libvstdlib.so) :
// FindCommandBase, 14e entrée de sa table (IAppSystem en a 8, puis
// AllocateDLLIdentifier, Register/UnregisterConCommand, UnregisterConCommands,
// GetCommandLineValue ; même indice que l'outil SAR pour Portal 2). Le résultat
// est vérifié par son nom (ConCommandBase : vptr, m_pNext, m_bRegistered,
// m_pszName) avant tout usage ; la valeur est posée par IConVar::SetValue(const
// char *), 1re entrée de la table d'IConVar (à +24 dans ConVar), et relue dans
// m_pszString (+36) du parent (+28).
static uint32_t icvar;            // 0 : pas encore ; 1 : introuvable
static int linear_wanted;         // demandé par touch_controls.m
static char saved_response[16];   // valeur du jeu à rendre ("" : rien à rendre)
static int response = -1;         // valeur actuelle de joy_response_move (-1 : inconnue)
static int convar_broken;         // vérification échouée : ne plus rien tenter

void rt_ios_move_response_linear(int on) { linear_wanted = on; }
int rt_ios_move_response(void) { return response; }

static uint32_t guest_text(const char *text) {
    uint32_t address = rt_alloc((uint32_t)strlen(text) + 1);
    if (address) memcpy(GPTR(address), text, strlen(text) + 1);
    return address;
}
static int plausible(uint32_t address) { return address >= 0x10000 && address < RT_IMPORT_BASE - 64; }
// ConVar invitée de ce nom, ou 0.
static uint32_t find_convar(CPU *c, const char *name) {
    if (!icvar) {
        const GuestModule *vstdlib = rt_find_module("libvstdlib.so");
        uint32_t create = vstdlib ? rt_module_symbol(vstdlib, "CreateInterface") : 0;
        uint32_t text = create ? guest_text("VEngineCvar007") : 0;
        uint32_t args[2] = {text, 0};
        icvar = text ? rt_invoke(c, create, args, 2) : 0;
        rt_free(text);
        if (!icvar) icvar = 1;
    }
    if (icvar == 1) return 0;
    uint32_t text = guest_text(name);
    if (!text) return 0;
    uint32_t args[2] = {icvar, text};
    uint32_t base = rt_invoke(c, R32(R32(icvar) + 13 * 4), args, 2);
    rt_free(text);
    if (!plausible(base) || !plausible(R32(base + 12)) || strcmp(GPTR(R32(base + 12)), name) ||
        !plausible(R32(base + 28))) return 0;
    return base;
}
static const char *convar_string(uint32_t convar) {
    uint32_t text = R32(R32(convar + 28) + 36);
    return plausible(text) ? GPTR(text) : "";
}
static int set_convar(CPU *c, uint32_t convar, const char *value) {
    uint32_t text = guest_text(value);
    if (!text) return 0;
    uint32_t args[2] = {convar + 24, text};
    rt_invoke(c, R32(R32(convar + 24)), args, 2);
    rt_free(text);
    return !strcmp(convar_string(convar), value);
}
static void update_move_response(CPU *c) {
    if (convar_broken) return;
    uint32_t convar = find_convar(c, "joy_response_move");
    if (!convar) {
        if (icvar && !ui_visible) { convar_broken = 1; fprintf(stderr, "[ios] joy_response_move introuvable : réponse du jeu gardée\n"); }
        return;
    }
    const char *now = convar_string(convar);
    if (linear_wanted && strcmp(now, "0")) {
        // Pas pendant un menu : la configuration y est encore exécutée et
        // écraserait la valeur ; elle l'est avant l'entrée en partie.
        if (ui_visible) { response = now[0] ? (int)strtol(now, NULL, 10) : -1; return; }
        snprintf(saved_response, sizeof(saved_response), "%s", now);
        if (!set_convar(c, convar, "0")) {
            convar_broken = 1; saved_response[0] = 0;
            fprintf(stderr, "[ios] joy_response_move : écriture non vérifiée, réponse du jeu gardée\n");
        } else fprintf(stderr, "[ios] joy_response_move %s -> 0 (stick tactile linéaire)\n", saved_response);
    } else if (!linear_wanted && saved_response[0]) {
        if (!strcmp(now, "0")) set_convar(c, convar, saved_response);
        fprintf(stderr, "[ios] joy_response_move rendu : %s\n", convar_string(convar));
        saved_response[0] = 0;
    }
    now = convar_string(convar);
    response = now[0] ? (int)strtol(now, NULL, 10) : -1;
}

// ---- Commandes console exécutées au prochain relevé --------------------
// Trouvées par ICvar (FindCommandBase, comme les variables), puis leur rappel
// appelé directement avec un CCommand invité : ConCommand garde le rappel à
// +24 et ses drapeaux à +32 (bit 1 : rappel à arguments, bit 2 : interface) ;
// CCommand = argc, taille d'argv[0], deux tampons de 512 octets, 64 pointeurs.
// Diagnostics seulement (getpos ; P2_IOS_AUTOEXEC pour le banc). Une commande
// du serveur (setpos…) passe par « cmd », qui l'envoie comme un joueur.
static char pending_command[256];
void rt_ios_console_command(const char *line) { snprintf(pending_command, sizeof(pending_command), "%s", line); }
static void run_console_command(CPU *c, const char *line) {
    enum { ARGV = 64, LENGTH = 512, SIZE = 8 + 2 * LENGTH + 4 * ARGV };
    char name[64] = {0};
    sscanf(line, "%63s", name);
    if (!name[0] || strlen(line) >= LENGTH) return;
    if (!icvar) find_convar(c, "joy_response_move");   // initialise icvar
    if (icvar <= 1) return;
    uint32_t text = guest_text(name);
    if (!text) return;
    uint32_t args[2] = {icvar, text};
    uint32_t base = rt_invoke(c, R32(R32(icvar) + 13 * 4), args, 2);
    rt_free(text);
    if (!plausible(base) || !plausible(R32(base + 12)) || strcmp(GPTR(R32(base + 12)), name)) {
        fprintf(stderr, "[ios] commande %s introuvable\n", name); return;
    }
    uint32_t callback = R32(base + 24); uint8_t bits = *(uint8_t *)GPTR(base + 32);
    if (!plausible(callback) || (bits & 4)) { fprintf(stderr, "[ios] commande %s : rappel inattendu\n", name); return; }
    uint32_t command = rt_alloc(SIZE);
    if (!command) return;
    memset(GPTR(command), 0, SIZE);
    char *full = GPTR(command + 8), *argv = GPTR(command + 8 + LENGTH);
    snprintf(full, LENGTH, "%s", line);
    // Mots séparés par des espaces, chacun terminé par un zéro dans m_pArgvBuffer.
    uint32_t argc = 0, at = 0;
    for (const char *p = line; *p && argc < ARGV;) {
        while (*p == ' ') ++p;
        if (!*p) break;
        W32(command + 8 + 2 * LENGTH + 4 * argc, command + 8 + LENGTH + at);
        while (*p && *p != ' ' && at < LENGTH - 1) argv[at++] = *p++;
        argv[at++] = 0; ++argc;
    }
    W32(command, argc); W32(command + 4, (uint32_t)strlen(name));
    fprintf(stderr, "[ios] commande %s\n", line);
    if (bits & 2) rt_invoke(c, callback, &command, 1); else rt_invoke(c, callback, NULL, 0);
    rt_free(command);
}
// P2_IOS_AUTOEXEC="var=valeur;commande args;pause 2;…" : exécuté 5 s après la
// première entrée en partie (banc : sv_cheats=1;cmd setpos …;cmd setang …) ;
// « pause N » attend N secondes avant la suite.
static void autoexec(CPU *c) {
    static int done; static uint64_t resume; static char plan[1024]; static char *next, *save;
    if (done || ui_visible) return;
    uint64_t now = SDL_GetTicks64();
    if (!next) {
        const char *e = getenv("P2_IOS_AUTOEXEC");
        if (!e || !*e) { done = 1; return; }
        snprintf(plan, sizeof(plan), "%s", e);
        next = strtok_r(plan, ";", &save);
        resume = now + 5000;
    }
    while (next && now >= resume) {
        char *item = next; next = strtok_r(NULL, ";", &save);
        while (*item == ' ') ++item;
        char *equal = strchr(item, '=');
        double seconds;
        if (sscanf(item, "pause %lf", &seconds) == 1) resume = now + (uint64_t)(seconds * 1000);
        else if (equal && !strchr(item, ' ')) {
            *equal = 0;
            uint32_t convar = find_convar(c, item);
            int ok = convar && set_convar(c, convar, equal + 1);
            fprintf(stderr, "[ios] %s = %s : %s\n", item, equal + 1, ok ? "posé" : "échec");
        } else run_console_command(c, item);
    }
    if (!next) done = 1;
}

void rt_ios_game_state_poll(void) {
    uint64_t now = SDL_GetTicks64();
    if (now < next_query) return;
    next_query = now + 100;
    CPU *c = rt_foreign_thread_attach();   // thread invité courant : son CPU
    if (!c) return;
    if (!engine_vgui) {
        if (now < next_lookup) return;
        next_lookup = now + 1000;   // engine.so pas encore initialisé : réessayer
        const GuestModule *engine = rt_find_module("engine.so");
        uint32_t create = engine ? rt_module_symbol(engine, "CreateInterface") : 0;
        static uint32_t name;
        if (!name && (name = rt_alloc(sizeof("VEngineVGui001")))) memcpy(GPTR(name), "VEngineVGui001", sizeof("VEngineVGui001"));
        if (!create || !name) return;
        uint32_t args[2] = {name, 0};
        engine_vgui = rt_invoke(c, create, args, 2);
        if (engine_vgui) fprintf(stderr, "[ios] état du jeu : IEngineVGui à %#x\n", engine_vgui);
        if (!engine_vgui) return;
    }
    uint32_t method = R32(R32(engine_vgui) + 3 * 4);
    int visible = (rt_invoke(c, method, &engine_vgui, 1) & 0xff) != 0;
    if (visible != ui_visible) fprintf(stderr, "[ios] interface du jeu %s\n", visible ? "affichée" : "masquée");
    // En jeu, les commandes tactiles sont une manette : Source ouvre alors ses
    // menus en mode manette, curseur caché et souris ignorée. Un vrai
    // déplacement de souris le ramène en mode souris (runtime/sdl_input.c).
    if (visible && !ui_visible) rt_input_nudge_mouse();
    ui_visible = visible;
    if (linear_wanted || saved_response[0]) update_move_response(c);
    if (pending_command[0]) { char line[256]; snprintf(line, sizeof(line), "%s", pending_command); pending_command[0] = 0; run_console_command(c, line); }
    autoexec(c);
}
#endif
