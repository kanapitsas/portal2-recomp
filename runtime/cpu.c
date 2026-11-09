#include "rt.h"
#include <stdlib.h>
#include <time.h>

void rt_cpu_reset(CPU *c) {
    memset(c, 0, sizeof(*c));
    c->fcw = 0x037f;
    c->mxcsr = 0x1f80;
    c->flags.pf = 1;
}

// CPUID describes the virtual i386 machine whose code is being recompiled,
// rather than pretending the ARM host can execute x86 instructions. The AOT
// lifter still rejects individual instructions it cannot yet lower.
// Nombre de processeurs logiques invités, source unique pour CPUID, sysconf et
// /proc/cpuinfo. 1 par défaut ; P2_GUEST_CPUS=N (1..16) pour expérimenter :
// l'exécution invitée reste sérialisée par le verrou (scheduler.c).
unsigned rt_guest_cpu_count(void) {
    static _Atomic unsigned count;
    unsigned value = atomic_load_explicit(&count, memory_order_relaxed);
    if (!value) {
        const char *e = getenv("P2_GUEST_CPUS");
        long n = e ? strtol(e, NULL, 10) : 1;
        value = n < 1 ? 1 : n > 16 ? 16 : (unsigned)n;
        atomic_store_explicit(&count, value, memory_order_relaxed);
    }
    return value;
}

void rt_cpuid(CPU *c) {
    uint32_t leaf = c->eax;
    c->eax = c->ebx = c->ecx = c->edx = 0;
    if (leaf == 0) {
        c->eax = 1;
        c->ebx = 0x756e6547; c->edx = 0x49656e69; c->ecx = 0x6c65746e; // GenuineIntel
    } else if (leaf == 1) {
        c->eax = 0x000006f2;
        unsigned cpus = rt_guest_cpu_count();
        c->ebx = cpus << 16; // processeurs logiques invités
        c->edx = (1u << 0) | (1u << 4) | (1u << 8) | (1u << 15) |
                 (1u << 23) | (1u << 24) | (1u << 25) | (1u << 26) | (cpus > 1 ? 1u << 28 : 0u);
    } else if (leaf == 0x80000000u) {
        c->eax = 0x80000004u;
    } else if (leaf >= 0x80000002u && leaf <= 0x80000004u) {
        static const char brand[48] = "Portal 2 AOT virtual CPU (Apple Silicon)";
        const char *part = brand + (leaf - 0x80000002u) * 16;
        memcpy(&c->eax, part, 4); memcpy(&c->ebx, part + 4, 4);
        memcpy(&c->ecx, part + 8, 4); memcpy(&c->edx, part + 12, 4);
    }
}

// One virtual tick per nanosecond. The original calibration code therefore
// measures a coherent 1 GHz timebase, independent of the ARM clock frequency.
uint64_t rt_rdtsc(void) {
    struct timespec now;
    if (clock_gettime(CLOCK_MONOTONIC, &now)) return 0;
    return (uint64_t)now.tv_sec * UINT64_C(1000000000) + (uint64_t)now.tv_nsec;
}

// Convert the runtime's binary64 x87 approximation to the architectural
// 80-bit storage format, including subnormals, infinities and NaNs.
void rt_store_extended(uint32_t address, double value) {
    uint64_t bits; memcpy(&bits, &value, 8);
    uint64_t fraction = bits & UINT64_C(0x000fffffffffffff), mantissa = 0;
    uint16_t exponent = (uint16_t)((bits >> 52) & 0x7ff);
    uint16_t sign = (uint16_t)((bits >> 48) & 0x8000);
    if (exponent == 0x7ff) {
        exponent = 0x7fff;
        mantissa = UINT64_C(0x8000000000000000) | (fraction << 11);
    } else if (exponent) {
        exponent = (uint16_t)(exponent + 16383 - 1023);
        mantissa = (fraction | (UINT64_C(1) << 52)) << 11;
    } else if (fraction) {
        unsigned top = 63u - (unsigned)__builtin_clzll(fraction);
        exponent = (uint16_t)(16383 + (int)top - 1074);
        mantissa = fraction << (63 - top);
    }
    W64(address, mantissa); W16(address + 8, sign | exponent);
}

void rt_fxsave(CPU *c, uint32_t address) {
    if (address & 15) rt_trap(c, c->eip, "unaligned FXSAVE");
    memset(rt_write_span(c, address, 512), 0, 512);
    W16(address, c->fcw);
    W16(address + 2, (c->fsw & ~(7u << 11)) | (c->ftop << 11));
    W8(address + 4, c->ftags);
    W32(address + 24, c->mxcsr);
    W32(address + 28, 0x0000ffffu);
    for (unsigned i = 0; i < 8; ++i) {
        rt_store_extended(address + 32 + 16 * i, c->st[(c->ftop + i) & 7]);
        WX(address + 160 + 16 * i, c->xmm[i]);
    }
}

void rt_ldmxcsr(CPU *c, uint32_t value) {
    if (value & 0xffff0000u) rt_trap(c, c->eip, "reserved MXCSR bits");
    c->mxcsr = value;
}

