#!/usr/bin/env python3
"""Strict integer i386 -> C AOT spike. Unsupported reachable instructions are errors.

The generated C contains one native function per requested export and direct C
branches for guest control flow. It never fetches or interprets guest opcodes.
Direct calls are followed transitively; indirect calls/tail jumps use a checked
dispatch table. SSE and x87 are outside this milestone.
"""
import argparse
import json
import hashlib
import marshal
import os
import shutil
from pathlib import Path
import re
import subprocess
import float_lift
import registry

REGS = {r: (r, 32, 0) for r in 'eax ecx edx ebx esp ebp esi edi'.split()}
for full, low, high, word in [('eax', 'al', 'ah', 'ax'), ('ebx', 'bl', 'bh', 'bx'),
                              ('ecx', 'cl', 'ch', 'cx'), ('edx', 'dl', 'dh', 'dx')]:
    REGS.update({low: (full, 8, 0), high: (full, 8, 8), word: (full, 16, 0)})
REGS.update({r[1:]: (r, 16, 0) for r in ['esp', 'ebp', 'esi', 'edi']})
CC = dict(o='O', no='NO', b='B', c='B', nae='B', ae='AE', nb='AE', nc='AE',
          e='E', z='E', ne='NE', nz='NE', be='BE', na='BE', a='A', nbe='A',
          s='S', ns='NS', p='P', pe='P', np='NP', po='NP', l='L', nge='L',
          ge='GE', nl='GE', le='LE', ng='LE', g='G', nle='G')


class Unsupported(ValueError):
    pass


# Greffons PE (generate_pe) : un mot couvert par une relocation de base est
# marqué `@0x<valeur au ImageBase préféré>` dans le texte désassemblé ; il vaut
# `valeur + d_` à l'exécution. libMiles projette l'image deux fois (vue
# écrivable et vue exécutable d'un même fichier) : une valeur qui désigne
# .text est relocalisée vers la vue exécutable (`@c0x...`, `valeur + x_`), les
# autres vers la vue écrivable (d_). Les adresses de code émises (appels,
# retours, tables de switch) utilisent x_. Voir lift() et runtime/pe.c.
RELOCATED = 'd_'
CODE_RELOCATED = 'x_'


def relocated(token):
    """`@0x...` (donnée) ou `@c0x...` (code) -> expression C relogée."""
    code = token.startswith('@c')
    return f'({int(token[2 if code else 1:], 0):#x}u + {CODE_RELOCATED if code else RELOCATED})'


def address(expr):
    """Accept only sums/differences of registers, scaled registers and constants."""
    tokens = re.findall(r'@c?0x[0-9a-f]+|0x[0-9a-f]+|[a-z]+|[0-9]+|[-+*]', expr)
    if ''.join(tokens) != re.sub(r'\s+', '', expr):
        raise Unsupported(f'address expression {expr!r}')
    parts = re.split(r'(?=[+-])', ''.join(tokens))
    out = []
    for part in filter(None, parts):
        sign = '-' if part.startswith('-') else '+'
        factors = part.lstrip('+-').split('*')
        values = []
        if len(factors) > 2:
            raise Unsupported(expr)
        for atom in factors:
            if atom in REGS and REGS[atom][1] == 32:
                values.append('c->' + atom)
            elif atom == 'eiz':
                values.append('0u')
            elif atom.startswith('@') and sign == '+' and len(factors) == 1:
                values.append(relocated(atom))
            else:
                try:
                    values.append(f'{int(atom, 0):#x}u')
                except ValueError:
                    raise Unsupported(expr) from None
        out.append(sign + '*'.join(values))
    if not out:
        raise Unsupported(expr)
    return '(uint32_t)(0u' + ''.join(out) + ')'


class Operand:
    def __init__(self, text):
        self.text = text
        self.reg = REGS.get(text)
        self.addr = None
        self.width = self.reg[1] if self.reg else None
        m = re.fullmatch(r'(?:(byte|word|dword|qword|xmmword) ptr )?(?:(gs|fs):)?\[(.*)\]', text)
        if m:
            if m[2] == 'fs': raise Unsupported('FS segment is not implemented')
            self.effective_addr = address(m[3])
            self.addr = f'(uint32_t)(rt_thread_pointer(c) + {self.effective_addr})' if m[2] == 'gs' else self.effective_addr
            self.width = {None: None, 'byte': 8, 'word': 16, 'dword': 32, 'qword': 64, 'xmmword': 128}[m[1]]
        self.imm = None
        self.relocated = None
        if not self.reg and self.addr is None and re.fullmatch(r'@c?0x[0-9a-f]+', text):
            self.relocated = text   # immédiat relocalisé : pas une constante
            return
        if not self.reg and self.addr is None:
            try:
                self.imm = int(text, 0) & 0xffffffff
            except ValueError:
                raise Unsupported(f'operand {text!r}') from None

    def read(self):
        if self.reg:
            full, width, shift = self.reg
            return f'((c->{full} >> {shift}) & {((1 << width) - 1):#x}u)'
        if self.addr is not None and self.width:
            return f'{"RX" if self.width == 128 else "R" + str(self.width)}({self.addr})'
        if self.imm is not None:
            return f'{self.imm:#x}u'
        if self.relocated is not None:
            return f'(uint32_t){relocated(self.relocated)}'
        raise Unsupported(self.text)

    def write(self, value):
        if self.reg:
            full, width, shift = self.reg
            mask = ((1 << width) - 1) << shift
            if width == 32:
                return f'c->{full} = (uint32_t)({value});'
            return (f'c->{full} = (c->{full} & {~mask & 0xffffffff:#x}u) | '
                    f'(((uint32_t)({value}) << {shift}) & {mask:#x}u);')
        if self.addr is not None and self.width:
            return f'{"WX" if self.width == 128 else "W" + str(self.width)}({self.addr}, {value});'
        raise Unsupported(f'destination {self.text!r}')


def disassemble(image, base, objdump, start=None, stop=None):
    window = [f'--start-address={start:#x}', f'--stop-address={stop:#x}'] if start is not None else []
    result = subprocess.run([objdump, '-d', '-z', '--x86-asm-syntax=intel',
                             f'--adjust-vma={base:#x}', *window, str(image)],
                            check=True, capture_output=True, text=True)
    insns = {}
    for line in result.stdout.splitlines():
        m = re.match(r'^\s*([0-9a-f]+):\s+((?:[0-9a-f]{2}\s+)+)\s*([a-z][a-z0-9]*)\s*(.*?)\s*$', line)
        if m:
            pc = int(m[1], 16)
            size = len(m[2].split())
            args = re.sub(r'\s*<.*$', '', m[4]).split('#', 1)[0].strip()
            insns[pc] = (pc + size, m[3], args)
    # LLVM sometimes emits LOCK as a separate disassembly line. It belongs to
    # the following instruction; the byte after LOCK is not a second entry.
    for pc, (nxt, op, args) in list(insns.items()):
        if op == 'lock' and not args and nxt in insns:
            end, following, operands = insns.pop(nxt)
            insns[pc] = (end, 'lock_' + following, operands)
    return insns


def string_instruction(op, args, nxt):
    repeat = None
    if op in ('rep', 'repe', 'repz', 'repne', 'repnz'):
        repeat = op
        fields = args.split(None, 1)
        if not fields:
            raise Unsupported('empty string prefix')
        op, args = fields[0], fields[1] if len(fields) > 1 else ''
    if op not in ('movsb', 'movsw', 'movsd', 'stosb', 'stosw', 'stosd',
                  'lodsb', 'lodsw', 'lodsd', 'scasb', 'scasw', 'scasd',
                  'cmpsb', 'cmpsw', 'cmpsd'):
        return None
    if 'xmm' in args:  # SSE MOVSD/CMPSD are distinct opcodes
        return None
    if any(segment in args for segment in ('gs:', 'fs:', 'cs:', 'ss:')):
        raise Unsupported('non-flat string segment')
    width = {'b': 8, 'w': 16, 'd': 32}[op[-1]]
    size = width // 8
    kind = op[:-1]
    body = []
    if kind == 'movs':
        body.append(f'a = R{width}(c->esi); W{width}(c->edi, a);')
    elif kind == 'stos':
        body.append(f'W{width}(c->edi, c->eax);')
    elif kind == 'lods':
        body.append(f'a = R{width}(c->esi); ' + Operand({8: 'al', 16: 'ax', 32: 'eax'}[width]).write('a'))
    elif kind == 'scas':
        body.append(f'f_sub(&f, c->eax, R{width}(c->edi), {width});')
    elif kind == 'cmps':
        body.append(f'f_sub(&f, R{width}(c->esi), R{width}(c->edi), {width});')
    step = f'(c->df ? (0u - {size}u) : {size}u)'
    if kind in ('movs', 'lods', 'cmps'):
        body.append(f'c->esi += {step};')
    if kind in ('movs', 'stos', 'scas', 'cmps'):
        body.append(f'c->edi += {step};')
    if repeat:
        body.append('--c->ecx;')
        if kind in ('cmps', 'scas'):
            body.append('if (' + ('f.zf' if repeat in ('repne', 'repnz') else '!f.zf') + ') break;')
        text = 'while (c->ecx) { ' + ' '.join(body) + ' }'
    else:
        text = ' '.join(body)
    return text + f' goto L{nxt:x};', [nxt]


def code_address(address, rebase):
    """Adresse invitée d'un point de code : constante, ou relative au module
    (greffons PE chargés à une base choisie à l'exécution)."""
    return f'({address:#x}u + {CODE_RELOCATED})' if rebase else f'{address:#x}u'


