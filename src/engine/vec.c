/* vec.c: XnGine's vector functions as readable C (xvec.h; see xngine.h and
   docs/xngine_readable.md). */
#include "xvec.h"
#include "xmath.h"

/* a vector from the registers of an asm caller, and back */
static void regs_to_vec(const xn_regs *r, xn_vec3 *v)
{
    v->x = r->eax;
    v->y = r->edx;
    v->z = r->ebx;
}

static void vec_to_regs(const xn_vec3 *v, xn_regs *r)
{
    r->eax = v->x;
    r->edx = v->y;
    r->ebx = v->z;
}

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

void xn_vec_dir_to_angles_regs_r(xn_regs *r)
{
    s32 yaw;

    r->eax = xn_vec_dir_to_angles_regs(r->eax, r->edx, r->ebx, &yaw);
    r->edx = yaw;
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

    xn_vec_angles_in.x = x;
    xn_vec_angles_in.y = y;
    xn_vec_angles_in.z = z;
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
        pitch = xn_s64_div(&sy, c);
    else
        pitch = xn_mulhi(-y >> 4, c) < 0 ? -XN_ONE28 : XN_ONE28;
    return xn_math_asin(pitch);
}

void xn_vec_unit_to_angles_regs_r(xn_regs *r)
{
    s32 yaw;

    r->eax = xn_vec_unit_to_angles_regs(r->eax, r->edx, r->ebx, &yaw);
    r->edx = yaw;
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

void xn_vec_scale_unit14_r(xn_regs *r)
{
    xn_vec3 v;
    int ok;

    regs_to_vec(r, &v);
    ok = xn_vec_scale_unit14(&v, r->ecx);
    vec_to_regs(&v, r);
    XN_SETFLAG(r, XN_CF, !ok);
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
    xn_tri_work *w = &xn_math_tri_edges;

    w->w_a = wa << 10;
    w->w_b = wb << 10;
    out->x = blend1(a->x, w->w_a, b->x, w->w_b);
    out->y = blend1(a->y, w->w_a, b->y, w->w_b);
    out->z = blend1(a->z, w->w_a, b->z, w->w_b);
}

void xn_vec_blend_r(xn_regs *r)
{
    xn_vec3 v;

    xn_vec_blend(r->eax, (const xn_vec3 *)r->edx, r->ebx, (const xn_vec3 *)r->ecx, &v);
    vec_to_regs(&v, r);
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

void xn_vec_cross_r(xn_regs *r)
{
    xn_vec3 c;

    xn_vec_cross((const xn_vec3 *)r->eax, (const xn_vec3 *)r->edx, &c);
    vec_to_regs(&c, r);
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

s32 xn_vec_length_approx(s32 x, s32 y, s32 z, xn_vec3 *terms)
{
    u32 ax = magnitude(x), ay = magnitude(y), az = magnitude(z);

    /* the largest stays, the other two count a quarter (signed comparisons, as the asm) */
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
    return ax + ay + az;
}

void xn_vec_length_approx_r(xn_regs *r)
{
    xn_vec3 t;

    r->eax = xn_vec_length_approx(r->eax, r->edx, r->ebx, &t);
    r->edx = t.y;
    r->ebx = t.z;
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
    v->z = xn_s64_div(&s, len);
    xn_s64_set(&s, v->y);
    xn_s64_shl(&s, shift);
    v->y = xn_s64_div(&s, len);
    xn_s64_set(&s, v->x);
    xn_s64_shl(&s, shift);
    v->x = xn_s64_div(&s, len);
}

void xn_vec_normalize(xn_vec3 *v)
{
    normalize(v, 16);
}

void xn_vec_normalize_r(xn_regs *r)
{
    xn_vec3 v;

    regs_to_vec(r, &v);
    xn_vec_normalize(&v);
    vec_to_regs(&v, r);
}

void xn_vec_normalize_q28(xn_vec3 *v)
{
    normalize(v, 28);
}

void xn_vec_normalize_q28_r(xn_regs *r)
{
    xn_vec3 v;

    regs_to_vec(r, &v);
    xn_vec_normalize_q28(&v);
    vec_to_regs(&v, r);
}

void xn_vec_normalize_shift(xn_vec3 *v, u32 shift)
{
    xn_s64 s;
    u32 len, recip;

    sum_squares(&s, v->x, v->y, v->z);
    len = xn_math_isqrt64(s.lo, s.hi);
    s.lo = 0;
    s.hi = 1 << (shift & 31);
    recip = xn_u64_div(&s, len);        /* 2^(32+shift) / |v| */
    v->z = xn_mulhi(v->z, recip);
    v->y = xn_mulhi(v->y, recip);
    v->x = xn_mulhi(v->x, recip);
}

void xn_vec_normalize_shift_r(xn_regs *r)
{
    xn_vec3 v;

    regs_to_vec(r, &v);
    xn_vec_normalize_shift(&v, r->ecx & 0xFF);
    vec_to_regs(&v, r);
}

void xn_vec_triangle_normal(const xn_vec3 *tri, xn_vec3 *out)
{
    xn_tri_work *w = &xn_math_tri_edges;
    xn_vec3 n;

    w->e1.x = tri[1].x - tri[0].x;
    w->e1.y = tri[1].y - tri[0].y;
    w->e1.z = tri[1].z - tri[0].z;
    w->e2.x = tri[2].x - tri[1].x;
    w->e2.y = tri[2].y - tri[1].y;
    w->e2.z = tri[2].z - tri[1].z;
    n.z = cross8(w->e1.x, w->e2.y, w->e1.y, w->e2.x, 0);
    n.y = cross8(w->e1.z, w->e2.x, w->e1.x, w->e2.z, 0);
    n.x = cross8(w->e1.y, w->e2.z, w->e1.z, w->e2.y, 0);
    *out = n;
}

void xn_vec_triangle_normal_r(xn_regs *r)
{
    xn_vec3 n;

    xn_vec_triangle_normal((const xn_vec3 *)r->eax, &n);
    vec_to_regs(&n, r);
}
