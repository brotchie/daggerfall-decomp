/* xngine.h: what the readable C of XnGine (FALL.EXE object 2) shares.

   The readable C is compiled by Watcom C32 10.0a (tools/xn_rc.py build) and runs inside the
   game on the game's own stack. Each function keeps the register interface its asm callers
   use (config/xngine_abi.csv): a `#pragma aux` gives the registers its parameters arrive in,
   the register its result leaves in, and the registers it may change (the row's clobber set:
   no caller reads them after the call). A function with no pragma has Watcom's own interface:
   arguments in EAX EDX EBX ECX, then the stack (the callee pops it); the result in EAX; EAX and
   the argument registers changed, the rest preserved.

   Write `modify exact [...]` (the clobbers and the value register): without `exact`, Watcom
   10.0a does not preserve a function's parameter registers, and asm callers may rely on them.
   Watcom's code never keeps EAX: when callers need it kept, list it anyway, and the build
   routes the asm entry through a stub (push eax; call; pop eax; ret N).

   A function whose asm callers read more than one register, or flags, after the call has a
   natural C version (results through pointers) and a glue function NAME_r(xn_regs *r) that
   unpacks the asm caller's registers into a call of it and packs the results back. The asm
   entry is then sent to a stub (pushfd; pushad; call NAME_r; popad; popfd; ret N) that tools/
   xn_rc.py writes.

   Data stays in object 2: the C declares the globals it uses as externs under their names in
   config/names.csv, and the linker resolves them to their addresses in the loaded game (an
   unnamed address is xn_data_XXXXXX). A function the C calls that is still asm is declared with
   the pragma of its interface and resolves to its asm entry; `asm_NAME` is always the asm
   entry, even once NAME is C. 8.3 file names: the compiler runs under DOSBox-X.

   Canonical modules (docs/xngine_canonical.md; vec, mat and math so far) have none of that:
   plain prototypes with Watcom's own convention, no pragma, no NAME_r in src/engine. Their
   asm interfaces live in test shims (src/engine_test/<subsys>_t.c), which only the tools
   build in; the game reaches them through boundary routes the build generates
   (config/xngine_boundary.csv). The inline helpers below are compiler support (Watcom
   C32 10.0a has no 64-bit integers), not interfaces. */
#ifndef XNGINE_H
#define XNGINE_H

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef signed char s8;
typedef short s16;
typedef int s32;

/* iptr / uptr: an int that holds an address (int under Watcom, pointer-wide natively) */
#include "ptrint.h"

/* ---- common types ---------------------------------------------------------------------- */

/* A vector or point: world units, or fixed point (the function says which: 16.16, 2.28...) */
typedef struct xn_vec3 {
    s32 x, y, z;
} xn_vec3;

/* A 3x3 rotation, rows of 2.28 fixed point (1.0 = 0x10000000): m[row][column] */
typedef struct xn_mat3 {
    s32 m[3][3];
} xn_mat3;

/* Angles: 2048 steps to a turn (0x800); sine and cosine tables in 2.28 */
#define XN_ANGLES       2048
#define XN_ANGLE_MASK   0x7FF
#define XN_ONE28        0x10000000      /* 1.0 in 2.28 */
/* sine of k/2048 of a turn, 2.28, for k = 0..2559: one table, and the cosine is the same table
   a quarter turn on (0x150200; the cosine's name is 0x150A00) */
extern s32 xn_sin_table[XN_ANGLES + XN_ANGLES / 4];
#define xn_cos_table (xn_sin_table + XN_ANGLES / 4)

/* ---- an asm caller's registers ----------------------------------------------------------- */

/* As a stub saves them (pushfd; pushad) for a glue function, and as xn_asmcall loads them */
#ifdef DAGGER_PORT
/* The native build (docs/port.md): pointer-wide registers, the layout of the virtual PC's
   struct vpc_regs (port/include/port_vpc.h): a DOS call passes a buffer's address in one */
typedef struct xn_regs {
    unsigned long edi, esi, ebp, esp, ebx, edx, ecx, eax;
    unsigned long eflags;
} xn_regs;
#else
typedef struct xn_regs {
    u32 edi, esi, ebp, esp, ebx, edx, ecx, eax;
    u32 eflags;
} xn_regs;
#endif

#define XN_CF   0x0001
#define XN_PF   0x0004
#define XN_AF   0x0010
#define XN_ZF   0x0040
#define XN_SF   0x0080
#define XN_OF   0x0800
#define XN_DF   0x0400
/* set (on != 0) or clear flag bits in a saved EFLAGS */
#define XN_SETFLAG(r, f, on) ((r)->eflags = ((r)->eflags & ~(u32)(f)) | ((on) ? (u32)(f) : 0))
/* the k-th dword argument on an asm caller's stack (above its return address) */
#define XN_STACK_ARG(r, k) (((u32 *)((r)->esp + 8))[k])