def translate(pc, insn, entries=frozenset(), callees=None, cases=None, rebase=False):
    if callees is None:
        callees = set()
    nxt, op, args = insn
    end = f'goto L{nxt:x};'
    here, after = code_address(pc, rebase), code_address(nxt, rebase)
    if op in ('int3', 'ud2', 'hlt'):
        return f'rt_trap(c, {here}, "{op}");', []
    string = string_instruction(op, args, nxt)
    if string is not None:
        return string
    if op in ('rep', 'repz', 'repe') and args == 'ret':
        op, args = 'ret', ''  # AMD branch-predictor idiom: REP is ignored on RET
    if op.startswith('prefetch') or op in ('lfence', 'sfence', 'mfence'):
        # Hints and fences: guest instructions are serialised on one logical CPU.
        fence = '' if op.startswith('prefetch') else '__atomic_thread_fence(__ATOMIC_SEQ_CST); '
        return fence + end, [nxt]
    if op == 'ret':
        adjustment = int(args, 0) if args else 0
        if not 0 <= adjustment <= 65535:
            raise Unsupported('ret stack adjustment')
        return f'c->flags = f; rt_return(c); c->esp += {adjustment}u; return;', []
    if op == 'call':
        target = Operand(args)
        if target.imm == nxt:
            return f'PUSH(c, {after}); {end}', [nxt]
        if target.imm is not None:
            callees.add(target.imm)
        elif target.width != 32:
            raise Unsupported('call operand width')
        if target.imm is not None:
            # Appel direct vers une fonction traduite (link_direct remplace
            # par rt_call une cible qui ne l'est pas, par ex. un import).
            call = f'rt_call_fn(c, fn_{target.imm:x}, {code_address(target.imm, rebase)}, {after});'
        else:
            call = f'a = {target.read()}; c->flags = f; rt_call_any(c, a, {after});'
        if target.imm is not None:
            call = 'c->flags = f; ' + call
        return f'{call} if (c->eip != {after}) goto resume; f = c->flags; {end}', [nxt]
    if op == 'jmp' or (op.startswith('j') and op[1:] in CC):
        target = Operand(args)
        if target.imm is None:
            if op != 'jmp' or target.width != 32:
                raise Unsupported('indirect control flow')
            if cases:
                # Statically recovered jump table: the guest still computes the
                # target; known values become local gotos, anything else goes
                # through the checked dispatcher exactly as before.
                local = sorted(t for t in set(cases) if t not in entries)
                callees.update(t for t in cases if t in entries)
                labels = ' '.join(f'case {t:#x}u: goto L{t:x};' for t in local)
                selector = f'(uint32_t)(a - {CODE_RELOCATED})' if rebase else 'a'
                return (f'a = {target.read()}; rt_poll(); switch ({selector}) {{ {labels} '
                        'default: c->flags = f; rt_jump_any(c, a); return; }'), local
            return f'a = {target.read()}; c->flags = f; rt_jump_any(c, a); return;', []
        if target.imm in entries:
            callees.add(target.imm)
            branch = f'c->flags = f; rt_jump_fn(c, fn_{target.imm:x}, {code_address(target.imm, rebase)}); return;'
            successors = []
        else:
            branch = ('rt_poll(); ' if target.imm <= pc else '') + f'goto L{target.imm:x};'
            successors = [target.imm]
        if op == 'jmp':
            return branch, successors
        return f'if (cond(&f, CC_{CC[op[1:]]})) {{ {branch} }} {end}', successors + [nxt]
    floating = float_lift.lower(op, args, nxt, Operand, Unsupported)
    if floating is not None:
        return floating
    operands = [Operand(x.strip()) for x in args.split(',')] if args else []
    if op in ('shl', 'shr', 'sar') and len(operands) == 1:
        operands.append(Operand('1'))
    if op == 'pushfd' and not operands:
        code = 'PUSH(c, get_eflags(c, &f));'
    elif op == 'popfd' and not operands:
        code = 'a = POP(c); set_eflags(c, &f, a);'
    elif op == 'cpuid' and not operands:
        code = 'rt_cpuid(c);'
    elif op == 'rdtsc' and not operands:
        code = '{ uint64_t ticks = rt_rdtsc(); c->eax = (uint32_t)ticks; c->edx = (uint32_t)(ticks >> 32); }'
    elif op == 'cdq' and not operands:
        code = 'c->edx = 0u - (c->eax >> 31);'
    elif op == 'cbw' and not operands:
        code = 'c->eax = (c->eax & 0xffff0000u) | (sx(c->eax & 0xffu, 8) & 0xffffu);'
    elif op == 'cwd' and not operands:
        code = 'c->edx = (c->edx & 0xffff0000u) | ((c->eax & 0x8000u) ? 0xffffu : 0u);'
    elif op == 'sahf' and not operands:
        code = ('a = c->eax >> 8; f.cf = a & 1; f.pf = (a >> 2) & 1 ? 0 : 1; f.af = (a >> 4) & 1; '
                'f.zf = (a >> 6) & 1; f.sf = (a >> 7) & 1;')
    elif op == 'lahf' and not operands:
        code = 'c->eax = (c->eax & 0xffff00ffu) | ((get_eflags(c, &f) & 0xd5u) | 2u) << 8;'
    elif op in ('bt', 'bts', 'btr', 'btc') and len(operands) == 2 and operands[0].width in (16, 32):
        base, index = operands
        n = base.width
        update = {'bt': None, 'bts': 'a | mask', 'btr': 'a & ~mask', 'btc': 'a ^ mask'}[op]
        if base.addr is not None and index.reg:
            # Bit-string form: a register offset may address any byte (signed).
            offset = f'(uint32_t)((int32_t)sx({index.read()}, {index.width}) >> {5 if n == 32 else 4}) * {n // 8}u'
            location = f'(uint32_t)({base.addr} + {offset})'
            store = f'W{n}(address, {update});' if update else ''
            code = (f'{{ uint32_t address = {location}; uint32_t mask = 1u << ({index.read()} & {n - 1}u); '
                    f'a = R{n}(address); f.cf = (a & mask) != 0; {store} }}')
        else:
            code = (f'{{ uint32_t mask = 1u << ({index.read()} & {n - 1}u); a = {base.read()}; '
                    f'f.cf = (a & mask) != 0; {base.write(update) if update else ""} }}')
    elif op in ('mul', 'imul') and len(operands) == 1 and operands[0].width in (8, 16):
        n = operands[0].width
        src = operands[0].read()
        if n == 8:
            value = (f'(uint32_t)((c->eax & 0xffu) * {src})' if op == 'mul'
                     else f'(uint32_t)((int32_t)(int8_t)(c->eax & 0xffu) * (int8_t){src})')
            code = (f'r = {value} & 0xffffu; c->eax = (c->eax & 0xffff0000u) | r; '
                    + ('f.cf = f.of = (r >> 8) != 0;' if op == 'mul' else 'f.cf = f.of = r != (sx(r & 0xffu, 8) & 0xffffu);'))
        else:
            value = (f'(uint32_t)((c->eax & 0xffffu) * {src})' if op == 'mul'
                     else f'(uint32_t)((int32_t)(int16_t)(c->eax & 0xffffu) * (int16_t){src})')
            code = (f'r = {value}; c->eax = (c->eax & 0xffff0000u) | (r & 0xffffu); '
                    'c->edx = (c->edx & 0xffff0000u) | (r >> 16); '
                    + ('f.cf = f.of = (r >> 16) != 0;' if op == 'mul' else 'f.cf = f.of = r != sx(r & 0xffffu, 16);'))
    elif op in ('div', 'idiv') and len(operands) == 1 and operands[0].width in (8, 16):
        code = f'rt_div_narrow(c, {operands[0].read()}, {operands[0].width}, {int(op == "idiv")}, {here});'
    elif op == 'cwde' and not operands:
        code = 'c->eax = sx(c->eax & 0xffffu, 16);'
    elif op == 'fxsave' and len(operands) == 1 and operands[0].addr:
        code = f'rt_fxsave(c, {operands[0].addr});'
    elif op == 'ldmxcsr' and len(operands) == 1:
        code = f'rt_ldmxcsr(c, {operands[0].read()});'
    elif op == 'stmxcsr' and len(operands) == 1:
        code = operands[0].write('c->mxcsr')
    elif op == 'pause' and not operands:
        code = 'rt_poll(); __atomic_signal_fence(__ATOMIC_SEQ_CST);'
    elif op in ('cld', 'std') and not operands:
        code = f'c->df = {int(op == "std")}u;'
    elif op == 'leave' and not operands:
        code = 'c->esp = c->ebp; c->ebp = POP(c);'
    elif op == 'nop' or (op == 'lea' and operands[0].reg and operands[1].addr):
        code = '' if op == 'nop' else operands[0].write(operands[1].effective_addr)
    elif op in ('mov', 'movzx', 'movsx') and len(operands) == 2:
        dst, src = operands
        value = src.read()
        if op == 'movsx':
            if not src.width:
                raise Unsupported('movsx width')
            value = f'sx({value}, {src.width})'
        code = dst.write(value)
    elif op == 'push' and len(operands) == 1:
        src = operands[0]
        if src.width == 16:
            code = f'a = {src.read()}; c->esp -= 2; W16(c->esp, a);'
        else:
            code = f'a = {src.read()}; PUSH(c, a);'
    elif op == 'pop' and len(operands) == 1 and operands[0].reg:
        dst = operands[0]
        code = ('a = R16(c->esp); c->esp += 2; ' if dst.width == 16 else 'a = POP(c); ') + dst.write('a')
    elif op in ('add', 'adc', 'sub', 'sbb', 'cmp', 'and', 'or', 'xor', 'test', 'shl', 'shr', 'sar') and len(operands) == 2:
        dst, src = operands
        n = dst.width
        if n is None:
            raise Unsupported('missing operand width')
        code = f'a = {dst.read()}; b = {src.read()}; '
        if op in ('and', 'or', 'xor', 'test'):
            operator = {'and': '&', 'test': '&', 'or': '|', 'xor': '^'}[op]
            value = f'f_logic(&f, a {operator} b, {n})'
        else:
            value = f'f_{"sub" if op == "cmp" else op}(&f, a, b, {n})'
        code += f'r = {value}; '
        if op not in ('cmp', 'test'):
            code += dst.write('r')
    elif op in ('rol', 'ror', 'rcl', 'rcr') and len(operands) in (1, 2):
        dst = operands[0]; count = operands[1].read() if len(operands) == 2 else '1u'
        kind = {'rol': 0, 'ror': 1, 'rcl': 2, 'rcr': 3}[op]
        code = f'a = {dst.read()}; b = {count}; r = f_rotate(&f, a, b, {dst.width}, {kind}); ' + dst.write('r')
    elif op == 'bswap' and len(operands) == 1 and operands[0].width == 32:
        code = operands[0].write(f'__builtin_bswap32({operands[0].read()})')
    elif op in ('not', 'neg', 'inc', 'dec') and len(operands) == 1:
        dst = operands[0]
        value = '~a' if op == 'not' else f'f_{op}(&f, a, {dst.width})'
        code = f'a = {dst.read()}; r = {value}; ' + dst.write('r')
    elif op in ('lock_add', 'lock_sub', 'lock_and', 'lock_or', 'lock_xor', 'lock_adc', 'lock_sbb', 'lock_inc', 'lock_dec'):
        operation = op[5:]
        dst = operands[0]
        if dst.addr is None or dst.width != 32 or len(operands) != (1 if operation in ('inc','dec') else 2):
            raise Unsupported('atomic arithmetic operand form')
        value = '1u' if operation in ('inc', 'dec') else operands[1].read()
        kind = {'add': 0, 'adc': 0, 'inc': 0, 'sub': 1, 'sbb': 1, 'dec': 1, 'and': 2, 'or': 3, 'xor': 4}[operation]
        increment = 'b + f.cf' if operation in ('adc', 'sbb') else 'b'
        code = f'b = {value}; a = rt_atomic32(c, {dst.addr}, {increment}, {kind}); '
        if operation in ('and', 'or', 'xor'):
            operator = {'and': '&', 'or': '|', 'xor': '^'}[operation]
            code += f'f_logic(&f, a {operator} b, 32);'
        elif operation in ('inc', 'dec'):
            code += f'f_{operation}(&f, a, 32);'
        else:
            code += f'f_{operation}(&f, a, b, 32);'
    elif op in ('xadd', 'lock_xadd') and len(operands) == 2:
        dst, src = operands
        if dst.addr is None or dst.width != 32 or not src.reg or src.width != 32:
            raise Unsupported('xadd operand form')
        code = f'b = {src.read()}; a = rt_atomic32(c, {dst.addr}, b, 0); f_add(&f, a, b, 32); ' + src.write('a')
    elif op == 'xchg' and len(operands) == 2:
        dst, src = operands
        if dst.addr is not None and dst.width == 32 and src.reg and src.width == 32:
            code = f'a = {src.read()}; r = rt_exchange32(c, {dst.addr}, a); ' + src.write('r')
        elif dst.reg and src.reg and dst.width == src.width:
            code = f'a = {dst.read()}; b = {src.read()}; ' + dst.write('b') + ' ' + src.write('a')
        else:
            raise Unsupported('xchg operand form')
    elif op in ('cmpxchg8b', 'lock_cmpxchg8b') and len(operands) == 1:
        dst = operands[0]
        if dst.addr is None or dst.width != 64:
            raise Unsupported('cmpxchg8b operand form')
        code = (f'{{ uint64_t expected = ((uint64_t)c->edx << 32) | c->eax; '
                f'uint64_t observed = rt_compare_exchange64(c, {dst.addr}, expected, ((uint64_t)c->ecx << 32) | c->ebx); '
                'f.zf = observed == expected; if (!f.zf) { c->eax = (uint32_t)observed; c->edx = (uint32_t)(observed >> 32); } }')
    elif op in ('cmpxchg', 'lock_cmpxchg') and len(operands) == 2:
        dst, src = operands
        if dst.addr is None or dst.width != 32 or not src.reg or src.width != 32:
            raise Unsupported('cmpxchg operand form')
        code = (f'a = c->eax; b = {src.read()}; r = rt_compare_exchange32(c, {dst.addr}, a, b); '
                'f_sub(&f, a, r, 32); if (a != r) c->eax = r;')
    elif op.startswith('cmov') and op[4:] in CC and len(operands) == 2:
        dst, src = operands
        code = f'a = {src.read()}; if (cond(&f, CC_{CC[op[4:]]})) {{ {dst.write("a")} }}'
    elif op.startswith('set') and op[3:] in CC and len(operands) == 1:
        code = operands[0].write(f'cond(&f, CC_{CC[op[3:]]})')
    elif op in ('div', 'idiv') and len(operands) == 1 and operands[0].width == 32:
        code = f'rt_div32(c, {operands[0].read()}, {int(op == "idiv")}, {here});'
    elif op == 'mul' and len(operands) == 1 and operands[0].width == 32:
        code = (f'{{ uint64_t value = (uint64_t)c->eax * {operands[0].read()}; '
                'c->eax = (uint32_t)value; c->edx = (uint32_t)(value >> 32); f.cf = f.of = c->edx != 0; }')
    elif op in ('bsr', 'bsf') and len(operands) == 2 and operands[0].width == 32:
        dst, src = operands
        value = '31u - (uint32_t)__builtin_clz(a)' if op == 'bsr' else '(uint32_t)__builtin_ctz(a)'
        code = f'a = {src.read()}; f.zf = a == 0; if (a) {{ {dst.write(value)} }}'
    elif op in ('shld', 'shrd') and len(operands) == 3 and operands[0].width == 32:
        dst, src, count = operands
        value = '(a << r) | (b >> (32 - r))' if op == 'shld' else '(a >> r) | (b << (32 - r))'
        carry = '(a >> (32 - r)) & 1' if op == 'shld' else '(a >> (r - 1)) & 1'
        code = (f'a = {dst.read()}; b = {src.read()}; r = ({count.read()}) & 31; '
                f'if (r) {{ f.cf = {carry}; uint32_t result = {value}; '
                f'f.of = ((a ^ result) >> 31) & 1; szp(&f, result, 32); {dst.write("result")} }}')
    elif op == 'imul' and len(operands) in (2, 3):
        dst = operands[0]
        lhs, rhs = (dst, operands[1]) if len(operands) == 2 else operands[1:]
        code = f'a = {lhs.read()}; b = {rhs.read()}; r = f_imul(&f, a, b, {dst.width}); ' + dst.write('r')
    elif op == 'imul' and len(operands) == 1 and operands[0].width == 32:
        code = (f'product = (int64_t)(int32_t)c->eax * (int64_t)(int32_t){operands[0].read()}; '
                'c->eax = (uint32_t)product; c->edx = (uint32_t)((uint64_t)product >> 32); '
                'f.cf = f.of = product != (int64_t)(int32_t)c->eax;')
    else:
        raise Unsupported(f'{op} {args}')
    return code + ' ' + end, [nxt]


