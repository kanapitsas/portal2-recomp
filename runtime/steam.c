// Pont Steam : remplace les exports Steam de libsteam_api.so invité par la
// libsteam_api.dylib officielle de Valve (SDK 1.53a, ARM64), chargée par dlopen
// depuis un chemin local jamais versionné (docs/steam.md). Toute réponse vient du
// vrai client Steam ; une méthode non traduite est un rt_trap nommé.
//
// - Interfaces C++ : l'invité reçoit des mandataires en mémoire invitée,
//   [vtable invitée][handle de l'objet hôte]. Chaque emplacement de vtable est une
//   adresse rt_dynamic_import("Version::Méthode") ; la méthode appelle la fonction
//   « flat » officielle (steam_generated.c, produit par tools/gen_steam.py).
// - Callbacks : SteamAPI_ManualDispatch_* pompe les callbacks hôtes ; les objets
//   CCallbackBase invités (vptr, uint8 flags @4, int m_iCallback @8 ; vtable Run,
//   Run(param, bIOFailure, hCall), GetCallbackSizeBytes) sont appelés par rt_invoke.
//   Les structures de callback sont pack(4) sur Linux comme sur macOS : copie octet
//   pour octet, après contrôle de la taille déclarée par l'objet invité.
// - Breakpad (UseBreakpadCrashHandler, SetMiniDumpComment, WriteMiniDump) : sans
//   effet, journalisé une fois, pour ne pas installer les gestionnaires de signaux
//   de breakpad par-dessus ceux du runtime.
#include "steam.h"
#include "modules.h"
#include <ctype.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <limits.h>
#include <mach-o/dyld.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// SHIMS: SteamAPI_InitSafe SteamAPI_Shutdown SteamAPI_RestartAppIfNecessary SteamAPI_RunCallbacks
// SHIMS: SteamAPI_RegisterCallback SteamAPI_UnregisterCallback SteamAPI_RegisterCallResult SteamAPI_UnregisterCallResult
// SHIMS: SteamAPI_GetHSteamPipe SteamAPI_GetHSteamUser SteamAPI_SetTryCatchCallbacks SteamAPI_GetSteamInstallPath
// SHIMS: SteamAPI_UseBreakpadCrashHandler SteamAPI_SetMiniDumpComment SteamAPI_WriteMiniDump
// SHIMS: SteamInternal_ContextInit SteamInternal_CreateInterface SteamInternal_FindOrCreateUserInterface
// SHIMS: SteamInternal_FindOrCreateGameServerInterface SteamInternal_GameServer_Init
// SHIMS: SteamGameServer_Shutdown SteamGameServer_RunCallbacks SteamGameServer_GetHSteamPipe
// SHIMS: SteamGameServer_GetHSteamUser SteamGameServer_GetIPCCallCount

#define KIND_INTERFACE 0x5354494du   // 'STIM'
#define OPAQUE_SLOTS 32u             // interfaces absentes du SDK 1.53a (GameCoordinator...)
#define CALLBACK_REGISTERED 0x01u
#define CALLBACK_GAMESERVER 0x02u
#define CALL_COMPLETED 703           // SteamAPICallCompleted_t::k_iCallback

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static void *library;
static char path_buffer[PATH_MAX];
static _Thread_local char message[512];

static uint32_t arg(CPU *c, unsigned i) { return R32(c->esp + 4 + 4 * i); }
static __attribute__((noreturn)) void fail(CPU *c, const char *format, const char *a, const char *b) {
    snprintf(message, sizeof(message), format, a, b);
    rt_trap(c, c->eip, message);
}

// ---------------------------------------------------------------- configuration
static int repository(char *out, size_t size) {
    char exe[PATH_MAX], real[PATH_MAX];
    uint32_t length = sizeof(exe);
    if (_NSGetExecutablePath(exe, &length) || !realpath(exe, real)) return -1;
    char *slash = strrchr(real, '/');
    if (!slash) return -1;
    *slash = 0;                                   // dossier de l'exécutable (build/)
    return snprintf(out, size, "%s/..", real) >= (int)size ? -1 : 0;
}

// Valeur (chaîne ou nombre) d'une clé de premier niveau de config/local.json.
static int config_value(const char *key, char *out, size_t size) {
    char root[PATH_MAX], file[PATH_MAX];
    if (repository(root, sizeof(root)) || snprintf(file, sizeof(file), "%s/config/local.json", root) >= (int)sizeof(file))
        return 0;
    FILE *f = fopen(file, "rb");
    if (!f) return 0;
    char text[16384];
    size_t n = fread(text, 1, sizeof(text) - 1, f);
    fclose(f);
    text[n] = 0;
    char quoted[128];
    snprintf(quoted, sizeof(quoted), "\"%s\"", key);
    const char *p = strstr(text, quoted);
    if (!p) return 0;
    p += strlen(quoted);
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') ++p;
    if (*p++ != ':') return 0;
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') ++p;
    size_t used = 0;
    if (*p == '"') {
        for (++p; *p && *p != '"' && used + 1 < size; ++p) {
            if (*p == '\\' && p[1]) ++p;
            out[used++] = *p;
        }
        if (*p != '"') return 0;
    } else {
        while ((*p == '-' || (*p >= '0' && *p <= '9')) && used + 1 < size) out[used++] = *p++;
        if (!used) return 0;
    }
    out[used] = 0;
    return 1;
}

const char *steam_dylib_path(void) {
    pthread_mutex_lock(&lock);
    if (!path_buffer[0]) {
        const char *env = getenv("P2_STEAM_API_DYLIB");
        char root[PATH_MAX];
        if (env && *env) snprintf(path_buffer, sizeof(path_buffer), "%s", env);
        else if (!config_value("steam_api_dylib", path_buffer, sizeof(path_buffer)) && !repository(root, sizeof(root)))
            snprintf(path_buffer, sizeof(path_buffer),
                     "%s/../work/downloads/steamworks/steam_api.bundle/Contents/MacOS/libsteam_api.dylib", root);
    }
    pthread_mutex_unlock(&lock);
    return path_buffer;
}

