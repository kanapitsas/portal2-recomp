// Semantics of instructions added after the full-game coverage audit. The
// guest code is real i386 machine code (tests/lifter_programs.s), lifted by
// tools/lift.py; expected values come from the architecture manuals, not from
// the runtime helpers under test.
#include "checks.h"
#include <math.h>
#include <stdio.h>

uint32_t lifter_entry(const char *name);
#define IN 0x20018000u
#define OUT 0x20019000u

static CPU run_named(const char *name) {
    uint32_t entry = lifter_entry(name);
    assert(entry);
    CPU c; rt_cpu_reset(&c); c.esp = 0x20010000;
    rt_call(&c, entry, RT_RETURN_SENTINEL);
    assert(c.esp == 0x20010000 && c.eip == RT_RETURN_SENTINEL);
    return c;
}
static void clear(void) { memset(GPTR(IN), 0, 0x2000); }
static float f32(uint32_t a) { return RF(a); }
static uint32_t float_bits(float v) { uint32_t b; memcpy(&b, &v, 4); return b; }
static void put_floats(uint32_t a, float x, float y, float z, float w) {
    WF(a, x); WF(a + 4, y); WF(a + 8, z); WF(a + 12, w);
}
static void expect_floats(uint32_t a, float x, float y, float z, float w) {
    assert(f32(a) == x && f32(a + 4) == y && f32(a + 8) == z && f32(a + 12) == w);
}

RT_CHECK(lifter_bit_tests) {
    clear();
    W32(IN + 0xc, 0); W32(IN + 0x10, 0x8); W32(IN + 0x14, 0x100);
    run_named("lf_bits");
    assert(R8(OUT) == 1 && R8(OUT + 1) == 0 && R8(OUT + 2) == 1);
    assert(R32(OUT + 4) == 0x80000003u && R32(OUT + 8) == 0x80000002u && R32(OUT + 0xc) == 0x80000012u);
    assert(R32(IN + 0xc) == 0x80000000u && R8(OUT + 3) == 0);       // bit string reaches the previous dword
    assert(R32(IN + 0x14) == 0 && R8(OUT + 0x10) == 1);              // and the following one
    assert(R32(IN + 0x10) == 0x8 && R8(OUT + 0x11) == 1);            // immediate index is modulo 32
}

RT_CHECK(lifter_narrow_integer) {
    clear();
    CPU c = run_named("lf_narrow");
    assert(R32(OUT + 0x24) == 0xaaaa0f00u && R8(OUT + 0x20) == 1);  // MUL r8, CF=OF=AH!=0
    assert(R32(OUT + 0x28) == 0x0000fffau && R8(OUT + 0x21) == 0);  // IMUL r8 fits in AL
    assert(R32(OUT + 0x2c) == 0x1111020eu);                         // DIV r8: AH=rem, AL=quot
    assert(R32(OUT + 0x30) == 0xfffffffdu && (R32(OUT + 0x34) & 0xffff) == 0xffff);  // IDIV r16
    assert(R32(OUT + 0x38) == 0x1234ff80u);                         // CBW
    assert(R8(OUT + 0x3c) == 1 && R8(OUT + 0x3d) == 1 && R8(OUT + 0x3e) == 1 && R8(OUT + 0x3f) == 1);
    assert(R32(OUT + 0x40) == 0x9700u);                             // LAHF: SF AF PF 1 CF
    (void)c;
}

