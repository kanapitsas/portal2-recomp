// Pont Direct3D 9 : shaderapivk.so (rendu « Vulkan » de Valve, i386) vers une
// implémentation Direct3D 9 native arm64 (libd3d9metal.dylib, contrat dans
// runtime/d3d9/d3d9metal.h), à la place de libdxvk_d3d9.so.
//
// - Direct3DCreate9 est le seul import graphique de shaderapivk.so : il charge
//   la bibliothèque (P2_D3D9METAL, sinon Frameworks/ de l'app, sinon à côté de
//   l'exécutable) et rend le mandataire invité de l'IDirect3D9 hôte.
// - Appels COM invités (cdecl, this en premier, l'appelant dépile) : chaque
//   emplacement de vtable invitée est une adresse dynamique dont la route mène
//   à d3d9_call puis au thunk généré (tools/gen_d3d9.py).
// - Mandataires : voir d3d9_bridge.h. Table objet hôte → mandataire en
//   adressage ouvert, sous le verrou invité.
// - Lock : l'hôte doit rendre de la mémoire obtenue par alloc_app (projetée
//   dans l'espace invité par rt_mmap) ; un pointeur hors de l'espace invité
//   arrête le jeu avec un message explicite.
// - Attentes hôtes (blocking_begin/end) : verrou invité relâché puis repris.
//
// SHIMS: Direct3DCreate9
#include "d3d9_bridge.h"
#include "sdl_common.h"
#include <dlfcn.h>
#include <limits.h>
#include <mach-o/dyld.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

const char *rt_user_root(void);                                            // paths.c


// ---------------------------------------------------------------- dispositions i386
// Décalages i386 écrits à la main (gcc -m32, Linux : int64 aligné sur 4),
// vérifiés contre les en-têtes natifs de DXVK compilés pour i386 et arm64.
enum {
    PP_SIZE = 56, PP_HWND = 0x1c, PP_WINDOWED = 0x20, PP_INTERVAL = 0x34,
    AID_SIZE = 1100, AID_VERSION = 1056, AID_VENDOR = 1064, AID_GUID = 1080, AID_WHQL = 1096,
    CREATION_SIZE = 16, CREATION_HWND = 8, CREATION_FLAGS = 12,
    LOCKED_RECT_SIZE = 8, LOCKED_RECT_BITS = 4, LOCKED_BOX_SIZE = 12, LOCKED_BOX_BITS = 8,
};
// Hôte arm64 : HWND et pointeurs sur 8 octets.
#define OFF(type, field) __builtin_offsetof(type, field)
_Static_assert(sizeof(D3d9HostParams) == 64 && OFF(D3d9HostParams, hDeviceWindow) == 0x20 &&
               OFF(D3d9HostParams, Windowed) == 0x28 && OFF(D3d9HostParams, PresentationInterval) == 0x3c,
               "D3DPRESENT_PARAMETERS arm64");
_Static_assert(OFF(D3d9HostParams, SwapEffect) == PP_HWND - 4 && PP_INTERVAL + 4 == PP_SIZE &&
               PP_WINDOWED == PP_HWND + 4, "D3DPRESENT_PARAMETERS i386: 7 DWORD, HWND, 6 DWORD");
_Static_assert(sizeof(D3d9HostAdapterId) == 1104 && OFF(D3d9HostAdapterId, DriverVersion) == AID_VERSION &&
               OFF(D3d9HostAdapterId, VendorId) == AID_VENDOR && OFF(D3d9HostAdapterId, DeviceIdentifier) == AID_GUID &&
               OFF(D3d9HostAdapterId, WHQLLevel) == AID_WHQL && AID_WHQL + 4 == AID_SIZE,
               "D3DADAPTER_IDENTIFIER9: same offsets, i386 size without the arm64 tail padding");
_Static_assert(sizeof(D3d9HostCreation) == 24 && OFF(D3d9HostCreation, hFocusWindow) == 8 &&
               OFF(D3d9HostCreation, BehaviorFlags) == 16 && CREATION_FLAGS + 4 == CREATION_SIZE,
               "D3DDEVICE_CREATION_PARAMETERS");
_Static_assert(sizeof(D3d9HostLocked) == 16 && OFF(D3d9HostLocked, pBits) == 8 &&
               LOCKED_RECT_BITS + 4 == LOCKED_RECT_SIZE && LOCKED_BOX_BITS + 4 == LOCKED_BOX_SIZE,
               "D3DLOCKED_RECT / D3DLOCKED_BOX");

static _Thread_local char message[512];
static __attribute__((noreturn, format(printf, 2, 3))) void fail(CPU *c, const char *format, ...) {
    va_list list;
    va_start(list, format);
    vsnprintf(message, sizeof(message), format, list);
    va_end(list);
    rt_trap(c, c->eip, message);
}

// ---------------------------------------------------------------- bibliothèque native
static pthread_mutex_t library_lock = PTHREAD_MUTEX_INITIALIZER;
static d3d9metal_init_fn library_init, test_init;
static d3d9metal_create_fn library_create, test_create;
static int library_state;   // 0 : à charger, 1 : prête, -1 : absente ou refusée
static char library_path[PATH_MAX];

static int executable_dir(char *out, size_t size) {
    char exe[PATH_MAX], real[PATH_MAX];
    uint32_t length = sizeof(exe);
    if (_NSGetExecutablePath(exe, &length) || !realpath(exe, real)) return -1;
    char *slash = strrchr(real, '/');
    if (!slash) return -1;
    *slash = 0;
    return snprintf(out, size, "%s", real) >= (int)size ? -1 : 0;
}

