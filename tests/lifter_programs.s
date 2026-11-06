# Real i386 code for tests/checks_lifter.c: assembled by clang, disassembled by
# objdump and lifted by tools/lift.py exactly like the game modules.
# Inputs live at IN = 0x20018000, outputs at OUT = 0x20019000.
.intel_syntax noprefix
.text
.globl lf_bits
lf_bits:                                   # bt/bts/btr/btc, register and bit-string forms
    mov ecx, 0x80000001
    mov edx, 31
    bt ecx, edx
    setc byte ptr [0x20019000]
    mov edx, 33                            # register form: index modulo 32 -> bit 1
    bt ecx, edx
    setc byte ptr [0x20019001]
    bts ecx, edx
    mov dword ptr [0x20019004], ecx
    mov edx, 0
    btr ecx, edx
    setc byte ptr [0x20019002]
    mov dword ptr [0x20019008], ecx
    mov edx, 4
    btc ecx, edx
    mov dword ptr [0x2001900c], ecx
    mov eax, 0x20018010
    mov edx, -1                            # bit string: bit 31 of the dword before
    bts dword ptr [eax], edx
    setc byte ptr [0x20019003]
    mov edx, 40                            # dword after, bit 8
    btr dword ptr [eax], edx
    setc byte ptr [0x20019010]
    bt dword ptr [eax], 35                 # immediate form: modulo 32 -> bit 3
    setc byte ptr [0x20019011]
    ret

.globl lf_narrow
lf_narrow:                                 # 8/16-bit multiply/divide, cbw/cwd, sahf/lahf
    mov eax, 0xaaaa00f0
    mov dl, 0x10
    mul dl                                 # ax = 0x0f00, CF=OF=1
    setc byte ptr [0x20019020]
    mov dword ptr [0x20019024], eax
    mov eax, 0x000000fe                    # al = -2
    mov cl, 0x03
    imul cl                                # ax = -6, fits in al: CF=0
    setc byte ptr [0x20019021]
    mov dword ptr [0x20019028], eax
    mov eax, 0x11110064                    # ax = 100
    mov cl, 7
    div cl                                 # al = 14, ah = 2
    mov dword ptr [0x2001902c], eax
    mov eax, 0xffff0000 + 0xfff9           # ax = -7
    cwd                                    # dx = 0xffff
    mov ecx, 2
    idiv cx                                # ax = -3, dx = -1
    mov dword ptr [0x20019030], eax
    mov dword ptr [0x20019034], edx
    mov eax, 0x12345680
    cbw                                    # ax = 0xff80
    mov dword ptr [0x20019038], eax
    mov eax, 0x0000c500                    # ah: SF ZF PF CF set (0xc5)
    sahf
    sets byte ptr [0x2001903c]
    setz byte ptr [0x2001903d]
    setp byte ptr [0x2001903e]
    setc byte ptr [0x2001903f]
    mov eax, 0
    cmp eax, 1                             # CF=1 SF=1 AF=1 PF=1 (0xff has even parity)
    lahf
    mov dword ptr [0x20019040], eax
    rep ret

.globl lf_sse
lf_sse:                                    # packed single shuffles and masks
    movaps xmm0, xmmword ptr [0x20018100]  # 1 2 3 4
    movaps xmm1, xmmword ptr [0x20018110]  # 5 6 7 8
    movaps xmm2, xmm0
    shufps xmm2, xmm1, 0x1b                # 4 3 6 5
    movups xmmword ptr [0x20019100], xmm2
    movaps xmm2, xmm0
    unpcklps xmm2, xmm1                    # 1 5 2 6
    movups xmmword ptr [0x20019110], xmm2
    movaps xmm2, xmm0
    unpckhps xmm2, xmm1                    # 3 7 4 8
    movups xmmword ptr [0x20019120], xmm2
    movaps xmm2, xmm0
    movlhps xmm2, xmm1                     # 1 2 5 6
    movhlps xmm2, xmm1                     # 7 8 5 6
    movups xmmword ptr [0x20019130], xmm2
    movaps xmm2, xmm0
    movhps xmm2, qword ptr [0x20018110]    # 1 2 5 6
    movlps xmm2, qword ptr [0x20018118]    # 7 8 5 6
    movhps qword ptr [0x20019140], xmm2    # 5 6
    movlps qword ptr [0x20019148], xmm2    # 7 8
    movaps xmm3, xmmword ptr [0x20018120]  # -1 2 -3 0
    movmskps eax, xmm3
    mov dword ptr [0x20019150], eax
    andnps xmm3, xmm0                      # ~mask & value
    movups xmmword ptr [0x20019160], xmm3
    movntps xmmword ptr [0x20019170], xmm1
    ret

