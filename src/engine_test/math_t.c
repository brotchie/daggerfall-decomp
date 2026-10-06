/* math_t.c: test shims of src/engine/math.c (built only by tools/xn_rc.py; see vec_t.c and
   docs/xngine_canonical.md). */
#include "xmath.h"

void xn_math_approx_dist2d_r(xn_regs *r)
{
    r->eax = xn_math_approx_dist2d(r->eax, r->edx, r->ebx, r->ecx);
}

void xn_math_approx_hypot_r(xn_regs *r)
{
    r->eax = xn_math_approx_hypot(r->eax, r->edx);
}

/* a EAX, b EDX, d EBX -> quotient EAX, remainder EDX */
void xn_math_diff_div_r(xn_regs *r)
{
    u32 rem;

    r->eax = xn_math_diff_div(r->eax, r->edx, r->ebx, &rem);
    r->edx = rem;
}

/* v in EDX */
void xn_math_exp_series_r(xn_regs *r)
{
    r->eax = xn_math_exp_series(r->edx);
}

void xn_math_angle_to_point_r(xn_regs *r)
{
    r->eax = xn_math_angle_to_point(r->eax, r->edx, r->ebx, r->ecx);
}

void xn_math_fixmul28_v2_r(xn_regs *r)
{
    r->eax = xn_math_fixmul28_v2(r->eax, r->edx);
}

/* -> EAX EDX EBX */
void xn_math_angles_to_vector_r(xn_regs *r)
{
    xn_vec3 v;

    xn_math_angles_to_vector(r->eax, r->edx, r->ebx, &v);
    r->eax = v.x;
    r->edx = v.y;
    r->ebx = v.z;
}

void xn_math_scale_110_r(xn_regs *r)
{
    s32 rem;

    r->eax = xn_math_scale_110(r->eax, r->edx, r->ebx, &rem);
    r->edx = rem;
}

void xn_math_mul_sin_r(xn_regs *r)
{
    r->eax = xn_math_mul_sin(r->eax, r->edx);
}

/* yaw EAX, dist EDX, &x EBX, &z ECX */
void xn_math_yaw_offset_xz_r(xn_regs *r)
{
    xn_math_yaw_offset_xz(r->eax, r->edx, (s32 *)r->ebx, (s32 *)r->ecx);
}

void xn_math_advance_pitch_yaw_r(xn_regs *r)
{
    xn_math_advance_pitch_yaw(r->eax, r->edx, r->ebx, (xn_vec3 *)r->ecx);
}

void xn_math_fixmul28_r(xn_regs *r)
{
    r->eax = xn_math_fixmul28(r->eax, r->edx);
}

void xn_math_asin_coarse_r(xn_regs *r)
{
    r->eax = xn_math_asin_coarse(r->eax);
}

void xn_math_asin_r(xn_regs *r)
{
    r->eax = xn_math_asin(r->eax);
}

void xn_math_acos_coarse_r(xn_regs *r)
{
    r->eax = xn_math_acos_coarse(r->eax);
}

void xn_math_acos_r(xn_regs *r)
{
    r->eax = xn_math_acos(r->eax);
}

/* v EAX; for 0 the asm's answer depends on ECX (the boundary adapter has it) */
void xn_math_isqrt_r(xn_regs *r)
{
    xn_math_isqrt_b(r);
}

/* lo EAX, hi EDX */
void xn_math_isqrt64_r(xn_regs *r)
{
    r->eax = xn_math_isqrt64(r->eax, r->edx);
}

void xn_math_angle_xy_r(xn_regs *r)
{
    r->eax = xn_math_angle_xy(r->eax, r->edx);
}

/* n (EAX, EDX, EBX), c ECX, x ESI, z EDI */
void xn_math_plane_y_at_r(xn_regs *r)
{
    xn_vec3 n;

    n.x = r->eax;
    n.y = r->edx;
    n.z = r->ebx;
    r->eax = xn_math_plane_y_at(&n, (const xn_vec3 *)r->ecx, r->esi, r->edi);
}

void xn_math_triangle_y_at_r(xn_regs *r)
{
    r->eax = xn_math_triangle_y_at((const xn_vec3 *)r->eax, r->edx, r->ebx);
}

/* x EAX, z EDX, angle EBX -> x EAX, z EDX */
void xn_math_rotate_xz_r(xn_regs *r)
{
    s32 x = r->eax, z = r->edx;

    xn_math_rotate_xz(&x, &z, r->ebx);
    r->eax = x;
    r->edx = z;
}

void xn_math_isqrt_lookup_r(xn_regs *r)
{
    r->eax = xn_math_isqrt_lookup(r->eax);
}
