/* pipe.c: the 3D pipeline's shared helpers (canonical C; the interface and the module's
   documentation are in xpipe.h). */
#include "xpipe.h"

u32 xn_umod64_or0(u32 hi, u32 lo, u32 d)
{
    xn_s64 n;
    u32 rem;

    n.lo = lo;
    n.hi = (s32)hi;
    xn_u64_divrem_or0(&n, d, &rem);
    return rem;
}

s32 xn_muldiv_or0(s32 a, s32 b, s32 d)
{
    xn_s64 n;

    xn_s64_mul(&n, a, b);
    return xn_s64_div_or0(&n, d);
}