.globl lf_sse_math
lf_sse_math:                               # scalar/packed sqrt, min/max NaN rules, reciprocals
    movss xmm0, dword ptr [0x20018200]     # 16
    sqrtss xmm0, xmm0
    movss dword ptr [0x20019200], xmm0
    movsd xmm1, qword ptr [0x20018208]     # 2.25
    sqrtsd xmm1, xmm1
    movsd qword ptr [0x20019208], xmm1
    movss xmm2, dword ptr [0x20018210]     # NaN
    movss xmm3, dword ptr [0x20018214]     # 3
    minss xmm2, xmm3                       # NaN first -> second operand (3)
    movss dword ptr [0x20019210], xmm2
    movss xmm2, dword ptr [0x20018214]     # 3
    maxss xmm2, dword ptr [0x20018210]     # NaN second -> NaN
    movss dword ptr [0x20019214], xmm2
    movsd xmm4, qword ptr [0x20018218]     # -0.0
    minsd xmm4, qword ptr [0x20018220]     # +0.0: equal -> second (+0.0)
    movsd qword ptr [0x20019218], xmm4
    movaps xmm5, xmmword ptr [0x20018230]  # 4 16 0.25 1
    sqrtps xmm6, xmm5
    movups xmmword ptr [0x20019220], xmm6
    rcpps xmm6, xmm5
    movups xmmword ptr [0x20019230], xmm6
    rsqrtps xmm6, xmm5
    movups xmmword ptr [0x20019240], xmm6
    ret

.globl lf_int
lf_int:                                    # SSE2 integer lanes
    movdqa xmm0, xmmword ptr [0x20018300]
    movdqa xmm1, xmmword ptr [0x20018310]
    pshufd xmm2, xmm0, 0x1b
    movdqu xmmword ptr [0x20019300], xmm2
    movdqa xmm2, xmm0
    punpcklwd xmm2, xmm1
    movdqu xmmword ptr [0x20019310], xmm2
    movdqa xmm2, xmm0
    paddd xmm2, xmm1
    movdqu xmmword ptr [0x20019320], xmm2
    movdqa xmm2, xmm0
    pcmpeqd xmm2, xmm1
    movdqu xmmword ptr [0x20019330], xmm2
    movdqa xmm2, xmm0
    pcmpgtw xmm2, xmm1
    movdqu xmmword ptr [0x20019340], xmm2
    movdqa xmm2, xmm0
    pmuludq xmm2, xmm1
    movdqu xmmword ptr [0x20019350], xmm2
    movdqa xmm2, xmm0
    packssdw xmm2, xmm1
    movdqu xmmword ptr [0x20019360], xmm2
    movdqa xmm2, xmm0
    psrldq xmm2, 5
    movdqu xmmword ptr [0x20019370], xmm2
    movdqa xmm2, xmm0
    pmullw xmm2, xmm1
    movdqu xmmword ptr [0x20019380], xmm2
    movdqa xmm2, xmm0
    pmulhw xmm2, xmm1
    movdqu xmmword ptr [0x20019390], xmm2
    pcmpeqd xmm3, xmm3
    movdqu xmmword ptr [0x200193a0], xmm3
    ret

.globl lf_mmx
lf_mmx:
    mov eax, 0x7fff8000
    movd mm0, eax
    movq mm1, qword ptr [0x20018400]
    paddw mm0, mm1
    movq qword ptr [0x20019400], mm0
    psrad mm1, qword ptr [0x20018408]
    movq qword ptr [0x20019408], mm1
    packssdw mm1, mm0
    movq qword ptr [0x20019410], mm1
    movd ecx, mm1
    mov dword ptr [0x20019418], ecx
    movq2dq xmm0, mm0
    movdqu xmmword ptr [0x20019420], xmm0
    cvtpi2ps xmm1, mm0
    movups xmmword ptr [0x20019430], xmm1
    emms
    ret

