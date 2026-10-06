/* vec.c: XnGine's vectors (canonical C; the interface and the module's documentation are in
   xvec.h). */
#include "xvec.h"
#include "xmath.h"

xn_vec3 *xn_vec_unit_direction(const xn_vec3 *from, const xn_vec3 *to, xn_vec3 *out)
{
    xn_vec3 d;

    d.x = to->x - from->x;
    d.y = to->y - from->y;
    d.z = to->z - from->z;
    if ((d.x | d.y | d.z) == 0)
        return 0;
    xn_vec_normalize(&d);               /* 16.16, rounded to 1.0 = 0x4000 */
    out->x = (d.x + 2) >> 2;
    out->y = (d.y + 2) >> 2;
    out->z = (d.z + 2) >> 2;
    return out;
}

xn_vec3 *xn_vec_advance(const xn_vec3 *dir, s32 dist, xn_vec3 *pos)
{
    xn_vec3 step = *dir;

    if (!xn_vec_scale_unit14(&step, dist))
        return 0;
    pos->x += step.x;
    pos->y += step.y;
    pos->z += step.z;
    return pos;
}

void xn_vec_dir_to_angles(xn_vec3 *v)
{
    s32 yaw;

    v->x = xn_vec_dir_to_angles_regs(v->x, v->y, v->z, &yaw);
    v->y = yaw;
    v->z = 0;
}

s32 xn_vec_dir_to_angles_regs(s32 x, s32 y, s32 z, s32 *yaw)
{
    xn_vec3 v;

    v.x = x;
    v.y = y;
    v.z = z;
    xn_vec_normalize(&v);               /* 16.16 -> 2.28 */
    return xn_vec_unit_to_angles_regs(v.x << 12, v.y << 12, v.z << 12, yaw);
}

s32 xn_vec_unit_to_angles(xn_vec3 *v)
{
    s32 yaw, pitch;

    pitch = xn_vec_unit_to_angles_regs(v->x, v->y, v->z, &yaw);
    v->x = pitch;
    v->y = yaw;
    v->z = 0;
    return pitch;
}

/* |v|, where the most negative value stays negative (as `neg` leaves it) */
static s32 magnitude(s32 v)
{
    return v < 0 ? -v : v;
}

s32 xn_vec_unit_to_angles_regs(s32 x, s32 y, s32 z, s32 *yaw)
{
    s32 a, c, pitch;
    xn_s64 sy;

    a = xn_math_asin(-x);
    if (z < 0) {                        /* facing back: the other side of the circle */
        a = 0x400 - a;
        if (a < 0)
            a += 0x800;
    }
    a &= XN_ANGLE_MASK;
    *yaw = a;
    c = xn_cos_table[a];
    /* sin(pitch) = -y / cos(yaw), or +-1 when that is not below 1 */
    xn_s64_set(&sy, -y);
    xn_s64_shl(&sy, 28);
    if (c != 0 && magnitude(-y) <= magnitude(c))
        pitch = xn_s64_div_or0(&sy, c);
    else
        pitch = xn_mulhi(-y >> 4, c) < 0 ? -XN_ONE28 : XN_ONE28;
    return xn_math_asin(pitch);
}

int xn_vec_scale_unit14(xn_vec3 *v, s32 dist)
{
    if (dist >= 0x10000)
        return 0;
    v->x = (v->x * dist + 0x2000) >> 14;
    v->y = (v->y * dist + 0x2000) >> 14;
    v->z = (v->z * dist + 0x2000) >> 14;
    return 1;
}

/* (a*wa + b*wb + 2^19) >> 20 with a 64-bit sum */
static s32 blend1(s32 a, s32 wa, s32 b, s32 wb)
{
    xn_s64 t;

    xn_s64_mul(&t, a, wa);
    xn_s64_mac(&t, b, wb);
    xn_s64_addu(&t, 0x80000);
    return xn_s64_shr(&t, 20);
}

void xn_vec_blend(s32 wa, const xn_vec3 *a, s32 wb, const xn_vec3 *b, xn_vec3 *out)
{
    xn_vec3 r;

    wa <<= 10;
    wb <<= 10;
    r.x = blend1(a->x, wa, b->x, wb);
    r.y = blend1(a->y, wa, b->y, wb);
    r.z = blend1(a->z, wa, b->z, wb);
    *out = r;
}