RT_CHECK(lifter_sse_shuffles) {
    clear();
    put_floats(IN + 0x100, 1, 2, 3, 4); put_floats(IN + 0x110, 5, 6, 7, 8);
    put_floats(IN + 0x120, -1, 2, -3, 0);
    run_named("lf_sse");
    expect_floats(OUT + 0x100, 4, 3, 6, 5);
    expect_floats(OUT + 0x110, 1, 5, 2, 6);
    expect_floats(OUT + 0x120, 3, 7, 4, 8);
    expect_floats(OUT + 0x130, 7, 8, 5, 6);
    assert(f32(OUT + 0x140) == 5 && f32(OUT + 0x144) == 6 && f32(OUT + 0x148) == 7 && f32(OUT + 0x14c) == 8);
    assert(R32(OUT + 0x150) == 5);
    float in[4] = {-1, 2, -3, 0}, value[4] = {1, 2, 3, 4};
    for (int i = 0; i < 4; ++i) assert(R32(OUT + 0x160 + 4 * i) == (~float_bits(in[i]) & float_bits(value[i])));
    expect_floats(OUT + 0x170, 5, 6, 7, 8);
}

RT_CHECK(lifter_sse_math) {
    clear();
    WF(IN + 0x200, 16); WD(IN + 0x208, 2.25); WF(IN + 0x210, NAN); WF(IN + 0x214, 3);
    WD(IN + 0x218, -0.0); WD(IN + 0x220, 0.0); put_floats(IN + 0x230, 4, 16, 0.25f, 1);
    run_named("lf_sse_math");
    assert(f32(OUT + 0x200) == 4 && RD(OUT + 0x208) == 1.5);
    assert(f32(OUT + 0x210) == 3);                    // MINSS: NaN first operand -> second operand
    assert(isnan(f32(OUT + 0x214)));                  // MAXSS: NaN second operand -> NaN
    double zero = RD(OUT + 0x218); assert(zero == 0 && !signbit(zero));  // equal -> second (+0)
    expect_floats(OUT + 0x220, 2, 4, 0.5f, 1);
    float rcp[4] = {0.25f, 0.0625f, 4, 1}, rsqrt[4] = {0.5f, 0.25f, 2, 1};
    for (int i = 0; i < 4; ++i) {                     // ISA bound: relative error <= 1.5 * 2^-12
        assert(fabsf(f32(OUT + 0x230 + 4 * i) - rcp[i]) <= 1.5f * 0x1p-12f * rcp[i]);
        assert(fabsf(f32(OUT + 0x240 + 4 * i) - rsqrt[i]) <= 1.5f * 0x1p-12f * rsqrt[i]);
    }
}

RT_CHECK(lifter_sse2_integer) {
    clear();
    uint32_t a[4] = {1, 0xffffffffu, 0x00010002u, 0x80000000u}, b[4] = {1, 2, 0x00030004u, 0x7fffffffu};
    memcpy(GPTR(IN + 0x300), a, 16); memcpy(GPTR(IN + 0x310), b, 16);
    run_named("lf_int");
    uint32_t shuffled[4] = {a[3], a[2], a[1], a[0]};
    assert(!memcmp(GPTR(OUT + 0x300), shuffled, 16));
    uint16_t aw[8], bw[8]; memcpy(aw, a, 16); memcpy(bw, b, 16);
    uint16_t unpacked[8] = {aw[0], bw[0], aw[1], bw[1], aw[2], bw[2], aw[3], bw[3]};
    assert(!memcmp(GPTR(OUT + 0x310), unpacked, 16));
    for (int i = 0; i < 4; ++i) {
        assert(R32(OUT + 0x320 + 4 * i) == a[i] + b[i]);
        assert(R32(OUT + 0x330 + 4 * i) == (a[i] == b[i] ? 0xffffffffu : 0));
    }
    for (int i = 0; i < 8; ++i) {
        assert(R16(OUT + 0x340 + 2 * i) == ((int16_t)aw[i] > (int16_t)bw[i] ? 0xffff : 0));
        assert(R16(OUT + 0x380 + 2 * i) == (uint16_t)((int16_t)aw[i] * (int16_t)bw[i]));
        assert(R16(OUT + 0x390 + 2 * i) == (uint16_t)(((int16_t)aw[i] * (int16_t)bw[i]) >> 16));
    }
    assert(R64(OUT + 0x350) == (uint64_t)a[0] * b[0] && R64(OUT + 0x358) == (uint64_t)a[2] * b[2]);
    uint16_t packed[8] = {1, 0xffff, 0x7fff, 0x8000, 1, 2, 0x7fff, 0x7fff};
    assert(!memcmp(GPTR(OUT + 0x360), packed, 16));
    uint8_t bytes[16] = {0}; memcpy(bytes, GPTR(IN + 0x300) + 5, 11);
    assert(!memcmp(GPTR(OUT + 0x370), bytes, 16));
    assert(R64(OUT + 0x3a0) == UINT64_MAX && R64(OUT + 0x3a8) == UINT64_MAX);
}

