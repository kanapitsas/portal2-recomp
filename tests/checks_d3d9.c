// Pont Direct3D 9 (runtime/d3d9.c) contre une fausse bibliothèque native :
// objets hôtes à vtables arm64 qui enregistrent les appels. Couvre la table
// générée, Direct3DCreate9, CreateDevice et ses D3DPRESENT_PARAMETERS,
// l'identité et la libération des mandataires, les interfaces en sortie,
// Lock vers de la mémoire alloc_app adressable par l'invité, les structures
// de disposition différente et le chemin chaud SetRenderState/Draw*.
#include "checks.h"
#include "d3d9_bridge.h"
#include "sdl_common.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <execinfo.h>
#include <SDL2/SDL.h>

#define STACK 0x2001c000u
#define SCRATCH 0x2001d000u

// ---------------------------------------------------------------- fausse implémentation hôte
typedef struct Fake { void **vtbl; int refs; uint32_t type; struct Fake *child; uint8_t *bits; } Fake;
static const D3D9MetalHost *fake_host;
static int fake_window;   // SDL_Window* factice (jamais déréférencé)
static struct {
    unsigned render_states, last_state, last_value, draws, blocking;
    int32_t base_vertex; uint32_t draw_args[5];
    float clear_z; uint32_t clear_flags, clear_color;
    const void *vertex_data, *shader, *decl;
    Fake *bound[4];
} seen;

static void unexpected(void) {
    void *frames[8]; int n = backtrace(frames, 8); backtrace_symbols_fd(frames, n, 2);
    assert(!"unexpected host D3D9 call");
}
static void fill(void **table, unsigned count) { for (unsigned i = 0; i < count; ++i) table[i] = (void *)unexpected; }

static uint32_t f_addref(Fake *self) { return (uint32_t)++self->refs; }
static uint32_t f_release(Fake *self) { assert(self->refs > 0); return (uint32_t)--self->refs; }
static uint32_t f_get_type(Fake *self) { return self->type; }
static uint32_t s_addref(Fake *self) { return f_addref(self->child); }   // surface : compte du conteneur
static uint32_t s_release(Fake *self) { return f_release(self->child); }

static void *texture_vtbl[22], *cube_vtbl[22], *surface_vtbl[17], *buffer_vtbl[14], *shader_vtbl[5], *device_vtbl[119], *d3d_vtbl[17];
static void *chain_vtbl[10];
static Fake d3d = {d3d_vtbl, 1, 0, NULL, NULL}, device = {device_vtbl, 0, 0, NULL, NULL};
// Le niveau 0 partage le compte de sa texture, comme dans d3d9 (child = conteneur pour la surface).
static Fake texture;
static Fake surface = {surface_vtbl, 0, 1, &texture, NULL};
static Fake texture = {texture_vtbl, 0, 3, &surface, NULL};
static Fake chain = {chain_vtbl, 1, 0, NULL, NULL}, chain_typed = {chain_vtbl, 0, 0, NULL, NULL};
static Fake backbuffer = {NULL, 1, 1, NULL, NULL};   // compte propre (vtable posée par setup_vtables)
static void *backbuffer_vtbl[17];
static Fake hidden_cube = {cube_vtbl, 1, 5, NULL, NULL}, buffer = {buffer_vtbl, 0, 6, NULL, NULL};
static Fake shader = {shader_vtbl, 0, 0, NULL, NULL}, declaration = {shader_vtbl, 0, 0, NULL, NULL};


static const uint8_t iid_unknown[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0xc0, 0, 0, 0, 0, 0, 0, 0x46};
static int32_t chain_query(Fake *self, const uint8_t *iid, void **out) {
    if (memcmp(iid, iid_unknown, 16) && memcmp(iid, rt_d3d9_interfaces[D3D9_IDirect3DSwapChain9].iid, 16)) {
        *out = NULL; return (int32_t)D3D9_E_NOINTERFACE;
    }
    // IID_IDirect3DSwapChain9 : pointeur d'interface distinct du pointeur IUnknown (permis par COM).
    Fake *typed = memcmp(iid, iid_unknown, 16) ? &chain_typed : self;
    ++typed->refs; *out = typed; return 0;
}
static int32_t surface_container(Fake *self, const uint8_t *iid, void **out) {
    (void)self; assert(!memcmp(iid, iid_unknown, 16));
    ++chain.refs; *out = &chain; return 0;   // conteneur jamais vu par le pont
}
static int32_t f_query(Fake *self, const uint8_t *iid, void **out) {
    // Texture : IUnknown, Resource, BaseTexture, Texture.
    const int known = !memcmp(iid, iid_unknown, 16) ||
                      !memcmp(iid, rt_d3d9_interfaces[D3D9_IDirect3DResource9].iid, 16) ||
                      !memcmp(iid, rt_d3d9_interfaces[D3D9_IDirect3DBaseTexture9].iid, 16) ||
                      !memcmp(iid, rt_d3d9_interfaces[D3D9_IDirect3DTexture9].iid, 16);
    if (!known) { *out = NULL; return (int32_t)D3D9_E_NOINTERFACE; }
    ++self->refs; *out = self; return 0;
}

// IDirect3D9
static int32_t d3d_identifier(Fake *self, uint32_t adapter, uint32_t flags, D3d9HostAdapterId *id) {
    (void)self; (void)flags;
    assert(adapter == 0);
    snprintf(id->Description, sizeof(id->Description), "Fake Metal GPU");
    id->DriverVersion = 0x0001000200030004ll;
    id->VendorId = fake_host->vendor_id; id->DeviceId = 0x1234;
    id->DeviceIdentifier[15] = 0x5a; id->WHQLLevel = 0xabcd;
    return 0;
}
static int32_t d3d_create_device(Fake *self, uint32_t adapter, uint32_t type, void *focus, uint32_t flags,
                                 D3d9HostParams *pp, void **out) {
    (void)self;
    assert(adapter == 0 && type == 1 && focus == &fake_window && flags == 0x40);
    assert(pp && pp->BackBufferWidth == 1280 && pp->BackBufferHeight == 720 && pp->BackBufferFormat == 22);
    assert(pp->SwapEffect == 1 && pp->hDeviceWindow == &fake_window && pp->Windowed == 1);
    assert(pp->EnableAutoDepthStencil == 1 && pp->AutoDepthStencilFormat == 75 && pp->PresentationInterval == 0x80000000u);
    pp->BackBufferCount = 1;   // valeur effective réécrite pour l'application
    device.refs = 1;
    *out = &device;
    return 0;
}

