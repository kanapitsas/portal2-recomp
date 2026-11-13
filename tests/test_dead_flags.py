"""Passe dead_flags de tools/lift.py : écritures d'indicateurs inutiles."""
import sys
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent / 'tools'))
import lift  # noqa: E402

CALL = 'c->flags = f; rt_call_fn(c, fn_9000, 0x9000u, 0x1010u); if (c->eip != 0x1010u) goto resume; f = c->flags; goto L1010;'


class DeadFlags(unittest.TestCase):
    def run_pass(self, code, flow):
        return lift.dead_flags(code, flow)

    def test_overwritten_writer_loses_flags(self):
        code = {0x1000: 'a = c->eax; b = 1u; r = f_add(&f, a, b, 32); c->eax = r; goto L1003;',
                0x1003: 'a = c->eax; b = 2u; r = f_sub(&f, a, b, 32);  goto L1006;',
                0x1006: 'if (cond(&f, CC_E)) { goto L1000; } goto L1008;',
                0x1008: 'c->flags = f; rt_return(c); c->esp += 0u; return;'}
        flow = {0x1000: [0x1003], 0x1003: [0x1006], 0x1006: [0x1000, 0x1008], 0x1008: []}
        out = self.run_pass(code, flow)
        self.assertIn('n_add(a, b, 32)', out[0x1000])
        self.assertIn('f_sub(&f, a, b, 32)', out[0x1003])   # lu par jcc

    def test_flags_returned_by_ret_are_kept(self):
        code = {0x1000: 'a = c->eax; b = 1u; r = f_add(&f, a, b, 32); c->eax = r; goto L1003;',
                0x1003: 'c->flags = f; rt_return(c); c->esp += 0u; return;'}
        out = self.run_pass(code, {0x1000: [0x1003], 0x1003: []})
        self.assertIn('f_add(&f', out[0x1000])

    def test_partial_writer_does_not_kill(self):
        code = {0x1000: 'a = c->eax; b = 1u; r = f_sub(&f, a, b, 32);  goto L1003;',
                0x1003: 'a = c->ecx; r = f_inc(&f, a, 32); c->ecx = r; goto L1005;',
                0x1005: 'if (cond(&f, CC_B)) { goto L1000; } goto L1007;',
                0x1007: 'c->flags = f; rt_return(c); c->esp += 0u; return;'}
        flow = {0x1000: [0x1003], 0x1003: [0x1005], 0x1005: [0x1000, 0x1007], 0x1007: []}
        out = self.run_pass(code, flow)
        self.assertIn('f_sub(&f', out[0x1000])   # CF traverse inc

    def test_call_keeps_save_and_drops_unused_restore(self):
        dead = {0x1000: 'a = c->eax; b = 1u; r = f_add(&f, a, b, 32); c->eax = r; goto L1003;',
                0x1003: CALL,
                0x1010: 'a = c->eax; b = 0u; r = f_logic(&f, a & b, 32);  goto L1012;',
                0x1012: 'if (cond(&f, CC_E)) { goto L1000; } goto L1014;',
                0x1014: 'c->flags = f; rt_return(c); c->esp += 0u; return;'}
        flow = {0x1000: [0x1003], 0x1003: [0x1010], 0x1010: [0x1012], 0x1012: [0x1000, 0x1014], 0x1014: []}
        out = self.run_pass(dead, flow)
        self.assertIn('c->flags = f;', out[0x1003])      # l'appelé peut lire les drapeaux
        self.assertNotIn('f = c->flags', out[0x1003])    # rien ne les lit après
        self.assertIn('f_add(&f', out[0x1000])           # donc l'écriture d'avant reste
        live = dict(dead)
        live[0x1010] = 'if (cond(&f, CC_B)) { goto L1000; } goto L1014;'   # lit CF rendu par l'appelé
        out = self.run_pass(live, {0x1000: [0x1003], 0x1003: [0x1010], 0x1010: [0x1000, 0x1014], 0x1014: []})
        self.assertIn('c->flags = f;', out[0x1003])
        self.assertIn('f = c->flags;', out[0x1003])

    def test_eflags_users_are_left_alone(self):
        code = {0x1000: 'a = c->eax; b = 1u; r = f_add(&f, a, b, 32); c->eax = r; goto L1003;',
                0x1003: 'PUSH(c, get_eflags(c, &f)); goto L1004;',
                0x1004: 'c->flags = f; rt_return(c); c->esp += 0u; return;'}
        out = self.run_pass(code, {0x1000: [0x1003], 0x1003: [0x1004], 0x1004: []})
        self.assertEqual(out, code)

    def test_longjmp_resume_point_keeps_flags_live_across_calls(self):
        # 0x1000 écrit des drapeaux, 0x1003 appelle ; un longjmp peut revenir
        # en 0x1020, qui lit les drapeaux.
        code = {0x1000: 'a = c->eax; b = 1u; r = f_sub(&f, a, b, 32);  goto L1003;',
                0x1003: CALL.replace('L1010', 'L1010'),
                0x1010: 'a = c->eax; b = 0u; r = f_logic(&f, a & b, 32);  goto L1012;',
                0x1012: 'c->flags = f; rt_return(c); c->esp += 0u; return;',
                0x1020: 'if (cond(&f, CC_B)) { goto L1012; } goto L1012;'}
        flow = {0x1000: [0x1003], 0x1003: [0x1010], 0x1010: [0x1012], 0x1012: [], 0x1020: [0x1012]}
        out = lift.dead_flags(code, flow, frozenset({0x1020}))
        self.assertIn('f = c->flags;', out[0x1003])   # relecture gardée pour la reprise
        self.assertNotIn('f = c->flags;', lift.dead_flags(code, flow)[0x1003])


