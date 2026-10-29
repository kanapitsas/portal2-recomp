#!/usr/bin/env python3
"""Génère runtime/d3d9_generated.{c,h} : pont COM Direct3D 9 invité i386 → hôte.

shaderapivk.so (Linux i386) appelle Direct3D 9 par des vtables COM, comme le
ferait DXVK-native : cdecl, `this` en premier sur la pile, l'appelant dépile.
Le runtime lui donne des mandataires invités {vtable invitée, étiquette, u64
objet hôte} ; chaque emplacement de vtable est une adresse
rt_dynamic_import("Interface::Méthode") dont la route mène au thunk généré
ici, qui lit les arguments i386, traduit les pointeurs selon une règle
explicite et appelle la méthode de l'objet hôte (ABI arm64, `this` en premier).

La table SIGNATURES décrit toutes les interfaces dans l'ordre de d3d9.h.
Types d'argument (un mot de pile chacun, d3d9 n'a aucun argument 64 bits) :

  u, i, f            scalaire 32 bits (UINT/DWORD/enum/BOOL/D3DCOLOR, INT, float)
  in:EXPR            pointeur invité lu en place (même disposition i386/arm64),
                     EXPR octets
  out:EXPR           idem, écrit par l'hôte
  if:IFACE           interface en entrée (mandataire → objet hôte)
  pp:IFACE           interface en sortie (objet hôte → mandataire unique)
  child:IFACE        idem, niveau de texture partageant le compte de this :
                     le groupe entier est retiré quand le compte tombe à 0
  owned:IFACE        idem, objet qui dépend de this sans partager son compte
                     (tampon arrière) : retiré avec this
  hwnd               HWND : handle SDL_Window invité → SDL_Window* hôte
  shared             HANDLE *pSharedHandle (doit être NULL en d3d9 non Ex)
  params, params_out D3DPRESENT_PARAMETERS (56 octets i386, 64 arm64) entrée/sortie, sortie
  adapterid          D3DADAPTER_IDENTIFIER9 en sortie (1100 / 1104)
  creation           D3DDEVICE_CREATION_PARAMETERS en sortie (16 / 24)
  lockptr            void ** de Lock : pointeur hôte → adresse invitée
  lockrect, lockbox  D3DLOCKED_RECT / D3DLOCKED_BOX en sortie (pBits traduit)
  shader             jetons de shader (taille lue jusqu'au jeton de fin)
  decl               D3DVERTEXELEMENT9[] (jusqu'à D3DDECL_END)
  rgndata            RGNDATA *

Un pointeur invité nul devient NULL côté hôte, quel que soit le type : la
validation des arguments obligatoires reste celle de l'implémentation hôte.

EXPR est une expression C sur les autres paramètres scalaires (par leur nom) ;
D(p) lit le DWORD invité pointé par le paramètre pointeur p (0 si NULL).
Suffixes de méthode : !manual (écrite dans runtime/d3d9.c, nommée
d3d9_manual_<Interface>_<Méthode>), !stub (E_NOTIMPL ou 0, journalisé une fois).

    python3 tools/gen_d3d9.py            # écrit runtime/d3d9_generated.{c,h}
    python3 tools/gen_d3d9.py --check    # vérifie qu'ils sont à jour
"""
import argparse
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent

SIGNATURES = r'''
interface IUnknown abstract {00000000-0000-0000-C000-000000000046}
    HRESULT QueryInterface(in:16 riid, u ppvObject) !manual
    ULONG AddRef()
    ULONG Release() !manual

interface IDirect3D9 : IUnknown {81BDCBCA-64D4-426D-AE8D-AD0147F4275C}
    HRESULT RegisterSoftwareDevice(u pInitializeFunction) !stub
    UINT GetAdapterCount()
    HRESULT GetAdapterIdentifier(u Adapter, u Flags, adapterid pIdentifier)
    UINT GetAdapterModeCount(u Adapter, u Format)
    HRESULT EnumAdapterModes(u Adapter, u Format, u Mode, out:16 pMode)
    HRESULT GetAdapterDisplayMode(u Adapter, out:16 pMode)
    HRESULT CheckDeviceType(u Adapter, u DevType, u AdapterFormat, u BackBufferFormat, u bWindowed)
    HRESULT CheckDeviceFormat(u Adapter, u DeviceType, u AdapterFormat, u Usage, u RType, u CheckFormat)
    HRESULT CheckDeviceMultiSampleType(u Adapter, u DeviceType, u SurfaceFormat, u Windowed, u MultiSampleType, out:4 pQualityLevels)
    HRESULT CheckDepthStencilMatch(u Adapter, u DeviceType, u AdapterFormat, u RenderTargetFormat, u DepthStencilFormat)
    HRESULT CheckDeviceFormatConversion(u Adapter, u DeviceType, u SourceFormat, u TargetFormat)
    HRESULT GetDeviceCaps(u Adapter, u DeviceType, out:304 pCaps)
    HMONITOR GetAdapterMonitor(u Adapter) !stub
    HRESULT CreateDevice(u Adapter, u DeviceType, hwnd hFocusWindow, u BehaviorFlags, params pPresentationParameters, pp:IDirect3DDevice9 ppReturnedDeviceInterface)

interface IDirect3DDevice9 : IUnknown {D0223B96-BF7A-43FD-92BD-A43B0D82B9EB}
    HRESULT TestCooperativeLevel()
    UINT GetAvailableTextureMem()
    HRESULT EvictManagedResources()
    HRESULT GetDirect3D(pp:IDirect3D9 ppD3D9)
    HRESULT GetDeviceCaps(out:304 pCaps)
    HRESULT GetDisplayMode(u iSwapChain, out:16 pMode)
    HRESULT GetCreationParameters(creation pParameters)
    HRESULT SetCursorProperties(u XHotSpot, u YHotSpot, if:IDirect3DSurface9 pCursorBitmap)
    void SetCursorPosition(i X, i Y, u Flags)
    BOOL ShowCursor(u bShow)
    HRESULT CreateAdditionalSwapChain(params pPresentationParameters, pp:IDirect3DSwapChain9 pSwapChain)
    HRESULT GetSwapChain(u iSwapChain, pp:IDirect3DSwapChain9 pSwapChain)
    UINT GetNumberOfSwapChains()
    HRESULT Reset(params pPresentationParameters)
    HRESULT Present(in:16 pSourceRect, in:16 pDestRect, hwnd hDestWindowOverride, rgndata pDirtyRegion)
    HRESULT GetBackBuffer(u iSwapChain, u iBackBuffer, u Type, owned:IDirect3DSurface9 ppBackBuffer)
    HRESULT GetRasterStatus(u iSwapChain, out:8 pRasterStatus)
    HRESULT SetDialogBoxMode(u bEnableDialogs)
    void SetGammaRamp(u iSwapChain, u Flags, in:1536 pRamp)
    void GetGammaRamp(u iSwapChain, out:1536 pRamp)
    HRESULT CreateTexture(u Width, u Height, u Levels, u Usage, u Format, u Pool, pp:IDirect3DTexture9 ppTexture, shared pSharedHandle)
    HRESULT CreateVolumeTexture(u Width, u Height, u Depth, u Levels, u Usage, u Format, u Pool, pp:IDirect3DVolumeTexture9 ppVolumeTexture, shared pSharedHandle)
    HRESULT CreateCubeTexture(u EdgeLength, u Levels, u Usage, u Format, u Pool, pp:IDirect3DCubeTexture9 ppCubeTexture, shared pSharedHandle)
    HRESULT CreateVertexBuffer(u Length, u Usage, u FVF, u Pool, pp:IDirect3DVertexBuffer9 ppVertexBuffer, shared pSharedHandle)
    HRESULT CreateIndexBuffer(u Length, u Usage, u Format, u Pool, pp:IDirect3DIndexBuffer9 ppIndexBuffer, shared pSharedHandle)
    HRESULT CreateRenderTarget(u Width, u Height, u Format, u MultiSample, u MultisampleQuality, u Lockable, pp:IDirect3DSurface9 ppSurface, shared pSharedHandle)
    HRESULT CreateDepthStencilSurface(u Width, u Height, u Format, u MultiSample, u MultisampleQuality, u Discard, pp:IDirect3DSurface9 ppSurface, shared pSharedHandle)
    HRESULT UpdateSurface(if:IDirect3DSurface9 pSourceSurface, in:16 pSourceRect, if:IDirect3DSurface9 pDestinationSurface, in:8 pDestPoint)
    HRESULT UpdateTexture(if:IDirect3DBaseTexture9 pSourceTexture, if:IDirect3DBaseTexture9 pDestinationTexture)
    HRESULT GetRenderTargetData(if:IDirect3DSurface9 pRenderTarget, if:IDirect3DSurface9 pDestSurface)
    HRESULT GetFrontBufferData(u iSwapChain, if:IDirect3DSurface9 pDestSurface)
    HRESULT StretchRect(if:IDirect3DSurface9 pSourceSurface, in:16 pSourceRect, if:IDirect3DSurface9 pDestSurface, in:16 pDestRect, u Filter)
    HRESULT ColorFill(if:IDirect3DSurface9 pSurface, in:16 pRect, u color)
    HRESULT CreateOffscreenPlainSurface(u Width, u Height, u Format, u Pool, pp:IDirect3DSurface9 ppSurface, shared pSharedHandle)
    HRESULT SetRenderTarget(u RenderTargetIndex, if:IDirect3DSurface9 pRenderTarget)
    HRESULT GetRenderTarget(u RenderTargetIndex, pp:IDirect3DSurface9 ppRenderTarget)
    HRESULT SetDepthStencilSurface(if:IDirect3DSurface9 pNewZStencil)
    HRESULT GetDepthStencilSurface(pp:IDirect3DSurface9 ppZStencilSurface)
    HRESULT BeginScene()
    HRESULT EndScene()
    HRESULT Clear(u Count, in:Count*16 pRects, u Flags, u Color, f Z, u Stencil)
    HRESULT SetTransform(u State, in:64 pMatrix)
    HRESULT GetTransform(u State, out:64 pMatrix)
    HRESULT MultiplyTransform(u State, in:64 pMatrix)
    HRESULT SetViewport(in:24 pViewport)
    HRESULT GetViewport(out:24 pViewport)
    HRESULT SetMaterial(in:68 pMaterial)
    HRESULT GetMaterial(out:68 pMaterial)
    HRESULT SetLight(u Index, in:104 pLight)
    HRESULT GetLight(u Index, out:104 pLight)
    HRESULT LightEnable(u Index, u Enable)
    HRESULT GetLightEnable(u Index, out:4 pEnable)
    HRESULT SetClipPlane(u Index, in:16 pPlane)
    HRESULT GetClipPlane(u Index, out:16 pPlane)
    HRESULT SetRenderState(u State, u Value)
    HRESULT GetRenderState(u State, out:4 pValue)
    HRESULT CreateStateBlock(u Type, pp:IDirect3DStateBlock9 ppSB)
    HRESULT BeginStateBlock()
    HRESULT EndStateBlock(pp:IDirect3DStateBlock9 ppSB)
    HRESULT SetClipStatus(in:8 pClipStatus)
    HRESULT GetClipStatus(out:8 pClipStatus)
    HRESULT GetTexture(u Stage, pp:IDirect3DBaseTexture9 ppTexture)
    HRESULT SetTexture(u Stage, if:IDirect3DBaseTexture9 pTexture)
    HRESULT GetTextureStageState(u Stage, u Type, out:4 pValue)
    HRESULT SetTextureStageState(u Stage, u Type, u Value)
    HRESULT GetSamplerState(u Sampler, u Type, out:4 pValue)
    HRESULT SetSamplerState(u Sampler, u Type, u Value)
    HRESULT ValidateDevice(out:4 pNumPasses)
    HRESULT SetPaletteEntries(u PaletteNumber, in:1024 pEntries)
    HRESULT GetPaletteEntries(u PaletteNumber, out:1024 pEntries)
    HRESULT SetCurrentTexturePalette(u PaletteNumber)
    HRESULT GetCurrentTexturePalette(out:4 PaletteNumber)
    HRESULT SetScissorRect(in:16 pRect)
    HRESULT GetScissorRect(out:16 pRect)
    HRESULT SetSoftwareVertexProcessing(u bSoftware)
    BOOL GetSoftwareVertexProcessing()
    HRESULT SetNPatchMode(f nSegments)
    float GetNPatchMode()
    HRESULT DrawPrimitive(u PrimitiveType, u StartVertex, u PrimitiveCount)
    HRESULT DrawIndexedPrimitive(u PrimitiveType, i BaseVertexIndex, u MinVertexIndex, u NumVertices, u startIndex, u primCount)
    HRESULT DrawPrimitiveUP(u PrimitiveType, u PrimitiveCount, in:(uint64_t)d3d9_vertex_count(PrimitiveType,PrimitiveCount)*VertexStreamZeroStride pVertexStreamZeroData, u VertexStreamZeroStride)
    HRESULT DrawIndexedPrimitiveUP(u PrimitiveType, u MinVertexIndex, u NumVertices, u PrimitiveCount, in:(uint64_t)d3d9_vertex_count(PrimitiveType,PrimitiveCount)*d3d9_index_size(IndexDataFormat) pIndexData, u IndexDataFormat, in:((uint64_t)MinVertexIndex+NumVertices)*VertexStreamZeroStride pVertexStreamZeroData, u VertexStreamZeroStride)
    HRESULT ProcessVertices(u SrcStartIndex, u DestIndex, u VertexCount, if:IDirect3DVertexBuffer9 pDestBuffer, if:IDirect3DVertexDeclaration9 pVertexDecl, u Flags)
    HRESULT CreateVertexDeclaration(decl pVertexElements, pp:IDirect3DVertexDeclaration9 ppDecl)
    HRESULT SetVertexDeclaration(if:IDirect3DVertexDeclaration9 pDecl)
    HRESULT GetVertexDeclaration(pp:IDirect3DVertexDeclaration9 ppDecl)
    HRESULT SetFVF(u FVF)
    HRESULT GetFVF(out:4 pFVF)
    HRESULT CreateVertexShader(shader pFunction, pp:IDirect3DVertexShader9 ppShader)
    HRESULT SetVertexShader(if:IDirect3DVertexShader9 pShader)
    HRESULT GetVertexShader(pp:IDirect3DVertexShader9 ppShader)
    HRESULT SetVertexShaderConstantF(u StartRegister, in:(uint64_t)Vector4fCount*16 pConstantData, u Vector4fCount)
    HRESULT GetVertexShaderConstantF(u StartRegister, out:(uint64_t)Vector4fCount*16 pConstantData, u Vector4fCount)
    HRESULT SetVertexShaderConstantI(u StartRegister, in:(uint64_t)Vector4iCount*16 pConstantData, u Vector4iCount)
    HRESULT GetVertexShaderConstantI(u StartRegister, out:(uint64_t)Vector4iCount*16 pConstantData, u Vector4iCount)
    HRESULT SetVertexShaderConstantB(u StartRegister, in:(uint64_t)BoolCount*4 pConstantData, u BoolCount)
    HRESULT GetVertexShaderConstantB(u StartRegister, out:(uint64_t)BoolCount*4 pConstantData, u BoolCount)
    HRESULT SetStreamSource(u StreamNumber, if:IDirect3DVertexBuffer9 pStreamData, u OffsetInBytes, u Stride)
    HRESULT GetStreamSource(u StreamNumber, pp:IDirect3DVertexBuffer9 ppStreamData, out:4 pOffsetInBytes, out:4 pStride)
    HRESULT SetStreamSourceFreq(u StreamNumber, u Setting)
    HRESULT GetStreamSourceFreq(u StreamNumber, out:4 pSetting)
    HRESULT SetIndices(if:IDirect3DIndexBuffer9 pIndexData)
    HRESULT GetIndices(pp:IDirect3DIndexBuffer9 ppIndexData)
    HRESULT CreatePixelShader(shader pFunction, pp:IDirect3DPixelShader9 ppShader)
    HRESULT SetPixelShader(if:IDirect3DPixelShader9 pShader)
    HRESULT GetPixelShader(pp:IDirect3DPixelShader9 ppShader)
    HRESULT SetPixelShaderConstantF(u StartRegister, in:(uint64_t)Vector4fCount*16 pConstantData, u Vector4fCount)
    HRESULT GetPixelShaderConstantF(u StartRegister, out:(uint64_t)Vector4fCount*16 pConstantData, u Vector4fCount)
    HRESULT SetPixelShaderConstantI(u StartRegister, in:(uint64_t)Vector4iCount*16 pConstantData, u Vector4iCount)
    HRESULT GetPixelShaderConstantI(u StartRegister, out:(uint64_t)Vector4iCount*16 pConstantData, u Vector4iCount)
    HRESULT SetPixelShaderConstantB(u StartRegister, in:(uint64_t)BoolCount*4 pConstantData, u BoolCount)
    HRESULT GetPixelShaderConstantB(u StartRegister, out:(uint64_t)BoolCount*4 pConstantData, u BoolCount)
    HRESULT DrawRectPatch(u Handle, u pNumSegs, u pRectPatchInfo) !stub
    HRESULT DrawTriPatch(u Handle, u pNumSegs, u pTriPatchInfo) !stub
    HRESULT DeletePatch(u Handle)
    HRESULT CreateQuery(u Type, pp:IDirect3DQuery9 ppQuery)

interface IDirect3DSwapChain9 : IUnknown {794950F2-ADFC-458A-905E-10A10B0B503B}
    HRESULT Present(in:16 pSourceRect, in:16 pDestRect, hwnd hDestWindowOverride, rgndata pDirtyRegion, u dwFlags)
    HRESULT GetFrontBufferData(if:IDirect3DSurface9 pDestSurface)
    HRESULT GetBackBuffer(u iBackBuffer, u Type, owned:IDirect3DSurface9 ppBackBuffer)
    HRESULT GetRasterStatus(out:8 pRasterStatus)
    HRESULT GetDisplayMode(out:16 pMode)
    HRESULT GetDevice(pp:IDirect3DDevice9 ppDevice)
    HRESULT GetPresentParameters(params_out pPresentationParameters)

interface IDirect3DResource9 : IUnknown abstract {05EEC05D-8F7D-4362-B999-D1BAF357C704}
    HRESULT GetDevice(pp:IDirect3DDevice9 ppDevice)
    HRESULT SetPrivateData(in:16 refguid, u pData, u SizeOfData, u Flags) !manual
    HRESULT GetPrivateData(in:16 refguid, out:D(pSizeOfData) pData, out:4 pSizeOfData)
    HRESULT FreePrivateData(in:16 refguid)
    DWORD SetPriority(u PriorityNew)
    DWORD GetPriority()
    void PreLoad()
    DWORD GetType()

interface IDirect3DBaseTexture9 : IDirect3DResource9 abstract {580CA87E-1D3C-4D54-991D-B7D3E3C298CE}
    DWORD SetLOD(u LODNew)
    DWORD GetLOD()
    DWORD GetLevelCount()
    HRESULT SetAutoGenFilterType(u FilterType)
    DWORD GetAutoGenFilterType()
    void GenerateMipSubLevels()

interface IDirect3DTexture9 : IDirect3DBaseTexture9 {85C31227-3DE5-4F00-9B3A-F11AC38C18B5}
    HRESULT GetLevelDesc(u Level, out:32 pDesc)
    HRESULT GetSurfaceLevel(u Level, child:IDirect3DSurface9 ppSurfaceLevel)
    HRESULT LockRect(u Level, lockrect pLockedRect, in:16 pRect, u Flags)
    HRESULT UnlockRect(u Level)
    HRESULT AddDirtyRect(in:16 pDirtyRect)

interface IDirect3DVolumeTexture9 : IDirect3DBaseTexture9 {2518526C-E789-4111-A7B9-47EF328D13E6}
    HRESULT GetLevelDesc(u Level, out:28 pDesc)
    HRESULT GetVolumeLevel(u Level, child:IDirect3DVolume9 ppVolumeLevel)
    HRESULT LockBox(u Level, lockbox pLockedVolume, in:24 pBox, u Flags)
    HRESULT UnlockBox(u Level)
    HRESULT AddDirtyBox(in:24 pDirtyBox)

interface IDirect3DCubeTexture9 : IDirect3DBaseTexture9 {FFF32F81-D953-473A-9223-93D652ABA93F}
    HRESULT GetLevelDesc(u Level, out:32 pDesc)
    HRESULT GetCubeMapSurface(u FaceType, u Level, child:IDirect3DSurface9 ppCubeMapSurface)
    HRESULT LockRect(u FaceType, u Level, lockrect pLockedRect, in:16 pRect, u Flags)
    HRESULT UnlockRect(u FaceType, u Level)
    HRESULT AddDirtyRect(u FaceType, in:16 pDirtyRect)

interface IDirect3DVertexBuffer9 : IDirect3DResource9 {B64BB1B5-FD70-4DF6-BF91-19D0A12455E3}
    HRESULT Lock(u OffsetToLock, u SizeToLock, lockptr ppbData, u Flags)
    HRESULT Unlock()
    HRESULT GetDesc(out:24 pDesc)

interface IDirect3DIndexBuffer9 : IDirect3DResource9 {7C9DD65E-D3F7-4529-ACEE-785830ACDE35}
    HRESULT Lock(u OffsetToLock, u SizeToLock, lockptr ppbData, u Flags)
    HRESULT Unlock()
    HRESULT GetDesc(out:20 pDesc)

interface IDirect3DSurface9 : IDirect3DResource9 {0CFBAF3A-9FF6-429A-99B3-A2796AF8B89B}
    HRESULT GetContainer(in:16 riid, u ppContainer) !manual
    HRESULT GetDesc(out:32 pDesc)
    HRESULT LockRect(lockrect pLockedRect, in:16 pRect, u Flags)
    HRESULT UnlockRect()
    HRESULT GetDC(u phdc) !stub
    HRESULT ReleaseDC(u hdc) !stub

interface IDirect3DVolume9 : IUnknown {24F416E6-1F67-4AA7-B88E-D33F6F3128A1}
    HRESULT GetDevice(pp:IDirect3DDevice9 ppDevice)
    HRESULT SetPrivateData(in:16 refguid, u pData, u SizeOfData, u Flags) !manual
    HRESULT GetPrivateData(in:16 refguid, out:D(pSizeOfData) pData, out:4 pSizeOfData)
    HRESULT FreePrivateData(in:16 refguid)
    HRESULT GetContainer(in:16 riid, u ppContainer) !manual
    HRESULT GetDesc(out:28 pDesc)
    HRESULT LockBox(lockbox pLockedVolume, in:24 pBox, u Flags)
    HRESULT UnlockBox()

interface IDirect3DVertexDeclaration9 : IUnknown {DD13C59C-36FA-4098-A8FB-C7ED39DC8546}
    HRESULT GetDevice(pp:IDirect3DDevice9 ppDevice)
    HRESULT GetDeclaration(u pElement, u pNumElements) !manual

interface IDirect3DVertexShader9 : IUnknown {EFC5557E-6265-4613-8A94-43857889EB36}
    HRESULT GetDevice(pp:IDirect3DDevice9 ppDevice)
    HRESULT GetFunction(out:D(pSizeOfData) pData, out:4 pSizeOfData)

interface IDirect3DPixelShader9 : IUnknown {6D3BDBDC-5B02-4415-B852-CE5E8BCCB289}
    HRESULT GetDevice(pp:IDirect3DDevice9 ppDevice)
    HRESULT GetFunction(out:D(pSizeOfData) pData, out:4 pSizeOfData)

interface IDirect3DStateBlock9 : IUnknown {B07C4FE5-310D-4BA8-A23C-4F0F206F218B}
    HRESULT GetDevice(pp:IDirect3DDevice9 ppDevice)
    HRESULT Capture()
    HRESULT Apply()

interface IDirect3DQuery9 : IUnknown {D9771460-A695-4F26-BBD3-27B840B541CC}
    HRESULT GetDevice(pp:IDirect3DDevice9 ppDevice)
    DWORD GetType()
    DWORD GetDataSize()
    HRESULT Issue(u dwIssueFlags)
    HRESULT GetData(out:dwSize pData, u dwSize, u dwGetDataFlags)
'''