RT_CHECK(lifter_ssse3) {
    clear();
    uint8_t a[16], b[16], control[16] = {15, 0x80, 3, 3, 0x8f, 0, 1, 2, 0x10, 14, 13, 0xff, 7, 6, 5, 4};
    for (int i = 0; i < 16; ++i) { a[i] = (uint8_t)i; b[i] = (uint8_t)(0x10 + i); }
    memcpy(GPTR(IN + 0xf00), a, 16); memcpy(GPTR(IN + 0xf10), control, 16); memcpy(GPTR(IN + 0xf20), b, 16);
    int8_t sb[16]; uint8_t ub[16];
    for (int i = 0; i < 16; ++i) { sb[i] = (int8_t)(i * 37 - 128); ub[i] = (uint8_t)(255 - i * 3); }
    sb[0] = 127; sb[1] = 127; ub[0] = 255; ub[1] = 255;            // 2 * 255 * 127 saturates
    memcpy(GPTR(IN + 0xf30), sb, 16); memcpy(GPTR(IN + 0xf40), ub, 16);
    int16_t words[8] = {-5, 5, -32768, 0, 32767, -1, 1, -300};
    memcpy(GPTR(IN + 0xf50), words, 16);
    W64(IN + 0xf60, UINT64_C(0x1122334455667788));
    WD(IN + 0xf70, 10); WD(IN + 0xf78, 20); WD(IN + 0xf80, 1); WD(IN + 0xf88, 2);
    W16(IN + 0xf90, 0x4321);
    run_named("lf_ssse3");
    for (int i = 0; i < 16; ++i)                                   // PSHUFB: bit 7 zeroes, low 4 bits index
        assert(R8(OUT + 0xf00 + i) == ((control[i] & 0x80) ? 0 : a[control[i] & 15]));
    for (int i = 0; i < 16; ++i) {                                 // PALIGNR: b:a >> n bytes
        assert(R8(OUT + 0xf10 + i) == (i + 5 < 16 ? a[i + 5] : b[i + 5 - 16]));
        assert(R8(OUT + 0xf20 + i) == (i + 4 < 16 ? b[i + 4] : 0));
    }
    for (int i = 0; i < 8; ++i) {
        int v = ub[2 * i] * sb[2 * i] + ub[2 * i + 1] * sb[2 * i + 1];
        v = v < -32768 ? -32768 : v > 32767 ? 32767 : v;
        assert(R16(OUT + 0xf30 + 2 * i) == (uint16_t)v);
        int w = words[i] < 0 ? -words[i] : words[i];               // PABSW: |-32768| stays 0x8000
        assert(R16(OUT + 0xf40 + 2 * i) == (uint16_t)w);
    }
    assert(R16(OUT + 0xf30) == 0x7fff);
    assert(R64(OUT + 0xf50) == UINT64_C(0x1122334455667788) && R64(OUT + 0xf58) == UINT64_C(0x1122334455667788));
    assert(RD(OUT + 0xf60) == 9 && RD(OUT + 0xf68) == 22);          // ADDSUBPD: low -, high +
    assert(R64(OUT + 0xf70) == 8 * 16 && R64(OUT + 0xf78) == 8 * 16);  // PSADBW per 64-bit half
    assert(R32(OUT + 0xf80) == 0x0706u && R32(OUT + 0xf84) == 0x1b1a1918u);
    assert(R64(OUT + 0xf88) == UINT64_C(0x07060504beef0100));       // PINSRW mm, then MOVNTQ
    assert(R16(OUT + 0xf9e) == 0x4321 && R8(OUT + 0xf90) == 0);       // PINSRW xmm, word 7
    for (int i = 0; i < 15; ++i) assert(R8(OUT + 0xfa0 + i) == a[i + 1]);  // LDDQU unaligned
}

