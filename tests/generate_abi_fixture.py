"""Emit small instruction graphs to exercise ABI cases absent from codec exports."""
from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'tools'))
import lift  # noqa: E402

# Addresses are intentionally synthetic; production binaries still go through
# the real objdump/ELF pipeline in the separate tier0 smoke suite.
programs = {
    0x1000: [('mov', 'eax, 0xffffffff'), ('add', 'eax, 1'), ('call', '0x2000'),
             ('adc', 'eax, 0'), ('ret', '')],
    0x2000: [('mov', 'edx, dword ptr [esp]'), ('ret', '')],
    0x3000: [('push', '2'), ('push', '3'), ('mov', 'ecx, 0x4000'),
             ('call', 'ecx'), ('ret', '')],
    0x4000: [('mov', 'eax, dword ptr [esp + 4]'), ('add', 'eax, dword ptr [esp + 8]'),
             ('ret', '8')],
    0x5000: [('mov', 'ecx, 0x6000'), ('jmp', 'ecx')],
    0x6000: [('mov', 'eax, 99'), ('ret', '')],
    0x7000: [('jmp', '0x6000')],
    0x8000: [('cmp', 'eax, eax'), ('call', '0x9000'), ('setne', 'al'), ('ret', '')],
    0x9000: [('cmp', 'eax, 123'), ('ret', '')],
    0xa000: [('mov', 'eax, 4'), ('call', '0xb000'), ('ret', '')],
    0xb000: [('cmp', 'eax, 0'), ('je', '0xb007'), ('push', 'eax'), ('sub', 'eax, 1'),
             ('call', '0xb000'), ('pop', 'ecx'), ('add', 'eax, ecx'), ('ret', '')],
    0xc000: [('mov', 'eax, 7'), ('mov', 'edx, 9'), ('cmp', 'eax, eax'),
             ('xchg', 'dword ptr [0x20001000], edx'), ('setne', 'bl'),
             ('mov', 'ecx, edx'), ('mov', 'eax, 9'), ('mov', 'edx, 11'),
             ('lock_cmpxchg', 'dword ptr [0x20001000], edx'), ('sete', 'bl'), ('ret', '')],
    0xd000: [('mov', 'eax, 8'), ('mov', 'edx, 12'),
             ('lock_cmpxchg', 'dword ptr [0x20001000], edx'), ('sete', 'bl'), ('ret', '')],
    0xe000: [('mov', 'esi, 0x20002000'), ('mov', 'edi, 0x20003000'), ('mov', 'ecx, 4'),
             ('rep', 'movsb byte ptr es:[edi], byte ptr [esi]'), ('ret', '')],
    0xf000: [('std', ''), ('mov', 'esi, 0x20002003'), ('mov', 'edi, 0x20003003'), ('mov', 'ecx, 4'),
             ('rep', 'movsb byte ptr es:[edi], byte ptr [esi]'), ('cld', ''), ('ret', '')],
    0x10000: [('mov', 'edi, 0x20003000'), ('mov', 'eax, 0x12345678'), ('mov', 'ecx, 3'),
              ('rep', 'stosd dword ptr es:[edi], eax'), ('ret', '')],
    0x11000: [('mov', 'edi, 0x20002000'), ('mov', 'eax, 98'), ('mov', 'ecx, 4'),
              ('repne', 'scasb al, byte ptr es:[edi]'), ('ret', '')],
    0x12000: [('mov', 'esi, 0'), ('mov', 'edi, 0'), ('mov', 'ecx, 0'),
              ('rep', 'movsd dword ptr es:[edi], dword ptr [esi]'), ('ret', '')],
    0x13000: [('mov', 'esi, 0x20002000'), ('mov', 'edi, 0x20003000'), ('mov', 'ecx, 4'),
              ('repe', 'cmpsb byte ptr [esi], byte ptr es:[edi]'), ('ret', '')],
    0x14000: [('mov', 'eax, 0x12345678'), ('push', 'ax'), ('mov', 'eax, 0'),
              ('pop', 'ax'), ('ret', '')],
    0x15000: [('mov', 'eax, 2'), ('cvtsi2sd', 'xmm0, eax'),
              ('addsd', 'xmm0, qword ptr [0x20004000]'),
              ('movsd', 'qword ptr [0x20005000], xmm0'), ('cvttsd2si', 'eax, xmm0'), ('ret', '')],
    0x16000: [('movaps', 'xmm0, xmmword ptr [0x20004010]'),
              ('movsd', 'xmm1, qword ptr [0x20004000]'), ('movsd', 'xmm0, xmm1'),
              ('movups', 'xmmword ptr [0x20005010], xmm0'),
              ('mov', 'eax, 0x11223344'), ('movd', 'xmm2, eax'),
              ('movq', 'qword ptr [0x20005008], xmm2'), ('pxor', 'xmm3, xmm3'),
              ('comisd', 'xmm3, xmm1'), ('ret', '')],
    0x17000: [('fld', 'qword ptr [0x20004000]'), ('fld1', ''), ('faddp', 'st(1), st'),
              ('fstp', 'qword ptr [0x20005020]'), ('fild', 'dword ptr [0x20004008]'),
              ('fchs', ''), ('fstp', 'dword ptr [0x20005028]'), ('ret', '')],
    0x19000: [('lock_add', 'dword ptr [0x20007000], 0xffffffff'), ('setb', 'cl'),
              ('sete','dl'), ('mov','eax, 0'), ('adc','eax, 0'), ('ret','')],
    0x1a000: [('mov','eax, 5'), ('lock_xadd','dword ptr [0x20007000], eax'), ('ret','')],
    0x1b000: [('mov','eax, 0x55667788'), ('mov','edx, 0x11223344'),
              ('mov','ebx, 0xeeff0011'), ('mov','ecx, 0xaabbccdd'),
              ('lock_cmpxchg8b','qword ptr [0x20007008]'), ('ret','')],
    0x1c000: [('mov','eax, 0'), ('mov','edx, 0'), ('cmp','eax, 1'),
              ('lock_cmpxchg8b','qword ptr [0x20007008]'), ('ret','')],
    0x1d000: [('mov','eax, dword ptr gs:[0]'), ('mov','edx, dword ptr gs:[0x14]'), ('ret','')],
    0x1e000: [('mov','eax, 100'), ('lea','eax, gs:[eax + 4]'), ('ret','')],
    0x1f000: [('call','0x1f001'), ('pop','eax'), ('ret','')],
    0x20000: [('movupd','xmm0, xmmword ptr [0x20008000]'),
              ('cmplepd','xmm0, xmmword ptr [0x20008010]'), ('ret','')],
    0x21000: [('movupd','xmm0, xmmword ptr [0x20008000]'),
              ('cmpnltpd','xmm0, xmmword ptr [0x20008010]'), ('ret','')],
    0x22000: [('movupd','xmm0, xmmword ptr [0x20008000]'),
              ('cvttpd2dq','xmm0, xmm0'), ('ret','')],
    0x23000: [('movupd','xmm0, xmmword ptr [0x20008000]'),
              ('psrlq','xmm0, 64'), ('ret','')],
    0x24000: [('movupd','xmm0, xmmword ptr [0x20008000]'),
              ('cvtpd2dq','xmm0, xmm0'), ('ret','')],
}
def assembled(base, items):
    labels = {}; count = 0
    for item in items:
        if isinstance(item, str): labels[item] = base + count
        else: count += 1
    return [(op, hex(labels[arg[1:]]) if arg.startswith('@') else arg)
            for op, arg in (item for item in items if not isinstance(item,str))]