RETURNS = {'HRESULT': 'int32_t', 'ULONG': 'uint32_t', 'UINT': 'uint32_t', 'DWORD': 'uint32_t',
           'BOOL': 'uint32_t', 'void': 'void', 'float': 'float', 'HMONITOR': 'void *'}
SIMPLE = {'hwnd', 'shared', 'params', 'params_out', 'adapterid', 'creation', 'lockptr', 'lockrect',
          'lockbox', 'shader', 'decl', 'rgndata'}


class Param:
    def __init__(self, kind, name):
        self.name = name
        self.kind, _, self.extra = kind.partition(':')
        if self.kind not in ('u', 'i', 'f', 'in', 'out', 'if', 'pp', 'child', 'owned') and self.kind not in SIMPLE:
            raise SystemExit(f'type inconnu {kind} pour {name}')
        if self.kind in ('in', 'out') and not self.extra:
            raise SystemExit(f'{name} : taille manquante')

    @property
    def scalar(self):
        return self.kind in ('u', 'i', 'f')


class Method:
    def __init__(self, iface, ret, name, params, flags):
        self.iface, self.ret, self.name, self.params, self.flags = iface, ret, name, params, flags
        if ret not in RETURNS:
            raise SystemExit(f'{iface.name}::{name} : retour inconnu {ret}')
        if ret == 'HMONITOR' and 'stub' not in flags and 'manual' not in flags:
            raise SystemExit(f'{iface.name}::{name} : retour pointeur à écrire à la main')
        self.slot = None
        self.index = None

    @property
    def full(self):
        return f'{self.iface.name}::{self.name}'

    @property
    def cname(self):
        return f'{self.iface.name}_{self.name}'