RT_CHECK(lifter_mmx) {
    clear();
    W64(IN + 0x400, UINT64_C(0x0000800000010001)); W64(IN + 0x408, 4);
    CPU c = run_named("lf_mmx");
    assert(R64(OUT + 0x400) == UINT64_C(0x0000800080008001));   // PADDW wraps per lane
    assert(R64(OUT + 0x408) == UINT64_C(0x0000080000001000));   // PSRAD by 4
    assert(R64(OUT + 0x410) == UINT64_C(0x7fff800008001000));   // PACKSSDW saturates
    assert(R32(OUT + 0x418) == 0x08001000u);
    assert(R64(OUT + 0x420) == UINT64_C(0x0000800080008001) && R64(OUT + 0x428) == 0);
    assert(f32(OUT + 0x430) == (float)(int32_t)0x80008001 && f32(OUT + 0x434) == 32768.0f);
    assert(f32(OUT + 0x438) == 0 && f32(OUT + 0x43c) == 0);     // upper lanes kept
    assert(c.ftags == 0 && c.ftop == 0);                          // EMMS
}

RT_CHECK(lifter_x87_compare) {
    clear();
    WD(IN + 0x500, 1); WD(IN + 0x508, 2); WD(IN + 0x510, NAN);
    CPU c = run_named("lf_x87_compare");
    assert(R8(OUT + 0x500) == 1 && R8(OUT + 0x501) == 0);
    assert(RD(OUT + 0x508) == 1);                                 // FCMOVNBE taken
    assert(R8(OUT + 0x502) == 1 && R8(OUT + 0x503) == 1 && R8(OUT + 0x504) == 1);  // unordered
    assert(RD(OUT + 0x510) == 2);                                 // FCMOVB not taken
    assert(c.ftop == 0 && c.ftags == 0);
}

RT_CHECK(lifter_x87_status_compare) {
    clear();
    WD(IN + 0xe00, 1); WD(IN + 0xe08, 2); WD(IN + 0xe10, 3); WF(IN + 0xe18, 2.0f); WD(IN + 0xe20, NAN);
    CPU c = run_named("lf_x87_fcom");
    const uint16_t conditions = 0x4700;
    assert((R16(OUT + 0xe00) & conditions) == 0);                 // greater
    assert((R16(OUT + 0xe02) & conditions) == 0x100);             // less: C0
    assert((R16(OUT + 0xe04) & conditions) == 0x4000);            // equal: C3
    assert((R16(OUT + 0xe06) & conditions) == 0x4500);            // unordered: C3 C2 C0
    assert((R16(OUT + 0xe08) & conditions) == 0 && R8(OUT + 0xe10) == 1);
    assert((R16(OUT + 0xe0a) & conditions) == 0);                 // FTST 1 > 0
    assert((R16(OUT + 0xe0c) & conditions) == 0);                 // FCOMPP 3 > 1
    assert(R8(OUT + 0xe11) == 1);                                 // SAHF: C0 -> CF
    assert(c.ftop == 0 && c.ftags == 0);                          // every push popped
}

RT_CHECK(lifter_cvtps2pi) {
    clear();
    put_floats(IN + 0xe40, 2.5f, -1.5f, NAN, 3e9f);
    CPU c = run_named("lf_cvtps2pi");
    assert(R32(OUT + 0xe40) == 2u && R32(OUT + 0xe44) == (uint32_t)-2);   // nearest even
    assert(R32(OUT + 0xe48) == 2u && R32(OUT + 0xe4c) == (uint32_t)-1);   // truncation
    assert(R32(OUT + 0xe50) == 0x80000000u && R32(OUT + 0xe54) == 0x80000000u);
    assert(c.ftags == 0);
}