def program(*items, base=0x1000):
    """{pc: (next, op, args)} of consecutive 4-byte instructions."""
    insns, pc = {}, base
    for op, args in items:
        insns[pc] = (pc + 4, op, args)
        pc += 4
    return insns


def body(code, pc):
    """Generated statement of the instruction at pc."""
    return next(line for line in code.splitlines() if line.startswith(f'L{pc:x}: '))


class PerFlag(unittest.TestCase):
    """Per-flag liveness (conservative mode: same boundaries as before)."""

    def test_inc_passes_cf_through_but_rewrites_zf(self):
        keep = program(('sub', 'eax, 1'), ('inc', 'ecx'), ('jb', '0x1000'), ('ret', ''))
        code, _ = lift.lift(0x1000, keep)
        self.assertIn('f_sub(&f', body(code, 0x1000))    # CF of sub read by jb across inc
        kill = program(('sub', 'eax, 1'), ('inc', 'ecx'), ('je', '0x1000'), ('add', 'eax, 2'), ('ret', ''))
        code, _ = lift.lift(0x1000, kill)
        self.assertIn('n_sub(', body(code, 0x1000))      # ZF rewritten by inc: sub's flags all dead
        self.assertIn('f_inc(&f', body(code, 0x1004))    # inc's ZF read by je

    def test_add_then_adc_keeps_carry(self):
        code, _ = lift.lift(0x1000, program(('add', 'eax, 1'), ('adc', 'edx, 0'), ('add', 'eax, 2'),
                                            ('je', '0x1000'), ('ret', '')))
        self.assertIn('f_add(&f', body(code, 0x1000))    # CF read by adc
        self.assertIn('f_adc(&f', body(code, 0x1004))    # no twin: always kept
        # adc writes all six flags: nothing of it reaches je, the second add rewrites them
        self.assertIn('f_add(&f', body(code, 0x1008))

    def test_writer_with_all_flags_dead_becomes_twin(self):
        code, _ = lift.lift(0x1000, program(('inc', 'eax'), ('shl', 'edx, cl'), ('cmp', 'eax, 3'),
                                            ('jb', '0x1000'), ('ret', '')))
        self.assertIn('n_inc(', body(code, 0x1000))
        self.assertIn('n_shl(', body(code, 0x1004))      # variable count: may write, all dead
        self.assertIn('f_sub(&f', body(code, 0x1008))

    def test_variable_shift_does_not_kill(self):
        code, _ = lift.lift(0x1000, program(('add', 'eax, 1'), ('shl', 'edx, cl'), ('jb', '0x1000'),
                                            ('ret', '')))
        self.assertIn('f_add(&f', body(code, 0x1000))    # shl by 0 leaves add's CF
        code, _ = lift.lift(0x1000, program(('add', 'eax, 1'), ('shl', 'edx, 3'), ('jb', '0x1000'),
                                            ('add', 'eax, 1'), ('ret', '')))
        self.assertIn('n_add(', body(code, 0x1000))      # shl by 3 rewrites CF
        self.assertIn('f_shl(&f', body(code, 0x1004))

    def test_cmpxchg8b_writes_only_zf(self):
        code, _ = lift.lift(0x1000, program(('cmp', 'eax, 1'), ('lock_cmpxchg8b', 'qword ptr [esi]'),
                                            ('jb', '0x1000'), ('add', 'eax, 1'), ('ret', '')))
        self.assertIn('f_sub(&f', body(code, 0x1000))    # CF of cmp survives cmpxchg8b
        code, _ = lift.lift(0x1000, program(('cmp', 'eax, 1'), ('lock_cmpxchg8b', 'qword ptr [esi]'),
                                            ('je', '0x1000'), ('add', 'eax, 1'), ('ret', '')))
        self.assertIn('n_sub(', body(code, 0x1000))

    def test_every_emitted_flag_form_is_classified(self):
        # Forms that mention f must be recognised exactly (or be opaque on
        # purpose): a misclassified reader would silently drop a live write.
        forms = {
            ('jb', '0x1000'): (lift.CF, 0, 0), ('setle', 'al'): (lift.ZF | lift.SF | lift.OF, 0, 0),
            ('cmovp', 'eax, ecx'): (lift.PF, 0, 0), ('fcmovbe', 'st, st(1)'): (lift.CF | lift.ZF, 0, 0),
            ('add', 'eax, 1'): (0, 63, 63), ('test', 'al, 1'): (0, 63, 63), ('neg', 'eax'): (0, 63, 63),
            ('imul', 'eax, ecx'): (0, 63, 63), ('dec', 'eax'): (0, 62, 62), ('lock_inc', 'dword ptr [esi]'): (0, 62, 62),
            ('shr', 'eax'): (0, 31, 31), ('sar', 'eax, 0x20'): (0, 0, 31), ('shl', 'eax, cl'): (0, 0, 31),
            ('adc', 'eax, 0'): (lift.CF, 63, 63), ('sbb', 'eax, eax'): (lift.CF, 63, 63),
            ('lock_adc', 'dword ptr [esi], 1'): (lift.CF, 63, 63),
            ('rol', 'eax, 0x5'): (0, lift.CF, lift.CF | lift.OF), ('ror', 'eax'): (0, lift.CF | lift.OF, lift.CF | lift.OF),
            ('rcl', 'eax, 1'): (lift.CF, lift.CF | lift.OF, lift.CF | lift.OF), ('rcr', 'eax, cl'): (lift.CF, 0, lift.CF | lift.OF),
            ('shld', 'eax, edx, 0x4'): (0, 31, 31), ('shrd', 'eax, edx, cl'): (0, 0, 31),
            ('bt', 'eax, 3'): (0, lift.CF, lift.CF), ('bsr', 'eax, ecx'): (0, lift.ZF, lift.ZF),
            ('mul', 'ecx'): (0, lift.CF | lift.OF, lift.CF | lift.OF), ('imul', 'cx'): (0, lift.CF | lift.OF, lift.CF | lift.OF),
            ('lock_cmpxchg8b', 'qword ptr [esi]'): (0, lift.ZF, lift.ZF), ('lock_cmpxchg', 'dword ptr [esi], ecx'): (0, 63, 63),
            ('xadd', 'dword ptr [esi], eax'): (0, 63, 63), ('sahf', ''): (0, 55, 55),
            ('comisd', 'xmm0, xmm1'): (0, 63, 63), ('fcomip', 'st, st(1)'): (0, 63, 63),
            ('cmpsb', 'byte ptr [esi], byte ptr es:[edi]'): (0, 63, 63),
            ('repz', 'cmpsb byte ptr [esi], byte ptr es:[edi]'): (0, 0, 63),
            ('repne', 'scasb al, byte ptr es:[edi]'): (0, 0, 63),
            ('pushfd', ''): (63, 0, 0), ('lahf', ''): (63, 0, 0), ('popfd', ''): (0, 63, 63),
            ('mov', 'eax, 1'): (0, 0, 0), ('lea', 'eax, [eax + 4]'): (0, 0, 0),
        }
        for (op, args), expected in forms.items():
            code, _ = lift.translate(0x1000, (0x1004, op, args))
            self.assertEqual(lift._flag_effect(code)[:3], expected, f'{op} {args}: {code}')
        self.assertEqual(lift._flag_effect('x = f; goto L1004;')[:3], (63, 0, 63))   # unknown: opaque