programs[0x25000] = assembled(0x25000, [
    ('push','esi'), ('mov','esi, dword ptr [esp + 8]'),
    ('lock_add','dword ptr [0x2000b080], 1'),
    'spin', ('cmp','dword ptr [0x2000b084], 0'), ('je','@spin'),
    ('call','0xf1000050'), ('mov','dword ptr [esi + 4], eax'),
    ('call','0xf1000070'), ('mov','dword ptr [esi + 8], eax'), ('mov','dword ptr [eax], esi'),
    ('stmxcsr','dword ptr [esi + 16]'),
    ('mov','eax, dword ptr [0x2000b088]'), ('push','esi'), ('push','eax'), ('call','0xf1000040'), ('add','esp, 8'),
    ('push','0x27000'), ('push','0x2000b090'), ('call','0xf1000080'), ('add','esp, 8'),
    ('push','0x2000b000'), ('call','0xf1000000'), ('add','esp, 4'),
    ('add','dword ptr [0x2000b08c], 1'),
    'wait', ('cmp','dword ptr [0x2000b084], 2'), ('je','@work'),
    ('push','0x2000b000'), ('push','0x2000b020'), ('call','0xf1000020'), ('add','esp, 8'), ('jmp','@wait'),
    'work', ('add','dword ptr [0x2000b094], 1'),
    ('push','0x2000b000'), ('call','0xf1000010'), ('add','esp, 4'),
    ('mov','eax, esi'), ('pop','esi'), ('ret','')])
