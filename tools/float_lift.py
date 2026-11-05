"""SSE and x87 lowering for tools/lift.py: scalar/packed SSE on c->xmm, x87 modelled as a double stack."""
import re


def _lower_base(op, args, nxt, Operand, Unsupported):
    def xmm(text):
        m = re.fullmatch(r'xmm([0-7])', text)
        return int(m[1]) if m else None

    def operand(text):
        index = xmm(text)
        if index is not None: return f'c->xmm[{index}]'
        value = Operand(text)
        if value.width == 64: return f'(XMM){{.q={{{value.read()},0}}}}'
        if value.width == 32: return f'(XMM){{.d={{{value.read()},0,0,0}}}}'
        return value.read()

    def scalar(text, size):
        index = xmm(text)
        if index is not None:
            return f'c->xmm[{index}].{"g" if size == 64 else "f"}[0]'
        mem = Operand(text)
        if mem.addr is None or mem.width != size:
            raise Unsupported('scalar floating operand')
        return f'{"RD" if size == 64 else "RF"}({mem.addr})'

    def st(text):
        if text == 'st':
            return 'ST(0)'
        m = re.fullmatch(r'st\(([0-7])\)', text)
        if not m:
            raise Unsupported(f'x87 register {text!r}')
        return f'ST({m[1]})'

    parts = [x.strip() for x in args.split(',')] if args else []
    code = None
    if op in ('movd', 'movq') and len(parts) == 2 and any(xmm(p) is not None for p in parts):
        dst, src = parts; width = 32 if op == 'movd' else 64
        field = 'd' if width == 32 else 'q'
        value = f'c->xmm[{xmm(src)}].{field}[0]' if xmm(src) is not None else Operand(src).read()
        if xmm(dst) is not None:
            code = f'{{ uint64_t value = {value}; c->xmm[{xmm(dst)}] = (XMM){{0}}; c->xmm[{xmm(dst)}].{field}[0] = value; }}'
        else:
            code = Operand(dst).write(value)
    elif op in ('movaps', 'movups', 'movapd', 'movupd', 'movdqa', 'movdqu') and len(parts) == 2:
        dst, src = parts
        if xmm(dst) is not None:
            code = f'c->xmm[{xmm(dst)}] = {operand(src)};'
        else:
            mem = Operand(dst)
            if mem.width != 128 or xmm(src) is None:
                raise Unsupported('packed move operands')
            code = mem.write(operand(src))
        if op in ('movaps', 'movapd', 'movdqa'):
            for text in parts:
                if xmm(text) is None:
                    address = Operand(text).addr
                    code = f'if (({address}) & 15) rt_trap(c, c->eip, "unaligned SSE move"); ' + code
    elif op in ('movsd', 'movss') and len(parts) == 2 and any(xmm(p) is not None for p in parts):
        dst, src = parts; size = 64 if op == 'movsd' else 32
        field = 'q' if size == 64 else 'd'
        value = f'c->xmm[{xmm(src)}].{field}[0]' if xmm(src) is not None else Operand(src).read()
        if xmm(dst) is not None:
            code = f'{{ uint64_t value = {value}; '
            if xmm(src) is None:
                code += f'c->xmm[{xmm(dst)}] = (XMM){{0}}; '
            code += f'c->xmm[{xmm(dst)}].{field}[0] = value; }}'
        else:
            code = Operand(dst).write(value)
    elif op in ('pxor', 'xorps', 'xorpd', 'pand', 'pandn', 'por', 'andps', 'andpd', 'orps', 'orpd') and len(parts) == 2:
        dst, src = parts
        if xmm(dst) is None:
            raise Unsupported('packed destination')
        operator = '^' if op in ('pxor', 'xorps', 'xorpd') else '|' if op in ('por', 'orps', 'orpd') else '&'
        negate = '~' if op == 'pandn' else ''
        code = (f'{{ XMM lhs = {operand(dst)}, rhs = {operand(src)}; '
                f'for (unsigned lane = 0; lane < 2; ++lane) '
                f'c->xmm[{xmm(dst)}].q[lane] = {negate}lhs.q[lane] {operator} rhs.q[lane]; }}')
    elif op in ('paddq', 'psubq') and len(parts) == 2 and xmm(parts[0]) is not None:
        dst, src = parts
        operator = '+' if op == 'paddq' else '-'
        code = (f'{{ XMM lhs = {operand(dst)}, rhs = {operand(src)}; '
                f'for (unsigned lane = 0; lane < 2; ++lane) c->xmm[{xmm(dst)}].q[lane] = lhs.q[lane] {operator} rhs.q[lane]; }}')
    elif op in ('psrlw','psrld','psrlq','psllw','pslld','psllq','psraw','psrad') and len(parts)==2 and xmm(parts[0]) is not None:
        dst, source = parts
        width={'w':16,'d':32,'q':64}[op[-1]]; field={16:'w',32:'d',64:'q'}[width]
        try: immediate=int(source,0)
        except ValueError: immediate=None
        count=f'{immediate}u' if immediate is not None else f'({operand(source)}).q[0]'
        arithmetic=op.startswith('psra'); left=op.startswith('psll')
        shifted=f'(int{width}_t)value.{field}[lane] >> count' if arithmetic else f'value.{field}[lane] {"<<" if left else ">>"} count'
        saturated=f'0u - (uint{width}_t)((value.{field}[lane] >> {width-1}) & 1)' if arithmetic else '0'
        code=(f'{{ XMM value={operand(dst)}; uint64_t count={count}; '
              f'for(unsigned lane=0;lane<{128//width};++lane) c->xmm[{xmm(dst)}].{field}[lane] = '
              f'count >= {width} ? {saturated} : {shifted}; }}')
    elif op == 'punpckldq' and len(parts) == 2 and xmm(parts[0]) is not None:
        dst, src = parts
        code = (f'{{ XMM lhs = {operand(dst)}, rhs = {operand(src)}; '
                f'c->xmm[{xmm(dst)}] = (XMM){{.d = {{lhs.d[0], rhs.d[0], lhs.d[1], rhs.d[1]}}}}; }}')
    elif op in ('addpd','subpd','mulpd','divpd','addps','subps','mulps','divps','minpd','maxpd','minps','maxps') and len(parts)==2 and xmm(parts[0]) is not None:
        dst,source=parts; field='g' if op.endswith('pd') else 'f'; lanes=2 if field=='g' else 4
        if op[:3] in ('min','max'):
            comparison='<' if op.startswith('min') else '>'
            value=f'lhs.{field}[lane] {comparison} rhs.{field}[lane] ? lhs.{field}[lane] : rhs.{field}[lane]'
        else:
            operator={'add':'+','sub':'-','mul':'*','div':'/'}[op[:3]]
            value=f'lhs.{field}[lane] {operator} rhs.{field}[lane]'
        code=(f'{{ XMM lhs={operand(dst)}, rhs={operand(source)}; '
              f'for(unsigned lane=0;lane<{lanes};++lane) c->xmm[{xmm(dst)}].{field}[lane] = {value}; }}')
    elif re.fullmatch(r'cmp(eq|lt|le|unord|neq|nlt|nle|ord)(pd|ps|sd|ss)',op) and len(parts)==2 and xmm(parts[0]) is not None:
        dst,source=parts; match=re.fullmatch(r'cmp(eq|lt|le|unord|neq|nlt|nle|ord)(pd|ps|sd|ss)',op)
        predicate,suffix=match.groups(); wide=suffix in ('pd','sd'); scalar_op=suffix in ('sd','ss')
        field,mask=('g','q') if wide else ('f','d'); lanes=1 if scalar_op else 2 if wide else 4
        rhs=f'(XMM){{.{field}={{{scalar(source,64 if wide else 32)}}}}}' if scalar_op else operand(source)
        compare={'eq':'a==b','lt':'a<b','le':'a<=b','unord':'isnan(a)||isnan(b)',
                 'neq':'a!=b','nlt':'!(a<b)','nle':'!(a<=b)','ord':'!isnan(a)&&!isnan(b)'}[predicate]
        full='UINT64_MAX' if wide else 'UINT32_MAX'
        code=(f'{{ XMM lhs={operand(dst)}, rhs={rhs}; for(unsigned lane=0;lane<{lanes};++lane) {{ '
              f'double a=lhs.{field}[lane], b=rhs.{field}[lane]; c->xmm[{xmm(dst)}].{mask}[lane]=({compare}) ? {full} : 0; }} }}')
    elif op in ('cvttpd2dq','cvtpd2dq','cvttps2dq','cvtps2dq') and len(parts)==2 and xmm(parts[0]) is not None:
        dst,source=parts; field='g' if 'pd' in op else 'f'; lanes=2 if field=='g' else 4
        value=f'value.{field}[lane]' if op.startswith('cvtt') else f'rt_sse_round(c,value.{field}[lane])'
        code=(f'{{ XMM value={operand(source)}; c->xmm[{xmm(dst)}]=(XMM){{0}}; '
              f'for(unsigned lane=0;lane<{lanes};++lane) c->xmm[{xmm(dst)}].i[lane]=cvt_trunc32({value}); }}')
    elif op in ('cvtdq2pd','cvtdq2ps') and len(parts)==2 and xmm(parts[0]) is not None:
        dst,source=parts; field='g' if op.endswith('pd') else 'f'; lanes=2 if field=='g' else 4
        code=(f'{{ XMM value={operand(source)}; '
              f'for(unsigned lane=0;lane<{lanes};++lane) c->xmm[{xmm(dst)}].{field}[lane]=value.i[lane]; }}')
    elif op in ('addsd', 'subsd', 'mulsd', 'divsd', 'addss', 'subss', 'mulss', 'divss') and len(parts) == 2:
        dst, src = parts
        if xmm(dst) is None:
            raise Unsupported('scalar SSE destination')
        size = 64 if op.endswith('sd') else 32
        operator = {'add': '+', 'sub': '-', 'mul': '*', 'div': '/'}[op[:3]]
        code = f'{scalar(dst, size)} = {scalar(dst, size)} {operator} {scalar(src, size)};'
    elif op in ('ucomisd', 'comisd', 'ucomiss', 'comiss') and len(parts) == 2:
        size = 64 if op.endswith('sd') else 32
        code = f'f_comi(&f, {scalar(parts[0], size)}, {scalar(parts[1], size)});'
    elif op in ('cvtsi2sd', 'cvtsi2ss') and len(parts) == 2 and xmm(parts[0]) is not None:
        size = 64 if op.endswith('sd') else 32
        code = f'{scalar(parts[0], size)} = (int32_t){Operand(parts[1]).read()};'
    elif op in ('cvtss2sd', 'cvtsd2ss') and len(parts) == 2 and xmm(parts[0]) is not None:
        size = 64 if op.endswith('sd') else 32
        code = f'{scalar(parts[0], size)} = {scalar(parts[1], 96 - size)};'
    elif op in ('cvttsd2si', 'cvtsd2si', 'cvttss2si', 'cvtss2si') and len(parts) == 2:
        size = 64 if 'sd' in op else 32
        function = 'cvt_trunc32' if op.startswith('cvtt') else 'cvt_round32'
        code = Operand(parts[0]).write(f'{function}({scalar(parts[1], size)})')
    elif op in ('fld1', 'fldz', 'fldpi', 'fldln2', 'fldl2e') and not parts:
        value = {'fld1': '1.0', 'fldz': '0.0', 'fldpi': '3.14159265358979323846',
                 'fldln2': '0.69314718055994530942', 'fldl2e': '1.44269504088896340736'}[op]
        code = f'fpush(c, {value});'
    elif op in ('fld', 'fild') and len(parts) == 1:
        if parts[0].startswith('st'):
            value = st(parts[0])
        elif op == 'fild':
            mem = Operand(parts[0])
            value = f'(int{mem.width}_t){mem.read()}'
        else:
            mem = Operand(parts[0]); value = scalar(parts[0], mem.width)
        code = f'{{ double value = {value}; fpush(c, value); }}'
    elif op in ('fist', 'fistp', 'fisttp') and len(parts) == 1:
        mem = Operand(parts[0])
        if mem.addr is None or mem.width not in (16, 32, 64):
            raise Unsupported('x87 integer store format')
        value = 'ST(0)' if op == 'fisttp' else 'rt_x87_round(c, ST(0))'
        converter = 'cvt_trunc64' if mem.width == 64 else 'cvt_trunc32'
        code = mem.write(f'{converter}({value})')
        if op != 'fist': code += ' fpop(c);'
    elif op in ('fst', 'fstp') and len(parts) == 1:
        if parts[0].startswith('st'):
            code = f'{st(parts[0])} = ST(0);'
        else:
            mem = Operand(parts[0])
            if mem.width not in (32, 64) or mem.addr is None:
                raise Unsupported('x87 store format')
            code = f'{"WD" if mem.width == 64 else "WF"}({mem.addr}, ST(0));'
        if op == 'fstp': code += ' fpop(c);'
    elif op in ('fadd', 'fsub', 'fsubr', 'fmul', 'fdiv', 'fdivr',
                'faddp', 'fsubp', 'fsubrp', 'fmulp', 'fdivp', 'fdivrp'):
        pop = op.endswith('p'); operation = op[:-1] if pop else op
        if len(parts) == 2:
            dst, src = st(parts[0]), st(parts[1])
        elif len(parts) == 1:
            dst = 'ST(0)'
            if parts[0].startswith('st'): src = st(parts[0])
            else:
                mem = Operand(parts[0]); src = scalar(parts[0], mem.width)
        elif not parts and pop:
            dst, src = 'ST(1)', 'ST(0)'
        else:
            raise Unsupported('x87 arithmetic form')
        operator = {'fadd': '+', 'fsub': '-', 'fsubr': '-', 'fmul': '*', 'fdiv': '/', 'fdivr': '/'}[operation]
        lhs, rhs = (src, dst) if operation.endswith('r') else (dst, src)
        code = f'{dst} = {lhs} {operator} {rhs};'
        if pop: code += ' fpop(c);'
    elif op == 'fxch' and len(parts) <= 1:
        other = st(parts[0]) if parts else 'ST(1)'
        code = f'{{ double value = ST(0); ST(0) = {other}; {other} = value; }}'
    elif op in ('fchs', 'fabs', 'fsqrt') and not parts:
        value = {'fchs': '-ST(0)', 'fabs': 'fabs(ST(0))', 'fsqrt': 'sqrt(ST(0))'}[op]
        code = f'ST(0) = {value};'
    if code is None:
        return None
    return code + f' goto L{nxt:x};', [nxt]