.globl lf_x87_compare
lf_x87_compare:                            # fcomi/fucomip/fcmov leave ZF PF CF in EFLAGS
    fld qword ptr [0x20018500]             # 1
    fld qword ptr [0x20018508]             # 2
    fcomi st, st(1)                        # 2 > 1: ZF=PF=CF=0
    seta byte ptr [0x20019500]
    setp byte ptr [0x20019501]
    fcmovnbe st, st(1)                     # condition true: st0 = 1
    fstp qword ptr [0x20019508]
    fld qword ptr [0x20018510]             # NaN
    fucomip st, st(1)                      # unordered: ZF=PF=CF=1, pops
    setp byte ptr [0x20019502]
    sete byte ptr [0x20019503]
    setb byte ptr [0x20019504]
    fld qword ptr [0x20018508]             # 2
    fcomi st, st(1)
    fcmovb st, st(1)                       # false: keeps 2
    fstp qword ptr [0x20019510]
    fstp st(0)
    ret

.globl lf_x87_math
lf_x87_math:
    fnstcw word ptr [0x20019600]
    mov word ptr [0x20019602], 0x0f7f      # round toward zero
    fldcw word ptr [0x20019602]
    fld qword ptr [0x20018600]             # -2.75
    frndint
    fstp qword ptr [0x20019608]            # -2
    fldcw word ptr [0x20019600]
    fld qword ptr [0x20018600]
    frndint                                # nearest: -3
    fimul dword ptr [0x20018608]           # * 5 -> -15
    fidivr dword ptr [0x20018608]          # 5 / -15
    fstp qword ptr [0x20019610]
    fld qword ptr [0x20018610]             # 1
    fld qword ptr [0x20018610]             # 1
    fpatan                                 # atan2(1, 1)
    fstp qword ptr [0x20019618]
    fld qword ptr [0x20018610]             # y = 1
    fld qword ptr [0x20018618]             # x = 8
    fyl2x                                  # 1 * log2(8)
    fstp qword ptr [0x20019620]
    fld qword ptr [0x20018620]             # 0.5
    fsin
    fstp qword ptr [0x20019628]
    fld qword ptr [0x20018620]
    fptan                                  # pushes 1.0 above tan(0.5)
    fstp qword ptr [0x20019630]
    fstp qword ptr [0x20019638]
    fld qword ptr [0x20018628]             # divisor 3
    fld qword ptr [0x20018630]             # dividend -11
1:  fprem
    fnstsw ax
    sahf
    jp 1b
    fstp qword ptr [0x20019640]            # -2
    fstp st(0)
    mov dword ptr [0x20019648], eax        # status word: C1 = Q1, C3 = Q0, C0 = Q2 of q=3
    fld tbyte ptr [0x20018640]             # extended 1.5
    fstp tbyte ptr [0x20019650]
    fldlg2
    fstp qword ptr [0x20019660]
    ret

.globl lf_switch
lf_switch:                                 # absolute jump table, bound by cmp/ja
    mov eax, dword ptr [0x20018800]
    cmp eax, 2
    ja lfl_default
    jmp dword ptr [4*eax + 0x20018810]
.globl lfl_case0
lfl_case0:
    mov ecx, 100
    jmp lfl_end
.globl lfl_case1
lfl_case1:
    mov ecx, 101
    jmp lfl_end
.globl lfl_case2
lfl_case2:
    mov ecx, 102
    jmp lfl_end
lfl_default:
    mov ecx, 1
lfl_end:
    mov dword ptr [0x20019800], ecx
    ret

.globl lf_switch_other
lf_switch_other:                           # reached only through the dispatcher fallback
    mov dword ptr [0x20019800], 777
    ret

# setjmp/longjmp through PLT-style stubs (GOT slots at IN+0xa00 hold the
# runtime's import addresses), across two nested guest calls.
.globl lf_plt_setjmp
lf_plt_setjmp:
.Lplt_setjmp:
    jmp dword ptr [0x20018a00]
.globl lf_plt_longjmp
lf_plt_longjmp:
.Lplt_longjmp:
    jmp dword ptr [0x20018a04]

