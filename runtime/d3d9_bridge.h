// Pont Direct3D 9 : types et aides communs à runtime/d3d9.c (écrit à la main)
// et runtime/d3d9_generated.c (tools/gen_d3d9.py).
//
// Mandataire invité d'un objet hôte (16 octets, tas invité) :
//   +0 vtable invitée (adresses rt_dynamic_import "Interface::Méthode")
//   +4 étiquette D3D9_TAG << 8 | identifiant d'interface concrète
//   +8 u64 : pointeur de l'objet hôte
// Un seul mandataire par objet hôte vivant (identité stable), libéré quand
// Release rend 0. Les tables du pont ne sont touchées que sous le verrou
// invité (tout appel invité le tient) : aucun mutex sur le chemin chaud.
#pragma once
#include "rt.h"
#include "d3d9_generated.h"

#define D3D9_TAG 0x443339u           /* 'D39' : étiquette = D3D9_TAG << 8 | interface */
#define D3D9_PROXY_SIZE 16u
#define D3D9_E_NOTIMPL 0x80004001u
#define D3D9_E_NOINTERFACE 0x80004002u
#define D3D9_ERR_INVALIDCALL 0x8876086cu

typedef void (*D3d9Thunk)(CPU *c, void *self, uint32_t args);   // args : adresse invitée du 1er argument après this
enum { D3D9_GENERATED, D3D9_MANUAL, D3D9_STUB };
enum { D3D9_RET_HRESULT, D3D9_RET_U32, D3D9_RET_VOID, D3D9_RET_FLOAT };
typedef struct {
    const char *name;     // "Interface::Méthode" (interface qui la définit)
    D3d9Thunk thunk;      // NULL pour un stub
    uint32_t accept;      // masque des interfaces concrètes acceptées pour this
    uint16_t slot;        // rang dans la vtable
    uint8_t kind, ret;
} D3d9Method;
typedef struct {
    const char *name;
    int8_t parent;        // -1 pour IUnknown
    uint8_t abstract;     // pas de vtable propre (IUnknown, Resource, BaseTexture)
    uint32_t derived;     // masque des interfaces concrètes qui en héritent (elle comprise)
    uint8_t iid[16];
    const uint16_t *slots;   // index dans rt_d3d9_methods, ordre de d3d9.h
    uint16_t slot_count;
} D3d9Interface;
extern const D3d9Method rt_d3d9_methods[];
extern const unsigned rt_d3d9_method_count;
extern const D3d9Interface rt_d3d9_interfaces[];
extern const unsigned rt_d3d9_interface_count;

// Structures dont la disposition diffère entre i386 (invité) et arm64 (hôte).
// Les décalages i386 sont vérifiés dans d3d9.c ; les autres structures d3d9
// (D3DCAPS9, D3DVIEWPORT9, RECT, ...) sont identiques et lues en place.
typedef struct {
    uint32_t BackBufferWidth, BackBufferHeight, BackBufferFormat, BackBufferCount;
    uint32_t MultiSampleType, MultiSampleQuality, SwapEffect;
    void *hDeviceWindow;
    uint32_t Windowed, EnableAutoDepthStencil, AutoDepthStencilFormat, Flags;
    uint32_t FullScreen_RefreshRateInHz, PresentationInterval;
} D3d9HostParams;                                   // D3DPRESENT_PARAMETERS
typedef struct {
    char Driver[512], Description[512], DeviceName[32];
    int64_t DriverVersion;
    uint32_t VendorId, DeviceId, SubSysId, Revision;
    uint8_t DeviceIdentifier[16];
    uint32_t WHQLLevel;
} D3d9HostAdapterId;                                // D3DADAPTER_IDENTIFIER9
typedef struct { uint32_t AdapterOrdinal, DeviceType; void *hFocusWindow; uint32_t BehaviorFlags; } D3d9HostCreation;
typedef struct { int32_t Pitch, SlicePitch; void *pBits; } D3d9HostLocked;   // D3DLOCKED_RECT et D3DLOCKED_BOX

#define D3D9_VTBL(self) (*(void *const *const *)(self))
static inline float d3d9_f32(uint32_t bits) { float f; memcpy(&f, &bits, 4); return f; }