class _Without:
    """`entries - {own}` for membership tests, without copying a large set."""
    __slots__ = ('entries', 'own')

    def __init__(self, entries, own):
        self.entries, self.own = entries, own

    def __contains__(self, address):
        return address != self.own and address in self.entries


def lift(entry, insns, entries=frozenset(), callees=None, tables=None, setjmp=None, delta=None):
    """Translate one function. `setjmp(target)` tells whether a direct call
    target is setjmp; its return sites become resume points for longjmp.
    `delta` (expression C) : code relogeable d'un greffon PE, chaque adresse
    de code et chaque valeur relocalisée vaut alors adresse + delta."""
    pending = [entry]
    translated = {}
    resume = set()
    others = _Without(entries, entry)
    while pending:
        pc = pending.pop()
        if pc in translated:
            continue
        if pc not in insns:
            raise Unsupported(f'no instruction at {pc:#x}')
        try:
            code, successors = translate(pc, insns[pc], others, callees,
                                         tables(pc) if tables else None, delta is not None)
        except (Unsupported, IndexError) as exc:
            raise Unsupported(f'{pc:#x}: {exc}') from exc
        translated[pc] = code
        pending.extend(successors)
        nxt, op, args = insns[pc]
        if op == 'call' and setjmp is not None:
            try:
                target = int(args, 0)
            except ValueError:
                target = None
            if target is not None and setjmp(target):
                resume.add(nxt)
    # `restrict` : le CPU vit hors de la mémoire invitée, et `mem_` est une
    # copie locale de M ; clang garde ainsi les registres invités dans des
    # registres hôtes entre les accès mémoire (voir RT_MEM dans rt.h).
    lines = ['#undef RT_MEM', '#define RT_MEM mem_', f'static void fn_{entry:x}(CPU *restrict c) {{',
             '    uint8_t *const mem_ = M; FL f = c->flags; uint32_t a, b, r; int64_t product;',
             '    (void)mem_; (void)f; (void)a; (void)b; (void)r; (void)product;']
    if delta is not None:
        lines.append(f'    const uint32_t {RELOCATED} = {delta}[0], {CODE_RELOCATED} = {delta}[1]; '
                     f'(void){RELOCATED}; (void){CODE_RELOCATED};')
    lines.append(f'    goto L{entry:x};')
    lines.extend(f'L{pc:x}: {code}' for pc, code in sorted(translated.items()))
    if any('goto resume;' in code for code in translated.values()):
        # A call returned elsewhere (non-local transfer): resume here when the
        # target belongs to this function, otherwise drop this host frame.
        if resume and delta is not None:
            raise Unsupported('setjmp resume points in relocatable code')
        cases = ' '.join(f'case {pc:#x}u: goto L{pc:x};' for pc in sorted(resume))
        lines.append(f'resume: f = c->flags; switch (c->eip) {{ {cases} default: return; }}' if resume
                     else 'resume: return;')
    lines += ['}', '#undef RT_MEM', '#define RT_MEM M']
    return '\n'.join(lines), len(translated)