# ---------------------------------------------------------------------------
# Extensions driven by tools/progress.py --all (see docs/lifter-coverage.md).
# MMX registers live in CPU.mm[]; they are not aliased onto the x87 stack
# (which is modelled in double precision), but MMX instructions still update
# the x87 tag word and top pointer as the hardware does.

LANE = {8: ('b', 'uint8_t', 'int8_t'), 16: ('w', 'uint16_t', 'int16_t'),
        32: ('d', 'uint32_t', 'int32_t'), 64: ('q', 'uint64_t', 'int64_t')}
SUFFIX_BITS = {'b': 8, 'w': 16, 'd': 32, 'q': 64}
FCMOV = {'fcmovb': 'B', 'fcmove': 'E', 'fcmovbe': 'BE', 'fcmovu': 'P',
         'fcmovnb': 'AE', 'fcmovne': 'NE', 'fcmovnbe': 'A', 'fcmovnu': 'NP'}


def lower(op, args, nxt, Operand, Unsupported):
    code = _lower_extended(op, args, Operand, Unsupported)
    if code is not None:
        return code + f' goto L{nxt:x};', [nxt]
    return _lower_base(op, args, nxt, Operand, Unsupported)


def _lower_extended(op, args, Operand, Unsupported):
    parts = [x.strip() for x in args.split(',')] if args else []

    def reg(text, kind):
        m = re.fullmatch(kind + r'([0-7])', text)
        return int(m[1]) if m else None

    def is_vec(text):
        return reg(text, 'xmm') is not None or reg(text, 'mm') is not None

    def vwidth(text):
        return 128 if reg(text, 'xmm') is not None else 64

    def vread(text, width=None):
        """Value of a vector operand as XMM; memory of `width` bits zero-extended."""
        if reg(text, 'xmm') is not None:
            return f'c->xmm[{reg(text, "xmm")}]'
        if reg(text, 'mm') is not None:
            return f'(XMM){{.q={{c->mm[{reg(text, "mm")}],0}}}}'
        mem = Operand(text)
        if mem.addr is None:
            raise Unsupported(f'vector operand {text!r}')
        bits = mem.width or width
        if bits == 128: return f'RX({mem.addr})'
        if bits == 64: return f'(XMM){{.q={{R64({mem.addr}),0}}}}'
        if bits == 32: return f'(XMM){{.d={{R32({mem.addr}),0,0,0}}}}'
        raise Unsupported(f'vector memory width {text!r}')

    def mmx_state(text):
        # Any MMX register access empties nothing but tags all x87 slots valid, TOP=0.
        return ' c->ftop = 0; c->ftags = 0xff;' if reg(text, 'mm') is not None else ''

    def vwrite(text, value):
        if reg(text, 'xmm') is not None:
            return f'c->xmm[{reg(text, "xmm")}] = {value};'
        if reg(text, 'mm') is not None:
            return f'c->mm[{reg(text, "mm")}] = ({value}).q[0];' + mmx_state(text)
        raise Unsupported(f'vector destination {text!r}')

    def aligned(text, code):
        if reg(text, 'xmm') is None and reg(text, 'mm') is None and '[' in text:
            mem = Operand(text)
            if mem.width == 128:
                return f'if (({mem.addr}) & 15) rt_trap(c, c->eip, "unaligned SSE operand"); ' + code
        return code

    def binary(dst, src, body):
        """dst = f(lhs, rhs), lanes computed into `out` (initialised to lhs)."""
        return aligned(src, f'{{ XMM lhs = {vread(dst)}, rhs = {vread(src, vwidth(dst))}, out = lhs; '
                            f'(void)rhs; {body} {vwrite(dst, "out")} }}')

    def lanes(dst, bits, expr):
        field, unsigned, _ = LANE[bits]
        count = vwidth(dst) // bits
        return (f'for (unsigned i = 0; i < {count}u; ++i) {{ {unsigned} x = lhs.{field}[i], y = rhs.{field}[i]; '
                f'(void)x; (void)y; out.{field}[i] = ({unsigned})({expr}); }}')

    def xmm_dst(text):
        if reg(text, 'xmm') is None:
            raise Unsupported(f'SSE destination {text!r}')
        return reg(text, 'xmm')

    def scalar_src(text, bits):
        if reg(text, 'xmm') is not None:
            return f'c->xmm[{reg(text, "xmm")}].{"g" if bits == 64 else "f"}[0]'
        mem = Operand(text)
        if mem.addr is None or mem.width != bits:
            raise Unsupported('scalar floating operand')
        return f'{"RD" if bits == 64 else "RF"}({mem.addr})'

    def st(text):
        if text == 'st':
            return 'ST(0)'
        m = re.fullmatch(r'st\(([0-7])\)', text)
        if not m:
            raise Unsupported(f'x87 register {text!r}')
        return f'ST({m[1]})'

    two = len(parts) == 2
    # --- MMX / SSE2 integer ---------------------------------------------------
    if op == 'emms' and not parts:
        return 'c->ftags = 0;'
    if op in ('movq', 'movd') and two and any(reg(p, 'mm') is not None for p in parts):
        dst, src = parts
        if reg(dst, 'mm') is not None:
            if reg(src, 'mm') is not None: value = f'c->mm[{reg(src, "mm")}]'
            elif reg(src, 'xmm') is not None: value = f'c->xmm[{reg(src, "xmm")}].q[0]'
            else: value = f'(uint64_t){Operand(src).read()}'
            if op == 'movd' and reg(src, 'mm') is None and reg(src, 'xmm') is None:
                value = f'(uint64_t)(uint32_t){Operand(src).read()}'
            return f'c->mm[{reg(dst, "mm")}] = {value};' + mmx_state(dst)
        value = f'c->mm[{reg(src, "mm")}]'
        if reg(dst, 'xmm') is not None:
            return f'c->xmm[{reg(dst, "xmm")}] = (XMM){{.q={{{value},0}}}};' + mmx_state(src)
        if op == 'movd': value = f'(uint32_t){value}'
        return Operand(dst).write(value) + mmx_state(src)
    if op == 'movdq2q' and two:
        return f'c->mm[{reg(parts[0], "mm")}] = c->xmm[{xmm_dst(parts[1])}].q[0];' + mmx_state(parts[0])
    if op == 'movq2dq' and two:
        return f'c->xmm[{xmm_dst(parts[0])}] = (XMM){{.q={{c->mm[{reg(parts[1], "mm")}],0}}}};' + mmx_state(parts[1])
    m = re.fullmatch(r'p(add|sub)([bwdq])', op)
    if m and two and is_vec(parts[0]):
        return binary(parts[0], parts[1], lanes(parts[0], SUFFIX_BITS[m[2]], 'x + y' if m[1] == 'add' else 'x - y'))
    m = re.fullmatch(r'p(add|sub)(u?)s([bw])', op)
    if m and two and is_vec(parts[0]):
        bits = SUFFIX_BITS[m[3]]; _, unsigned, signed = LANE[bits]
        sign = '+' if m[1] == 'add' else '-'
        if m[2]:
            expr = f'({{ int v = (int)x {sign} (int)y; v < 0 ? 0 : v > {(1 << bits) - 1} ? {(1 << bits) - 1} : v; }})'
        else:
            lo, hi = -(1 << (bits - 1)), (1 << (bits - 1)) - 1
            expr = f'({{ int v = (int)({signed})x {sign} (int)({signed})y; v < {lo} ? {lo} : v > {hi} ? {hi} : v; }})'
        return binary(parts[0], parts[1], lanes(parts[0], bits, expr))
    if op in ('pmullw', 'pmulhw', 'pmulhuw') and two and is_vec(parts[0]):
        expr = {'pmullw': '(int32_t)(int16_t)x * (int16_t)y',
                'pmulhw': '((int32_t)(int16_t)x * (int16_t)y) >> 16',
                'pmulhuw': '((uint32_t)x * y) >> 16'}[op]
        return binary(parts[0], parts[1], lanes(parts[0], 16, expr))
    if op == 'pmaddwd' and two and is_vec(parts[0]):
        count = vwidth(parts[0]) // 32
        return binary(parts[0], parts[1],
                      f'for (unsigned i = 0; i < {count}u; ++i) out.d[i] = (uint32_t)((int64_t)(int16_t)lhs.w[2*i] * (int16_t)rhs.w[2*i] + '
                      f'(int64_t)(int16_t)lhs.w[2*i+1] * (int16_t)rhs.w[2*i+1]);')
    if op == 'pmuludq' and two and is_vec(parts[0]):
        count = vwidth(parts[0]) // 64
        return binary(parts[0], parts[1],
                      f'for (unsigned i = 0; i < {count}u; ++i) out.q[i] = (uint64_t)lhs.d[2*i] * rhs.d[2*i];')
    m = re.fullmatch(r'pcmp(eq|gt)([bwd])', op)
    if m and two and is_vec(parts[0]):
        bits = SUFFIX_BITS[m[2]]; _, unsigned, signed = LANE[bits]
        test = 'x == y' if m[1] == 'eq' else f'({signed})x > ({signed})y'
        return binary(parts[0], parts[1], lanes(parts[0], bits, f'({test}) ? ({unsigned})~({unsigned})0 : 0'))
    if op in ('pminub', 'pmaxub', 'pminsw', 'pmaxsw', 'pavgb', 'pavgw') and two and is_vec(parts[0]):
        bits = 8 if op[-1] == 'b' else 16; _, unsigned, signed = LANE[bits]
        expr = {'pminub': 'x < y ? x : y', 'pmaxub': 'x > y ? x : y',
                'pminsw': f'({signed})x < ({signed})y ? x : y', 'pmaxsw': f'({signed})x > ({signed})y ? x : y',
                'pavgb': '((uint32_t)x + y + 1) >> 1', 'pavgw': '((uint32_t)x + y + 1) >> 1'}[op]
        return binary(parts[0], parts[1], lanes(parts[0], bits, expr))
    if op in ('pand', 'pandn', 'por', 'pxor', 'andnps', 'andnpd') and two and is_vec(parts[0]):
        expr = {'pand': 'x & y', 'pandn': '~x & y', 'andnps': '~x & y', 'andnpd': '~x & y',
                'por': 'x | y', 'pxor': 'x ^ y'}[op]
        if parts[0] == parts[1] and op in ('pxor',):
            return vwrite(parts[0], '(XMM){0}')
        return binary(parts[0], parts[1], lanes(parts[0], 64, expr))
    m = re.fullmatch(r'pack(ss|us)(wb|dw)', op)
    if m and two and is_vec(parts[0]):
        src_bits = 16 if m[2] == 'wb' else 32; dst_bits = src_bits // 2
        sfield, _, stype = LANE[src_bits]; dfield, dtype, _ = LANE[dst_bits]
        lo, hi = ((-(1 << (dst_bits - 1)), (1 << (dst_bits - 1)) - 1) if m[1] == 'ss' else (0, (1 << dst_bits) - 1))
        count = vwidth(parts[0]) // src_bits
        return binary(parts[0], parts[1],
                      f'for (unsigned i = 0; i < {2 * count}u; ++i) {{ int64_t v = ({stype})(i < {count}u ? lhs.{sfield}[i] : rhs.{sfield}[i - {count}u]); '
                      f'out.{dfield}[i] = ({dtype})(v < {lo} ? {lo} : v > {hi} ? {hi} : v); }}')
    m = re.fullmatch(r'punpck([lh])(bw|wd|dq|qdq)', op)
    if m and two and is_vec(parts[0]):
        bits = {'bw': 8, 'wd': 16, 'dq': 32, 'qdq': 64}[m[2]]; field = LANE[bits][0]
        half = vwidth(parts[0]) // bits // 2
        offset = 0 if m[1] == 'l' else half
        return binary(parts[0], parts[1],
                      f'for (unsigned i = 0; i < {half}u; ++i) {{ out.{field}[2*i] = lhs.{field}[i + {offset}u]; '
                      f'out.{field}[2*i+1] = rhs.{field}[i + {offset}u]; }}')
    if op in ('pshufd', 'pshuflw', 'pshufhw', 'pshufw') and len(parts) == 3:
        dst, src, imm = parts; imm = int(imm, 0) & 0xff
        value = vread(src, vwidth(dst))
        if op == 'pshufd':
            body = ', '.join(f'v.d[{(imm >> (2 * i)) & 3}]' for i in range(4))
            return aligned(src, f'{{ XMM v = {value}; {vwrite(dst, "(XMM){.d={" + body + "}}")} }}')
        base = 4 if op == 'pshufhw' else 0
        lanes4 = ', '.join(f'v.w[{base + ((imm >> (2 * i)) & 3)}]' for i in range(4))
        if op == 'pshufw':
            return f'{{ XMM v = {value}; {vwrite(dst, "(XMM){.w={" + lanes4 + "}}")} }}'
        keep = ', '.join(f'v.w[{i}]' for i in (range(4, 8) if op == 'pshuflw' else range(0, 4)))
        words = f'{lanes4}, {keep}' if op == 'pshuflw' else f'{keep}, {lanes4}'
        return aligned(src, f'{{ XMM v = {value}; {vwrite(dst, "(XMM){.w={" + words + "}}")} }}')
    if op in ('psrldq', 'pslldq') and two and reg(parts[0], 'xmm') is not None:
        count = int(parts[1], 0) & 0xff
        dst = reg(parts[0], 'xmm')
        if count > 15:
            return f'c->xmm[{dst}] = (XMM){{0}};'
        move = (f'out.b[i] = i + {count}u < 16u ? v.b[i + {count}u] : 0;' if op == 'psrldq'
                else f'out.b[i] = i >= {count}u ? v.b[i - {count}u] : 0;')
        return f'{{ XMM v = c->xmm[{dst}], out; for (unsigned i = 0; i < 16u; ++i) {move} c->xmm[{dst}] = out; }}'
    if op in ('psrlw', 'psrld', 'psrlq', 'psllw', 'pslld', 'psllq', 'psraw', 'psrad') and two and reg(parts[0], 'mm') is not None:
        bits = SUFFIX_BITS[op[-1]]; field, unsigned, signed = LANE[bits]
        try:
            count = f'{int(parts[1], 0) & 0xff}u'
        except ValueError:
            count = f'({vread(parts[1], 64)}).q[0]'
        arithmetic = op.startswith('psra'); left = op.startswith('psll')
        shifted = (f'({unsigned})(({signed})v.{field}[i] >> (count >= {bits} ? {bits - 1} : count))' if arithmetic
                   else f'count >= {bits} ? 0 : ({unsigned})(v.{field}[i] {"<<" if left else ">>"} count)')
        return (f'{{ XMM v = {vread(parts[0])}, out = v; uint64_t count = {count}; '
                f'for (unsigned i = 0; i < {64 // bits}u; ++i) out.{field}[i] = {shifted}; {vwrite(parts[0], "out")} }}')
    if op in ('pmovmskb', 'movmskps', 'movmskpd') and two and is_vec(parts[1]):
        bits = {'pmovmskb': 8, 'movmskps': 32, 'movmskpd': 64}[op]; field = LANE[bits][0]
        count = vwidth(parts[1]) // bits
        return (f'{{ XMM v = {vread(parts[1])}; uint32_t mask = 0; for (unsigned i = 0; i < {count}u; ++i) '
                f'mask |= (uint32_t)(v.{field}[i] >> {bits - 1}) << i; {Operand(parts[0]).write("mask")} }}')
    if op == 'cvtpi2ps' and two:
        dst = xmm_dst(parts[0])
        return (f'{{ XMM v = {vread(parts[1], 64)}; c->xmm[{dst}].f[0] = (float)v.i[0]; '
                f'c->xmm[{dst}].f[1] = (float)v.i[1]; }}' + mmx_state(parts[1]))
    if op in ('cvtps2pi', 'cvttps2pi') and two and reg(parts[0], 'mm') is not None:
        # Deux flottants bas vers deux entiers (MXCSR ou troncature) ; hors
        # plage ou NaN : entier indéfini 0x80000000.
        value = 'v.f[i]' if op == 'cvttps2pi' else 'rt_sse_round(c, v.f[i])'
        return (f'{{ XMM v = {vread(parts[1], 64)}, out = {{0}}; for (unsigned i = 0; i < 2u; ++i) '
                f'out.i[i] = cvt_trunc32({value}); {vwrite(parts[0], "out")} }}')
    if op in ('cvtps2pd', 'cvtpd2ps') and two:
        dst = xmm_dst(parts[0])
        if op == 'cvtps2pd':
            return f'{{ XMM v = {vread(parts[1], 64)}; c->xmm[{dst}] = (XMM){{.g={{v.f[0], v.f[1]}}}}; }}'
        return aligned(parts[1], f'{{ XMM v = {vread(parts[1])}; c->xmm[{dst}] = (XMM){{.f={{(float)v.g[0], (float)v.g[1], 0, 0}}}}; }}')
    # --- SSE packed floating point -------------------------------------------
    if op in ('shufps', 'shufpd') and len(parts) == 3:
        dst, src, imm = parts; imm = int(imm, 0) & 0xff
        if op == 'shufps':
            body = (f'l.f[{imm & 3}], l.f[{(imm >> 2) & 3}], r.f[{(imm >> 4) & 3}], r.f[{(imm >> 6) & 3}]')
            value = f'(XMM){{.f={{{body}}}}}'
        else:
            value = f'(XMM){{.g={{l.g[{imm & 1}], r.g[{(imm >> 1) & 1}]}}}}'
        return aligned(src, f'{{ XMM l = {vread(dst)}, r = {vread(src)}; c->xmm[{xmm_dst(dst)}] = {value}; }}')
    if op in ('unpcklps', 'unpckhps', 'unpcklpd', 'unpckhpd') and two:
        dst, src = parts
        if op.endswith('ps'):
            o = 0 if op == 'unpcklps' else 2
            value = f'(XMM){{.d={{l.d[{o}], r.d[{o}], l.d[{o + 1}], r.d[{o + 1}]}}}}'
        else:
            o = 0 if op == 'unpcklpd' else 1
            value = f'(XMM){{.q={{l.q[{o}], r.q[{o}]}}}}'
        return aligned(src, f'{{ XMM l = {vread(dst)}, r = {vread(src)}; c->xmm[{xmm_dst(dst)}] = {value}; }}')
    if op in ('movhps', 'movlps', 'movhpd', 'movlpd') and two:
        dst, src = parts; lane = 1 if op in ('movhps', 'movhpd') else 0
        if reg(dst, 'xmm') is not None:
            mem = Operand(src)
            if mem.addr is None: raise Unsupported(f'{op} needs memory')
            return f'c->xmm[{reg(dst, "xmm")}].q[{lane}] = R64({mem.addr});'
        mem = Operand(dst)
        if mem.addr is None: raise Unsupported(f'{op} needs memory')
        return f'W64({mem.addr}, c->xmm[{xmm_dst(src)}].q[{lane}]);'
    if op in ('movlhps', 'movhlps') and two:
        dst, src = xmm_dst(parts[0]), xmm_dst(parts[1])
        if op == 'movlhps':
            return f'c->xmm[{dst}].q[1] = c->xmm[{src}].q[0];'
        return f'c->xmm[{dst}].q[0] = c->xmm[{src}].q[1];'
    if op in ('movntps', 'movntpd', 'movntdq') and two:
        mem = Operand(parts[0])
        if mem.addr is None or mem.width != 128: raise Unsupported('non-temporal store operand')
        return (f'if (({mem.addr}) & 15) rt_trap(c, c->eip, "unaligned SSE operand"); '
                f'WX({mem.addr}, c->xmm[{xmm_dst(parts[1])}]);')
    if op == 'movnti' and two:
        return Operand(parts[0]).write(Operand(parts[1]).read())
    if op in ('sqrtps', 'sqrtpd', 'rcpps', 'rsqrtps') and two:
        dst, src = parts
        field, count = ('g', 2) if op.endswith('pd') else ('f', 4)
        expr = {'sqrtps': 'sqrtf(v.f[i])', 'sqrtpd': 'sqrt(v.g[i])',
                'rcpps': '1.0f / v.f[i]', 'rsqrtps': '1.0f / sqrtf(v.f[i])'}[op]
        return aligned(src, f'{{ XMM v = {vread(src)}; for (unsigned i = 0; i < {count}u; ++i) '
                            f'c->xmm[{xmm_dst(dst)}].{field}[i] = {expr}; }}')
    if op in ('sqrtss', 'sqrtsd', 'rcpss', 'rsqrtss') and two:
        bits = 64 if op == 'sqrtsd' else 32
        value = scalar_src(parts[1], bits)
        expr = {'sqrtss': f'sqrtf({value})', 'sqrtsd': f'sqrt({value})',
                'rcpss': f'1.0f / {value}', 'rsqrtss': f'1.0f / sqrtf({value})'}[op]
        return f'c->xmm[{xmm_dst(parts[0])}].{"g" if bits == 64 else "f"}[0] = {expr};'
    if op in ('minss', 'maxss', 'minsd', 'maxsd') and two:
        bits = 64 if op.endswith('sd') else 32
        lhs = f'c->xmm[{xmm_dst(parts[0])}].{"g" if bits == 64 else "f"}[0]'
        kind = 'double' if bits == 64 else 'float'
        compare = '<' if op.startswith('min') else '>'
        # x86: the second operand is returned when either is NaN or both are zero.
        return f'{{ {kind} x = {lhs}, y = {scalar_src(parts[1], bits)}; {lhs} = x {compare} y ? x : y; }}'
    if op in ('andps', 'andpd', 'orps', 'orpd', 'xorps', 'xorpd') and two and parts[0] == parts[1] and op.startswith('xor'):
        return f'c->xmm[{xmm_dst(parts[0])}] = (XMM){{0}};'
    # --- x87 ------------------------------------------------------------------
    if op in ('fcomi', 'fucomi', 'fcompi', 'fucompi', 'fcomip', 'fucomip') and len(parts) in (1, 2):
        other = st(parts[-1]) if parts[-1] != 'st' or len(parts) == 1 else 'ST(1)'
        code = f'f_comi(&f, ST(0), {other});'
        return code + (' fpop(c);' if op.endswith('pi') or op.endswith('ip') else '')
    if op in ('fcom', 'fcomp', 'fucom', 'fucomp', 'fcompp', 'fucompp', 'ftst'):
        # Comparaison x87 vers le mot d'état : C3 (0x4000), C2 (0x400), C0 (0x100) ;
        # C1 effacé. Lue ensuite par fnstsw (+ sahf ou test ah).
        if op == 'ftst' and not parts:
            other = '0.0'
        elif op.endswith('pp') and not parts:
            other = 'ST(1)'
        elif op not in ('ftst', 'fcompp', 'fucompp') and len(parts) <= 1:
            if not parts:
                other = 'ST(1)'
            elif parts[0].startswith('st'):
                other = st(parts[0])
            else:
                mem = Operand(parts[0])
                if mem.addr is None or mem.width not in (32, 64):
                    raise Unsupported('x87 compare operand')
                other = f'(double){"RD" if mem.width == 64 else "RF"}({mem.addr})'
        else:
            raise Unsupported(f'x87 compare form {op} {args}')
        pops = 2 if op.endswith('pp') else 1 if op.endswith('p') else 0
        return (f'{{ double x = ST(0), y = {other}; c->fsw = (uint16_t)((c->fsw & ~0x4700u) | '
                '(x > y ? 0u : x < y ? 0x100u : x == y ? 0x4000u : 0x4500u)); }' + ' fpop(c);' * pops)
    if op in FCMOV and len(parts) == 2:
        return f'if (cond(&f, CC_{FCMOV[op]})) ST(0) = {st(parts[1])};'
    if op in ('fldcw',) and len(parts) == 1:
        return f'c->fcw = (uint16_t){Operand(parts[0]).read()};'
    if op in ('fnstcw', 'fstcw') and len(parts) == 1:
        return Operand(parts[0]).write('c->fcw')
    if op in ('fnstsw', 'fstsw') and len(parts) == 1:
        return Operand(parts[0]).write('(uint16_t)((c->fsw & ~0x3800u) | ((c->ftop & 7u) << 11))')
    if op in ('fiadd', 'fimul', 'fisub', 'fisubr', 'fidiv', 'fidivr') and len(parts) == 1:
        mem = Operand(parts[0])
        if mem.addr is None or mem.width not in (16, 32):
            raise Unsupported('x87 integer operand')
        value = f'(double)(int{mem.width}_t){mem.read()}'
        expr = {'fiadd': f'ST(0) + {value}', 'fimul': f'ST(0) * {value}', 'fisub': f'ST(0) - {value}',
                'fisubr': f'{value} - ST(0)', 'fidiv': f'ST(0) / {value}', 'fidivr': f'{value} / ST(0)'}[op]
        return f'ST(0) = {expr};'
    if op == 'frndint' and not parts:
        return 'ST(0) = rt_x87_round(c, ST(0));'
    if op in ('fsin', 'fcos', 'fptan', 'fsincos') and not parts:
        # Out of range (|x| >= 2^63): C2 set, operand unchanged.
        body = {'fsin': 'ST(0) = sin(ST(0));', 'fcos': 'ST(0) = cos(ST(0));',
                'fptan': 'ST(0) = tan(ST(0)); fpush(c, 1.0);',
                'fsincos': '{ double x = ST(0); ST(0) = sin(x); fpush(c, cos(x)); }'}[op]
        return (f'if (fabs(ST(0)) >= 9223372036854775808.0) c->fsw |= 0x400u; '
                f'else {{ c->fsw &= ~0x400u; {body} }}')
    if op == 'fpatan' and not parts:
        return 'ST(1) = atan2(ST(1), ST(0)); fpop(c);'
    if op == 'fyl2x' and not parts:
        return 'ST(1) = ST(1) * log2(ST(0)); fpop(c);'
    if op == 'fyl2xp1' and not parts:
        return 'ST(1) = ST(1) * (log1p(ST(0)) / 0.69314718055994530942); fpop(c);'
    if op == 'f2xm1' and not parts:
        return 'ST(0) = exp2(ST(0)) - 1.0;'
    if op == 'fscale' and not parts:
        return 'ST(0) = ldexp(ST(0), (int)fmax(fmin(trunc(ST(1)), 100000.0), -100000.0));'
    if op in ('fprem', 'fprem1') and not parts:
        return f'rt_fprem(c, {int(op == "fprem1")});'
    if op in ('fldlg2', 'fldl2t') and not parts:
        return 'fpush(c, ' + ('0.30102999566398119521' if op == 'fldlg2' else '3.32192809488736234787') + ');'
    if op in ('fld', 'fstp') and len(parts) == 1 and parts[0].startswith('tbyte ptr'):
        mem = Operand(parts[0].replace('tbyte ptr ', '', 1))
        if op == 'fld':
            return f'fpush(c, rt_load_extended({mem.addr}));'
        return f'rt_store_extended({mem.addr}, ST(0)); fpop(c);'
    if op in ('fninit', 'finit') and not parts:
        return 'c->fcw = 0x037f; c->fsw = 0; c->ftags = 0; c->ftop = 0;'
    if op in ('fnclex', 'fclex') and not parts:
        return 'c->fsw &= 0x7f00u;'
    if op in ('fwait', 'wait') and not parts:
        return ''
    return None
