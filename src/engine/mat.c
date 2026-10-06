/* mat.c: XnGine's matrices (canonical C; the interface and the module's documentation are in
   xmat.h). Rotations are 3x3, rows of 2.28 fixed point; angles 2048 steps to a turn. */
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

/* The arc sine of num / den for |num| <= |den|, den != 0: (num << 28) / den */
static s32 asin_ratio(s32 num, s32 den)
{
    xn_s64 n;

    xn_s64_set(&n, num);
    xn_s64_shl(&n, 28);
    return xn_math_asin(xn_s64_div_or0(&n, den));
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
    s32 sp, cp, sy, cy, sr, cr, sr_sy, sy_cr, sp_cy;

    pitch &= XN_ANGLE_MASK;
    yaw &= XN_ANGLE_MASK;
    roll &= XN_ANGLE_MASK;
    sp = xn_sin_table[pitch];
    cp = xn_cos_table[pitch];
    sy = xn_sin_table[yaw];
    cy = xn_cos_table[yaw];
    sr = xn_sin_table[roll];
    cr = xn_cos_table[roll];
    sr_sy = xn_fixmul28r(sr, sy);
    sy_cr = xn_fixmul28r(sy, cr);
    sp_cy = xn_fixmul28r(sp, cy);
    m->m[0][0] = dot2r28(sp, sr_sy, cr, cy);
    m->m[0][1] = xn_fixmul28r(-sr, cp);
    m->m[0][2] = xn_fixmul28r(sr, sp_cy) - sy_cr;
    m->m[1][0] = dot2r28(-sp, sy_cr, sr, cy);
    m->m[1][1] = xn_fixmul28r(cp, cr);
    m->m[1][2] = xn_fixmul28r(-cr, sp_cy) - sr_sy;
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
    m->m[1][1] = (s32)(iptr)m;          /* Quirk Q-MAT-01: meant xn_cam_scale_y << 14
                                           (natively the address's low half) */
}

void xn_mat_transform(xn_vec3 *v, const xn_mat3 *m)
{
    s32 x = v->x << 4, y = v->y << 4, z = v->z << 4;

    v->x = xn_mulhi(x, m->m[0][0]) + xn_mulhi(y, m->m[0][1]) + xn_mulhi(z, m->m[0][2]);
    v->y = xn_mulhi(x, m->m[1][0]) + xn_mulhi(y, m->m[1][1]) + xn_mulhi(z, m->m[1][2]);
    v->z = xn_mulhi(x, m->m[2][0]) + xn_mulhi(y, m->m[2][1]) + xn_mulhi(z, m->m[2][2]);
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

void xn_mat_transform_ptr_v2(s32 *x, s32 *y, s32 *z, const xn_mat3 *m)
{
    xn_mat_transform_ptr(x, y, z, m);
}

void xn_mat_transform_transposed(xn_vec3 *v, const xn_mat3 *m)
{
    s32 x = v->x << 4, y = v->y << 4, z = v->z << 4;

    v->x = xn_mulhi(x, m->m[0][0]) + xn_mulhi(y, m->m[1][0]) + xn_mulhi(z, m->m[2][0]);
    v->y = xn_mulhi(x, m->m[0][1]) + xn_mulhi(y, m->m[1][1]) + xn_mulhi(z, m->m[2][1]);
    v->z = xn_mulhi(x, m->m[0][2]) + xn_mulhi(y, m->m[1][2]) + xn_mulhi(z, m->m[2][2]);
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

void xn_mat_transform_transposed_v2(xn_vec3 *v, const xn_mat3 *m)
{
    xn_mat_transform_transposed(v, m);
}

void xn_mat_transform_wide(xn_vec3 *v, const xn_mat3 *m)
{
    s32 x = v->x, y = v->y, z = v->z;

    v->x = xn_fixmul28(x, m->m[0][0]) + xn_fixmul28(y, m->m[0][1]) + xn_fixmul28(z, m->m[0][2]);
    v->y = xn_fixmul28(x, m->m[1][0]) + xn_fixmul28(y, m->m[1][1]) + xn_fixmul28(z, m->m[1][2]);
    v->z = xn_fixmul28(x, m->m[2][0]) + xn_fixmul28(y, m->m[2][1]) + xn_fixmul28(z, m->m[2][2]);
}

void xn_mat_scaled_axes(s32 a, s32 b, s32 c, const xn_mat3 *m)
{
    xn_vec3 v;

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

/* ---- general n x m products (collision) -------------------------------------------------- */

s32 xn_mat_dot_int(s32 n, const s32 *a, const s32 *b, s32 b_stride)
{
    s32 sum = 0;

    do {
        sum += *a++ * *b;
        b += b_stride;
    } while (--n != 0);
    return sum;
}

void xn_mat_mul_int(s32 *c, const s32 *a, const s32 *b, s32 rows, s32 inner, s32 cols)
{
    s32 i, j;

    for (i = rows; i != 0; i--, a += inner, c += cols)
        for (j = 0; j != cols; j++)
            c[j] = xn_mat_dot_int(inner, a, b + j, cols);
}

s32 xn_mat_dot_fixed(s32 n, const s32 *a, const s32 *b, s32 b_stride, u32 shift)
{
    xn_s64 sum;

    sum.lo = 0;
    sum.hi = 0;
    do {
        xn_s64_mac(&sum, *a++, *b);
        b += b_stride;
    } while (--n != 0);
    return xn_s64_shr(&sum, shift & 31);
}

void xn_mat_mul_fixed(s32 *c, const s32 *a, const s32 *b, s32 rows, s32 inner, s32 cols,
                      u32 shift)
{
    s32 i, j;

    for (i = rows; i != 0; i--, a += inner, c += cols)
        for (j = 0; j != cols; j++)
            c[j] = xn_mat_dot_fixed(inner, a, b + j, cols, shift);
}

void xn_mat_dot_fixed64(s32 n, const s32 *a, const s32 *b, s32 b_stride, u32 shift, s32 out[2])
{
    xn_s64 sum;

    shift &= 31;
    sum.lo = 0;
    sum.hi = 0;
    do {
        xn_s64_mac(&sum, *a, *b);      /* the low dwords of the 8-byte entries */
        a += 2;
        b += b_stride;
    } while (--n != 0);
    out[0] = xn_s64_shr(&sum, shift);
    out[1] = sum.hi >> shift;
}

void xn_mat_mul_fixed64(s32 *c, const s32 *a, const s32 *b, s32 rows, s32 inner, s32 cols,
                        u32 shift)
{
    s32 i, j;

    for (i = rows; i != 0; i--, a += 2 * inner, c += 2 * cols)
        for (j = 0; j != cols; j++)
            xn_mat_dot_fixed64(inner, a, b + 2 * j, 2 * cols, shift, c + 2 * j);
}

void xn_mat_identity(s32 *m, s32 shift, s32 n)
{
    s32 one = 1 << shift, count = n * n, k;

    for (;;) {
        *m++ = one;
        if (--count == 0)
            break;
        for (k = n; k != 0; k--, count--)
            *m++ = 0;
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
        for (k = n; k != 0; k--, count--) {
            m[0] = 0;
            m[1] = 0;
            m += 2;
        }
    }
}