static void *open_library(void) {
    void *handle = __atomic_load_n(&library, __ATOMIC_ACQUIRE);
    if (handle) return handle;
    const char *path = steam_dylib_path();
    handle = *path ? dlopen(path, RTLD_NOW | RTLD_LOCAL) : NULL;
    if (handle) {
        void *expected = NULL;
        if (!__atomic_compare_exchange_n(&library, &expected, handle, 0, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)) {
            dlclose(handle); handle = expected;
        }
    }
    return handle;
}

int steam_dylib_available(void) { return open_library() != NULL; }

void *steam_flat(CPU *c, const char *symbol) {
    void *handle = open_library();
    if (!handle) fail(c, "Steam: cannot load the official libsteam_api.dylib (%s; config key steam_api_dylib): %s",
                      steam_dylib_path(), dlerror());
    void *fn = dlsym(handle, symbol);
    if (!fn) fail(c, "Steam: %s is absent from %s", symbol, steam_dylib_path());
    return fn;
}
#define FLAT(type, name) ((type)steam_flat(c, name))

// ---------------------------------------------------------------- aides du code généré
const char *steam_in_string(CPU *c, uint32_t address) {
    if (!address) return NULL;
    rt_string_length(c, address);
    return GPTR(address);
}
void *steam_out(CPU *c, uint32_t address, uint32_t size) {
    return address ? rt_write_span(c, address, size) : NULL;
}
static uint64_t buffer_size(CPU *c, uint32_t address, int64_t count, uint32_t element) {
    if (address && count < 0) rt_trap(c, c->eip, "Steam: negative buffer size");
    return address ? (uint64_t)count * element : 0;
}
void *steam_in_buffer(CPU *c, uint32_t address, int64_t count, uint32_t element) {
    uint64_t size = buffer_size(c, address, count, element);
    return address ? rt_span(c, address, size) : NULL;
}
void *steam_out_buffer(CPU *c, uint32_t address, int64_t count, uint32_t element) {
    uint64_t size = buffer_size(c, address, count, element);
    return address ? rt_write_span(c, address, size) : NULL;
}
void steam_ret64(CPU *c, uint64_t value) { c->eax = (uint32_t)value; c->edx = (uint32_t)(value >> 32); }
void steam_ret_sret(CPU *c, uint32_t sret, uint64_t value) {
    memcpy(rt_write_span(c, sret, 8), &value, 8);
    c->eax = sret;
}
// Retour de structure i386 Linux : l'appelé dépile le pointeur caché (`ret 4`).
// rt_return ne dépilera que l'adresse de retour : on la remonte d'un mot.
void steam_pop_hidden_pointer(CPU *c) {
    uint32_t return_address = R32(c->esp);
    W32(c->esp + 4, return_address);
    c->esp += 4;
}

// Chaînes rendues par Steam : copie invitée internée (valide pour toute la session).
typedef struct Interned { struct Interned *next; uint32_t guest; char text[]; } Interned;
static Interned *interned[1024];
uint32_t steam_guest_string(CPU *c, const char *value) {
    if (!value) return 0;
    size_t length = strlen(value);
    uint32_t hash = 2166136261u;
    for (size_t i = 0; i < length; ++i) hash = (hash ^ (uint8_t)value[i]) * 16777619u;
    pthread_mutex_lock(&lock);
    Interned **bucket = &interned[hash % 1024], *item = *bucket;
    while (item && strcmp(item->text, value)) item = item->next;
    if (!item && length < UINT32_MAX && (item = malloc(sizeof(*item) + length + 1))) {
        item->guest = rt_alloc((uint32_t)length + 1);
        if (!item->guest) { free(item); item = NULL; }
        else {
            memcpy(item->text, value, length + 1);
            memcpy(GPTR(item->guest), value, length + 1);
            item->next = *bucket; *bucket = item;
        }
    }
    uint32_t result = item ? item->guest : 0;
    pthread_mutex_unlock(&lock);
    if (!result) rt_trap(c, c->eip, "Steam: guest string allocation failed");
    return result;
}

// ---------------------------------------------------------------- mandataires d'interface
typedef struct { char *version; uint32_t slots, vtable; } Interface;
static Interface *extra;          // interfaces sans table (au-delà de steam_interface_count)
static size_t extra_count;
static uint32_t *vtables;         // une vtable invitée par interface générée
typedef struct { void *host; uint32_t iface, proxy; } Proxy;
static Proxy *proxies;
static size_t proxy_count, proxy_capacity;

static size_t interface_count(void) { return steam_interface_count + extra_count; }
static const char *interface_version(size_t i) {
    return i < steam_interface_count ? steam_interfaces[i].version : extra[i - steam_interface_count].version;
}
static uint32_t interface_slots(size_t i) {
    return i < steam_interface_count ? steam_interfaces[i].slots : extra[i - steam_interface_count].slots;
}
static uint32_t *interface_vtable(size_t i) {
    return i < steam_interface_count ? &vtables[i] : &extra[i - steam_interface_count].vtable;
}
// Index de l'interface (lock tenu) ; length borne la comparaison, create ajoute une opaque.
static long find_interface(const char *version, size_t length, int create) {
    for (size_t i = 0; i < interface_count(); ++i) {
        const char *v = interface_version(i);
        if (strlen(v) == length && !memcmp(v, version, length)) return (long)i;
    }
    if (!create) return -1;
    Interface *grown = realloc(extra, (extra_count + 1) * sizeof(*grown));
    if (!grown) return -1;
    extra = grown;
    char *copy = strndup(version, length);
    if (!copy) return -1;
    extra[extra_count++] = (Interface){copy, OPAQUE_SLOTS, 0};
    return (long)interface_count() - 1;
}