// Ordre : P2_D3D9METAL, ../Frameworks (app macOS), Frameworks (app iOS), dossier de l'exécutable.
static void *open_library(char *path, size_t size) {
    const char *env = getenv("P2_D3D9METAL");
    if (env && *env) { snprintf(path, size, "%s", env); return dlopen(env, RTLD_NOW | RTLD_LOCAL); }
    char dir[PATH_MAX];
    if (executable_dir(dir, sizeof(dir))) return NULL;
    static const char *const candidates[] = {"%s/../Frameworks/libd3d9metal.dylib", "%s/Frameworks/libd3d9metal.dylib",
                                             "%s/libd3d9metal.dylib"};
    for (size_t i = 0; i < sizeof(candidates) / sizeof(*candidates); ++i) {
        snprintf(path, size, candidates[i], dir);
        struct stat info;
        if (stat(path, &info)) continue;
        void *handle = dlopen(path, RTLD_NOW | RTLD_LOCAL);
        if (handle) return handle;
        fprintf(stderr, "d3d9: dlopen %s: %s\n", path, dlerror());
    }
    snprintf(path, size, "libd3d9metal.dylib (P2_D3D9METAL, Frameworks/ or next to the executable)");
    return NULL;
}

void rt_d3d9_set_library(d3d9metal_init_fn init, d3d9metal_create_fn create) {
    pthread_mutex_lock(&library_lock);
    test_init = init; test_create = create;
    library_init = NULL; library_create = NULL; library_state = 0;
    pthread_mutex_unlock(&library_lock);
}

// ---------------------------------------------------------------- services rendus à l'hôte
static void *host_metal_layer(void *user, void *hwnd, uint32_t *width, uint32_t *height) {
    (void)user;
    uint32_t w = 0, h = 0;
    void *layer = hwnd ? rt_sdl_metal_layer(hwnd, &w, &h) : NULL;
    if (width) *width = w;
    if (height) *height = h;
    return layer;
}

// Mémoire adressable par le jeu : pages invitées projetées par rt_mmap (zone
// 0x21000000-0x70000000), alignées sur la page hôte (16 Kio). La table des
// projections est protégée par le verrou invité : rt_guest_enter est
// réentrant, il est donc pris ici que l'appelant le tienne déjà ou non.
static void *host_alloc_app(void *user, size_t size, size_t align) {
    (void)user;
    size_t page = (size_t)sysconf(_SC_PAGESIZE);
    if (!size || size > UINT32_MAX - page || align > page || (align & (align - 1))) {
        fprintf(stderr, "d3d9: alloc_app(%zu, align %zu) refused (max alignment %zu)\n", size, align, page);
        return NULL;
    }
    rt_guest_enter();
    uint32_t address = rt_mmap(NULL, 0, (uint32_t)size, 3 /* PROT_READ|PROT_WRITE */, 0x22 /* MAP_PRIVATE|MAP_ANON */, -1, 0);
    rt_guest_leave();
    if (address == UINT32_MAX) {
        fprintf(stderr, "d3d9: alloc_app(%zu): guest address space exhausted\n", size);
        return NULL;
    }
    return GPTR(address);
}
static void host_free_app(void *user, void *ptr, size_t size) {
    (void)user;
    if (!ptr || !size) return;
    uintptr_t offset = (uintptr_t)ptr - (uintptr_t)M;
    if ((uintptr_t)ptr < (uintptr_t)M || offset >= RT_VM_END || offset < RT_VM_BEGIN || size > RT_VM_END - offset) {
        fprintf(stderr, "d3d9: free_app(%p, %zu) outside alloc_app memory: ignored\n", ptr, size);
        return;
    }
    rt_guest_enter();
    if (rt_munmap((uint32_t)offset, (uint32_t)size)) fprintf(stderr, "d3d9: free_app(%p, %zu) failed\n", ptr, size);
    rt_guest_leave();
}

// Pile par thread des niveaux du verrou invité relâchés (attentes imbriquées).
enum { BLOCKING_DEPTH = 16 };
static _Thread_local unsigned blocking_saved[BLOCKING_DEPTH], blocking_depth;
static void host_blocking_begin(void *user) {
    (void)user;
    unsigned saved = rt_guest_suspend();
    if (blocking_depth < BLOCKING_DEPTH) blocking_saved[blocking_depth] = saved;
    else if (saved) abort();   // imbrication trop profonde : impossible de rendre le verrou
    ++blocking_depth;
}
static void host_blocking_end(void *user) {
    (void)user;
    if (!blocking_depth) abort();   // blocking_end sans blocking_begin
    --blocking_depth;
    rt_guest_resume(blocking_depth < BLOCKING_DEPTH ? blocking_saved[blocking_depth] : 0);
}

static uint16_t env_id(const char *name, uint16_t fallback) {
    const char *value = getenv(name);
    if (!value || !*value) return fallback;
    char *end;
    unsigned long parsed = strtoul(value, &end, 16);
    return *end || parsed > 0xffff ? fallback : (uint16_t)parsed;
}

// Disposition arm64 de la table hôte, la même que dans mtld3d (native/include/d3d9metal.h).
_Static_assert(OFF(D3D9MetalHost, metal_layer) == 16 && OFF(D3D9MetalHost, vendor_id) == 56 &&
               OFF(D3D9MetalHost, cache_dir) == 64 && OFF(D3D9MetalHost, modes) == 72 &&
               OFF(D3D9MetalHost, mode_count) == 80 && sizeof(D3D9MetalHost) == 88, "D3D9MetalHost");
_Static_assert(sizeof(D3D9MetalDisplayMode) == 12, "D3D9MetalDisplayMode");

