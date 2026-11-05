import importlib.util
from pathlib import Path
import unittest

import sys
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))

spec = importlib.util.spec_from_file_location('lift', Path(__file__).resolve().parents[1] / 'tools/lift.py')
lift = importlib.util.module_from_spec(spec)
spec.loader.exec_module(lift)


class LifterTests(unittest.TestCase):
    def test_runtime_files_declare_shims(self):
        import tempfile
        with tempfile.TemporaryDirectory() as tmp:
            (Path(tmp) / 'a.c').write_text('// SHIMS: SDL_Init SDL_Quit\nint x;\n// SHIMS: sinf\n')
            (Path(tmp) / 'b.h').write_text('// SHIMS: ignored_in_headers\n')
            self.assertEqual(lift.declared_shims(Path(tmp)), {'SDL_Init', 'SDL_Quit', 'sinf'})

    def test_reject_unsupported_reachable_instruction(self):
        with self.assertRaisesRegex(lift.Unsupported, '0x10.*rsm'):
            lift.lift(0x10, {0x10: (0x12, 'rsm', '')})

    def test_direct_call_records_dependency_and_return_site(self):
        calls = set()
        code, count = lift.lift(0x10, {0x10: (0x15, 'call', '0x30'),
                                      0x15: (0x16, 'ret', '')}, callees=calls)
        self.assertEqual(calls, {0x30})
        self.assertEqual(count, 2)
        self.assertIn('rt_call_fn(c, fn_30, 0x30u, 0x15u);', code)
        self.assertIn('if (c->eip != 0x15u) goto resume;', code)

    def test_link_direct_keeps_compiled_targets_only(self):
        insns = {0x10: (0x15, 'call', '0x30'), 0x15: (0x1a, 'call', '0x40'),
                 0x1a: (0x1f, 'jmp', '0x50')}
        code, _ = lift.lift(0x10, insns, entries={0x50})
        linked, used = lift.link_direct(code, {0x30, 0x50})
        self.assertEqual(used, {0x30, 0x50})
        self.assertIn('rt_call_fn(c, fn_30, 0x30u, 0x15u);', linked)
        self.assertIn('rt_call(c, 0x40u, 0x1au);', linked)   # not compiled: checked path
        self.assertNotIn('fn_40', linked)
        self.assertIn('rt_jump_fn(c, fn_50, 0x50u); return;', linked)
        imported, _ = lift.link_direct(code, {0x30, 0x50}, imports={0x40: 'strlen'})
        self.assertIn('rt_call_import(c, 0x40u, 0x1au);', imported)
        unlinked, used = lift.link_direct(code, set())
        self.assertEqual(used, set())
        self.assertIn('rt_dispatch(c, 0x50u); return;', unlinked)
        self.assertNotIn('rt_call_fn', unlinked)

    def test_indirect_jump_uses_checked_dispatch(self):
        code, count = lift.lift(0x10, {0x10: (0x12, 'jmp', 'eax')})
        self.assertEqual(count, 1)
        self.assertIn('rt_jump_any(c, a)', code)

    def test_reject_unknown_direct_import(self):
        layout = {'modules': [], 'imports': [{'name': 'not_supported', 'addr': 0x30}]}
        from unittest.mock import patch
        with patch.object(lift, 'resolve_export', return_value=0x30):
            with self.assertRaisesRegex(lift.Unsupported, 'has no shim'):
                lift.generate(layout, ['entry'], 'objdump')

    def test_reject_target_inside_instruction(self):
        with self.assertRaisesRegex(lift.Unsupported, 'no instruction at 0x11'):
            lift.lift(0x10, {0x10: (0x12, 'jmp', '0x11')})

    def test_only_reachable_code_is_required(self):
        code, count = lift.lift(0x10, {0x10: (0x11, 'ret', ''), 0x11: (0x13, 'ud2', '')})
        self.assertEqual(count, 1)
        self.assertIn('rt_return(c)', code)

    def test_lock_prefix_keeps_the_real_instruction_boundary(self):
        from unittest.mock import patch
        import subprocess
        output = '10: f0 \tlock\n11: 0f b1 16 \tcmpxchg dword ptr [esi], edx\n14: c3 \tret\n'
        result = subprocess.CompletedProcess([], 0, output, '')
        with patch.object(lift.subprocess, 'run', return_value=result):
            insns = lift.disassemble('fixture', 0, 'objdump')
        self.assertNotIn(0x11, insns)
        self.assertEqual(insns[0x10], (0x14, 'lock_cmpxchg', 'dword ptr [esi], edx'))

    def test_reject_segment_addressing(self):
        with self.assertRaises(lift.Unsupported):
            lift.Operand('dword ptr fs:[0x14]')

    def test_reject_malformed_address(self):
        for expr in ('eax/4', 'eax++4', 'eax*bad', 'eax;abort()', ''):
            with self.subTest(expr=expr), self.assertRaises(lift.Unsupported):
                lift.address(expr)

    def test_pic_jump_table_becomes_local_switch(self):
        import tempfile
        got, base = 0x10020000, 0x10000000
        insns = {
            0x10: (0x13, 'cmp', 'eax, 0x2'), 0x13: (0x15, 'ja', '0x40'),
            0x15: (0x1c, 'mov', 'ecx, dword ptr [ebx + 4*eax - 0x100]'),
            0x1c: (0x1e, 'add', 'ecx, ebx'), 0x1e: (0x20, 'jmp', 'ecx'),
            0x20: (0x21, 'ret', ''), 0x21: (0x22, 'ret', ''), 0x22: (0x23, 'ret', ''),
            0x40: (0x41, 'ret', '')}
        insns = {pc + base: (nxt + base, op, args) for pc, (nxt, op, args) in insns.items()}
        insns[base + 0x13] = (base + 0x15, 'ja', hex(base + 0x40))
        with tempfile.TemporaryDirectory() as tmp:
            image = bytearray(0x30000)
            table = got - 0x100 - base
            for i, target in enumerate((0x20, 0x21, 0x22, 0x20)):   # 4th entry is beyond the bound
                image[table + 4 * i:table + 4 * i + 4] = ((base + target - got) & 0xffffffff).to_bytes(4, 'little')
            path = Path(tmp) / 'image'; path.write_bytes(bytes(image))
            layout = {'modules': [{'name': 'm', 'image': str(path), 'gotplt': got,
                                   'loads': [dict(off=0, vaddr=base, filesz=0x30000, memsz=0x30000, flags=5)]}]}
            tables = lift.JumpTables(layout, insns)
            self.assertEqual(tables(base + 0x1e), [base + 0x20, base + 0x21, base + 0x22])
            code, count = lift.lift(base + 0x10, insns, tables=tables)
        self.assertIn(f'case {base + 0x21:#x}u: goto L{base + 0x21:x};', code)
        self.assertIn('default: c->flags = f; rt_jump_any(c, a); return;', code)
        self.assertEqual(count, 9)

    def test_jump_table_with_bad_declared_case_is_not_guessed(self):
        insns = {0x10: (0x13, 'cmp', 'eax, 0x1'), 0x13: (0x15, 'ja', '0x40'),
                 0x15: (0x1c, 'jmp', 'dword ptr [4*eax + 0x1000]'), 0x40: (0x41, 'ret', ''),
                 0x20: (0x21, 'ret', '')}
        tables = lift.JumpTables({'modules': []}, insns)
        tables.read32 = lambda address: {0x1000: 0x20, 0x1004: 0x25}.get(address)  # 0x25: not code
        self.assertIsNone(tables(0x15))
        code, _ = lift.lift(0x10, insns, tables=tables)
        self.assertNotIn('switch', code)

    def test_setjmp_return_site_is_a_resume_point(self):
        insns = {0x10: (0x15, 'call', '0x40'), 0x15: (0x1a, 'call', '0x50'), 0x1a: (0x1b, 'ret', '')}
        code, _ = lift.lift(0x10, insns, setjmp=lambda target: target == 0x40)
        self.assertIn('if (c->eip != 0x15u) goto resume;', code)
        self.assertIn('if (c->eip != 0x1au) goto resume;', code)
        self.assertIn('switch (c->eip) { case 0x15u: goto L15; default: return; }', code)
        plain, _ = lift.lift(0x10, insns)
        self.assertIn('resume: return;', plain)
        self.assertNotIn('resume', lift.lift(0x10, {0x10: (0x11, 'ret', '')})[0])

    def test_plt_stub_resolves_to_import(self):
        insns = {0x100: (0x106, 'jmp', 'dword ptr [ebx + 0xc]'), 0x200: (0x206, 'jmp', 'dword ptr [0x3000]')}
        tables = lift.JumpTables({'modules': []}, insns)
        tables.module = lambda address: {'gotplt': 0x2000}
        tables.read32 = lambda address: {0x200c: 0xf0000010, 0x3000: 0xf0000020}.get(address)
        imports = {0xf0000010: '_setjmp', 0xf0000020: 'longjmp'}
        self.assertEqual(lift.import_called(0x100, insns, imports, tables), '_setjmp')
        self.assertEqual(lift.import_called(0x200, insns, imports, tables), 'longjmp')
        self.assertEqual(lift.import_called(0xf0000010, insns, imports, tables), '_setjmp')
        self.assertIsNone(lift.import_called(0x300, insns, imports, tables))

    def test_audit_scan_collects_every_rejection(self):
        insns = {0x10: (0x12, 'rsm', ''), 0x12: (0x14, 'vzeroall', ''), 0x14: (0x15, 'ret', '')}
        count, failures, indirect = lift.scan(0x10, insns)
        self.assertEqual(count, 3)
        self.assertEqual([op for _, op, _, _ in failures], ['rsm', 'vzeroall'])
        with self.assertRaises(lift.Unsupported):   # the generator itself stays fail-closed
            lift.lift(0x10, insns)