static int compare_method(const void *key, const void *item) {
    return strcmp((const char *)key, ((const SteamMethodInfo *)item)->name);
}
static const SteamMethodInfo *find_method(const char *name) {
    return bsearch(name, steam_methods, steam_method_count, sizeof(*steam_methods), compare_method);
}

// Vtable invitée de l'interface i (lock tenu).
static uint32_t build_vtable(CPU *c, size_t i) {
    if (!vtables && !(vtables = calloc(steam_interface_count, sizeof(*vtables))))
        rt_trap(c, c->eip, "Steam: out of memory");
    uint32_t *vtable = interface_vtable(i);
    if (*vtable) return *vtable;
    uint32_t slots = interface_slots(i), table = rt_alloc(slots * 4);
    if (!table) rt_trap(c, c->eip, "Steam: guest vtable allocation failed");
    for (uint32_t s = 0; s < slots; ++s) {
        char name[160];
        snprintf(name, sizeof(name), "%s::slot%u", interface_version(i), s);
        if (i < steam_interface_count)
            for (size_t m = 0; m < steam_method_count; ++m)
                if (steam_methods[m].iface == i && steam_methods[m].slot == s) { snprintf(name, sizeof(name), "%s", steam_methods[m].name); break; }
        uint32_t address = rt_dynamic_import(name);
        if (!address) rt_trap(c, c->eip, "Steam: dynamic import table is full");
        W32(table + 4 * s, address);
    }
    return *vtable = table;
}

uint32_t steam_vtable(CPU *c, const char *version) {
    pthread_mutex_lock(&lock);
    long i = find_interface(version, strlen(version), 1);
    uint32_t table = i < 0 ? 0 : build_vtable(c, (size_t)i);
    pthread_mutex_unlock(&lock);
    return table;
}

uint32_t steam_proxy(CPU *c, void *host, const char *version) {
    if (!host) return 0;
    if (!version) rt_trap(c, c->eip, "Steam: interface without version string");
    pthread_mutex_lock(&lock);
    long i = find_interface(version, strlen(version), 1);
    if (i < 0) { pthread_mutex_unlock(&lock); rt_trap(c, c->eip, "Steam: out of memory"); }
    for (size_t p = 0; p < proxy_count; ++p)
        if (proxies[p].host == host && proxies[p].iface == (uint32_t)i) {
            uint32_t found = proxies[p].proxy; pthread_mutex_unlock(&lock); return found;
        }
    uint32_t table = build_vtable(c, (size_t)i), object = rt_alloc(8), handle = rt_handle_new(host, KIND_INTERFACE);
    if (proxy_count == proxy_capacity) {
        size_t capacity = proxy_capacity ? proxy_capacity * 2 : 64;
        Proxy *grown = realloc(proxies, capacity * sizeof(*grown));
        if (grown) { proxies = grown; proxy_capacity = capacity; }
    }
    if (!object || !handle || proxy_count == proxy_capacity) {
        pthread_mutex_unlock(&lock); rt_trap(c, c->eip, "Steam: proxy allocation failed");
    }
    W32(object, table);
    W32(object + 4, handle);
    proxies[proxy_count++] = (Proxy){host, (uint32_t)i, object};
    pthread_mutex_unlock(&lock);
    return object;
}

void *steam_proxy_host(uint32_t proxy) {
    if (!proxy) return NULL;
    return rt_handle_get(R32(proxy + 4), KIND_INTERFACE);
}

