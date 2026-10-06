/* colm_t.c: test shims of src/engine/colmodel.c (built only by tools/xn_rc.py; see
   src/engine_test/vec_t.c and docs/xngine_canonical.md). Each NAME_r maps the registers of
   NAME's asm entry (config/xngine_abi.csv) to the canonical C call. */
#include "xcollide.h"

/* the handle EAX, start EDX, end EBX, the mode ECX (Watcom's order) */
void xn_collide_segment_model_r(xn_regs *r)
{
    r->eax = xn_collide_segment_model((struct xn_model_handle *)r->eax, (const xn_vec3 *)r->edx,
                                      (const xn_vec3 *)r->ebx, r->ecx);
}

void xn_collide_model_model_r(xn_regs *r)
{
    r->eax = xn_collide_model_model((struct xn_model_handle *)r->eax,
                                    (struct xn_model_handle *)r->edx, r->ebx);
}

/* the handle EAX, the probe EDX, the mode EBX */
void xn_collide_spheres_model_r(xn_regs *r)
{
    r->eax = xn_collide_spheres_model((struct xn_model_handle *)r->eax,
                                      (struct xn_collide_probe *)r->edx, r->ebx);
}

void xn_collide_segment_spheres_r(xn_regs *r)
{
    r->eax = xn_collide_segment_spheres((struct xn_collide_probe *)r->eax,
                                        (const xn_vec3 *)r->edx, (const xn_vec3 *)r->ebx);
}

void xn_collide_spheres_spheres_r(xn_regs *r)
{
    r->eax = xn_collide_spheres_spheres((struct xn_collide_probe *)r->eax,
                                        (struct xn_collide_probe *)r->edx);
}

/* its two arguments on the stack (the stub pops them) */
void xn_collide_miss_stk_r(xn_regs *r)
{
    r->eax = xn_collide_miss_stk(XN_STACK_ARG(r, 0), XN_STACK_ARG(r, 1));
}

void xn_collide_miss_r(xn_regs *r)
{
    r->eax = xn_collide_miss();
}

/* pos EAX, start EDX, end EBX, the image ECX; the flags, the scale and the mode on the stack */
void xn_collide_segment_flat_stk_r(xn_regs *r)
{
    r->eax = xn_collide_segment_flat_stk((const xn_vec3 *)r->eax, (const xn_vec3 *)r->edx,
                                         (const xn_vec3 *)r->ebx, r->ecx, XN_STACK_ARG(r, 0),
                                         XN_STACK_ARG(r, 1), XN_STACK_ARG(r, 2));
}

/* the flags ESI, the scale EDI, the mode EBP */
void xn_collide_segment_flat_r(xn_regs *r)
{
    r->eax = xn_collide_segment_flat((const xn_vec3 *)r->eax, (const xn_vec3 *)r->edx,
                                     (const xn_vec3 *)r->ebx, r->ecx, r->esi, r->edi, r->ebp);
}

/* the model EAX, r EDX, out EBX -> the bytes EAX, the count EDX */
void xn_collide_build_model_spheres_r(xn_regs *r)
{
    s32 count;

    r->eax = xn_collide_build_model_spheres((struct xn_model *)r->eax, r->edx, (u8 *)r->ebx,
                                            &count);
    if ((s32)r->eax >= 0)
        r->edx = count;
}