// IDirect3DDevice9
static int32_t dev_get_direct3d(Fake *self, void **out) { (void)self; ++d3d.refs; *out = &d3d; return 0; }
static int32_t dev_creation(Fake *self, D3d9HostCreation *p) {
    (void)self;
    p->AdapterOrdinal = 0; p->DeviceType = 1; p->hFocusWindow = &fake_window; p->BehaviorFlags = 0x40;
    return 0;
}
static int32_t dev_present(Fake *self, const void *src, const void *dst, void *window, const void *dirty) {
    (void)self;
    assert(!src && !dst && !window && !dirty);
    fake_host->blocking_begin(fake_host->user);   // attente de drawable : verrou invité relâché
    fake_host->blocking_begin(fake_host->user);
    fake_host->blocking_end(fake_host->user);
    fake_host->blocking_end(fake_host->user);
    ++seen.blocking;
    return 0;
}
static int32_t dev_back_buffer(Fake *self, uint32_t chain, uint32_t index, uint32_t type, void **out) {
    (void)self;
    assert(chain == 0 && index == 0 && type == 0);
    ++backbuffer.refs; *out = &backbuffer; return 0;
}
static int32_t dev_create_texture(Fake *self, uint32_t w, uint32_t h, uint32_t levels, uint32_t usage, uint32_t format,
                                  uint32_t pool, void **out, void *shared) {
    (void)self; (void)usage;
    assert(w == 256 && h == 128 && levels == 1 && format == 21 && pool == 1 && !shared);
    texture.refs = 1;
    texture.bits = fake_host->alloc_app(fake_host->user, (size_t)w * h * 4, 256);
    assert(texture.bits);
    *out = &texture;
    return 0;
}
static int32_t dev_create_vb(Fake *self, uint32_t length, uint32_t usage, uint32_t fvf, uint32_t pool, void **out, void *shared) {
    (void)self; (void)usage; (void)fvf; (void)pool; (void)shared;
    buffer.refs = 1;
    buffer.bits = fake_host->alloc_app(fake_host->user, length, 16);
    *out = &buffer;
    return 0;
}
static int32_t dev_clear(Fake *self, uint32_t count, const void *rects, uint32_t flags, uint32_t color, float z, uint32_t stencil) {
    (void)self; assert(count == 0 && !rects && stencil == 0);
    seen.clear_flags = flags; seen.clear_color = color; seen.clear_z = z;
    return 0;
}
static int32_t dev_set_render_state(Fake *self, uint32_t state, uint32_t value) {
    (void)self; ++seen.render_states; seen.last_state = state; seen.last_value = value; return 0;
}
static int32_t dev_get_texture(Fake *self, uint32_t stage, void **out) {
    (void)self;
    Fake *t = stage == 1 ? &hidden_cube : seen.bound[stage];
    if (t) ++t->refs;
    *out = t;
    return 0;
}
static int32_t dev_set_texture(Fake *self, uint32_t stage, Fake *t) { (void)self; seen.bound[stage] = t; return 0; }
static float dev_get_npatch(Fake *self) { (void)self; return 4.5f; }
static int32_t dev_draw_indexed(Fake *self, uint32_t type, int32_t base, uint32_t min, uint32_t count, uint32_t start, uint32_t prims) {
    (void)self; ++seen.draws; seen.base_vertex = base;
    uint32_t args[5] = {type, min, count, start, prims};
    memcpy(seen.draw_args, args, sizeof(args));
    return 0;
}
static int32_t dev_draw_up(Fake *self, uint32_t type, uint32_t prims, const void *data, uint32_t stride) {
    (void)self; assert(type == 4 && prims == 2 && stride == 12);
    seen.vertex_data = data; return 0;
}
static int32_t dev_create_decl(Fake *self, const void *elements, void **out) {
    (void)self; seen.decl = elements; declaration.refs = 1; *out = &declaration; return 0;
}
static int32_t dev_create_vs(Fake *self, const uint32_t *tokens, void **out) {
    (void)self; seen.shader = tokens; shader.refs = 1; *out = &shader; return 0;
}

// IDirect3DTexture9 / IDirect3DSurface9 / IDirect3DVertexBuffer9
static int32_t tex_surface_level(Fake *self, uint32_t level, void **out) {
    assert(level == 0);
    s_addref(self->child); *out = self->child; return 0;
}
static int32_t tex_lock(Fake *self, uint32_t level, D3d9HostLocked *locked, const void *rect, uint32_t flags) {
    assert(level == 0 && !rect && flags == 0x2000);
    locked->Pitch = 1024; locked->pBits = self->bits + 64;
    return 0;
}
static int32_t tex_unlock(Fake *self, uint32_t level) { (void)self; assert(level == 0); return 0; }
static int32_t vb_lock(Fake *self, uint32_t offset, uint32_t size, void **out, uint32_t flags) {
    assert(offset == 16 && size == 32 && flags == 0x1000);
    *out = self->bits + offset; return 0;
}
static int32_t vb_unlock(Fake *self) { (void)self; return 0; }

static void setup_vtables(void) {
    fill(d3d_vtbl, 17); fill(device_vtbl, 119); fill(texture_vtbl, 22); fill(cube_vtbl, 22);
    fill(surface_vtbl, 17); fill(buffer_vtbl, 14); fill(shader_vtbl, 5); fill(chain_vtbl, 10);
    void **all[] = {d3d_vtbl, device_vtbl, texture_vtbl, cube_vtbl, buffer_vtbl, shader_vtbl, chain_vtbl};
    for (size_t i = 0; i < sizeof(all) / sizeof(*all); ++i) { all[i][1] = (void *)f_addref; all[i][2] = (void *)f_release; }
    surface_vtbl[1] = (void *)s_addref; surface_vtbl[2] = (void *)s_release; surface_vtbl[11] = (void *)surface_container;
    chain_vtbl[0] = (void *)chain_query;
    fill(backbuffer_vtbl, 17); backbuffer_vtbl[1] = (void *)f_addref; backbuffer_vtbl[2] = (void *)f_release;
    backbuffer.vtbl = backbuffer_vtbl;
    texture_vtbl[0] = (void *)f_query;
    texture_vtbl[10] = cube_vtbl[10] = surface_vtbl[10] = buffer_vtbl[10] = (void *)f_get_type;
    d3d_vtbl[5] = (void *)d3d_identifier; d3d_vtbl[16] = (void *)d3d_create_device;
    device_vtbl[6] = (void *)dev_get_direct3d; device_vtbl[9] = (void *)dev_creation;
    device_vtbl[17] = (void *)dev_present; device_vtbl[18] = (void *)dev_back_buffer;
    device_vtbl[23] = (void *)dev_create_texture; device_vtbl[26] = (void *)dev_create_vb;
    device_vtbl[43] = (void *)dev_clear; device_vtbl[57] = (void *)dev_set_render_state;
    device_vtbl[64] = (void *)dev_get_texture; device_vtbl[65] = (void *)dev_set_texture;
    device_vtbl[80] = (void *)dev_get_npatch; device_vtbl[82] = (void *)dev_draw_indexed;
    device_vtbl[83] = (void *)dev_draw_up; device_vtbl[86] = (void *)dev_create_decl;
    device_vtbl[91] = (void *)dev_create_vs;
    texture_vtbl[18] = (void *)tex_surface_level; texture_vtbl[19] = (void *)tex_lock; texture_vtbl[20] = (void *)tex_unlock;
    buffer_vtbl[11] = (void *)vb_lock; buffer_vtbl[12] = (void *)vb_unlock;
}

static int fake_init(const D3D9MetalHost *host) { fake_host = host; return host->size == sizeof(D3D9MetalHost) ? 0 : -1; }
static void *fake_create(unsigned sdk) { assert(sdk == 32); d3d.refs = 1; return &d3d; }
// Le chargement lit les modes d'affichage par SDL : pilote dummy, pas d'écran réel.
static void use_test_library(d3d9metal_init_fn init) {
    SDL_SetHint(SDL_HINT_VIDEODRIVER, "dummy");
    rt_d3d9_set_library(init, fake_create);
}
static void drop_test_library(void) {
    rt_d3d9_set_library(NULL, NULL);
    SDL_ResetHint(SDL_HINT_VIDEODRIVER);
}