// Interfaces absentes de l'API flat mais servies par steamclient : appel de la
// méthode virtuelle de l'objet hôte (ABI C++ ARM64 : this en premier argument).
// ISteamGameCoordinator001 : SendMessage, IsMessageAvailable, RetrieveMessage.
static int virtual_bridge(CPU *c, const char *name, uint32_t table, long iface) {
    unsigned slot;
    if (!strcmp(name, "SteamInput006::slot17") || !strcmp(name, "SteamInput006::slot21") ||
        !strcmp(name, "SteamInput006::slot29")) {
        // Steam Input returns these structures by value. C++ i386 uses a
        // hidden result pointer even for the two-byte digital result; ARM64
        // uses its own structure-return ABI. The compiler translates the host
        // call when the function pointer has the actual return type.
        typedef struct __attribute__((packed)) { uint8_t state, active; } Digital;
        typedef struct __attribute__((packed)) { int32_t mode; float x, y; uint8_t active; } Analog;
        typedef struct { float value[10]; } Motion;
        uint32_t out = arg(c, 0), object = arg(c, 1), a = c->esp + 12;
        void *host = object && R32(object) == table ? steam_proxy_host(object) : NULL;
        if (!host) fail(c, "Steam: %s called on an object that is not a %s proxy", name, interface_version((size_t)iface));
        uint64_t pad = R64(a);
        void **methods = *(void ***)host;
        if (!strcmp(name, "SteamInput006::slot17")) {
            typedef Digital (*Fn)(void *, uint64_t, uint64_t);
            Digital value = ((Fn)methods[17])(host, pad, R64(a + 8));
            memcpy(rt_write_span(c, out, sizeof(value)), &value, sizeof(value));
        } else if (!strcmp(name, "SteamInput006::slot21")) {
            typedef Analog (*Fn)(void *, uint64_t, uint64_t);
            Analog value = ((Fn)methods[21])(host, pad, R64(a + 8));
            memcpy(rt_write_span(c, out, sizeof(value)), &value, sizeof(value));
        } else {
            typedef Motion (*Fn)(void *, uint64_t);
            Motion value = ((Fn)methods[29])(host, pad);
            memcpy(rt_write_span(c, out, sizeof(value)), &value, sizeof(value));
        }
        c->eax = out;
        steam_pop_hidden_pointer(c);
        return 1;
    }
    if (!strcmp(name, "SteamUtils010::slot7")) {
        // GetCSERIPPort(uint32 *ip, uint16 *port) -> bool : obsolète, sans
        // fonction flat, toujours dans la vtable du client (lu au chargement de carte).
        uint32_t object = R32(c->esp + 4), args = c->esp + 8;
        void *host = object && R32(object) == table ? steam_proxy_host(object) : NULL;
        if (!host) fail(c, "Steam: %s called on an object that is not a %s proxy", name, interface_version((size_t)iface));
        typedef bool (*Get)(void *, uint32_t *, uint16_t *);
        c->eax = ((Get)(*(void ***)host)[7])(host, steam_out(c, R32(args), 4), steam_out(c, R32(args + 4), 2)) ? 1u : 0u;
        return 1;
    }
    if (sscanf(name, "SteamGameStats001::slot%u", &slot) == 1 && slot <= 12) {
        // ISteamGameStats001 (isteamgamestats.h, SDK historique ; ordre vérifié
        // pour GetNewSession au site d'appel de client.so). uint64 invités = 2 mots.
        uint32_t object = R32(c->esp + 4), a = c->esp + 8;
        void *host = object && R32(object) == table ? steam_proxy_host(object) : NULL;
        if (!host) fail(c, "Steam: %s called on an object that is not a %s proxy", name, interface_version((size_t)iface));
        void *fn = (*(void ***)host)[slot];
        #define Q(o) ((uint64_t)R32(a + (o)) | (uint64_t)R32(a + (o) + 4) << 32)
        #define S(o) steam_in_string(c, R32(a + (o)))
        #define F(o) ({ uint32_t bits_ = R32(a + (o)); float f_; memcpy(&f_, &bits_, 4); f_; })
        switch (slot) {
        case 0: { typedef uint64_t (*T)(void *, int8_t, uint64_t, int32_t, uint32_t);   // GetNewSession
            steam_ret64(c, ((T)fn)(host, (int8_t)R32(a), Q(4), (int32_t)R32(a + 12), R32(a + 16))); break; }
        case 1: { typedef uint64_t (*T)(void *, uint64_t, uint32_t, int32_t);           // EndSession
            steam_ret64(c, ((T)fn)(host, Q(0), R32(a + 8), (int32_t)R32(a + 12))); break; }
        case 2: case 8: { typedef int32_t (*T)(void *, uint64_t, const char *, int32_t); // Add{Session,Row}AttributeInt
            c->eax = (uint32_t)((T)fn)(host, Q(0), S(8), (int32_t)R32(a + 12)); break; }
        case 3: case 9: { typedef int32_t (*T)(void *, uint64_t, const char *, const char *); // ...String
            c->eax = (uint32_t)((T)fn)(host, Q(0), S(8), S(12)); break; }
        case 4: case 10: { typedef int32_t (*T)(void *, uint64_t, const char *, float);  // ...Float
            c->eax = (uint32_t)((T)fn)(host, Q(0), S(8), F(12)); break; }
        case 5: { typedef int32_t (*T)(void *, uint64_t *, uint64_t, const char *);      // AddNewRow
            uint64_t row = 0; uint32_t out = R32(a);
            c->eax = (uint32_t)((T)fn)(host, out ? &row : NULL, Q(4), S(12));
            if (out) { W32(out, (uint32_t)row); W32(out + 4, (uint32_t)(row >> 32)); }
            break; }
        case 6: case 7: { typedef int32_t (*T)(void *, uint64_t);                        // CommitRow, CommitOutstandingRows
            c->eax = (uint32_t)((T)fn)(host, Q(0)); break; }
        default: { typedef int32_t (*T)(void *, uint64_t, const char *, int64_t);        // Add{Session,Row}AttributeInt64
            c->eax = (uint32_t)((T)fn)(host, Q(0), S(8), (int64_t)Q(12)); break; }
        }
        #undef Q
        #undef S
        #undef F
        return 1;
    }
    if (sscanf(name, "SteamGameCoordinator001::slot%u", &slot) != 1 || slot > 2) return 0;
    uint32_t object = R32(c->esp + 4), args = c->esp + 8;
    void *host = object && R32(object) == table ? steam_proxy_host(object) : NULL;
    if (!host) fail(c, "Steam: %s called on an object that is not a %s proxy", name, interface_version((size_t)iface));
    void **methods = *(void ***)host;
    if (slot == 0) {
        typedef int32_t (*Send)(void *, uint32_t, const void *, uint32_t);
        uint32_t size = R32(args + 8);
        c->eax = (uint32_t)((Send)methods[0])(host, R32(args), steam_in_buffer(c, R32(args + 4), size, 1), size);
    } else if (slot == 1) {
        typedef bool (*Available)(void *, uint32_t *);
        c->eax = ((Available)methods[1])(host, steam_out(c, R32(args), 4)) ? 1u : 0u;
    } else {
        typedef int32_t (*Retrieve)(void *, uint32_t *, void *, uint32_t, uint32_t *);
        uint32_t capacity = R32(args + 8);
        c->eax = (uint32_t)((Retrieve)methods[2])(host, steam_out(c, R32(args), 4),
                                                  steam_out_buffer(c, R32(args + 4), capacity, 1), capacity,
                                                  steam_out(c, R32(args + 12), 4));
    }
    return 1;
}