_DIRECT = re.compile(r'rt_(call|jump)_fn\(c, fn_([0-9a-f]+), ')


def link_direct(body, compiled, imports=frozenset()):
    """Résout les appels et sauts directs d'un corps généré : une cible
    traduite garde l'appel C direct, toute autre (import, fonction non
    traduite) repasse par rt_call/rt_dispatch, qui piège si elle est inconnue.
    Renvoie (corps, adresses traduites référencées)."""
    used = set()

    def resolve(m):
        target = int(m[2], 16)
        if target in compiled:
            used.add(target)
            return m[0]
        if m[1] == 'call':
            return 'rt_call_import(c, ' if target in imports else 'rt_call(c, '
        return 'rt_dispatch(c, '
    return _DIRECT.sub(resolve, body), used


def prototypes(targets, static=False):
    return [f'{"static " if static else ""}void fn_{t:x}(CPU *);' for t in sorted(targets)]


class JumpTables:
    """Recover candidate targets of switch jumps from the prepared images.

    Recognised forms (GCC, i386):
      jmp dword ptr [4*I + T]                          absolute table (relocated)
      mov R, dword ptr [B + 4*I + D]; add R, B; jmp R  PIC table of GOT offsets, B = GOT
    The bound comes from a preceding `cmp I, N; ja/jae`; otherwise entries are
    read while they remain plausible code addresses. Candidates are only an
    optimisation: the generated code compares the real computed target with
    them and falls back to the checked dispatcher, so an imprecise table can
    add dead labels but never change control flow.
    """
    MAX_ENTRIES = 1024
    MAX_DISTANCE = 0x100000

    def __init__(self, layout, insns):
        self.layout, self.insns = layout, insns
        self.images, self.previous, self.cache = {}, None, {}

    def module(self, address):
        for mod in self.layout['modules']:
            if any(s['vaddr'] <= address < s['vaddr'] + s['filesz'] for s in mod['loads']):
                return mod
        return None

    def read32(self, address):
        mod = self.module(address)
        if mod is None or self.module(address + 3) is not mod:
            return None
        if mod['name'] not in self.images:
            self.images[mod['name']] = Path(mod['image']).read_bytes()
        data = self.images[mod['name']]
        for s in mod['loads']:
            if s['vaddr'] <= address and address + 4 <= s['vaddr'] + s['filesz']:
                offset = s['off'] + address - s['vaddr']
                return int.from_bytes(data[offset:offset + 4], 'little')
        return None

    def before(self, pc, count):
        shared = getattr(self.insns, 'previous', None)   # LazyInsns : tenue à jour au chargement
        if shared is not None:
            self.previous = shared
        elif self.previous is None or len(self.previous) < len(self.insns):
            self.previous = {nxt: start for start, (nxt, _, _) in self.insns.items()}
        out = []
        while len(out) < count and pc in self.previous:
            pc = self.previous[pc]
            out.append((pc,) + tuple(self.insns[pc]))
        return out

    def bound(self, history, index):
        """Number of table entries implied by `cmp index, N; ja` (None if unknown)."""
        aliases = {index}
        for i, (_, _, op, args) in enumerate(history):
            parts = [x.strip() for x in args.split(',')]
            if op == 'movzx' and len(parts) == 2 and REGS.get(parts[0], ('',))[0] in aliases and parts[1] in REGS:
                aliases.add(REGS[parts[1]][0])
            if op in ('ja', 'jae', 'jnbe', 'jnb') and i + 1 < len(history):
                _, _, cop, cargs = history[i + 1]
                cparts = [x.strip() for x in cargs.split(',')]
                if cop == 'cmp' and len(cparts) == 2 and cparts[0] in REGS and REGS[cparts[0]][0] in aliases:
                    try:
                        limit = int(cparts[1], 0) & MSK_BITS[REGS[cparts[0]][1]]
                    except ValueError:
                        return None
                    return limit + 1 if op in ('ja', 'jnbe') else limit
                return None
            if op in ('call', 'ret', 'jmp') or op.startswith('j'):
                return None
        return None

    def __call__(self, pc):
        if pc not in self.cache:
            self.cache[pc] = self.resolve(pc)
        return self.cache[pc]

    def resolve(self, pc):
        nxt, op, args = self.insns[pc]
        if op != 'jmp':
            return None
        history = self.before(pc, 8)
        pattern = r'(?:dword ptr )?\[(?:4\*(\w+) \+ (0x[0-9a-f]+)|(0x[0-9a-f]+) \+ 4\*(\w+))\]'
        m = re.fullmatch(pattern, args)
        if m:
            index = m[1] or m[4]; table = int(m[2] or m[3], 16); base = 0; rest = history
        elif args in REGS and REGS[args][1] == 32 and len(history) >= 2:
            (_, _, add_op, add_args), (_, _, mov_op, mov_args) = history[0], history[1]
            add = [x.strip() for x in add_args.split(',')]
            m = re.fullmatch(r'dword ptr \[(\w+) \+ 4\*(\w+) ([+-]) (0x[0-9a-f]+)\]',
                             mov_args.split(',', 1)[-1].strip())
            if not (add_op == 'add' and add[0] == args and mov_op == 'mov' and m
                    and mov_args.split(',')[0].strip() == args and add[1] == m[1]):
                return None
            mod = self.module(pc)
            base = mod['gotplt'] if mod else 0
            displacement = int(m[4], 16) * (-1 if m[3] == '-' else 1)
            table, index, rest = (base + displacement) & 0xffffffff, m[2], history[2:]
        else:
            return None
        if index not in REGS:
            return None
        count = self.bound(rest, REGS[index][0])
        exact = count is not None
        count = min(count if exact else self.MAX_ENTRIES, self.MAX_ENTRIES)
        targets = []
        for i in range(count):
            value = self.read32(table + 4 * i)
            if value is None:
                break
            target = (value + base) & 0xffffffff
            far = not exact and abs(target - pc) > self.MAX_DISTANCE   # cold parts may be far away
            if target not in self.insns or far or self.module(target) is not self.module(pc):
                if exact:
                    return None   # a declared case does not land on code: do not guess
                break
            targets.append(target)
        return targets or None


MSK_BITS = {8: 0xff, 16: 0xffff, 32: 0xffffffff}
SETJMP_IMPORTS = frozenset({'_setjmp', 'setjmp', '__sigsetjmp', 'sigsetjmp'})


def import_called(target, insns, imports, tables):
    """Import reached by a direct call: its thunk, or a PLT stub jumping through
    a GOT slot (`jmp [ebx + X]` with EBX = GOT, or `jmp [X]`) holding the thunk."""
    if target in imports:
        return imports[target]
    insn = insns.get(target)
    if not insn or insn[1] != 'jmp':
        return None
    m = re.fullmatch(r'dword ptr \[(?:(ebx) \+ )?(0x[0-9a-f]+)\]', insn[2])
    if not m:
        return None
    mod = tables.module(target)
    if m[1] and not mod:
        return None
    value = tables.read32(((mod['gotplt'] if m[1] else 0) + int(m[2], 16)) & 0xffffffff)
    return imports.get(value)


def _guessed_successors(pc, insn):
    """Control-flow successors of an instruction the lifter rejected (audit only)."""
    nxt, op, args = insn
    if op in ('ret', 'jmp', 'int3', 'ud2', 'hlt', 'retf', 'iret', 'iretd') or op.startswith('ljmp'):
        targets = []
    else:
        targets = [nxt]
    if op.startswith('j') or op.startswith('loop'):
        try:
            targets.append(int(args, 0))
        except ValueError:
            pass
    return targets


def scan(entry, insns, entries=frozenset(), callees=None, visited=None, tables=None):
    """Collect-mode audit of one function: never emits code, never stops early.

    Returns (instruction count, [(pc, op, args, reason)] for every rejected
    instruction, [pc of every indirect jmp]). `lift` itself stays fail-closed.
    """
    if callees is None:
        callees = set()
    pending, seen, failures, indirect = [entry], set(), [], []
    others = entries - {entry}
    while pending:
        pc = pending.pop()
        if pc in seen:
            continue
        seen.add(pc)
        if pc not in insns:
            failures.append((pc, '<none>', '', 'no instruction boundary'))
            continue
        insn = insns[pc]
        try:
            code, successors = translate(pc, insn, others, callees, tables(pc) if tables else None)
            if 'rt_jump_any(c, a)' in code and insn[1] == 'jmp':
                indirect.append((pc, 'switch (a)' in code))
        except Exception as exc:  # noqa: BLE001 - every rejection is data here
            failures.append((pc, insn[1], insn[2], f'{type(exc).__name__}: {exc}'))
            successors = _guessed_successors(pc, insn)
            if insn[1] == 'call':
                try:
                    callees.add(int(insn[2], 0))
                except ValueError:
                    pass
        pending.extend(successors)
    if visited is not None:
        visited |= seen
    return len(seen), failures, indirect


