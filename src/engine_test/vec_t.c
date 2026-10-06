/* vec_t.c: test shims of src/engine/vec.c (built only by tools/xn_rc.py; see
   docs/xngine_canonical.md). Each NAME_r maps the registers of NAME's asm entry, as
   config/xngine_abi.csv gives them, to the canonical C call and back: a record test routes
   the asm entry here (kind shim), and compares as the ABI row says, with
   config/xngine_dropped.csv's excuses. */
#include "xvec.h"

static void get3(const xn_regs *r, xn_vec3 *v)
{
    v->x = r->eax;
    v->y = r->edx;
    v->z = r->ebx;
}

static void put3(const xn_vec3 *v, xn_regs *r)
{
    r->eax = v->x;
    r->edx = v->y;
    r->ebx = v->z;
}

/* Watcom's convention: (EAX, EDX, EBX) -> EAX */
void xn_vec_unit_direction_r(xn_regs *r)
{
    r->eax = (u32)xn_vec_unit_direction((const xn_vec3 *)r->eax, (const xn_vec3 *)r->edx,
                                        (xn_vec3 *)r->ebx);
}

void xn_vec_advance_r(xn_regs *r)
{
    r->eax = (u32)xn_vec_advance((const xn_vec3 *)r->eax, r->edx, (xn_vec3 *)r->ebx);
}

void xn_vec_dir_to_angles_r(xn_regs *r)
{
    xn_vec_dir_to_angles((xn_vec3 *)r->eax);
}

/* (x, y, z) in EAX EDX EBX -> pitch EAX, yaw EDX */
void xn_vec_dir_to_angles_regs_r(xn_regs *r)
{
    s32 yaw;

    r->eax = xn_vec_dir_to_angles_regs(r->eax, r->edx, r->ebx, &yaw);
    r->edx = yaw;
}

void xn_vec_unit_to_angles_r(xn_regs *r)
{
    r->eax = xn_vec_unit_to_angles((xn_vec3 *)r->eax);
}

void xn_vec_unit_to_angles_regs_r(xn_regs *r)
{
    s32 yaw;

    r->eax = xn_vec_unit_to_angles_regs(r->eax, r->edx, r->ebx, &yaw);
    r->edx = yaw;
}

/* v in EAX EDX EBX, dist in ECX -> v; CF when dist >= 0x10000 */
void xn_vec_scale_unit14_r(xn_regs *r)
{
    xn_vec3 v;
    int ok;

    get3(r, &v);
    ok = xn_vec_scale_unit14(&v, r->ecx);
    put3(&v, r);
    XN_SETFLAG(r, XN_CF, !ok);
}

/* wa EAX, a EDX, wb EBX, b ECX -> EAX EDX EBX */
void xn_vec_blend_r(xn_regs *r)
{
    xn_vec3 v;

    xn_vec_blend(r->eax, (const xn_vec3 *)r->edx, r->ebx, (const xn_vec3 *)r->ecx, &v);
    put3(&v, r);
}

/* a EAX, b EDX -> EAX EDX EBX */
void xn_vec_cross_r(xn_regs *r)
{
    xn_vec3 c;

    xn_vec_cross((const xn_vec3 *)r->eax, (const xn_vec3 *)r->edx, &c);
    put3(&c, r);
}

void xn_vec_length_r(xn_regs *r)
{
    r->eax = xn_vec_length(r->eax, r->edx, r->ebx);
}

/* -> the sum in EAX, the y and z terms in EDX and EBX (where the asm left them) */
void xn_vec_length_approx_r(xn_regs *r)
{
    xn_vec3 t;

    xn_vec_length_approx_terms(r->eax, r->edx, r->ebx, &t);
    r->eax = t.x + t.y + t.z;
    r->edx = t.y;
    r->ebx = t.z;
}

void xn_vec_normalize_ptr_r(xn_regs *r)
{
    xn_vec_normalize_ptr((xn_vec3 *)r->eax);
}

void xn_vec_normalize_r(xn_regs *r)
{
    xn_vec3 v;

    get3(r, &v);
    xn_vec_normalize(&v);
    put3(&v, r);
}

void xn_vec_normalize_q28_r(xn_regs *r)
{
    xn_vec3 v;

    get3(r, &v);
    xn_vec_normalize_q28(&v);
    put3(&v, r);
}

/* the shift in CL */
void xn_vec_normalize_shift_r(xn_regs *r)
{
    xn_vec3 v;

    get3(r, &v);
    xn_vec_normalize_shift(&v, r->ecx & 0xFF);
    put3(&v, r);
}

void xn_vec_triangle_normal_r(xn_regs *r)
{
    xn_vec3 n;

    xn_vec_triangle_normal((const xn_vec3 *)r->eax, &n);
    put3(&n, r);
}
