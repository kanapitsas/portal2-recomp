#!/usr/bin/env python3
"""Static (lower bound) count of the Steam methods the game calls.

Follows the `SteamInternal_ContextInit` accessors -> object -> vtable -> `call [vt+off]`, the
fields of the global contexts those accessors fill and the global pointers to those contexts.
Contexts that are object members (Steam3Client...) escape this heuristic.
Usage: python3 tools/steam_callsites.py build/game/prep/layout.json
"""
import json, re, sys, struct, collections
sys.path.insert(0, 'tools')
import lift
L = json.load(open(sys.argv[1]))
imp = {i['name']: i['addr'] for i in L['imports']}
CI = hex(imp['SteamInternal_ContextInit'])
CREATORS = {hex(imp[n]) for n in ('SteamInternal_FindOrCreateUserInterface', 'SteamInternal_FindOrCreateGameServerInterface', 'SteamInternal_CreateInterface') if n in imp}
gen = open('runtime/steam_generated.c').read()
table = {(n.split('::')[0], int(s)): (n, f) for n, s, f in re.findall(r'\{"([^"]+)", \d+u, (\d+)u, [01], (\w+),', gen)}
agg = collections.Counter(); per_module = collections.defaultdict(set); unresolved = 0
for m in L['modules']:
    if 'libsteam_api.so' not in m['needed']: continue
    data = open(m['image'], 'rb').read()
    def at(a):
        for s in m['loads']:
            if s['vaddr'] <= a < s['vaddr'] + s['filesz']: return s['off'] + a - s['vaddr']
    def u32(a):
        o = at(a); return struct.unpack_from('<I', data, o)[0] if o is not None else None
    def cstr(a):
        o = at(a)
        return data[o:data.index(b'\0', o)].decode('latin1') if o is not None else None
    ins = lift.disassemble(m['image'], m['base'], 'objdump')
    pcs = sorted(ins)
    def version_of_init(fn):
        pc, last = fn, None
        for _ in range(40):
            if pc not in ins: return None
            nxt, op, args = ins[pc]
            if op == 'push' and re.fullmatch(r'0x[0-9a-f]+', args):
                s = cstr(int(args, 16))
                if s and re.fullmatch(r'[A-Za-z_]+\d+', s): last = s
            if op == 'call' and args in CREATORS: return last
            if op == 'ret': return None
            pc = nxt
    fields = {}   # adresse absolue de champ -> version
    fillers = {}  # fonction de remplissage -> {offset: version}
    def function_start(i):
        while i > 0 and not (ins[pcs[i - 1]][1] in ('ret', 'nop') or ins[pcs[i-1]][1] == 'jmp'): i -= 1
        return pcs[i]
    for i, pc in enumerate(pcs):
        nxt, op, args = ins[pc]
        if op != 'call' or args != CI: continue
        prev = ins[pcs[i - 1]]
        if prev[1] != 'push' or not re.fullmatch(r'0x[0-9a-f]+', prev[2]): continue
        fn = u32(int(prev[2], 16)); ver = version_of_init(fn) if fn else None
        if not ver: continue
        regs = {'eax': 'ctx'}
        for j in range(i + 1, min(i + 16, len(pcs))):
            _, op2, a2 = ins[pcs[j]]
            mm = re.fullmatch(r'(e[a-z]{2}), dword ptr \[(e[a-z]{2})\]', a2)
            if op2 == 'mov' and mm and regs.get(mm[2]) == 'ctx': regs[mm[1]] = 'obj'; continue
            if op2 == 'mov' and mm and regs.get(mm[2]) == 'obj': regs[mm[1]] = 'vt'; continue
            ms = re.fullmatch(r'dword ptr \[(?:(e[a-z]{2}))?(?: \+ )?(0x[0-9a-f]+)?\], (e[a-z]{2})', a2)
            if op2 == 'mov' and ms and regs.get(ms[3]) == 'obj':
                if ms[1] is None: fields[int(ms[2], 16)] = ver
                else: fillers.setdefault(function_start(i), {})[int(ms[2] or '0', 16)] = ver
                continue
            mc = re.fullmatch(r'dword ptr \[(e[a-z]{2})(?: \+ (0x[0-9a-f]+))?\]', a2)
            if op2 == 'call' and mc and regs.get(mc[1]) == 'vt':
                agg[(ver, int(mc[2] or '0', 16) // 4)] += 1; per_module[m['name']].add((ver, int(mc[2] or '0', 16) // 4)); break
            if op2 == 'call': break
    # contextes globaux : push G ; call filler
    for i, pc in enumerate(pcs):
        nxt, op, args = ins[pc]
        if op == 'call' and re.fullmatch(r'0x[0-9a-f]+', args) and int(args, 16) in fillers:
            prev = ins[pcs[i - 1]]
            if prev[1] == 'push' and re.fullmatch(r'0x[0-9a-f]+', prev[2]):
                g = int(prev[2], 16)
                for k, v in fillers[int(args, 16)].items(): fields[g + k] = v
    # pointeurs globaux vers un contexte : u32 en données égal à la base G d'un contexte
    groups = set()
    for g in {f - k for f in fields for k in range(0, 0x70, 4)}:
        if g in fields and all((g + k) in fields for k in (0, 4)): groups.add(g)
    pointers = {}
    for seg in m['loads']:
        if seg['flags'] & 1: continue
        for o in range(0, seg['filesz'] - 3, 4):
            v = struct.unpack_from('<I', data, seg['off'] + o)[0]
            if v in groups: pointers[seg['vaddr'] + o] = v
    for i, pc in enumerate(pcs):
        _, op, args = ins[pc]
        mp = re.fullmatch(r'(e[a-z]{2}), dword ptr \[(0x[0-9a-f]+)\]', args)
        if op != 'mov' or not mp or int(mp[2], 16) not in pointers: continue
        g = pointers[int(mp[2], 16)]; regs = {mp[1]: 'ctx'}; ver = None
        for j in range(i + 1, min(i + 16, len(pcs))):
            _, op2, a2 = ins[pcs[j]]
            mk = re.fullmatch(r'(e[a-z]{2}), dword ptr \[(e[a-z]{2})(?: \+ (0x[0-9a-f]+))?\]', a2)
            if op2 == 'mov' and mk and regs.get(mk[2]) == 'ctx' and (g + int(mk[3] or '0', 16)) in fields:
                ver = fields[g + int(mk[3] or '0', 16)]; regs[mk[1]] = 'obj'; continue
            if op2 == 'mov' and mk and regs.get(mk[2]) == 'obj' and not mk[3]: regs[mk[1]] = 'vt'; continue
            mc = re.fullmatch(r'dword ptr \[(e[a-z]{2})(?: \+ (0x[0-9a-f]+))?\]', a2)
            if op2 == 'call' and mc and regs.get(mc[1]) == 'vt' and ver:
                key = (ver, int(mc[2] or '0', 16) // 4); agg[key] += 1; per_module[m['name']].add(key); break
            if op2 in ('call', 'ret', 'jmp'): break
    # usages : mov r, [CHAMP] ; mov r2, [r] ; call [r2 + off]
    for i, pc in enumerate(pcs):
        _, op, args = ins[pc]
        mf = re.fullmatch(r'(e[a-z]{2}), dword ptr \[(0x[0-9a-f]+)\]', args)
        if op != 'mov' or not mf or int(mf[2], 16) not in fields: continue
        ver = fields[int(mf[2], 16)]; regs = {mf[1]: 'obj'}
        for j in range(i + 1, min(i + 14, len(pcs))):
            _, op2, a2 = ins[pcs[j]]
            mm = re.fullmatch(r'(e[a-z]{2}), dword ptr \[(e[a-z]{2})\]', a2)
            if op2 == 'mov' and mm and regs.get(mm[2]) == 'obj': regs[mm[1]] = 'vt'; continue
            mc = re.fullmatch(r'dword ptr \[(e[a-z]{2})(?: \+ (0x[0-9a-f]+))?\]', a2)
            if op2 == 'call' and mc and regs.get(mc[1]) == 'vt':
                key = (ver, int(mc[2] or '0', 16) // 4); agg[key] += 1; per_module[m['name']].add(key); break
            if op2 in ('call', 'ret', 'jmp'): break
    print(m['name'], len(fields), 'champs de contexte', len(per_module[m['name']]), 'couples', file=sys.stderr)
print(len(agg), 'couples distincts')
for (ver, slot), n in sorted(agg.items(), key=lambda x: (str(x[0][0]), x[0][1])):
    name, fn = table.get((ver, slot), (f'{ver}::slot{slot}', 'NULL'))
    print(f'{n:3} {name:72} {"OK" if fn != "NULL" else "PIEGE"}')