# Keep this list synchronized with runtime/shims.c. Unknown imports fail closed.
# New runtime files declare their names on `// SHIMS: a b c` lines instead (see declared_shims).
_CORE_SHIMS = frozenset('pthread_create pthread_join pthread_attr_init pthread_attr_setstacksize pthread_attr_destroy pthread_getschedparam pthread_setschedparam pthread_exit pthread_equal pthread_cond_wait pthread_cond_timedwait '
                  'feof ferror clearerr fileno fileno_unlocked ftell ftello ftello64 fseek fseeko fseeko64 rewind strrchr '
                  'memset memcpy memmove memcmp strlen strcpy stpcpy strncpy strcmp '
                  'malloc calloc realloc free malloc_usable_size __cxa_atexit abort puts snprintf vsnprintf pthread_self '
                  'pthread_mutex_init pthread_mutex_lock pthread_mutex_unlock '
                  'pthread_mutex_trylock pthread_mutex_destroy pthread_mutexattr_init '
                  'pthread_mutexattr_settype pthread_mutexattr_destroy __errno_location getenv '
                  '___tls_get_addr __tls_get_addr gettimeofday usleep fopen fopen64 fclose fgets fread fwrite access opendir closedir '
                  'readdir strpbrk strstr strcasestr strchr strcasecmp strncasecmp strdup tolower toupper getppid strtol strtoul __isoc99_sscanf sscanf strcat strncat strtod strerror pthread_key_create __pthread_key_create pthread_key_delete pthread_setspecific pthread_getspecific pthread_once pthread_cond_init pthread_cond_destroy pthread_cond_signal pthread_cond_broadcast dlopen dlsym dlerror clock_gettime sysconf mmap mmap64 munmap mprotect madvise syscall fputs fputc putc fgetc getc fflush printf fprintf vprintf vfprintf open open64 __open_2 __open64_2 close read write lseek lseek64 chdir getcwd readlink stat stat64 lstat lstat64 fstat fstat64 __xstat __xstat64 __lxstat __lxstat64 __fxstat __fxstat64'.split())


def declared_shims(runtime_dir=Path(__file__).resolve().parent.parent / 'runtime'):
    """Names declared by runtime/*.c on lines starting with `// SHIMS:`."""
    names = set()
    for path in sorted(runtime_dir.glob('*.c')):
        for line in path.read_text().splitlines():
            if line.startswith('// SHIMS:'):
                names.update(line[len('// SHIMS:'):].split())
    return names


SHIMS = _CORE_SHIMS | frozenset(declared_shims())

def resolve_export(layout, spec):
    if ':symbol:' in spec:
        name, symbol = spec.split(':symbol:')
        mod = next((m for m in layout['modules'] if m['name'] == name), None)
        matches = [int(address, 16) for address, value in mod['names'].items() if value == symbol] if mod else []
        if len(matches) != 1:
            raise Unsupported(f'expected one function symbol for {spec!r}, found {len(matches)}')
        return matches[0]
    if ':code:' in spec:
        name, offset = spec.split(':code:')
        mod = next((m for m in layout['modules'] if m['name'] == name), None)
        try:
            address = mod['base'] + int(offset, 0) if mod else -1
        except ValueError:
            raise Unsupported(f'invalid code offset {spec!r}') from None
        if not mod or not any(s['flags'] & 1 and s['vaddr'] <= address < s['vaddr'] + s['filesz']
                              for s in mod['loads']):
            raise Unsupported(f'code root outside executable segments: {spec!r}')
        return address
    if ':init:' in spec:
        name, index = spec.split(':init:')
        mod = next((m for m in layout['modules'] if m['name'] == name), None)
        if mod is None:
            raise Unsupported(f'unknown module {name!r}')
        if index == 'dt':
            return mod['init']
        try:
            return mod['init_array'][int(index)]
        except (ValueError, IndexError):
            raise Unsupported(f'unknown constructor {spec!r}') from None
    matches = [m for m in layout['modules'] if spec in m['exports']]
    if len(matches) != 1:
        raise Unsupported(f'export {spec!r}: expected one definition, found {len(matches)}')
    return matches[0]['exports'][spec]


def module_instructions(mod, objdump, disasm_cache=None):
    """Disassemble one prepared module, through the optional content-addressed cache."""
    cache_path = None
    if disasm_cache is not None:
        tool = Path(shutil.which(objdump) or objdump).resolve()
        identity = (f'parser-v2:{mod["base"]}:{tool}:'
                    f'{tool.stat().st_mtime_ns}:{tool.stat().st_size}').encode()
        digest = hashlib.sha256(identity + Path(mod['image']).read_bytes()).hexdigest()
        cache_path = Path(disasm_cache) / (digest + '.json')
        fast_path = cache_path.with_suffix('.marshal')   # même contenu, chargé ~5x plus vite
        if fast_path.exists():
            return marshal.loads(fast_path.read_bytes())
        if cache_path.exists():
            decoded = {int(pc): tuple(value) for pc, value in json.loads(cache_path.read_text()).items()}
            _write_atomic(fast_path, marshal.dumps(decoded))
            return decoded
    decoded = disassemble(mod['image'], mod['base'], objdump)
    if cache_path is not None:
        cache_path.parent.mkdir(parents=True, exist_ok=True)
        _write_atomic(fast_path, marshal.dumps(decoded))
    return decoded


def _write_atomic(path, data):
    temporary = path.with_name(f'.{path.name}.{os.getpid()}.tmp')
    temporary.write_bytes(data)
    os.replace(temporary, path)


def generate(layout, exports, objdump, allow_unimplemented=False, disasm_cache=None):
    code = ['/* Generated by tools/lift.py. Do not edit. */', '#include "rt.h"']
    selected = [(name, resolve_export(layout, name)) for name in exports]
    insns = {}
    loaded = set()
    def ensure_module(entry):
        for mod in layout['modules']:
            if any(s['flags'] & 1 and s['vaddr'] <= entry < s['vaddr'] + s['filesz'] for s in mod['loads']):
                if mod['name'] not in loaded:
                    insns.update(module_instructions(mod, objdump, disasm_cache))
                    loaded.add(mod['name'])
                return
        raise Unsupported(f'entry {entry:#x} lies outside executable segments')
    imports = {item['addr']: item['name'] for item in layout['imports']}
    entries = {e for m in layout['modules'] for e in m['entries']}
    entries.update(entry for _, entry in selected)
    entries.update(imports)
    pending = [entry for _, entry in selected]
    tables = JumpTables(layout, insns)
    compiled = set()
    used_imports = set()
    unimplemented = set()
    total = 0
    bodies = []
    while pending:
        entry = pending.pop()
        if entry in compiled or entry in used_imports:
            continue
        if entry in imports:
            name = imports[entry]
            if name not in SHIMS:
                if not allow_unimplemented:
                    raise Unsupported(f'import {name!r} at {entry:#x} has no shim')
                unimplemented.add(name)
            used_imports.add(entry)
            continue
        ensure_module(entry)
        callees = set()
        body, count = lift(entry, insns, entries, callees, tables,
                           lambda t: import_called(t, insns, imports, tables) in SETJMP_IMPORTS)
        bodies.append(body)
        compiled.add(entry)
        total += count
        pending.extend(sorted(callees))
    code += prototypes(compiled, static=True)
    code += [link_direct(body, compiled, imports)[0] for body in bodies]
    print(f'{len(selected)} roots, {len(compiled)} native functions, '
          f'{total} instructions, {len(used_imports)} direct imports')
    if unimplemented:
        print('Runtime traps for unimplemented imports: ' + ', '.join(sorted(unimplemented)))
    code.append('gfn rt_lookup(uint32_t target) { switch (target) {')
    for entry in sorted(compiled):
        code.append(f'case {entry:#x}u: return fn_{entry:x};')
    code.extend(['default: return NULL; } }', 'uint32_t rt_export(const char *name) {'])
    for name, entry in selected:
        code.append(f'if (!strcmp(name, {json.dumps(name)})) return {entry:#x}u;')
    code.extend(['return 0; }', 'const char *rt_import_name(uint32_t target) { switch (target) {'])
    for addr, name in sorted(imports.items()):
        code.append(f'case {addr:#x}u: return {json.dumps(name)};')
    code.extend(['default: return NULL; } }', 'int rt_load_images(const char *directory) {'])
    for mod in layout['modules']:
        for seg in mod['loads']:
            code.append(f'if (rt_load_segment(directory, {json.dumps(mod["name"])}, '
                        f'{seg["off"]}u, {seg["vaddr"]:#x}u, {seg["filesz"]}u, {seg["memsz"]}u)) return -1;')
        tls = mod.get('tls')
        if tls:
            code.append(f'if (rt_register_tls(directory, {json.dumps(mod["name"])}, {mod["modid"]}u, '
                        f'{tls["off"]}u, {tls["filesz"]}u, {tls["memsz"]}u, {tls["align"]}u)) return -1;')
    for item in layout.get('data_imports', []):
        code.append(f'if (rt_init_import_data({json.dumps(item["name"])}, {item["addr"]:#x}u, {item["size"]}u)) return -1;')
    code.extend(['return 0; }', ''])
    code.append(registry.generate(layout))
    return '\n'.join(code)


# ---------------------------------------------------------------------------
# Génération répartie (tools/build_game.py) : tout le jeu en fichiers C stables.

def executable(module, address):
    return any(s['flags'] & 1 and s['vaddr'] <= address < s['vaddr'] + s['filesz'] for s in module['loads'])


def module_boundaries(module):
    """Strong function starts of one module: symbols/entries, exports,
    DT_INIT/FINI and init/fini arrays (not code pointers found in data)."""
    starts = set(module['entries']) | set(module['exports'].values())
    starts |= {a for a in module['init_array'] + module.get('fini_array', []) if a not in (0, 0xffffffff)}
    starts |= {a for a in (module['init'], module['fini']) if a}
    return {a for a in starts if executable(module, a)}


def module_roots(module):
    """Every known function start of one module: symbols/entries, exports,
    DT_INIT/FINI, init/fini arrays and code pointers found in data."""
    roots = set(module['entries']) | set(module.get('data_ptrs', []))
    roots |= set(module['exports'].values())
    roots |= {a for a in module['init_array'] + module.get('fini_array', []) if a not in (0, 0xffffffff)}
    roots |= {a for a in (module['init'], module['fini']) if a}
    return sorted(a for a in roots if executable(module, a))


