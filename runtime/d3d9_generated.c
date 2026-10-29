// GÉNÉRÉ par tools/gen_d3d9.py — ne pas éditer à la main.
// Thunks COM i386 cdecl → objets Direct3D 9 hôtes (ABI arm64), tables des
// méthodes et des vtables. Aides d3d9_* : runtime/d3d9.c (d3d9_bridge.h).
#include "d3d9_bridge.h"

static void t_IDirect3D9_CheckDepthStencilMatch(CPU *c, void *self, uint32_t a) {
    uint32_t g_Adapter = R32(a + 0);
    uint32_t g_DeviceType = R32(a + 4);
    uint32_t g_AdapterFormat = R32(a + 8);
    uint32_t g_RenderTargetFormat = R32(a + 12);
    uint32_t g_DepthStencilFormat = R32(a + 16);
    uint32_t a_Adapter = g_Adapter;
    uint32_t a_DeviceType = g_DeviceType;
    uint32_t a_AdapterFormat = g_AdapterFormat;
    uint32_t a_RenderTargetFormat = g_RenderTargetFormat;
    uint32_t a_DepthStencilFormat = g_DepthStencilFormat;
    int32_t hr = ((int32_t (*)(void *, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t))D3D9_VTBL(self)[12])(self, a_Adapter, a_DeviceType, a_AdapterFormat, a_RenderTargetFormat, a_DepthStencilFormat);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3D9_CheckDeviceFormat(CPU *c, void *self, uint32_t a) {
    uint32_t g_Adapter = R32(a + 0);
    uint32_t g_DeviceType = R32(a + 4);
    uint32_t g_AdapterFormat = R32(a + 8);
    uint32_t g_Usage = R32(a + 12);
    uint32_t g_RType = R32(a + 16);
    uint32_t g_CheckFormat = R32(a + 20);
    uint32_t a_Adapter = g_Adapter;
    uint32_t a_DeviceType = g_DeviceType;
    uint32_t a_AdapterFormat = g_AdapterFormat;
    uint32_t a_Usage = g_Usage;
    uint32_t a_RType = g_RType;
    uint32_t a_CheckFormat = g_CheckFormat;
    int32_t hr = ((int32_t (*)(void *, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t))D3D9_VTBL(self)[10])(self, a_Adapter, a_DeviceType, a_AdapterFormat, a_Usage, a_RType, a_CheckFormat);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3D9_CheckDeviceFormatConversion(CPU *c, void *self, uint32_t a) {
    uint32_t g_Adapter = R32(a + 0);
    uint32_t g_DeviceType = R32(a + 4);
    uint32_t g_SourceFormat = R32(a + 8);
    uint32_t g_TargetFormat = R32(a + 12);
    uint32_t a_Adapter = g_Adapter;
    uint32_t a_DeviceType = g_DeviceType;
    uint32_t a_SourceFormat = g_SourceFormat;
    uint32_t a_TargetFormat = g_TargetFormat;
    int32_t hr = ((int32_t (*)(void *, uint32_t, uint32_t, uint32_t, uint32_t))D3D9_VTBL(self)[13])(self, a_Adapter, a_DeviceType, a_SourceFormat, a_TargetFormat);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3D9_CheckDeviceMultiSampleType(CPU *c, void *self, uint32_t a) {
    uint32_t g_Adapter = R32(a + 0);
    uint32_t g_DeviceType = R32(a + 4);
    uint32_t g_SurfaceFormat = R32(a + 8);
    uint32_t g_Windowed = R32(a + 12);
    uint32_t g_MultiSampleType = R32(a + 16);
    uint32_t g_pQualityLevels = R32(a + 20);
    uint32_t a_Adapter = g_Adapter;
    uint32_t a_DeviceType = g_DeviceType;
    uint32_t a_SurfaceFormat = g_SurfaceFormat;
    uint32_t a_Windowed = g_Windowed;
    uint32_t a_MultiSampleType = g_MultiSampleType;
    void *a_pQualityLevels = d3d9_out(c, g_pQualityLevels, (uint64_t)(4));
    int32_t hr = ((int32_t (*)(void *, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, void *))D3D9_VTBL(self)[11])(self, a_Adapter, a_DeviceType, a_SurfaceFormat, a_Windowed, a_MultiSampleType, a_pQualityLevels);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3D9_CheckDeviceType(CPU *c, void *self, uint32_t a) {
    uint32_t g_Adapter = R32(a + 0);
    uint32_t g_DevType = R32(a + 4);
    uint32_t g_AdapterFormat = R32(a + 8);
    uint32_t g_BackBufferFormat = R32(a + 12);
    uint32_t g_bWindowed = R32(a + 16);
    uint32_t a_Adapter = g_Adapter;
    uint32_t a_DevType = g_DevType;
    uint32_t a_AdapterFormat = g_AdapterFormat;
    uint32_t a_BackBufferFormat = g_BackBufferFormat;
    uint32_t a_bWindowed = g_bWindowed;
    int32_t hr = ((int32_t (*)(void *, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t))D3D9_VTBL(self)[9])(self, a_Adapter, a_DevType, a_AdapterFormat, a_BackBufferFormat, a_bWindowed);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3D9_CreateDevice(CPU *c, void *self, uint32_t a) {
    uint32_t g_Adapter = R32(a + 0);
    uint32_t g_DeviceType = R32(a + 4);
    uint32_t g_hFocusWindow = R32(a + 8);
    uint32_t g_BehaviorFlags = R32(a + 12);
    uint32_t g_pPresentationParameters = R32(a + 16);
    uint32_t g_ppReturnedDeviceInterface = R32(a + 20);
    uint32_t a_Adapter = g_Adapter;
    uint32_t a_DeviceType = g_DeviceType;
    uint32_t a_BehaviorFlags = g_BehaviorFlags;
    void *a_hFocusWindow = d3d9_hwnd(c, g_hFocusWindow);
    D3d9HostParams h_pPresentationParameters; void *a_pPresentationParameters = d3d9_params_in(c, g_pPresentationParameters, &h_pPresentationParameters, 1);
    void *h_ppReturnedDeviceInterface = NULL, **a_ppReturnedDeviceInterface = d3d9_out_slot(c, g_ppReturnedDeviceInterface) ? &h_ppReturnedDeviceInterface : NULL;
    int32_t hr = ((int32_t (*)(void *, uint32_t, uint32_t, void *, uint32_t, void *, void **))D3D9_VTBL(self)[16])(self, a_Adapter, a_DeviceType, a_hFocusWindow, a_BehaviorFlags, a_pPresentationParameters, a_ppReturnedDeviceInterface);
    if (hr >= 0) d3d9_params_out(c, g_pPresentationParameters, &h_pPresentationParameters);
    d3d9_iface_out(c, g_ppReturnedDeviceInterface, h_ppReturnedDeviceInterface, D3D9_IDirect3DDevice9);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3D9_EnumAdapterModes(CPU *c, void *self, uint32_t a) {
    uint32_t g_Adapter = R32(a + 0);
    uint32_t g_Format = R32(a + 4);
    uint32_t g_Mode = R32(a + 8);
    uint32_t g_pMode = R32(a + 12);
    uint32_t a_Adapter = g_Adapter;
    uint32_t a_Format = g_Format;
    uint32_t a_Mode = g_Mode;
    void *a_pMode = d3d9_out(c, g_pMode, (uint64_t)(16));
    int32_t hr = ((int32_t (*)(void *, uint32_t, uint32_t, uint32_t, void *))D3D9_VTBL(self)[7])(self, a_Adapter, a_Format, a_Mode, a_pMode);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3D9_GetAdapterCount(CPU *c, void *self, uint32_t a) {
    c->eax = ((uint32_t (*)(void *))D3D9_VTBL(self)[4])(self);
    (void)a;
}

static void t_IDirect3D9_GetAdapterDisplayMode(CPU *c, void *self, uint32_t a) {
    uint32_t g_Adapter = R32(a + 0);
    uint32_t g_pMode = R32(a + 4);
    uint32_t a_Adapter = g_Adapter;
    void *a_pMode = d3d9_out(c, g_pMode, (uint64_t)(16));
    int32_t hr = ((int32_t (*)(void *, uint32_t, void *))D3D9_VTBL(self)[8])(self, a_Adapter, a_pMode);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3D9_GetAdapterIdentifier(CPU *c, void *self, uint32_t a) {
    uint32_t g_Adapter = R32(a + 0);
    uint32_t g_Flags = R32(a + 4);
    uint32_t g_pIdentifier = R32(a + 8);
    uint32_t a_Adapter = g_Adapter;
    uint32_t a_Flags = g_Flags;
    D3d9HostAdapterId h_pIdentifier; void *a_pIdentifier = d3d9_adapter_id_in(c, g_pIdentifier, &h_pIdentifier);
    int32_t hr = ((int32_t (*)(void *, uint32_t, uint32_t, void *))D3D9_VTBL(self)[5])(self, a_Adapter, a_Flags, a_pIdentifier);
    if (hr >= 0) d3d9_adapter_id_out(c, g_pIdentifier, &h_pIdentifier);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3D9_GetAdapterModeCount(CPU *c, void *self, uint32_t a) {
    uint32_t g_Adapter = R32(a + 0);
    uint32_t g_Format = R32(a + 4);
    uint32_t a_Adapter = g_Adapter;
    uint32_t a_Format = g_Format;
    c->eax = ((uint32_t (*)(void *, uint32_t, uint32_t))D3D9_VTBL(self)[6])(self, a_Adapter, a_Format);
}

static void t_IDirect3D9_GetDeviceCaps(CPU *c, void *self, uint32_t a) {
    uint32_t g_Adapter = R32(a + 0);
    uint32_t g_DeviceType = R32(a + 4);
    uint32_t g_pCaps = R32(a + 8);
    uint32_t a_Adapter = g_Adapter;
    uint32_t a_DeviceType = g_DeviceType;
    void *a_pCaps = d3d9_out(c, g_pCaps, (uint64_t)(304));
    int32_t hr = ((int32_t (*)(void *, uint32_t, uint32_t, void *))D3D9_VTBL(self)[14])(self, a_Adapter, a_DeviceType, a_pCaps);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DBaseTexture9_GenerateMipSubLevels(CPU *c, void *self, uint32_t a) {
    ((void (*)(void *))D3D9_VTBL(self)[16])(self);
    (void)c; (void)a;
}

static void t_IDirect3DBaseTexture9_GetAutoGenFilterType(CPU *c, void *self, uint32_t a) {
    c->eax = ((uint32_t (*)(void *))D3D9_VTBL(self)[15])(self);
    (void)a;
}

static void t_IDirect3DBaseTexture9_GetLOD(CPU *c, void *self, uint32_t a) {
    c->eax = ((uint32_t (*)(void *))D3D9_VTBL(self)[12])(self);
    (void)a;
}

static void t_IDirect3DBaseTexture9_GetLevelCount(CPU *c, void *self, uint32_t a) {
    c->eax = ((uint32_t (*)(void *))D3D9_VTBL(self)[13])(self);
    (void)a;
}

static void t_IDirect3DBaseTexture9_SetAutoGenFilterType(CPU *c, void *self, uint32_t a) {
    uint32_t g_FilterType = R32(a + 0);
    uint32_t a_FilterType = g_FilterType;
    int32_t hr = ((int32_t (*)(void *, uint32_t))D3D9_VTBL(self)[14])(self, a_FilterType);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DBaseTexture9_SetLOD(CPU *c, void *self, uint32_t a) {
    uint32_t g_LODNew = R32(a + 0);
    uint32_t a_LODNew = g_LODNew;
    c->eax = ((uint32_t (*)(void *, uint32_t))D3D9_VTBL(self)[11])(self, a_LODNew);
}

static void t_IDirect3DCubeTexture9_AddDirtyRect(CPU *c, void *self, uint32_t a) {
    uint32_t g_FaceType = R32(a + 0);
    uint32_t g_pDirtyRect = R32(a + 4);
    uint32_t a_FaceType = g_FaceType;
    void *a_pDirtyRect = d3d9_in(c, g_pDirtyRect, (uint64_t)(16));
    int32_t hr = ((int32_t (*)(void *, uint32_t, void *))D3D9_VTBL(self)[21])(self, a_FaceType, a_pDirtyRect);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DCubeTexture9_GetCubeMapSurface(CPU *c, void *self, uint32_t a) {
    uint32_t g_FaceType = R32(a + 0);
    uint32_t g_Level = R32(a + 4);
    uint32_t g_ppCubeMapSurface = R32(a + 8);
    uint32_t a_FaceType = g_FaceType;
    uint32_t a_Level = g_Level;
    void *h_ppCubeMapSurface = NULL, **a_ppCubeMapSurface = d3d9_out_slot(c, g_ppCubeMapSurface) ? &h_ppCubeMapSurface : NULL;
    int32_t hr = ((int32_t (*)(void *, uint32_t, uint32_t, void **))D3D9_VTBL(self)[18])(self, a_FaceType, a_Level, a_ppCubeMapSurface);
    d3d9_child_out(c, g_ppCubeMapSurface, h_ppCubeMapSurface, D3D9_IDirect3DSurface9, self, 1);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DCubeTexture9_GetLevelDesc(CPU *c, void *self, uint32_t a) {
    uint32_t g_Level = R32(a + 0);
    uint32_t g_pDesc = R32(a + 4);
    uint32_t a_Level = g_Level;
    void *a_pDesc = d3d9_out(c, g_pDesc, (uint64_t)(32));
    int32_t hr = ((int32_t (*)(void *, uint32_t, void *))D3D9_VTBL(self)[17])(self, a_Level, a_pDesc);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DCubeTexture9_LockRect(CPU *c, void *self, uint32_t a) {
    uint32_t g_FaceType = R32(a + 0);
    uint32_t g_Level = R32(a + 4);
    uint32_t g_pLockedRect = R32(a + 8);
    uint32_t g_pRect = R32(a + 12);
    uint32_t g_Flags = R32(a + 16);
    uint32_t a_FaceType = g_FaceType;
    uint32_t a_Level = g_Level;
    uint32_t a_Flags = g_Flags;
    D3d9HostLocked h_pLockedRect; void *a_pLockedRect = d3d9_locked_in(c, g_pLockedRect, &h_pLockedRect, 8);
    void *a_pRect = d3d9_in(c, g_pRect, (uint64_t)(16));
    int32_t hr = ((int32_t (*)(void *, uint32_t, uint32_t, void *, void *, uint32_t))D3D9_VTBL(self)[19])(self, a_FaceType, a_Level, a_pLockedRect, a_pRect, a_Flags);
    d3d9_lockrect_out(c, g_pLockedRect, &h_pLockedRect, hr >= 0);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DCubeTexture9_UnlockRect(CPU *c, void *self, uint32_t a) {
    uint32_t g_FaceType = R32(a + 0);
    uint32_t g_Level = R32(a + 4);
    uint32_t a_FaceType = g_FaceType;
    uint32_t a_Level = g_Level;
    int32_t hr = ((int32_t (*)(void *, uint32_t, uint32_t))D3D9_VTBL(self)[20])(self, a_FaceType, a_Level);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_BeginScene(CPU *c, void *self, uint32_t a) {
    int32_t hr = ((int32_t (*)(void *))D3D9_VTBL(self)[41])(self);
    c->eax = (uint32_t)hr;
    (void)a;
}

static void t_IDirect3DDevice9_BeginStateBlock(CPU *c, void *self, uint32_t a) {
    int32_t hr = ((int32_t (*)(void *))D3D9_VTBL(self)[60])(self);
    c->eax = (uint32_t)hr;
    (void)a;
}

static void t_IDirect3DDevice9_Clear(CPU *c, void *self, uint32_t a) {
    uint32_t g_Count = R32(a + 0);
    uint32_t g_pRects = R32(a + 4);
    uint32_t g_Flags = R32(a + 8);
    uint32_t g_Color = R32(a + 12);
    uint32_t g_Z = R32(a + 16);
    uint32_t g_Stencil = R32(a + 20);
    uint32_t a_Count = g_Count;
    uint32_t a_Flags = g_Flags;
    uint32_t a_Color = g_Color;
    float a_Z = d3d9_f32(g_Z);
    uint32_t a_Stencil = g_Stencil;
    void *a_pRects = d3d9_in(c, g_pRects, (uint64_t)(a_Count*16));
    int32_t hr = ((int32_t (*)(void *, uint32_t, void *, uint32_t, uint32_t, float, uint32_t))D3D9_VTBL(self)[43])(self, a_Count, a_pRects, a_Flags, a_Color, a_Z, a_Stencil);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_ColorFill(CPU *c, void *self, uint32_t a) {
    uint32_t g_pSurface = R32(a + 0);
    uint32_t g_pRect = R32(a + 4);
    uint32_t g_color = R32(a + 8);
    uint32_t a_color = g_color;
    void *a_pSurface = d3d9_iface_in(c, g_pSurface, 0x800u);
    void *a_pRect = d3d9_in(c, g_pRect, (uint64_t)(16));
    int32_t hr = ((int32_t (*)(void *, void *, void *, uint32_t))D3D9_VTBL(self)[35])(self, a_pSurface, a_pRect, a_color);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_CreateAdditionalSwapChain(CPU *c, void *self, uint32_t a) {
    uint32_t g_pPresentationParameters = R32(a + 0);
    uint32_t g_pSwapChain = R32(a + 4);
    D3d9HostParams h_pPresentationParameters; void *a_pPresentationParameters = d3d9_params_in(c, g_pPresentationParameters, &h_pPresentationParameters, 1);
    void *h_pSwapChain = NULL, **a_pSwapChain = d3d9_out_slot(c, g_pSwapChain) ? &h_pSwapChain : NULL;
    int32_t hr = ((int32_t (*)(void *, void *, void **))D3D9_VTBL(self)[13])(self, a_pPresentationParameters, a_pSwapChain);
    if (hr >= 0) d3d9_params_out(c, g_pPresentationParameters, &h_pPresentationParameters);
    d3d9_iface_out(c, g_pSwapChain, h_pSwapChain, D3D9_IDirect3DSwapChain9);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_CreateCubeTexture(CPU *c, void *self, uint32_t a) {
    uint32_t g_EdgeLength = R32(a + 0);
    uint32_t g_Levels = R32(a + 4);
    uint32_t g_Usage = R32(a + 8);
    uint32_t g_Format = R32(a + 12);
    uint32_t g_Pool = R32(a + 16);
    uint32_t g_ppCubeTexture = R32(a + 20);
    uint32_t g_pSharedHandle = R32(a + 24);
    uint32_t a_EdgeLength = g_EdgeLength;
    uint32_t a_Levels = g_Levels;
    uint32_t a_Usage = g_Usage;
    uint32_t a_Format = g_Format;
    uint32_t a_Pool = g_Pool;
    void *h_ppCubeTexture = NULL, **a_ppCubeTexture = d3d9_out_slot(c, g_ppCubeTexture) ? &h_ppCubeTexture : NULL;
    void *a_pSharedHandle = d3d9_shared(c, g_pSharedHandle);
    int32_t hr = ((int32_t (*)(void *, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, void **, void *))D3D9_VTBL(self)[25])(self, a_EdgeLength, a_Levels, a_Usage, a_Format, a_Pool, a_ppCubeTexture, a_pSharedHandle);
    d3d9_iface_out(c, g_ppCubeTexture, h_ppCubeTexture, D3D9_IDirect3DCubeTexture9);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_CreateDepthStencilSurface(CPU *c, void *self, uint32_t a) {
    uint32_t g_Width = R32(a + 0);
    uint32_t g_Height = R32(a + 4);
    uint32_t g_Format = R32(a + 8);
    uint32_t g_MultiSample = R32(a + 12);
    uint32_t g_MultisampleQuality = R32(a + 16);
    uint32_t g_Discard = R32(a + 20);
    uint32_t g_ppSurface = R32(a + 24);
    uint32_t g_pSharedHandle = R32(a + 28);
    uint32_t a_Width = g_Width;
    uint32_t a_Height = g_Height;
    uint32_t a_Format = g_Format;
    uint32_t a_MultiSample = g_MultiSample;
    uint32_t a_MultisampleQuality = g_MultisampleQuality;
    uint32_t a_Discard = g_Discard;
    void *h_ppSurface = NULL, **a_ppSurface = d3d9_out_slot(c, g_ppSurface) ? &h_ppSurface : NULL;
    void *a_pSharedHandle = d3d9_shared(c, g_pSharedHandle);
    int32_t hr = ((int32_t (*)(void *, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, void **, void *))D3D9_VTBL(self)[29])(self, a_Width, a_Height, a_Format, a_MultiSample, a_MultisampleQuality, a_Discard, a_ppSurface, a_pSharedHandle);
    d3d9_iface_out(c, g_ppSurface, h_ppSurface, D3D9_IDirect3DSurface9);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_CreateIndexBuffer(CPU *c, void *self, uint32_t a) {
    uint32_t g_Length = R32(a + 0);
    uint32_t g_Usage = R32(a + 4);
    uint32_t g_Format = R32(a + 8);
    uint32_t g_Pool = R32(a + 12);
    uint32_t g_ppIndexBuffer = R32(a + 16);
    uint32_t g_pSharedHandle = R32(a + 20);
    uint32_t a_Length = g_Length;
    uint32_t a_Usage = g_Usage;
    uint32_t a_Format = g_Format;
    uint32_t a_Pool = g_Pool;
    void *h_ppIndexBuffer = NULL, **a_ppIndexBuffer = d3d9_out_slot(c, g_ppIndexBuffer) ? &h_ppIndexBuffer : NULL;
    void *a_pSharedHandle = d3d9_shared(c, g_pSharedHandle);
    int32_t hr = ((int32_t (*)(void *, uint32_t, uint32_t, uint32_t, uint32_t, void **, void *))D3D9_VTBL(self)[27])(self, a_Length, a_Usage, a_Format, a_Pool, a_ppIndexBuffer, a_pSharedHandle);
    d3d9_iface_out(c, g_ppIndexBuffer, h_ppIndexBuffer, D3D9_IDirect3DIndexBuffer9);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_CreateOffscreenPlainSurface(CPU *c, void *self, uint32_t a) {
    uint32_t g_Width = R32(a + 0);
    uint32_t g_Height = R32(a + 4);
    uint32_t g_Format = R32(a + 8);
    uint32_t g_Pool = R32(a + 12);
    uint32_t g_ppSurface = R32(a + 16);
    uint32_t g_pSharedHandle = R32(a + 20);
    uint32_t a_Width = g_Width;
    uint32_t a_Height = g_Height;
    uint32_t a_Format = g_Format;
    uint32_t a_Pool = g_Pool;
    void *h_ppSurface = NULL, **a_ppSurface = d3d9_out_slot(c, g_ppSurface) ? &h_ppSurface : NULL;
    void *a_pSharedHandle = d3d9_shared(c, g_pSharedHandle);
    int32_t hr = ((int32_t (*)(void *, uint32_t, uint32_t, uint32_t, uint32_t, void **, void *))D3D9_VTBL(self)[36])(self, a_Width, a_Height, a_Format, a_Pool, a_ppSurface, a_pSharedHandle);
    d3d9_iface_out(c, g_ppSurface, h_ppSurface, D3D9_IDirect3DSurface9);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_CreatePixelShader(CPU *c, void *self, uint32_t a) {
    uint32_t g_pFunction = R32(a + 0);
    uint32_t g_ppShader = R32(a + 4);
    const void *a_pFunction = d3d9_shader(c, g_pFunction);
    void *h_ppShader = NULL, **a_ppShader = d3d9_out_slot(c, g_ppShader) ? &h_ppShader : NULL;
    int32_t hr = ((int32_t (*)(void *, const void *, void **))D3D9_VTBL(self)[106])(self, a_pFunction, a_ppShader);
    d3d9_iface_out(c, g_ppShader, h_ppShader, D3D9_IDirect3DPixelShader9);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_CreateQuery(CPU *c, void *self, uint32_t a) {
    uint32_t g_Type = R32(a + 0);
    uint32_t g_ppQuery = R32(a + 4);
    uint32_t a_Type = g_Type;
    void *h_ppQuery = NULL, **a_ppQuery = d3d9_out_slot(c, g_ppQuery) ? &h_ppQuery : NULL;
    int32_t hr = ((int32_t (*)(void *, uint32_t, void **))D3D9_VTBL(self)[118])(self, a_Type, a_ppQuery);
    d3d9_iface_out(c, g_ppQuery, h_ppQuery, D3D9_IDirect3DQuery9);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_CreateRenderTarget(CPU *c, void *self, uint32_t a) {
    uint32_t g_Width = R32(a + 0);
    uint32_t g_Height = R32(a + 4);
    uint32_t g_Format = R32(a + 8);
    uint32_t g_MultiSample = R32(a + 12);
    uint32_t g_MultisampleQuality = R32(a + 16);
    uint32_t g_Lockable = R32(a + 20);
    uint32_t g_ppSurface = R32(a + 24);
    uint32_t g_pSharedHandle = R32(a + 28);
    uint32_t a_Width = g_Width;
    uint32_t a_Height = g_Height;
    uint32_t a_Format = g_Format;
    uint32_t a_MultiSample = g_MultiSample;
    uint32_t a_MultisampleQuality = g_MultisampleQuality;
    uint32_t a_Lockable = g_Lockable;
    void *h_ppSurface = NULL, **a_ppSurface = d3d9_out_slot(c, g_ppSurface) ? &h_ppSurface : NULL;
    void *a_pSharedHandle = d3d9_shared(c, g_pSharedHandle);
    int32_t hr = ((int32_t (*)(void *, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, void **, void *))D3D9_VTBL(self)[28])(self, a_Width, a_Height, a_Format, a_MultiSample, a_MultisampleQuality, a_Lockable, a_ppSurface, a_pSharedHandle);
    d3d9_iface_out(c, g_ppSurface, h_ppSurface, D3D9_IDirect3DSurface9);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_CreateStateBlock(CPU *c, void *self, uint32_t a) {
    uint32_t g_Type = R32(a + 0);
    uint32_t g_ppSB = R32(a + 4);
    uint32_t a_Type = g_Type;
    void *h_ppSB = NULL, **a_ppSB = d3d9_out_slot(c, g_ppSB) ? &h_ppSB : NULL;
    int32_t hr = ((int32_t (*)(void *, uint32_t, void **))D3D9_VTBL(self)[59])(self, a_Type, a_ppSB);
    d3d9_iface_out(c, g_ppSB, h_ppSB, D3D9_IDirect3DStateBlock9);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_CreateTexture(CPU *c, void *self, uint32_t a) {
    uint32_t g_Width = R32(a + 0);
    uint32_t g_Height = R32(a + 4);
    uint32_t g_Levels = R32(a + 8);
    uint32_t g_Usage = R32(a + 12);
    uint32_t g_Format = R32(a + 16);
    uint32_t g_Pool = R32(a + 20);
    uint32_t g_ppTexture = R32(a + 24);
    uint32_t g_pSharedHandle = R32(a + 28);
    uint32_t a_Width = g_Width;
    uint32_t a_Height = g_Height;
    uint32_t a_Levels = g_Levels;
    uint32_t a_Usage = g_Usage;
    uint32_t a_Format = g_Format;
    uint32_t a_Pool = g_Pool;
    void *h_ppTexture = NULL, **a_ppTexture = d3d9_out_slot(c, g_ppTexture) ? &h_ppTexture : NULL;
    void *a_pSharedHandle = d3d9_shared(c, g_pSharedHandle);
    int32_t hr = ((int32_t (*)(void *, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, void **, void *))D3D9_VTBL(self)[23])(self, a_Width, a_Height, a_Levels, a_Usage, a_Format, a_Pool, a_ppTexture, a_pSharedHandle);
    d3d9_iface_out(c, g_ppTexture, h_ppTexture, D3D9_IDirect3DTexture9);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_CreateVertexBuffer(CPU *c, void *self, uint32_t a) {
    uint32_t g_Length = R32(a + 0);
    uint32_t g_Usage = R32(a + 4);
    uint32_t g_FVF = R32(a + 8);
    uint32_t g_Pool = R32(a + 12);
    uint32_t g_ppVertexBuffer = R32(a + 16);
    uint32_t g_pSharedHandle = R32(a + 20);
    uint32_t a_Length = g_Length;
    uint32_t a_Usage = g_Usage;
    uint32_t a_FVF = g_FVF;
    uint32_t a_Pool = g_Pool;
    void *h_ppVertexBuffer = NULL, **a_ppVertexBuffer = d3d9_out_slot(c, g_ppVertexBuffer) ? &h_ppVertexBuffer : NULL;
    void *a_pSharedHandle = d3d9_shared(c, g_pSharedHandle);
    int32_t hr = ((int32_t (*)(void *, uint32_t, uint32_t, uint32_t, uint32_t, void **, void *))D3D9_VTBL(self)[26])(self, a_Length, a_Usage, a_FVF, a_Pool, a_ppVertexBuffer, a_pSharedHandle);
    d3d9_iface_out(c, g_ppVertexBuffer, h_ppVertexBuffer, D3D9_IDirect3DVertexBuffer9);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_CreateVertexDeclaration(CPU *c, void *self, uint32_t a) {
    uint32_t g_pVertexElements = R32(a + 0);
    uint32_t g_ppDecl = R32(a + 4);
    const void *a_pVertexElements = d3d9_decl(c, g_pVertexElements);
    void *h_ppDecl = NULL, **a_ppDecl = d3d9_out_slot(c, g_ppDecl) ? &h_ppDecl : NULL;
    int32_t hr = ((int32_t (*)(void *, const void *, void **))D3D9_VTBL(self)[86])(self, a_pVertexElements, a_ppDecl);
    d3d9_iface_out(c, g_ppDecl, h_ppDecl, D3D9_IDirect3DVertexDeclaration9);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_CreateVertexShader(CPU *c, void *self, uint32_t a) {
    uint32_t g_pFunction = R32(a + 0);
    uint32_t g_ppShader = R32(a + 4);
    const void *a_pFunction = d3d9_shader(c, g_pFunction);
    void *h_ppShader = NULL, **a_ppShader = d3d9_out_slot(c, g_ppShader) ? &h_ppShader : NULL;
    int32_t hr = ((int32_t (*)(void *, const void *, void **))D3D9_VTBL(self)[91])(self, a_pFunction, a_ppShader);
    d3d9_iface_out(c, g_ppShader, h_ppShader, D3D9_IDirect3DVertexShader9);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_CreateVolumeTexture(CPU *c, void *self, uint32_t a) {
    uint32_t g_Width = R32(a + 0);
    uint32_t g_Height = R32(a + 4);
    uint32_t g_Depth = R32(a + 8);
    uint32_t g_Levels = R32(a + 12);
    uint32_t g_Usage = R32(a + 16);
    uint32_t g_Format = R32(a + 20);
    uint32_t g_Pool = R32(a + 24);
    uint32_t g_ppVolumeTexture = R32(a + 28);
    uint32_t g_pSharedHandle = R32(a + 32);
    uint32_t a_Width = g_Width;
    uint32_t a_Height = g_Height;
    uint32_t a_Depth = g_Depth;
    uint32_t a_Levels = g_Levels;
    uint32_t a_Usage = g_Usage;
    uint32_t a_Format = g_Format;
    uint32_t a_Pool = g_Pool;
    void *h_ppVolumeTexture = NULL, **a_ppVolumeTexture = d3d9_out_slot(c, g_ppVolumeTexture) ? &h_ppVolumeTexture : NULL;
    void *a_pSharedHandle = d3d9_shared(c, g_pSharedHandle);
    int32_t hr = ((int32_t (*)(void *, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, void **, void *))D3D9_VTBL(self)[24])(self, a_Width, a_Height, a_Depth, a_Levels, a_Usage, a_Format, a_Pool, a_ppVolumeTexture, a_pSharedHandle);
    d3d9_iface_out(c, g_ppVolumeTexture, h_ppVolumeTexture, D3D9_IDirect3DVolumeTexture9);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_DeletePatch(CPU *c, void *self, uint32_t a) {
    uint32_t g_Handle = R32(a + 0);
    uint32_t a_Handle = g_Handle;
    int32_t hr = ((int32_t (*)(void *, uint32_t))D3D9_VTBL(self)[117])(self, a_Handle);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_DrawIndexedPrimitive(CPU *c, void *self, uint32_t a) {
    uint32_t g_PrimitiveType = R32(a + 0);
    uint32_t g_BaseVertexIndex = R32(a + 4);
    uint32_t g_MinVertexIndex = R32(a + 8);
    uint32_t g_NumVertices = R32(a + 12);
    uint32_t g_startIndex = R32(a + 16);
    uint32_t g_primCount = R32(a + 20);
    uint32_t a_PrimitiveType = g_PrimitiveType;
    int32_t a_BaseVertexIndex = (int32_t)g_BaseVertexIndex;
    uint32_t a_MinVertexIndex = g_MinVertexIndex;
    uint32_t a_NumVertices = g_NumVertices;
    uint32_t a_startIndex = g_startIndex;
    uint32_t a_primCount = g_primCount;
    int32_t hr = ((int32_t (*)(void *, uint32_t, int32_t, uint32_t, uint32_t, uint32_t, uint32_t))D3D9_VTBL(self)[82])(self, a_PrimitiveType, a_BaseVertexIndex, a_MinVertexIndex, a_NumVertices, a_startIndex, a_primCount);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_DrawIndexedPrimitiveUP(CPU *c, void *self, uint32_t a) {
    uint32_t g_PrimitiveType = R32(a + 0);
    uint32_t g_MinVertexIndex = R32(a + 4);
    uint32_t g_NumVertices = R32(a + 8);
    uint32_t g_PrimitiveCount = R32(a + 12);
    uint32_t g_pIndexData = R32(a + 16);
    uint32_t g_IndexDataFormat = R32(a + 20);
    uint32_t g_pVertexStreamZeroData = R32(a + 24);
    uint32_t g_VertexStreamZeroStride = R32(a + 28);
    uint32_t a_PrimitiveType = g_PrimitiveType;
    uint32_t a_MinVertexIndex = g_MinVertexIndex;
    uint32_t a_NumVertices = g_NumVertices;
    uint32_t a_PrimitiveCount = g_PrimitiveCount;
    uint32_t a_IndexDataFormat = g_IndexDataFormat;
    uint32_t a_VertexStreamZeroStride = g_VertexStreamZeroStride;
    void *a_pIndexData = d3d9_in(c, g_pIndexData, (uint64_t)((uint64_t)d3d9_vertex_count(a_PrimitiveType,a_PrimitiveCount)*d3d9_index_size(a_IndexDataFormat)));
    void *a_pVertexStreamZeroData = d3d9_in(c, g_pVertexStreamZeroData, (uint64_t)(((uint64_t)a_MinVertexIndex+a_NumVertices)*a_VertexStreamZeroStride));
    int32_t hr = ((int32_t (*)(void *, uint32_t, uint32_t, uint32_t, uint32_t, void *, uint32_t, void *, uint32_t))D3D9_VTBL(self)[84])(self, a_PrimitiveType, a_MinVertexIndex, a_NumVertices, a_PrimitiveCount, a_pIndexData, a_IndexDataFormat, a_pVertexStreamZeroData, a_VertexStreamZeroStride);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_DrawPrimitive(CPU *c, void *self, uint32_t a) {
    uint32_t g_PrimitiveType = R32(a + 0);
    uint32_t g_StartVertex = R32(a + 4);
    uint32_t g_PrimitiveCount = R32(a + 8);
    uint32_t a_PrimitiveType = g_PrimitiveType;
    uint32_t a_StartVertex = g_StartVertex;
    uint32_t a_PrimitiveCount = g_PrimitiveCount;
    int32_t hr = ((int32_t (*)(void *, uint32_t, uint32_t, uint32_t))D3D9_VTBL(self)[81])(self, a_PrimitiveType, a_StartVertex, a_PrimitiveCount);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_DrawPrimitiveUP(CPU *c, void *self, uint32_t a) {
    uint32_t g_PrimitiveType = R32(a + 0);
    uint32_t g_PrimitiveCount = R32(a + 4);
    uint32_t g_pVertexStreamZeroData = R32(a + 8);
    uint32_t g_VertexStreamZeroStride = R32(a + 12);
    uint32_t a_PrimitiveType = g_PrimitiveType;
    uint32_t a_PrimitiveCount = g_PrimitiveCount;
    uint32_t a_VertexStreamZeroStride = g_VertexStreamZeroStride;
    void *a_pVertexStreamZeroData = d3d9_in(c, g_pVertexStreamZeroData, (uint64_t)((uint64_t)d3d9_vertex_count(a_PrimitiveType,a_PrimitiveCount)*a_VertexStreamZeroStride));
    int32_t hr = ((int32_t (*)(void *, uint32_t, uint32_t, void *, uint32_t))D3D9_VTBL(self)[83])(self, a_PrimitiveType, a_PrimitiveCount, a_pVertexStreamZeroData, a_VertexStreamZeroStride);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_EndScene(CPU *c, void *self, uint32_t a) {
    int32_t hr = ((int32_t (*)(void *))D3D9_VTBL(self)[42])(self);
    c->eax = (uint32_t)hr;
    (void)a;
}

static void t_IDirect3DDevice9_EndStateBlock(CPU *c, void *self, uint32_t a) {
    uint32_t g_ppSB = R32(a + 0);
    void *h_ppSB = NULL, **a_ppSB = d3d9_out_slot(c, g_ppSB) ? &h_ppSB : NULL;
    int32_t hr = ((int32_t (*)(void *, void **))D3D9_VTBL(self)[61])(self, a_ppSB);
    d3d9_iface_out(c, g_ppSB, h_ppSB, D3D9_IDirect3DStateBlock9);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_EvictManagedResources(CPU *c, void *self, uint32_t a) {
    int32_t hr = ((int32_t (*)(void *))D3D9_VTBL(self)[5])(self);
    c->eax = (uint32_t)hr;
    (void)a;
}

static void t_IDirect3DDevice9_GetAvailableTextureMem(CPU *c, void *self, uint32_t a) {
    c->eax = ((uint32_t (*)(void *))D3D9_VTBL(self)[4])(self);
    (void)a;
}

static void t_IDirect3DDevice9_GetBackBuffer(CPU *c, void *self, uint32_t a) {
    uint32_t g_iSwapChain = R32(a + 0);
    uint32_t g_iBackBuffer = R32(a + 4);
    uint32_t g_Type = R32(a + 8);
    uint32_t g_ppBackBuffer = R32(a + 12);
    uint32_t a_iSwapChain = g_iSwapChain;
    uint32_t a_iBackBuffer = g_iBackBuffer;
    uint32_t a_Type = g_Type;
    void *h_ppBackBuffer = NULL, **a_ppBackBuffer = d3d9_out_slot(c, g_ppBackBuffer) ? &h_ppBackBuffer : NULL;
    int32_t hr = ((int32_t (*)(void *, uint32_t, uint32_t, uint32_t, void **))D3D9_VTBL(self)[18])(self, a_iSwapChain, a_iBackBuffer, a_Type, a_ppBackBuffer);
    d3d9_child_out(c, g_ppBackBuffer, h_ppBackBuffer, D3D9_IDirect3DSurface9, self, 0);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_GetClipPlane(CPU *c, void *self, uint32_t a) {
    uint32_t g_Index = R32(a + 0);
    uint32_t g_pPlane = R32(a + 4);
    uint32_t a_Index = g_Index;
    void *a_pPlane = d3d9_out(c, g_pPlane, (uint64_t)(16));
    int32_t hr = ((int32_t (*)(void *, uint32_t, void *))D3D9_VTBL(self)[56])(self, a_Index, a_pPlane);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_GetClipStatus(CPU *c, void *self, uint32_t a) {
    uint32_t g_pClipStatus = R32(a + 0);
    void *a_pClipStatus = d3d9_out(c, g_pClipStatus, (uint64_t)(8));
    int32_t hr = ((int32_t (*)(void *, void *))D3D9_VTBL(self)[63])(self, a_pClipStatus);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_GetCreationParameters(CPU *c, void *self, uint32_t a) {
    uint32_t g_pParameters = R32(a + 0);
    D3d9HostCreation h_pParameters; void *a_pParameters = d3d9_creation_in(c, g_pParameters, &h_pParameters);
    int32_t hr = ((int32_t (*)(void *, void *))D3D9_VTBL(self)[9])(self, a_pParameters);
    if (hr >= 0) d3d9_creation_out(c, g_pParameters, &h_pParameters);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_GetCurrentTexturePalette(CPU *c, void *self, uint32_t a) {
    uint32_t g_PaletteNumber = R32(a + 0);
    void *a_PaletteNumber = d3d9_out(c, g_PaletteNumber, (uint64_t)(4));
    int32_t hr = ((int32_t (*)(void *, void *))D3D9_VTBL(self)[74])(self, a_PaletteNumber);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_GetDepthStencilSurface(CPU *c, void *self, uint32_t a) {
    uint32_t g_ppZStencilSurface = R32(a + 0);
    void *h_ppZStencilSurface = NULL, **a_ppZStencilSurface = d3d9_out_slot(c, g_ppZStencilSurface) ? &h_ppZStencilSurface : NULL;
    int32_t hr = ((int32_t (*)(void *, void **))D3D9_VTBL(self)[40])(self, a_ppZStencilSurface);
    d3d9_iface_out(c, g_ppZStencilSurface, h_ppZStencilSurface, D3D9_IDirect3DSurface9);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_GetDeviceCaps(CPU *c, void *self, uint32_t a) {
    uint32_t g_pCaps = R32(a + 0);
    void *a_pCaps = d3d9_out(c, g_pCaps, (uint64_t)(304));
    int32_t hr = ((int32_t (*)(void *, void *))D3D9_VTBL(self)[7])(self, a_pCaps);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_GetDirect3D(CPU *c, void *self, uint32_t a) {
    uint32_t g_ppD3D9 = R32(a + 0);
    void *h_ppD3D9 = NULL, **a_ppD3D9 = d3d9_out_slot(c, g_ppD3D9) ? &h_ppD3D9 : NULL;
    int32_t hr = ((int32_t (*)(void *, void **))D3D9_VTBL(self)[6])(self, a_ppD3D9);
    d3d9_iface_out(c, g_ppD3D9, h_ppD3D9, D3D9_IDirect3D9);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_GetDisplayMode(CPU *c, void *self, uint32_t a) {
    uint32_t g_iSwapChain = R32(a + 0);
    uint32_t g_pMode = R32(a + 4);
    uint32_t a_iSwapChain = g_iSwapChain;
    void *a_pMode = d3d9_out(c, g_pMode, (uint64_t)(16));
    int32_t hr = ((int32_t (*)(void *, uint32_t, void *))D3D9_VTBL(self)[8])(self, a_iSwapChain, a_pMode);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_GetFVF(CPU *c, void *self, uint32_t a) {
    uint32_t g_pFVF = R32(a + 0);
    void *a_pFVF = d3d9_out(c, g_pFVF, (uint64_t)(4));
    int32_t hr = ((int32_t (*)(void *, void *))D3D9_VTBL(self)[90])(self, a_pFVF);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_GetFrontBufferData(CPU *c, void *self, uint32_t a) {
    uint32_t g_iSwapChain = R32(a + 0);
    uint32_t g_pDestSurface = R32(a + 4);
    uint32_t a_iSwapChain = g_iSwapChain;
    void *a_pDestSurface = d3d9_iface_in(c, g_pDestSurface, 0x800u);
    int32_t hr = ((int32_t (*)(void *, uint32_t, void *))D3D9_VTBL(self)[33])(self, a_iSwapChain, a_pDestSurface);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_GetGammaRamp(CPU *c, void *self, uint32_t a) {
    uint32_t g_iSwapChain = R32(a + 0);
    uint32_t g_pRamp = R32(a + 4);
    uint32_t a_iSwapChain = g_iSwapChain;
    void *a_pRamp = d3d9_out(c, g_pRamp, (uint64_t)(1536));
    ((void (*)(void *, uint32_t, void *))D3D9_VTBL(self)[22])(self, a_iSwapChain, a_pRamp);
    (void)c;
}

static void t_IDirect3DDevice9_GetIndices(CPU *c, void *self, uint32_t a) {
    uint32_t g_ppIndexData = R32(a + 0);
    void *h_ppIndexData = NULL, **a_ppIndexData = d3d9_out_slot(c, g_ppIndexData) ? &h_ppIndexData : NULL;
    int32_t hr = ((int32_t (*)(void *, void **))D3D9_VTBL(self)[105])(self, a_ppIndexData);
    d3d9_iface_out(c, g_ppIndexData, h_ppIndexData, D3D9_IDirect3DIndexBuffer9);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_GetLight(CPU *c, void *self, uint32_t a) {
    uint32_t g_Index = R32(a + 0);
    uint32_t g_pLight = R32(a + 4);
    uint32_t a_Index = g_Index;
    void *a_pLight = d3d9_out(c, g_pLight, (uint64_t)(104));
    int32_t hr = ((int32_t (*)(void *, uint32_t, void *))D3D9_VTBL(self)[52])(self, a_Index, a_pLight);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_GetLightEnable(CPU *c, void *self, uint32_t a) {
    uint32_t g_Index = R32(a + 0);
    uint32_t g_pEnable = R32(a + 4);
    uint32_t a_Index = g_Index;
    void *a_pEnable = d3d9_out(c, g_pEnable, (uint64_t)(4));
    int32_t hr = ((int32_t (*)(void *, uint32_t, void *))D3D9_VTBL(self)[54])(self, a_Index, a_pEnable);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_GetMaterial(CPU *c, void *self, uint32_t a) {
    uint32_t g_pMaterial = R32(a + 0);
    void *a_pMaterial = d3d9_out(c, g_pMaterial, (uint64_t)(68));
    int32_t hr = ((int32_t (*)(void *, void *))D3D9_VTBL(self)[50])(self, a_pMaterial);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_GetNPatchMode(CPU *c, void *self, uint32_t a) {
    rt_return_st0(c, ((float (*)(void *))D3D9_VTBL(self)[80])(self));
    (void)a;
}

static void t_IDirect3DDevice9_GetNumberOfSwapChains(CPU *c, void *self, uint32_t a) {
    c->eax = ((uint32_t (*)(void *))D3D9_VTBL(self)[15])(self);
    (void)a;
}

static void t_IDirect3DDevice9_GetPaletteEntries(CPU *c, void *self, uint32_t a) {
    uint32_t g_PaletteNumber = R32(a + 0);
    uint32_t g_pEntries = R32(a + 4);
    uint32_t a_PaletteNumber = g_PaletteNumber;
    void *a_pEntries = d3d9_out(c, g_pEntries, (uint64_t)(1024));
    int32_t hr = ((int32_t (*)(void *, uint32_t, void *))D3D9_VTBL(self)[72])(self, a_PaletteNumber, a_pEntries);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_GetPixelShader(CPU *c, void *self, uint32_t a) {
    uint32_t g_ppShader = R32(a + 0);
    void *h_ppShader = NULL, **a_ppShader = d3d9_out_slot(c, g_ppShader) ? &h_ppShader : NULL;
    int32_t hr = ((int32_t (*)(void *, void **))D3D9_VTBL(self)[108])(self, a_ppShader);
    d3d9_iface_out(c, g_ppShader, h_ppShader, D3D9_IDirect3DPixelShader9);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_GetPixelShaderConstantB(CPU *c, void *self, uint32_t a) {
    uint32_t g_StartRegister = R32(a + 0);
    uint32_t g_pConstantData = R32(a + 4);
    uint32_t g_BoolCount = R32(a + 8);
    uint32_t a_StartRegister = g_StartRegister;
    uint32_t a_BoolCount = g_BoolCount;
    void *a_pConstantData = d3d9_out(c, g_pConstantData, (uint64_t)((uint64_t)a_BoolCount*4));
    int32_t hr = ((int32_t (*)(void *, uint32_t, void *, uint32_t))D3D9_VTBL(self)[114])(self, a_StartRegister, a_pConstantData, a_BoolCount);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_GetPixelShaderConstantF(CPU *c, void *self, uint32_t a) {
    uint32_t g_StartRegister = R32(a + 0);
    uint32_t g_pConstantData = R32(a + 4);
    uint32_t g_Vector4fCount = R32(a + 8);
    uint32_t a_StartRegister = g_StartRegister;
    uint32_t a_Vector4fCount = g_Vector4fCount;
    void *a_pConstantData = d3d9_out(c, g_pConstantData, (uint64_t)((uint64_t)a_Vector4fCount*16));
    int32_t hr = ((int32_t (*)(void *, uint32_t, void *, uint32_t))D3D9_VTBL(self)[110])(self, a_StartRegister, a_pConstantData, a_Vector4fCount);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_GetPixelShaderConstantI(CPU *c, void *self, uint32_t a) {
    uint32_t g_StartRegister = R32(a + 0);
    uint32_t g_pConstantData = R32(a + 4);
    uint32_t g_Vector4iCount = R32(a + 8);
    uint32_t a_StartRegister = g_StartRegister;
    uint32_t a_Vector4iCount = g_Vector4iCount;
    void *a_pConstantData = d3d9_out(c, g_pConstantData, (uint64_t)((uint64_t)a_Vector4iCount*16));
    int32_t hr = ((int32_t (*)(void *, uint32_t, void *, uint32_t))D3D9_VTBL(self)[112])(self, a_StartRegister, a_pConstantData, a_Vector4iCount);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_GetRasterStatus(CPU *c, void *self, uint32_t a) {
    uint32_t g_iSwapChain = R32(a + 0);
    uint32_t g_pRasterStatus = R32(a + 4);
    uint32_t a_iSwapChain = g_iSwapChain;
    void *a_pRasterStatus = d3d9_out(c, g_pRasterStatus, (uint64_t)(8));
    int32_t hr = ((int32_t (*)(void *, uint32_t, void *))D3D9_VTBL(self)[19])(self, a_iSwapChain, a_pRasterStatus);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_GetRenderState(CPU *c, void *self, uint32_t a) {
    uint32_t g_State = R32(a + 0);
    uint32_t g_pValue = R32(a + 4);
    uint32_t a_State = g_State;
    void *a_pValue = d3d9_out(c, g_pValue, (uint64_t)(4));
    int32_t hr = ((int32_t (*)(void *, uint32_t, void *))D3D9_VTBL(self)[58])(self, a_State, a_pValue);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_GetRenderTarget(CPU *c, void *self, uint32_t a) {
    uint32_t g_RenderTargetIndex = R32(a + 0);
    uint32_t g_ppRenderTarget = R32(a + 4);
    uint32_t a_RenderTargetIndex = g_RenderTargetIndex;
    void *h_ppRenderTarget = NULL, **a_ppRenderTarget = d3d9_out_slot(c, g_ppRenderTarget) ? &h_ppRenderTarget : NULL;
    int32_t hr = ((int32_t (*)(void *, uint32_t, void **))D3D9_VTBL(self)[38])(self, a_RenderTargetIndex, a_ppRenderTarget);
    d3d9_iface_out(c, g_ppRenderTarget, h_ppRenderTarget, D3D9_IDirect3DSurface9);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_GetRenderTargetData(CPU *c, void *self, uint32_t a) {
    uint32_t g_pRenderTarget = R32(a + 0);
    uint32_t g_pDestSurface = R32(a + 4);
    void *a_pRenderTarget = d3d9_iface_in(c, g_pRenderTarget, 0x800u);
    void *a_pDestSurface = d3d9_iface_in(c, g_pDestSurface, 0x800u);
    int32_t hr = ((int32_t (*)(void *, void *, void *))D3D9_VTBL(self)[32])(self, a_pRenderTarget, a_pDestSurface);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_GetSamplerState(CPU *c, void *self, uint32_t a) {
    uint32_t g_Sampler = R32(a + 0);
    uint32_t g_Type = R32(a + 4);
    uint32_t g_pValue = R32(a + 8);
    uint32_t a_Sampler = g_Sampler;
    uint32_t a_Type = g_Type;
    void *a_pValue = d3d9_out(c, g_pValue, (uint64_t)(4));
    int32_t hr = ((int32_t (*)(void *, uint32_t, uint32_t, void *))D3D9_VTBL(self)[68])(self, a_Sampler, a_Type, a_pValue);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_GetScissorRect(CPU *c, void *self, uint32_t a) {
    uint32_t g_pRect = R32(a + 0);
    void *a_pRect = d3d9_out(c, g_pRect, (uint64_t)(16));
    int32_t hr = ((int32_t (*)(void *, void *))D3D9_VTBL(self)[76])(self, a_pRect);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_GetSoftwareVertexProcessing(CPU *c, void *self, uint32_t a) {
    c->eax = ((uint32_t (*)(void *))D3D9_VTBL(self)[78])(self);
    (void)a;
}

static void t_IDirect3DDevice9_GetStreamSource(CPU *c, void *self, uint32_t a) {
    uint32_t g_StreamNumber = R32(a + 0);
    uint32_t g_ppStreamData = R32(a + 4);
    uint32_t g_pOffsetInBytes = R32(a + 8);
    uint32_t g_pStride = R32(a + 12);
    uint32_t a_StreamNumber = g_StreamNumber;
    void *h_ppStreamData = NULL, **a_ppStreamData = d3d9_out_slot(c, g_ppStreamData) ? &h_ppStreamData : NULL;
    void *a_pOffsetInBytes = d3d9_out(c, g_pOffsetInBytes, (uint64_t)(4));
    void *a_pStride = d3d9_out(c, g_pStride, (uint64_t)(4));
    int32_t hr = ((int32_t (*)(void *, uint32_t, void **, void *, void *))D3D9_VTBL(self)[101])(self, a_StreamNumber, a_ppStreamData, a_pOffsetInBytes, a_pStride);
    d3d9_iface_out(c, g_ppStreamData, h_ppStreamData, D3D9_IDirect3DVertexBuffer9);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_GetStreamSourceFreq(CPU *c, void *self, uint32_t a) {
    uint32_t g_StreamNumber = R32(a + 0);
    uint32_t g_pSetting = R32(a + 4);
    uint32_t a_StreamNumber = g_StreamNumber;
    void *a_pSetting = d3d9_out(c, g_pSetting, (uint64_t)(4));
    int32_t hr = ((int32_t (*)(void *, uint32_t, void *))D3D9_VTBL(self)[103])(self, a_StreamNumber, a_pSetting);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_GetSwapChain(CPU *c, void *self, uint32_t a) {
    uint32_t g_iSwapChain = R32(a + 0);
    uint32_t g_pSwapChain = R32(a + 4);
    uint32_t a_iSwapChain = g_iSwapChain;
    void *h_pSwapChain = NULL, **a_pSwapChain = d3d9_out_slot(c, g_pSwapChain) ? &h_pSwapChain : NULL;
    int32_t hr = ((int32_t (*)(void *, uint32_t, void **))D3D9_VTBL(self)[14])(self, a_iSwapChain, a_pSwapChain);
    d3d9_iface_out(c, g_pSwapChain, h_pSwapChain, D3D9_IDirect3DSwapChain9);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_GetTexture(CPU *c, void *self, uint32_t a) {
    uint32_t g_Stage = R32(a + 0);
    uint32_t g_ppTexture = R32(a + 4);
    uint32_t a_Stage = g_Stage;
    void *h_ppTexture = NULL, **a_ppTexture = d3d9_out_slot(c, g_ppTexture) ? &h_ppTexture : NULL;
    int32_t hr = ((int32_t (*)(void *, uint32_t, void **))D3D9_VTBL(self)[64])(self, a_Stage, a_ppTexture);
    d3d9_iface_out(c, g_ppTexture, h_ppTexture, D3D9_IDirect3DBaseTexture9);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_GetTextureStageState(CPU *c, void *self, uint32_t a) {
    uint32_t g_Stage = R32(a + 0);
    uint32_t g_Type = R32(a + 4);
    uint32_t g_pValue = R32(a + 8);
    uint32_t a_Stage = g_Stage;
    uint32_t a_Type = g_Type;
    void *a_pValue = d3d9_out(c, g_pValue, (uint64_t)(4));
    int32_t hr = ((int32_t (*)(void *, uint32_t, uint32_t, void *))D3D9_VTBL(self)[66])(self, a_Stage, a_Type, a_pValue);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_GetTransform(CPU *c, void *self, uint32_t a) {
    uint32_t g_State = R32(a + 0);
    uint32_t g_pMatrix = R32(a + 4);
    uint32_t a_State = g_State;
    void *a_pMatrix = d3d9_out(c, g_pMatrix, (uint64_t)(64));
    int32_t hr = ((int32_t (*)(void *, uint32_t, void *))D3D9_VTBL(self)[45])(self, a_State, a_pMatrix);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_GetVertexDeclaration(CPU *c, void *self, uint32_t a) {
    uint32_t g_ppDecl = R32(a + 0);
    void *h_ppDecl = NULL, **a_ppDecl = d3d9_out_slot(c, g_ppDecl) ? &h_ppDecl : NULL;
    int32_t hr = ((int32_t (*)(void *, void **))D3D9_VTBL(self)[88])(self, a_ppDecl);
    d3d9_iface_out(c, g_ppDecl, h_ppDecl, D3D9_IDirect3DVertexDeclaration9);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_GetVertexShader(CPU *c, void *self, uint32_t a) {
    uint32_t g_ppShader = R32(a + 0);
    void *h_ppShader = NULL, **a_ppShader = d3d9_out_slot(c, g_ppShader) ? &h_ppShader : NULL;
    int32_t hr = ((int32_t (*)(void *, void **))D3D9_VTBL(self)[93])(self, a_ppShader);
    d3d9_iface_out(c, g_ppShader, h_ppShader, D3D9_IDirect3DVertexShader9);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_GetVertexShaderConstantB(CPU *c, void *self, uint32_t a) {
    uint32_t g_StartRegister = R32(a + 0);
    uint32_t g_pConstantData = R32(a + 4);
    uint32_t g_BoolCount = R32(a + 8);
    uint32_t a_StartRegister = g_StartRegister;
    uint32_t a_BoolCount = g_BoolCount;
    void *a_pConstantData = d3d9_out(c, g_pConstantData, (uint64_t)((uint64_t)a_BoolCount*4));
    int32_t hr = ((int32_t (*)(void *, uint32_t, void *, uint32_t))D3D9_VTBL(self)[99])(self, a_StartRegister, a_pConstantData, a_BoolCount);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_GetVertexShaderConstantF(CPU *c, void *self, uint32_t a) {
    uint32_t g_StartRegister = R32(a + 0);
    uint32_t g_pConstantData = R32(a + 4);
    uint32_t g_Vector4fCount = R32(a + 8);
    uint32_t a_StartRegister = g_StartRegister;
    uint32_t a_Vector4fCount = g_Vector4fCount;
    void *a_pConstantData = d3d9_out(c, g_pConstantData, (uint64_t)((uint64_t)a_Vector4fCount*16));
    int32_t hr = ((int32_t (*)(void *, uint32_t, void *, uint32_t))D3D9_VTBL(self)[95])(self, a_StartRegister, a_pConstantData, a_Vector4fCount);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_GetVertexShaderConstantI(CPU *c, void *self, uint32_t a) {
    uint32_t g_StartRegister = R32(a + 0);
    uint32_t g_pConstantData = R32(a + 4);
    uint32_t g_Vector4iCount = R32(a + 8);
    uint32_t a_StartRegister = g_StartRegister;
    uint32_t a_Vector4iCount = g_Vector4iCount;
    void *a_pConstantData = d3d9_out(c, g_pConstantData, (uint64_t)((uint64_t)a_Vector4iCount*16));
    int32_t hr = ((int32_t (*)(void *, uint32_t, void *, uint32_t))D3D9_VTBL(self)[97])(self, a_StartRegister, a_pConstantData, a_Vector4iCount);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_GetViewport(CPU *c, void *self, uint32_t a) {
    uint32_t g_pViewport = R32(a + 0);
    void *a_pViewport = d3d9_out(c, g_pViewport, (uint64_t)(24));
    int32_t hr = ((int32_t (*)(void *, void *))D3D9_VTBL(self)[48])(self, a_pViewport);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_LightEnable(CPU *c, void *self, uint32_t a) {
    uint32_t g_Index = R32(a + 0);
    uint32_t g_Enable = R32(a + 4);
    uint32_t a_Index = g_Index;
    uint32_t a_Enable = g_Enable;
    int32_t hr = ((int32_t (*)(void *, uint32_t, uint32_t))D3D9_VTBL(self)[53])(self, a_Index, a_Enable);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_MultiplyTransform(CPU *c, void *self, uint32_t a) {
    uint32_t g_State = R32(a + 0);
    uint32_t g_pMatrix = R32(a + 4);
    uint32_t a_State = g_State;
    void *a_pMatrix = d3d9_in(c, g_pMatrix, (uint64_t)(64));
    int32_t hr = ((int32_t (*)(void *, uint32_t, void *))D3D9_VTBL(self)[46])(self, a_State, a_pMatrix);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_Present(CPU *c, void *self, uint32_t a) {
    uint32_t g_pSourceRect = R32(a + 0);
    uint32_t g_pDestRect = R32(a + 4);
    uint32_t g_hDestWindowOverride = R32(a + 8);
    uint32_t g_pDirtyRegion = R32(a + 12);
    void *a_pSourceRect = d3d9_in(c, g_pSourceRect, (uint64_t)(16));
    void *a_pDestRect = d3d9_in(c, g_pDestRect, (uint64_t)(16));
    void *a_hDestWindowOverride = d3d9_hwnd(c, g_hDestWindowOverride);
    const void *a_pDirtyRegion = d3d9_rgndata(c, g_pDirtyRegion);
    int32_t hr = ((int32_t (*)(void *, void *, void *, void *, const void *))D3D9_VTBL(self)[17])(self, a_pSourceRect, a_pDestRect, a_hDestWindowOverride, a_pDirtyRegion);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_ProcessVertices(CPU *c, void *self, uint32_t a) {
    uint32_t g_SrcStartIndex = R32(a + 0);
    uint32_t g_DestIndex = R32(a + 4);
    uint32_t g_VertexCount = R32(a + 8);
    uint32_t g_pDestBuffer = R32(a + 12);
    uint32_t g_pVertexDecl = R32(a + 16);
    uint32_t g_Flags = R32(a + 20);
    uint32_t a_SrcStartIndex = g_SrcStartIndex;
    uint32_t a_DestIndex = g_DestIndex;
    uint32_t a_VertexCount = g_VertexCount;
    uint32_t a_Flags = g_Flags;
    void *a_pDestBuffer = d3d9_iface_in(c, g_pDestBuffer, 0x200u);
    void *a_pVertexDecl = d3d9_iface_in(c, g_pVertexDecl, 0x2000u);
    int32_t hr = ((int32_t (*)(void *, uint32_t, uint32_t, uint32_t, void *, void *, uint32_t))D3D9_VTBL(self)[85])(self, a_SrcStartIndex, a_DestIndex, a_VertexCount, a_pDestBuffer, a_pVertexDecl, a_Flags);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_Reset(CPU *c, void *self, uint32_t a) {
    uint32_t g_pPresentationParameters = R32(a + 0);
    D3d9HostParams h_pPresentationParameters; void *a_pPresentationParameters = d3d9_params_in(c, g_pPresentationParameters, &h_pPresentationParameters, 1);
    int32_t hr = ((int32_t (*)(void *, void *))D3D9_VTBL(self)[16])(self, a_pPresentationParameters);
    if (hr >= 0) d3d9_params_out(c, g_pPresentationParameters, &h_pPresentationParameters);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_SetClipPlane(CPU *c, void *self, uint32_t a) {
    uint32_t g_Index = R32(a + 0);
    uint32_t g_pPlane = R32(a + 4);
    uint32_t a_Index = g_Index;
    void *a_pPlane = d3d9_in(c, g_pPlane, (uint64_t)(16));
    int32_t hr = ((int32_t (*)(void *, uint32_t, void *))D3D9_VTBL(self)[55])(self, a_Index, a_pPlane);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_SetClipStatus(CPU *c, void *self, uint32_t a) {
    uint32_t g_pClipStatus = R32(a + 0);
    void *a_pClipStatus = d3d9_in(c, g_pClipStatus, (uint64_t)(8));
    int32_t hr = ((int32_t (*)(void *, void *))D3D9_VTBL(self)[62])(self, a_pClipStatus);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_SetCurrentTexturePalette(CPU *c, void *self, uint32_t a) {
    uint32_t g_PaletteNumber = R32(a + 0);
    uint32_t a_PaletteNumber = g_PaletteNumber;
    int32_t hr = ((int32_t (*)(void *, uint32_t))D3D9_VTBL(self)[73])(self, a_PaletteNumber);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_SetCursorPosition(CPU *c, void *self, uint32_t a) {
    uint32_t g_X = R32(a + 0);
    uint32_t g_Y = R32(a + 4);
    uint32_t g_Flags = R32(a + 8);
    int32_t a_X = (int32_t)g_X;
    int32_t a_Y = (int32_t)g_Y;
    uint32_t a_Flags = g_Flags;
    ((void (*)(void *, int32_t, int32_t, uint32_t))D3D9_VTBL(self)[11])(self, a_X, a_Y, a_Flags);
    (void)c;
}

static void t_IDirect3DDevice9_SetCursorProperties(CPU *c, void *self, uint32_t a) {
    uint32_t g_XHotSpot = R32(a + 0);
    uint32_t g_YHotSpot = R32(a + 4);
    uint32_t g_pCursorBitmap = R32(a + 8);
    uint32_t a_XHotSpot = g_XHotSpot;
    uint32_t a_YHotSpot = g_YHotSpot;
    void *a_pCursorBitmap = d3d9_iface_in(c, g_pCursorBitmap, 0x800u);
    int32_t hr = ((int32_t (*)(void *, uint32_t, uint32_t, void *))D3D9_VTBL(self)[10])(self, a_XHotSpot, a_YHotSpot, a_pCursorBitmap);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_SetDepthStencilSurface(CPU *c, void *self, uint32_t a) {
    uint32_t g_pNewZStencil = R32(a + 0);
    void *a_pNewZStencil = d3d9_iface_in(c, g_pNewZStencil, 0x800u);
    int32_t hr = ((int32_t (*)(void *, void *))D3D9_VTBL(self)[39])(self, a_pNewZStencil);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_SetDialogBoxMode(CPU *c, void *self, uint32_t a) {
    uint32_t g_bEnableDialogs = R32(a + 0);
    uint32_t a_bEnableDialogs = g_bEnableDialogs;
    int32_t hr = ((int32_t (*)(void *, uint32_t))D3D9_VTBL(self)[20])(self, a_bEnableDialogs);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_SetFVF(CPU *c, void *self, uint32_t a) {
    uint32_t g_FVF = R32(a + 0);
    uint32_t a_FVF = g_FVF;
    int32_t hr = ((int32_t (*)(void *, uint32_t))D3D9_VTBL(self)[89])(self, a_FVF);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_SetGammaRamp(CPU *c, void *self, uint32_t a) {
    uint32_t g_iSwapChain = R32(a + 0);
    uint32_t g_Flags = R32(a + 4);
    uint32_t g_pRamp = R32(a + 8);
    uint32_t a_iSwapChain = g_iSwapChain;
    uint32_t a_Flags = g_Flags;
    void *a_pRamp = d3d9_in(c, g_pRamp, (uint64_t)(1536));
    ((void (*)(void *, uint32_t, uint32_t, void *))D3D9_VTBL(self)[21])(self, a_iSwapChain, a_Flags, a_pRamp);
    (void)c;
}

static void t_IDirect3DDevice9_SetIndices(CPU *c, void *self, uint32_t a) {
    uint32_t g_pIndexData = R32(a + 0);
    void *a_pIndexData = d3d9_iface_in(c, g_pIndexData, 0x400u);
    int32_t hr = ((int32_t (*)(void *, void *))D3D9_VTBL(self)[104])(self, a_pIndexData);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_SetLight(CPU *c, void *self, uint32_t a) {
    uint32_t g_Index = R32(a + 0);
    uint32_t g_pLight = R32(a + 4);
    uint32_t a_Index = g_Index;
    void *a_pLight = d3d9_in(c, g_pLight, (uint64_t)(104));
    int32_t hr = ((int32_t (*)(void *, uint32_t, void *))D3D9_VTBL(self)[51])(self, a_Index, a_pLight);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_SetMaterial(CPU *c, void *self, uint32_t a) {
    uint32_t g_pMaterial = R32(a + 0);
    void *a_pMaterial = d3d9_in(c, g_pMaterial, (uint64_t)(68));
    int32_t hr = ((int32_t (*)(void *, void *))D3D9_VTBL(self)[49])(self, a_pMaterial);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_SetNPatchMode(CPU *c, void *self, uint32_t a) {
    uint32_t g_nSegments = R32(a + 0);
    float a_nSegments = d3d9_f32(g_nSegments);
    int32_t hr = ((int32_t (*)(void *, float))D3D9_VTBL(self)[79])(self, a_nSegments);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_SetPaletteEntries(CPU *c, void *self, uint32_t a) {
    uint32_t g_PaletteNumber = R32(a + 0);
    uint32_t g_pEntries = R32(a + 4);
    uint32_t a_PaletteNumber = g_PaletteNumber;
    void *a_pEntries = d3d9_in(c, g_pEntries, (uint64_t)(1024));
    int32_t hr = ((int32_t (*)(void *, uint32_t, void *))D3D9_VTBL(self)[71])(self, a_PaletteNumber, a_pEntries);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_SetPixelShader(CPU *c, void *self, uint32_t a) {
    uint32_t g_pShader = R32(a + 0);
    void *a_pShader = d3d9_iface_in(c, g_pShader, 0x8000u);
    int32_t hr = ((int32_t (*)(void *, void *))D3D9_VTBL(self)[107])(self, a_pShader);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_SetPixelShaderConstantB(CPU *c, void *self, uint32_t a) {
    uint32_t g_StartRegister = R32(a + 0);
    uint32_t g_pConstantData = R32(a + 4);
    uint32_t g_BoolCount = R32(a + 8);
    uint32_t a_StartRegister = g_StartRegister;
    uint32_t a_BoolCount = g_BoolCount;
    void *a_pConstantData = d3d9_in(c, g_pConstantData, (uint64_t)((uint64_t)a_BoolCount*4));
    int32_t hr = ((int32_t (*)(void *, uint32_t, void *, uint32_t))D3D9_VTBL(self)[113])(self, a_StartRegister, a_pConstantData, a_BoolCount);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_SetPixelShaderConstantF(CPU *c, void *self, uint32_t a) {
    uint32_t g_StartRegister = R32(a + 0);
    uint32_t g_pConstantData = R32(a + 4);
    uint32_t g_Vector4fCount = R32(a + 8);
    uint32_t a_StartRegister = g_StartRegister;
    uint32_t a_Vector4fCount = g_Vector4fCount;
    void *a_pConstantData = d3d9_in(c, g_pConstantData, (uint64_t)((uint64_t)a_Vector4fCount*16));
    int32_t hr = ((int32_t (*)(void *, uint32_t, void *, uint32_t))D3D9_VTBL(self)[109])(self, a_StartRegister, a_pConstantData, a_Vector4fCount);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_SetPixelShaderConstantI(CPU *c, void *self, uint32_t a) {
    uint32_t g_StartRegister = R32(a + 0);
    uint32_t g_pConstantData = R32(a + 4);
    uint32_t g_Vector4iCount = R32(a + 8);
    uint32_t a_StartRegister = g_StartRegister;
    uint32_t a_Vector4iCount = g_Vector4iCount;
    void *a_pConstantData = d3d9_in(c, g_pConstantData, (uint64_t)((uint64_t)a_Vector4iCount*16));
    int32_t hr = ((int32_t (*)(void *, uint32_t, void *, uint32_t))D3D9_VTBL(self)[111])(self, a_StartRegister, a_pConstantData, a_Vector4iCount);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_SetRenderState(CPU *c, void *self, uint32_t a) {
    uint32_t g_State = R32(a + 0);
    uint32_t g_Value = R32(a + 4);
    uint32_t a_State = g_State;
    uint32_t a_Value = g_Value;
    int32_t hr = ((int32_t (*)(void *, uint32_t, uint32_t))D3D9_VTBL(self)[57])(self, a_State, a_Value);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_SetRenderTarget(CPU *c, void *self, uint32_t a) {
    uint32_t g_RenderTargetIndex = R32(a + 0);
    uint32_t g_pRenderTarget = R32(a + 4);
    uint32_t a_RenderTargetIndex = g_RenderTargetIndex;
    void *a_pRenderTarget = d3d9_iface_in(c, g_pRenderTarget, 0x800u);
    int32_t hr = ((int32_t (*)(void *, uint32_t, void *))D3D9_VTBL(self)[37])(self, a_RenderTargetIndex, a_pRenderTarget);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_SetSamplerState(CPU *c, void *self, uint32_t a) {
    uint32_t g_Sampler = R32(a + 0);
    uint32_t g_Type = R32(a + 4);
    uint32_t g_Value = R32(a + 8);
    uint32_t a_Sampler = g_Sampler;
    uint32_t a_Type = g_Type;
    uint32_t a_Value = g_Value;
    int32_t hr = ((int32_t (*)(void *, uint32_t, uint32_t, uint32_t))D3D9_VTBL(self)[69])(self, a_Sampler, a_Type, a_Value);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_SetScissorRect(CPU *c, void *self, uint32_t a) {
    uint32_t g_pRect = R32(a + 0);
    void *a_pRect = d3d9_in(c, g_pRect, (uint64_t)(16));
    int32_t hr = ((int32_t (*)(void *, void *))D3D9_VTBL(self)[75])(self, a_pRect);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_SetSoftwareVertexProcessing(CPU *c, void *self, uint32_t a) {
    uint32_t g_bSoftware = R32(a + 0);
    uint32_t a_bSoftware = g_bSoftware;
    int32_t hr = ((int32_t (*)(void *, uint32_t))D3D9_VTBL(self)[77])(self, a_bSoftware);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_SetStreamSource(CPU *c, void *self, uint32_t a) {
    uint32_t g_StreamNumber = R32(a + 0);
    uint32_t g_pStreamData = R32(a + 4);
    uint32_t g_OffsetInBytes = R32(a + 8);
    uint32_t g_Stride = R32(a + 12);
    uint32_t a_StreamNumber = g_StreamNumber;
    uint32_t a_OffsetInBytes = g_OffsetInBytes;
    uint32_t a_Stride = g_Stride;
    void *a_pStreamData = d3d9_iface_in(c, g_pStreamData, 0x200u);
    int32_t hr = ((int32_t (*)(void *, uint32_t, void *, uint32_t, uint32_t))D3D9_VTBL(self)[100])(self, a_StreamNumber, a_pStreamData, a_OffsetInBytes, a_Stride);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_SetStreamSourceFreq(CPU *c, void *self, uint32_t a) {
    uint32_t g_StreamNumber = R32(a + 0);
    uint32_t g_Setting = R32(a + 4);
    uint32_t a_StreamNumber = g_StreamNumber;
    uint32_t a_Setting = g_Setting;
    int32_t hr = ((int32_t (*)(void *, uint32_t, uint32_t))D3D9_VTBL(self)[102])(self, a_StreamNumber, a_Setting);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_SetTexture(CPU *c, void *self, uint32_t a) {
    uint32_t g_Stage = R32(a + 0);
    uint32_t g_pTexture = R32(a + 4);
    uint32_t a_Stage = g_Stage;
    void *a_pTexture = d3d9_iface_in(c, g_pTexture, 0x1c0u);
    int32_t hr = ((int32_t (*)(void *, uint32_t, void *))D3D9_VTBL(self)[65])(self, a_Stage, a_pTexture);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_SetTextureStageState(CPU *c, void *self, uint32_t a) {
    uint32_t g_Stage = R32(a + 0);
    uint32_t g_Type = R32(a + 4);
    uint32_t g_Value = R32(a + 8);
    uint32_t a_Stage = g_Stage;
    uint32_t a_Type = g_Type;
    uint32_t a_Value = g_Value;
    int32_t hr = ((int32_t (*)(void *, uint32_t, uint32_t, uint32_t))D3D9_VTBL(self)[67])(self, a_Stage, a_Type, a_Value);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_SetTransform(CPU *c, void *self, uint32_t a) {
    uint32_t g_State = R32(a + 0);
    uint32_t g_pMatrix = R32(a + 4);
    uint32_t a_State = g_State;
    void *a_pMatrix = d3d9_in(c, g_pMatrix, (uint64_t)(64));
    int32_t hr = ((int32_t (*)(void *, uint32_t, void *))D3D9_VTBL(self)[44])(self, a_State, a_pMatrix);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_SetVertexDeclaration(CPU *c, void *self, uint32_t a) {
    uint32_t g_pDecl = R32(a + 0);
    void *a_pDecl = d3d9_iface_in(c, g_pDecl, 0x2000u);
    int32_t hr = ((int32_t (*)(void *, void *))D3D9_VTBL(self)[87])(self, a_pDecl);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_SetVertexShader(CPU *c, void *self, uint32_t a) {
    uint32_t g_pShader = R32(a + 0);
    void *a_pShader = d3d9_iface_in(c, g_pShader, 0x4000u);
    int32_t hr = ((int32_t (*)(void *, void *))D3D9_VTBL(self)[92])(self, a_pShader);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_SetVertexShaderConstantB(CPU *c, void *self, uint32_t a) {
    uint32_t g_StartRegister = R32(a + 0);
    uint32_t g_pConstantData = R32(a + 4);
    uint32_t g_BoolCount = R32(a + 8);
    uint32_t a_StartRegister = g_StartRegister;
    uint32_t a_BoolCount = g_BoolCount;
    void *a_pConstantData = d3d9_in(c, g_pConstantData, (uint64_t)((uint64_t)a_BoolCount*4));
    int32_t hr = ((int32_t (*)(void *, uint32_t, void *, uint32_t))D3D9_VTBL(self)[98])(self, a_StartRegister, a_pConstantData, a_BoolCount);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_SetVertexShaderConstantF(CPU *c, void *self, uint32_t a) {
    uint32_t g_StartRegister = R32(a + 0);
    uint32_t g_pConstantData = R32(a + 4);
    uint32_t g_Vector4fCount = R32(a + 8);
    uint32_t a_StartRegister = g_StartRegister;
    uint32_t a_Vector4fCount = g_Vector4fCount;
    void *a_pConstantData = d3d9_in(c, g_pConstantData, (uint64_t)((uint64_t)a_Vector4fCount*16));
    int32_t hr = ((int32_t (*)(void *, uint32_t, void *, uint32_t))D3D9_VTBL(self)[94])(self, a_StartRegister, a_pConstantData, a_Vector4fCount);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_SetVertexShaderConstantI(CPU *c, void *self, uint32_t a) {
    uint32_t g_StartRegister = R32(a + 0);
    uint32_t g_pConstantData = R32(a + 4);
    uint32_t g_Vector4iCount = R32(a + 8);
    uint32_t a_StartRegister = g_StartRegister;
    uint32_t a_Vector4iCount = g_Vector4iCount;
    void *a_pConstantData = d3d9_in(c, g_pConstantData, (uint64_t)((uint64_t)a_Vector4iCount*16));
    int32_t hr = ((int32_t (*)(void *, uint32_t, void *, uint32_t))D3D9_VTBL(self)[96])(self, a_StartRegister, a_pConstantData, a_Vector4iCount);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_SetViewport(CPU *c, void *self, uint32_t a) {
    uint32_t g_pViewport = R32(a + 0);
    void *a_pViewport = d3d9_in(c, g_pViewport, (uint64_t)(24));
    int32_t hr = ((int32_t (*)(void *, void *))D3D9_VTBL(self)[47])(self, a_pViewport);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_ShowCursor(CPU *c, void *self, uint32_t a) {
    uint32_t g_bShow = R32(a + 0);
    uint32_t a_bShow = g_bShow;
    c->eax = ((uint32_t (*)(void *, uint32_t))D3D9_VTBL(self)[12])(self, a_bShow);
}

static void t_IDirect3DDevice9_StretchRect(CPU *c, void *self, uint32_t a) {
    uint32_t g_pSourceSurface = R32(a + 0);
    uint32_t g_pSourceRect = R32(a + 4);
    uint32_t g_pDestSurface = R32(a + 8);
    uint32_t g_pDestRect = R32(a + 12);
    uint32_t g_Filter = R32(a + 16);
    uint32_t a_Filter = g_Filter;
    void *a_pSourceSurface = d3d9_iface_in(c, g_pSourceSurface, 0x800u);
    void *a_pSourceRect = d3d9_in(c, g_pSourceRect, (uint64_t)(16));
    void *a_pDestSurface = d3d9_iface_in(c, g_pDestSurface, 0x800u);
    void *a_pDestRect = d3d9_in(c, g_pDestRect, (uint64_t)(16));
    int32_t hr = ((int32_t (*)(void *, void *, void *, void *, void *, uint32_t))D3D9_VTBL(self)[34])(self, a_pSourceSurface, a_pSourceRect, a_pDestSurface, a_pDestRect, a_Filter);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_TestCooperativeLevel(CPU *c, void *self, uint32_t a) {
    int32_t hr = ((int32_t (*)(void *))D3D9_VTBL(self)[3])(self);
    c->eax = (uint32_t)hr;
    (void)a;
}

static void t_IDirect3DDevice9_UpdateSurface(CPU *c, void *self, uint32_t a) {
    uint32_t g_pSourceSurface = R32(a + 0);
    uint32_t g_pSourceRect = R32(a + 4);
    uint32_t g_pDestinationSurface = R32(a + 8);
    uint32_t g_pDestPoint = R32(a + 12);
    void *a_pSourceSurface = d3d9_iface_in(c, g_pSourceSurface, 0x800u);
    void *a_pSourceRect = d3d9_in(c, g_pSourceRect, (uint64_t)(16));
    void *a_pDestinationSurface = d3d9_iface_in(c, g_pDestinationSurface, 0x800u);
    void *a_pDestPoint = d3d9_in(c, g_pDestPoint, (uint64_t)(8));
    int32_t hr = ((int32_t (*)(void *, void *, void *, void *, void *))D3D9_VTBL(self)[30])(self, a_pSourceSurface, a_pSourceRect, a_pDestinationSurface, a_pDestPoint);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_UpdateTexture(CPU *c, void *self, uint32_t a) {
    uint32_t g_pSourceTexture = R32(a + 0);
    uint32_t g_pDestinationTexture = R32(a + 4);
    void *a_pSourceTexture = d3d9_iface_in(c, g_pSourceTexture, 0x1c0u);
    void *a_pDestinationTexture = d3d9_iface_in(c, g_pDestinationTexture, 0x1c0u);
    int32_t hr = ((int32_t (*)(void *, void *, void *))D3D9_VTBL(self)[31])(self, a_pSourceTexture, a_pDestinationTexture);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DDevice9_ValidateDevice(CPU *c, void *self, uint32_t a) {
    uint32_t g_pNumPasses = R32(a + 0);
    void *a_pNumPasses = d3d9_out(c, g_pNumPasses, (uint64_t)(4));
    int32_t hr = ((int32_t (*)(void *, void *))D3D9_VTBL(self)[70])(self, a_pNumPasses);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DIndexBuffer9_GetDesc(CPU *c, void *self, uint32_t a) {
    uint32_t g_pDesc = R32(a + 0);
    void *a_pDesc = d3d9_out(c, g_pDesc, (uint64_t)(20));
    int32_t hr = ((int32_t (*)(void *, void *))D3D9_VTBL(self)[13])(self, a_pDesc);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DIndexBuffer9_Lock(CPU *c, void *self, uint32_t a) {
    uint32_t g_OffsetToLock = R32(a + 0);
    uint32_t g_SizeToLock = R32(a + 4);
    uint32_t g_ppbData = R32(a + 8);
    uint32_t g_Flags = R32(a + 12);
    uint32_t a_OffsetToLock = g_OffsetToLock;
    uint32_t a_SizeToLock = g_SizeToLock;
    uint32_t a_Flags = g_Flags;
    void *h_ppbData = NULL, **a_ppbData = d3d9_out_slot(c, g_ppbData) ? &h_ppbData : NULL;
    int32_t hr = ((int32_t (*)(void *, uint32_t, uint32_t, void **, uint32_t))D3D9_VTBL(self)[11])(self, a_OffsetToLock, a_SizeToLock, a_ppbData, a_Flags);
    d3d9_lock_ptr_out(c, g_ppbData, h_ppbData, hr >= 0);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DIndexBuffer9_Unlock(CPU *c, void *self, uint32_t a) {
    int32_t hr = ((int32_t (*)(void *))D3D9_VTBL(self)[12])(self);
    c->eax = (uint32_t)hr;
    (void)a;
}

static void t_IDirect3DPixelShader9_GetDevice(CPU *c, void *self, uint32_t a) {
    uint32_t g_ppDevice = R32(a + 0);
    void *h_ppDevice = NULL, **a_ppDevice = d3d9_out_slot(c, g_ppDevice) ? &h_ppDevice : NULL;
    int32_t hr = ((int32_t (*)(void *, void **))D3D9_VTBL(self)[3])(self, a_ppDevice);
    d3d9_iface_out(c, g_ppDevice, h_ppDevice, D3D9_IDirect3DDevice9);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DPixelShader9_GetFunction(CPU *c, void *self, uint32_t a) {
    uint32_t g_pData = R32(a + 0);
    uint32_t g_pSizeOfData = R32(a + 4);
    void *a_pData = d3d9_out(c, g_pData, (uint64_t)(d3d9_deref(c, g_pSizeOfData)));
    void *a_pSizeOfData = d3d9_out(c, g_pSizeOfData, (uint64_t)(4));
    int32_t hr = ((int32_t (*)(void *, void *, void *))D3D9_VTBL(self)[4])(self, a_pData, a_pSizeOfData);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DQuery9_GetData(CPU *c, void *self, uint32_t a) {
    uint32_t g_pData = R32(a + 0);
    uint32_t g_dwSize = R32(a + 4);
    uint32_t g_dwGetDataFlags = R32(a + 8);
    uint32_t a_dwSize = g_dwSize;
    uint32_t a_dwGetDataFlags = g_dwGetDataFlags;
    void *a_pData = d3d9_out(c, g_pData, (uint64_t)(a_dwSize));
    int32_t hr = ((int32_t (*)(void *, void *, uint32_t, uint32_t))D3D9_VTBL(self)[7])(self, a_pData, a_dwSize, a_dwGetDataFlags);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DQuery9_GetDataSize(CPU *c, void *self, uint32_t a) {
    c->eax = ((uint32_t (*)(void *))D3D9_VTBL(self)[5])(self);
    (void)a;
}

static void t_IDirect3DQuery9_GetDevice(CPU *c, void *self, uint32_t a) {
    uint32_t g_ppDevice = R32(a + 0);
    void *h_ppDevice = NULL, **a_ppDevice = d3d9_out_slot(c, g_ppDevice) ? &h_ppDevice : NULL;
    int32_t hr = ((int32_t (*)(void *, void **))D3D9_VTBL(self)[3])(self, a_ppDevice);
    d3d9_iface_out(c, g_ppDevice, h_ppDevice, D3D9_IDirect3DDevice9);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DQuery9_GetType(CPU *c, void *self, uint32_t a) {
    c->eax = ((uint32_t (*)(void *))D3D9_VTBL(self)[4])(self);
    (void)a;
}

static void t_IDirect3DQuery9_Issue(CPU *c, void *self, uint32_t a) {
    uint32_t g_dwIssueFlags = R32(a + 0);
    uint32_t a_dwIssueFlags = g_dwIssueFlags;
    int32_t hr = ((int32_t (*)(void *, uint32_t))D3D9_VTBL(self)[6])(self, a_dwIssueFlags);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DResource9_FreePrivateData(CPU *c, void *self, uint32_t a) {
    uint32_t g_refguid = R32(a + 0);
    void *a_refguid = d3d9_in(c, g_refguid, (uint64_t)(16));
    int32_t hr = ((int32_t (*)(void *, void *))D3D9_VTBL(self)[6])(self, a_refguid);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DResource9_GetDevice(CPU *c, void *self, uint32_t a) {
    uint32_t g_ppDevice = R32(a + 0);
    void *h_ppDevice = NULL, **a_ppDevice = d3d9_out_slot(c, g_ppDevice) ? &h_ppDevice : NULL;
    int32_t hr = ((int32_t (*)(void *, void **))D3D9_VTBL(self)[3])(self, a_ppDevice);
    d3d9_iface_out(c, g_ppDevice, h_ppDevice, D3D9_IDirect3DDevice9);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DResource9_GetPriority(CPU *c, void *self, uint32_t a) {
    c->eax = ((uint32_t (*)(void *))D3D9_VTBL(self)[8])(self);
    (void)a;
}

static void t_IDirect3DResource9_GetPrivateData(CPU *c, void *self, uint32_t a) {
    uint32_t g_refguid = R32(a + 0);
    uint32_t g_pData = R32(a + 4);
    uint32_t g_pSizeOfData = R32(a + 8);
    void *a_refguid = d3d9_in(c, g_refguid, (uint64_t)(16));
    void *a_pData = d3d9_out(c, g_pData, (uint64_t)(d3d9_deref(c, g_pSizeOfData)));
    void *a_pSizeOfData = d3d9_out(c, g_pSizeOfData, (uint64_t)(4));
    int32_t hr = ((int32_t (*)(void *, void *, void *, void *))D3D9_VTBL(self)[5])(self, a_refguid, a_pData, a_pSizeOfData);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DResource9_GetType(CPU *c, void *self, uint32_t a) {
    c->eax = ((uint32_t (*)(void *))D3D9_VTBL(self)[10])(self);
    (void)a;
}

static void t_IDirect3DResource9_PreLoad(CPU *c, void *self, uint32_t a) {
    ((void (*)(void *))D3D9_VTBL(self)[9])(self);
    (void)c; (void)a;
}

static void t_IDirect3DResource9_SetPriority(CPU *c, void *self, uint32_t a) {
    uint32_t g_PriorityNew = R32(a + 0);
    uint32_t a_PriorityNew = g_PriorityNew;
    c->eax = ((uint32_t (*)(void *, uint32_t))D3D9_VTBL(self)[7])(self, a_PriorityNew);
}

static void t_IDirect3DStateBlock9_Apply(CPU *c, void *self, uint32_t a) {
    int32_t hr = ((int32_t (*)(void *))D3D9_VTBL(self)[5])(self);
    c->eax = (uint32_t)hr;
    (void)a;
}

static void t_IDirect3DStateBlock9_Capture(CPU *c, void *self, uint32_t a) {
    int32_t hr = ((int32_t (*)(void *))D3D9_VTBL(self)[4])(self);
    c->eax = (uint32_t)hr;
    (void)a;
}

static void t_IDirect3DStateBlock9_GetDevice(CPU *c, void *self, uint32_t a) {
    uint32_t g_ppDevice = R32(a + 0);
    void *h_ppDevice = NULL, **a_ppDevice = d3d9_out_slot(c, g_ppDevice) ? &h_ppDevice : NULL;
    int32_t hr = ((int32_t (*)(void *, void **))D3D9_VTBL(self)[3])(self, a_ppDevice);
    d3d9_iface_out(c, g_ppDevice, h_ppDevice, D3D9_IDirect3DDevice9);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DSurface9_GetDesc(CPU *c, void *self, uint32_t a) {
    uint32_t g_pDesc = R32(a + 0);
    void *a_pDesc = d3d9_out(c, g_pDesc, (uint64_t)(32));
    int32_t hr = ((int32_t (*)(void *, void *))D3D9_VTBL(self)[12])(self, a_pDesc);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DSurface9_LockRect(CPU *c, void *self, uint32_t a) {
    uint32_t g_pLockedRect = R32(a + 0);
    uint32_t g_pRect = R32(a + 4);
    uint32_t g_Flags = R32(a + 8);
    uint32_t a_Flags = g_Flags;
    D3d9HostLocked h_pLockedRect; void *a_pLockedRect = d3d9_locked_in(c, g_pLockedRect, &h_pLockedRect, 8);
    void *a_pRect = d3d9_in(c, g_pRect, (uint64_t)(16));
    int32_t hr = ((int32_t (*)(void *, void *, void *, uint32_t))D3D9_VTBL(self)[13])(self, a_pLockedRect, a_pRect, a_Flags);
    d3d9_lockrect_out(c, g_pLockedRect, &h_pLockedRect, hr >= 0);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DSurface9_UnlockRect(CPU *c, void *self, uint32_t a) {
    int32_t hr = ((int32_t (*)(void *))D3D9_VTBL(self)[14])(self);
    c->eax = (uint32_t)hr;
    (void)a;
}

static void t_IDirect3DSwapChain9_GetBackBuffer(CPU *c, void *self, uint32_t a) {
    uint32_t g_iBackBuffer = R32(a + 0);
    uint32_t g_Type = R32(a + 4);
    uint32_t g_ppBackBuffer = R32(a + 8);
    uint32_t a_iBackBuffer = g_iBackBuffer;
    uint32_t a_Type = g_Type;
    void *h_ppBackBuffer = NULL, **a_ppBackBuffer = d3d9_out_slot(c, g_ppBackBuffer) ? &h_ppBackBuffer : NULL;
    int32_t hr = ((int32_t (*)(void *, uint32_t, uint32_t, void **))D3D9_VTBL(self)[5])(self, a_iBackBuffer, a_Type, a_ppBackBuffer);
    d3d9_child_out(c, g_ppBackBuffer, h_ppBackBuffer, D3D9_IDirect3DSurface9, self, 0);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DSwapChain9_GetDevice(CPU *c, void *self, uint32_t a) {
    uint32_t g_ppDevice = R32(a + 0);
    void *h_ppDevice = NULL, **a_ppDevice = d3d9_out_slot(c, g_ppDevice) ? &h_ppDevice : NULL;
    int32_t hr = ((int32_t (*)(void *, void **))D3D9_VTBL(self)[8])(self, a_ppDevice);
    d3d9_iface_out(c, g_ppDevice, h_ppDevice, D3D9_IDirect3DDevice9);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DSwapChain9_GetDisplayMode(CPU *c, void *self, uint32_t a) {
    uint32_t g_pMode = R32(a + 0);
    void *a_pMode = d3d9_out(c, g_pMode, (uint64_t)(16));
    int32_t hr = ((int32_t (*)(void *, void *))D3D9_VTBL(self)[7])(self, a_pMode);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DSwapChain9_GetFrontBufferData(CPU *c, void *self, uint32_t a) {
    uint32_t g_pDestSurface = R32(a + 0);
    void *a_pDestSurface = d3d9_iface_in(c, g_pDestSurface, 0x800u);
    int32_t hr = ((int32_t (*)(void *, void *))D3D9_VTBL(self)[4])(self, a_pDestSurface);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DSwapChain9_GetPresentParameters(CPU *c, void *self, uint32_t a) {
    uint32_t g_pPresentationParameters = R32(a + 0);
    D3d9HostParams h_pPresentationParameters; void *a_pPresentationParameters = d3d9_params_in(c, g_pPresentationParameters, &h_pPresentationParameters, 0);
    int32_t hr = ((int32_t (*)(void *, void *))D3D9_VTBL(self)[9])(self, a_pPresentationParameters);
    if (hr >= 0) d3d9_params_out(c, g_pPresentationParameters, &h_pPresentationParameters);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DSwapChain9_GetRasterStatus(CPU *c, void *self, uint32_t a) {
    uint32_t g_pRasterStatus = R32(a + 0);
    void *a_pRasterStatus = d3d9_out(c, g_pRasterStatus, (uint64_t)(8));
    int32_t hr = ((int32_t (*)(void *, void *))D3D9_VTBL(self)[6])(self, a_pRasterStatus);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DSwapChain9_Present(CPU *c, void *self, uint32_t a) {
    uint32_t g_pSourceRect = R32(a + 0);
    uint32_t g_pDestRect = R32(a + 4);
    uint32_t g_hDestWindowOverride = R32(a + 8);
    uint32_t g_pDirtyRegion = R32(a + 12);
    uint32_t g_dwFlags = R32(a + 16);
    uint32_t a_dwFlags = g_dwFlags;
    void *a_pSourceRect = d3d9_in(c, g_pSourceRect, (uint64_t)(16));
    void *a_pDestRect = d3d9_in(c, g_pDestRect, (uint64_t)(16));
    void *a_hDestWindowOverride = d3d9_hwnd(c, g_hDestWindowOverride);
    const void *a_pDirtyRegion = d3d9_rgndata(c, g_pDirtyRegion);
    int32_t hr = ((int32_t (*)(void *, void *, void *, void *, const void *, uint32_t))D3D9_VTBL(self)[3])(self, a_pSourceRect, a_pDestRect, a_hDestWindowOverride, a_pDirtyRegion, a_dwFlags);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DTexture9_AddDirtyRect(CPU *c, void *self, uint32_t a) {
    uint32_t g_pDirtyRect = R32(a + 0);
    void *a_pDirtyRect = d3d9_in(c, g_pDirtyRect, (uint64_t)(16));
    int32_t hr = ((int32_t (*)(void *, void *))D3D9_VTBL(self)[21])(self, a_pDirtyRect);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DTexture9_GetLevelDesc(CPU *c, void *self, uint32_t a) {
    uint32_t g_Level = R32(a + 0);
    uint32_t g_pDesc = R32(a + 4);
    uint32_t a_Level = g_Level;
    void *a_pDesc = d3d9_out(c, g_pDesc, (uint64_t)(32));
    int32_t hr = ((int32_t (*)(void *, uint32_t, void *))D3D9_VTBL(self)[17])(self, a_Level, a_pDesc);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DTexture9_GetSurfaceLevel(CPU *c, void *self, uint32_t a) {
    uint32_t g_Level = R32(a + 0);
    uint32_t g_ppSurfaceLevel = R32(a + 4);
    uint32_t a_Level = g_Level;
    void *h_ppSurfaceLevel = NULL, **a_ppSurfaceLevel = d3d9_out_slot(c, g_ppSurfaceLevel) ? &h_ppSurfaceLevel : NULL;
    int32_t hr = ((int32_t (*)(void *, uint32_t, void **))D3D9_VTBL(self)[18])(self, a_Level, a_ppSurfaceLevel);
    d3d9_child_out(c, g_ppSurfaceLevel, h_ppSurfaceLevel, D3D9_IDirect3DSurface9, self, 1);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DTexture9_LockRect(CPU *c, void *self, uint32_t a) {
    uint32_t g_Level = R32(a + 0);
    uint32_t g_pLockedRect = R32(a + 4);
    uint32_t g_pRect = R32(a + 8);
    uint32_t g_Flags = R32(a + 12);
    uint32_t a_Level = g_Level;
    uint32_t a_Flags = g_Flags;
    D3d9HostLocked h_pLockedRect; void *a_pLockedRect = d3d9_locked_in(c, g_pLockedRect, &h_pLockedRect, 8);
    void *a_pRect = d3d9_in(c, g_pRect, (uint64_t)(16));
    int32_t hr = ((int32_t (*)(void *, uint32_t, void *, void *, uint32_t))D3D9_VTBL(self)[19])(self, a_Level, a_pLockedRect, a_pRect, a_Flags);
    d3d9_lockrect_out(c, g_pLockedRect, &h_pLockedRect, hr >= 0);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DTexture9_UnlockRect(CPU *c, void *self, uint32_t a) {
    uint32_t g_Level = R32(a + 0);
    uint32_t a_Level = g_Level;
    int32_t hr = ((int32_t (*)(void *, uint32_t))D3D9_VTBL(self)[20])(self, a_Level);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DVertexBuffer9_GetDesc(CPU *c, void *self, uint32_t a) {
    uint32_t g_pDesc = R32(a + 0);
    void *a_pDesc = d3d9_out(c, g_pDesc, (uint64_t)(24));
    int32_t hr = ((int32_t (*)(void *, void *))D3D9_VTBL(self)[13])(self, a_pDesc);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DVertexBuffer9_Lock(CPU *c, void *self, uint32_t a) {
    uint32_t g_OffsetToLock = R32(a + 0);
    uint32_t g_SizeToLock = R32(a + 4);
    uint32_t g_ppbData = R32(a + 8);
    uint32_t g_Flags = R32(a + 12);
    uint32_t a_OffsetToLock = g_OffsetToLock;
    uint32_t a_SizeToLock = g_SizeToLock;
    uint32_t a_Flags = g_Flags;
    void *h_ppbData = NULL, **a_ppbData = d3d9_out_slot(c, g_ppbData) ? &h_ppbData : NULL;
    int32_t hr = ((int32_t (*)(void *, uint32_t, uint32_t, void **, uint32_t))D3D9_VTBL(self)[11])(self, a_OffsetToLock, a_SizeToLock, a_ppbData, a_Flags);
    d3d9_lock_ptr_out(c, g_ppbData, h_ppbData, hr >= 0);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DVertexBuffer9_Unlock(CPU *c, void *self, uint32_t a) {
    int32_t hr = ((int32_t (*)(void *))D3D9_VTBL(self)[12])(self);
    c->eax = (uint32_t)hr;
    (void)a;
}

static void t_IDirect3DVertexDeclaration9_GetDevice(CPU *c, void *self, uint32_t a) {
    uint32_t g_ppDevice = R32(a + 0);
    void *h_ppDevice = NULL, **a_ppDevice = d3d9_out_slot(c, g_ppDevice) ? &h_ppDevice : NULL;
    int32_t hr = ((int32_t (*)(void *, void **))D3D9_VTBL(self)[3])(self, a_ppDevice);
    d3d9_iface_out(c, g_ppDevice, h_ppDevice, D3D9_IDirect3DDevice9);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DVertexShader9_GetDevice(CPU *c, void *self, uint32_t a) {
    uint32_t g_ppDevice = R32(a + 0);
    void *h_ppDevice = NULL, **a_ppDevice = d3d9_out_slot(c, g_ppDevice) ? &h_ppDevice : NULL;
    int32_t hr = ((int32_t (*)(void *, void **))D3D9_VTBL(self)[3])(self, a_ppDevice);
    d3d9_iface_out(c, g_ppDevice, h_ppDevice, D3D9_IDirect3DDevice9);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DVertexShader9_GetFunction(CPU *c, void *self, uint32_t a) {
    uint32_t g_pData = R32(a + 0);
    uint32_t g_pSizeOfData = R32(a + 4);
    void *a_pData = d3d9_out(c, g_pData, (uint64_t)(d3d9_deref(c, g_pSizeOfData)));
    void *a_pSizeOfData = d3d9_out(c, g_pSizeOfData, (uint64_t)(4));
    int32_t hr = ((int32_t (*)(void *, void *, void *))D3D9_VTBL(self)[4])(self, a_pData, a_pSizeOfData);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DVolume9_FreePrivateData(CPU *c, void *self, uint32_t a) {
    uint32_t g_refguid = R32(a + 0);
    void *a_refguid = d3d9_in(c, g_refguid, (uint64_t)(16));
    int32_t hr = ((int32_t (*)(void *, void *))D3D9_VTBL(self)[6])(self, a_refguid);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DVolume9_GetDesc(CPU *c, void *self, uint32_t a) {
    uint32_t g_pDesc = R32(a + 0);
    void *a_pDesc = d3d9_out(c, g_pDesc, (uint64_t)(28));
    int32_t hr = ((int32_t (*)(void *, void *))D3D9_VTBL(self)[8])(self, a_pDesc);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DVolume9_GetDevice(CPU *c, void *self, uint32_t a) {
    uint32_t g_ppDevice = R32(a + 0);
    void *h_ppDevice = NULL, **a_ppDevice = d3d9_out_slot(c, g_ppDevice) ? &h_ppDevice : NULL;
    int32_t hr = ((int32_t (*)(void *, void **))D3D9_VTBL(self)[3])(self, a_ppDevice);
    d3d9_iface_out(c, g_ppDevice, h_ppDevice, D3D9_IDirect3DDevice9);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DVolume9_GetPrivateData(CPU *c, void *self, uint32_t a) {
    uint32_t g_refguid = R32(a + 0);
    uint32_t g_pData = R32(a + 4);
    uint32_t g_pSizeOfData = R32(a + 8);
    void *a_refguid = d3d9_in(c, g_refguid, (uint64_t)(16));
    void *a_pData = d3d9_out(c, g_pData, (uint64_t)(d3d9_deref(c, g_pSizeOfData)));
    void *a_pSizeOfData = d3d9_out(c, g_pSizeOfData, (uint64_t)(4));
    int32_t hr = ((int32_t (*)(void *, void *, void *, void *))D3D9_VTBL(self)[5])(self, a_refguid, a_pData, a_pSizeOfData);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DVolume9_LockBox(CPU *c, void *self, uint32_t a) {
    uint32_t g_pLockedVolume = R32(a + 0);
    uint32_t g_pBox = R32(a + 4);
    uint32_t g_Flags = R32(a + 8);
    uint32_t a_Flags = g_Flags;
    D3d9HostLocked h_pLockedVolume; void *a_pLockedVolume = d3d9_locked_in(c, g_pLockedVolume, &h_pLockedVolume, 12);
    void *a_pBox = d3d9_in(c, g_pBox, (uint64_t)(24));
    int32_t hr = ((int32_t (*)(void *, void *, void *, uint32_t))D3D9_VTBL(self)[9])(self, a_pLockedVolume, a_pBox, a_Flags);
    d3d9_lockbox_out(c, g_pLockedVolume, &h_pLockedVolume, hr >= 0);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DVolume9_UnlockBox(CPU *c, void *self, uint32_t a) {
    int32_t hr = ((int32_t (*)(void *))D3D9_VTBL(self)[10])(self);
    c->eax = (uint32_t)hr;
    (void)a;
}

static void t_IDirect3DVolumeTexture9_AddDirtyBox(CPU *c, void *self, uint32_t a) {
    uint32_t g_pDirtyBox = R32(a + 0);
    void *a_pDirtyBox = d3d9_in(c, g_pDirtyBox, (uint64_t)(24));
    int32_t hr = ((int32_t (*)(void *, void *))D3D9_VTBL(self)[21])(self, a_pDirtyBox);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DVolumeTexture9_GetLevelDesc(CPU *c, void *self, uint32_t a) {
    uint32_t g_Level = R32(a + 0);
    uint32_t g_pDesc = R32(a + 4);
    uint32_t a_Level = g_Level;
    void *a_pDesc = d3d9_out(c, g_pDesc, (uint64_t)(28));
    int32_t hr = ((int32_t (*)(void *, uint32_t, void *))D3D9_VTBL(self)[17])(self, a_Level, a_pDesc);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DVolumeTexture9_GetVolumeLevel(CPU *c, void *self, uint32_t a) {
    uint32_t g_Level = R32(a + 0);
    uint32_t g_ppVolumeLevel = R32(a + 4);
    uint32_t a_Level = g_Level;
    void *h_ppVolumeLevel = NULL, **a_ppVolumeLevel = d3d9_out_slot(c, g_ppVolumeLevel) ? &h_ppVolumeLevel : NULL;
    int32_t hr = ((int32_t (*)(void *, uint32_t, void **))D3D9_VTBL(self)[18])(self, a_Level, a_ppVolumeLevel);
    d3d9_child_out(c, g_ppVolumeLevel, h_ppVolumeLevel, D3D9_IDirect3DVolume9, self, 1);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DVolumeTexture9_LockBox(CPU *c, void *self, uint32_t a) {
    uint32_t g_Level = R32(a + 0);
    uint32_t g_pLockedVolume = R32(a + 4);
    uint32_t g_pBox = R32(a + 8);
    uint32_t g_Flags = R32(a + 12);
    uint32_t a_Level = g_Level;
    uint32_t a_Flags = g_Flags;
    D3d9HostLocked h_pLockedVolume; void *a_pLockedVolume = d3d9_locked_in(c, g_pLockedVolume, &h_pLockedVolume, 12);
    void *a_pBox = d3d9_in(c, g_pBox, (uint64_t)(24));
    int32_t hr = ((int32_t (*)(void *, uint32_t, void *, void *, uint32_t))D3D9_VTBL(self)[19])(self, a_Level, a_pLockedVolume, a_pBox, a_Flags);
    d3d9_lockbox_out(c, g_pLockedVolume, &h_pLockedVolume, hr >= 0);
    c->eax = (uint32_t)hr;
}

static void t_IDirect3DVolumeTexture9_UnlockBox(CPU *c, void *self, uint32_t a) {
    uint32_t g_Level = R32(a + 0);
    uint32_t a_Level = g_Level;
    int32_t hr = ((int32_t (*)(void *, uint32_t))D3D9_VTBL(self)[20])(self, a_Level);
    c->eax = (uint32_t)hr;
}

static void t_IUnknown_AddRef(CPU *c, void *self, uint32_t a) {
    c->eax = ((uint32_t (*)(void *))D3D9_VTBL(self)[1])(self);
    (void)a;
}

// Trié par nom (bsearch) ; slot : rang dans la vtable de l’interface qui définit la méthode.
const D3d9Method rt_d3d9_methods[] = {
    {"IDirect3D9::CheckDepthStencilMatch", t_IDirect3D9_CheckDepthStencilMatch, 0x2u, 12, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3D9::CheckDeviceFormat", t_IDirect3D9_CheckDeviceFormat, 0x2u, 10, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3D9::CheckDeviceFormatConversion", t_IDirect3D9_CheckDeviceFormatConversion, 0x2u, 13, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3D9::CheckDeviceMultiSampleType", t_IDirect3D9_CheckDeviceMultiSampleType, 0x2u, 11, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3D9::CheckDeviceType", t_IDirect3D9_CheckDeviceType, 0x2u, 9, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3D9::CreateDevice", t_IDirect3D9_CreateDevice, 0x2u, 16, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3D9::EnumAdapterModes", t_IDirect3D9_EnumAdapterModes, 0x2u, 7, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3D9::GetAdapterCount", t_IDirect3D9_GetAdapterCount, 0x2u, 4, D3D9_GENERATED, D3D9_RET_U32},
    {"IDirect3D9::GetAdapterDisplayMode", t_IDirect3D9_GetAdapterDisplayMode, 0x2u, 8, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3D9::GetAdapterIdentifier", t_IDirect3D9_GetAdapterIdentifier, 0x2u, 5, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3D9::GetAdapterModeCount", t_IDirect3D9_GetAdapterModeCount, 0x2u, 6, D3D9_GENERATED, D3D9_RET_U32},
    {"IDirect3D9::GetAdapterMonitor", NULL, 0x2u, 15, D3D9_STUB, D3D9_RET_U32},
    {"IDirect3D9::GetDeviceCaps", t_IDirect3D9_GetDeviceCaps, 0x2u, 14, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3D9::RegisterSoftwareDevice", NULL, 0x2u, 3, D3D9_STUB, D3D9_RET_HRESULT},
    {"IDirect3DBaseTexture9::GenerateMipSubLevels", t_IDirect3DBaseTexture9_GenerateMipSubLevels, 0x1c0u, 16, D3D9_GENERATED, D3D9_RET_VOID},
    {"IDirect3DBaseTexture9::GetAutoGenFilterType", t_IDirect3DBaseTexture9_GetAutoGenFilterType, 0x1c0u, 15, D3D9_GENERATED, D3D9_RET_U32},
    {"IDirect3DBaseTexture9::GetLOD", t_IDirect3DBaseTexture9_GetLOD, 0x1c0u, 12, D3D9_GENERATED, D3D9_RET_U32},
    {"IDirect3DBaseTexture9::GetLevelCount", t_IDirect3DBaseTexture9_GetLevelCount, 0x1c0u, 13, D3D9_GENERATED, D3D9_RET_U32},
    {"IDirect3DBaseTexture9::SetAutoGenFilterType", t_IDirect3DBaseTexture9_SetAutoGenFilterType, 0x1c0u, 14, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DBaseTexture9::SetLOD", t_IDirect3DBaseTexture9_SetLOD, 0x1c0u, 11, D3D9_GENERATED, D3D9_RET_U32},
    {"IDirect3DCubeTexture9::AddDirtyRect", t_IDirect3DCubeTexture9_AddDirtyRect, 0x100u, 21, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DCubeTexture9::GetCubeMapSurface", t_IDirect3DCubeTexture9_GetCubeMapSurface, 0x100u, 18, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DCubeTexture9::GetLevelDesc", t_IDirect3DCubeTexture9_GetLevelDesc, 0x100u, 17, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DCubeTexture9::LockRect", t_IDirect3DCubeTexture9_LockRect, 0x100u, 19, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DCubeTexture9::UnlockRect", t_IDirect3DCubeTexture9_UnlockRect, 0x100u, 20, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::BeginScene", t_IDirect3DDevice9_BeginScene, 0x4u, 41, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::BeginStateBlock", t_IDirect3DDevice9_BeginStateBlock, 0x4u, 60, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::Clear", t_IDirect3DDevice9_Clear, 0x4u, 43, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::ColorFill", t_IDirect3DDevice9_ColorFill, 0x4u, 35, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::CreateAdditionalSwapChain", t_IDirect3DDevice9_CreateAdditionalSwapChain, 0x4u, 13, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::CreateCubeTexture", t_IDirect3DDevice9_CreateCubeTexture, 0x4u, 25, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::CreateDepthStencilSurface", t_IDirect3DDevice9_CreateDepthStencilSurface, 0x4u, 29, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::CreateIndexBuffer", t_IDirect3DDevice9_CreateIndexBuffer, 0x4u, 27, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::CreateOffscreenPlainSurface", t_IDirect3DDevice9_CreateOffscreenPlainSurface, 0x4u, 36, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::CreatePixelShader", t_IDirect3DDevice9_CreatePixelShader, 0x4u, 106, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::CreateQuery", t_IDirect3DDevice9_CreateQuery, 0x4u, 118, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::CreateRenderTarget", t_IDirect3DDevice9_CreateRenderTarget, 0x4u, 28, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::CreateStateBlock", t_IDirect3DDevice9_CreateStateBlock, 0x4u, 59, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::CreateTexture", t_IDirect3DDevice9_CreateTexture, 0x4u, 23, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::CreateVertexBuffer", t_IDirect3DDevice9_CreateVertexBuffer, 0x4u, 26, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::CreateVertexDeclaration", t_IDirect3DDevice9_CreateVertexDeclaration, 0x4u, 86, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::CreateVertexShader", t_IDirect3DDevice9_CreateVertexShader, 0x4u, 91, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::CreateVolumeTexture", t_IDirect3DDevice9_CreateVolumeTexture, 0x4u, 24, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::DeletePatch", t_IDirect3DDevice9_DeletePatch, 0x4u, 117, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::DrawIndexedPrimitive", t_IDirect3DDevice9_DrawIndexedPrimitive, 0x4u, 82, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::DrawIndexedPrimitiveUP", t_IDirect3DDevice9_DrawIndexedPrimitiveUP, 0x4u, 84, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::DrawPrimitive", t_IDirect3DDevice9_DrawPrimitive, 0x4u, 81, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::DrawPrimitiveUP", t_IDirect3DDevice9_DrawPrimitiveUP, 0x4u, 83, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::DrawRectPatch", NULL, 0x4u, 115, D3D9_STUB, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::DrawTriPatch", NULL, 0x4u, 116, D3D9_STUB, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::EndScene", t_IDirect3DDevice9_EndScene, 0x4u, 42, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::EndStateBlock", t_IDirect3DDevice9_EndStateBlock, 0x4u, 61, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::EvictManagedResources", t_IDirect3DDevice9_EvictManagedResources, 0x4u, 5, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::GetAvailableTextureMem", t_IDirect3DDevice9_GetAvailableTextureMem, 0x4u, 4, D3D9_GENERATED, D3D9_RET_U32},
    {"IDirect3DDevice9::GetBackBuffer", t_IDirect3DDevice9_GetBackBuffer, 0x4u, 18, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::GetClipPlane", t_IDirect3DDevice9_GetClipPlane, 0x4u, 56, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::GetClipStatus", t_IDirect3DDevice9_GetClipStatus, 0x4u, 63, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::GetCreationParameters", t_IDirect3DDevice9_GetCreationParameters, 0x4u, 9, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::GetCurrentTexturePalette", t_IDirect3DDevice9_GetCurrentTexturePalette, 0x4u, 74, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::GetDepthStencilSurface", t_IDirect3DDevice9_GetDepthStencilSurface, 0x4u, 40, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::GetDeviceCaps", t_IDirect3DDevice9_GetDeviceCaps, 0x4u, 7, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::GetDirect3D", t_IDirect3DDevice9_GetDirect3D, 0x4u, 6, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::GetDisplayMode", t_IDirect3DDevice9_GetDisplayMode, 0x4u, 8, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::GetFVF", t_IDirect3DDevice9_GetFVF, 0x4u, 90, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::GetFrontBufferData", t_IDirect3DDevice9_GetFrontBufferData, 0x4u, 33, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::GetGammaRamp", t_IDirect3DDevice9_GetGammaRamp, 0x4u, 22, D3D9_GENERATED, D3D9_RET_VOID},
    {"IDirect3DDevice9::GetIndices", t_IDirect3DDevice9_GetIndices, 0x4u, 105, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::GetLight", t_IDirect3DDevice9_GetLight, 0x4u, 52, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::GetLightEnable", t_IDirect3DDevice9_GetLightEnable, 0x4u, 54, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::GetMaterial", t_IDirect3DDevice9_GetMaterial, 0x4u, 50, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::GetNPatchMode", t_IDirect3DDevice9_GetNPatchMode, 0x4u, 80, D3D9_GENERATED, D3D9_RET_FLOAT},
    {"IDirect3DDevice9::GetNumberOfSwapChains", t_IDirect3DDevice9_GetNumberOfSwapChains, 0x4u, 15, D3D9_GENERATED, D3D9_RET_U32},
    {"IDirect3DDevice9::GetPaletteEntries", t_IDirect3DDevice9_GetPaletteEntries, 0x4u, 72, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::GetPixelShader", t_IDirect3DDevice9_GetPixelShader, 0x4u, 108, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::GetPixelShaderConstantB", t_IDirect3DDevice9_GetPixelShaderConstantB, 0x4u, 114, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::GetPixelShaderConstantF", t_IDirect3DDevice9_GetPixelShaderConstantF, 0x4u, 110, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::GetPixelShaderConstantI", t_IDirect3DDevice9_GetPixelShaderConstantI, 0x4u, 112, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::GetRasterStatus", t_IDirect3DDevice9_GetRasterStatus, 0x4u, 19, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::GetRenderState", t_IDirect3DDevice9_GetRenderState, 0x4u, 58, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::GetRenderTarget", t_IDirect3DDevice9_GetRenderTarget, 0x4u, 38, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::GetRenderTargetData", t_IDirect3DDevice9_GetRenderTargetData, 0x4u, 32, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::GetSamplerState", t_IDirect3DDevice9_GetSamplerState, 0x4u, 68, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::GetScissorRect", t_IDirect3DDevice9_GetScissorRect, 0x4u, 76, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::GetSoftwareVertexProcessing", t_IDirect3DDevice9_GetSoftwareVertexProcessing, 0x4u, 78, D3D9_GENERATED, D3D9_RET_U32},
    {"IDirect3DDevice9::GetStreamSource", t_IDirect3DDevice9_GetStreamSource, 0x4u, 101, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::GetStreamSourceFreq", t_IDirect3DDevice9_GetStreamSourceFreq, 0x4u, 103, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::GetSwapChain", t_IDirect3DDevice9_GetSwapChain, 0x4u, 14, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::GetTexture", t_IDirect3DDevice9_GetTexture, 0x4u, 64, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::GetTextureStageState", t_IDirect3DDevice9_GetTextureStageState, 0x4u, 66, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::GetTransform", t_IDirect3DDevice9_GetTransform, 0x4u, 45, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::GetVertexDeclaration", t_IDirect3DDevice9_GetVertexDeclaration, 0x4u, 88, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::GetVertexShader", t_IDirect3DDevice9_GetVertexShader, 0x4u, 93, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::GetVertexShaderConstantB", t_IDirect3DDevice9_GetVertexShaderConstantB, 0x4u, 99, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::GetVertexShaderConstantF", t_IDirect3DDevice9_GetVertexShaderConstantF, 0x4u, 95, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::GetVertexShaderConstantI", t_IDirect3DDevice9_GetVertexShaderConstantI, 0x4u, 97, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::GetViewport", t_IDirect3DDevice9_GetViewport, 0x4u, 48, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::LightEnable", t_IDirect3DDevice9_LightEnable, 0x4u, 53, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::MultiplyTransform", t_IDirect3DDevice9_MultiplyTransform, 0x4u, 46, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::Present", t_IDirect3DDevice9_Present, 0x4u, 17, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::ProcessVertices", t_IDirect3DDevice9_ProcessVertices, 0x4u, 85, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::Reset", t_IDirect3DDevice9_Reset, 0x4u, 16, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::SetClipPlane", t_IDirect3DDevice9_SetClipPlane, 0x4u, 55, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::SetClipStatus", t_IDirect3DDevice9_SetClipStatus, 0x4u, 62, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::SetCurrentTexturePalette", t_IDirect3DDevice9_SetCurrentTexturePalette, 0x4u, 73, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::SetCursorPosition", t_IDirect3DDevice9_SetCursorPosition, 0x4u, 11, D3D9_GENERATED, D3D9_RET_VOID},
    {"IDirect3DDevice9::SetCursorProperties", t_IDirect3DDevice9_SetCursorProperties, 0x4u, 10, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::SetDepthStencilSurface", t_IDirect3DDevice9_SetDepthStencilSurface, 0x4u, 39, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::SetDialogBoxMode", t_IDirect3DDevice9_SetDialogBoxMode, 0x4u, 20, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::SetFVF", t_IDirect3DDevice9_SetFVF, 0x4u, 89, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::SetGammaRamp", t_IDirect3DDevice9_SetGammaRamp, 0x4u, 21, D3D9_GENERATED, D3D9_RET_VOID},
    {"IDirect3DDevice9::SetIndices", t_IDirect3DDevice9_SetIndices, 0x4u, 104, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::SetLight", t_IDirect3DDevice9_SetLight, 0x4u, 51, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::SetMaterial", t_IDirect3DDevice9_SetMaterial, 0x4u, 49, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::SetNPatchMode", t_IDirect3DDevice9_SetNPatchMode, 0x4u, 79, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::SetPaletteEntries", t_IDirect3DDevice9_SetPaletteEntries, 0x4u, 71, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::SetPixelShader", t_IDirect3DDevice9_SetPixelShader, 0x4u, 107, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::SetPixelShaderConstantB", t_IDirect3DDevice9_SetPixelShaderConstantB, 0x4u, 113, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::SetPixelShaderConstantF", t_IDirect3DDevice9_SetPixelShaderConstantF, 0x4u, 109, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::SetPixelShaderConstantI", t_IDirect3DDevice9_SetPixelShaderConstantI, 0x4u, 111, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::SetRenderState", t_IDirect3DDevice9_SetRenderState, 0x4u, 57, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::SetRenderTarget", t_IDirect3DDevice9_SetRenderTarget, 0x4u, 37, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::SetSamplerState", t_IDirect3DDevice9_SetSamplerState, 0x4u, 69, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::SetScissorRect", t_IDirect3DDevice9_SetScissorRect, 0x4u, 75, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::SetSoftwareVertexProcessing", t_IDirect3DDevice9_SetSoftwareVertexProcessing, 0x4u, 77, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::SetStreamSource", t_IDirect3DDevice9_SetStreamSource, 0x4u, 100, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::SetStreamSourceFreq", t_IDirect3DDevice9_SetStreamSourceFreq, 0x4u, 102, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::SetTexture", t_IDirect3DDevice9_SetTexture, 0x4u, 65, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::SetTextureStageState", t_IDirect3DDevice9_SetTextureStageState, 0x4u, 67, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::SetTransform", t_IDirect3DDevice9_SetTransform, 0x4u, 44, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::SetVertexDeclaration", t_IDirect3DDevice9_SetVertexDeclaration, 0x4u, 87, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::SetVertexShader", t_IDirect3DDevice9_SetVertexShader, 0x4u, 92, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::SetVertexShaderConstantB", t_IDirect3DDevice9_SetVertexShaderConstantB, 0x4u, 98, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::SetVertexShaderConstantF", t_IDirect3DDevice9_SetVertexShaderConstantF, 0x4u, 94, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::SetVertexShaderConstantI", t_IDirect3DDevice9_SetVertexShaderConstantI, 0x4u, 96, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::SetViewport", t_IDirect3DDevice9_SetViewport, 0x4u, 47, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::ShowCursor", t_IDirect3DDevice9_ShowCursor, 0x4u, 12, D3D9_GENERATED, D3D9_RET_U32},
    {"IDirect3DDevice9::StretchRect", t_IDirect3DDevice9_StretchRect, 0x4u, 34, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::TestCooperativeLevel", t_IDirect3DDevice9_TestCooperativeLevel, 0x4u, 3, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::UpdateSurface", t_IDirect3DDevice9_UpdateSurface, 0x4u, 30, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::UpdateTexture", t_IDirect3DDevice9_UpdateTexture, 0x4u, 31, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DDevice9::ValidateDevice", t_IDirect3DDevice9_ValidateDevice, 0x4u, 70, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DIndexBuffer9::GetDesc", t_IDirect3DIndexBuffer9_GetDesc, 0x400u, 13, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DIndexBuffer9::Lock", t_IDirect3DIndexBuffer9_Lock, 0x400u, 11, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DIndexBuffer9::Unlock", t_IDirect3DIndexBuffer9_Unlock, 0x400u, 12, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DPixelShader9::GetDevice", t_IDirect3DPixelShader9_GetDevice, 0x8000u, 3, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DPixelShader9::GetFunction", t_IDirect3DPixelShader9_GetFunction, 0x8000u, 4, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DQuery9::GetData", t_IDirect3DQuery9_GetData, 0x20000u, 7, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DQuery9::GetDataSize", t_IDirect3DQuery9_GetDataSize, 0x20000u, 5, D3D9_GENERATED, D3D9_RET_U32},
    {"IDirect3DQuery9::GetDevice", t_IDirect3DQuery9_GetDevice, 0x20000u, 3, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DQuery9::GetType", t_IDirect3DQuery9_GetType, 0x20000u, 4, D3D9_GENERATED, D3D9_RET_U32},
    {"IDirect3DQuery9::Issue", t_IDirect3DQuery9_Issue, 0x20000u, 6, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DResource9::FreePrivateData", t_IDirect3DResource9_FreePrivateData, 0xfc0u, 6, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DResource9::GetDevice", t_IDirect3DResource9_GetDevice, 0xfc0u, 3, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DResource9::GetPriority", t_IDirect3DResource9_GetPriority, 0xfc0u, 8, D3D9_GENERATED, D3D9_RET_U32},
    {"IDirect3DResource9::GetPrivateData", t_IDirect3DResource9_GetPrivateData, 0xfc0u, 5, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DResource9::GetType", t_IDirect3DResource9_GetType, 0xfc0u, 10, D3D9_GENERATED, D3D9_RET_U32},
    {"IDirect3DResource9::PreLoad", t_IDirect3DResource9_PreLoad, 0xfc0u, 9, D3D9_GENERATED, D3D9_RET_VOID},
    {"IDirect3DResource9::SetPriority", t_IDirect3DResource9_SetPriority, 0xfc0u, 7, D3D9_GENERATED, D3D9_RET_U32},
    {"IDirect3DResource9::SetPrivateData", d3d9_manual_IDirect3DResource9_SetPrivateData, 0xfc0u, 4, D3D9_MANUAL, D3D9_RET_HRESULT},
    {"IDirect3DStateBlock9::Apply", t_IDirect3DStateBlock9_Apply, 0x10000u, 5, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DStateBlock9::Capture", t_IDirect3DStateBlock9_Capture, 0x10000u, 4, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DStateBlock9::GetDevice", t_IDirect3DStateBlock9_GetDevice, 0x10000u, 3, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DSurface9::GetContainer", d3d9_manual_IDirect3DSurface9_GetContainer, 0x800u, 11, D3D9_MANUAL, D3D9_RET_HRESULT},
    {"IDirect3DSurface9::GetDC", NULL, 0x800u, 15, D3D9_STUB, D3D9_RET_HRESULT},
    {"IDirect3DSurface9::GetDesc", t_IDirect3DSurface9_GetDesc, 0x800u, 12, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DSurface9::LockRect", t_IDirect3DSurface9_LockRect, 0x800u, 13, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DSurface9::ReleaseDC", NULL, 0x800u, 16, D3D9_STUB, D3D9_RET_HRESULT},
    {"IDirect3DSurface9::UnlockRect", t_IDirect3DSurface9_UnlockRect, 0x800u, 14, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DSwapChain9::GetBackBuffer", t_IDirect3DSwapChain9_GetBackBuffer, 0x8u, 5, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DSwapChain9::GetDevice", t_IDirect3DSwapChain9_GetDevice, 0x8u, 8, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DSwapChain9::GetDisplayMode", t_IDirect3DSwapChain9_GetDisplayMode, 0x8u, 7, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DSwapChain9::GetFrontBufferData", t_IDirect3DSwapChain9_GetFrontBufferData, 0x8u, 4, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DSwapChain9::GetPresentParameters", t_IDirect3DSwapChain9_GetPresentParameters, 0x8u, 9, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DSwapChain9::GetRasterStatus", t_IDirect3DSwapChain9_GetRasterStatus, 0x8u, 6, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DSwapChain9::Present", t_IDirect3DSwapChain9_Present, 0x8u, 3, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DTexture9::AddDirtyRect", t_IDirect3DTexture9_AddDirtyRect, 0x40u, 21, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DTexture9::GetLevelDesc", t_IDirect3DTexture9_GetLevelDesc, 0x40u, 17, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DTexture9::GetSurfaceLevel", t_IDirect3DTexture9_GetSurfaceLevel, 0x40u, 18, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DTexture9::LockRect", t_IDirect3DTexture9_LockRect, 0x40u, 19, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DTexture9::UnlockRect", t_IDirect3DTexture9_UnlockRect, 0x40u, 20, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DVertexBuffer9::GetDesc", t_IDirect3DVertexBuffer9_GetDesc, 0x200u, 13, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DVertexBuffer9::Lock", t_IDirect3DVertexBuffer9_Lock, 0x200u, 11, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DVertexBuffer9::Unlock", t_IDirect3DVertexBuffer9_Unlock, 0x200u, 12, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DVertexDeclaration9::GetDeclaration", d3d9_manual_IDirect3DVertexDeclaration9_GetDeclaration, 0x2000u, 4, D3D9_MANUAL, D3D9_RET_HRESULT},
    {"IDirect3DVertexDeclaration9::GetDevice", t_IDirect3DVertexDeclaration9_GetDevice, 0x2000u, 3, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DVertexShader9::GetDevice", t_IDirect3DVertexShader9_GetDevice, 0x4000u, 3, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DVertexShader9::GetFunction", t_IDirect3DVertexShader9_GetFunction, 0x4000u, 4, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DVolume9::FreePrivateData", t_IDirect3DVolume9_FreePrivateData, 0x1000u, 6, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DVolume9::GetContainer", d3d9_manual_IDirect3DVolume9_GetContainer, 0x1000u, 7, D3D9_MANUAL, D3D9_RET_HRESULT},
    {"IDirect3DVolume9::GetDesc", t_IDirect3DVolume9_GetDesc, 0x1000u, 8, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DVolume9::GetDevice", t_IDirect3DVolume9_GetDevice, 0x1000u, 3, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DVolume9::GetPrivateData", t_IDirect3DVolume9_GetPrivateData, 0x1000u, 5, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DVolume9::LockBox", t_IDirect3DVolume9_LockBox, 0x1000u, 9, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DVolume9::SetPrivateData", d3d9_manual_IDirect3DVolume9_SetPrivateData, 0x1000u, 4, D3D9_MANUAL, D3D9_RET_HRESULT},
    {"IDirect3DVolume9::UnlockBox", t_IDirect3DVolume9_UnlockBox, 0x1000u, 10, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DVolumeTexture9::AddDirtyBox", t_IDirect3DVolumeTexture9_AddDirtyBox, 0x80u, 21, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DVolumeTexture9::GetLevelDesc", t_IDirect3DVolumeTexture9_GetLevelDesc, 0x80u, 17, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DVolumeTexture9::GetVolumeLevel", t_IDirect3DVolumeTexture9_GetVolumeLevel, 0x80u, 18, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DVolumeTexture9::LockBox", t_IDirect3DVolumeTexture9_LockBox, 0x80u, 19, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IDirect3DVolumeTexture9::UnlockBox", t_IDirect3DVolumeTexture9_UnlockBox, 0x80u, 20, D3D9_GENERATED, D3D9_RET_HRESULT},
    {"IUnknown::AddRef", t_IUnknown_AddRef, 0x3ffceu, 1, D3D9_GENERATED, D3D9_RET_U32},
    {"IUnknown::QueryInterface", d3d9_manual_IUnknown_QueryInterface, 0x3ffceu, 0, D3D9_MANUAL, D3D9_RET_HRESULT},
    {"IUnknown::Release", d3d9_manual_IUnknown_Release, 0x3ffceu, 2, D3D9_MANUAL, D3D9_RET_U32},
};
const unsigned rt_d3d9_method_count = 203;

static const uint16_t slots_IDirect3D9[] = {201, 200, 202, 13, 7, 9, 10, 6, 8, 4, 1, 3, 0, 2, 12, 11, 5};
static const uint16_t slots_IDirect3DDevice9[] = {201, 200, 202, 137, 53, 52, 61, 60, 62, 57, 105, 104, 135, 29, 86, 71, 100, 98, 54, 77, 107, 109, 65, 38, 42, 30, 39, 32, 36, 31, 138, 139, 80, 64, 136, 28, 33, 120, 79, 106, 59, 25, 50, 27, 128, 89, 97, 134, 95, 112, 69, 111, 67, 96, 68, 101, 55, 119, 78, 37, 26, 51, 102, 56, 87, 126, 88, 127, 81, 121, 140, 114, 72, 103, 58, 122, 82, 123, 83, 113, 70, 46, 44, 47, 45, 99, 40, 129, 90, 108, 63, 41, 130, 91, 132, 93, 133, 94, 131, 92, 124, 84, 125, 85, 110, 66, 34, 115, 73, 117, 75, 118, 76, 116, 74, 48, 49, 43, 35};
static const uint16_t slots_IDirect3DSwapChain9[] = {201, 200, 202, 174, 171, 168, 173, 170, 169, 172};
static const uint16_t slots_IDirect3DTexture9[] = {201, 200, 202, 152, 158, 154, 151, 157, 153, 156, 155, 19, 16, 17, 18, 15, 14, 176, 177, 178, 179, 175};
static const uint16_t slots_IDirect3DVolumeTexture9[] = {201, 200, 202, 152, 158, 154, 151, 157, 153, 156, 155, 19, 16, 17, 18, 15, 14, 196, 197, 198, 199, 195};
static const uint16_t slots_IDirect3DCubeTexture9[] = {201, 200, 202, 152, 158, 154, 151, 157, 153, 156, 155, 19, 16, 17, 18, 15, 14, 22, 21, 23, 24, 20};
static const uint16_t slots_IDirect3DVertexBuffer9[] = {201, 200, 202, 152, 158, 154, 151, 157, 153, 156, 155, 181, 182, 180};
static const uint16_t slots_IDirect3DIndexBuffer9[] = {201, 200, 202, 152, 158, 154, 151, 157, 153, 156, 155, 142, 143, 141};
static const uint16_t slots_IDirect3DSurface9[] = {201, 200, 202, 152, 158, 154, 151, 157, 153, 156, 155, 162, 164, 165, 167, 163, 166};
static const uint16_t slots_IDirect3DVolume9[] = {201, 200, 202, 190, 193, 191, 187, 188, 189, 192, 194};
static const uint16_t slots_IDirect3DVertexDeclaration9[] = {201, 200, 202, 184, 183};
static const uint16_t slots_IDirect3DVertexShader9[] = {201, 200, 202, 185, 186};
static const uint16_t slots_IDirect3DPixelShader9[] = {201, 200, 202, 144, 145};
static const uint16_t slots_IDirect3DStateBlock9[] = {201, 200, 202, 161, 160, 159};
static const uint16_t slots_IDirect3DQuery9[] = {201, 200, 202, 148, 149, 147, 150, 146};

const D3d9Interface rt_d3d9_interfaces[] = {
    {"IUnknown", -1, 1, 0x3ffceu, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}, NULL, 0},
    {"IDirect3D9", D3D9_IUnknown, 0, 0x2u, {0xca, 0xcb, 0xbd, 0x81, 0xd4, 0x64, 0x6d, 0x42, 0xae, 0x8d, 0xad, 0x01, 0x47, 0xf4, 0x27, 0x5c}, slots_IDirect3D9, 17},
    {"IDirect3DDevice9", D3D9_IUnknown, 0, 0x4u, {0x96, 0x3b, 0x22, 0xd0, 0x7a, 0xbf, 0xfd, 0x43, 0x92, 0xbd, 0xa4, 0x3b, 0x0d, 0x82, 0xb9, 0xeb}, slots_IDirect3DDevice9, 119},
    {"IDirect3DSwapChain9", D3D9_IUnknown, 0, 0x8u, {0xf2, 0x50, 0x49, 0x79, 0xfc, 0xad, 0x8a, 0x45, 0x90, 0x5e, 0x10, 0xa1, 0x0b, 0x0b, 0x50, 0x3b}, slots_IDirect3DSwapChain9, 10},
    {"IDirect3DResource9", D3D9_IUnknown, 1, 0xfc0u, {0x5d, 0xc0, 0xee, 0x05, 0x7d, 0x8f, 0x62, 0x43, 0xb9, 0x99, 0xd1, 0xba, 0xf3, 0x57, 0xc7, 0x04}, NULL, 0},
    {"IDirect3DBaseTexture9", D3D9_IDirect3DResource9, 1, 0x1c0u, {0x7e, 0xa8, 0x0c, 0x58, 0x3c, 0x1d, 0x54, 0x4d, 0x99, 0x1d, 0xb7, 0xd3, 0xe3, 0xc2, 0x98, 0xce}, NULL, 0},
    {"IDirect3DTexture9", D3D9_IDirect3DBaseTexture9, 0, 0x40u, {0x27, 0x12, 0xc3, 0x85, 0xe5, 0x3d, 0x00, 0x4f, 0x9b, 0x3a, 0xf1, 0x1a, 0xc3, 0x8c, 0x18, 0xb5}, slots_IDirect3DTexture9, 22},
    {"IDirect3DVolumeTexture9", D3D9_IDirect3DBaseTexture9, 0, 0x80u, {0x6c, 0x52, 0x18, 0x25, 0x89, 0xe7, 0x11, 0x41, 0xa7, 0xb9, 0x47, 0xef, 0x32, 0x8d, 0x13, 0xe6}, slots_IDirect3DVolumeTexture9, 22},
    {"IDirect3DCubeTexture9", D3D9_IDirect3DBaseTexture9, 0, 0x100u, {0x81, 0x2f, 0xf3, 0xff, 0x53, 0xd9, 0x3a, 0x47, 0x92, 0x23, 0x93, 0xd6, 0x52, 0xab, 0xa9, 0x3f}, slots_IDirect3DCubeTexture9, 22},
    {"IDirect3DVertexBuffer9", D3D9_IDirect3DResource9, 0, 0x200u, {0xb5, 0xb1, 0x4b, 0xb6, 0x70, 0xfd, 0xf6, 0x4d, 0xbf, 0x91, 0x19, 0xd0, 0xa1, 0x24, 0x55, 0xe3}, slots_IDirect3DVertexBuffer9, 14},
    {"IDirect3DIndexBuffer9", D3D9_IDirect3DResource9, 0, 0x400u, {0x5e, 0xd6, 0x9d, 0x7c, 0xf7, 0xd3, 0x29, 0x45, 0xac, 0xee, 0x78, 0x58, 0x30, 0xac, 0xde, 0x35}, slots_IDirect3DIndexBuffer9, 14},
    {"IDirect3DSurface9", D3D9_IDirect3DResource9, 0, 0x800u, {0x3a, 0xaf, 0xfb, 0x0c, 0xf6, 0x9f, 0x9a, 0x42, 0x99, 0xb3, 0xa2, 0x79, 0x6a, 0xf8, 0xb8, 0x9b}, slots_IDirect3DSurface9, 17},
    {"IDirect3DVolume9", D3D9_IUnknown, 0, 0x1000u, {0xe6, 0x16, 0xf4, 0x24, 0x67, 0x1f, 0xa7, 0x4a, 0xb8, 0x8e, 0xd3, 0x3f, 0x6f, 0x31, 0x28, 0xa1}, slots_IDirect3DVolume9, 11},
    {"IDirect3DVertexDeclaration9", D3D9_IUnknown, 0, 0x2000u, {0x9c, 0xc5, 0x13, 0xdd, 0xfa, 0x36, 0x98, 0x40, 0xa8, 0xfb, 0xc7, 0xed, 0x39, 0xdc, 0x85, 0x46}, slots_IDirect3DVertexDeclaration9, 5},
    {"IDirect3DVertexShader9", D3D9_IUnknown, 0, 0x4000u, {0x7e, 0x55, 0xc5, 0xef, 0x65, 0x62, 0x13, 0x46, 0x8a, 0x94, 0x43, 0x85, 0x78, 0x89, 0xeb, 0x36}, slots_IDirect3DVertexShader9, 5},
    {"IDirect3DPixelShader9", D3D9_IUnknown, 0, 0x8000u, {0xdc, 0xdb, 0x3b, 0x6d, 0x02, 0x5b, 0x15, 0x44, 0xb8, 0x52, 0xce, 0x5e, 0x8b, 0xcc, 0xb2, 0x89}, slots_IDirect3DPixelShader9, 5},
    {"IDirect3DStateBlock9", D3D9_IUnknown, 0, 0x10000u, {0xe5, 0x4f, 0x7c, 0xb0, 0x0d, 0x31, 0xa8, 0x4b, 0xa2, 0x3c, 0x4f, 0x0f, 0x20, 0x6f, 0x21, 0x8b}, slots_IDirect3DStateBlock9, 6},
    {"IDirect3DQuery9", D3D9_IUnknown, 0, 0x20000u, {0x60, 0x14, 0x77, 0xd9, 0x95, 0xa6, 0x26, 0x4f, 0xbb, 0xd3, 0x27, 0xb8, 0x40, 0xb5, 0x41, 0xcc}, slots_IDirect3DQuery9, 8},
};
const unsigned rt_d3d9_interface_count = 18;
