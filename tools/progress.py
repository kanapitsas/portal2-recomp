#!/usr/bin/env python3
"""Conservative static constructor audit; translatable does not mean executed."""
import argparse
from collections import Counter
from concurrent.futures import ProcessPoolExecutor
import json
import os
import re
import subprocess
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import games  # noqa: E402
import lift  # noqa: E402

ROOT = games.ROOT


def audit(layout, objdump):
    insns = {}
    entries = {entry for mod in layout['modules'] for entry in mod['entries']}
    imports = lift.import_map(layout)
    entries.update(imports)
    for mod in layout['modules']:
        insns.update(lift.disassemble(mod['image'], mod['base'], objdump))
    records = []
    for mod in layout['modules']:
        roots = [('dt', mod['init'])] + [(str(i), addr) for i, addr in enumerate(mod['init_array'])]
        for index, root in roots:
            if not root or root == 0xffffffff:
                continue
            pending, seen, missing, errors = [root], set(), set(), set()
            count = 0
            while pending:
                entry = pending.pop()
                if entry in seen:
                    continue
                seen.add(entry)
                if entry in imports:
                    if imports[entry] not in lift.SHIMS:
                        missing.add(imports[entry])
                    continue
                dependencies = set()
                try:
                    _, instructions = lift.lift(entry, insns, entries, dependencies)
                    count += instructions
                except lift.Unsupported as exc:
                    errors.add(str(exc))
                pending.extend(sorted(dependencies))
            records.append(dict(root=f'{mod["name"]}:init:{index}', address=hex(root),
                                translatable=not errors, discovered_functions=len(seen),
                                translated_instructions=count, missing_shims=sorted(missing),
                                instruction_blockers=sorted(errors)))
    return dict(note='Static lower-bound audit: discovery stops at unsupported instructions; '
                     'indirect targets and runtime execution require separate validation.', constructors=records)


def operand_form(args):
    """Shape of an operand list: registers, memory widths and immediates, not values."""
    forms = []
    for text in [x.strip() for x in args.split(',')] if args else []:
        if text in lift.REGS:
            forms.append(f'r{lift.REGS[text][1]}')
        elif re.fullmatch(r'xmm[0-7]', text):
            forms.append('xmm')
        elif re.fullmatch(r'mm[0-7]', text):
            forms.append('mm')
        elif text.startswith('st'):
            forms.append('st')
        elif '[' in text:
            m = re.match(r'(byte|word|dword|qword|xmmword|tbyte|ptr)?', text)
            forms.append('m' + {'byte': '8', 'word': '16', 'dword': '32', 'qword': '64',
                                'xmmword': '128', 'tbyte': '80'}.get(m[1] or '', ''))
        elif re.fullmatch(r'-?(0x[0-9a-f]+|[0-9]+)', text):
            forms.append('imm')
        else:
            forms.append(text)
    return ', '.join(forms)


module_roots = lift.module_roots   # mêmes racines que build_game.py


