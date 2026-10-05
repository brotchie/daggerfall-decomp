/* runtime.c: the register file, lazy flags and the helpers of XnGine's literal C translation
   (see runtime.h). The flag formulas are the ones the CPU emulator (QEMU, under Unicorn) uses,
   undefined flags included: logic ops, shifts and multiplies clear AF, multiplies set ZF, SF
   and PF from the low half, a shift's OF compares the top bits before and after the last step,
   and divides leave the flags alone. */
#include "runtime.h"

struct xn_regs R;
struct xn_lazy F;

const u8 xn_parity[256] = {
    1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1, 0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0,
    0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0, 1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1,
    0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0, 1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1,
    1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1, 0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0,
    0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0, 1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1,
    1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1, 0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0,
    1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1, 0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0,
    0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0, 1,0,0,1,0,1,1,0,0,1,1,0,1,0,0,1
};

static const u32 mask[4] = {0xFF, 0xFFFF, 0xFFFFFFFF, 0xFFFFFFFF};
static const u32 sign[4] = {0x80, 0x8000, 0x80000000, 0x80000000};

#define K (F.op >> 2)
#define M mask[F.op & 3]
#define S sign[F.op & 3]

u32 xn_cf(void)
{
    switch (K) {
    case XF_ADD >> 2: return (F.r & M) < (F.a & M);
    case XF_ADC >> 2: return F.x ? (F.r & M) <= (F.a & M) : (F.r & M) < (F.a & M);
    case XF_SUB >> 2: return (F.a & M) < (F.b & M);
    case XF_SBB >> 2: return F.x ? (F.a & M) <= (F.b & M) : (F.a & M) < (F.b & M);
    case XF_LOGIC >> 2: return 0;
    case XF_INC >> 2: case XF_DEC >> 2: return F.x;
    case XF_SHL >> 2: return (F.a & S) != 0;
    case XF_SAR >> 2: return F.a & 1;
    case XF_MUL >> 2: return F.b != 0;
    }
    return R.eflags & 1;
}

static u32 zf(void)
{
    return F.op == XF_NONE ? (R.eflags >> 6) & 1 : (F.r & M) == 0;
}

static u32 sf(void)
{
    return F.op == XF_NONE ? (R.eflags >> 7) & 1 : (F.r & S) != 0;
}

static u32 pf(void)
{
    return F.op == XF_NONE ? (R.eflags >> 2) & 1 : xn_parity[F.r & 0xFF];
}

static u32 af(void)
{
    switch (K) {
    case XF_ADD >> 2: case XF_ADC >> 2: case XF_SUB >> 2: case XF_SBB >> 2:
        return ((F.a ^ F.b ^ F.r) >> 4) & 1;
    case XF_INC >> 2: return ((F.r ^ (F.r - 1) ^ 1) >> 4) & 1;
    case XF_DEC >> 2: return ((F.r ^ (F.r + 1) ^ 1) >> 4) & 1;
    case 0: return (R.eflags >> 4) & 1;
    }
    return 0;
}

static u32 of(void)
{
    switch (K) {
    case XF_ADD >> 2: case XF_ADC >> 2: return (~(F.a ^ F.b) & (F.a ^ F.r) & S) != 0;
    case XF_SUB >> 2: case XF_SBB >> 2: return ((F.a ^ F.b) & (F.a ^ F.r) & S) != 0;
    case XF_INC >> 2: return (F.r & M) == S;
    case XF_DEC >> 2: return (F.r & M) == S - 1;
    case XF_SHL >> 2: case XF_SAR >> 2: return ((F.a ^ F.r) & S) != 0;
    case XF_MUL >> 2: return F.b != 0;
    case 0: return (R.eflags >> 11) & 1;
    }
    return 0;
}