class Interface:
    def __init__(self, name, parent, abstract, iid):
        self.name, self.parent, self.abstract, self.iid = name, parent, abstract, iid
        self.methods = []
        self.id = None

    def vtable(self):
        """Méthodes de la vtable complète, dans l'ordre de d3d9.h."""
        return (self.parent.vtable() if self.parent else []) + self.methods


def parse(text=SIGNATURES):
    interfaces, current = {}, None
    for raw in text.splitlines():
        line = raw.strip()
        if not line:
            continue
        m = re.match(r'interface (\w+)(?: : (\w+))?( abstract)? \{([0-9A-Fa-f-]{36})\}$', line)
        if m:
            name, parent, abstract, iid = m.groups()
            if parent and parent not in interfaces:
                raise SystemExit(f'{name} : parent {parent} inconnu')
            current = interfaces[name] = Interface(name, interfaces.get(parent), bool(abstract), iid)
            continue
        m = re.match(r'(\w+) (\w+)\((.*)\)((?: !\w+)*)$', line)
        if not m or current is None:
            raise SystemExit(f'ligne illisible : {line}')
        ret, name, args, flags = m.groups()
        params = []
        for arg in filter(None, (a.strip() for a in split_args(args))):
            kind, _, pname = arg.rpartition(' ')
            params.append(Param(kind, pname))
        names = [p.name for p in params]
        if len(set(names)) != len(names):
            raise SystemExit(f'{current.name}::{name} : paramètres en double')
        current.methods.append(Method(current, ret, name, params, set(flags.split()) and {f[1:] for f in flags.split()}))
    ordered = list(interfaces.values())
    for i, iface in enumerate(ordered):
        iface.id = i
        for slot, method in enumerate(iface.vtable()):
            if method.iface is iface:
                method.slot = slot
    if len(ordered) > 32:
        raise SystemExit('plus de 32 interfaces : le masque d’acceptation ne tient plus')
    return ordered