def audit_module(job):
    """Every function reachable by direct calls from all known roots of one module."""
    module, imports, foreign, objdump, cache, want_forms = job
    visited = set() if want_forms else None
    insns = lift.module_instructions(module, objdump, cache)
    plt = module.get('plt') or [0, 0]
    pending = module_roots(module)
    tables = lift.JumpTables({'modules': [module]}, insns)
    entries = set(pending) | set(imports) | foreign
    functions, external, used_imports = {}, set(), Counter()
    while pending:
        entry = pending.pop()
        if entry in functions:
            continue
        if entry in imports:
            used_imports[imports[entry]] += 1
            functions[entry] = None
            continue
        if entry not in insns:
            external.add(entry)
            functions[entry] = None
            continue
        callees = set()
        count, failures, indirect = lift.scan(entry, insns, entries, callees, visited, tables)
        functions[entry] = (count, failures, indirect)
        pending.extend(sorted(callees - functions.keys()))
    by_form, by_op, fn_by_op = Counter(), Counter(), Counter()
    only_blocker, examples, blocked = Counter(), {}, []
    indirect_kinds = Counter()
    lifted = total = 0
    for entry, record in functions.items():
        if record is None:
            continue
        count, failures, indirect = record
        total += count
        for pc, resolved in indirect:
            nxt, op, args = insns[pc]
            if resolved:
                indirect_kinds['switch table resolved statically'] += 1
            elif plt[0] <= pc < plt[1]:
                indirect_kinds['plt stub'] += 1
            elif lift.REGS.get(args):
                indirect_kinds['jmp register (computed tail call or unresolved table)'] += 1
            else:
                indirect_kinds['jmp memory'] += 1
        if not failures:
            lifted += 1
            continue
        ops = set()
        for pc, op, args, reason in failures:
            key = f'{op} {operand_form(args)}'.strip()
            by_form[key] += 1
            by_op[op] += 1
            ops.add(op)
            examples.setdefault(key, f'{pc:#x}: {op} {args} ({reason})')
        for op in ops:
            fn_by_op[op] += 1
        if len(ops) == 1:
            only_blocker[next(iter(ops))] += 1
        blocked.append(entry)
    forms = {}
    for pc in sorted(visited or ()):
        if pc in insns:
            forms.setdefault(f'{insns[pc][1]} {operand_form(insns[pc][2])}'.strip(), (pc, insns[pc]))
    return dict(forms=forms, module=module['name'], roots=len(module_roots(module)), functions=sum(r is not None for r in functions.values()),
                translatable_functions=lifted, instructions=total, unknown_targets=len(external),
                imports=dict(used_imports), unsupported_by_form=dict(by_form), unsupported_by_op=dict(by_op),
                functions_by_op=dict(fn_by_op), sole_blocker=dict(only_blocker), examples=examples,
                indirect_jumps=dict(indirect_kinds), blocked_functions=[hex(e) for e in sorted(blocked)])


def form_check_source(forms):
    """One tiny C function per distinct (mnemonic, operand form) found in the game."""
    code = ['#include "rt.h"']
    for key, (pc, insn) in sorted(forms.items(), key=lambda kv: kv[1][0]):
        nxt, op, args = insn
        mini = {pc: tuple(insn)}
        targets = [nxt]
        if op.startswith('j') or op == 'call':
            try:
                targets.append(int(args, 0))
            except ValueError:
                pass
        for target in targets:
            mini.setdefault(target, (target + 1, 'ret', ''))
        try:
            body, _ = lift.lift(pc, mini, frozenset(), set())
        except lift.Unsupported:
            continue
        code.append(f'/* {key} */')
        code.append(body.replace('static void', 'void', 1))
    return '\n'.join(code) + '\n'


def compile_forms(forms, out):
    source = out.with_suffix('.forms.c')
    source.write_text(form_check_source(forms))
    result = subprocess.run(['clang', '-fsyntax-only', '-Wall', '-Wextra', '-Werror', '-Wno-missing-prototypes',
                             f'-I{ROOT / "runtime"}', str(source)], capture_output=True, text=True)
    return source, result


