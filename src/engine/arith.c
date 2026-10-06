/* arith.c: 64-bit division without the divide error (canonical C; declared in xngine.h).

   XnGine's asm divides 64-bit values with `idiv` and `div` and lets its own divide-error
   handler (xn_sys_divide_error_handler, installed through DPMI) take any quotient that does
   not fit in 32 bits or a divisor of 0: the handler steps over the divide and leaves EAX = EDX
   = 0, so the quotient and the remainder both come out 0 (docs/engine/quirks.md Q-SYS-01).
   The functions here give those results without raising the exception: they check whether
   the divide would fault, and divide only when it would not. */
#include "xngine.h"

int xn_s64_div_fits(const xn_s64 *n, s32 d)
{
    u32 hi = (u32)n->hi, lo = n->lo, dm, k;
    xn_s64 lim;

    if (d == 0)
        return 0;
    if (n->hi < 0) {                    /* |n|, as an unsigned 64-bit value */
        lo = 0u - lo;
        hi = ~hi + (lo == 0);
    }
    dm = d < 0 ? 0u - (u32)d : (u32)d;
    /* idiv faults unless the truncated |n / d| is at most 2^31 - 1 (a positive quotient) or
       2^31 (a negative one): it fits when |n| < (that + 1) * |d| */
    k = (n->hi < 0) != (d < 0) ? 0x80000001u : 0x80000000u;
    xn_u64_mul(&lim, k, dm);
    return hi < (u32)lim.hi || hi == (u32)lim.hi && lo < lim.lo;
}

s32 xn_s64_div_or0(const xn_s64 *n, s32 d)
{
    return xn_s64_div_fits(n, d) ? xn_s64_div(n, d) : 0;
}

s32 xn_s64_divrem_or0(const xn_s64 *n, s32 d, s32 *rem)
{
    if (!xn_s64_div_fits(n, d)) {
        *rem = 0;
        return 0;
    }
    return xn_s64_divrem(n, d, rem);
}

u32 xn_u64_div_or0(const xn_s64 *n, u32 d)
{
    /* div faults when d is 0 or the quotient needs more than 32 bits (hi >= d) */
    return d != 0 && (u32)n->hi < d ? xn_u64_div(n, d) : 0;
}

u32 xn_u64_divrem_or0(const xn_s64 *n, u32 d, u32 *rem)
{
    if (d == 0 || (u32)n->hi >= d) {
        *rem = 0;
        return 0;
    }
    return xn_u64_divrem(n, d, rem);
}
