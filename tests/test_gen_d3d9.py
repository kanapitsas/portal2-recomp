"""tools/gen_d3d9.py : table des interfaces Direct3D 9 et thunks générés.

Avec P2_D3D9_HEADER=/chemin/d3d9.h (en-têtes natifs de DXVK ou de Wine), la
table est aussi comparée méthode par méthode à l'en-tête officiel.
"""
import os
import re
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / 'tools'))
import gen_d3d9  # noqa: E402


def interfaces():
    return {i.name: i for i in gen_d3d9.parse()}


class Table(unittest.TestCase):
    def test_generated_files_are_up_to_date(self):
        header, source = gen_d3d9.generate()
        self.assertEqual((ROOT / 'runtime/d3d9_generated.h').read_text(), header,
                         'run python3 tools/gen_d3d9.py')
        self.assertEqual((ROOT / 'runtime/d3d9_generated.c').read_text(), source,
                         'run python3 tools/gen_d3d9.py')

    def test_vtable_sizes_follow_d3d9_h(self):
        sizes = {'IDirect3D9': 17, 'IDirect3DDevice9': 119, 'IDirect3DSwapChain9': 10,
                 'IDirect3DTexture9': 22, 'IDirect3DCubeTexture9': 22, 'IDirect3DVolumeTexture9': 22,
                 'IDirect3DSurface9': 17, 'IDirect3DVolume9': 11, 'IDirect3DVertexBuffer9': 14,
                 'IDirect3DIndexBuffer9': 14, 'IDirect3DVertexDeclaration9': 5,
                 'IDirect3DVertexShader9': 5, 'IDirect3DPixelShader9': 5,
                 'IDirect3DStateBlock9': 6, 'IDirect3DQuery9': 8}
        table = interfaces()
        for name, size in sizes.items():
            self.assertEqual(len(table[name].vtable()), size, name)
        device = [m.name for m in table['IDirect3DDevice9'].vtable()]
        self.assertEqual(device.index('SetRenderState'), 57)
        self.assertEqual(device.index('Present'), 17)
        self.assertEqual(device.index('DrawIndexedPrimitive'), 82)
        self.assertEqual(device.index('CreateQuery'), 118)

    def test_every_method_has_one_definition_and_slot(self):
        for iface in interfaces().values():
            for slot, method in enumerate(iface.vtable()):
                self.assertEqual(method.slot, slot, method.full)

    def test_size_expressions_name_parameters(self):
        table = interfaces()
        set_f = next(m for m in table['IDirect3DDevice9'].methods if m.name == 'SetVertexShaderConstantF')
        self.assertEqual(gen_d3d9.size_expr(set_f, '(uint64_t)Vector4fCount*16'), '(uint64_t)a_Vector4fCount*16')
        get = next(m for m in table['IDirect3DResource9'].methods if m.name == 'GetPrivateData')
        self.assertEqual(gen_d3d9.size_expr(get, 'D(pSizeOfData)'), 'd3d9_deref(c, g_pSizeOfData)')
        with self.assertRaises(SystemExit):
            gen_d3d9.size_expr(get, 'pSizeOfData*2')   # pointeur utilisé comme scalaire

    def test_unknown_parameter_kind_is_refused(self):
        with self.assertRaises(SystemExit):
            gen_d3d9.parse('interface IUnknown abstract {00000000-0000-0000-C000-000000000046}\n'
                           '    HRESULT Bad(ptr p)\n')

    def test_thunk_translates_pointers_and_returns(self):
        table = interfaces()
        device = table['IDirect3DDevice9']
        by_name = {m.name: m for m in device.methods}
        everything = list(table.values())
        lock = gen_d3d9.thunk(everything, next(m for m in table['IDirect3DVertexBuffer9'].methods if m.name == 'Lock'))
        self.assertIn('d3d9_lock_ptr_out(c, g_ppbData, h_ppbData, hr >= 0);', lock)
        create = gen_d3d9.thunk(everything, by_name['CreateTexture'])
        self.assertIn('d3d9_iface_out(c, g_ppTexture, h_ppTexture, D3D9_IDirect3DTexture9);', create)
        self.assertIn('d3d9_shared(c, g_pSharedHandle)', create)
        clear = gen_d3d9.thunk(everything, by_name['Clear'])
        self.assertIn('float a_Z = d3d9_f32(g_Z);', clear)
        self.assertIn('d3d9_in(c, g_pRects, (uint64_t)(a_Count*16))', clear)
        npatch = gen_d3d9.thunk(everything, by_name['GetNPatchMode'])
        self.assertIn('rt_return_st0(c, ', npatch)
        set_texture = gen_d3d9.thunk(everything, by_name['SetTexture'])
        mask = gen_d3d9.derived(everything, table['IDirect3DBaseTexture9'])
        self.assertIn(f'd3d9_iface_in(c, g_pTexture, 0x{mask:x}u)', set_texture)
        self.assertEqual(mask, sum(1 << table[n].id for n in
                                   ('IDirect3DTexture9', 'IDirect3DCubeTexture9', 'IDirect3DVolumeTexture9')))

    def test_iid_bytes_are_little_endian_guid(self):
        self.assertEqual(gen_d3d9.iid_bytes('81BDCBCA-64D4-426D-AE8D-AD0147F4275C'),
                         '0xca, 0xcb, 0xbd, 0x81, 0xd4, 0x64, 0x6d, 0x42, 0xae, 0x8d, 0xad, 0x01, 0x47, 0xf4, 0x27, 0x5c')


@unittest.skipUnless(os.environ.get('P2_D3D9_HEADER'), 'P2_D3D9_HEADER=/path/to/d3d9.h not set')
class AgainstHeader(unittest.TestCase):
    def test_methods_and_arity_match_header(self):
        text = Path(os.environ['P2_D3D9_HEADER']).read_text()
        table = interfaces()
        checked = 0
        for name, iface in table.items():
            if name == 'IUnknown':
                continue
            m = re.search(r'DECLARE_INTERFACE_\(%s,\s*\w+\)\s*\{(.*?)\n\};' % name, text, re.S)
            self.assertIsNotNone(m, name)
            body = re.sub(r'\s+', ' ', m.group(1))
            methods = re.findall(r'STDMETHOD_?\((?:[^,()]+,\s*)?(\w+)\)\s*\(THIS_?\s*([^)]*)\)', body)
            ours = iface.vtable()
            self.assertEqual([n for n, _ in methods], [x.name for x in ours], name)
            for (method, args), mine in zip(methods, ours):
                count = 0 if not args.strip() else len(args.split(','))
                self.assertEqual(count, len(mine.params), f'{name}::{method}')
            checked += 1
        self.assertEqual(checked, len(table) - 1)

    def test_iids_match_header(self):
        text = Path(os.environ['P2_D3D9_HEADER']).read_text()
        for name, iface in interfaces().items():
            if name == 'IUnknown':
                continue
            m = re.search(r'DEFINE_GUID\(IID_%s,\s*([^)]*)\)' % name, text)
            self.assertIsNotNone(m, name)
            values = [int(v, 16) for v in m.group(1).replace(' ', '').split(',')]
            parts = iface.iid.split('-')
            expected = [int(parts[0], 16), int(parts[1], 16), int(parts[2], 16)] + list(bytes.fromhex(parts[3] + parts[4]))
            self.assertEqual(values, expected, name)


if __name__ == '__main__':
    unittest.main()