def audit_all(layout, objdump, cache, jobs, names=None, want_forms=False):
    imports = lift.import_map(layout)
    modules = [m for m in layout['modules'] if not names or m['name'] in names]
    # Largest modules first so the pool stays busy.
    modules.sort(key=lambda m: -Path(m['image']).stat().st_size)
    # Exported entries of other modules: direct cross-module jumps are tail calls.
    work = [(m, imports, {e for o in layout['modules'] if o is not m for e in o['entries']}, objdump, cache,
             want_forms) for m in modules]
    if jobs > 1:
        with ProcessPoolExecutor(max_workers=jobs) as pool:
            results = list(pool.map(audit_module, work))
    else:
        results = [audit_module(item) for item in work]
    results.sort(key=lambda r: r['module'])
    total = dict(functions=0, translatable_functions=0, instructions=0, unknown_targets=0)
    sums = {key: Counter() for key in ('unsupported_by_form', 'unsupported_by_op', 'functions_by_op',
                                       'sole_blocker', 'imports', 'indirect_jumps')}
    for r in results:
        for key in total:
            total[key] += r[key]
        for key, counter in sums.items():
            counter.update(r[key])
    missing = {name: n for name, n in sums['imports'].items() if name not in lift.SHIMS}
    forms = {}
    for r in results:
        for key, example in r.pop('forms').items():
            forms.setdefault(key, example)
    return dict(note='Static audit: all symbol, export, init/fini and data-pointer roots of every prepared '
                     'module, followed through direct calls. Collect mode continues past rejected '
                     'instructions; indirect targets (vtables, switch tables) beyond data pointers are '
                     'not discovered. Translatable does not mean executed or correct.',
                totals=total, unsupported_by_op=dict(sums['unsupported_by_op'].most_common()),
                functions_by_op=dict(sums['functions_by_op'].most_common()),
                sole_blocker=dict(sums['sole_blocker'].most_common()),
                unsupported_by_form=dict(sums['unsupported_by_form'].most_common()),
                indirect_jumps=dict(sums['indirect_jumps']),
                missing_imports=dict(sorted(missing.items(), key=lambda kv: -kv[1])),
                distinct_forms=len(forms), modules=results), forms


def print_summary(result, top):
    t = result['totals']
    print(f'{t["functions"]} functions, {t["translatable_functions"]} fully translatable '
          f'({100 * t["translatable_functions"] / max(t["functions"], 1):.1f} %), {t["instructions"]} instructions')
    print(f'indirect jumps: {result["indirect_jumps"]}')
    print('mnemonic: occurrences / functions touched / sole blocker')
    for op, n in list(result['unsupported_by_op'].items())[:top]:
        print(f'  {op:14} {n:8} {result["functions_by_op"].get(op, 0):7} {result["sole_blocker"].get(op, 0):7}')
    print(f'{len(result["missing_imports"])} imports without shim are called directly')


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--layout', type=Path,
                    help='prepared layout (default: build/game/prep/layout.json with --all, else build/prep/layout.json; '
                         'build/hl2/prep/layout.json for Half-Life 2)')
    ap.add_argument('--out', default=ROOT / 'build/progress.json', type=Path)
    ap.add_argument('--objdump', default='objdump')
    ap.add_argument('--all', action='store_true', help='audit every function of every prepared module')
    ap.add_argument('--modules', nargs='*', help='restrict --all to these module names')
    ap.add_argument('--cache', default=ROOT / 'build/disasm-cache', type=Path)
    ap.add_argument('--jobs', type=int, default=os.cpu_count() or 1)
    ap.add_argument('--top', type=int, default=40)
    ap.add_argument('--compile-forms', action='store_true',
                    help='compile one lifted example of every distinct instruction form (clang -fsyntax-only)')
    args = ap.parse_args()
    if args.layout is None:
        args.layout = ROOT / ('build/game/prep/layout.json' if args.all else 'build/prep/layout.json')
    if args.all:
        result, forms = audit_all(lift.load_layout(args.layout), args.objdump, args.cache, args.jobs,
                                  args.modules, args.compile_forms)
        args.out.parent.mkdir(parents=True, exist_ok=True)
        args.out.write_text(json.dumps(result, indent=1) + '\n')
        print_summary(result, args.top)
        if args.compile_forms:
            source, compiled = compile_forms(forms, args.out)
            print(f'{len(forms)} distinct instruction forms -> {source}: '
                  + ('compiles' if not compiled.returncode else 'COMPILE ERRORS'))
            if compiled.returncode:
                print(compiled.stderr[:4000])
                raise SystemExit(1)
        return
    result = audit(lift.load_layout(args.layout), args.objdump)
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(json.dumps(result, indent=2) + '\n')
    for item in result['constructors']:
        status = 'C OK' if item['translatable'] else 'BLOCKED'
        print(f'{item["root"]}: {status}; {len(item["missing_shims"])} missing direct shims')
        for error in item['instruction_blockers']:
            print(f'  {error}')


if __name__ == '__main__':
    main()
