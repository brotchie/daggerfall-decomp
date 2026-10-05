/* pipe.c: helpers of the 3D pipeline's readable C (xpipe.h); candidates for xngine.h. */
#include "xpipe.h"

u32 xn_add_flags(s32 a, s32 b)
{
    s32 sum = (s32)((u32)a + (u32)b);
    u32 f = 0;

    if (sum == 0)
        f |= XN_ZF;
    if (sum < 0)
        f |= XN_SF;
    if (((a ^ sum) & (b ^ sum)) < 0)
        f |= XN_OF;
    return f;
}

void xn_call_asm(xn_routine fn)
{
    xn_regs r;

    r.eax = r.ecx = r.edx = r.ebx = r.ebp = r.esi = r.edi = 0;
    xn_asmcall(fn, &r);
}

s32 xn_call_light_setup(xn_routine fn, struct xn_poly *poly, xn_regs *r)
{
    xn_regs lr = *r;

    lr.edi = (u32)poly;
    xn_asmcall(fn, &lr);
    r->edx = lr.edx;
    return lr.eax;
}

void xn_run_span(xn_routine fn, xn_regs *r)
{
    r->ecx = (u32)fn;
    xn_asmcall(fn, r);
}