.globl lf_setjmp
lf_setjmp:
    push ebx
    push esi
    mov ebx, 0x11111111
    push 0x20018900
    call .Lplt_setjmp
    add esp, 4
    add dword ptr [0x20019904], 1          # returns from setjmp
    test eax, eax
    jnz .Lsecond
    mov ebx, 0x22222222                    # longjmp restores the saved EBX
    push dword ptr [0x200189a0]            # value passed to longjmp
    push 0x20018900
    call .Ldeep
    mov dword ptr [0x20019900], 0xdead     # never reached
    jmp .Lout
.Lsecond:
    mov dword ptr [0x20019900], eax
    mov dword ptr [0x2001990c], ebx
    mov dword ptr [0x20019910], esp
.Lout:
    pop esi
    pop ebx
    ret

.globl lf_deep
lf_deep:
.Ldeep:
    push ebp
    mov ebp, esp
    push dword ptr [ebp + 12]
    push dword ptr [ebp + 8]
    call .Ldeeper
    leave
    ret

.globl lf_deeper
lf_deeper:
.Ldeeper:
    push dword ptr [esp + 8]
    push dword ptr [esp + 8]
    call .Lplt_longjmp
    ud2

# Loader: one synthetic module (tests/generate_abi_fixture.py) whose
# initialisers, destructors and atexit handlers append a letter to a log at
# OUT+0xc04 (length at OUT+0xc00).
.macro LOGGER name, letter
.globl \name
\name:
    mov eax, dword ptr [0x20019c00]
    mov byte ptr [eax + 0x20019c04], \letter
    add dword ptr [0x20019c00], 1
    ret
.endm
LOGGER lf_log_init, 0x49        # I: DT_INIT
LOGGER lf_log_ctor, 0x43        # C: init_array
LOGGER lf_log_fini_a, 0x61      # a: fini_array[0]
LOGGER lf_log_fini_b, 0x62      # b: fini_array[1]
LOGGER lf_log_fini, 0x46        # F: DT_FINI
LOGGER lf_log_exit_x, 0x78
LOGGER lf_log_exit_y, 0x79
LOGGER lf_log_exit_z, 0x7a

.globl lf_phdr_callback
lf_phdr_callback:                          # copies dl_phdr_info into data; returns data[8]
    mov eax, dword ptr [esp + 4]
    mov ecx, dword ptr [esp + 12]
    mov edx, dword ptr [eax]
    mov dword ptr [ecx], edx
    mov edx, dword ptr [eax + 4]
    mov dword ptr [ecx + 4], edx
    mov edx, dword ptr [eax + 8]
    mov dword ptr [ecx + 8], edx
    movzx edx, word ptr [eax + 12]
    mov dword ptr [ecx + 12], edx
    mov edx, dword ptr [esp + 8]
    mov dword ptr [ecx + 16], edx
    mov edx, dword ptr [eax + 16]
    mov dword ptr [ecx + 20], edx
    mov edx, dword ptr [eax + 32]
    mov dword ptr [ecx + 24], edx
    add dword ptr [ecx + 28], 1
    mov eax, dword ptr [ecx + 32]
    ret

.globl lf_start_init
lf_start_init:
    mov dword ptr [0x20019d00], 1
    ret
.globl lf_main
lf_main:                                   # returns 40 + argc + (init ran)
    mov eax, dword ptr [esp + 4]
    add eax, 40
    add eax, dword ptr [0x20019d00]
    ret