void *d3d9_in(CPU *c, uint32_t guest, uint64_t size);
void *d3d9_out(CPU *c, uint32_t guest, uint64_t size);
uint32_t d3d9_deref(CPU *c, uint32_t guest);
void *d3d9_iface_in(CPU *c, uint32_t guest, uint32_t accept);
int d3d9_out_slot(CPU *c, uint32_t guest);
void d3d9_iface_out(CPU *c, uint32_t guest, void *host, unsigned iface);
// Interface en sortie appartenant à l'objet hôte parent, retirée avec lui :
// shared = 1 pour un niveau de texture ou une face de cube (compte partagé
// avec le conteneur : sa fin détruit tout le groupe), 0 pour un tampon
// arrière (dépend de la chaîne d'échange ou du périphérique sans partager).
void d3d9_child_out(CPU *c, uint32_t guest, void *host, unsigned iface, const void *parent, int shared);
void *d3d9_hwnd(CPU *c, uint32_t guest);
void *d3d9_shared(CPU *c, uint32_t guest);
void *d3d9_params_in(CPU *c, uint32_t guest, D3d9HostParams *host, int input);
void d3d9_params_out(CPU *c, uint32_t guest, const D3d9HostParams *host);
void *d3d9_adapter_id_in(CPU *c, uint32_t guest, D3d9HostAdapterId *host);
void d3d9_adapter_id_out(CPU *c, uint32_t guest, const D3d9HostAdapterId *host);
void *d3d9_creation_in(CPU *c, uint32_t guest, D3d9HostCreation *host);
void d3d9_creation_out(CPU *c, uint32_t guest, const D3d9HostCreation *host);
void d3d9_lock_ptr_out(CPU *c, uint32_t guest, void *bits, int ok);
void *d3d9_locked_in(CPU *c, uint32_t guest, D3d9HostLocked *host, uint32_t guest_size);
void d3d9_lockrect_out(CPU *c, uint32_t guest, const D3d9HostLocked *host, int ok);
void d3d9_lockbox_out(CPU *c, uint32_t guest, const D3d9HostLocked *host, int ok);
const void *d3d9_shader(CPU *c, uint32_t guest);
const void *d3d9_decl(CPU *c, uint32_t guest);
const void *d3d9_rgndata(CPU *c, uint32_t guest);
uint32_t d3d9_vertex_count(uint32_t primitive_type, uint32_t primitive_count);
uint32_t d3d9_index_size(uint32_t format);

// Mandataire de l'objet hôte (créé au besoin) ; 0 pour NULL. iface peut être
// abstraite (BaseTexture, Resource) : le type concret vient de GetType.
uint32_t d3d9_proxy(CPU *c, void *host, unsigned iface);
// Adresse invitée d'un pointeur hôte rendu par Lock (rt_trap s'il est hors de la mémoire invitée).
uint32_t d3d9_guest_pointer(CPU *c, const void *host);

// Tests (tests/checks_d3d9.c) : bibliothèque native factice au lieu du dlopen,
// NULL/NULL pour revenir à libd3d9metal.dylib.
#include "d3d9/d3d9metal.h"
void rt_d3d9_set_library(d3d9metal_init_fn init, d3d9metal_create_fn create);
const D3D9MetalHost *rt_d3d9_host(void);
uint32_t rt_d3d9_find_proxy(const void *host);   // 0 si aucun
// Tests sans fenêtre : CAMetalLayer rendue pour un HWND à la place de la vue SDL (NULL : SDL).
void rt_d3d9_set_test_layer(void *(*layer)(void *hwnd, uint32_t *width, uint32_t *height));
unsigned rt_d3d9_live_proxies(void);
// libd3d9metal.dylib chargée et initialisée (d3d9metal_init réussi), une
// fois pour toutes : choix du rendu au lancement, avant que le jeu démarre.
int rt_d3d9_available(void);
// Appelé après chaque Present réussi (NULL : rien), sur le thread du jeu,
// verrou invité tenu : doit rester bref.
extern void (*volatile rt_d3d9_on_present)(void);
// Plafonds mémoire posés devant MTLD3D_CONFIG avant d3d9metal_init.
extern const char rt_d3d9_memory_defaults[];