_GOTOFF = re.compile(r'\[(e[a-z]{2}) ([+-]) (0x[0-9a-f]+)\]$')


def code_pointer_roots(module, insns):
    """Function addresses taken in code rather than stored in data: relocated
    immediates (`push imm`, `mov r/m, imm`) and PIC `lea r, [R + GOTOFF]`
    (R holds the GOT; |offset| >= 0x1000 to avoid stack/field offsets).
    Only instruction boundaries of executable segments of the same module
    qualify. Extra roots only add compiled code; they never change control
    flow (a direct jump to a root goes through the checked dispatcher)."""
    found = set()
    for pc, (_, op, args) in insns.items():
        if not executable(module, pc):
            continue
        candidates = []
        if op == 'lea':
            m = _GOTOFF.search(args)
            if m and m[1] not in ('esp', 'ebp'):
                offset = int(m[3], 16) * (1 if m[2] == '+' else -1)
                if abs(offset) >= 0x1000:
                    candidates.append((module['gotplt'] + offset) & 0xffffffff)
        elif op in ('push', 'mov'):
            candidates += [int(x, 16) for x in (o.strip() for o in args.split(','))
                           if re.fullmatch(r'0x[0-9a-f]+', x)]
        found.update(a for a in candidates if a in insns and executable(module, a))
    return found


def _code_pointer_job(name):
    module = next(m for m in _JOB['layout']['modules'] if m['name'] == name)
    insns = _JOB['insns']
    try:
        insns.load(module, transient=True)
        return name, sorted(code_pointer_roots(module, insns.modules[name]))
    finally:
        insns.drop_transient()


def owner(layout, address):
    return next((m for m in layout['modules'] if executable(m, address)), None)


class LazyInsns(dict):
    """Instruction map that disassembles (through the cache) the module owning
    an address on first use, and keeps the reverse map JumpTables needs."""

    def __init__(self, layout, objdump, disasm_cache):
        super().__init__()
        self.layout, self.objdump, self.disasm_cache = layout, objdump, disasm_cache
        self.loaded, self.transient, self.previous, self.modules = set(), {}, {}, {}

    def load(self, module, transient=False):
        if module['name'] in self.loaded:
            return False
        decoded = module_instructions(module, self.objdump, self.disasm_cache)
        self.update(decoded)
        self.previous.update({nxt: start for start, (nxt, _, _) in decoded.items()})
        self.loaded.add(module['name'])
        self.modules[module['name']] = decoded
        if transient:
            self.transient[module['name']] = decoded
        return True

    def drop_transient(self):
        """Forget modules loaded by one job (bounded memory per worker)."""
        for name, decoded in self.transient.items():
            for pc, (nxt, _, _) in decoded.items():
                dict.pop(self, pc, None)
                self.previous.pop(nxt, None)
            self.loaded.discard(name)
            self.modules.pop(name, None)
        self.transient.clear()

    def _fault(self, pc):
        module = owner(self.layout, pc)
        return module is not None and self.load(module, transient=True)

    def __contains__(self, pc):
        return dict.__contains__(self, pc) or (self._fault(pc) and dict.__contains__(self, pc))

    def __getitem__(self, pc):
        try:
            return dict.__getitem__(self, pc)
        except KeyError:
            if self._fault(pc):
                return dict.__getitem__(self, pc)
            raise

    def get(self, pc, default=None):
        return self[pc] if pc in self else default


_JOB = {}   # état partagé par fork avec les processus de génération


def _claim(address):
    """Atomically reserve one function entry across all worker processes."""
    index = address - _JOB['low']
    if not 0 <= index < _JOB['span']:
        raise Unsupported(f'function {address:#x} outside prepared code')
    bits, lock = _JOB['claims'], _JOB['lock']
    with lock:
        byte = bits[index >> 3]
        if byte & (1 << (index & 7)):
            return False
        bits[index >> 3] = byte | (1 << (index & 7))
        return True


def _generate_job(job):
    """Lift the closure of some roots; entries claimed elsewhere are skipped."""
    number, roots = job
    insns, imports, tables = _JOB['insns'], _JOB['imports'], _JOB['tables']
    setjmp = lambda t: import_called(t, insns, imports, tables) in SETJMP_IMPORTS
    functions, used_imports, pending = {}, set(), list(reversed(roots))
    initial, optional, skipped, failed = set(roots), _JOB['optional'], [], []
    try:
        while pending:
            entry = pending.pop()
            if entry in imports:
                used_imports.add(entry)
                continue
            if entry in initial and entry in optional and entry not in insns:
                # Root from symbols/data pointers that is not code (an object
                # in .rodata of an executable segment): nothing to compile; a
                # guest transfer there would still trap in rt_dispatch.
                skipped.append(entry)
                continue
            if entry in functions:
                continue
            callees = set()
            try:
                if not _claim(entry):
                    continue
                functions[entry] = lift(entry, insns, _JOB['entries'], callees, tables, setjmp)
            except Unsupported as exc:
                if entry in _JOB['named']:
                    raise Unsupported(f'{exc} (function {entry:#x})') from None
                # Not compiled: rt_dispatch traps ("target not recompiled") if
                # the guest ever transfers there. Reported, never skipped silently.
                failed.append((entry, str(exc)))
                continue
            pending.extend(sorted(callees, reverse=True))
    finally:
        insns.drop_transient()
        tables.cache.clear()
    part = Path(_JOB['parts']) / f'{number:05d}.part'
    _write_atomic(part, marshal.dumps(functions))
    return number, {entry: count for entry, (_, count) in functions.items()}, sorted(used_imports), skipped, failed


SHARD_BITS = 17   # 128 Kio de code invité par fichier : une nouvelle racine ne touche qu'un fichier


def shard_name(module, entry):
    return f'{module["name"].replace(".", "_")}.{(entry - module["base"]) >> SHARD_BITS:03x}.c'