/* Call asm code with a register file: loads EAX..EBP from r, calls target, stores them back
   (with EFLAGS). For an asm function with several outputs: xn_asmcall(asm_NAME, &r). */
void xn_asmcall(void (*target)(void), xn_regs *r);

/* ---- fixed point ------------------------------------------------------------------------- */
/* Watcom C32 10.0a has no 64-bit integer type: products and quotients that need 64 bits are
   one-instruction inline functions. */

/* A signed 64-bit value, for sums of products */
typedef struct xn_s64 {
    u32 lo;
    s32 hi;
} xn_s64;

#ifndef DAGGER_PORT
/* (a * b) >> 28 of the 64-bit product: 2.28 times anything (imul; shrd 28 / shld 4) */
s32 xn_fixmul28(s32 a, s32 b);
#pragma aux xn_fixmul28 = "imul edx" "shrd eax, edx, 28" parm [eax] [edx] value [eax] modify [edx];

/* (a * b + 2^27) >> 28: rounded */
s32 xn_fixmul28r(s32 a, s32 b);
#pragma aux xn_fixmul28r = "imul edx" "add eax, 08000000h" "adc edx, 0" "shrd eax, edx, 28" \
    parm [eax] [edx] value [eax] modify [edx];

/* (a * b) >> 32: the high dword of the product */
s32 xn_mulhi(s32 a, s32 b);
#pragma aux xn_mulhi = "imul edx" parm [eax] [edx] value [edx] modify [eax];

/* (a * b) >> n for 0 <= n < 32 */
s32 xn_mulshr(s32 a, s32 b, u32 n);
#pragma aux xn_mulshr = "imul edx" "shrd eax, edx, cl" parm [eax] [edx] [ecx] value [eax] \
    modify [edx];

/* (a * b) / d with a 64-bit product (a divide error when the quotient does not fit) */
s32 xn_muldiv(s32 a, s32 b, s32 d);
#pragma aux xn_muldiv = "imul edx" "idiv ebx" parm [eax] [edx] [ebx] value [eax] modify [edx];



/* *r = v, sign-extended */
void xn_s64_set(xn_s64 *r, s32 v);
#pragma aux xn_s64_set = "cdq" "mov [ebx], eax" "mov [ebx+4], edx" parm [ebx] [eax] modify [edx];

/* *r = a * b */
void xn_s64_mul(xn_s64 *r, s32 a, s32 b);
#pragma aux xn_s64_mul = "imul edx" "mov [ebx], eax" "mov [ebx+4], edx" parm [ebx] [eax] [edx] \
    modify [eax edx];

/* *r += a * b */
void xn_s64_mac(xn_s64 *r, s32 a, s32 b);
#pragma aux xn_s64_mac = "imul edx" "add [ebx], eax" "adc [ebx+4], edx" parm [ebx] [eax] [edx] \
    modify [eax edx];

/* *r -= a * b */
void xn_s64_msub(xn_s64 *r, s32 a, s32 b);
#pragma aux xn_s64_msub = "imul edx" "sub [ebx], eax" "sbb [ebx+4], edx" parm [ebx] [eax] [edx] \
    modify [eax edx];

/* *r = a * b, unsigned */
void xn_u64_mul(xn_s64 *r, u32 a, u32 b);
#pragma aux xn_u64_mul = "mul edx" "mov [ebx], eax" "mov [ebx+4], edx" parm [ebx] [eax] [edx] \
    modify [eax edx];

/* *r += v (v as an unsigned 32-bit value: the rounding constants) */
void xn_s64_addu(xn_s64 *r, u32 v);
#pragma aux xn_s64_addu = "add [ebx], eax" "adc dword ptr [ebx+4], 0" parm [ebx] [eax];

/* *r <<= n for 0 <= n < 32 */
void xn_s64_shl(xn_s64 *r, u32 n);
#pragma aux xn_s64_shl = "mov eax, [ebx]" "mov edx, [ebx+4]" "shld edx, eax, cl" "shl eax, cl" \
    "mov [ebx], eax" "mov [ebx+4], edx" parm [ebx] [ecx] modify [eax edx];

/* bits n..n+31 of *r (shrd), 0 <= n < 32 */
s32 xn_s64_shr(const xn_s64 *r, u32 n);
#pragma aux xn_s64_shr = "mov eax, [ebx]" "mov edx, [ebx+4]" "shrd eax, edx, cl" parm [ebx] [ecx] \
    value [eax] modify [edx];

