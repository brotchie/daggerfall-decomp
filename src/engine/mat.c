/* mat.c: XnGine's matrix functions as readable C (xmat.h; see xngine.h and
   docs/xngine_readable.md). Rotations are 3x3, rows of 2.28 fixed point; angles 2048 steps to
   a turn. */
#include "xmat.h"
#include "xmath.h"

/* the other side of the circle: a -> 0x400 - a, in 0..0x7FF */
static s32 mirror(s32 a)
{
    a = 0x400 - a;
    if (a < 0)
        a += 0x800;
    return a;
}

/* |v|, where the most negative value stays negative (as `neg` leaves it) */
static s32 magnitude(s32 v)
{
    return v < 0 ? -v : v;
}

/* The arc sine of num / den for |num| <= |den|: (num << 28) / den */
static s32 asin_ratio(s32 num, s32 den)
{
    xn_s64 n;

    xn_s64_set(&n, num);
    xn_s64_shl(&n, 28);
    return xn_math_asin(xn_s64_div(&n, den));
}

int xn_mat_to_angles(const xn_mat3 *m, s32 *pitch, s32 *yaw, s32 *roll)
{
    s32 p, y, r, c;

    p = xn_math_asin(m->m[2][1]);
    c = xn_cos_table[p];
    if (c == 0 || magnitude(m->m[2][0]) > magnitude(c))
        goto fail;
    y = asin_ratio(m->m[2][0], c);
    if (m->m[2][2] < 0)
        y = mirror(y);
    if (magnitude(-m->m[0][1]) > magnitude(c))
        goto fail;
    r = asin_ratio(-m->m[0][1], c);
    if (m->m[1][1] < 0)
        r = mirror(r);
    *pitch = p;
    *yaw = y;
    *roll = r;
    return 1;
fail:
    *pitch = *yaw = *roll = 0;
    return 0;
}

void xn_mat_to_angles_r(xn_regs *r)
{
    s32 p, y, rl;
    int ok = xn_mat_to_angles((const xn_mat3 *)r->eax, &p, &y, &rl);

    r->eax = p;
    r->edx = y;
    r->ebx = rl;
    XN_SETFLAG(r, XN_CF, !ok);
}

/* (a*b + c*d + 2^27) >> 28 with a 64-bit sum */
static s32 dot2r28(s32 a, s32 b, s32 c, s32 d)
{
    xn_s64 t;

    xn_s64_mul(&t, a, b);
    xn_s64_mac(&t, c, d);
    xn_s64_addu(&t, 0x08000000);
    return xn_s64_shr(&t, 28);
}

void xn_mat_from_angles(s32 pitch, s32 yaw, s32 roll, xn_mat3 *m)
{
    xn_mat_angles_tmp *t = &xn_mat_from_angles_tmp;
    s32 sp, cp, sy, cy, sr, cr;

    pitch &= XN_ANGLE_MASK;
    yaw &= XN_ANGLE_MASK;
    roll &= XN_ANGLE_MASK;
    sp = xn_sin_table[pitch];
    cp = xn_cos_table[pitch];
    sy = xn_sin_table[yaw];
    cy = xn_cos_table[yaw];
    sr = xn_sin_table[roll];
    cr = xn_cos_table[roll];
    t->sr_sy = xn_fixmul28r(sr, sy);
    t->sy_cr = xn_fixmul28r(sy, cr);
    t->sp_cy = xn_fixmul28r(sp, cy);
    m->m[0][0] = dot2r28(sp, t->sr_sy, cr, cy);
    m->m[0][1] = xn_fixmul28r(-sr, cp);
    m->m[0][2] = xn_fixmul28r(sr, t->sp_cy) - t->sy_cr;
    m->m[1][0] = dot2r28(-sp, t->sy_cr, sr, cy);
    m->m[1][1] = xn_fixmul28r(cp, cr);
    m->m[1][2] = xn_fixmul28r(-cr, t->sp_cy) - t->sr_sy;
    m->m[2][0] = xn_fixmul28r(sy, cp);
    m->m[2][1] = sp;
    m->m[2][2] = xn_fixmul28r(cp, cy);
}

void xn_mat_identity_q28(xn_mat3 *m)
{
    xn_mat_identity(&m->m[0][0], 28, 3);
}

void xn_mat_set_scale_thunk(xn_mat3 *m)
{
    xn_mat_set_scale(m);
}

void xn_mat_set_scale(xn_mat3 *m)
{
    xn_mat_identity(&m->m[0][0], 28, 3);
    m->m[0][0] = xn_cam_scale_x << 14;
    m->m[1][1] = (s32)m;               /* meant: xn_cam_scale_y << 14 */
}

void xn_mat_transform_ptr(s32 *x, s32 *y, s32 *z, const xn_mat3 *m)
{
    xn_vec3 v;

    v.x = *x;
    v.y = *y;
    v.z = *z;
    xn_mat_transform(&v, m);
    *z = v.z;
    *y = v.y;
    *x = v.x;
}