static D3D9MetalHost host;
static char cache_dir[PATH_MAX];
enum { MODE_CAPACITY = 64 };
static D3D9MetalDisplayMode modes[MODE_CAPACITY];   // prêtés à la bibliothèque pour toute la vie du processus
const D3D9MetalHost *rt_d3d9_host(void) { return &host; }

static void fill_host(void) {
    host = (D3D9MetalHost){
        .size = sizeof(D3D9MetalHost), .user = NULL,
        .metal_layer = host_metal_layer, .alloc_app = host_alloc_app, .free_app = host_free_app,
        .blocking_begin = host_blocking_begin, .blocking_end = host_blocking_end,
        // Apple par défaut : aucune branche propre à un fabricant dans Source.
        .vendor_id = env_id("P2_D3D9_VENDOR_ID", 0x106b), .device_id = env_id("P2_D3D9_DEVICE_ID", 0),
        .cache_dir = NULL,
    };
    const char *env = getenv("P2_D3D9_CACHE"), *user = rt_user_root();
    if (env && *env) snprintf(cache_dir, sizeof(cache_dir), "%s", env);
    else if (user && *user) snprintf(cache_dir, sizeof(cache_dir), "%s/d3d9metal-cache", user);
    if (cache_dir[0] && (!mkdir(cache_dir, 0755) || access(cache_dir, W_OK) == 0)) host.cache_dir = cache_dir;
    // Modes de l'écran tels que le jeu les voit par SDL (EnumAdapterModes).
    uint32_t found[MODE_CAPACITY][3];
    unsigned count = rt_sdl_game_modes(found, MODE_CAPACITY);
    for (unsigned i = 0; i < count; ++i) modes[i] = (D3D9MetalDisplayMode){found[i][0], found[i][1], found[i][2]};
    host.modes = count ? modes : NULL;
    host.mode_count = count;
}

// Charge et initialise la bibliothèque une fois (d3d9metal_init n'accepte
// qu'une table par processus : une seconde initialisation par la même
// fonction, après rt_d3d9_set_library dans les tests, garde la première).
static int load_library(void) {
    static d3d9metal_init_fn initialized;
    pthread_mutex_lock(&library_lock);
    if (!library_state) {
        library_state = -1;
        d3d9metal_init_fn init = test_init;
        d3d9metal_create_fn create = test_create;
        if (!init) {
            void *handle = open_library(library_path, sizeof(library_path));
            init = handle ? (d3d9metal_init_fn)dlsym(handle, "d3d9metal_init") : NULL;
            create = handle ? (d3d9metal_create_fn)dlsym(handle, "d3d9metal_Direct3DCreate9") : NULL;
            if (!init || !create) fprintf(stderr, "d3d9: %s: %s\n", library_path, handle ? "d3d9metal_* exports missing" : "not found");
        } else snprintf(library_path, sizeof(library_path), "(test library)");
        if (init && create) {
            int status = 0;
            if (init != initialized) {
                fill_host();
                status = init(&host);
            }
            if (status) fprintf(stderr, "d3d9: d3d9metal_init failed (%d)\n", status);
            else {
                if (!test_init) initialized = init;
                library_init = init; library_create = create; library_state = 1;
                fprintf(stderr, "d3d9: %s ready (vendor %04x device %04x, %u display modes, cache %s)\n", library_path,
                        host.vendor_id, host.device_id, host.mode_count, host.cache_dir ? host.cache_dir : "none");
            }
        }
    }
    int ready = library_state > 0;
    pthread_mutex_unlock(&library_lock);
    return ready;
}

int rt_d3d9_available(void) { return load_library(); }

// ---------------------------------------------------------------- mandataires
// Table pointeur d'interface hôte → mandataire invité, adressage ouvert
// (puissance de 2). Accès sous le verrou invité uniquement (voir
// d3d9_bridge.h). parent : objet hôte propriétaire (d3d9_child_out) ;
// shared : l'objet partage le compte du parent (niveau de texture, autre
// interface du même objet) plutôt que d'en dépendre seulement (tampon
// arrière) ; children : nombre d'entrées qui désignent celle-ci.
//
// Identité COM : un objet peut avoir des pointeurs d'interface distincts, un
// mandataire par pointeur. QueryInterface(IID_IUnknown) doit pourtant rendre
// toujours le même pointeur : l'entrée de clé U (pointeur IUnknown hôte)
// retient dans canon le mandataire rendu pour IUnknown, choisi à la première
// demande puis stable. Si U n'est le pointeur d'aucune interface pontée,
// l'entrée est un alias (proxy 0). L'entrée du mandataire canonique garde U
// dans identity pour effacer canon quand elle est retirée.
typedef struct {
    const void *host, *parent, *identity;
    uint32_t proxy, children, canon;
    uint8_t shared;
} Entry;
#define TOMBSTONE ((const void *)1)
static Entry *entries;
static size_t capacity, used, filled, live;   // used : cases non vides (pierres tombales comprises) ; filled : entrées ; live : mandataires
static uint32_t vtables[D3D9_INTERFACE_COUNT];