// Appel invité d'un emplacement de vtable : "Version::Méthode" ou "Version::slotN".
static int method_shim(CPU *c, const char *name) {
    const char *separator = strstr(name, "::");
    if (!separator) return 0;
    pthread_mutex_lock(&lock);
    long iface = find_interface(name, (size_t)(separator - name), 0);
    uint32_t table = iface < 0 || ((size_t)iface < steam_interface_count && !vtables) ? 0 : *interface_vtable((size_t)iface);
    pthread_mutex_unlock(&lock);
    if (iface < 0) return 0;
    // Diagnostic P2_STEAM_INPUT=0 : le jeu ne démarre jamais Steam Input et n'y
    // voit aucune manette ; il ne lit alors les manettes que par SDL. Sert à
    // savoir si l'initialisation de Steam Input coupe les rapports HID au jeu.
    const char *steam_input = getenv("P2_STEAM_INPUT");
    if (steam_input && *steam_input == '0' &&
        (!strcmp(name, "SteamInput006::Init") || !strcmp(name, "SteamInput006::GetConnectedControllers") ||
         !strcmp(name, "SteamController008::Init") || !strcmp(name, "SteamController008::GetConnectedControllers"))) {
        c->eax = 0;
        return 1;
    }
    const SteamMethodInfo *m = find_method(name);
    if (!m && virtual_bridge(c, name, table, iface)) return 1;
    if (!m) fail(c, "Steam: %s has no flat function in SDK 1.53a%s", name, "");
    if (!m->fn) {
        snprintf(message, sizeof(message), "Steam: %s (vtable slot %u) is not bridged: %s", name, m->slot, m->why);
        rt_trap(c, c->eip, message);
    }
    uint32_t sret = 0, self = c->esp + 4;
    if (m->sret) { sret = R32(self); self += 4; }
    uint32_t object = R32(self);
    void *host = object && R32(object) == table ? steam_proxy_host(object) : NULL;
    if (!host) fail(c, "Steam: %s called on an object that is not a %s proxy", name, interface_version((size_t)iface));
    m->fn(c, host, self + 4, sret);
    if (m->sret) steam_pop_hidden_pointer(c);
    return 1;
}

// ---------------------------------------------------------------- contextes d'accesseurs
// SteamInternal_ContextInit(data) : data = {pFn, compteur, contexte...}. Le vrai
// compteur est tenu par la dylib : on lui passe un double hôte par bloc invité, et
// s'il appelle son pFn, on appelle le pFn invité sur le contexte invité.
typedef struct { void (*fn)(void *); uintptr_t counter; void *context[32]; } HostContext;
typedef struct { uint32_t guest; HostContext *host; } ContextPair;
static ContextPair *contexts;
static size_t context_count;
static _Thread_local int context_refresh;
static void host_context_fn(void *context) { (void)context; context_refresh = 1; }

static uint32_t context_init(CPU *c, uint32_t data) {
    rt_span(c, data, 12);
    pthread_mutex_lock(&lock);
    HostContext *host = NULL;
    for (size_t i = 0; i < context_count && !host; ++i) if (contexts[i].guest == data) host = contexts[i].host;
    if (!host) {
        ContextPair *grown = realloc(contexts, (context_count + 1) * sizeof(*grown));
        if (grown) contexts = grown;
        if (grown && (host = calloc(1, sizeof(*host)))) {
            host->fn = host_context_fn;
            contexts[context_count++] = (ContextPair){data, host};
        }
    }
    pthread_mutex_unlock(&lock);
    if (!host) rt_trap(c, c->eip, "Steam: out of memory");
    context_refresh = 0;
    FLAT(void *(*)(void *), "SteamInternal_ContextInit")(host);
    if (context_refresh) {
        uint32_t context = data + 8;
        rt_invoke(c, R32(data), &context, 1);
        W32(data + 4, (uint32_t)host->counter);
    }
    return data + 8;
}

// ---------------------------------------------------------------- callbacks
typedef struct { uint32_t object; int32_t id; } Registered;
typedef struct { uint32_t object; uint64_t call; } Pending;
static Registered *callbacks;
static size_t callback_count, callback_capacity;
static Pending *results;
static size_t result_count, result_capacity;
static int dispatching[2];
static int manual_dispatch;

static int grow(void **items, size_t *capacity, size_t count, size_t size) {
    if (count < *capacity) return 0;
    size_t next = *capacity ? *capacity * 2 : 32;
    void *grown = realloc(*items, next * size);
    if (!grown) return -1;
    *items = grown; *capacity = next;
    return 0;
}

static void register_callback(CPU *c, uint32_t object, int32_t id) {
    rt_write_span(c, object, 12);
    W8(object + 4, R8(object + 4) | CALLBACK_REGISTERED);
    W32(object + 8, (uint32_t)id);
    pthread_mutex_lock(&lock);
    size_t i = 0;
    while (i < callback_count && callbacks[i].object != object) ++i;
    if (i == callback_count && grow((void **)&callbacks, &callback_capacity, callback_count, sizeof(*callbacks))) {
        pthread_mutex_unlock(&lock); rt_trap(c, c->eip, "Steam: out of memory");
    }
    callbacks[i] = (Registered){object, id};
    if (i == callback_count) ++callback_count;
    pthread_mutex_unlock(&lock);
}
static void unregister_callback(CPU *c, uint32_t object) {
    rt_write_span(c, object, 12);
    W8(object + 4, R8(object + 4) & ~CALLBACK_REGISTERED);
    pthread_mutex_lock(&lock);
    for (size_t i = 0; i < callback_count; ++i)
        if (callbacks[i].object == object) { callbacks[i] = callbacks[--callback_count]; break; }
    pthread_mutex_unlock(&lock);
}
static void register_result(CPU *c, uint32_t object, uint64_t call) {
    rt_span(c, object, 12);
    pthread_mutex_lock(&lock);
    if (grow((void **)&results, &result_capacity, result_count, sizeof(*results))) {
        pthread_mutex_unlock(&lock); rt_trap(c, c->eip, "Steam: out of memory");
    }
    results[result_count++] = (Pending){object, call};
    pthread_mutex_unlock(&lock);
}
static void unregister_result(uint32_t object, uint64_t call) {
    pthread_mutex_lock(&lock);
    for (size_t i = 0; i < result_count; ++i)
        if (results[i].object == object && results[i].call == call) { results[i] = results[--result_count]; break; }
    pthread_mutex_unlock(&lock);
}

