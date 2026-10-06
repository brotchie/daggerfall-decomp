/* mat_t.c: test shims of src/engine/mat.c (built only by tools/xn_rc.py; see vec_t.c and
   docs/xngine_canonical.md). */
#include "xmat.h"

/* The general products' asm passes the row strides (in bytes) and the shift from the
   product routine to the dot-product routine through these globals in its code bytes; the
   dot products' asm entries read them, so their shims do. */
extern s32 xn_mat_int_strides[2];
extern s32 xn_mat_fixed_strides[2];
extern s32 xn_mat_fixed_shift;
extern s32 xn_mat_fixed64_strides[2];
extern u16 xn_mat_fixed64_shift;

/* m in EAX -> (pitch, yaw, roll) in EAX EDX EBX; CF when there are none */
void xn_mat_to_angles_r(xn_regs *r)
{
    s32 p, y, rl;
    int ok = xn_mat_to_angles((const xn_mat3 *)r->eax, &p, &y, &rl);

    r->eax = p;
    r->edx = y;
    r->ebx = rl;
    XN_SETFLAG(r, XN_CF, !ok);
}

void xn_mat_from_angles_r(xn_regs *r)
{
    xn_mat_from_angles(r->eax, r->edx, r->ebx, (xn_mat3 *)r->ecx);
}

/* (these three keep every register) */
void xn_mat_identity_q28_r(xn_regs *r)
{
    xn_mat_identity_q28((xn_mat3 *)r->eax);
}

void xn_mat_set_scale_thunk_r(xn_regs *r)
{
    xn_mat_set_scale_thunk((xn_mat3 *)r->eax);
}

void xn_mat_set_scale_r(xn_regs *r)
{
    xn_mat_set_scale((xn_mat3 *)r->eax);
}

void xn_mat_transform_ptr_r(xn_regs *r)
{
    xn_mat_transform_ptr((s32 *)r->eax, (s32 *)r->edx, (s32 *)r->ebx, (const xn_mat3 *)r->ecx);
}

void xn_mat_transform_transposed_ptr_r(xn_regs *r)
{
    xn_mat_transform_transposed_ptr((s32 *)r->eax, (s32 *)r->edx, (s32 *)r->ebx,
                                    (const xn_mat3 *)r->ecx);
}

void xn_mat_transform_ptr_v2_r(xn_regs *r)
{
    xn_mat_transform_ptr_v2((s32 *)r->eax, (s32 *)r->edx, (s32 *)r->ebx, (const xn_mat3 *)r->ecx);
}

/* v in EAX EDX EBX, m in ECX -> v */
static void transform_regs(xn_regs *r, void (*f)(xn_vec3 *, const xn_mat3 *))
{
    xn_vec3 v;

    v.x = r->eax;
    v.y = r->edx;
    v.z = r->ebx;
    f(&v, (const xn_mat3 *)r->ecx);
    r->eax = v.x;
    r->edx = v.y;
    r->ebx = v.z;
}

void xn_mat_transform_r(xn_regs *r)
{
    transform_regs(r, xn_mat_transform);
}

void xn_mat_transform_transposed_r(xn_regs *r)
{
    transform_regs(r, xn_mat_transform_transposed);
}

void xn_mat_transform_wide_r(xn_regs *r)
{
    transform_regs(r, xn_mat_transform_wide);
}

void xn_mat_transform_transposed_v2_r(xn_regs *r)
{
    transform_regs(r, xn_mat_transform_transposed_v2);
}

void xn_mat_scaled_axes_r(xn_regs *r)
{
    xn_mat_scaled_axes(r->eax, r->edx, r->ebx, (const xn_mat3 *)r->ecx);
}

void xn_mat_multiply_r(xn_regs *r)
{
    xn_mat_multiply((const xn_mat3 *)r->eax, (const xn_mat3 *)r->edx, (xn_mat3 *)r->ebx);
}

/* cols EAX, inner ECX, rows EDX, c EBX, a ESI, b EDI */
void xn_mat_mul_int_r(xn_regs *r)
{
    xn_mat_mul_int((s32 *)r->ebx, (const s32 *)r->esi, (const s32 *)r->edi, r->edx, r->ecx,
                   r->eax);
}

/* n ECX, out EBX, a ESI, b EDI; b's stride (bytes) from the product's global */
void xn_mat_dot_int_r(xn_regs *r)
{
    *(s32 *)r->ebx = xn_mat_dot_int(r->ecx, (const s32 *)r->esi, (const s32 *)r->edi,
                                    xn_mat_int_strides[1] >> 2);
}

/* the same and the shift in EBP */
void xn_mat_mul_fixed_r(xn_regs *r)
{
    xn_mat_mul_fixed((s32 *)r->ebx, (const s32 *)r->esi, (const s32 *)r->edi, r->edx, r->ecx,
                     r->eax, r->ebp);
}

void xn_mat_dot_fixed_r(xn_regs *r)
{
    *(s32 *)r->ebx = xn_mat_dot_fixed(r->ecx, (const s32 *)r->esi, (const s32 *)r->edi,
                                      xn_mat_fixed_strides[1] >> 2, xn_mat_fixed_shift);
}

/* the shift in BP */
void xn_mat_mul_fixed64_r(xn_regs *r)
{
    xn_mat_mul_fixed64((s32 *)r->ebx, (const s32 *)r->esi, (const s32 *)r->edi, r->edx, r->ecx,
                       r->eax, (u16)r->ebp);
}

void xn_mat_dot_fixed64_r(xn_regs *r)
{
    xn_mat_dot_fixed64(r->ecx, (const s32 *)r->esi, (const s32 *)r->edi,
                       xn_mat_fixed64_strides[1] >> 2, xn_mat_fixed64_shift, (s32 *)r->ebx);
}

void xn_mat_identity_r(xn_regs *r)
{
    xn_mat_identity((s32 *)r->eax, r->edx, r->ebx);
}

void xn_mat_identity64_r(xn_regs *r)
{
    xn_mat_identity64((s32 *)r->eax, r->edx, r->ebx);
}

void xn_mat_transpose_copy3_r(xn_regs *r)
{
    xn_mat_transpose_copy3((const xn_mat3 *)r->eax, (xn_mat3 *)r->edx);
}