RT_CHECK(lifter_x87_math) {
    clear();
    WD(IN + 0x600, -2.75); W32(IN + 0x608, 5); WD(IN + 0x610, 1); WD(IN + 0x618, 8);
    WD(IN + 0x620, 0.5); WD(IN + 0x628, 3); WD(IN + 0x630, -11);
    W64(IN + 0x640, UINT64_C(0xc000000000000000)); W16(IN + 0x648, 0x3fff);
    CPU c = run_named("lf_x87_math");
    assert(R16(OUT + 0x600) == 0x037f);
    assert(RD(OUT + 0x608) == -2);                                // truncating FRNDINT
    assert(RD(OUT + 0x610) == 5.0 / -15.0);
    assert(RD(OUT + 0x618) == atan2(1, 1) && RD(OUT + 0x620) == 3);
    assert(RD(OUT + 0x628) == sin(0.5));
    assert(RD(OUT + 0x630) == 1 && RD(OUT + 0x638) == tan(0.5));
    assert(RD(OUT + 0x640) == -2);                                // FPREM keeps the dividend's sign
    assert((R32(OUT + 0x648) & 0xffff) == 0x7200);                // TOP=6, C3=Q0, C1=Q1, C0=Q2=0, C2=0
    assert(R64(OUT + 0x650) == UINT64_C(0xc000000000000000) && R16(OUT + 0x658) == 0x3fff);
    assert(RD(OUT + 0x660) == log10(2.0));
    assert(c.ftop == 0 && c.fcw == 0x037f);
}

RT_CHECK(lifter_extended_conversions) {
    double cases[] = {0.0, -0.0, 1.0, -3.5, 1e-310, 1e300, INFINITY, -INFINITY};
    for (unsigned i = 0; i < sizeof(cases) / sizeof(*cases); ++i) {
        rt_store_extended(OUT + 0x700, cases[i]);
        double back = rt_load_extended(OUT + 0x700);
        assert(back == cases[i] && signbit(back) == signbit(cases[i]));
    }
    rt_store_extended(OUT + 0x700, NAN);
    assert(isnan(rt_load_extended(OUT + 0x700)));
    W64(OUT + 0x700, 1); W16(OUT + 0x708, 0x3fff + 63);             // unnormal: 1 * 2^(63-63)
    assert(rt_load_extended(OUT + 0x700) == 1.0);
}

RT_CHECK(lifter_switch_tables) {
    clear();
    for (uint32_t i = 0; i < 3; ++i) {
        char name[16]; snprintf(name, sizeof(name), "lfl_case%u", i);
        W32(IN + 0x810 + 4 * i, lifter_entry(name));
    }
    for (uint32_t selector = 0; selector < 5; ++selector) {
        W32(IN + 0x800, selector);
        run_named("lf_switch");
        assert(R32(OUT + 0x800) == (selector < 3 ? 100 + selector : 1));
    }
    // A table value outside the statically recovered cases still follows the
    // guest: the checked dispatcher runs the separately compiled function.
    W32(IN + 0x814, lifter_entry("lf_switch_other"));
    W32(IN + 0x800, 1);
    run_named("lf_switch");
    assert(R32(OUT + 0x800) == 777);
}

RT_CHECK(lifter_setjmp_longjmp) {
    for (uint32_t value = 0; value < 2; ++value) {
        clear();
        W32(IN + 0xa00, 0xf10000a0); W32(IN + 0xa04, 0xf10000b0);   // GOT slots -> _setjmp, longjmp
        W32(IN + 0x9a0, value ? 7 : 0);   // after the 156-byte jmp_buf
        CPU c = run_named("lf_setjmp");
        assert(R32(OUT + 0x904) == 2);                         // setjmp returned twice
        assert(R32(OUT + 0x900) == (value ? 7u : 1u));         // longjmp(env, 0) makes setjmp return 1
        assert(R32(OUT + 0x90c) == 0x11111111u);               // callee-saved register from the buffer
        assert(R32(OUT + 0x910) == 0x20010000u - 12);          // sentinel, EBX, ESI: argument popped
        assert(R32(IN + 0x900 + 20) != 0 && R32(IN + 0x900 + 24) == 0);
        (void)c;
    }
}