/* *r / d, truncated (idiv: a divide error when the quotient does not fit in 32 bits) */
s32 xn_s64_div(const xn_s64 *r, s32 d);
#pragma aux xn_s64_div = "mov eax, [ebx]" "mov edx, [ebx+4]" "idiv ecx" parm [ebx] [ecx] \
    value [eax] modify [edx];

/* *r / d, truncated, and the remainder to *rem (both 0 after a divide error) */
s32 xn_s64_divrem(const xn_s64 *r, s32 d, s32 *rem);
#pragma aux xn_s64_divrem = "mov eax, [ebx]" "mov edx, [ebx+4]" "idiv ecx" "mov [esi], edx" \
    parm [ebx] [ecx] [esi] value [eax] modify [edx];

/* *r / d, unsigned, and the remainder to *rem */
u32 xn_u64_divrem(const xn_s64 *r, u32 d, u32 *rem);
#pragma aux xn_u64_divrem = "mov eax, [ebx]" "mov edx, [ebx+4]" "div ecx" "mov [esi], edx" \
    parm [ebx] [ecx] [esi] value [eax] modify [edx];

/* *r / d, unsigned (div) */
u32 xn_u64_div(const xn_s64 *r, u32 d);
#pragma aux xn_u64_div = "mov eax, [ebx]" "mov edx, [ebx+4]" "div ecx" parm [ebx] [ecx] \
    value [eax] modify [edx];

#else
/* The native build: the same arithmetic in C. Where the asm's divide would fault (a divisor
   of 0, a quotient that does not fit), the result is XnGine's handler's: 0, and a remainder of
   0 (docs/engine/quirks.md Q-SYS-01). Shift counts are taken mod 32, as the CPU takes CL. */
#include <stdint.h>

static __inline__ int64_t xn__s64_get(const xn_s64 *r)
{
    return (int64_t)((uint64_t)(u32)r->hi << 32 | r->lo);
}

static __inline__ void xn__s64_put(xn_s64 *r, uint64_t v)
{
    r->lo = (u32)v;
    r->hi = (s32)(u32)(v >> 32);
}

static __inline__ s32 xn_fixmul28(s32 a, s32 b) { return (s32)(((int64_t)a * b) >> 28); }
static __inline__ s32 xn_fixmul28r(s32 a, s32 b)
{
    return (s32)((int64_t)((uint64_t)((int64_t)a * b) + 0x08000000u) >> 28);
}
static __inline__ s32 xn_mulhi(s32 a, s32 b) { return (s32)(((int64_t)a * b) >> 32); }
static __inline__ s32 xn_mulshr(s32 a, s32 b, u32 n) { return (s32)(((int64_t)a * b) >> (n & 31)); }

/* idiv of a 64-bit value: 0 where it would fault */
static __inline__ int xn__idiv(int64_t n, s32 d, s32 *q, s32 *r)
{
    int64_t qq;
    if (d == 0 || (n == INT64_MIN && d == -1))
        return 0;
    qq = n / d;
    if (qq < INT32_MIN || qq > INT32_MAX)
        return 0;
    *q = (s32)qq;
    *r = (s32)(n % d);
    return 1;
}

static __inline__ int xn__div(uint64_t n, u32 d, u32 *q, u32 *r)
{
    if (d == 0 || n / d > 0xFFFFFFFFu)
        return 0;
    *q = (u32)(n / d);
    *r = (u32)(n % d);
    return 1;
}

static __inline__ s32 xn_muldiv(s32 a, s32 b, s32 d)
{
    s32 q, r;
    return xn__idiv((int64_t)a * b, d, &q, &r) ? q : 0;
}

