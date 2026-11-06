// Pont Steam : interfaces C++ invitées (i386) servies par la libsteam_api.dylib officielle.
#pragma once
#include "rt.h"
#include <stdbool.h>
#include <stddef.h>

// Méthode d'interface : `self` est l'objet hôte, `args` l'adresse invitée du premier
// argument après `this`, `sret` le pointeur caché de retour de structure (ou 0).
typedef void (*steam_method_fn)(CPU *c, void *self, uint32_t args, uint32_t sret);
typedef struct { const char *version, *klass; uint32_t slots; } SteamInterfaceInfo;
typedef struct {
    const char *name;      // "SteamUser021::GetSteamID", trié par strcmp
    uint32_t iface, slot;  // index dans steam_interfaces, emplacement de vtable
    int sret;              // retour CSteamID/CGameID par pointeur caché (`ret 4`)
    steam_method_fn fn;    // NULL : piège, raison dans why
    const char *why;
} SteamMethodInfo;
extern const SteamInterfaceInfo steam_interfaces[];
extern const size_t steam_interface_count;
extern const SteamMethodInfo steam_methods[];
extern const size_t steam_method_count;

// Aides du code généré (steam.c).
void *steam_flat(CPU *c, const char *symbol);                // symbole de la dylib, piège s'il manque
void *steam_slot(CPU *c, void *self, uint32_t slot);         // emplacement de la vtable d'un objet hôte
const char *steam_in_string(CPU *c, uint32_t address);       // NULL si 0
void *steam_out(CPU *c, uint32_t address, uint32_t size);    // NULL si 0
// Tampons de count éléments : lus par Steam (contrôle de lecture) ou écrits (contrôle d'écriture).
void *steam_in_buffer(CPU *c, uint32_t address, int64_t count, uint32_t element);
void *steam_out_buffer(CPU *c, uint32_t address, int64_t count, uint32_t element);
void steam_ret64(CPU *c, uint64_t value);                    // edx:eax
void steam_ret_sret(CPU *c, uint32_t sret, uint64_t value);  // écrit *sret, eax = sret
uint32_t steam_guest_string(CPU *c, const char *value);      // copie invitée stable, 0 si NULL
uint32_t steam_proxy(CPU *c, void *host, const char *version);

// Introspection pour les tests.
const char *steam_dylib_path(void);
int steam_dylib_available(void);
uint32_t steam_vtable(CPU *c, const char *version);
void *steam_proxy_host(uint32_t proxy);
void steam_pop_hidden_pointer(CPU *c);
void steam_mask_ids(char *text, size_t length);   // masque les SteamID64 d'un texte
void steam_test_warning_hook(uint32_t guest_fn, int severity, const char *text);
void steam_deliver_warnings(CPU *c);   // file des avertissements des threads Steam
int steam_callback_guest_size(int32_t id, int32_t host_size);   // remplissage final i386 vs ARM64
