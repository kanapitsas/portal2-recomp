// libd3d9metal seule, liée statiquement comme dans l'app iOS (sans le pont ni
// le runtime) : init avec une table hôte minimale, appareil sur une
// CAMetalLayer hors écran, effacement relu par GetRenderTargetData, textures
// DXT (BC) créées et remplies, Present, tout libéré. Compilé pour macOS
// (make build/d3d9metal-check D3D9METAL_A=…) ou pour le simulateur iOS
// (tools/ios_d3d9metal_check.sh), où il tourne par simctl spawn : il vérifie
// le lien statique et les chemins iOS de mtld3d sans lancer le jeu.
#include "../runtime/d3d9/d3d9metal.h"
#include <dlfcn.h>
#include <objc/message.h>
#include <objc/runtime.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

// Rangs de vtable (ordre de d3d9.h, ceux de runtime/d3d9_generated.c).
enum {
    RELEASE = 2,
    D3D_CHECK_DEVICE_FORMAT = 10, D3D_CREATE_DEVICE = 16,
    DEV_PRESENT = 17, DEV_CREATE_TEXTURE = 23, DEV_GET_RENDER_TARGET_DATA = 32, DEV_CREATE_OFFSCREEN = 36,
    DEV_GET_RENDER_TARGET = 38, DEV_BEGIN_SCENE = 41, DEV_END_SCENE = 42, DEV_CLEAR = 43,
    SURF_LOCK = 13, SURF_UNLOCK = 14, TEX_LOCK = 19, TEX_UNLOCK = 20,
};
#define VTBL(o) (*(void *const *const *)(o))
#define METHOD(o, slot, type) ((type)VTBL(o)[slot])
typedef int32_t HR;
typedef struct {   // D3DPRESENT_PARAMETERS, disposition arm64 (HWND 64 bits)
    uint32_t BackBufferWidth, BackBufferHeight, BackBufferFormat, BackBufferCount;
    uint32_t MultiSampleType, MultiSampleQuality, SwapEffect;
    void *hDeviceWindow;
    uint32_t Windowed, EnableAutoDepthStencil, AutoDepthStencilFormat, Flags;
    uint32_t FullScreen_RefreshRateInHz, PresentationInterval;
} Params;
typedef struct { int32_t Pitch; void *pBits; } Locked;   // D3DLOCKED_RECT
enum { A8R8G8B8 = 21, X8R8G8B8 = 22, SIZE = 64 };
static uint32_t fourcc(const char *c) { return (uint32_t)c[0] | (uint32_t)c[1] << 8 | (uint32_t)c[2] << 16 | (uint32_t)c[3] << 24; }

static int failures;
#define CHECK(cond, ...) do { if (!(cond)) { ++failures; fprintf(stderr, "FAIL %s:%d: ", __FILE__, __LINE__); \
    fprintf(stderr, __VA_ARGS__); fputc('\n', stderr); } } while (0)

static void *layer;
static int window;   // HWND factice
static void *metal_layer(void *user, void *hwnd, uint32_t *w, uint32_t *h) {
    (void)user;
    if (hwnd != &window) return NULL;
    *w = SIZE; *h = SIZE;
    return layer;
}
static void *alloc_app(void *user, size_t size, size_t align) {
    (void)user; (void)align;
    void *p = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    return p == MAP_FAILED ? NULL : p;
}
static void free_app(void *user, void *ptr, size_t size) { (void)user; munmap(ptr, size); }
static void blocking(void *user) { (void)user; }

static void *make_layer(void) {
    Class class = objc_getClass("CAMetalLayer");
    if (!class) return NULL;
    id l = ((id (*)(Class, SEL))objc_msgSend)(class, sel_registerName("new"));
    typedef struct { double x, y, w, h; } Rect;
    ((void (*)(id, SEL, Rect))objc_msgSend)(l, sel_registerName("setBounds:"), (Rect){0, 0, SIZE, SIZE});
    ((void (*)(id, SEL, double))objc_msgSend)(l, sel_registerName("setContentsScale:"), 1.0);
    return l;
}