static size_t hash(const void *host) {
    uint64_t x = (uint64_t)(uintptr_t)host;
    x ^= x >> 33; x *= 0xff51afd7ed558ccdull; x ^= x >> 33;
    return (size_t)x;
}
static Entry *find(const void *host) {
    if (!capacity) return NULL;
    for (size_t i = hash(host) & (capacity - 1);; i = (i + 1) & (capacity - 1)) {
        if (entries[i].host == host) return &entries[i];
        if (!entries[i].host) return NULL;
    }
}
static int grow(void) {
    size_t next = capacity ? capacity * 2 : 1024;
    Entry *fresh = calloc(next, sizeof(*fresh));
    if (!fresh) return -1;
    for (size_t i = 0; i < capacity; ++i) {
        if (!entries[i].host || entries[i].host == TOMBSTONE) continue;
        size_t j = hash(entries[i].host) & (next - 1);
        while (fresh[j].host) j = (j + 1) & (next - 1);
        fresh[j] = entries[i];
    }
    free(entries);
    entries = fresh; capacity = next; used = filled;
    return 0;
}
// Nouvelle entrée (host absent de la table). Les pointeurs Entry * obtenus
// avant peuvent être invalidés (agrandissement).
static Entry *insert(CPU *c, const void *host, uint32_t proxy) {
    if ((used + 1) * 4 > capacity * 3 && grow()) fail(c, "D3D9 bridge: out of memory (proxy table)");
    size_t i = hash(host) & (capacity - 1);
    while (entries[i].host && entries[i].host != TOMBSTONE) i = (i + 1) & (capacity - 1);
    if (!entries[i].host) ++used;
    entries[i] = (Entry){.host = host, .proxy = proxy};
    ++filled;
    if (proxy) ++live;
    return &entries[i];
}
static void erase(Entry *e) {
    *e = (Entry){.host = TOMBSTONE};
    --filled;
}
// Le mandataire de e cesse d'être le représentant IUnknown de son objet.
static void forget_identity(Entry *e) {
    if (!e->identity) return;
    Entry *u = find(e->identity);
    e->identity = NULL;
    if (!u || u->canon != e->proxy) return;
    u->canon = 0;
    if (!u->proxy && u != e) erase(u);   // alias devenu inutile
}

uint32_t rt_d3d9_find_proxy(const void *host) { Entry *e = host ? find(host) : NULL; return e ? e->proxy : 0; }
unsigned rt_d3d9_live_proxies(void) { return (unsigned)live; }

static uint32_t tag_of(uint32_t proxy) { return R32(proxy + 4); }
static int proxy_accepts(uint32_t tag, uint32_t accept) {
    return (tag >> 8) == D3D9_TAG && (tag & 0xff) < 32 && ((accept >> (tag & 0xff)) & 1);
}
static void *proxy_host(uint32_t proxy) { return (void *)(uintptr_t)R64(proxy + 8); }

static uint32_t vtable(CPU *c, unsigned iface) {
    if (vtables[iface]) return vtables[iface];
    const D3d9Interface *info = &rt_d3d9_interfaces[iface];
    uint32_t table = rt_alloc(4u * info->slot_count);
    if (!table) fail(c, "D3D9 bridge: guest vtable allocation failed (%s)", info->name);
    for (unsigned s = 0; s < info->slot_count; ++s) {
        uint32_t address = rt_dynamic_import(rt_d3d9_methods[info->slots[s]].name);
        if (!address) fail(c, "D3D9 bridge: dynamic import table is full");
        W32(table + 4 * s, address);
    }
    return vtables[iface] = table;
}

// Type concret d'une ressource rendue sous un type abstrait (GetTexture...).
static unsigned concrete(CPU *c, void *host, unsigned iface) {
    if (!rt_d3d9_interfaces[iface].abstract) return iface;
    if (iface != D3D9_IDirect3DBaseTexture9 && iface != D3D9_IDirect3DResource9)
        fail(c, "D3D9 bridge: cannot tell the concrete type of a %s", rt_d3d9_interfaces[iface].name);
    // IDirect3DResource9::GetType (emplacement 10) : D3DRESOURCETYPE.
    uint32_t type = ((uint32_t (*)(void *))D3D9_VTBL(host)[10])(host);
    static const int8_t by_type[] = {-1, D3D9_IDirect3DSurface9, -1, D3D9_IDirect3DTexture9, D3D9_IDirect3DVolumeTexture9,
                                     D3D9_IDirect3DCubeTexture9, D3D9_IDirect3DVertexBuffer9, D3D9_IDirect3DIndexBuffer9};
    int result = type < sizeof(by_type) ? by_type[type] : -1;
    if (result < 0 || !((rt_d3d9_interfaces[iface].derived >> result) & 1))
        fail(c, "D3D9 bridge: host %s reports resource type %u", rt_d3d9_interfaces[iface].name, type);
    return (unsigned)result;
}

uint32_t d3d9_proxy(CPU *c, void *host, unsigned iface) {
    if (!host) return 0;
    if (iface >= D3D9_INTERFACE_COUNT) fail(c, "D3D9 bridge: bad interface id %u", iface);
    Entry *e = find(host);
    // Mandataire existant d'un type compatible : même identité pour le jeu.
    // Sinon (adresse hôte réutilisée par un objet d'un autre type après la
    // destruction d'un conteneur), un nouveau mandataire remplace l'ancien.
    if (e && e->proxy && proxy_accepts(tag_of(e->proxy), rt_d3d9_interfaces[iface].derived)) return e->proxy;
    unsigned type = concrete(c, host, iface);
    uint32_t table = vtable(c, type), proxy = rt_alloc(D3D9_PROXY_SIZE);
    if (!proxy) fail(c, "D3D9 bridge: guest proxy allocation failed");
    W32(proxy, table);
    W32(proxy + 4, D3D9_TAG << 8 | type);
    W64(proxy + 8, (uint64_t)(uintptr_t)host);
    if (!e) { insert(c, host, proxy); return proxy; }
    if (!e->proxy) { e->proxy = proxy; ++live; return proxy; }   // alias IUnknown : canon conservé
    // Objet nouveau à cette adresse : ancien mandataire laissé (jamais libéré,
    // peut être encore référencé), liens et identité de l'ancien objet oubliés.
    if (e->parent) { Entry *p = find(e->parent); if (p && p->children) --p->children; }
    forget_identity(e);
    e->proxy = proxy; e->parent = NULL; e->shared = 0; e->canon = 0;
    return proxy;
}