void xn_mat_transform(xn_vec3 *v, const xn_mat3 *m)
{
    s32 x = v->x << 4, y = v->y << 4, z = v->z << 4;

    v->x = xn_mulhi(x, m->m[0][0]) + xn_mulhi(y, m->m[0][1]) + xn_mulhi(z, m->m[0][2]);
    v->y = xn_mulhi(x, m->m[1][0]) + xn_mulhi(y, m->m[1][1]) + xn_mulhi(z, m->m[1][2]);
    v->z = xn_mulhi(x, m->m[2][0]) + xn_mulhi(y, m->m[2][1]) + xn_mulhi(z, m->m[2][2]);
}

/* the asm interface of a transform: (EAX, EDX, EBX) and the matrix in ECX, in and out */
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

void xn_mat_transform_transposed_ptr(s32 *x, s32 *y, s32 *z, const xn_mat3 *m)
{
    xn_vec3 v;

    v.x = *x;
    v.y = *y;
    v.z = *z;
    xn_mat_transform_transposed(&v, m);
    *z = v.z;
    *y = v.y;
    *x = v.x;
}

void xn_mat_transform_transposed(xn_vec3 *v, const xn_mat3 *m)
{
    s32 x = v->x << 4, y = v->y << 4, z = v->z << 4;

    v->x = xn_mulhi(x, m->m[0][0]) + xn_mulhi(y, m->m[1][0]) + xn_mulhi(z, m->m[2][0]);
    v->y = xn_mulhi(x, m->m[0][1]) + xn_mulhi(y, m->m[1][1]) + xn_mulhi(z, m->m[2][1]);
    v->z = xn_mulhi(x, m->m[0][2]) + xn_mulhi(y, m->m[1][2]) + xn_mulhi(z, m->m[2][2]);
}

void xn_mat_transform_transposed_r(xn_regs *r)
{
    transform_regs(r, xn_mat_transform_transposed);
}

void xn_mat_transform_ptr_v2(s32 *x, s32 *y, s32 *z, const xn_mat3 *m)
{
    xn_mat_transform_ptr(x, y, z, m);
}

void xn_mat_transform_wide(xn_vec3 *v, const xn_mat3 *m)
{
    s32 x = v->x, y = v->y, z = v->z;

    v->x = xn_fixmul28(x, m->m[0][0]) + xn_fixmul28(y, m->m[0][1]) + xn_fixmul28(z, m->m[0][2]);
    v->y = xn_fixmul28(x, m->m[1][0]) + xn_fixmul28(y, m->m[1][1]) + xn_fixmul28(z, m->m[1][2]);
    v->z = xn_fixmul28(x, m->m[2][0]) + xn_fixmul28(y, m->m[2][1]) + xn_fixmul28(z, m->m[2][2]);
}

void xn_mat_transform_wide_r(xn_regs *r)
{
    transform_regs(r, xn_mat_transform_wide);
}

void xn_mat_transform_transposed_v2(xn_vec3 *v, const xn_mat3 *m)
{
    xn_mat_transform_transposed(v, m);
}

void xn_mat_transform_transposed_v2_r(xn_regs *r)
{
    transform_regs(r, xn_mat_transform_transposed_v2);
}

void xn_mat_scaled_axes(s32 a, s32 b, s32 c, const xn_mat3 *m)
{
    xn_vec3 v;

    xn_mat_scaled_axes_in.x = a;
    xn_mat_scaled_axes_in.y = b;
    xn_mat_scaled_axes_in.z = c;
    v.x = a;
    v.y = 0;
    v.z = 0;
    xn_mat_transform_wide(&v, m);
    xn_terrain_step_x = v;
    v.x = 0;
    v.y = b;
    v.z = 0;
    xn_mat_transform_wide(&v, m);
    xn_terrain_step_y = v;
    v.x = 0;
    v.y = 0;
    v.z = c;
    xn_mat_transform_wide(&v, m);
    xn_terrain_step_z = v;
}

void xn_mat_multiply(const xn_mat3 *a, const xn_mat3 *b, xn_mat3 *out)
{
    int i, j;
    xn_s64 s;

    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            xn_s64_mul(&s, a->m[i][0], b->m[0][j]);
            xn_s64_mac(&s, a->m[i][1], b->m[1][j]);
            xn_s64_mac(&s, a->m[i][2], b->m[2][j]);
            out->m[i][j] = xn_s64_shr(&s, 28);
        }
    }
}

/* the address `bytes` past p */
#define XN_STEP(p, bytes) ((void *)((u8 *)(p) + (bytes)))