int main(void) {
    // Synchrone (l'image est relue aussitôt), sans cache écrit.
    setenv("MTLD3D_CONFIG", "shader.asyncCompile=false;shaderCache.enable=false", 0);
    layer = make_layer();
    if (!layer) { fprintf(stderr, "no CAMetalLayer\n"); return 2; }
    static const D3D9MetalDisplayMode modes[] = {{SIZE, SIZE, 60}};
    D3D9MetalHost host = {.size = sizeof(host), .metal_layer = metal_layer, .alloc_app = alloc_app,
                          .free_app = free_app, .blocking_begin = blocking, .blocking_end = blocking,
                          .vendor_id = 0x106b, .modes = modes, .mode_count = 1};
    int status = d3d9metal_init(&host);
    if (status) { fprintf(stderr, "d3d9metal_init: %d\n", status); return 1; }
    void *d3d = d3d9metal_Direct3DCreate9(32);
    if (!d3d) { fprintf(stderr, "Direct3DCreate9 returned NULL\n"); return 1; }

    typedef HR (*CheckFormat)(void *, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
    const char *dxt[] = {"DXT1", "DXT3", "DXT5"};
    for (int i = 0; i < 3; ++i) {
        HR hr = METHOD(d3d, D3D_CHECK_DEVICE_FORMAT, CheckFormat)(d3d, 0, 1, X8R8G8B8, 0, 3 /* TEXTURE */, fourcc(dxt[i]));
        printf("CheckDeviceFormat %s texture: %s\n", dxt[i], hr >= 0 ? "supported" : "not supported");
        CHECK(hr >= 0, "%s refused (%#x)", dxt[i], (unsigned)hr);
    }

    Params pp = {.BackBufferWidth = SIZE, .BackBufferHeight = SIZE, .BackBufferFormat = A8R8G8B8, .BackBufferCount = 1,
                 .SwapEffect = 1, .hDeviceWindow = &window, .Windowed = 1, .PresentationInterval = 0x80000000u};
    void *dev = NULL;
    HR hr = METHOD(d3d, D3D_CREATE_DEVICE, HR (*)(void *, uint32_t, uint32_t, void *, uint32_t, Params *, void **))(
        d3d, 0, 1, &window, 0x40 /* HARDWARE_VERTEXPROCESSING */, &pp, &dev);
    if (hr < 0 || !dev) { fprintf(stderr, "CreateDevice: %#x\n", (unsigned)hr); return 1; }

    // Effacement relu depuis la mémoire « app ».
    const uint32_t color = 0xff3366ccu;
    METHOD(dev, DEV_BEGIN_SCENE, HR (*)(void *))(dev);
    hr = METHOD(dev, DEV_CLEAR, HR (*)(void *, uint32_t, const void *, uint32_t, uint32_t, float, uint32_t))(
        dev, 0, NULL, 1 /* TARGET */, color, 1.0f, 0);
    CHECK(hr >= 0, "Clear %#x", (unsigned)hr);
    METHOD(dev, DEV_END_SCENE, HR (*)(void *))(dev);
    void *rt = NULL, *sys = NULL;
    CHECK(METHOD(dev, DEV_GET_RENDER_TARGET, HR (*)(void *, uint32_t, void **))(dev, 0, &rt) >= 0 && rt, "GetRenderTarget");
    CHECK(METHOD(dev, DEV_CREATE_OFFSCREEN, HR (*)(void *, uint32_t, uint32_t, uint32_t, uint32_t, void **, void *))(
              dev, SIZE, SIZE, A8R8G8B8, 2 /* SYSTEMMEM */, &sys, NULL) >= 0 && sys, "CreateOffscreenPlainSurface");
    if (rt && sys) {
        CHECK(METHOD(dev, DEV_GET_RENDER_TARGET_DATA, HR (*)(void *, void *, void *))(dev, rt, sys) >= 0, "GetRenderTargetData");
        Locked lr = {0};
        hr = METHOD(sys, SURF_LOCK, HR (*)(void *, Locked *, const void *, uint32_t))(sys, &lr, NULL, 0x10 /* READONLY */);
        CHECK(hr >= 0 && lr.pBits, "LockRect %#x", (unsigned)hr);
        if (hr >= 0 && lr.pBits) {
            uint32_t pixel = ((const uint32_t *)lr.pBits)[SIZE / 2 * (lr.Pitch / 4) + SIZE / 2];
            printf("cleared pixel: %08x (expected %08x)\n", pixel, color);
            CHECK(pixel == color, "pixel %08x", pixel);
            METHOD(sys, SURF_UNLOCK, HR (*)(void *))(sys);
        }
    }

    // Textures DXT gérées, comme celles des matériaux de Source.
    for (int i = 0; i < 3; ++i) {
        void *tex = NULL;
        hr = METHOD(dev, DEV_CREATE_TEXTURE, HR (*)(void *, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t,
                                                     void **, void *))(dev, SIZE, SIZE, 1, 0, fourcc(dxt[i]), 1 /* MANAGED */, &tex, NULL);
        CHECK(hr >= 0 && tex, "CreateTexture %s %#x", dxt[i], (unsigned)hr);
        if (!tex) continue;
        Locked lr = {0};
        hr = METHOD(tex, TEX_LOCK, HR (*)(void *, uint32_t, Locked *, const void *, uint32_t))(tex, 0, &lr, NULL, 0);
        CHECK(hr >= 0 && lr.pBits, "texture LockRect %s %#x", dxt[i], (unsigned)hr);
        if (hr >= 0 && lr.pBits) {
            memset(lr.pBits, 0x5a, (size_t)lr.Pitch * (SIZE / 4));   // Pitch : octets par ligne de blocs
            METHOD(tex, TEX_UNLOCK, HR (*)(void *, uint32_t))(tex, 0);
        }
        CHECK(METHOD(tex, RELEASE, uint32_t (*)(void *))(tex) == 0, "texture Release");
    }

    hr = METHOD(dev, DEV_PRESENT, HR (*)(void *, const void *, const void *, void *, const void *))(dev, NULL, NULL, NULL, NULL);
    CHECK(hr >= 0, "Present %#x", (unsigned)hr);
    if (sys) METHOD(sys, RELEASE, uint32_t (*)(void *))(sys);
    if (rt) METHOD(rt, RELEASE, uint32_t (*)(void *))(rt);
    CHECK(METHOD(dev, RELEASE, uint32_t (*)(void *))(dev) == 0, "device Release");
    CHECK(METHOD(d3d, RELEASE, uint32_t (*)(void *))(d3d) == 0, "IDirect3D9 Release");
    printf("d3d9metal native check: %s\n", failures ? "FAILED" : "OK");
    return failures ? 1 : 0;
}