def split_args(text):
    """Découpe sur les virgules hors parenthèses (expressions de taille)."""
    depth, start, out = 0, 0, []
    for i, ch in enumerate(text):
        if ch == '(':
            depth += 1
        elif ch == ')':
            depth -= 1
        elif ch == ',' and depth == 0:
            out.append(text[start:i])
            start = i + 1
    out.append(text[start:])
    return out


def derived(interfaces, base):
    """Masque des interfaces concrètes qui héritent de base (base comprise)."""
    mask = 0
    for iface in interfaces:
        walk = iface
        while walk and walk is not base:
            walk = walk.parent
        if walk is base and not iface.abstract:
            mask |= 1 << iface.id
    return mask


def size_expr(method, expr):
    """Expression de taille : noms de paramètres scalaires → a_<nom>, D(p) → déréférence invitée."""
    scalars = {p.name for p in method.params if p.scalar}
    pointers = {p.name for p in method.params if not p.scalar}

    def deref(m):
        if m.group(1) not in pointers:
            raise SystemExit(f'{method.full} : D({m.group(1)}) ne désigne pas un pointeur')
        return f'd3d9_deref(c, g_{m.group(1)})'
    expr = re.sub(r'\bD\((\w+)\)', deref, expr)

    def name(m):
        word = m.group(0)
        if word in scalars:
            return f'a_{word}'
        if word in pointers:
            raise SystemExit(f'{method.full} : {word} est un pointeur, utiliser D({word})')
        return word
    return re.sub(r'(?<![\w.])[A-Za-z_]\w*', name, expr)


