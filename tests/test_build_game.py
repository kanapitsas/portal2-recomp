from pathlib import Path
import subprocess
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
import build_game  # noqa: E402
import lift  # noqa: E402

FAKE_MODULE = dict(name='libfake.so', modid=1, base=0x10000000, entry=0, init=0, fini=0, init_array=[],
                   exports={}, names={}, needed=[], loads=[dict(off=0, vaddr=0x10000000, filesz=0x40000,
                                                             memsz=0x40000, flags=5)])


class DispatchTableTests(unittest.TestCase):
    def test_lookup_finds_every_function_and_nothing_else(self):
        functions = sorted({0x10000000, 0x10000010, 0x10000ff0, 0x10001000, 0x10003004, 0x1003fffc}
                           | {0x10010000 + 7 * i for i in range(300)})
        layout = dict(modules=[FAKE_MODULE], imports=[dict(name='puts', addr=0xf0000000)], data_imports=[])
        dispatch = lift.dispatch_source(layout, [('named', 0x10000010)], {0xf0000000: 'puts'}, functions)
        probes = sorted(set(functions) | {f + d for f in functions for d in (-1, 1)} |
                        {0, 0x0fffffff, 0x10040000, 0xffffffff})
        harness = ['#include "rt.h"', '#include <stdio.h>']
        harness += [f'void fn_{f:x}(CPU *c) {{ (void)c; }}' for f in functions]
        harness += ['int rt_load_segment(const char *d, const char *n, uint32_t o, uint32_t a, uint32_t f, uint32_t m)'
                    ' { (void)d; (void)n; (void)o; (void)a; (void)f; (void)m; return 0; }',
                    'int rt_register_tls(const char *d, const char *n, uint32_t i, uint32_t o, uint32_t f, uint32_t m,'
                    ' uint32_t a) { (void)d; (void)n; (void)i; (void)o; (void)f; (void)m; (void)a; return 0; }',
                    'int rt_init_import_data(const char *n, uint32_t a, uint32_t s) { (void)n; (void)a; (void)s; return 0; }',
                    'int main(void) { int bad = 0;']
        for probe in probes:
            expected = f'fn_{probe:x}' if probe in functions else 'NULL'
            harness.append(f'if (rt_lookup({probe:#x}u) != {expected}) {{ printf("%x\\n", {probe:#x}u); ++bad; }}')
        harness += ['if (rt_export("named") != 0x10000010u || rt_export("other")) ++bad;',
                    'if (strcmp(rt_import_name(0xf0000000u), "puts") || rt_import_name(0xf0000010u)) ++bad;',
                    'return bad; }']
        with tempfile.TemporaryDirectory() as tmp:
            (Path(tmp) / 'dispatch.c').write_text(dispatch)
            (Path(tmp) / 'harness.c').write_text('\n'.join(harness))
            subprocess.run(['clang', '-O1', '-Wall', '-Wextra', '-Werror', f'-I{ROOT / "runtime"}',
                            str(Path(tmp) / 'dispatch.c'), str(Path(tmp) / 'harness.c'), '-o', str(Path(tmp) / 't')],
                           check=True)
            result = subprocess.run([str(Path(tmp) / 't')], capture_output=True, text=True)
            self.assertEqual(result.returncode, 0, result.stdout)

    def test_shard_depends_only_on_guest_address_range(self):
        self.assertEqual(lift.shard_name(FAKE_MODULE, 0x10000000), lift.shard_name(FAKE_MODULE, 0x1001ffff))
        self.assertNotEqual(lift.shard_name(FAKE_MODULE, 0x1001ffff), lift.shard_name(FAKE_MODULE, 0x10020000))

    def test_module_roots_keep_only_executable_addresses(self):
        module = dict(FAKE_MODULE, entries=[0x10000010], data_ptrs=[0x10000020, 0x20000000],
                      exports={'f': 0x10000030}, init_array=[0x10000040, 0xffffffff], fini_array=[0],
                      init=0x10000050, fini=0)
        self.assertEqual(lift.module_roots(module), [0x10000010, 0x10000020, 0x10000030, 0x10000040, 0x10000050])

    def test_code_pointers_in_data_are_roots_but_not_function_boundaries(self):
        # Les cibles de tables de switch sont dans data_ptrs : compilées, mais un
        # saut interne vers elles doit rester un goto local.
        module = dict(FAKE_MODULE, entries=[0x10000010], data_ptrs=[0x10000020],
                      exports={'f': 0x10000030}, init_array=[0x10000040], fini_array=[],
                      init=0x10000050, fini=0)
        self.assertIn(0x10000020, lift.module_roots(module))
        self.assertEqual(lift.module_boundaries(module), {0x10000010, 0x10000030, 0x10000040, 0x10000050})