static void retire(uint32_t proxy) {
    W32(proxy + 4, 0);   // un appel ultérieur sur ce mandataire piège
    W64(proxy + 8, 0);
    rt_free(proxy);
}
static void unlink_entry(Entry *e) {
    if (e->parent) { Entry *p = find(e->parent); if (p && p->children) --p->children; }
    forget_identity(e);
    erase(e);
    --live;
}
// Retire une entrée, son mandataire et tous les objets qui en dépendent.
static void retire_group(Entry *e) {
    const void *host = e->host;
    uint32_t proxy = e->proxy, children = e->children;
    unlink_entry(e);
    retire(proxy);
    for (size_t i = 0; children && i < capacity; ++i)
        if (entries[i].parent == host && entries[i].host != TOMBSTONE) {
            entries[i].parent = NULL;
            retire_group(&entries[i]);
            --children;
        }
}

// host dépend de parent (premier propriétaire seulement, jamais de cycle :
// deux interfaces d'un même objet peuvent se désigner l'une l'autre).
static void link_child(const void *host, const void *parent, int shared) {
    Entry *e = find(host), *p = find(parent);
    if (!e || !e->proxy || !p || !p->proxy || e->parent || e == p) return;
    for (const Entry *up = p; up->parent; ) {
        if (up->parent == host) return;
        if (!(up = find(up->parent))) break;
    }
    {
        e->parent = parent;
        e->shared = (uint8_t)shared;
        ++p->children;
    }
}

// Release a rendu 0 : l'objet hôte n'existe plus, ni ce qui en dépend. Un
// niveau qui partage le compte de son conteneur à 0 signifie que tout le
// groupe (conteneur, autres niveaux) est détruit, quel que soit l'ordre des
// Release. Les mandataires retirés n'ont plus de référence invitée valable.
static void drop(uint32_t proxy) {
    Entry *e = find(proxy_host(proxy));
    if (!e || e->proxy != proxy) { retire(proxy); return; }   // mandataire déjà remplacé
    Entry *root = e;
    while (root->shared && root->parent) {
        Entry *up = find(root->parent);
        if (!up) break;
        root = up;
    }
    retire_group(root);
}

static void *self_of(CPU *c, const D3d9Method *m) {
    uint32_t object = R32(c->esp + 4);
    if (__builtin_expect(object != 0, 1)) {
        uint32_t tag = R32(object + 4);
        if (__builtin_expect(proxy_accepts(tag, m->accept), 1)) return proxy_host(object);
    }
    fail(c, "D3D9 bridge: %s called on %#x, which is not a live proxy of that interface", m->name, object);
}

// ---------------------------------------------------------------- aides du code généré
void *d3d9_in(CPU *c, uint32_t guest, uint64_t size) { return guest ? rt_span(c, guest, size) : NULL; }
void *d3d9_out(CPU *c, uint32_t guest, uint64_t size) { return guest ? rt_write_span(c, guest, size) : NULL; }
uint32_t d3d9_deref(CPU *c, uint32_t guest) { (void)c; return guest ? R32(guest) : 0; }

void *d3d9_iface_in(CPU *c, uint32_t guest, uint32_t accept) {
    if (!guest) return NULL;
    uint32_t tag = R32(guest + 4);
    if (!proxy_accepts(tag, accept))
        fail(c, "D3D9 bridge: interface argument %#x is not a live D3D9 proxy of the expected type (tag %#x)", guest, tag);
    return proxy_host(guest);
}
int d3d9_out_slot(CPU *c, uint32_t guest) {
    if (!guest) return 0;
    rt_write_span(c, guest, 4);
    return 1;
}
void d3d9_iface_out(CPU *c, uint32_t guest, void *host, unsigned iface) {
    if (guest) W32(guest, d3d9_proxy(c, host, iface));
}
void d3d9_child_out(CPU *c, uint32_t guest, void *host, unsigned iface, const void *parent, int shared) {
    if (!guest) return;
    uint32_t proxy = d3d9_proxy(c, host, iface);
    W32(guest, proxy);
    if (proxy) link_child(host, parent, shared);
}

void *d3d9_hwnd(CPU *c, uint32_t guest) {
    if (!guest) return NULL;
    void *window = rt_handle_get(guest, RT_SDL_WINDOW);
    if (!window) fail(c, "D3D9 bridge: HWND %#x is not an SDL_Window created by the SDL bridge", guest);
    return window;
}
static uint32_t guest_hwnd(const void *window) { return window ? rt_handle_find(window, RT_SDL_WINDOW) : 0; }

void *d3d9_shared(CPU *c, uint32_t guest) {
    static int logged;
    (void)c;
    if (guest && !logged) { logged = 1; fprintf(stderr, "d3d9: pSharedHandle ignored (not supported by Direct3D 9)\n"); }
    return NULL;
}

void *d3d9_params_in(CPU *c, uint32_t guest, D3d9HostParams *h, int input) {
    if (!guest) return NULL;
    memset(h, 0, sizeof(*h));
    if (!input) { rt_write_span(c, guest, PP_SIZE); return h; }
    rt_write_span(c, guest, PP_SIZE);   // CreateDevice/Reset y réécrivent les valeurs effectives
    uint32_t *before = &h->BackBufferWidth, *after = &h->Windowed;
    for (unsigned i = 0; i < 7; ++i) before[i] = R32(guest + 4 * i);
    h->hDeviceWindow = d3d9_hwnd(c, R32(guest + PP_HWND));
    for (unsigned i = 0; i < 6; ++i) after[i] = R32(guest + PP_WINDOWED + 4 * i);
    return h;
}
void d3d9_params_out(CPU *c, uint32_t guest, const D3d9HostParams *h) {
    if (!guest) return;
    rt_write_span(c, guest, PP_SIZE);
    const uint32_t *before = &h->BackBufferWidth, *after = &h->Windowed;
    for (unsigned i = 0; i < 7; ++i) W32(guest + 4 * i, before[i]);
    W32(guest + PP_HWND, guest_hwnd(h->hDeviceWindow));
    for (unsigned i = 0; i < 6; ++i) W32(guest + PP_WINDOWED + 4 * i, after[i]);
}

