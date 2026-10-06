/* smc.c: the rasteriser's arithmetic helpers (xnsmc.h). */
#include "xnsmc.h"

u32 xn_udiv64_or0(u32 hi, u32 lo, u32 d)
{
    xn_s64 n;

    n.lo = lo;
    n.hi = (s32)hi;
    return xn_u64_div_or0(&n, d);
}

s32 xn_idiv64_or0(s32 hi, u32 lo, s32 d)
{
    xn_s64 n;

    n.lo = lo;
    n.hi = hi;
    return xn_s64_div_or0(&n, d);
}

int xn_add_lt0(s32 a, s32 b)
{
    /* floor((a + b) / 2), which never overflows, has the exact sum's sign */
    return (a >> 1) + (b >> 1) + (a & b & 1) < 0;
}