// ---------------------------------------------------------------- appels invités
static uint32_t slot_of(const char *iface, const char *method) {
    char name[128];
    snprintf(name, sizeof(name), "%s::%s", iface, method);
    for (unsigned i = 0; i < rt_d3d9_method_count; ++i)
        if (!strcmp(rt_d3d9_methods[i].name, name)) return rt_d3d9_methods[i].slot;
    assert(!"D3D9 method missing");
    return 0;
}
// Appel cdecl invité de l'emplacement `slot` du mandataire : l'appelant dépile.
static CPU call(uint32_t proxy, uint32_t slot, unsigned count, const uint32_t *args) {
    CPU c; rt_cpu_reset(&c); c.esp = STACK;
    for (unsigned i = count; i; --i) PUSH(&c, args[i - 1]);
    PUSH(&c, proxy);
    uint32_t before = c.esp;
    rt_call(&c, R32(R32(proxy) + 4 * slot), RT_RETURN_SENTINEL);
    assert(c.eip == RT_RETURN_SENTINEL && c.esp == before);
    return c;
}
#define CALL(proxy, iface, method, ...) \
    call((proxy), slot_of(#iface, #method), sizeof((uint32_t[]){__VA_ARGS__}) / 4, (uint32_t[]){__VA_ARGS__})
#define CALL0(proxy, iface, method) call((proxy), slot_of(#iface, #method), 0, NULL)
static unsigned presents;
static void count_present(void) { ++presents; }
static uint32_t tag(unsigned iface) { return D3D9_TAG << 8 | iface; }
static uint32_t bits_of(float f) { uint32_t u; memcpy(&u, &f, 4); return u; }

RT_CHECK(d3d9_generated_tables_match_d3d9_h) {
    for (unsigned i = 1; i < rt_d3d9_method_count; ++i) assert(strcmp(rt_d3d9_methods[i - 1].name, rt_d3d9_methods[i].name) < 0);
    static const struct { unsigned iface, slots; } sizes[] = {
        {D3D9_IDirect3D9, 17}, {D3D9_IDirect3DDevice9, 119}, {D3D9_IDirect3DSwapChain9, 10},
        {D3D9_IDirect3DTexture9, 22}, {D3D9_IDirect3DCubeTexture9, 22}, {D3D9_IDirect3DVolumeTexture9, 22},
        {D3D9_IDirect3DSurface9, 17}, {D3D9_IDirect3DVolume9, 11}, {D3D9_IDirect3DVertexBuffer9, 14},
        {D3D9_IDirect3DIndexBuffer9, 14}, {D3D9_IDirect3DVertexDeclaration9, 5}, {D3D9_IDirect3DVertexShader9, 5},
        {D3D9_IDirect3DPixelShader9, 5}, {D3D9_IDirect3DStateBlock9, 6}, {D3D9_IDirect3DQuery9, 8}};
    for (size_t i = 0; i < sizeof(sizes) / sizeof(*sizes); ++i) {
        const D3d9Interface *info = &rt_d3d9_interfaces[sizes[i].iface];
        assert(!info->abstract && info->slot_count == sizes[i].slots);
        for (unsigned s = 0; s < info->slot_count; ++s) {
            const D3d9Method *m = &rt_d3d9_methods[info->slots[s]];
            assert(m->slot == s && ((m->accept >> sizes[i].iface) & 1));
            assert(m->kind == D3D9_STUB ? !m->thunk : m->thunk != NULL);
        }
    }
    assert(slot_of("IDirect3DDevice9", "SetRenderState") == 57 && slot_of("IDirect3DDevice9", "DrawIndexedPrimitive") == 82);
    assert(slot_of("IDirect3DDevice9", "CreateQuery") == 118 && slot_of("IDirect3DTexture9", "LockRect") == 19);
    assert(D3D9_MASK_IDirect3DBaseTexture9 == (1u << D3D9_IDirect3DTexture9 | 1u << D3D9_IDirect3DCubeTexture9 |
                                               1u << D3D9_IDirect3DVolumeTexture9));
}

RT_CHECK(d3d9_window_flags_become_metal) {
    assert(rt_sdl_window_flags(0x10000008u) == 0x20000008u);   // launcher.so -vulkan : VULKAN | HIDDEN
    assert(rt_sdl_window_flags(0xau) == 0xau);                 // chemin OpenGL inchangé
}

RT_CHECK(d3d9_bridge_against_fake_native_library) {
    setup_vtables();
    memset(&seen, 0, sizeof(seen));
    use_test_library(fake_init);
    unsigned proxies_before = rt_d3d9_live_proxies();
    uint32_t hwnd = rt_handle_new(&fake_window, RT_SDL_WINDOW);
    assert(hwnd);

    // Direct3DCreate9(D3D_SDK_VERSION) : import statique de shaderapivk.so.
    CPU c; rt_cpu_reset(&c); c.esp = STACK;
    PUSH(&c, 32); PUSH(&c, RT_RETURN_SENTINEL);
    assert(rt_shim(&c, "Direct3DCreate9") == 1);
    uint32_t d3d_proxy = c.eax;
    assert(d3d_proxy && R32(d3d_proxy + 4) == tag(D3D9_IDirect3D9) && R64(d3d_proxy + 8) == (uintptr_t)&d3d);
    assert(fake_host && fake_host->vendor_id == 0x106b && fake_host->metal_layer && fake_host->alloc_app);
    assert(!strcmp(rt_dynamic_import_name(R32(R32(d3d_proxy) + 4 * 16)), "IDirect3D9::CreateDevice"));

    // D3DADAPTER_IDENTIFIER9 : 1100 octets i386, rien au-delà.
    uint32_t id = SCRATCH;
    memset(GPTR(id), 0xcc, 1104);
    assert(CALL(d3d_proxy, IDirect3D9, GetAdapterIdentifier, 0, 0, id).eax == 0);
    assert(!strcmp((const char *)GPTR(id + 512), "Fake Metal GPU") && R64(id + 1056) == 0x0001000200030004ull);
    assert(R32(id + 1064) == 0x106b && R32(id + 1068) == 0x1234 && R8(id + 1095) == 0x5a && R32(id + 1096) == 0xabcd);
    assert(R32(id + 1100) == 0xccccccccu);

    // CreateDevice : D3DPRESENT_PARAMETERS i386 (56 octets, HWND en 0x1c) traduits et réécrits.
    uint32_t pp = SCRATCH, out = SCRATCH + 0x100;
    memset(GPTR(pp), 0, 64); W32(pp + 56, 0x5e5e5e5eu);
    W32(pp, 1280); W32(pp + 4, 720); W32(pp + 8, 22); W32(pp + 0x18, 1); W32(pp + 0x1c, hwnd); W32(pp + 0x20, 1);
    W32(pp + 0x24, 1); W32(pp + 0x28, 75); W32(pp + 0x34, 0x80000000u);
    assert(CALL(d3d_proxy, IDirect3D9, CreateDevice, 0, 1, hwnd, 0x40, pp, out).eax == 0);
    uint32_t dev = R32(out);
    assert(dev && R32(dev + 4) == tag(D3D9_IDirect3DDevice9) && R64(dev + 8) == (uintptr_t)&device);
    assert(R32(pp + 0xc) == 1 && R32(pp + 0x1c) == hwnd && R32(pp + 0x20) == 1 && R32(pp + 0x34) == 0x80000000u);
    assert(R32(pp + 56) == 0x5e5e5e5eu);

    // D3DDEVICE_CREATION_PARAMETERS (16 octets i386) et identité de l'IDirect3D9.
    assert(CALL(dev, IDirect3DDevice9, GetCreationParameters, SCRATCH).eax == 0);
    assert(R32(SCRATCH + 4) == 1 && R32(SCRATCH + 8) == hwnd && R32(SCRATCH + 12) == 0x40);
    assert(CALL(dev, IDirect3DDevice9, GetDirect3D, out).eax == 0 && R32(out) == d3d_proxy && d3d.refs == 2);
    assert(CALL0(d3d_proxy, IUnknown, Release).eax == 1);

    // Chemin chaud : SetRenderState, DrawIndexedPrimitive (INT négatif), Clear (float).
    for (unsigned i = 0; i < 1000; ++i) assert(CALL(dev, IDirect3DDevice9, SetRenderState, 7, i).eax == 0);
    assert(seen.render_states == 1000 && seen.last_state == 7 && seen.last_value == 999);
    assert(CALL(dev, IDirect3DDevice9, DrawIndexedPrimitive, 4, (uint32_t)-5, 0, 100, 6, 30).eax == 0);
    assert(seen.draws == 1 && seen.base_vertex == -5 && seen.draw_args[0] == 4 && seen.draw_args[2] == 100 &&
           seen.draw_args[3] == 6 && seen.draw_args[4] == 30);
    assert(CALL(dev, IDirect3DDevice9, Clear, 0, 0, 3, 0xff102030u, bits_of(1.0f), 0).eax == 0);
    assert(seen.clear_z == 1.0f && seen.clear_flags == 3 && seen.clear_color == 0xff102030u);
    CPU fc = CALL0(dev, IDirect3DDevice9, GetNPatchMode);   // float rendu dans ST(0)
    assert(fpop(&fc) == 4.5);

    // Pointeurs d'entrée lus en place : sommets DrawPrimitiveUP, déclaration, jetons de shader.
    uint32_t vertices = SCRATCH + 0x200;
    assert(CALL(dev, IDirect3DDevice9, DrawPrimitiveUP, 4, 2, vertices, 12).eax == 0 && seen.vertex_data == GPTR(vertices));
    uint32_t decl = SCRATCH + 0x300;
    memset(GPTR(decl), 0, 24); W16(decl + 8, 0xff); W8(decl + 12, 17);   // un élément puis D3DDECL_END
    assert(CALL(dev, IDirect3DDevice9, CreateVertexDeclaration, decl, out).eax == 0 && seen.decl == GPTR(decl));
    uint32_t decl_proxy = R32(out);
    assert(R32(decl_proxy + 4) == tag(D3D9_IDirect3DVertexDeclaration9));
    uint32_t tokens = SCRATCH + 0x400;
    static const uint32_t vs[] = {0xfffe0300u, 0x0002fffeu, 0x0000ffffu, 0x1234u,   // commentaire contenant le jeton de fin
                                  0x0200001fu, 0x80000000u, 0x900f0000u, 0x0000ffffu};
    memcpy(GPTR(tokens), vs, sizeof(vs));
    assert(d3d9_shader(&c, tokens) == GPTR(tokens) && d3d9_decl(&c, decl) == GPTR(decl));
    assert(CALL(dev, IDirect3DDevice9, CreateVertexShader, tokens, out).eax == 0 && seen.shader == GPTR(tokens));
    uint32_t vs_proxy = R32(out);
    assert(vs_proxy != decl_proxy && R32(vs_proxy + 4) == tag(D3D9_IDirect3DVertexShader9));

    // Texture : création, Lock vers de la mémoire alloc_app adressable par l'invité.
    assert(CALL(dev, IDirect3DDevice9, CreateTexture, 256, 128, 1, 0, 21, 1, out, 0).eax == 0);
    uint32_t tex = R32(out);
    assert(R32(tex + 4) == tag(D3D9_IDirect3DTexture9) && rt_d3d9_find_proxy(&texture) == tex);
    uint32_t locked = SCRATCH + 0x500;
    assert(CALL(tex, IDirect3DTexture9, LockRect, 0, locked, 0, 0x2000).eax == 0);
    uint32_t pixels = R32(locked + 4);
    assert(R32(locked) == 1024 && pixels == d3d9_guest_pointer(&c, texture.bits + 64) && GPTR(pixels) == texture.bits + 64);
    assert(pixels >= RT_VM_BEGIN && pixels < RT_VM_END && ((pixels - 64) & (uint32_t)(sysconf(_SC_PAGESIZE) - 1)) == 0);
    assert(R32(pixels) == 0);   // mémoire projetée à zéro
    W32(pixels, 0xa1b2c3d4u);   // écriture invitée vue par l'hôte
    assert(texture.bits[64] == 0xd4 && texture.bits[67] == 0xa1);
    assert(CALL(tex, IDirect3DTexture9, UnlockRect, 0).eax == 0);

    // Tampon de sommets : Lock(void **) traduit.
    assert(CALL(dev, IDirect3DDevice9, CreateVertexBuffer, 4096, 8, 0, 0, out, 0).eax == 0);
    uint32_t vb = R32(out);
    assert(CALL(vb, IDirect3DVertexBuffer9, Lock, 16, 32, out, 0x1000).eax == 0);
    assert(R32(out) && GPTR(R32(out)) == buffer.bits + 16);
    assert(CALL0(vb, IDirect3DVertexBuffer9, Unlock).eax == 0);

    // Interfaces en sortie : un mandataire par objet hôte (identité stable).
    assert(CALL(tex, IDirect3DTexture9, GetSurfaceLevel, 0, out).eax == 0);
    uint32_t level = R32(out);
    assert(level && R32(level + 4) == tag(D3D9_IDirect3DSurface9) && texture.refs == 2);
    assert(CALL(tex, IDirect3DTexture9, GetSurfaceLevel, 0, out).eax == 0 && R32(out) == level && texture.refs == 3);
    assert(CALL0(level, IUnknown, Release).eax == 2 && CALL0(level, IUnknown, Release).eax == 1);
    // Tampon arrière : compte propre, retiré avec le périphérique.
    assert(CALL(dev, IDirect3DDevice9, GetBackBuffer, 0, 0, 0, out).eax == 0);
    uint32_t back = R32(out);
    assert(back != level && R32(back + 4) == tag(D3D9_IDirect3DSurface9) && backbuffer.refs == 2);
    assert(CALL0(back, IUnknown, Release).eax == 1 && rt_d3d9_find_proxy(&backbuffer) == back);
    assert(rt_d3d9_find_proxy(&surface) == level);   // encore vivant (compte du conteneur)
    // GetContainer(IID_IUnknown) vers un objet jamais vu : type trouvé par QueryInterface.
    uint32_t unknown = SCRATCH + 0x700;
    memcpy(GPTR(unknown), iid_unknown, 16);
    assert(CALL(level, IDirect3DSurface9, GetContainer, unknown, out).eax == 0);
    uint32_t chain_proxy = R32(out);
    assert(R32(chain_proxy + 4) == tag(D3D9_IDirect3DSwapChain9) && R64(chain_proxy + 8) == (uintptr_t)&chain_typed);
    assert(chain.refs == 1 && chain_typed.refs == 1);   // une seule référence rendue, celle de l'interface typée
    // QueryInterface vers une interface parente : même mandataire.
    uint32_t iid = SCRATCH + 0x600;
    memcpy(GPTR(iid), rt_d3d9_interfaces[D3D9_IDirect3DBaseTexture9].iid, 16);
    assert(CALL(tex, IUnknown, QueryInterface, iid, out).eax == 0 && R32(out) == tex && texture.refs == 2);
    memcpy(GPTR(iid), rt_d3d9_interfaces[D3D9_IDirect3DQuery9].iid, 16);
    assert(CALL(tex, IUnknown, QueryInterface, iid, out).eax == D3D9_E_NOINTERFACE && R32(out) == 0);
    assert(CALL0(tex, IUnknown, Release).eax == 1);
    // Interface en entrée (type de base accepté) puis en sortie sous un type abstrait.
    assert(CALL(dev, IDirect3DDevice9, SetTexture, 0, tex).eax == 0 && seen.bound[0] == &texture);
    assert(CALL(dev, IDirect3DDevice9, GetTexture, 0, out).eax == 0 && R32(out) == tex && texture.refs == 2);
    assert(CALL0(tex, IUnknown, Release).eax == 1);
    // Objet jamais vu, rendu comme IDirect3DBaseTexture9 : type concret par GetType.
    assert(CALL(dev, IDirect3DDevice9, GetTexture, 1, out).eax == 0);
    uint32_t cube = R32(out);
    assert(R32(cube + 4) == tag(D3D9_IDirect3DCubeTexture9));
    assert(!strcmp(rt_dynamic_import_name(R32(R32(cube) + 4 * 18)), "IDirect3DCubeTexture9::GetCubeMapSurface"));
    assert(CALL0(cube, IUnknown, Release).eax == 1);

    // Present : attentes hôtes imbriquées autour du verrou invité ; chaque
    // Present réussi prévient rt_d3d9_on_present (compteur d'images iOS), par
    // la première résolution de l'import comme par la route mémorisée.
    presents = 0;
    rt_d3d9_on_present = count_present;
    assert(CALL(dev, IDirect3DDevice9, Present, 0, 0, 0, 0).eax == 0 && seen.blocking == 1);
    assert(CALL(dev, IDirect3DDevice9, Present, 0, 0, 0, 0).eax == 0 && presents == 2);
    // Méthode non pontée : E_NOTIMPL sans appel hôte.
    assert(CALL(dev, IDirect3DDevice9, DrawRectPatch, 0, 0, 0).eax == D3D9_E_NOTIMPL);
    assert(presents == 2);
    rt_d3d9_on_present = NULL;

    // Release à 0 : mandataire libéré, objet suivant à la même adresse = nouveau mandataire.
    unsigned live = rt_d3d9_live_proxies();
    assert(CALL0(tex, IUnknown, Release).eax == 0 && rt_d3d9_find_proxy(&texture) == 0);
    assert(rt_d3d9_find_proxy(&surface) == 0);   // niveau retiré avec son conteneur
    assert(rt_d3d9_live_proxies() == live - 2);
    fake_host->free_app(fake_host->user, texture.bits, 256 * 128 * 4);
    // Ordre inverse : la texture lâchée d'abord, son niveau ensuite détruit tout le groupe.
    // (Même adresse hôte qu'avant : nouveaux mandataires.)
    assert(CALL(dev, IDirect3DDevice9, CreateTexture, 256, 128, 1, 0, 21, 1, out, 0).eax == 0);
    uint32_t tex2 = R32(out);
    assert(CALL(tex2, IDirect3DTexture9, GetSurfaceLevel, 0, out).eax == 0);
    uint32_t level2 = R32(out);
    assert(rt_d3d9_find_proxy(&texture) == tex2 && rt_d3d9_find_proxy(&surface) == level2);
    assert(CALL0(tex2, IUnknown, Release).eax == 1 && rt_d3d9_find_proxy(&texture) == tex2);
    assert(CALL0(level2, IUnknown, Release).eax == 0);
    assert(rt_d3d9_find_proxy(&texture) == 0 && rt_d3d9_find_proxy(&surface) == 0);
    fake_host->free_app(fake_host->user, texture.bits, 256 * 128 * 4);
    fake_host->free_app(fake_host->user, buffer.bits, 4096);
    assert(CALL0(vb, IUnknown, Release).eax == 0);
    assert(CALL0(vs_proxy, IUnknown, Release).eax == 0 && CALL0(decl_proxy, IUnknown, Release).eax == 0);
    assert(CALL0(dev, IUnknown, Release).eax == 0 && rt_d3d9_find_proxy(&device) == 0);
    assert(rt_d3d9_find_proxy(&backbuffer) == 0);   // retiré avec le périphérique
    assert(CALL0(d3d_proxy, IUnknown, Release).eax == 0);
    // Restent : la chaîne d'échange et le cube cachés (jamais libérés par le faux hôte).
    assert(rt_d3d9_live_proxies() == proxies_before + 2);

    rt_handle_free(hwnd, RT_SDL_WINDOW);
    drop_test_library();
}

RT_CHECK(d3d9_alloc_app_is_guest_memory) {
    use_test_library(fake_init);
    CPU c; rt_cpu_reset(&c); c.esp = STACK;
    PUSH(&c, 32); PUSH(&c, RT_RETURN_SENTINEL);
    assert(rt_shim(&c, "Direct3DCreate9") == 1 && c.eax);
    size_t page = (size_t)sysconf(_SC_PAGESIZE);
    uint8_t *p = fake_host->alloc_app(fake_host->user, 3 * page + 5, page);
    assert(p && ((uintptr_t)p & (page - 1)) == 0);
    uint32_t guest = d3d9_guest_pointer(&c, p);
    assert(guest >= RT_VM_BEGIN && guest + 3 * page + 5 <= RT_VM_END && GPTR(guest) == p);
    rt_write_span(&c, guest, 3 * page + 5);   // accessible en écriture par l'invité
    p[3 * page + 4] = 7;
    assert(R8(guest + 3 * (uint32_t)page + 4) == 7);
    assert(!fake_host->alloc_app(fake_host->user, page, 2 * page));   // alignement au-delà de la page refusé
    fake_host->free_app(fake_host->user, p, 3 * page + 5);
    // Échec propre sans bibliothèque : Direct3DCreate9 rend NULL.
    rt_d3d9_set_library(NULL, NULL);
    const char *given = getenv("P2_D3D9METAL");
    char *saved = given ? strdup(given) : NULL;
    setenv("P2_D3D9METAL", "/nonexistent/libd3d9metal.dylib", 1);
    assert(!rt_d3d9_available());
    rt_cpu_reset(&c); c.esp = STACK;
    PUSH(&c, 32); PUSH(&c, RT_RETURN_SENTINEL);
    assert(rt_shim(&c, "Direct3DCreate9") == 1 && c.eax == 0);
    if (saved) setenv("P2_D3D9METAL", saved, 1); else unsetenv("P2_D3D9METAL");
    free(saved);
    drop_test_library();
}

// Le choix du rendu (src/main.c) initialise la bibliothèque : un refus de
// d3d9metal_init laisse le jeu en OpenGL au lieu d'un Direct3DCreate9 NULL.
static int refusing_init(const D3D9MetalHost *host) { (void)host; return -2; }
RT_CHECK(d3d9_available_means_initialized) {
    use_test_library(refusing_init);
    assert(!rt_d3d9_available());
    CPU c; rt_cpu_reset(&c); c.esp = STACK;
    PUSH(&c, 32); PUSH(&c, RT_RETURN_SENTINEL);
    assert(rt_shim(&c, "Direct3DCreate9") == 1 && c.eax == 0);
    drop_test_library();
}

// Modes d'affichage remis à la bibliothèque : ceux que le jeu voit par SDL
// (pixels), le mode du bureau en premier (pilote dummy : 1024x768 points).
RT_CHECK(d3d9_host_gets_sdl_display_modes) {
    assert(!SDL_WasInit(SDL_INIT_VIDEO) || !strcmp(SDL_GetCurrentVideoDriver(), "dummy"));
    rt_sdl_force_scales(2, 2);
    use_test_library(fake_init);
    assert(rt_d3d9_available());
    assert(fake_host->size == sizeof(D3D9MetalHost) && fake_host->mode_count >= 1 && fake_host->modes);
    assert(fake_host->modes[0].width == 2048 && fake_host->modes[0].height == 1536);
    for (unsigned i = 1; i < fake_host->mode_count; ++i)
        assert(fake_host->modes[i].width <= 2048 && fake_host->modes[i].height <= 1536);
    drop_test_library();
    rt_sdl_force_scales(0, 0);
}

// ---------------------------------------------------------------- identité COM
// Objet aux pointeurs d'interface distincts (permis par COM) : chaîne d'échange
// en multi_a, requête en multi_b, et IUnknown = multi_b (première base, comme
// en héritage multiple C++). Compte unique pour l'objet.
static int multi_refs;
static void *multi_a_vtbl[10], *multi_b_vtbl[8];
static Fake multi_a = {multi_a_vtbl, 0, 0, NULL, NULL}, multi_b = {multi_b_vtbl, 0, 0, NULL, NULL};
static uint32_t m_addref(Fake *self) { (void)self; return (uint32_t)++multi_refs; }
static uint32_t m_release(Fake *self) { (void)self; assert(multi_refs > 0); return (uint32_t)--multi_refs; }
static int32_t m_query(Fake *self, const uint8_t *iid, void **out) {
    (void)self;
    Fake *typed = !memcmp(iid, iid_unknown, 16) ? &multi_b
                  : !memcmp(iid, rt_d3d9_interfaces[D3D9_IDirect3DSwapChain9].iid, 16) ? &multi_a
                  : !memcmp(iid, rt_d3d9_interfaces[D3D9_IDirect3DQuery9].iid, 16) ? &multi_b : NULL;
    *out = typed;
    if (!typed) return (int32_t)D3D9_E_NOINTERFACE;
    ++multi_refs;
    return 0;
}

RT_CHECK(d3d9_iunknown_identity_is_stable_across_interface_pointers) {
    fill(multi_a_vtbl, 10); fill(multi_b_vtbl, 8);
    multi_a_vtbl[0] = multi_b_vtbl[0] = (void *)m_query;
    multi_a_vtbl[1] = multi_b_vtbl[1] = (void *)m_addref;
    multi_a_vtbl[2] = multi_b_vtbl[2] = (void *)m_release;
    unsigned live = rt_d3d9_live_proxies();
    CPU c; rt_cpu_reset(&c); c.esp = STACK;
    multi_refs = 1;
    uint32_t a = d3d9_proxy(&c, &multi_a, D3D9_IDirect3DSwapChain9), out = SCRATCH + 0x100;
    uint32_t unknown = SCRATCH + 0x700, query = SCRATCH + 0x600;
    memcpy(GPTR(unknown), iid_unknown, 16);
    memcpy(GPTR(query), rt_d3d9_interfaces[D3D9_IDirect3DQuery9].iid, 16);
    assert(CALL(a, IUnknown, QueryInterface, unknown, out).eax == 0);
    uint32_t identity = R32(out);
    assert(identity == a && multi_refs == 2);
    assert(CALL(a, IUnknown, QueryInterface, query, out).eax == 0);
    uint32_t b = R32(out);
    assert(b != a && R32(b + 4) == tag(D3D9_IDirect3DQuery9) && R64(b + 8) == (uintptr_t)&multi_b && multi_refs == 3);
    // QueryInterface(IID_IUnknown) rend le même pointeur quel que soit le mandataire interrogé.
    assert(CALL(b, IUnknown, QueryInterface, unknown, out).eax == 0 && R32(out) == identity && multi_refs == 4);
    assert(CALL(a, IUnknown, QueryInterface, unknown, out).eax == 0 && R32(out) == identity && multi_refs == 5);
    assert(CALL(b, IUnknown, QueryInterface, query, out).eax == 0 && R32(out) == b && multi_refs == 6);
    for (int expected = 5; expected >= 1; --expected)
        assert(CALL0(expected & 1 ? a : b, IUnknown, Release).eax == (uint32_t)expected);
    // Dernier Release par l'autre interface : tout l'objet est retiré.
    assert(CALL0(b, IUnknown, Release).eax == 0);
    assert(!rt_d3d9_find_proxy(&multi_a) && !rt_d3d9_find_proxy(&multi_b) && rt_d3d9_live_proxies() == live);
}

// GetContainer(IID_IUnknown) vers un conteneur jamais vu dont l'interface typée
// est le même pointeur : une seule référence rendue au jeu.
static void *orphan_level_vtbl[17];
static Fake orphan = {texture_vtbl, 0, 3, NULL, NULL}, orphan_level = {orphan_level_vtbl, 0, 1, NULL, NULL};
static int32_t orphan_container(Fake *self, const uint8_t *iid, void **out) {
    (void)self; assert(!memcmp(iid, iid_unknown, 16));
    ++orphan.refs; *out = &orphan; return 0;
}

RT_CHECK(d3d9_get_container_iunknown_keeps_one_reference) {
    setup_vtables();
    fill(orphan_level_vtbl, 17);
    orphan_level_vtbl[1] = (void *)f_addref; orphan_level_vtbl[2] = (void *)f_release;
    orphan_level_vtbl[11] = (void *)orphan_container;
    unsigned live = rt_d3d9_live_proxies();
    CPU c; rt_cpu_reset(&c); c.esp = STACK;
    orphan.refs = 1; orphan_level.refs = 1;
    uint32_t level = d3d9_proxy(&c, &orphan_level, D3D9_IDirect3DSurface9), out = SCRATCH + 0x100, unknown = SCRATCH + 0x700;
    memcpy(GPTR(unknown), iid_unknown, 16);
    assert(CALL(level, IDirect3DSurface9, GetContainer, unknown, out).eax == 0);
    uint32_t container = R32(out);
    assert(R32(container + 4) == tag(D3D9_IDirect3DTexture9) && R64(container + 8) == (uintptr_t)&orphan);
    assert(orphan.refs == 2);
    assert(CALL0(container, IUnknown, Release).eax == 1 && CALL0(container, IUnknown, Release).eax == 0);
    assert(CALL0(level, IUnknown, Release).eax == 0 && rt_d3d9_live_proxies() == live);
}

// Plafonds mémoire de mtld3d adaptés à l'espace invité, posés avant
// l'initialisation ; une clé donnée par l'utilisateur reste la dernière (gagne).
RT_CHECK(d3d9_memory_caps_precede_user_config) {
    const char *given = getenv("MTLD3D_CONFIG");
    char *saved = given ? strdup(given) : NULL;
    unsetenv("MTLD3D_CONFIG");
    use_test_library(fake_init);
    assert(rt_d3d9_available());
    assert(getenv("MTLD3D_CONFIG") && !strcmp(getenv("MTLD3D_CONFIG"), rt_d3d9_memory_defaults));
    assert(strstr(rt_d3d9_memory_defaults, "memory.pageboxPoolCapMB=") && strstr(rt_d3d9_memory_defaults, "memory.vbibRetentionCapMB="));
    drop_test_library();
    setenv("MTLD3D_CONFIG", "memory.pageboxPoolCapMB=16", 1);
    use_test_library(fake_init);
    assert(rt_d3d9_available());
    const char *joined = getenv("MTLD3D_CONFIG");
    size_t n = strlen(rt_d3d9_memory_defaults);
    assert(!strncmp(joined, rt_d3d9_memory_defaults, n) && !strcmp(joined + n, ";memory.pageboxPoolCapMB=16"));
    drop_test_library();
    use_test_library(fake_init);   // pas de doublon au rechargement
    assert(rt_d3d9_available() && !strcmp(getenv("MTLD3D_CONFIG"), joined));
    drop_test_library();
    if (saved) setenv("MTLD3D_CONFIG", saved, 1); else unsetenv("MTLD3D_CONFIG");
    free(saved);
}

// ---------------------------------------------------------------- vraie bibliothèque
// Avec P2_D3D9METAL=/chemin/libd3d9metal.dylib (sinon ignoré) : le parcours
// du jeu par les thunks invités, contre la vraie implémentation, sans
// fenêtre (CAMetalLayer hors écran) et verrou invité tenu comme par un thread
// du jeu. Formats FOURCC que shaderapivk.so connaît : tableau sur stderr.
#include <dlfcn.h>
#include <objc/message.h>
#include <objc/runtime.h>

enum { NATIVE_SIZE = 64 };
static void *native_layer;
static int native_window;   // SDL_Window* factice, jamais déréférencé
static void *native_metal_layer(void *hwnd, uint32_t *width, uint32_t *height) {
    if (hwnd != &native_window) return NULL;
    *width = NATIVE_SIZE; *height = NATIVE_SIZE;
    return native_layer;
}
static void *make_offscreen_layer(void) {
    if (!dlopen("/System/Library/Frameworks/QuartzCore.framework/QuartzCore", RTLD_NOW)) return NULL;
    Class class = objc_getClass("CAMetalLayer");
    if (!class) return NULL;
    id layer = ((id (*)(Class, SEL))objc_msgSend)(class, sel_registerName("new"));
    typedef struct { double x, y, w, h; } Rect;
    ((void (*)(id, SEL, Rect))objc_msgSend)(layer, sel_registerName("setBounds:"), (Rect){0, 0, NATIVE_SIZE, NATIVE_SIZE});
    ((void (*)(id, SEL, double))objc_msgSend)(layer, sel_registerName("setContentsScale:"), 1.0);
    return layer;
}
static uint32_t fourcc(const char *code) { return (uint32_t)code[0] | (uint32_t)code[1] << 8 | (uint32_t)code[2] << 16 | (uint32_t)code[3] << 24; }
static uint32_t guest_copy(const void *data, uint32_t size) {
    uint32_t address = rt_alloc(size);
    assert(address);
    memcpy(GPTR(address), data, size);
    return address;
}

RT_CHECK(d3d9_bridge_against_real_native_library) {
    const char *path = getenv("P2_D3D9METAL");
    if (!path || !*path) { fprintf(stderr, "d3d9: P2_D3D9METAL unset, real library check skipped\n"); return; }
    native_layer = make_offscreen_layer();
    assert(native_layer);
    rt_d3d9_set_test_layer(native_metal_layer);
    // Dessins synchrones (l'image est relue aussitôt), aucun cache écrit.
    const char *given = getenv("MTLD3D_CONFIG");
    char *saved = given ? strdup(given) : NULL;
    setenv("MTLD3D_CONFIG", "shader.asyncCompile=false;shaderCache.enable=false", 1);
    SDL_SetHint(SDL_HINT_VIDEODRIVER, "dummy");
    rt_d3d9_set_library(NULL, NULL);
    unsigned proxies_before = rt_d3d9_live_proxies();
    uint32_t hwnd = rt_handle_new(&native_window, RT_SDL_WINDOW);
    rt_guest_enter();   // comme un thread du jeu

    CPU c; rt_cpu_reset(&c); c.esp = STACK;
    PUSH(&c, 32); PUSH(&c, RT_RETURN_SENTINEL);
    assert(rt_shim(&c, "Direct3DCreate9") == 1);
    uint32_t d3d = c.eax;
    assert(d3d && R32(d3d + 4) == tag(D3D9_IDirect3D9));
    const D3D9MetalHost *host = rt_d3d9_host();

    uint32_t id = rt_alloc(1104), out = rt_alloc(16);
    assert(CALL(d3d, IDirect3D9, GetAdapterIdentifier, 0, 0, id).eax == 0);
    fprintf(stderr, "d3d9 native: adapter \"%s\" vendor %04x device %04x\n", (const char *)GPTR(id + 512), R32(id + 1064), R32(id + 1068));
    assert(R32(id + 1064) == host->vendor_id);
    if (host->mode_count) {
        uint32_t mode = rt_alloc(16);   // D3DDISPLAYMODE : Width, Height, RefreshRate, Format
        assert(CALL(d3d, IDirect3D9, GetAdapterDisplayMode, 0, mode).eax == 0);
        fprintf(stderr, "d3d9 native: display mode %ux%u@%u (host first mode %ux%u)\n", R32(mode), R32(mode + 4), R32(mode + 8),
                host->modes[0].width, host->modes[0].height);
    }

    // CheckDeviceFormat(adapter, HAL, X8R8G8B8, usage, type, format).
    static const struct { const char *code; uint32_t usage, type; const char *as; } formats[] = {
        {"INTZ", 2, 3, "depth texture"}, {"DF24", 2, 3, "depth texture"}, {"DF16", 2, 3, "depth texture"},
        {"RAWZ", 2, 3, "depth texture"}, {"NULL", 1, 1, "render target surface"}, {"ATI1", 0, 3, "texture"},
        {"ATI2", 0, 3, "texture"}, {"ATOC", 0, 1, "surface"}};
    fprintf(stderr, "d3d9 native: CheckDeviceFormat\n");
    for (size_t i = 0; i < sizeof(formats) / sizeof(*formats); ++i) {
        uint32_t hr = CALL(d3d, IDirect3D9, CheckDeviceFormat, 0, 1, 22, formats[i].usage, formats[i].type, fourcc(formats[i].code)).eax;
        fprintf(stderr, "  %s as %-21s %s (%#x)\n", formats[i].code, formats[i].as, hr == 0 ? "supported" : "no", hr);
    }

    // CreateDevice : D3DPRESENT_PARAMETERS i386, fenêtre factice → couche hors écran.
    uint32_t pp = rt_alloc(64);
    memset(GPTR(pp), 0, 56);
    W32(pp, NATIVE_SIZE); W32(pp + 4, NATIVE_SIZE); W32(pp + 8, 22); W32(pp + 0xc, 1); W32(pp + 0x18, 1);
    W32(pp + 0x1c, hwnd); W32(pp + 0x20, 1); W32(pp + 0x34, 0x80000000u);
    assert(CALL(d3d, IDirect3D9, CreateDevice, 0, 1, hwnd, 0x20 /* HARDWARE_VERTEXPROCESSING */, pp, out).eax == 0);
    uint32_t dev = R32(out);
    assert(dev && R32(dev + 4) == tag(D3D9_IDirect3DDevice9));
    // Requête d'événement (« D3DQUERYTYPE_EVENT not available on this driver » sinon).
    uint32_t query_hr = CALL(dev, IDirect3DDevice9, CreateQuery, 8, 0).eax;
    fprintf(stderr, "  D3DQUERYTYPE_EVENT       %s (%#x)\n", query_hr == 0 ? "supported" : "no", query_hr);

    // Tampon de sommets écrit par l'invité via Lock.
    static const float positions[3][3] = {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.5f, 0.5f}, {0.5f, -0.5f, 0.5f}};
    assert(CALL(dev, IDirect3DDevice9, CreateVertexBuffer, 48, 8 /* WRITEONLY */, 0x42 /* XYZ|DIFFUSE */, 0, out, 0).eax == 0);
    uint32_t vb = R32(out);
    assert(CALL(vb, IDirect3DVertexBuffer9, Lock, 0, 0, out, 0).eax == 0);
    uint32_t data = R32(out);
    assert(data >= RT_VM_BEGIN && data < RT_VM_END);
    for (unsigned v = 0; v < 3; ++v) {
        memcpy(GPTR(data + 16 * v), positions[v], 12);
        W32(data + 16 * v + 12, 0xff00ff00u);
    }
    assert(CALL0(vb, IDirect3DVertexBuffer9, Unlock).eax == 0);

    static const uint32_t vs_tokens[] = {0xfffe0200, 0x0200001f, 0x80000000, 0x900f0000, 0x0200001f, 0x8000000a, 0x900f0001,
                                         0x02000001, 0xc00f0000, 0x90e40000, 0x02000001, 0xd00f0000, 0x90e40001, 0x0000ffff};
    static const uint32_t ps_tokens[] = {0xffff0200, 0x0200001f, 0x80000000, 0x900f0000,
                                         0x02000001, 0x800f0800, 0x90e40000, 0x0000ffff};
    assert(CALL(dev, IDirect3DDevice9, CreateVertexShader, guest_copy(vs_tokens, sizeof(vs_tokens)), out).eax == 0);
    uint32_t vs = R32(out);
    assert(CALL(dev, IDirect3DDevice9, CreatePixelShader, guest_copy(ps_tokens, sizeof(ps_tokens)), out).eax == 0);
    uint32_t ps = R32(out);

    assert(CALL(dev, IDirect3DDevice9, Clear, 0, 0, 1 /* TARGET */, 0xffff0000u, bits_of(1.0f), 0).eax == 0);
    assert(CALL0(dev, IDirect3DDevice9, BeginScene).eax == 0);
    assert(CALL(dev, IDirect3DDevice9, SetRenderState, 137 /* LIGHTING */, 0).eax == 0);
    assert(CALL(dev, IDirect3DDevice9, SetRenderState, 22 /* CULLMODE */, 1 /* NONE */).eax == 0);
    assert(CALL(dev, IDirect3DDevice9, SetVertexShader, vs).eax == 0);
    assert(CALL(dev, IDirect3DDevice9, SetPixelShader, ps).eax == 0);
    assert(CALL(dev, IDirect3DDevice9, SetFVF, 0x42).eax == 0);
    assert(CALL(dev, IDirect3DDevice9, SetStreamSource, 0, vb, 0, 16).eax == 0);
    assert(CALL(dev, IDirect3DDevice9, DrawPrimitive, 4 /* TRIANGLELIST */, 0, 1).eax == 0);
    assert(CALL0(dev, IDirect3DDevice9, EndScene).eax == 0);

    // Relecture : GetRenderTargetData vers une surface système, LockRect adressable par l'invité.
    assert(CALL(dev, IDirect3DDevice9, GetBackBuffer, 0, 0, 0, out).eax == 0);
    uint32_t back = R32(out);
    assert(CALL(dev, IDirect3DDevice9, CreateOffscreenPlainSurface, NATIVE_SIZE, NATIVE_SIZE, 21, 2 /* SYSTEMMEM */, out, 0).eax == 0);
    uint32_t readback = R32(out);
    assert(CALL(dev, IDirect3DDevice9, GetRenderTargetData, back, readback).eax == 0);
    uint32_t locked = rt_alloc(8);
    assert(CALL(readback, IDirect3DSurface9, LockRect, locked, 0, 0x10 /* READONLY */).eax == 0);
    uint32_t pitch = R32(locked), bits = R32(locked + 4);
    assert(pitch >= 4 * NATIVE_SIZE && bits >= RT_VM_BEGIN && bits + pitch * NATIVE_SIZE <= RT_VM_END);
    uint32_t center = R32(bits + (NATIVE_SIZE / 2 + 4) * pitch + 4 * (NATIVE_SIZE / 2)), corner = R32(bits + pitch + 4);
    fprintf(stderr, "d3d9 native: read back centre %#010x corner %#010x\n", center, corner);
    assert((center & 0xffffff) == 0x00ff00 && (corner & 0xffffff) == 0xff0000);
    assert(CALL(readback, IDirect3DSurface9, UnlockRect).eax == 0);

    assert(CALL(dev, IDirect3DDevice9, Present, 0, 0, 0, 0).eax == 0);

    // Conteneur du tampon arrière demandé comme IUnknown : typé par
    // QueryInterface (chaîne d'échange), identité stable.
    uint32_t unknown = guest_copy(iid_unknown, 16);
    assert(CALL(back, IDirect3DSurface9, GetContainer, unknown, out).eax == 0);
    uint32_t container = R32(out);
    fprintf(stderr, "d3d9 native: back buffer container is a %s\n", rt_d3d9_interfaces[R32(container + 4) & 0xff].name);
    assert(CALL(container, IUnknown, QueryInterface, unknown, out).eax == 0 && R32(out) == container);
    assert(CALL(back, IUnknown, QueryInterface, unknown, out).eax == 0);
    uint32_t back_identity = R32(out);
    assert(CALL(back, IUnknown, QueryInterface, unknown, out).eax == 0 && R32(out) == back_identity);
    CALL0(back_identity, IUnknown, Release); CALL0(back_identity, IUnknown, Release);
    CALL0(container, IUnknown, Release); CALL0(container, IUnknown, Release);

    // Tout rendu à 0 ; le tampon arrière est retiré avec le périphérique.
    assert(CALL0(readback, IUnknown, Release).eax == 0);
    CALL0(back, IUnknown, Release);
    assert(CALL0(vb, IUnknown, Release).eax == 0);
    assert(CALL0(vs, IUnknown, Release).eax == 0 && CALL0(ps, IUnknown, Release).eax == 0);
    assert(CALL0(dev, IUnknown, Release).eax == 0);
    assert(CALL0(d3d, IUnknown, Release).eax == 0);
    assert(rt_d3d9_live_proxies() == proxies_before);

    rt_guest_leave();
    rt_handle_free(hwnd, RT_SDL_WINDOW);
    rt_d3d9_set_test_layer(NULL);
    rt_d3d9_set_library(NULL, NULL);
    SDL_ResetHint(SDL_HINT_VIDEODRIVER);
    if (saved) setenv("MTLD3D_CONFIG", saved, 1); else unsetenv("MTLD3D_CONFIG");
    free(saved);
}
