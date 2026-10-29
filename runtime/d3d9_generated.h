// GÉNÉRÉ par tools/gen_d3d9.py — ne pas éditer à la main.
// Identifiants des interfaces Direct3D 9 du pont (runtime/d3d9.c) et
// prototypes des méthodes écrites à la main.
#pragma once
#include "rt.h"

enum {
    D3D9_IUnknown = 0,
    D3D9_IDirect3D9 = 1,
    D3D9_IDirect3DDevice9 = 2,
    D3D9_IDirect3DSwapChain9 = 3,
    D3D9_IDirect3DResource9 = 4,
    D3D9_IDirect3DBaseTexture9 = 5,
    D3D9_IDirect3DTexture9 = 6,
    D3D9_IDirect3DVolumeTexture9 = 7,
    D3D9_IDirect3DCubeTexture9 = 8,
    D3D9_IDirect3DVertexBuffer9 = 9,
    D3D9_IDirect3DIndexBuffer9 = 10,
    D3D9_IDirect3DSurface9 = 11,
    D3D9_IDirect3DVolume9 = 12,
    D3D9_IDirect3DVertexDeclaration9 = 13,
    D3D9_IDirect3DVertexShader9 = 14,
    D3D9_IDirect3DPixelShader9 = 15,
    D3D9_IDirect3DStateBlock9 = 16,
    D3D9_IDirect3DQuery9 = 17,
    D3D9_INTERFACE_COUNT = 18,
};
#define D3D9_MASK_IUnknown 0x3ffceu
#define D3D9_MASK_IDirect3D9 0x2u
#define D3D9_MASK_IDirect3DDevice9 0x4u
#define D3D9_MASK_IDirect3DSwapChain9 0x8u
#define D3D9_MASK_IDirect3DResource9 0xfc0u
#define D3D9_MASK_IDirect3DBaseTexture9 0x1c0u
#define D3D9_MASK_IDirect3DTexture9 0x40u
#define D3D9_MASK_IDirect3DVolumeTexture9 0x80u
#define D3D9_MASK_IDirect3DCubeTexture9 0x100u
#define D3D9_MASK_IDirect3DVertexBuffer9 0x200u
#define D3D9_MASK_IDirect3DIndexBuffer9 0x400u
#define D3D9_MASK_IDirect3DSurface9 0x800u
#define D3D9_MASK_IDirect3DVolume9 0x1000u
#define D3D9_MASK_IDirect3DVertexDeclaration9 0x2000u
#define D3D9_MASK_IDirect3DVertexShader9 0x4000u
#define D3D9_MASK_IDirect3DPixelShader9 0x8000u
#define D3D9_MASK_IDirect3DStateBlock9 0x10000u
#define D3D9_MASK_IDirect3DQuery9 0x20000u

#define D3D9_SLOT_IDirect3DResource9_SetPrivateData 4
void d3d9_manual_IDirect3DResource9_SetPrivateData(CPU *c, void *self, uint32_t a);
#define D3D9_SLOT_IDirect3DSurface9_GetContainer 11
void d3d9_manual_IDirect3DSurface9_GetContainer(CPU *c, void *self, uint32_t a);
#define D3D9_SLOT_IDirect3DVertexDeclaration9_GetDeclaration 4
void d3d9_manual_IDirect3DVertexDeclaration9_GetDeclaration(CPU *c, void *self, uint32_t a);
#define D3D9_SLOT_IDirect3DVolume9_GetContainer 7
void d3d9_manual_IDirect3DVolume9_GetContainer(CPU *c, void *self, uint32_t a);
#define D3D9_SLOT_IDirect3DVolume9_SetPrivateData 4
void d3d9_manual_IDirect3DVolume9_SetPrivateData(CPU *c, void *self, uint32_t a);
#define D3D9_SLOT_IUnknown_QueryInterface 0
void d3d9_manual_IUnknown_QueryInterface(CPU *c, void *self, uint32_t a);
#define D3D9_SLOT_IUnknown_Release 2
void d3d9_manual_IUnknown_Release(CPU *c, void *self, uint32_t a);