// --- loader ------------------------------------------------------------------
#include "modules.h"
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

static uint32_t guest_call(const char *name, unsigned count, const uint32_t *args) {
    CPU c; rt_cpu_reset(&c); c.esp = 0x20010000;
    for (unsigned i = count; i; --i) PUSH(&c, args[i - 1]);
    rt_call(&c, rt_dynamic_import(name), RT_RETURN_SENTINEL);   // same path as a guest import call
    assert(c.esp == 0x20010000 - 4 * count);
    return c.eax;
}
#define CALL(name, ...) guest_call(name, sizeof((uint32_t[]){__VA_ARGS__}) / 4, (uint32_t[]){__VA_ARGS__})
static const char *log_text(void) { W8(OUT + 0xc04 + R32(OUT + 0xc00), 0); return GPTR(OUT + 0xc04); }
static void log_reset(void) { W32(OUT + 0xc00, 0); }

static void build_fake_image(void) {
    const uint32_t base = 0x2001c000;
    memset(GPTR(base), 0, 0x1000);
    memcpy(GPTR(base), "\177ELF\001\001\001", 7);
    W16(base + 16, 3); W16(base + 18, 3); W32(base + 28, 52);
    W16(base + 40, 52); W16(base + 42, 32); W16(base + 44, 3);
    uint32_t ph = base + 52;
    uint32_t headers[3][8] = {{1, 0, 0, 0, 0x1000, 0x1000, 5, 0x1000},
                              {2, 0x200, 0x200, 0x200, 24, 24, 6, 4},
                              {0x6474e550, 0x300, 0x300, 0x300, 0x20, 0x20, 4, 4}};
    memcpy(GPTR(ph), headers, sizeof(headers));
    uint32_t dynamic[6] = {26, 0x280, 28, 8, 0, 0};                 // DT_FINI_ARRAY(SZ), DT_NULL
    memcpy(GPTR(base + 0x200), dynamic, sizeof(dynamic));
    W32(base + 0x280, lifter_entry("lf_log_fini_a")); W32(base + 0x284, lifter_entry("lf_log_fini_b"));
}