void *d3d9_adapter_id_in(CPU *c, uint32_t guest, D3d9HostAdapterId *h) {
    if (!guest) return NULL;
    rt_write_span(c, guest, AID_SIZE);
    memset(h, 0, sizeof(*h));
    return h;
}
void d3d9_adapter_id_out(CPU *c, uint32_t guest, const D3d9HostAdapterId *h) {
    if (guest) memcpy(rt_write_span(c, guest, AID_SIZE), h, AID_SIZE);   // mêmes décalages, sans le remplissage final
}

void *d3d9_creation_in(CPU *c, uint32_t guest, D3d9HostCreation *h) {
    if (!guest) return NULL;
    rt_write_span(c, guest, CREATION_SIZE);
    memset(h, 0, sizeof(*h));
    return h;
}
void d3d9_creation_out(CPU *c, uint32_t guest, const D3d9HostCreation *h) {
    (void)c;   // tampon déjà validé par d3d9_creation_in
    if (!guest) return;
    W32(guest, h->AdapterOrdinal); W32(guest + 4, h->DeviceType);
    W32(guest + CREATION_HWND, guest_hwnd(h->hFocusWindow)); W32(guest + CREATION_FLAGS, h->BehaviorFlags);
}

uint32_t d3d9_guest_pointer(CPU *c, const void *bits) {
    if (!bits) return 0;
    uintptr_t p = (uintptr_t)bits, base = (uintptr_t)M;
    if (p < base || p - base > UINT32_MAX)
        fail(c, "D3D9 bridge: the native library returned lock pointer %p outside guest memory "
                "(lockable memory must come from D3D9MetalHost.alloc_app)", bits);
    return (uint32_t)(p - base);
}
void d3d9_lock_ptr_out(CPU *c, uint32_t guest, void *bits, int ok) {
    if (guest) W32(guest, ok ? d3d9_guest_pointer(c, bits) : 0);
}
void *d3d9_locked_in(CPU *c, uint32_t guest, D3d9HostLocked *h, uint32_t guest_size) {
    if (!guest) return NULL;
    rt_write_span(c, guest, guest_size);
    memset(h, 0, sizeof(*h));
    return h;
}
void d3d9_lockrect_out(CPU *c, uint32_t guest, const D3d9HostLocked *h, int ok) {
    if (!guest) return;
    W32(guest, ok ? (uint32_t)h->Pitch : 0);
    W32(guest + LOCKED_RECT_BITS, ok ? d3d9_guest_pointer(c, h->pBits) : 0);
}
void d3d9_lockbox_out(CPU *c, uint32_t guest, const D3d9HostLocked *h, int ok) {
    if (!guest) return;
    W32(guest, ok ? (uint32_t)h->Pitch : 0);
    W32(guest + 4, ok ? (uint32_t)h->SlicePitch : 0);
    W32(guest + LOCKED_BOX_BITS, ok ? d3d9_guest_pointer(c, h->pBits) : 0);
}

// Jetons de shader : version, puis instructions jusqu'à D3DSIO_END (0x0000ffff).
// Commentaires (0xfffe) : longueur en bits 16-30. SM 2+ : longueur de
// l'instruction en bits 24-27 ; SM 1 : un jeton à la fois (paramètres bit 31).
enum { SHADER_LIMIT = 1u << 20 };
const void *d3d9_shader(CPU *c, uint32_t guest) {
    if (!guest) return NULL;
    uint32_t version = R32(guest), major = (version >> 8) & 0xff, i = 1;
    if ((version & 0xfffe0000u) != 0xfffe0000u) fail(c, "D3D9 bridge: shader token %#x is not a version token", version);
    for (;;) {
        if (i >= SHADER_LIMIT / 4) fail(c, "D3D9 bridge: shader at %#x has no end token within 1 MiB", guest);
        uint32_t token = R32(guest + 4 * i);
        if (token == 0x0000ffffu) break;
        if ((token & 0xffff) == 0xfffe) i += 1 + ((token >> 16) & 0x7fff);
        else if (major >= 2 && !(token & 0x80000000u)) i += 1 + ((token >> 24) & 0xf);
        else ++i;
    }
    return rt_span(c, guest, 4ull * (i + 1));
}
// D3DVERTEXELEMENT9 (8 octets) jusqu'à D3DDECL_END (Stream 0xff), 64 éléments au plus + la fin.
const void *d3d9_decl(CPU *c, uint32_t guest) {
    if (!guest) return NULL;
    for (uint32_t i = 0; i <= 64; ++i)
        if (R16(guest + 8 * i) == 0xff) return rt_span(c, guest, 8ull * (i + 1));
    fail(c, "D3D9 bridge: vertex declaration at %#x has no D3DDECL_END", guest);
}
// RGNDATA : en-tête de 32 octets (nCount en +8) puis nCount RECT.
const void *d3d9_rgndata(CPU *c, uint32_t guest) {
    if (!guest) return NULL;
    rt_span(c, guest, 32);
    return rt_span(c, guest, 32 + 16ull * R32(guest + 8));
}