// Callbacks dont la disposition i386 (uint64 alignés sur 4) ne diffère de
// l'hôte ARM64 (alignés sur 8) que par le remplissage final : mêmes champs aux
// mêmes décalages, on recopie seulement la taille invitée.
static const struct { int32_t id, host, guest; } tail_padding[] = {
    {2002, 16, 12},   // GameStatsSessionClosed_t { uint64 m_ulSessionID; EResult m_eResult; }
};
int steam_callback_guest_size(int32_t id, int32_t host_size) {
    for (size_t i = 0; i < sizeof(tail_padding) / sizeof(*tail_padding); ++i)
        if (tail_padding[i].id == id && tail_padding[i].host == host_size) return tail_padding[i].guest;
    return host_size;
}

// Appelle Run sur l'objet invité après avoir vérifié GetCallbackSizeBytes.
static void deliver(CPU *c, uint32_t object, int32_t id, const void *data, int32_t size,
                    int result, int failed, uint64_t call) {
    uint32_t vtable = R32(object);
    int32_t expected = (int32_t)rt_invoke(c, R32(vtable + 8), &object, 1);
    if (expected != size && expected == steam_callback_guest_size(id, size)) size = expected;
    if (expected != size) {
        snprintf(message, sizeof(message), "Steam: callback %d is %d bytes on the host but the guest object expects %d",
                 id, size, expected);
        rt_trap(c, c->eip, message);
    }
    uint32_t buffer = rt_alloc(size > 0 ? (uint32_t)size : 1);
    if (!buffer) rt_trap(c, c->eip, "Steam: callback buffer allocation failed");
    if (size > 0) memcpy(GPTR(buffer), data, (size_t)size);
    if (result) {
        uint32_t args[] = {object, buffer, failed ? 1u : 0u, (uint32_t)call, (uint32_t)(call >> 32)};
        rt_invoke(c, R32(vtable + 4), args, 5);
    } else {
        uint32_t args[] = {object, buffer};
        rt_invoke(c, R32(vtable), args, 2);
    }
    rt_free(buffer);
}

static void dispatch_callback(CPU *c, int32_t id, const void *data, int32_t size, int server) {
    pthread_mutex_lock(&lock);
    size_t count = 0;
    uint32_t *targets = callback_count ? malloc(callback_count * sizeof(*targets)) : NULL;
    for (size_t i = 0; targets && i < callback_count; ++i)
        if (callbacks[i].id == id && !!(R8(callbacks[i].object + 4) & CALLBACK_GAMESERVER) == server)
            targets[count++] = callbacks[i].object;
    pthread_mutex_unlock(&lock);
    for (size_t t = 0; t < count; ++t) {
        pthread_mutex_lock(&lock);
        int alive = 0;   // un gestionnaire précédent a pu désinscrire celui-ci
        for (size_t i = 0; i < callback_count && !alive; ++i) alive = callbacks[i].object == targets[t] && callbacks[i].id == id;
        pthread_mutex_unlock(&lock);
        if (alive) deliver(c, targets[t], id, data, size, 0, 0, 0);
    }
    free(targets);
}

static void dispatch_result(CPU *c, uint64_t call, int32_t id, const void *data, int32_t size, int failed) {
    for (;;) {
        pthread_mutex_lock(&lock);
        uint32_t object = 0;
        for (size_t i = 0; i < result_count && !object; ++i)
            if (results[i].call == call) { object = results[i].object; results[i] = results[--result_count]; }
        pthread_mutex_unlock(&lock);
        if (!object) return;
        deliver(c, object, id, data, size, 1, failed, call);
    }
}

typedef struct { int32_t user, id; uint8_t *param; int32_t size; } CallbackMsg;
static void run_callbacks(CPU *c, int server) {
    if (!manual_dispatch || dispatching[server]) return;   // pas de ré-entrance, comme la dylib
    int32_t pipe = server ? FLAT(int32_t (*)(void), "SteamGameServer_GetHSteamPipe")()
                          : FLAT(int32_t (*)(void), "SteamAPI_GetHSteamPipe")();
    if (!pipe) return;
    dispatching[server] = 1;
    FLAT(void (*)(int32_t), "SteamAPI_ManualDispatch_RunFrame")(pipe);
    bool (*next)(int32_t, CallbackMsg *) = FLAT(bool (*)(int32_t, CallbackMsg *), "SteamAPI_ManualDispatch_GetNextCallback");
    void (*release)(int32_t) = FLAT(void (*)(int32_t), "SteamAPI_ManualDispatch_FreeLastCallback");
    bool (*result)(int32_t, uint64_t, void *, int32_t, int32_t, bool *) =
        FLAT(bool (*)(int32_t, uint64_t, void *, int32_t, int32_t, bool *), "SteamAPI_ManualDispatch_GetAPICallResult");
    CallbackMsg msg;
    while (next(pipe, &msg)) {
        if (msg.id == CALL_COMPLETED && msg.size >= 16) {
            uint64_t call; int32_t id; uint32_t size;
            memcpy(&call, msg.param, 8); memcpy(&id, msg.param + 8, 4); memcpy(&size, msg.param + 12, 4);
            void *buffer = malloc(size ? size : 1);
            bool failed = false;
            if (buffer && result(pipe, call, buffer, (int32_t)size, id, &failed))
                dispatch_result(c, call, id, buffer, (int32_t)size, failed);
            free(buffer);
        }
        dispatch_callback(c, msg.id, msg.param, msg.size, server);
        release(pipe);
    }
    dispatching[server] = 0;
}