double rt_x87_round(CPU *c, double value) {
    switch ((c->fcw >> 10) & 3) {
    case 1: return floor(value);
    case 2: return ceil(value);
    case 3: return trunc(value);
    default: return nearbyint(value);
    }
}

void rt_div32(CPU *c, uint32_t divisor, int is_signed, uint32_t pc) {
    if (!divisor) rt_trap(c, pc, "integer divide by zero");
    uint64_t numerator = ((uint64_t)c->edx << 32) | c->eax;
    if (is_signed) {
        int64_t lhs = (int64_t)numerator, rhs = (int32_t)divisor;
        if (lhs == INT64_MIN && rhs == -1) rt_trap(c, pc, "integer quotient overflow");
        int64_t quotient = lhs / rhs;
        if (quotient < INT32_MIN || quotient > INT32_MAX) rt_trap(c, pc, "integer quotient overflow");
        c->eax = (uint32_t)quotient;
        c->edx = (uint32_t)(lhs % rhs);
    } else {
        uint64_t quotient = numerator / divisor;
        if (quotient > UINT32_MAX) rt_trap(c, pc, "integer quotient overflow");
        c->eax = (uint32_t)quotient; c->edx = (uint32_t)(numerator % divisor);
    }
}

double rt_sse_round(CPU *c, double value) {
    switch ((c->mxcsr >> 13) & 3) {
    case 1: return floor(value);
    case 2: return ceil(value);
    case 3: return trunc(value);
    default: return nearbyint(value);
    }
}

// Inverse of rt_store_extended. Rounds the 64-bit significand to binary64
// (the documented precision limit of the x87 model); keeps NaN payload bits.
double rt_load_extended(uint32_t address) {
    uint64_t mantissa = R64(address);
    uint16_t top = R16(address + 8);
    int negative = top >> 15;
    unsigned exponent = top & 0x7fffu;
    double value;
    if (exponent == 0x7fff) {
        uint64_t fraction = mantissa & UINT64_C(0x7fffffffffffffff);
        uint64_t bits = UINT64_C(0x7ff0000000000000) | (fraction >> 11);
        if (fraction && !(fraction >> 11)) bits |= 1;   // keep it a NaN
        memcpy(&value, &bits, 8);
    } else {
        int scale = (exponent ? (int)exponent : 1) - 16383 - 63;
        value = ldexp((double)mantissa, scale);
    }
    return negative ? -value : value;
}

void rt_fprem(CPU *c, int ieee) {
    double a = ST(0), b = ST(1);
    c->fsw &= (uint16_t)~0x4700u;                        // C0, C1, C2, C3
    if (isnan(a) || isnan(b) || isinf(a) || b == 0) { ST(0) = NAN; return; }
    if (isinf(b)) return;                                // ST(0) unchanged, quotient 0
    int quotient;
    double result;
    if (ieee) {
        result = remquo(a, b, &quotient);
        if (quotient < 0) quotient = -quotient;
    } else {
        double magnitude = remquo(fabs(a), fabs(b), &quotient);
        if (magnitude < 0) { magnitude += fabs(b); --quotient; }
        result = copysign(magnitude, a);
    }
    unsigned q = (unsigned)quotient & 7u;
    c->fsw |= (uint16_t)(((q >> 2) & 1u) << 8 | (q & 1u) << 14 | ((q >> 1) & 1u) << 9);
    ST(0) = result;
}

void rt_div_narrow(CPU *c, uint32_t divisor, int width, int is_signed, uint32_t pc) {
    uint32_t mask = width == 8 ? 0xffu : 0xffffu;
    divisor &= mask;
    if (!divisor) rt_trap(c, pc, "integer divide by zero");
    uint32_t dividend = width == 8 ? (c->eax & 0xffffu) : (((c->edx & 0xffffu) << 16) | (c->eax & 0xffffu));
    uint32_t quotient, remainder;
    if (is_signed) {
        int32_t lhs = (int32_t)sx(dividend, width * 2), rhs = (int32_t)sx(divisor, width);
        int32_t limit = width == 8 ? 127 : 32767;
        // -32768/-1 (ou -128/-1) : quotient hors limite, comme rt_div32 pour INT_MIN/-1.
        if (rhs == -1 && lhs == -limit - 1) rt_trap(c, pc, "integer quotient overflow");
        int32_t q = lhs / rhs;
        if (q > limit || q < -limit - 1) rt_trap(c, pc, "integer quotient overflow");
        quotient = (uint32_t)q & mask; remainder = (uint32_t)(lhs % rhs) & mask;
    } else {
        if (dividend / divisor > mask) rt_trap(c, pc, "integer quotient overflow");
        quotient = dividend / divisor; remainder = dividend % divisor;
    }
    if (width == 8) c->eax = (c->eax & 0xffff0000u) | (remainder << 8) | quotient;
    else {
        c->eax = (c->eax & 0xffff0000u) | quotient;
        c->edx = (c->edx & 0xffff0000u) | remainder;
    }
}