RT_CHECK(lifter_loader) {
    clear(); log_reset(); build_fake_image();
    const uint32_t path = IN + 0xc00, missing = IN + 0xc20, symbol = IN + 0xc40, data = IN + 0xc80;
    strcpy(GPTR(path), "/some/dir/libfake.so"); strcpy(GPTR(missing), "missing.so");
    strcpy(GPTR(symbol), "fake_symbol");
    uint32_t callback = lifter_entry("lf_phdr_callback");

    assert(CALL("dl_iterate_phdr", callback, data) == 0 && R32(data + 28) == 0);   // nothing loaded yet
    uint32_t handle = CALL("dlopen", path, 2);
    assert(handle == 0xcc000700u && !strcmp(log_text(), "IC"));                       // DT_INIT, then init_array
    assert(CALL("dlopen", path, 2) == handle && !strcmp(log_text(), "IC"));           // initialised once
    assert(CALL("dlsym", handle, symbol) == 0x2001c400u);

    assert(CALL("dl_iterate_phdr", callback, data) == 0 && R32(data + 28) == 1);
    assert(R32(data) == 0x2001c000u && R32(data + 8) == 0x2001c034u && R32(data + 12) == 3);
    assert(!strcmp(GPTR(R32(data + 4)), "libfake.so") && R32(data + 16) == 40);
    assert(R32(data + 20) == 1 && R32(data + 24) == 0);                               // dlpi_adds, no TLS
    W32(data + 32, 5);
    assert(CALL("dl_iterate_phdr", callback, data) == 5 && R32(data + 28) == 2);     // callback result stops

    assert(CALL("dlclose", handle) == 0 && CALL("dlclose", handle) == 0);
    assert(CALL("dlclose", handle) == UINT32_MAX);                                     // not open any more
    uint32_t message = CALL("dlerror", 0);
    assert(message && strstr(GPTR(message), "not open") && CALL("dlerror", 0) == 0);
    assert(CALL("dlclose", 0xcc000800u) == UINT32_MAX);                                // unknown handle
    assert(CALL("dlopen", path, 4 | 2) == handle);           // RTLD_NOLOAD: the image is kept (NODELETE)
    assert(CALL("dlopen", missing, 4 | 2) == 0);
    assert(CALL("dlmopen", 0, path, 2) == handle);                                     // LM_ID_BASE
    assert(CALL("dlmopen", UINT32_MAX, missing, 2) == 0 && CALL("dlerror", 0));       // LM_ID_NEWLM
    assert(!strcmp(log_text(), "IC"));

    log_reset();
    const uint32_t d1 = 0x2001c500, d2 = 0x2001c504;
    assert(CALL("__cxa_atexit", lifter_entry("lf_log_exit_x"), 1, d1) == 0);
    assert(CALL("__cxa_atexit", lifter_entry("lf_log_exit_y"), 2, d2) == 0);
    assert(CALL("__cxa_atexit", lifter_entry("lf_log_exit_z"), 3, d1) == 0);
    CALL("__cxa_finalize", d1);
    assert(!strcmp(log_text(), "zx"));                       // this DSO only, newest first
    CALL("__cxa_finalize", d1);
    assert(!strcmp(log_text(), "zx"));                       // each handler runs once
    CALL("__cxa_finalize", d2);
    assert(!strcmp(log_text(), "zxy"));
    CALL("_Jv_RegisterClasses", 0x2001c600);

    log_reset();
    CPU c; rt_cpu_reset(&c); c.esp = 0x20010000;
    rt_finalize_modules(&c);
    assert(!strcmp(log_text(), "baF"));                      // fini_array reversed, then DT_FINI
    rt_finalize_modules(&c);
    assert(!strcmp(log_text(), "baF"));
}

static int child_exit(CPU *c, const char *name) {
    static int in_child;
    if (!strcmp(name, "__lifter_child")) { in_child = 1; return 1; }
    if (!in_child || strcmp(name, "exit")) return 0;
    fflush(NULL);
    _exit((int)R32(c->esp + 4));
}
RT_SHIM_PROVIDER(child_exit)

RT_CHECK(lifter_libc_start_main) {
    clear();
    W32(IN + 0xd00, 0);                                     // argv = {"x", NULL}, envp = {NULL}
    strcpy(GPTR(IN + 0xd40), "x"); W32(IN + 0xd10, IN + 0xd40); W32(IN + 0xd14, 0); W32(IN + 0xd18, 0);
    fflush(NULL);
    pid_t pid = fork();
    assert(pid >= 0);
    if (!pid) {
        CPU c; rt_cpu_reset(&c);
        // Le vrai shim exit (process.c) peut passer avant child_exit : vider les
        // handlers atexit laissés par d'autres checks de ce processus.
        for (size_t i = rt_atexit_count(); i--;) rt_atexit_clear(i);
        rt_shim(&c, "__lifter_child");
        CALL("__libc_start_main", lifter_entry("lf_main"), 1, IN + 0xd10, lifter_entry("lf_start_init"), 0, 0, 0);
        _exit(99);                                          // __libc_start_main must not return
    }
    int status = 0, done = 0;
    for (int tries = 0; tries < 1000 && !(done = waitpid(pid, &status, WNOHANG) == pid); ++tries) usleep(10000);
    if (!done) { kill(pid, SIGKILL); waitpid(pid, &status, 0); assert(!"child hung"); }
    assert(WIFEXITED(status) && WEXITSTATUS(status) == 42);  // 40 + argc + init ran
}
