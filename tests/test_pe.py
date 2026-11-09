from pathlib import Path
import struct
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
sys.path[:0] = [str(ROOT / 'tools'), str(ROOT / 'tests')]
import lift  # noqa: E402
import localconfig  # noqa: E402
import pe_fixture  # noqa: E402
import peprep  # noqa: E402


class _Words:
    """Image minimale pour mark_relocations : mots relocalisés à des RVA ;
    .text couvre les RVA 0x1000-0x2000."""
    image_base = 0x10000000

    def code_target(self, value):
        return 0x10001000 <= value < 0x10002000

    def __init__(self, words):
        self.words = words

    def u32(self, rva):
        return self.words[rva]


class MarkRelocationsTests(unittest.TestCase):
    def test_immediate_and_displacement_are_marked_separately(self):
        image = _Words({0x1002: 0x10002000, 0x1006: 0x10001040})
        insn = (0x1000100a, 'mov', 'dword ptr [0x10002000], 0x10001040')
        _, op, args = lift.mark_relocations(insn, [0x10001002, 0x10001006], image)
        self.assertEqual((op, args), ('mov', 'dword ptr [@0x10002000], @c0x10001040'))
        self.assertIn('(0x10002000u + d_)', lift.Operand('dword ptr [@0x10002000]').addr)
        self.assertEqual(lift.Operand('@c0x10001040').read(), '(uint32_t)(0x10001040u + x_)')

    def test_unrelocated_twin_value_is_refused(self):
        # La même valeur en immédiat et en déplacement, une seule relocation :
        # impossible de savoir laquelle relocaliser, l'instruction est refusée.
        image = _Words({0x1002: 0x10002000})
        insn = (0x1000100a, 'mov', 'dword ptr [0x10002000], 0x10002000')
        self.assertEqual(lift.mark_relocations(insn, [0x10001002], image)[1], 'unrelocatable')
        with self.assertRaises(lift.Unsupported):
            lift.translate(0x10001000, (0x1000100a, 'unrelocatable', ''))

    def test_negative_displacement_becomes_relocated_sum(self):
        image = _Words({0x1003: 0xfffff000})
        _, _, args = lift.mark_relocations((0x10001007, 'mov', 'eax, dword ptr [ecx - 0x1000]'), [0x10001003], image)
        self.assertEqual(args, 'eax, dword ptr [ecx + @0xfffff000]')
        self.assertIn('(0xfffff000u + d_)', lift.Operand('dword ptr [ecx + @0xfffff000]').addr)

    def test_relocated_value_cannot_be_scaled_or_subtracted(self):
        for text in ('dword ptr [4*@0x10002000]', 'dword ptr [eax - @0x10002000]'):
            with self.assertRaises(lift.Unsupported):
                lift.Operand(text)


class TestDllTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.tmp = tempfile.TemporaryDirectory()
        cls.path, cls.symbols = pe_fixture.build(cls.tmp.name)
        cls.image = peprep.PeImage(cls.path)
        cls.source, cls.stats = lift.generate_pe(cls.image, 'testpe', log=lambda *_: None)

    @classmethod
    def tearDownClass(cls):
        cls.tmp.cleanup()

    def test_parser_reads_sections_exports_and_relocations(self):
        image, s = self.image, self.symbols
        self.assertEqual(image.image_base, 0x10000000)
        self.assertEqual([x['name'] for x in image.executable_sections()], ['.text'])
        self.assertEqual(image.exports, {n: s[n] for n in ('pe_address', 'pe_load', 'pe_switch', 'pe_indirect')})
        # Une relocation par adresse absolue : immédiats, déplacement, table du switch, pointeur en .data.
        expected = {s['pe_address'] + 1, s['pe_load'] + 1, s['pe_switch'] + 12, s['table'], s['table'] + 4,
                    s['table'] + 8, s['pe_indirect'] + 1, s['callbacks']}
        self.assertEqual(set(image.relocs), expected)
        self.assertEqual(image.u32(s['callbacks']), image.image_base + s['callback'])
        self.assertEqual(image.summary()['file_hash'], peprep.fnv1a64(self.path.read_bytes()))

    def test_every_function_is_lifted_and_table_is_not_code(self):
        base, s = self.image.image_base, self.symbols
        self.assertEqual(self.stats['not_compiled'], {})
        rvas = {int(x, 16) for x in self.source.split('function_rvas[')[1].split('{')[1].split('}')[0].split('u,') if x.strip()}
        for name in ('pe_address', 'pe_load', 'helper', 'pe_switch', 'pe_indirect', 'callback'):
            self.assertIn(s[name], rvas, name)
        self.assertNotIn(s['table'], rvas)
        self.assertIn(f'c->eax = (uint32_t)((uint32_t)({base + s["value"]:#x}u + d_));', self.source)
        self.assertIn(f'rt_call_fn(c, pe_testpe_{base + s["helper"]:x}, ({base + s["helper"]:#x}u + x_), ', self.source)
        # Pointeur vers .text : vue exécutable (x_) ; donnée : vue écrivable (d_).
        self.assertIn(f'c->ecx = (uint32_t)((uint32_t)({base + s["helper"]:#x}u + x_));', self.source)
        self.assertIn('switch ((uint32_t)(a - x_))', self.source)
        self.assertNotIn('fn_', self.source.replace('pe_testpe_', '').replace('function_rvas', '').replace('functions', ''))

    def test_code_section_bytes_and_relocations_are_embedded(self):
        text = self.image.executable_sections()[0]
        first = self.image.mapped[text['rva']:text['rva'] + 8]
        self.assertIn(f'static const uint8_t section_0[{text["size"]}] = {{\n' + ','.join(str(b) for b in first), self.source)
        relocations = self.source.split('relocations[')[1]
        self.assertIn(f'{self.symbols["table"] | 0x80000000:#x}u', relocations)        # vise .text
        self.assertIn(f'{self.symbols["pe_load"] + 1:#x}u,', relocations)              # vise .data


@unittest.skipUnless(localconfig.DEFAULT_PATH.exists(), 'no local game copy')
class RealPluginTests(unittest.TestCase):
    def test_mssmp3_is_fully_lifted(self):
        config = localconfig.for_game('portal2')
        path = Path(config.get('game_root', '')) / 'bin/linux32/mssmp3.asi'
        if not path.is_file():
            self.skipTest('mssmp3.asi not found')
        image = peprep.PeImage(path)
        self.assertEqual(image.imports, [])
        source, stats = lift.generate_pe(image, 'mssmp3', log=lambda *_: None)
        self.assertEqual(stats['not_compiled'], {})
        self.assertIn(f'pe_mssmp3_{image.image_base + image.exports["RIB_Main"]:x}(CPU *restrict c)', source)
        self.assertNotIn('unrelocatable', source)


if __name__ == '__main__':
    unittest.main()