def generate_sharded(layout, specs, whole_game, objdump, disasm_cache, out_dir, jobs, log=print):
    """Lift every function reachable from `specs` (and, with whole_game, from
    every known root of every module) into C files grouped by guest address.

    Returns a manifest: shard name -> sorted function entries, plus counts.
    Generated code is identical to `generate` for the same entry set; only
    the file layout differs (functions are extern, looked up by dispatch.c).
    """
    import gc
    import multiprocessing
    import time
    started = time.time()
    selected = [(name, resolve_export(layout, name)) for name in specs]
    imports = {item['addr']: item['name'] for item in layout['imports']}
    roots = {entry for _, entry in selected}
    named = set(roots)
    if whole_game:
        for module in layout['modules']:
            roots.update(module_roots(module))
    context = multiprocessing.get_context('fork')
    insns = LazyInsns(layout, objdump, disasm_cache)
    for module in layout['modules']:
        # Gros modules : désassemblés une fois dans le parent, partagés par fork
        # (gc.freeze : le ramasse-miettes ne recopie pas leurs pages).
        if sum(s['filesz'] for s in module['loads'] if s['flags'] & 1) > 0x100000:
            insns.load(module)
    gc.collect()
    gc.freeze()
    taken = 0
    if whole_game:
        _JOB.update(layout=layout, insns=insns)
        try:
            with context.Pool(jobs) as pool:
                for name, found in pool.imap_unordered(_code_pointer_job, [m['name'] for m in layout['modules']]):
                    taken += len(set(found) - roots)
                    roots.update(found)
        finally:
            _JOB.clear()
    # Frontières de fonction : seulement les vrais débuts (symboles, exports,
    # constructeurs, racines nommées). Les adresses de code trouvées dans les
    # données ou le code (tables de switch, pointeurs) sont compilées comme
    # points d'entrée mais ne coupent pas une fonction : un saut interne vers
    # elles reste un goto, au lieu d'un rt_dispatch imbriqué à chaque tour de
    # boucle (pile hôte qui croît, cas du traducteur de shaders de togl).
    entries = {e for m in layout['modules'] for e in module_boundaries(m)} | named | set(imports)
    by_module = {}
    for entry in sorted(roots):
        if entry in imports:
            continue
        module = owner(layout, entry)
        if module is None:
            raise Unsupported(f'root {entry:#x} lies outside executable segments')
        by_module.setdefault(module['name'], []).append(entry)
    big = insns.loaded - set(insns.transient)
    load_time = time.time() - started
    work, number = [], 0
    target = max(1, len(roots) // max(1, 6 * jobs))
    for name, module_roots_list in sorted(by_module.items(), key=lambda kv: -len(kv[1])):
        step = target if name in big else len(module_roots_list)
        for i in range(0, len(module_roots_list), step):
            work.append((number, module_roots_list[i:i + step]))
            number += 1
    executable_ranges = [(s['vaddr'], s['vaddr'] + s['filesz']) for m in layout['modules']
                         for s in m['loads'] if s['flags'] & 1]
    low = min(a for a, _ in executable_ranges)
    span = max(b for _, b in executable_ranges) - low
    parts = Path(out_dir) / 'parts'
    shutil.rmtree(parts, ignore_errors=True)
    parts.mkdir(parents=True)
    _JOB.update(insns=insns, imports=imports, entries=frozenset(entries), low=low, span=span,
                optional=frozenset(roots - named), named=frozenset(named),
                claims=context.RawArray('B', (span + 7) // 8), lock=context.Lock(), parts=str(parts),
                tables=JumpTables(layout, insns))
    counts, used_imports, not_code, failures = {}, set(), set(), {}
    try:
        with context.Pool(jobs) as pool:
            for done, (_, functions, used, skipped, failed) in enumerate(pool.imap_unordered(_generate_job, work), 1):
                counts.update(functions)
                failures.update(failed)
                used_imports.update(used)
                not_code.update(skipped)
                if done % 10 == 0 or done == len(work):
                    log(f'  lifted {done}/{len(work)} jobs: {len(counts)} functions, '
                        f'{sum(counts.values())} instructions, {time.time() - started:.0f} s')
    finally:
        gc.unfreeze()
        _JOB.clear()
    del insns
    gc.collect()
    lift_time = time.time() - started - load_time
    # Regroupement stable par adresse : le contenu d'un fichier ne dépend que
    # des fonctions de sa plage.
    shards = {}
    for part in sorted(parts.glob('*.part')):
        for entry, (body, _) in marshal.loads(part.read_bytes()).items():
            module = owner(layout, entry)
            shards.setdefault(shard_name(module, entry), {})[entry] = body
    out_dir = Path(out_dir)
    sources = out_dir / 'src'
    sources.mkdir(parents=True, exist_ok=True)
    manifest = dict(shards={}, functions=len(counts), instructions=sum(counts.values()),
                    used_imports=sorted(used_imports), roots=len(roots), code_pointer_roots=taken, c_bytes=0,
                    non_code_roots=[f'{a:#x}' for a in sorted(not_code)],
                    not_compiled={f'{a:#x}': why for a, why in sorted(failures.items()) if a not in counts})
    compiled = frozenset(counts)
    for name, bodies in sorted(shards.items()):
        linked, referenced = [], set()
        for entry in sorted(bodies):
            body, used = link_direct(bodies[entry].replace('static void fn_', 'void fn_', 1), compiled, imports)
            linked.append(body)
            referenced |= used
        text = '/* Generated by tools/lift.py. Do not edit. */\n#include "rt.h"\n' + '\n'.join(
            prototypes(referenced) + linked) + '\n'
        data = text.encode()
        path = sources / name
        if not path.exists() or path.stat().st_size != len(data) or path.read_bytes() != data:
            _write_atomic(path, data)
        manifest['shards'][name] = sorted(bodies)
        manifest['c_bytes'] += len(data)
    for stale in sources.glob('*.c'):
        if stale.name not in manifest['shards'] and stale.name != 'dispatch.c':
            stale.unlink()
    dispatch = dispatch_source(layout, selected, imports, sorted(counts))
    data = dispatch.encode()
    path = sources / 'dispatch.c'
    if not path.exists() or path.read_bytes() != data:
        _write_atomic(path, data)
    manifest['c_bytes'] += len(data)
    shutil.rmtree(parts, ignore_errors=True)
    manifest['timing'] = dict(load=round(load_time, 1), lift=round(lift_time, 1),
                              write=round(time.time() - started - load_time - lift_time, 1))
    log(f'{len(selected)} named roots, {len(roots)} roots ({taken} from code pointers), {len(counts)} native functions, '
        f'{manifest["instructions"]} instructions, {len(used_imports)} direct imports, '
        f'{len(shards)} files, {manifest["c_bytes"] / 1e6:.0f} MB of C, {len(not_code)} non-code roots ignored '
        f'(load {load_time:.0f} s, lift {lift_time:.0f} s)')
    if manifest['not_compiled']:
        log(f'{len(manifest["not_compiled"])} functions NOT compiled (runtime trap if reached), e.g. '
            + '; '.join(f'{a}: {why}' for a, why in list(manifest['not_compiled'].items())[:5]))
    return manifest


def dispatch_source(layout, selected, imports, functions):
    """rt_lookup over every compiled function: a page index over the guest
    code range, then a binary search among the few entries of that page."""
    code = ['/* Generated by tools/lift.py. Do not edit. */', '#include "rt.h"']
    code += [f'void fn_{entry:x}(CPU *c);' for entry in functions]
    code.append(f'static const uint32_t entry_address[{max(1, len(functions))}] = {{')
    code += [f'{entry:#x}u,' for entry in functions] or ['0']
    code.append('};')
    code.append(f'static const gfn entry_function[{max(1, len(functions))}] = {{')
    code += [f'fn_{entry:x},' for entry in functions] or ['0']
    code.append('};')
    if functions:
        first, last = functions[0] >> 12, functions[-1] >> 12
        starts, index = [], 0
        for page in range(first, last + 2):
            while index < len(functions) and functions[index] >> 12 < page:
                index += 1
            starts.append(index)
        code.append(f'static const uint32_t page_start[{len(starts)}] = {{')
        code += [', '.join(f'{v}u' for v in starts[i:i + 16]) + ',' for i in range(0, len(starts), 16)]
        code.append('};')
        code += ['gfn rt_lookup(uint32_t target) {',
                 f'    uint32_t page = target >> 12;',
                 f'    if (page < {first:#x}u || page > {last:#x}u) return NULL;',
                 f'    uint32_t lo = page_start[page - {first:#x}u], hi = page_start[page - {first:#x}u + 1];',
                 '    while (lo < hi) {',
                 '        uint32_t mid = lo + (hi - lo) / 2;',
                 '        if (entry_address[mid] < target) lo = mid + 1; else hi = mid;',
                 '    }',
                 '    return lo < ' + str(len(functions)) + 'u && entry_address[lo] == target ? entry_function[lo] : NULL;',
                 '}']
    else:
        code.append('gfn rt_lookup(uint32_t target) { (void)target; (void)entry_address; (void)entry_function; return NULL; }')
    code.append('uint32_t rt_export(const char *name) {')
    for name, entry in selected:
        code.append(f'if (!strcmp(name, {json.dumps(name)})) return {entry:#x}u;')
    code.extend(['return 0; }', 'const char *rt_import_name(uint32_t target) { switch (target) {'])
    for addr, name in sorted(imports.items()):
        code.append(f'case {addr:#x}u: return {json.dumps(name)};')
    code.extend(['default: return NULL; } }', 'int rt_load_images(const char *directory) {'])
    for mod in layout['modules']:
        for seg in mod['loads']:
            code.append(f'if (rt_load_segment(directory, {json.dumps(mod["name"])}, '
                        f'{seg["off"]}u, {seg["vaddr"]:#x}u, {seg["filesz"]}u, {seg["memsz"]}u)) return -1;')
        tls = mod.get('tls')
        if tls:
            code.append(f'if (rt_register_tls(directory, {json.dumps(mod["name"])}, {mod["modid"]}u, '
                        f'{tls["off"]}u, {tls["filesz"]}u, {tls["memsz"]}u, {tls["align"]}u)) return -1;')
    for item in layout.get('data_imports', []):
        code.append(f'if (rt_init_import_data({json.dumps(item["name"])}, {item["addr"]:#x}u, {item["size"]}u)) return -1;')
    code.extend(['return 0; }', ''])
    code.append(registry.generate(layout))
    return '\n'.join(code)


# ---------------------------------------------------------------------------
# Greffons PE de libMiles (bin/linux32/*.asi) : code relogeable, indexé par RVA.
#
# libMiles charge ces DLL Windows avec son propre chargeur, à une base choisie
# à l'exécution (mmap), puis appelle leurs fonctions par pointeurs. Le code est
# traduit ici au ImageBase préféré du fichier ; chaque mot couvert par une
# relocation de base est marqué `@valeur` (voir address/Operand) et vaut
# `valeur + d_` à l'exécution, comme toute adresse de code émise (appels,
# adresses de retour, tables de switch). runtime/pe.c reconnaît l'image en
# mémoire, fixe d_ et aiguille par RVA.

_TOKEN = re.compile(r'([+-]\s*)?\b(0x[0-9a-f]+)\b')


def mark_relocations(insn, words, image):
    """Instruction `insn` dont les octets couvrent les mots relocalisés `words`
    (adresses au ImageBase préféré). Chaque mot doit apparaître comme un
    opérand (immédiat ou déplacement) de même valeur ; sinon l'instruction est
    refusée (`unrelocatable`), jamais traduite avec une constante fausse."""
    pc_end, op, args = insn
    values = [image.u32(w - image.image_base) for w in words]
    tokens = [(m, ((-int(m[2], 16)) if m[1] and m[1].strip() == '-' else int(m[2], 16)) & 0xffffffff)
              for m in _TOKEN.finditer(args)]
    for value in set(values):
        if sum(v == value for _, v in tokens) != values.count(value):
            return (pc_end, 'unrelocatable', f'{op} {args}')
    out, last = [], 0
    for m, value in tokens:
        if value in values:
            out.append(args[last:m.start()])
            out.append(('+ ' if m[1] else '') + ('@c' if image.code_target(value) else '@') + f'{value:#x}')
            last = m.end()
    out.append(args[last:])
    return (pc_end, op, ''.join(out))


class PeInstructions(dict):
    """Instructions d'un greffon PE, relocalisations marquées. Le code des
    DLL MSVC contient des données (tables de switch) : le désassemblage
    linéaire peut s'y désynchroniser. Toute adresse de code demandée qui n'est
    pas une frontière connue est donc redécodée à partir d'elle ; le décodage
    d'une adresse ne dépend que de ses octets, l'union des décodages reste
    cohérente. `raw` garde le texte non marqué (reconnaissance des tables)."""
    WINDOW = 0x800

    def __init__(self, image, objdump):
        super().__init__()
        self.image, self.objdump = image, objdump
        self.raw, self.previous, self.decoded_windows = {}, {}, set()
        self.relocated = sorted(image.image_base + r for r in image.relocs)
        self._add(disassemble(image.path, 0, objdump))

    def _add(self, decoded):
        import bisect
        for pc, insn in decoded.items():
            if pc in self.raw or not self.image.executable(pc):
                continue
            self.raw[pc] = insn
            self.previous.setdefault(insn[0], pc)
            nxt = insn[0]
            low = bisect.bisect_left(self.relocated, pc - 3)
            high = bisect.bisect_left(self.relocated, nxt)
            words = self.relocated[low:high]
            if not words:
                dict.__setitem__(self, pc, insn)
            elif any(w <= pc or w + 4 > nxt for w in words):
                dict.__setitem__(self, pc, (nxt, 'unrelocatable', f'{insn[1]} {insn[2]}'))
            else:
                dict.__setitem__(self, pc, mark_relocations(insn, words, self.image))

    def _fault(self, pc):
        if dict.__contains__(self, pc) or pc in self.decoded_windows or not self.image.executable(pc):
            return False
        self.decoded_windows.add(pc)
        rva = pc - self.image.image_base
        section = next(x for x in self.image.executable_sections() if x['rva'] <= rva < x['rva'] + x['size'])
        stop = min(pc + self.WINDOW, self.image.image_base + section['rva'] + section['size'])
        self._add(disassemble(self.image.path, 0, self.objdump, pc, stop))
        return True

    def __contains__(self, pc):
        return dict.__contains__(self, pc) or (self._fault(pc) and dict.__contains__(self, pc))

    def __getitem__(self, pc):
        if not dict.__contains__(self, pc):
            self._fault(pc)
        return dict.__getitem__(self, pc)

    def get(self, pc, default=None):
        return self[pc] if pc in self else default


class _RawView:
    """Vue non marquée de PeInstructions pour JumpTables (même décodage paresseux)."""
    def __init__(self, insns):
        self.insns = insns
        self.previous = insns.previous

    def __contains__(self, pc):
        return pc in self.insns

    def __getitem__(self, pc):
        self.insns[pc]
        return self.insns.raw[pc]

    def __len__(self):
        return len(self.insns.raw)

    def items(self):
        return self.insns.raw.items()


class PeTables(JumpTables):
    """Tables de switch MSVC (`jmp [4*I + T]`, T relocalisé) lues dans l'image
    au ImageBase préféré ; le code généré compare la cible calculée à
    l'exécution, moins d_, à ces valeurs."""

    def __init__(self, image, insns):
        super().__init__({'modules': []}, _RawView(insns))
        self.image = image

    def module(self, address):
        return self.image if 0 <= address - self.image.image_base < self.image.image_size else None

    def read32(self, address):
        if self.module(address) is None or self.module(address + 3) is None:
            return None
        return self.image.u32(address - self.image.image_base)


def pe_pointer_roots(image, insns):
    """Valeurs relocalisées qui désignent du code et servent de pointeur de
    fonction : stockées hors du code, immédiates d'une instruction, ou entrées
    de table dans le code (cibles de switch). Une valeur qui n'apparaît que
    comme déplacement mémoire (`[4*eax + T]`, base de table) n'en est pas une."""
    import bisect
    starts = sorted(insns.raw)
    roots = set()
    for location, value in image.code_pointers():
        address = image.image_base + location
        if not image.executable(address):
            roots.add(value)
            continue
        covering = []
        i = bisect.bisect_right(starts, address) - 1
        while i >= 0 and address - starts[i] < 16:
            pc = starts[i]
            nxt, _, args = insns.raw[pc]
            if pc < address and address + 4 <= nxt:
                covering.append(args)
            i -= 1
        token = f'{value:#x}'
        bracketed = lambda args: any(token in part for part in re.findall(r'\[[^\]]*\]', args))
        if not covering or not all(bracketed(args) for args in covering):
            roots.add(value)
    return roots


def generate_pe(image, tag, objdump='objdump', log=print):
    """C relogeable d'un greffon PE et son descripteur RtPeImage (runtime/pe.c).
    Renvoie (source, statistiques). Une fonction non traduisible n'est pas
    émise : si Miles l'appelle, l'aiguillage piège (« not recompiled »)."""
    if not re.fullmatch(r'[a-z0-9_]+', tag):
        raise Unsupported(f'invalid plugin tag {tag!r}')
    for rva in image.relocs:
        sections = [x for x in image.executable_sections() if x['rva'] < rva + 4 and rva < x['rva'] + x['size']]
        if sections and not (sections[0]['rva'] <= rva and rva + 4 <= sections[0]['rva'] + sections[0]['size']):
            raise Unsupported(f'{image.name}: relocation {rva:#x} straddles a code section boundary')
    insns = PeInstructions(image, objdump)
    tables = PeTables(image, insns)
    base = image.image_base
    named = {base + rva for rva in image.exports.values()}
    if image.entry_rva:
        named.add(base + image.entry_rva)
    named = {a for a in named if image.executable(a)}
    roots = named | {a for a in pe_pointer_roots(image, insns) if image.executable(a)}
    # Frontières de fonction : exportations, point d'entrée et cibles d'appels
    # directs (point fixe) ; les pointeurs de code sont des racines compilées
    # mais ne coupent pas une fonction (comme data_ptrs des modules ELF).
    boundaries, pending, seen = set(named), sorted(roots), set()
    while pending:
        entry = pending.pop()
        if entry in seen:
            continue
        seen.add(entry)
        callees = set()
        scan(entry, insns, boundaries, callees, tables=tables)
        for target in callees:
            if image.executable(target) and target not in boundaries:
                boundaries.add(target)
                pending.append(target)
    functions, failures, pending = {}, {}, sorted(roots | boundaries, reverse=True)
    while pending:
        entry = pending.pop()
        if entry in functions or entry in failures:
            continue
        callees = set()
        try:
            functions[entry] = lift(entry, insns, boundaries, callees, tables, None, 'pe_delta')
        except Unsupported as exc:
            failures[entry] = str(exc)
            continue
        pending.extend(t for t in callees if image.executable(t))
    compiled = frozenset(functions)
    name = lambda address: f'pe_{tag}_{address:x}'
    rename = lambda body: re.sub(r'\bfn_([0-9a-f]+)\b', lambda m: name(int(m[1], 16)), body)
    lines = [f'/* Generated by tools/lift.py from {image.name}. Do not edit. */', '#include "rt.h"',
             'static uint32_t pe_delta[2];   // vues écrivable et exécutable - ImageBase, fixés par rt_pe_lookup']
    lines += [f'static void {name(a)}(CPU *);' for a in sorted(compiled)]
    lines += [rename(link_direct(functions[a][0], compiled)[0]) for a in sorted(compiled)]
    code_sections = image.executable_sections()
    for i, section in enumerate(code_sections):
        data = image.mapped[section['rva']:section['rva'] + section['size']]
        lines.append(f'static const uint8_t section_{i}[{len(data)}] = {{')
        lines += [','.join(str(b) for b in data[j:j + 32]) + ',' for j in range(0, len(data), 32)]
        lines.append('};')
    lines.append('static const RtPeSection sections[] = {')
    lines += [f'{{{s["rva"]:#x}u, {s["size"]:#x}u, section_{i}}},' for i, s in enumerate(code_sections)]
    lines.append('};')
    code_relocs = [r for r in image.relocs if image.executable(base + r)]
    lines.append(f'static const uint32_t relocations[{max(1, len(code_relocs))}] = {{')
    # Bit 31 : la valeur désigne .text (relocalisée vers la vue exécutable).
    flagged = [r | (0x80000000 if image.code_target(image.u32(r)) else 0) for r in code_relocs]
    lines += [', '.join(f'{r:#x}u' for r in flagged[j:j + 12]) + ',' for j in range(0, len(flagged), 12)]
    lines += ['};' if code_relocs else '0};']
    ordered = sorted(compiled)
    lines.append(f'static const uint32_t function_rvas[{max(1, len(ordered))}] = {{')
    lines += [', '.join(f'{a - base:#x}u' for a in ordered[j:j + 12]) + ',' for j in range(0, len(ordered), 12)]
    lines += ['};' if ordered else '0};']
    lines.append(f'static const gfn functions[{max(1, len(ordered))}] = {{')
    lines += [', '.join(name(a) for a in ordered[j:j + 6]) + ',' for j in range(0, len(ordered), 6)]
    lines += ['};' if ordered else '0};']
    from peprep import fnv1a64
    lines += [f'static const RtPeImage image = {{{json.dumps(image.name)}, {base:#x}u, {image.image_size:#x}u, '
              f'{image.section_alignment:#x}u, {len(image.data)}u, {fnv1a64(image.data):#x}ull, sections, '
              f'{len(code_sections)}u, relocations, {len(code_relocs)}u, function_rvas, functions, '
              f'{len(ordered)}u, pe_delta}};',
              '__attribute__((constructor)) static void pe_register(void) { rt_pe_register_image(&image); }', '']
    stats = dict(name=image.name, functions=len(compiled), instructions=sum(n for _, n in functions.values()),
                 roots=len(roots), boundaries=len(boundaries), relocations=len(code_relocs),
                 not_compiled={f'{a - base:#x}': why for a, why in sorted(failures.items())},
                 named_not_compiled=sorted(f'{a - base:#x}' for a in named if a in failures))
    log(f'{image.name}: {stats["functions"]} functions, {stats["instructions"]} instructions, '
        f'{len(failures)} not compiled' + (f' (exported/entry: {", ".join(stats["named_not_compiled"])})'
                                           if stats['named_not_compiled'] else ''))
    return '\n'.join(lines), stats


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--layout', required=True, type=Path)
    ap.add_argument('--out', required=True, type=Path)
    ap.add_argument('--objdump', default='objdump')
    ap.add_argument('--allow-unimplemented-imports', action='store_true',
                    help='emit runtime traps for missing shims instead of rejecting direct imports')
    ap.add_argument('exports', nargs='+')
    args = ap.parse_args()
    try:
        code = generate(json.loads(args.layout.read_text()), args.exports, args.objdump, args.allow_unimplemented_imports)
    except Unsupported as exc:
        ap.exit(1, f'lift: {exc}\n')
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(code)


if __name__ == '__main__':
    main()