uint32_t d3d9_vertex_count(uint32_t type, uint32_t count) {
    switch (type) {
    case 1: return count;                    // POINTLIST
    case 2: return 2 * count;                // LINELIST
    case 3: return count ? count + 1 : 0;    // LINESTRIP
    case 4: return 3 * count;                // TRIANGLELIST
    case 5: case 6: return count ? count + 2 : 0;   // TRIANGLESTRIP, TRIANGLEFAN
    default: return 0;
    }
}
uint32_t d3d9_index_size(uint32_t format) { return format == 102 /* D3DFMT_INDEX32 */ ? 4 : 2; }

// ---------------------------------------------------------------- méthodes écrites à la main
static int interface_of_iid(const uint8_t *iid) {
    for (unsigned i = 0; i < rt_d3d9_interface_count; ++i)
        if (!memcmp(rt_d3d9_interfaces[i].iid, iid, 16)) return (int)i;
    return -1;
}

typedef int32_t (*Query)(void *, const void *, void **);
static uint32_t host_addref(void *object) { return ((uint32_t (*)(void *))D3D9_VTBL(object)[1])(object); }
static uint32_t host_release(void *object) { return ((uint32_t (*)(void *))D3D9_VTBL(object)[2])(object); }

// Type concret d'un objet rendu comme IUnknown sans mandataire connu
// (conteneur d'une surface : texture, cube ou chaîne d'échange) : demandé à
// l'objet lui-même par QueryInterface, dans l'ordre fixe des interfaces. -1
// si aucune interface d3d9 ; sinon *typed reçoit le pointeur de cette
// interface (avec sa référence), qui peut différer du pointeur IUnknown.
static int identify(void *object, void **typed) {
    for (unsigned i = 0; i < rt_d3d9_interface_count; ++i) {
        if (rt_d3d9_interfaces[i].abstract) continue;
        void *same = NULL;
        if (((Query)D3D9_VTBL(object)[0])(object, rt_d3d9_interfaces[i].iid, &same) >= 0 && same) {
            *typed = same;
            return (int)i;
        }
    }
    return -1;
}

// Mandataire représentant l'identité de l'objet dont unknown est le pointeur
// IUnknown hôte (rendu par QueryInterface(IID_IUnknown) ou GetContainer). La
// référence portée par unknown est consommée ; le mandataire rendu en porte
// une sur son propre pointeur d'interface. 0 si l'objet n'a aucune interface d3d9.
static uint32_t identity_proxy(CPU *c, void *unknown) {
    Entry *u = find(unknown);
    uint32_t proxy;
    if (u && u->canon) {
        proxy = u->canon;
        void *typed = proxy_host(proxy);
        if (typed != unknown) { host_addref(typed); host_release(unknown); }
        return proxy;
    }
    if (u && u->proxy) proxy = u->proxy;   // le pointeur IUnknown est aussi celui d'une interface pontée
    else {
        void *typed = NULL;
        int iface = identify(unknown, &typed);
        host_release(unknown);   // garder une seule référence, celle de l'interface typée
        if (iface < 0) return 0;
        proxy = d3d9_proxy(c, typed, (unsigned)iface);
    }
    u = find(unknown);
    if (!u) u = insert(c, unknown, 0);   // alias : U n'est le pointeur d'aucune interface pontée
    u->canon = proxy;
    Entry *e = find(proxy_host(proxy));
    if (e && e->proxy == proxy) {
        if (e->identity && e->identity != unknown) forget_identity(e);
        e->identity = unknown;
    }
    return proxy;
}

// Objet rendu par QueryInterface/GetContainer pour riid : mandataire du type
// demandé ; IUnknown : mandataire d'identité ; abstrait : type déduit.
static int32_t wrap_by_iid(CPU *c, int32_t hr, const uint8_t *riid, void *object, uint32_t out) {
    if (hr < 0 || !object) { if (out) W32(out, 0); return hr; }
    int iface = interface_of_iid(riid);
    if (iface < 0) {
        static int logged;
        if (!logged) { logged = 1; fprintf(stderr, "d3d9: interface id not bridged, object released (E_NOINTERFACE)\n"); }
        host_release(object);
        if (out) W32(out, 0);
        return (int32_t)D3D9_E_NOINTERFACE;
    }
    uint32_t proxy = iface == D3D9_IUnknown ? identity_proxy(c, object) : d3d9_proxy(c, object, (unsigned)iface);
    if (out) W32(out, proxy);
    return proxy ? hr : (int32_t)D3D9_E_NOINTERFACE;
}

void d3d9_manual_IUnknown_QueryInterface(CPU *c, void *self, uint32_t a) {
    const uint8_t *riid = rt_span(c, R32(a), 16);
    uint32_t out = R32(a + 4);
    if (out) rt_write_span(c, out, 4);
    void *object = NULL;
    int32_t hr = ((Query)D3D9_VTBL(self)[0])(self, riid, out ? &object : NULL);
    c->eax = (uint32_t)wrap_by_iid(c, hr, riid, object, out);
    // Autre pointeur d'interface du même objet : même compte, retiré avec lui.
    uint32_t proxy = out && (int32_t)c->eax >= 0 ? R32(out) : 0;
    void *typed = proxy ? proxy_host(proxy) : NULL;
    if (typed && typed != self) link_child(typed, self, 1);
}

void d3d9_manual_IUnknown_Release(CPU *c, void *self, uint32_t a) {
    uint32_t proxy = R32(a - 4);
    uint32_t count = host_release(self);
    if (!count) drop(proxy);
    c->eax = count;
}