.globl lf_x87_fcom
lf_x87_fcom:                               # FCOM family: C3 C2 C0 of the status word (MSVC code)
    fld qword ptr [0x20018e00]             # 1
    fld qword ptr [0x20018e08]             # 2
    fcom st(1)                             # 2 > 1: none
    fnstsw ax
    mov word ptr [0x20019e00], ax
    fcom qword ptr [0x20018e10]            # 2 < 3: C0
    fnstsw ax
    mov word ptr [0x20019e02], ax
    fcom dword ptr [0x20018e18]            # 2 == 2.0f: C3
    fnstsw ax
    mov word ptr [0x20019e04], ax
    fld qword ptr [0x20018e20]             # NaN
    fucomp st(1)                           # unordered: C3 C2 C0, pops
    fnstsw ax
    mov word ptr [0x20019e06], ax
    fcomp                                  # 2 vs 1: none, pops
    fnstsw ax
    mov word ptr [0x20019e08], ax
    test ah, 0x41                          # MSVC: ST(0) > operand when C3 = C0 = 0
    setz byte ptr [0x20019e10]
    ftst                                   # 1 vs 0: none
    fnstsw ax
    mov word ptr [0x20019e0a], ax
    fld qword ptr [0x20018e10]             # 3 over 1
    fcompp                                 # 3 vs 1, pops both
    fnstsw ax
    mov word ptr [0x20019e0c], ax
    fld qword ptr [0x20018e00]             # 1
    fcomp qword ptr [0x20018e10]           # 1 < 3: C0, pops
    fnstsw ax
    sahf                                   # C0 -> CF
    setb byte ptr [0x20019e11]
    ret

.globl lf_cvtps2pi
lf_cvtps2pi:                               # packed float -> MMX integers
    movups xmm0, xmmword ptr [0x20018e40]
    cvtps2pi mm0, xmm0                     # round to nearest even
    movq qword ptr [0x20019e40], mm0
    cvttps2pi mm1, qword ptr [0x20018e40]  # truncate
    movq qword ptr [0x20019e48], mm1
    cvtps2pi mm2, qword ptr [0x20018e48]   # NaN, out of range: integer indefinite
    movq qword ptr [0x20019e50], mm2
    emms
    ret

.globl lf_ssse3
lf_ssse3:                                   # SSE3/SSSE3 and missing SSE2 ops (Half-Life 2)
    movdqu xmm0, xmmword ptr [0x20018f00]   # a: bytes 0x00..0x0f
    movdqu xmm1, xmmword ptr [0x20018f10]   # pshufb control
    movdqa xmm2, xmm0
    pshufb xmm2, xmm1
    movdqu xmmword ptr [0x20019f00], xmm2
    movdqu xmm3, xmmword ptr [0x20018f20]   # b: bytes 0x10..0x1f
    movdqa xmm2, xmm3
    palignr xmm2, xmm0, 5                   # (b:a) >> 5 bytes
    movdqu xmmword ptr [0x20019f10], xmm2
    movdqa xmm2, xmm3
    palignr xmm2, xmm0, 20                  # beyond 16: high part shifted, zero filled
    movdqu xmmword ptr [0x20019f20], xmm2
    movdqu xmm4, xmmword ptr [0x20018f30]   # signed bytes for pmaddubsw
    movdqu xmm5, xmmword ptr [0x20018f40]   # unsigned bytes
    pmaddubsw xmm5, xmm4
    movdqu xmmword ptr [0x20019f30], xmm5
    pabsw xmm6, xmmword ptr [0x20018f50]
    movdqu xmmword ptr [0x20019f40], xmm6
    movddup xmm7, qword ptr [0x20018f60]
    movdqu xmmword ptr [0x20019f50], xmm7
    movupd xmm6, xmmword ptr [0x20018f70]
    movupd xmm7, xmmword ptr [0x20018f80]
    addsubpd xmm6, xmm7
    movupd xmmword ptr [0x20019f60], xmm6
    movdqa xmm2, xmm0
    psadbw xmm2, xmm3
    movdqu xmmword ptr [0x20019f70], xmm2
    pextrw eax, xmm0, 3
    mov dword ptr [0x20019f80], eax
    pextrd ecx, xmm3, 2
    mov dword ptr [0x20019f84], ecx
    movq mm0, qword ptr [0x20018f00]
    mov edx, 0xbeef
    pinsrw mm0, edx, 1
    movntq qword ptr [0x20019f88], mm0
    pinsrw xmm0, word ptr [0x20018f90], 7
    movdqu xmmword ptr [0x20019f90], xmm0
    lddqu xmm1, xmmword ptr [0x20018f01]
    movdqu xmmword ptr [0x20019fa0], xmm1
    emms
    ret

.globl lf_jecxz
lf_jecxz:                                   # JECXZ: branch on ECX == 0, flags untouched
    mov ecx, dword ptr [0x20018ff0]
    mov eax, 1
    jecxz 1f
    mov eax, 2
1:
    mov dword ptr [0x20019ff0], eax
    ret