u32 xn_cond(u32 cc)
{
    u32 v;
    switch (cc >> 1) {
    case 0: v = of(); break;
    case 1: v = xn_cf(); break;
    case 2: v = zf(); break;
    case 3: v = xn_cf() | zf(); break;
    case 4: v = sf(); break;
    case 5: v = pf(); break;
    case 6: v = sf() != of(); break;
    default: v = zf() | (sf() != of()); break;
    }
    return v ^ (cc & 1);
}

u32 xn_eflags(void)
{
    u32 f;
    if (F.op == XF_NONE)
        return R.eflags;
    f = R.eflags & ~0x8D5u;
    f |= xn_cf() | pf() << 2 | af() << 4 | zf() << 6 | sf() << 7 | of() << 11;
    R.eflags = f;
    F.op = XF_NONE;
    return f;
}

void xn_popf(u32 v)
{
    R.eflags = v;
    F.op = XF_NONE;
}

void xn_setcf(u32 cf)
{
    xn_eflags();
    R.eflags = (R.eflags & ~1u) | cf;
}

/* rol, ror, rcl, rcr: CF and OF change (when the count is not 0), the rest stay */
void xn_rotate(u32 kind, u32 size, void *dst, u32 count)
{
    u32 bits = 8 << size, v, v0, cf, o, k;
    count &= 31;
    if (kind >= 2) {                    /* through the carry: a (bits + 1)-bit rotate */
        count %= bits + 1;
        if (count == 0)
            return;
    } else if (count == 0)
        return;
    xn_eflags();
    v = v0 = size == 0 ? *(u8 *)dst : size == 1 ? *(u16 *)dst : *(u32 *)dst;
    cf = R.eflags & 1;
    for (k = 0; k < count; k++) {
        u32 top = (v >> (bits - 1)) & 1, low = v & 1;
        switch (kind) {
        case 0: v = v << 1 | top; cf = top; break;
        case 1: v = v >> 1 | low << (bits - 1); cf = low; break;
        case 2: v = v << 1 | cf; cf = top; break;
        default: v = v >> 1 | cf << (bits - 1); cf = low; break;
        }
        if (bits < 32)
            v &= (1u << bits) - 1;
    }
    if (kind == 0)
        o = cf ^ ((v >> (bits - 1)) & 1);
    else if (kind == 1)
        o = ((v >> (bits - 1)) ^ (v >> (bits - 2))) & 1;
    else
        o = ((v0 ^ v) >> (bits - 1)) & 1;      /* rcl, rcr: the top bit before and after */
    if (size == 0) *(u8 *)dst = v; else if (size == 1) *(u16 *)dst = v; else *(u32 *)dst = v;
    R.eflags = (R.eflags & ~0x801u) | cf | o << 11;
}

u32 xn_bsf(u32 src, u32 old)
{
    u32 k;
    LF(XF_LOGIC | 2, 0, 0, src);
    if (src == 0)
        return old;
    for (k = 0; !(src & 1); k++)
        src >>= 1;
    return k;
}

u32 xn_bsr(u32 src, u32 old)
{
    u32 k;
    LF(XF_LOGIC | 2, 0, 0, src);
    if (src == 0)
        return old;
    for (k = 31; !(src & 0x80000000u); k--)
        src <<= 1;
    return k;
}

/* -- multiplies ------------------------------------------------------------------------ */

void xn_mul1(u32 v, u32 size)
{
    u32 hi, lo;
    if (size == 0) {
        lo = (u32)AL * (v & 0xFF);
        AX = lo;
        LF(XF_MUL | 0, 0, (lo >> 8) != 0, lo);
    } else if (size == 1) {
        lo = (u32)AX * (v & 0xFFFF);
        AX = lo;
        DX = lo >> 16;
        LF(XF_MUL | 1, 0, (lo >> 16) != 0, lo);
    } else {
        lo = xn_mulu(R.eax, v, &hi);
        R.eax = lo;
        R.edx = hi;
        LF(XF_MUL | 2, 0, hi != 0, lo);
    }
}