class CompileCacheTests(unittest.TestCase):
    def test_unchanged_sources_are_not_recompiled_and_headers_count(self):
        with tempfile.TemporaryDirectory() as tmp:
            tmp = Path(tmp)
            (tmp / 'inc.h').write_text('#define VALUE 1\n')
            (tmp / 'a.c').write_text('#include "inc.h"\nint a(void) { return VALUE; }\n')
            (tmp / 'b.c').write_text('int b(void) { return 2; }\n')
            units = [(str(tmp / 'a.c'), ['-O0']), (str(tmp / 'b.c'), ['-O0'])]
            cache = tmp / 'obj'
            objects, compiled, _ = build_game.compile_units(units, cache, 2, 'clang-test')
            self.assertEqual(compiled, 2)
            self.assertTrue(all(o.exists() for o in objects))
            self.assertEqual(build_game.compile_units(units, cache, 2, 'clang-test')[1], 0)
            (tmp / 'inc.h').write_text('#define VALUE 3\n')            # a.c only
            self.assertEqual(build_game.compile_units(units, cache, 2, 'clang-test')[1], 1)
            units[1] = (str(tmp / 'b.c'), ['-O1'])                       # flags are part of the key
            self.assertEqual(build_game.compile_units(units, cache, 2, 'clang-test')[1], 1)
            self.assertEqual(build_game.compile_units(units, cache, 2, 'other-compiler')[1], 2)

    def test_compile_errors_fail_the_build(self):
        with tempfile.TemporaryDirectory() as tmp:
            (Path(tmp) / 'bad.c').write_text('int x = ;\n')
            with self.assertRaises(SystemExit):
                build_game.compile_units([(str(Path(tmp) / 'bad.c'), [])], Path(tmp) / 'obj', 1, 'clang')
            self.assertEqual(list((Path(tmp) / 'obj').glob('*.o')), [])


class LazyInstructionTests(unittest.TestCase):
    def test_modules_are_loaded_on_first_use_and_dropped(self):
        from unittest.mock import patch
        other = dict(FAKE_MODULE, name='libother.so', base=0x20000000,
                     loads=[dict(off=0, vaddr=0x20000000, filesz=0x100, memsz=0x100, flags=5)])
        decoded = {'libfake.so': {0x10000000: (0x10000001, 'ret', '')},
                   'libother.so': {0x20000000: (0x20000002, 'nop', ''), 0x20000002: (0x20000003, 'ret', '')}}
        with patch.object(lift, 'module_instructions', side_effect=lambda m, *_: decoded[m['name']]):
            insns = lift.LazyInsns(dict(modules=[FAKE_MODULE, other]), 'objdump', None)
            insns.load(FAKE_MODULE)
            self.assertNotIn(0x30000000, insns)
            self.assertIn(0x20000002, insns)
            self.assertEqual(insns.previous[0x20000002], 0x20000000)
            insns.drop_transient()
            self.assertEqual(dict(insns), decoded['libfake.so'])
            self.assertEqual(insns[0x20000000], (0x20000002, 'nop', ''))



class LayoutLoadingTests(unittest.TestCase):
    def test_old_relative_image_paths_resolve_next_to_the_layout(self):
        import json
        with tempfile.TemporaryDirectory() as tmp:
            prep = Path(tmp).resolve() / 'prep'
            prep.mkdir()
            (prep / 'libfake.so').write_bytes(b'\x7fELF')
            modules = [dict(name='libfake.so', image='build/game/prep/libfake.so'),       # pre-November 2025 layout
                       dict(name='libgone.so', image='build/game/prep/libgone.so'),       # not beside the layout
                       dict(name='libabs.so', image=str(prep / 'libabs.so'))]
            (prep / 'layout.json').write_text(json.dumps(dict(modules=modules, imports=[])))
            layout = lift.load_layout(prep / 'layout.json')
        images = [m['image'] for m in layout['modules']]
        self.assertEqual(images[0], str(prep / 'libfake.so'))
        self.assertEqual(images[1], str(ROOT / 'build/game/prep/libgone.so'))
        self.assertEqual(images[2], str(prep / 'libabs.so'))


class SymbolScopeTests(unittest.TestCase):
    def test_dlopened_modules_prefer_own_copy_over_sibling(self):
        sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
        import elfprep
        class Fake:
            def __init__(self, name, needed): self.name, self.needed = name, needed
        program = Fake('portal2_linux', ['libtier0.so'])
        tier0 = Fake('libtier0.so', [])
        client = Fake('client.so', ['libtier0.so'])
        server = Fake('server.so', ['libtier0.so'])
        scopes = elfprep.lookup_scopes([tier0, client, server, program])
        # Portée globale d'abord, puis le module lui-même : jamais client.so pour server.so.
        self.assertEqual([m.name for m in scopes['server.so']], ['portal2_linux', 'libtier0.so', 'server.so'])
        self.assertNotIn('client.so', [m.name for m in scopes['server.so']])
        self.assertEqual([m.name for m in scopes['portal2_linux']], ['portal2_linux', 'libtier0.so'])


if __name__ == '__main__':
    unittest.main()