def ctype(param):
    return {'u': 'uint32_t', 'i': 'int32_t', 'f': 'float'}.get(param.kind, 'void *')


def thunk(interfaces, method):
    by_name = {i.name: i for i in interfaces}
    lines = [f'static void t_{method.cname}(CPU *c, void *self, uint32_t a) {{']
    for i, p in enumerate(method.params):
        lines.append(f'    uint32_t g_{p.name} = R32(a + {4 * i});')
    after, call_args, types = [], [], []
    for p in method.params:       # scalaires d'abord : les tailles les citent
        if p.kind == 'u':
            lines.append(f'    uint32_t a_{p.name} = g_{p.name};')
        elif p.kind == 'i':
            lines.append(f'    int32_t a_{p.name} = (int32_t)g_{p.name};')
        elif p.kind == 'f':
            lines.append(f'    float a_{p.name} = d3d9_f32(g_{p.name});')
    for p in method.params:
        n = p.name
        if p.kind in ('in', 'out'):
            lines.append(f'    void *a_{n} = d3d9_{p.kind}(c, g_{n}, (uint64_t)({size_expr(method, p.extra)}));')
        elif p.kind == 'if':
            target = by_name.get(p.extra)
            if not target:
                raise SystemExit(f'{method.full} : interface {p.extra} inconnue')
            lines.append(f'    void *a_{n} = d3d9_iface_in(c, g_{n}, 0x{derived(interfaces, target):x}u);')
        elif p.kind in ('pp', 'child', 'owned'):
            target = by_name.get(p.extra)
            if not target:
                raise SystemExit(f'{method.full} : interface {p.extra} inconnue')
            lines.append(f'    void *h_{n} = NULL, **a_{n} = d3d9_out_slot(c, g_{n}) ? &h_{n} : NULL;')
            if p.kind == 'pp':
                after.append(f'    d3d9_iface_out(c, g_{n}, h_{n}, D3D9_{target.name});')
            else:
                shared = int(p.kind == 'child')
                after.append(f'    d3d9_child_out(c, g_{n}, h_{n}, D3D9_{target.name}, self, {shared});')
        elif p.kind == 'hwnd':
            lines.append(f'    void *a_{n} = d3d9_hwnd(c, g_{n});')
        elif p.kind == 'shared':
            lines.append(f'    void *a_{n} = d3d9_shared(c, g_{n});')
        elif p.kind in ('params', 'params_out'):
            lines.append(f'    D3d9HostParams h_{n}; void *a_{n} = d3d9_params_in(c, g_{n}, &h_{n}, {int(p.kind == "params")});')
            after.append(f'    if (hr >= 0) d3d9_params_out(c, g_{n}, &h_{n});')
        elif p.kind == 'adapterid':
            lines.append(f'    D3d9HostAdapterId h_{n}; void *a_{n} = d3d9_adapter_id_in(c, g_{n}, &h_{n});')
            after.append(f'    if (hr >= 0) d3d9_adapter_id_out(c, g_{n}, &h_{n});')
        elif p.kind == 'creation':
            lines.append(f'    D3d9HostCreation h_{n}; void *a_{n} = d3d9_creation_in(c, g_{n}, &h_{n});')
            after.append(f'    if (hr >= 0) d3d9_creation_out(c, g_{n}, &h_{n});')
        elif p.kind == 'lockptr':
            lines.append(f'    void *h_{n} = NULL, **a_{n} = d3d9_out_slot(c, g_{n}) ? &h_{n} : NULL;')
            after.append(f'    d3d9_lock_ptr_out(c, g_{n}, h_{n}, hr >= 0);')
        elif p.kind in ('lockrect', 'lockbox'):
            size = 8 if p.kind == 'lockrect' else 12   # D3DLOCKED_RECT / D3DLOCKED_BOX i386
            lines.append(f'    D3d9HostLocked h_{n}; void *a_{n} = d3d9_locked_in(c, g_{n}, &h_{n}, {size});')
            after.append(f'    d3d9_{p.kind}_out(c, g_{n}, &h_{n}, hr >= 0);')
        elif p.kind in ('shader', 'decl', 'rgndata'):
            lines.append(f'    const void *a_{n} = d3d9_{p.kind}(c, g_{n});')
        call_args.append(f'a_{n}')
        types.append('void **' if p.kind in ('pp', 'child', 'owned', 'lockptr') else
                     'const void *' if p.kind in ('shader', 'decl', 'rgndata') else ctype(p))
    rtype = RETURNS[method.ret]
    signature = f'{rtype} (*)({", ".join(["void *"] + types)})'
    call = f'(({signature})D3D9_VTBL(self)[{method.slot}])({", ".join(["self"] + call_args)})'
    if after and method.ret != 'HRESULT':
        raise SystemExit(f'{method.full} : sorties traduites sans HRESULT')
    if rtype == 'void':
        lines.append(f'    {call};')
        lines.append('    (void)c;' if method.params else '    (void)c; (void)a;')
    elif method.ret == 'HRESULT':
        lines.append(f'    int32_t hr = {call};')
        lines += after
        lines.append('    c->eax = (uint32_t)hr;')
    elif rtype == 'float':
        lines.append(f'    rt_return_st0(c, {call});')
    else:
        lines.append(f'    c->eax = {call};')
    if not method.params and rtype != 'void':
        lines.append('    (void)a;')
    lines.append('}')
    return '\n'.join(lines)