void xn_imul1(u32 v, u32 size)
{
    u32 hi, lo;
    s32 p;
    if (size == 0) {
        p = (s32)(s8)AL * (s32)(s8)v;
        AX = p;
        LF(XF_MUL | 0, 0, p != (s8)p, p);
    } else if (size == 1) {
        p = (s32)(s16)AX * (s32)(s16)v;
        AX = p;
        DX = (u32)p >> 16;
        LF(XF_MUL | 1, 0, p != (s16)p, p);
    } else {
        lo = xn_muls(R.eax, v, &hi);
        R.eax = lo;
        R.edx = hi;
        LF(XF_MUL | 2, 0, hi != (u32)((s32)lo >> 31), lo);
    }
}

u32 xn_imul2(u32 a, u32 b, u32 size)
{
    u32 hi, lo;
    s32 p;
    if (size == 1) {
        p = (s32)(s16)a * (s32)(s16)b;
        LF(XF_MUL | 1, 0, p != (s16)p, p);
        return p & 0xFFFF;
    }
    lo = xn_muls(a, b, &hi);
    LF(XF_MUL | 2, 0, hi != (u32)((s32)lo >> 31), lo);
    return lo;
}

/* -- divides ---------------------------------------------------------------------------- */

u32 xn_udiv(u32 lo, u32 hi, u32 d, u32 *rem);
#pragma aux xn_udiv = "div ecx" "mov [ebx], edx" parm [eax] [edx] [ecx] [ebx] value [eax] modify [edx];

/* The divide error: the program's handler (0x149FC8) skips the instruction by its ModRM byte
   (6, 3 or 2 bytes) and returns with EAX = EDX = 0. xn_divfault takes a real one with the
   asm's registers and stack, so the handler sees the same frame; its return address there is
   then made the original instruction's. */
u32 xn_faulted;

static void fault(u32 va)
{
    u32 m = M8(XN(va) + 1), skip = 2;
    if (m == 0x3D || m == 0x35 || (m >= 0xB0 && m <= 0xBF && m != 0xB4 && m != 0xBC))
        skip = 6;
    else if (m >= 0x70 && m <= 0x7F && m != 0x74 && m != 0x7C)
        skip = 3;
    xn_divfault();
    M32(R.esp - 0x220 + 12) = XN(va) + skip;
    xn_faulted = 1;
}

void xn_div(u32 d, u32 size, u32 va)
{
    u32 q, r;
    if (size == 0) {
        d &= 0xFF;
        if (d == 0 || AX / d > 0xFF) { fault(va); return; }
        q = AX / d; r = AX % d;
        AL = q; AH = r;
    } else if (size == 1) {
        u32 n = (u32)DX << 16 | AX;
        d &= 0xFFFF;
        if (d == 0 || n / d > 0xFFFF) { fault(va); return; }
        AX = n / d; DX = n % d;
    } else {
        if (R.edx >= d) { fault(va); return; }     /* d == 0 included */
        q = xn_udiv(R.eax, R.edx, d, &r);
        R.eax = q; R.edx = r;
    }
}

void xn_idiv(u32 d, u32 size, u32 va)
{
    if (size == 0) {
        s32 n = (s16)AX, v = (s8)d, q;
        if (v == 0) { fault(va); return; }
        q = n / v;
        if (q != (s8)q) { fault(va); return; }
        AL = q; AH = n % v;
    } else if (size == 1) {
        s32 n = (s32)((u32)DX << 16 | AX), v = (s16)d, q;
        if (v == 0 || (n == (s32)0x80000000 && v == -1)) { fault(va); return; }
        q = n / v;
        if (q != (s16)q) { fault(va); return; }
        AX = q; DX = n % v;
    } else {
        u32 lo = R.eax, hi = R.edx, ud = d, q, r;
        int neg = (s32)hi < 0, dneg = (s32)d < 0;
        if (neg) { lo = -lo; hi = ~hi + (lo == 0); }
        if (dneg) ud = -d;
        if (ud == 0 || hi >= ud) { fault(va); return; }
        q = xn_udiv(lo, hi, ud, &r);
        if (neg != dneg) {
            if (q > 0x80000000u) { fault(va); return; }
            q = -q;
        } else if (q > 0x7FFFFFFFu) { fault(va); return; }
        R.eax = q;
        R.edx = neg ? -r : r;
    }
}