static __inline__ void xn_s64_set(xn_s64 *r, s32 v) { xn__s64_put(r, (uint64_t)(int64_t)v); }
static __inline__ void xn_s64_mul(xn_s64 *r, s32 a, s32 b) { xn__s64_put(r, (uint64_t)((int64_t)a * b)); }
static __inline__ void xn_s64_mac(xn_s64 *r, s32 a, s32 b)
{
    xn__s64_put(r, (uint64_t)xn__s64_get(r) + (uint64_t)((int64_t)a * b));
}
static __inline__ void xn_s64_msub(xn_s64 *r, s32 a, s32 b)
{
    xn__s64_put(r, (uint64_t)xn__s64_get(r) - (uint64_t)((int64_t)a * b));
}
static __inline__ void xn_u64_mul(xn_s64 *r, u32 a, u32 b) { xn__s64_put(r, (uint64_t)a * b); }
static __inline__ void xn_s64_addu(xn_s64 *r, u32 v) { xn__s64_put(r, (uint64_t)xn__s64_get(r) + v); }
static __inline__ void xn_s64_shl(xn_s64 *r, u32 n) { xn__s64_put(r, (uint64_t)xn__s64_get(r) << (n & 31)); }
static __inline__ s32 xn_s64_shr(const xn_s64 *r, u32 n) { return (s32)(u32)((uint64_t)xn__s64_get(r) >> (n & 31)); }
static __inline__ s32 xn_s64_div(const xn_s64 *r, s32 d)
{
    s32 q, rem;
    return xn__idiv(xn__s64_get(r), d, &q, &rem) ? q : 0;
}
static __inline__ s32 xn_s64_divrem(const xn_s64 *r, s32 d, s32 *rem)
{
    s32 q;
    if (!xn__idiv(xn__s64_get(r), d, &q, rem)) {
        *rem = 0;
        return 0;
    }
    return q;
}
static __inline__ u32 xn_u64_divrem(const xn_s64 *r, u32 d, u32 *rem)
{
    u32 q;
    if (!xn__div((uint64_t)xn__s64_get(r), d, &q, rem)) {
        *rem = 0;
        return 0;
    }
    return q;
}
static __inline__ u32 xn_u64_div(const xn_s64 *r, u32 d)
{
    u32 q, rem;
    return xn__div((uint64_t)xn__s64_get(r), d, &q, &rem) ? q : 0;
}

#endif

/* A divide that overflows (or divides by 0) raises a divide error. XnGine's own handler
   (xn_sys_divide_error_handler) steps over the instruction and makes EAX and EDX 0, and the
   emulator logs the exception: the C divides where the asm did, with these helpers (a
   register operand: the handler steps over its 2 bytes), never with C's `/` where a divide
   can fail. */

/* Canonical C does not raise the exception (src/engine/arith.c): the same results, the
   quotient and the remainder 0 when the asm's divide would fault (docs/engine/quirks.md
   Q-SYS-01). */
int xn_s64_div_fits(const xn_s64 *n, s32 d);            /* 1 when idiv would not fault */
s32 xn_s64_div_or0(const xn_s64 *n, s32 d);             /* n / d, truncated, or 0 */
s32 xn_s64_divrem_or0(const xn_s64 *n, s32 d, s32 *rem);
u32 xn_u64_div_or0(const xn_s64 *n, u32 d);             /* unsigned n / d, or 0 */
u32 xn_u64_divrem_or0(const xn_s64 *n, u32 d, u32 *rem);

/* ---- bits ------------------------------------------------------------------------------- */
/* The index of the highest (bsr) or lowest (bsf) set bit of v; for v = 0, `old` (the CPU
   leaves the destination register as it was) */
#ifndef DAGGER_PORT
s32 xn_bsr(u32 v, s32 old);
#pragma aux xn_bsr = "bsr eax, edx" parm [edx] [eax] value [eax];
s32 xn_bsf(u32 v, s32 old);
#pragma aux xn_bsf = "bsf eax, edx" parm [edx] [eax] value [eax];
#else
static __inline__ s32 xn_bsr(u32 v, s32 old) { return v ? 31 - __builtin_clz(v) : old; }
static __inline__ s32 xn_bsf(u32 v, s32 old) { return v ? __builtin_ctz(v) : old; }
#endif

/* ---- ports and interrupts ----------------------------------------------------------------- */
u8 xn_inb(u32 port);
#pragma aux xn_inb = "in al, dx" parm [edx] value [al];
void xn_outb(u32 port, u8 v);
#pragma aux xn_outb = "out dx, al" parm [edx] [al];
u16 xn_inw(u32 port);
#pragma aux xn_inw = "in ax, dx" parm [edx] value [ax];
void xn_outw(u32 port, u16 v);
#pragma aux xn_outw = "out dx, ax" parm [edx] [ax];

/* int n with a register file (EAX..EBP in and out, EFLAGS out: CF is the DOS/DPMI status) */
void xn_int10(xn_regs *r);
void xn_int15(xn_regs *r);
void xn_int16(xn_regs *r);
void xn_int21(xn_regs *r);
void xn_int2f(xn_regs *r);
void xn_int31(xn_regs *r);
void xn_int33(xn_regs *r);
/* glue.asm takes r in EDX and returns with EAX and EDX changed (the rest restored) */
#pragma aux xn_int10 parm [edx] modify [eax edx];
#pragma aux xn_int15 parm [edx] modify [eax edx];
#pragma aux xn_int16 parm [edx] modify [eax edx];
#pragma aux xn_int21 parm [edx] modify [eax edx];
#pragma aux xn_int2f parm [edx] modify [eax edx];
#pragma aux xn_int31 parm [edx] modify [eax edx];
#pragma aux xn_int33 parm [edx] modify [eax edx];

#endif