def iid_bytes(text):
    parts = text.split('-')
    data1, data2, data3 = int(parts[0], 16), int(parts[1], 16), int(parts[2], 16)
    rest = bytes.fromhex(parts[3] + parts[4])
    raw = data1.to_bytes(4, 'little') + data2.to_bytes(2, 'little') + data3.to_bytes(2, 'little') + rest
    return ', '.join(f'0x{b:02x}' for b in raw)


HEADER = '''// GÉNÉRÉ par tools/gen_d3d9.py — ne pas éditer à la main.
// Identifiants des interfaces Direct3D 9 du pont (runtime/d3d9.c) et
// prototypes des méthodes écrites à la main.
#pragma once
#include "rt.h"
'''

SOURCE = '''// GÉNÉRÉ par tools/gen_d3d9.py — ne pas éditer à la main.
// Thunks COM i386 cdecl → objets Direct3D 9 hôtes (ABI arm64), tables des
// méthodes et des vtables. Aides d3d9_* : runtime/d3d9.c (d3d9_bridge.h).
#include "d3d9_bridge.h"
'''


def generate():
    interfaces = parse()
    methods = sorted((m for i in interfaces for m in i.methods), key=lambda m: m.full)
    for index, m in enumerate(methods):
        m.index = index
    header = [HEADER, 'enum {']
    for iface in interfaces:
        header.append(f'    D3D9_{iface.name} = {iface.id},')
    header.append(f'    D3D9_INTERFACE_COUNT = {len(interfaces)},')
    header.append('};')
    for iface in interfaces:
        header.append(f'#define D3D9_MASK_{iface.name} 0x{derived(interfaces, iface):x}u')
    header.append('')
    for m in methods:
        if 'manual' in m.flags:
            header.append(f'#define D3D9_SLOT_{m.cname} {m.slot}')
            header.append(f'void d3d9_manual_{m.cname}(CPU *c, void *self, uint32_t a);')
    source = [SOURCE]
    for m in methods:
        if not m.flags:
            source.append(thunk(interfaces, m))
            source.append('')
    source.append('// Trié par nom (bsearch) ; slot : rang dans la vtable de l’interface qui définit la méthode.')
    source.append('const D3d9Method rt_d3d9_methods[] = {')
    for m in methods:
        fn = f'd3d9_manual_{m.cname}' if 'manual' in m.flags else 'NULL' if 'stub' in m.flags else f't_{m.cname}'
        kind = 'D3D9_STUB' if 'stub' in m.flags else 'D3D9_MANUAL' if 'manual' in m.flags else 'D3D9_GENERATED'
        ret = 'D3D9_RET_HRESULT' if m.ret == 'HRESULT' else 'D3D9_RET_VOID' if m.ret == 'void' else \
            'D3D9_RET_FLOAT' if m.ret == 'float' else 'D3D9_RET_U32'
        source.append(f'    {{"{m.full}", {fn}, 0x{derived(interfaces, m.iface):x}u, {m.slot}, {kind}, {ret}}},')
    source.append('};')
    source.append(f'const unsigned rt_d3d9_method_count = {len(methods)};')
    source.append('')
    for iface in interfaces:
        if iface.abstract:
            continue
        slots = ', '.join(str(m.index) for m in iface.vtable())
        source.append(f'static const uint16_t slots_{iface.name}[] = {{{slots}}};')
    source.append('')
    source.append('const D3d9Interface rt_d3d9_interfaces[] = {')
    for iface in interfaces:
        parent = f'D3D9_{iface.parent.name}' if iface.parent else '-1'
        slots = 'NULL, 0' if iface.abstract else f'slots_{iface.name}, {len(iface.vtable())}'
        source.append(f'    {{"{iface.name}", {parent}, {int(iface.abstract)}, 0x{derived(interfaces, iface):x}u, '
                      f'{{{iid_bytes(iface.iid)}}}, {slots}}},')
    source.append('};')
    source.append(f'const unsigned rt_d3d9_interface_count = {len(interfaces)};')
    return '\n'.join(header) + '\n', '\n'.join(source) + '\n'


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('--check', action='store_true', help='échoue si les fichiers générés ne sont pas à jour')
    args = parser.parse_args()
    header, source = generate()
    targets = {ROOT / 'runtime/d3d9_generated.h': header, ROOT / 'runtime/d3d9_generated.c': source}
    if args.check:
        stale = [str(p) for p, text in targets.items() if not p.exists() or p.read_text() != text]
        if stale:
            sys.exit('à régénérer (python3 tools/gen_d3d9.py) : ' + ', '.join(stale))
        return 0
    for path, text in targets.items():
        path.write_text(text)
    interfaces = parse()
    count = sum(len(i.methods) for i in interfaces)
    print(f'runtime/d3d9_generated.c : {len(interfaces)} interfaces, {count} méthodes')
    return 0


if __name__ == '__main__':
    sys.exit(main())
