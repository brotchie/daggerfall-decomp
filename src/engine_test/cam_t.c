/* cam_t.c: test shims of src/engine/cam.c (built only by tools/xn_rc.py; see
   src/engine_test/vec_t.c and docs/xngine_canonical.md). Each NAME_r maps the registers of
   NAME's asm entry (config/xngine_abi.csv) to the canonical C call and back. */
#include "xcam.h"

/* the flags an asm `add` leaves (ZF, SF, OF) for a + b */
static u32 add_flags(s32 a, s32 b)
{
    s32 s = (s32)((u32)a + (u32)b);
    u32 f = 0;

    if (s == 0)
        f |= XN_ZF;
    if (s < 0)
        f |= XN_SF;
    if (((a ^ s) & (b ^ s)) < 0)
        f |= XN_OF;
    return f;
}

void xn_cam_set_focal_r(xn_regs *r)
{
    xn_cam_set_focal(r->eax, r->edx);
}

void xn_cam_set_view_window_r(xn_regs *r)
{
    xn_cam_set_view_window(r->eax, r->edx, r->ebx, r->ecx);
}

void xn_cam_update_derived_r(xn_regs *r)
{
    (void)r;
    xn_cam_update_derived();
}

/* (x, y, z) in EAX EDX EBX, sx and sy pointers in ECX and on the stack */
void xn_cam_project_ptr_r(xn_regs *r)
{
    xn_cam_project_ptr(r->eax, r->edx, r->ebx, (s32 *)r->ecx, (s32 *)XN_STACK_ARG(r, 0));
}

/* (x, y, z) in EAX EDX EBX -> sx EAX, sy EDX */
void xn_cam_project_r(xn_regs *r)
{
    s32 sx, sy;

    xn_cam_project(r->eax, r->edx, r->ebx, &sx, &sy);
    r->eax = sx;
    r->edx = sy;
}

void xn_cam_project_scaled_r(xn_regs *r)
{
    s32 sx, sy;

    xn_cam_project_scaled(r->eax, r->edx, r->ebx, &sx, &sy);
    r->eax = sx;
    r->edx = sy;
}

void xn_cam_scale_matrix_r(xn_regs *r)
{
    xn_cam_scale_matrix((const xn_mat3 *)r->eax, (xn_mat3 *)r->edx);
}

void xn_cam_scale_matrix_in_place_r(xn_regs *r)
{
    xn_cam_scale_matrix_in_place((xn_mat3 *)r->eax);
}

void xn_cam_unscale_matrix_r(xn_regs *r)
{
    xn_cam_unscale_matrix((xn_mat3 *)r->eax);
}

void xn_cam_forward_angles_r(xn_regs *r)
{
    xn_cam_forward_angles((s32 *)r->eax, (s32 *)r->edx);
}

/* (x, y, z) EAX EDX EBX, radius ECX -> the residue in EAX, CF when culled */
void xn_cam_cull_sphere_r(xn_regs *r)
{
    s32 residue;
    int culled = xn_cam_cull_sphere(r->eax, r->edx, r->ebx, r->ecx, &residue);

    r->eax = residue;
    XN_SETFLAG(r, XN_CF, culled);
}

/* the normal's (nx, nz) in EAX EBX, the radius in EDI -> the low product in EAX; the flags of
   distance + radius (the caller's jle) */
void xn_cam_sphere_dist_x_r(xn_regs *r)
{
    s32 low, d = xn_cam_sphere_dist_x(r->eax, r->ebx, &low);

    r->eax = low;
    r->eflags = (r->eflags & ~(u32)(XN_ZF | XN_SF | XN_OF)) | add_flags(d, r->edi);
}

void xn_cam_sphere_dist_y_r(xn_regs *r)
{
    s32 low, d = xn_cam_sphere_dist_y(r->eax, r->ebx, &low);

    r->eax = low;
    r->eflags = (r->eflags & ~(u32)(XN_ZF | XN_SF | XN_OF)) | add_flags(d, r->edi);
}