class ResolveExportTests(unittest.TestCase):
    """lift.resolve_export on a synthetic layout: the four spec forms and their errors."""
    LAYOUT = {'imports': [], 'modules': [
        {'name': 'libfake.so', 'base': 0x10000000, 'init': 0x10000500, 'init_array': [0x10000600, 0x10000700],
         'names': {'0x10000100': 'f', '0x10000200': 'g', '0x10000210': 'g'},
         'exports': {'f': 0x10000100, 'both': 0x10000300},
         'loads': [dict(off=0, vaddr=0x10000000, filesz=0x1000, memsz=0x1000, flags=5),
                   dict(off=0x1000, vaddr=0x10001000, filesz=0x1000, memsz=0x2000, flags=6)]},
        {'name': 'libother.so', 'base': 0x20000000, 'init': 0, 'init_array': [], 'names': {},
         'exports': {'both': 0x20000300},
         'loads': [dict(off=0, vaddr=0x20000000, filesz=0x1000, memsz=0x1000, flags=5)]}]}

    def test_export_form(self):
        self.assertEqual(lift.resolve_export(self.LAYOUT, 'f'), 0x10000100)
        with self.assertRaisesRegex(lift.Unsupported, 'found 0'):
            lift.resolve_export(self.LAYOUT, 'missing')
        with self.assertRaisesRegex(lift.Unsupported, 'found 2'):
            lift.resolve_export(self.LAYOUT, 'both')          # defined by two modules

    def test_symbol_form(self):
        self.assertEqual(lift.resolve_export(self.LAYOUT, 'libfake.so:symbol:f'), 0x10000100)
        with self.assertRaisesRegex(lift.Unsupported, 'found 2'):
            lift.resolve_export(self.LAYOUT, 'libfake.so:symbol:g')   # two addresses carry the name
        with self.assertRaisesRegex(lift.Unsupported, 'found 0'):
            lift.resolve_export(self.LAYOUT, 'nope.so:symbol:f')

    def test_code_form(self):
        self.assertEqual(lift.resolve_export(self.LAYOUT, 'libfake.so:code:0x40'), 0x10000040)
        self.assertEqual(lift.resolve_export(self.LAYOUT, 'libfake.so:code:64'), 0x10000040)
        with self.assertRaisesRegex(lift.Unsupported, 'outside executable'):
            lift.resolve_export(self.LAYOUT, 'libfake.so:code:0x1000')   # writable segment
        with self.assertRaisesRegex(lift.Unsupported, 'outside executable'):
            lift.resolve_export(self.LAYOUT, 'nope.so:code:0x40')
        with self.assertRaisesRegex(lift.Unsupported, 'invalid code offset'):
            lift.resolve_export(self.LAYOUT, 'libfake.so:code:zz')

    def test_init_form(self):
        self.assertEqual(lift.resolve_export(self.LAYOUT, 'libfake.so:init:dt'), 0x10000500)
        self.assertEqual(lift.resolve_export(self.LAYOUT, 'libfake.so:init:1'), 0x10000700)
        with self.assertRaisesRegex(lift.Unsupported, 'unknown module'):
            lift.resolve_export(self.LAYOUT, 'nope.so:init:dt')
        for bad in ('libfake.so:init:2', 'libfake.so:init:x'):
            with self.assertRaisesRegex(lift.Unsupported, 'unknown constructor'):
                lift.resolve_export(self.LAYOUT, bad)

    def test_owner_follows_executable_segments(self):
        self.assertEqual(lift.owner(self.LAYOUT, 0x10000fff)['name'], 'libfake.so')
        self.assertEqual(lift.owner(self.LAYOUT, 0x20000000)['name'], 'libother.so')
        for outside in (0x0fffffff, 0x10001000, 0x10001800, 0x20001000):
            self.assertIsNone(lift.owner(self.LAYOUT, outside))


if __name__ == '__main__':
    unittest.main()