// ---------------------------------------------------------------- confidentialité
// steamclient.dylib écrit le SteamID64 du compte sur stderr pendant l'initialisation.
// On capture stderr le temps de l'appel et on masque tout SteamID64 (7656119 + 10
// chiffres) avant de le réémettre, pour qu'il n'atteigne pas les journaux.
typedef struct { int saved, pipe; } Capture;
static void capture_begin(Capture *k) {
    int ends[2];
    k->saved = k->pipe = -1;
    fflush(stderr);
    if (pipe(ends)) return;
    fcntl(ends[0], F_SETFL, O_NONBLOCK); fcntl(ends[1], F_SETFL, O_NONBLOCK);   // jamais de blocage
    k->saved = dup(2);
    if (k->saved < 0 || dup2(ends[1], 2) < 0) {
        if (k->saved >= 0) close(k->saved);
        close(ends[0]); close(ends[1]); k->saved = -1; return;
    }
    close(ends[1]);
    k->pipe = ends[0];
}
void steam_mask_ids(char *text, size_t length) {
    for (size_t i = 0; i + 17 <= length; ++i) {
        if (memcmp(text + i, "7656119", 7) || (i && isdigit((unsigned char)text[i - 1]))) continue;
        size_t digits = 7;
        while (i + digits < length && isdigit((unsigned char)text[i + digits])) ++digits;
        if (digits == 17) memset(text + i, 'X', 17);
    }
}
static void capture_end(Capture *k) {
    if (k->saved < 0) return;
    fflush(stderr);
    dup2(k->saved, 2); close(k->saved);
    char buffer[65536];
    size_t used = 0;
    ssize_t n;
    while (used < sizeof(buffer) && (n = read(k->pipe, buffer + used, sizeof(buffer) - used)) > 0) used += (size_t)n;
    close(k->pipe);
    steam_mask_ids(buffer, used);
    fwrite(buffer, 1, used, stderr);
}

// ---------------------------------------------------------------- imports invités
static void enable_manual_dispatch(CPU *c) {
    if (manual_dispatch) return;
    FLAT(void (*)(void), "SteamAPI_ManualDispatch_Init")();
    manual_dispatch = 1;
}

static void log_once(int *done, const char *what) {
    if (!*done) { *done = 1; fprintf(stderr, "steam: %s ignored (breakpad stays disabled)\n", what); }
}