class AbiMode(unittest.TestCase):
    CALLER = program(('cmp', 'eax, 1'), ('call', '0x9000'), ('add', 'eax, 1'), ('ret', ''))

    def test_call_and_ret_drop_save_and_reload(self):
        code, _ = lift.lift(0x1000, self.CALLER, abi=True)
        self.assertNotIn('c->flags = f;', code)
        self.assertNotIn(' f = c->flags; goto', code)
        self.assertIn('FL f = c->flags;', code)          # entry load stays
        self.assertIn('n_sub(', body(code, 0x1000))      # flags dead across the call
        self.assertIn('n_add(', body(code, 0x1008))      # dead at ret
        conservative, _ = lift.lift(0x1000, self.CALLER)
        self.assertIn('c->flags = f; rt_call_fn', conservative)
        self.assertIn('f_sub(&f', body(conservative, 0x1000))
        self.assertIn('c->flags = f; rt_return', conservative)

    def test_indirect_call_and_reload(self):
        insns = program(('call', 'eax'), ('jb', '0x1000'), ('ret', ''))
        code, _ = lift.lift(0x1000, insns, abi=True)
        self.assertNotIn('c->flags = f;', code)
        self.assertNotIn('f = c->flags; goto', code)
        self.assertIn('f = c->flags; goto', lift.lift(0x1000, insns)[0])   # conservative: reload kept

    def test_tail_jumps_keep_the_save(self):
        insns = program(('add', 'eax, 1'), ('jb', '0x9000'), ('jmp', 'ecx'))
        code, _ = lift.lift(0x1000, insns, entries={0x9000}, abi=True)
        self.assertIn('c->flags = f; rt_jump_fn(c, fn_9000', code)
        self.assertIn('c->flags = f; rt_jump_any(c, a)', code)
        self.assertIn('f_add(&f', body(code, 0x1000))    # read by the save of the tail jumps

    def test_setjmp_resume_stays(self):
        insns = program(('call', '0x40'), ('ret', ''))
        code, _ = lift.lift(0x1000, insns, setjmp=lambda target: target == 0x40, abi=True)
        self.assertIn('resume: f = c->flags;', code)


