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

    def test_call_drops_save_only_when_nothing_reads_after(self):
        dead = {0x1000: CALL.replace('L1010', 'L1010'), 0x1010: 'a = c->eax; b = 0u; r = f_logic(&f, a & b, 32);  goto L1012;',
                0x1012: 'if (cond(&f, CC_E)) { goto L1000; } goto L1014;',
                0x1014: 'c->flags = f; rt_return(c); c->esp += 0u; return;'}
        flow = {0x1000: [0x1010], 0x1010: [0x1012], 0x1012: [0x1000, 0x1014], 0x1014: []}
        out = self.run_pass(dead, flow)
        self.assertNotIn('c->flags = f', out[0x1000])
        self.assertNotIn('f = c->flags', out[0x1000])
        live = dict(dead)
        live[0x1010] = 'if (cond(&f, CC_B)) { goto L1000; } goto L1014;'   # lit CF rendu par l'appelé
        out = self.run_pass(live, {0x1000: [0x1010], 0x1010: [0x1000, 0x1014], 0x1014: []})
        self.assertIn('c->flags = f;', out[0x1000])
        self.assertIn('f = c->flags;', out[0x1000])

    def test_eflags_users_are_left_alone(self):
        code = {0x1000: 'a = c->eax; b = 1u; r = f_add(&f, a, b, 32); c->eax = r; goto L1003;',
                0x1003: 'PUSH(c, get_eflags(c, &f)); goto L1004;',
                0x1004: 'c->flags = f; rt_return(c); c->esp += 0u; return;'}
        out = self.run_pass(code, {0x1000: [0x1003], 0x1003: [0x1004], 0x1004: []})
        self.assertEqual(out, code)


if __name__ == '__main__':
    unittest.main()