/* (a*b - c*d) >> 8 with a 64-bit difference; rounded adds 0x80 first */
static s32 cross8(s32 a, s32 b, s32 c, s32 d, int rounded)
{
    xn_s64 t;

    xn_s64_mul(&t, a, b);
    xn_s64_msub(&t, c, d);
    if (rounded)
        xn_s64_addu(&t, 0x80);
    return xn_s64_shr(&t, 8);
}

void xn_vec_cross(const xn_vec3 *a, const xn_vec3 *b, xn_vec3 *out)
{
    xn_vec3 c;

    c.y = cross8(a->z, b->x, a->x, b->z, 1);
    c.z = cross8(a->x, b->y, a->y, b->x, 1);
    c.x = cross8(a->y, b->z, a->z, b->y, 1);
    *out = c;
}

/* x^2 + y^2 + z^2 in 64 bits */
static void sum_squares(xn_s64 *s, s32 x, s32 y, s32 z)
{
    xn_s64_mul(s, x, x);
    xn_s64_mac(s, y, y);
    xn_s64_mac(s, z, z);
}

s32 xn_vec_length(s32 x, s32 y, s32 z)
{
    xn_s64 s;

    sum_squares(&s, x, y, z);
    return xn_math_isqrt64(s.lo, s.hi);
}

void xn_vec_length_approx_terms(s32 x, s32 y, s32 z, xn_vec3 *terms)
{
    u32 ax = magnitude(x), ay = magnitude(y), az = magnitude(z);

    /* the largest stays, the other two count a quarter (signed comparisons, as the asm: an
       axis whose magnitude stayed negative never counts as the largest) */
    if ((s32)ax < (s32)ay) {
        if ((s32)az < (s32)ay) {
            ax >>= 2;
            az >>= 2;
        } else {
            ax >>= 2;
            ay >>= 2;
        }
    } else if ((s32)az < (s32)ax) {
        ay >>= 2;
        az >>= 2;
    } else {
        ax >>= 2;
        ay >>= 2;
    }
    terms->x = ax;
    terms->y = ay;
    terms->z = az;
}

s32 xn_vec_length_approx(s32 x, s32 y, s32 z)
{
    xn_vec3 t;

    xn_vec_length_approx_terms(x, y, z, &t);
    return t.x + t.y + t.z;
}

void xn_vec_normalize_ptr(xn_vec3 *v)
{
    xn_vec_normalize(v);
}

/* v * 2^shift / |v| (shift 16: 16.16, 28: 2.28); a zero vector unchanged */
static void normalize(xn_vec3 *v, u32 shift)
{
    xn_s64 s;
    s32 len;

    sum_squares(&s, v->x, v->y, v->z);
    len = xn_math_isqrt64(s.lo, s.hi);
    if (len == 0)
        return;
    xn_s64_set(&s, v->z);
    xn_s64_shl(&s, shift);
    v->z = xn_s64_div_or0(&s, len);
    xn_s64_set(&s, v->y);
    xn_s64_shl(&s, shift);
    v->y = xn_s64_div_or0(&s, len);
    xn_s64_set(&s, v->x);
    xn_s64_shl(&s, shift);
    v->x = xn_s64_div_or0(&s, len);
}

void xn_vec_normalize(xn_vec3 *v)
{
    normalize(v, 16);
}

void xn_vec_normalize_q28(xn_vec3 *v)
{
    normalize(v, 28);
}

void xn_vec_normalize_shift(xn_vec3 *v, u32 shift)
{
    xn_s64 s;
    u32 len, recip;

    sum_squares(&s, v->x, v->y, v->z);
    len = xn_math_isqrt64(s.lo, s.hi);
    s.lo = 0;
    s.hi = 1 << (shift & 31);
    recip = xn_u64_div_or0(&s, len);    /* 2^(32+shift) / |v| */
    v->z = xn_mulhi(v->z, recip);
    v->y = xn_mulhi(v->y, recip);
    v->x = xn_mulhi(v->x, recip);
}

void xn_vec_triangle_normal(const xn_vec3 *tri, xn_vec3 *out)
{
    xn_vec3 e1, e2, n;

    e1.x = tri[1].x - tri[0].x;
    e1.y = tri[1].y - tri[0].y;
    e1.z = tri[1].z - tri[0].z;
    e2.x = tri[2].x - tri[1].x;
    e2.y = tri[2].y - tri[1].y;
    e2.z = tri[2].z - tri[1].z;
    n.z = cross8(e1.x, e2.y, e1.y, e2.x, 0);
    n.y = cross8(e1.z, e2.x, e1.x, e2.z, 0);
    n.x = cross8(e1.y, e2.z, e1.z, e2.y, 0);
    *out = n;
}