class Proof(unittest.TestCase):
    def proof(self, insns, entries=frozenset()):
        report = {}
        lift.lift(0x1000, insns, entries=entries, report=report)
        return report

    def test_entry_live_with_a_conditional_first(self):
        report = self.proof(program(('je', '0x1008'), ('cmp', 'eax, 1'), ('ret', '')))
        mask, readers = report['entry_live']
        self.assertEqual(mask, lift.ZF)
        self.assertEqual(readers, [(0x1000, 'cond', 'je 0x1008')])
        self.assertNotIn('read_after_call', report)

    def test_no_entry_reader(self):
        self.assertEqual(self.proof(program(('cmp', 'eax, 1'), ('je', '0x1000'), ('call', '0x9000'),
                                            ('ret', ''))), {})

    def test_tail_jump_and_call_do_not_count_as_entry_reads(self):
        # The flags given to a tail-jump target are checked at that target's entry.
        self.assertEqual(self.proof(program(('jmp', '0x9000')), entries={0x9000}), {})
        self.assertEqual(self.proof(program(('call', '0x9000'), ('jb', '0x1000'), ('ret', '')))
                         .get('entry_live'), None)

    def test_pushfd_reader(self):
        report = self.proof(program(('pushfd', ''), ('pop', 'eax'), ('ret', '')))
        self.assertEqual(report['entry_live'], (63, [(0x1000, 'eflags', 'pushfd')]))

    def test_read_after_call(self):
        report = self.proof(program(('cmp', 'eax, 1'), ('call', '0x9000'), ('jb', '0x1000'), ('ret', '')))
        self.assertEqual(report['read_after_call'], [(0x1004, lift.CF, [(0x1008, 'cond', 'jb 0x1000')])])

    def test_inline_memcmp_after_call_is_not_a_read(self):
        insns = program(('call', '0x9000'), ('mov', 'esi, eax'), ('mov', 'ecx, 5'),
                        ('repz', 'cmpsb byte ptr [esi], byte ptr es:[edi]'), ('seta', 'al'), ('ret', ''))
        self.assertEqual(self.proof(insns), {})
        unknown = dict(insns)
        unknown[0x1008] = (0x100c, 'mov', 'ecx, edx')     # count may be 0: CF of the callee read
        report = self.proof(unknown)
        self.assertEqual([(pc, mask) for pc, mask, _ in report['read_after_call']], [(0x1000, lift.CF | lift.ZF)])

    def test_check_classifies_entry_readers(self):
        proof = dict(entry_live={0x1000: (63, [(0x1000, 'eflags', 'pushfd')]),
                                 0x2000: (lift.ZF, [(0x2000, 'cond', 'je 0x2010')]),
                                 0x3000: (lift.ZF, [(0x3000, 'cond', 'je 0x3010')]),
                                 0x4000: (lift.ZF, [(0x4000, 'cond', 'je 0x4010')]),
                                 0x5000: (63, [(0x5000, 'eflags', 'pushfd'), (0x5004, 'cond', 'jb 0x5000')])},
                     read_after_call={}, direct={0x3000})
        check = lift.flags_check({'modules': []}, proof, {0x4000, 0x5000})
        self.assertEqual(check['pushfd_only'], ['0x1000'])
        self.assertEqual(check['code_pointer_only'], ['0x2000'])
        self.assertEqual(len(check['offenders']), 3)   # direct target, boundary, pushfd + jb
        self.assertEqual(check['read_after_call'], 0)
        proof = dict(entry_live={}, direct=set(),
                     read_after_call={0x6000: [(0x6004, lift.CF, [(0x6008, 'cond', 'jb 0x6000')])]})
        check = lift.flags_check({'modules': []}, proof, set())
        self.assertEqual((check['read_after_call'], len(check['offenders'])), (1, 1))

    def test_flags_mode_environment(self):
        from unittest.mock import patch
        with patch.dict('os.environ', {}, clear=True):
            self.assertEqual(lift.flags_mode(), (True, []))
            self.assertEqual(lift.flags_mode(False), (False, ['conservative mode requested']))
        with patch.dict('os.environ', {'P2_LIFT_ABI_FLAGS': '0'}, clear=True):
            self.assertEqual(lift.flags_mode(), (False, ['P2_LIFT_ABI_FLAGS=0']))
        with patch.dict('os.environ', {'P2_LIFT_KEEP_FLAGS': '1'}, clear=True):
            self.assertEqual(lift.flags_mode(True), (False, ['P2_LIFT_KEEP_FLAGS is set']))


if __name__ == '__main__':
    unittest.main()