/* -- strings ---------------------------------------------------------------------------- */

#define STEP(n) ((R.eflags & 0x400) ? -(s32)(n) : (s32)(n))

void xn_movs(u32 size, u32 rep)
{
    s32 d = STEP(1 << size);
    if (size == 2) d = STEP(4);
    do {
        if (rep) { if (R.ecx == 0) break; R.ecx--; }
        if (size == 0) M8(R.edi) = M8(R.esi);
        else if (size == 1) M16(R.edi) = M16(R.esi);
        else M32(R.edi) = M32(R.esi);
        R.esi += d; R.edi += d;
    } while (rep);
}

void xn_stos(u32 size, u32 rep)
{
    s32 d = size == 2 ? STEP(4) : STEP(1 << size);
    do {
        if (rep) { if (R.ecx == 0) break; R.ecx--; }
        if (size == 0) M8(R.edi) = AL;
        else if (size == 1) M16(R.edi) = AX;
        else M32(R.edi) = R.eax;
        R.edi += d;
    } while (rep);
}

void xn_lods(u32 size, u32 rep)
{
    s32 d = size == 2 ? STEP(4) : STEP(1 << size);
    do {
        if (rep) { if (R.ecx == 0) break; R.ecx--; }
        if (size == 0) AL = M8(R.esi);
        else if (size == 1) AX = M16(R.esi);
        else R.eax = M32(R.esi);
        R.esi += d;
    } while (rep);
}

static u32 load(u32 a, u32 size)
{
    return size == 0 ? M8(a) : size == 1 ? M16(a) : M32(a);
}

void xn_scas(u32 size, u32 rep)
{
    s32 d = size == 2 ? STEP(4) : STEP(1 << size);
    u32 a = size == 0 ? AL : size == 1 ? AX : R.eax, b;
    do {
        if (rep) { if (R.ecx == 0) break; R.ecx--; }
        b = load(R.edi, size);
        LF(XF_SUB | size, a, b, a - b);
        R.edi += d;
    } while (rep && ((F.r & mask[size]) == 0) == (rep == 1));
}

void xn_cmps(u32 size, u32 rep)
{
    s32 d = size == 2 ? STEP(4) : STEP(1 << size);
    u32 a, b;
    do {
        if (rep) { if (R.ecx == 0) break; R.ecx--; }
        a = load(R.esi, size);
        b = load(R.edi, size);
        LF(XF_SUB | size, a, b, a - b);
        R.esi += d; R.edi += d;
    } while (rep && ((F.r & mask[size]) == 0) == (rep == 1));
}

void xn_ins(u32 size, u32 rep)
{
    s32 d = size == 2 ? STEP(4) : STEP(1 << size);
    do {
        if (rep) { if (R.ecx == 0) break; R.ecx--; }
        if (size == 0) M8(R.edi) = xn_in8(DX);
        else if (size == 1) M16(R.edi) = xn_in16(DX);
        else M32(R.edi) = xn_in32(DX);
        R.edi += d;
    } while (rep);
}

void xn_outs(u32 size, u32 rep)
{
    s32 d = size == 2 ? STEP(4) : STEP(1 << size);
    do {
        if (rep) { if (R.ecx == 0) break; R.ecx--; }
        if (size == 0) xn_out8(DX, M8(R.esi));
        else if (size == 1) xn_out16(DX, M16(R.esi));
        else xn_out32(DX, M32(R.esi));
        R.esi += d;
    } while (rep);
}