static int steam_shim(CPU *c, const char *name) {
    static int breakpad_logged, comment_logged, minidump_logged;
    if (strstr(name, "::")) return method_shim(c, name);
    if (strncmp(name, "Steam", 5)) return 0;
    if (!strcmp(name, "SteamAPI_InitSafe")) {
        // Mécanisme documenté par Valve : SteamAppId désigne le jeu ; le client
        // vérifie toujours la possession. Pris dans config/local.json s'il manque.
        char appid[32];
        if (!getenv("SteamAppId") && config_value("steam_appid", appid, sizeof(appid))) {
            setenv("SteamAppId", appid, 0); setenv("SteamGameId", appid, 0);
            fprintf(stderr, "steam: SteamAppId=%s from config/local.json\n", appid);
        }
        bool (*init)(void) = FLAT(bool (*)(void), "SteamAPI_InitSafe");
        Capture capture; capture_begin(&capture);
        bool ok = init();
        capture_end(&capture);
        if (ok) enable_manual_dispatch(c);
        c->eax = ok;
    } else if (!strcmp(name, "SteamAPI_Shutdown")) {
        FLAT(void (*)(void), "SteamAPI_Shutdown")();
    } else if (!strcmp(name, "SteamAPI_RestartAppIfNecessary")) {
        c->eax = FLAT(bool (*)(uint32_t), "SteamAPI_RestartAppIfNecessary")(arg(c, 0));
    } else if (!strcmp(name, "SteamAPI_RunCallbacks")) {
        run_callbacks(c, 0);
    } else if (!strcmp(name, "SteamGameServer_RunCallbacks")) {
        run_callbacks(c, 1);
    } else if (!strcmp(name, "SteamAPI_RegisterCallback")) {
        register_callback(c, arg(c, 0), (int32_t)arg(c, 1));
    } else if (!strcmp(name, "SteamAPI_UnregisterCallback")) {
        unregister_callback(c, arg(c, 0));
    } else if (!strcmp(name, "SteamAPI_RegisterCallResult")) {
        register_result(c, arg(c, 0), R64(c->esp + 8));
    } else if (!strcmp(name, "SteamAPI_UnregisterCallResult")) {
        unregister_result(arg(c, 0), R64(c->esp + 8));
    } else if (!strcmp(name, "SteamAPI_GetHSteamPipe")) {
        c->eax = (uint32_t)FLAT(int32_t (*)(void), "SteamAPI_GetHSteamPipe")();
    } else if (!strcmp(name, "SteamAPI_GetHSteamUser")) {
        c->eax = (uint32_t)FLAT(int32_t (*)(void), "SteamAPI_GetHSteamUser")();
    } else if (!strcmp(name, "SteamGameServer_GetHSteamPipe")) {
        c->eax = (uint32_t)FLAT(int32_t (*)(void), "SteamGameServer_GetHSteamPipe")();
    } else if (!strcmp(name, "SteamGameServer_GetHSteamUser")) {
        c->eax = (uint32_t)FLAT(int32_t (*)(void), "SteamGameServer_GetHSteamUser")();
    } else if (!strcmp(name, "SteamGameServer_GetIPCCallCount")) {
        c->eax = FLAT(uint32_t (*)(void), "SteamGameServer_GetIPCCallCount")();
    } else if (!strcmp(name, "SteamGameServer_Shutdown")) {
        FLAT(void (*)(void), "SteamGameServer_Shutdown")();
    } else if (!strcmp(name, "SteamAPI_SetTryCatchCallbacks")) {
        FLAT(void (*)(bool), "SteamAPI_SetTryCatchCallbacks")(R8(c->esp + 4) != 0);
    } else if (!strcmp(name, "SteamAPI_GetSteamInstallPath")) {
        c->eax = steam_guest_string(c, FLAT(const char *(*)(void), "SteamAPI_GetSteamInstallPath")());
    } else if (!strcmp(name, "SteamAPI_UseBreakpadCrashHandler")) {
        log_once(&breakpad_logged, "SteamAPI_UseBreakpadCrashHandler");
    } else if (!strcmp(name, "SteamAPI_SetMiniDumpComment")) {
        log_once(&comment_logged, "SteamAPI_SetMiniDumpComment");
    } else if (!strcmp(name, "SteamAPI_WriteMiniDump")) {
        log_once(&minidump_logged, "SteamAPI_WriteMiniDump");
    } else if (!strcmp(name, "SteamInternal_ContextInit")) {
        c->eax = context_init(c, arg(c, 0));
    } else if (!strcmp(name, "SteamInternal_CreateInterface")) {
        const char *version = steam_in_string(c, arg(c, 0));
        void *host = FLAT(void *(*)(const char *), "SteamInternal_CreateInterface")(version);
        c->eax = steam_proxy(c, host, version);
    } else if (!strcmp(name, "SteamInternal_FindOrCreateUserInterface") ||
               !strcmp(name, "SteamInternal_FindOrCreateGameServerInterface")) {
        const char *version = steam_in_string(c, arg(c, 1));
        void *host = FLAT(void *(*)(int32_t, const char *), name)((int32_t)arg(c, 0), version);
        c->eax = steam_proxy(c, host, version);
    } else if (!strcmp(name, "SteamInternal_GameServer_Init")) {
        // (uint32 unIP, uint16 usLegacySteamPort, uint16 usGamePort, uint16 usQueryPort,
        //  EServerMode eServerMode, const char *pchVersionString) ; six arguments dans la dylib.
        bool (*init)(uint32_t, uint16_t, uint16_t, uint16_t, int32_t, const char *) =
            FLAT(bool (*)(uint32_t, uint16_t, uint16_t, uint16_t, int32_t, const char *), name);
        const char *version = steam_in_string(c, arg(c, 5));
        Capture capture; capture_begin(&capture);
        bool ok = init(arg(c, 0), (uint16_t)arg(c, 1), (uint16_t)arg(c, 2), (uint16_t)arg(c, 3), (int32_t)arg(c, 4), version);
        capture_end(&capture);
        if (ok) enable_manual_dispatch(c);
        c->eax = ok;
    } else return 0;
    return 1;
}
RT_SHIM_PROVIDER(steam_shim)

// ---- SetWarningMessageHook : rappel invité void (*)(int severity, const char *text) ----
// Steam appelle le crochet de façon synchrone pendant un appel de l'API (thread
// invité dans un shim : rt_shim_cpu) ou depuis un de ses threads (rattaché le
// temps de l'appel). Le texte est recopié en mémoire invitée, SteamID64 masqués.
static uint32_t warning_hook;
static void host_warning_hook(int severity, const char *text) {
    uint32_t fn = __atomic_load_n(&warning_hook, __ATOMIC_ACQUIRE);
    if (!fn || !text) return;
    CPU *c = rt_shim_cpu();
    int attached = !c;
    if (attached) c = rt_foreign_thread_attach();
    size_t length = strlen(text);
    char *copy = malloc(length + 1);
    if (!copy) rt_trap(c, c->eip, "Steam warning hook: out of memory");
    memcpy(copy, text, length + 1);
    steam_mask_ids(copy, length);
    uint32_t guest = rt_alloc((uint32_t)length + 1);
    if (!guest) rt_trap(c, c->eip, "Steam warning hook: guest memory exhausted");
    memcpy(GPTR(guest), copy, length + 1);
    free(copy);
    uint32_t arguments[] = {(uint32_t)severity, guest};
    rt_invoke(c, fn, arguments, 2);
    rt_free(guest);
    if (attached) rt_foreign_thread_detach();
}
static void set_warning_hook(CPU *c, void *self, uint32_t args, const char *symbol) {
    typedef void (*Hook)(int, const char *);
    typedef void (*F)(void *, Hook);
    uint32_t fn = R32(args);
    __atomic_store_n(&warning_hook, fn, __ATOMIC_RELEASE);
    ((F)steam_flat(c, symbol))(self, fn ? host_warning_hook : NULL);
}
void steam_manual_client_warning_hook(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)sret; set_warning_hook(c, self, args, "SteamAPI_ISteamClient_SetWarningMessageHook");
}
void steam_manual_utils_warning_hook(CPU *c, void *self, uint32_t args, uint32_t sret) {
    (void)sret; set_warning_hook(c, self, args, "SteamAPI_ISteamUtils_SetWarningMessageHook");
}

// Tests : déclenche le crochet hôte comme le ferait Steam.
void steam_test_warning_hook(uint32_t guest_fn, int severity, const char *text) {
    __atomic_store_n(&warning_hook, guest_fn, __ATOMIC_RELEASE);
    host_warning_hook(severity, text);
    __atomic_store_n(&warning_hook, 0, __ATOMIC_RELEASE);
}