void xn_mat_mul_int(s32 bcols, s32 rows, s32 *c, s32 inner, const s32 *a, const s32 *b)
{
    s32 k, *cc;
    const s32 *bb;

    xn_mat_int_strides[0] = inner * 4;
    xn_mat_int_strides[1] = bcols * 4;
    do {
        cc = c;
        bb = b;
        k = bcols;
        do {
            xn_mat_dot_int(inner, cc++, a, bb++);
        } while (--k != 0);
        a = XN_STEP(a, xn_mat_int_strides[0]);
        c = XN_STEP(c, xn_mat_int_strides[1]);
    } while (--rows != 0);
}

void xn_mat_dot_int(s32 n, s32 *out, const s32 *a, const s32 *b)
{
    s32 sum = 0;

    do {
        sum += *a++ * *b;
        b = XN_STEP(b, xn_mat_int_strides[1]);
    } while (--n != 0);
    *out = sum;
}

void xn_mat_mul_fixed(s32 bcols, s32 rows, s32 *c, s32 inner, const s32 *a, const s32 *b,
                      s32 shift)
{
    s32 k, *cc;
    const s32 *bb;

    xn_mat_fixed_strides[0] = inner * 4;
    xn_mat_fixed_strides[1] = bcols * 4;
    xn_mat_fixed_shift = shift;
    do {
        cc = c;
        bb = b;
        k = bcols;
        do {
            xn_mat_dot_fixed(inner, cc++, a, bb++);
        } while (--k != 0);
        a = XN_STEP(a, xn_mat_fixed_strides[0]);
        c = XN_STEP(c, xn_mat_fixed_strides[1]);
    } while (--rows != 0);
}

void xn_mat_mul_fixed_r(xn_regs *r)
{
    xn_mat_mul_fixed(r->eax, r->edx, (s32 *)r->ebx, r->ecx, (const s32 *)r->esi,
                     (const s32 *)r->edi, r->ebp);
}

void xn_mat_dot_fixed(s32 n, s32 *out, const s32 *a, const s32 *b)
{
    xn_s64 sum;

    sum.lo = 0;
    sum.hi = 0;
    do {
        xn_s64_mac(&sum, *a++, *b);
        b = XN_STEP(b, xn_mat_fixed_strides[1]);
    } while (--n != 0);
    *out = xn_s64_shr(&sum, xn_mat_fixed_shift);
}

void xn_mat_mul_fixed64(s32 bcols, s32 rows, s32 *c, s32 inner, const s32 *a, const s32 *b,
                        u16 shift)
{
    s32 k, *cc;
    const s32 *bb;

    xn_mat_fixed64_strides[0] = inner * 8;
    xn_mat_fixed64_strides[1] = bcols * 8;
    xn_mat_fixed64_shift = shift;
    do {
        cc = c;
        bb = b;
        k = bcols;
        do {
            xn_mat_dot_fixed64(inner, cc, a, bb);
            cc += 2;
            bb += 2;
        } while (--k != 0);
        a = XN_STEP(a, xn_mat_fixed64_strides[0]);
        c = XN_STEP(c, xn_mat_fixed64_strides[1]);
    } while (--rows != 0);
}

void xn_mat_mul_fixed64_r(xn_regs *r)
{
    xn_mat_mul_fixed64(r->eax, r->edx, (s32 *)r->ebx, r->ecx, (const s32 *)r->esi,
                       (const s32 *)r->edi, (u16)r->ebp);
}

void xn_mat_dot_fixed64(s32 n, s32 *out, const s32 *a, const s32 *b)
{
    xn_s64 sum;
    u32 sh = xn_mat_fixed64_shift & 31;

    sum.lo = 0;
    sum.hi = 0;
    do {
        xn_s64_mac(&sum, *a, *b);      /* the low dwords of the 8-byte entries */
        a += 2;
        b = XN_STEP(b, xn_mat_fixed64_strides[1]);
    } while (--n != 0);
    out[0] = xn_s64_shr(&sum, sh);
    out[1] = sum.hi >> sh;
}

void xn_mat_identity(s32 *m, s32 shift, s32 n)
{
    s32 one = 1 << shift, count = n * n, k;

    for (;;) {
        *m++ = one;
        if (--count == 0)
            break;
        k = n;
        do {
            *m++ = 0;
            count--;
        } while (--k != 0);
    }
}

void xn_mat_identity64(s32 *m, s32 shift, s32 n)
{
    s32 one = 1 << shift, count = n * n, k;

    for (;;) {
        m[0] = one;
        m[1] = 0;
        m += 2;
        if (--count == 0)
            break;
        k = n;
        do {
            m[0] = 0;
            m[1] = 0;
            m += 2;
            count--;
        } while (--k != 0);
    }
}

void xn_mat_transpose_copy3(const xn_mat3 *src, xn_mat3 *dst)
{
    s32 t;

    *dst = *src;
    t = dst->m[0][1];
    dst->m[0][1] = dst->m[1][0];
    dst->m[1][0] = t;
    t = dst->m[0][2];
    dst->m[0][2] = dst->m[2][0];
    dst->m[2][0] = t;
    t = dst->m[1][2];
    dst->m[1][2] = dst->m[2][1];
    dst->m[2][1] = t;
}