static void get_container(CPU *c, void *self, uint32_t a, unsigned slot) {
    const uint8_t *riid = rt_span(c, R32(a), 16);
    uint32_t out = R32(a + 4);
    if (out) rt_write_span(c, out, 4);
    void *object = NULL;
    int32_t hr = ((Query)D3D9_VTBL(self)[slot])(self, riid, out ? &object : NULL);
    c->eax = (uint32_t)wrap_by_iid(c, hr, riid, object, out);
}
void d3d9_manual_IDirect3DSurface9_GetContainer(CPU *c, void *self, uint32_t a) {
    get_container(c, self, a, D3D9_SLOT_IDirect3DSurface9_GetContainer);
}
void d3d9_manual_IDirect3DVolume9_GetContainer(CPU *c, void *self, uint32_t a) {
    get_container(c, self, a, D3D9_SLOT_IDirect3DVolume9_GetContainer);
}

// SetPrivateData(REFGUID, const void *data, DWORD size, DWORD flags) : avec
// D3DSPD_IUNKNOWN, data est un IUnknown* invité (non pris en charge).
static void set_private_data(CPU *c, void *self, uint32_t a, unsigned slot) {
    const void *guid = rt_span(c, R32(a), 16);
    uint32_t data = R32(a + 4), size = R32(a + 8), flags = R32(a + 12);
    if (flags & 1) {
        fprintf(stderr, "d3d9: SetPrivateData with D3DSPD_IUNKNOWN is not bridged (E_NOTIMPL)\n");
        c->eax = D3D9_E_NOTIMPL;
        return;
    }
    c->eax = (uint32_t)((int32_t (*)(void *, const void *, const void *, uint32_t, uint32_t))D3D9_VTBL(self)[slot])(
        self, guid, d3d9_in(c, data, size), size, flags);
}
void d3d9_manual_IDirect3DResource9_SetPrivateData(CPU *c, void *self, uint32_t a) {
    set_private_data(c, self, a, D3D9_SLOT_IDirect3DResource9_SetPrivateData);
}
void d3d9_manual_IDirect3DVolume9_SetPrivateData(CPU *c, void *self, uint32_t a) {
    set_private_data(c, self, a, D3D9_SLOT_IDirect3DVolume9_SetPrivateData);
}

// GetDeclaration(D3DVERTEXELEMENT9 *elements, UINT *count) : la taille vient
// de l'hôte (appel avec elements NULL) avant de valider le tampon invité.
void d3d9_manual_IDirect3DVertexDeclaration9_GetDeclaration(CPU *c, void *self, uint32_t a) {
    typedef int32_t (*Get)(void *, void *, uint32_t *);
    Get get = (Get)D3D9_VTBL(self)[D3D9_SLOT_IDirect3DVertexDeclaration9_GetDeclaration];
    uint32_t elements = R32(a), count_at = R32(a + 4), count = 0;
    if (count_at) rt_write_span(c, count_at, 4);
    int32_t hr = get(self, NULL, &count);
    if (hr >= 0 && elements) hr = get(self, rt_write_span(c, elements, 8ull * count), &count);
    if (count_at) W32(count_at, count);
    c->eax = (uint32_t)hr;
}

static void stub(CPU *c, const D3d9Method *m) {
    static _Atomic uint32_t logged[16];   // un bit par méthode (rt_d3d9_method_count < 512)
    size_t index = (size_t)(m - rt_d3d9_methods);
    uint32_t bit = 1u << (index & 31);
    if (index / 32 < 16 && !(atomic_fetch_or(&logged[index / 32], bit) & bit))
        fprintf(stderr, "d3d9: %s is not bridged (%s)\n", m->name, m->ret == D3D9_RET_HRESULT ? "E_NOTIMPL" : "returns 0");
    if (m->ret == D3D9_RET_FLOAT) rt_return_st0(c, 0.0);
    else if (m->ret != D3D9_RET_VOID) c->eax = m->ret == D3D9_RET_HRESULT ? D3D9_E_NOTIMPL : 0;
}

// ---------------------------------------------------------------- aiguillage
static int d3d9_call(CPU *c, const char *name, void *data) {
    (void)name;
    const D3d9Method *m = data;
    void *self = self_of(c, m);
    if (__builtin_expect(m->kind == D3D9_STUB, 0)) stub(c, m);
    else m->thunk(c, self, c->esp + 8);
    return 1;
}

static int compare_method(const void *key, const void *item) {
    return strcmp((const char *)key, ((const D3d9Method *)item)->name);
}
static const D3d9Method *find_method(const char *name) {
    if (strncmp(name, "IDirect3D", 9) && strncmp(name, "IUnknown::", 10)) return NULL;
    return bsearch(name, rt_d3d9_methods, rt_d3d9_method_count, sizeof(*rt_d3d9_methods), compare_method);
}

static int d3d9_shim(CPU *c, const char *name) {
    if (!strcmp(name, "Direct3DCreate9")) {
        uint32_t sdk = R32(c->esp + 4);
        if (!load_library()) {
            fprintf(stderr, "d3d9: Direct3DCreate9(%u): no native Direct3D 9 library, returning NULL "
                            "(run without -vulkan / P2_RENDERER to use OpenGL)\n", sdk);
            c->eax = 0;
            return 1;
        }
        void *d3d = library_create(sdk);
        if (!d3d) fprintf(stderr, "d3d9: d3d9metal_Direct3DCreate9(%u) returned NULL\n", sdk);
        c->eax = d3d9_proxy(c, d3d, D3D9_IDirect3D9);
        return 1;
    }
    const D3d9Method *m = find_method(name);
    return m ? d3d9_call(c, name, (void *)m) : 0;
}
static int d3d9_resolve(const char *name, RtShimRoute *route) {
    const D3d9Method *m = find_method(name);
    if (!m) return 0;
    route->handler = d3d9_call;
    route->data = (void *)m;
    return 1;
}
RT_SHIM_PROVIDER_RESOLVER(d3d9_shim, d3d9_resolve)