programs[0x26000] = [('add','dword ptr [0x2000b098], 1'), ('ret','')]
programs[0x27000] = [('add','dword ptr [0x2000b09c], 1'), ('ret','')]
programs[0x28000] = [('lock_add','dword ptr [0x2000b104], 1'),
    ('push','0'), ('push','0'), ('push','128'), ('push','0x2000b100'), ('push','240'),
    ('call','0xf1000090'), ('add','esp, 20'), ('ret','')]
fixture_imports = {0xf1000000:'pthread_mutex_lock', 0xf1000010:'pthread_mutex_unlock',
    0xf1000020:'pthread_cond_wait',0xf1000040:'pthread_setspecific',
    0xf1000050:'pthread_self',0xf1000070:'__errno_location',0xf1000080:'pthread_once',0xf1000090:'syscall',
    0xf10000a0:'_setjmp',0xf10000b0:'longjmp'}
insns = {base + i: (base + i + 1, op, args)
         for base, instructions in programs.items() for i, (op, args) in enumerate(instructions)}

# Real x86 bytes: tests/lifter_programs.s is assembled for i386 Linux and goes
# through the same objdump parser as the game modules.
import re
import subprocess
ASM_BASE = 0x400000
source = Path(__file__).with_name('lifter_programs.s')
obj = Path(sys.argv[1]).with_name('lifter_programs.o')
subprocess.run(['clang', '-target', 'i386-linux-gnu', '-c', str(source), '-o', str(obj)], check=True)
asm_insns = lift.disassemble(obj, ASM_BASE, 'objdump')
table = subprocess.run(['objdump', '-t', str(obj)], check=True, capture_output=True, text=True).stdout
asm_entries = {m[2]: int(m[1], 16) + ASM_BASE
               for m in re.finditer(r'^([0-9a-f]{8}) g\s+\S*\s+\.text\s+[0-9a-f]+ (\S+)$', table, re.M)}
assert asm_entries, 'no global symbols in lifter_programs.s'
insns.update(asm_insns)
labels = {name: address for name, address in asm_entries.items() if name.startswith('lfl_')}
functions = {name: address for name, address in asm_entries.items() if name.startswith('lf_')}
entries = set(programs) | set(functions.values())

# Jump tables: the guest table at 0x20018810 is written by checks_lifter.c with
# the three case labels; the generator sees the same contents.
class FixtureTables(lift.JumpTables):
    def read32(self, address):
        table = {0x20018810 + 4 * i: labels[f'lfl_case{i}'] for i in range(3)}
        table.update({0x20018a00: 0xf10000a0, 0x20018a04: 0xf10000b0})   # GOT slots of the PLT stubs
        return table.get(address)
tables = FixtureTables({'modules': []}, insns)
# One synthetic guest module for the loader checks (image built in guest
# memory at 0x2001c000 by checks_lifter.c).
f = functions
code = ['#include "modules.h"',
        f'static const uint32_t fake_constructors[] = {{{f["lf_log_ctor"]:#x}u}};',
        'static const GuestSymbol fake_symbols[] = {{"fake_symbol", 0x2001c400u}, {NULL, 0}};',
        'const GuestModule rt_modules[1] = {{"libfake.so", 7, 0x2001c000u, 0x2001c000u, '
        f'{f["lf_log_init"]:#x}u, {f["lf_log_fini"]:#x}u, fake_constructors, 1, fake_symbols, 1, NULL, 0}}}};',
        'const size_t rt_module_count = 1;']
is_setjmp = lambda target: lift.import_called(target, insns, fixture_imports, tables) in lift.SETJMP_IMPORTS
bodies = [lift.lift(entry, insns, entries, set(), tables, is_setjmp)[0] for entry in sorted(entries)]
code += lift.prototypes(entries, static=True)
code += [lift.link_direct(body, set(entries))[0] for body in bodies]
code.append('uint32_t lifter_entry(const char *name);')
code.append('uint32_t lifter_entry(const char *name) {')
code += [f'if (!strcmp(name, "{name}")) return {address:#x}u;' for name, address in sorted(asm_entries.items())]
code.append('return 0; }')
code.append('gfn rt_lookup(uint32_t target) { switch (target) {')
for entry in sorted(entries):
    code.append(f'case {entry:#x}: return fn_{entry:x};')
code += ['default: return NULL; } }',
         'const char *rt_import_name(uint32_t target) { switch(target) {' + ''.join(f'case {address:#x}u: return {name!r};'.replace(chr(39),chr(34)) for address,name in fixture_imports.items()) + 'default: return NULL; } }']
Path(sys.argv[1]).write_text('\n'.join(code) + '\n')
