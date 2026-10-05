/* math.c: XnGine's math functions as readable C (xmath.h; see xngine.h and
   docs/xngine_readable.md). Angles are 2048 steps to a turn; sines and cosines are 2.28 fixed
   point (1.0 = 0x10000000). */
#include "xmath.h"

s32 xn_math_approx_dist2d(s32 x1, s32 z1, s32 x2, s32 z2)
{
    s32 dx = x1 - x2;
    s32 dz = z1 - z2;
    s32 small;

    if (dx < 0)
        dx = -dx;
    if (dz < 0)
        dz = -dz;
    small = dx < dz ? dx : dz;
    return dx + dz - (s32)((u32)small >> 1);
}

s32 xn_math_approx_hypot(s32 a, s32 b)
{
    u32 ua = a < 0 ? -a : a;
    u32 ub = b < 0 ? -b : b;

    if (ua > ub)
        return (ub >> 2) + ua;
    return (ua >> 2) + ub;
}

u32 xn_math_diff_div(u32 a, u32 b, u32 d, u32 *rem)
{
    xn_s64 n;

    n.lo = a - b;
    n.hi = 0;
    return xn_u64_divrem(&n, d, rem);
}

void xn_math_diff_div_r(xn_regs *r)
{
    u32 rem;

    r->eax = xn_math_diff_div(r->eax, r->edx, r->ebx, &rem);
    r->edx = rem;
}

s32 xn_math_exp_series(s32 v)
{
    s32 sq = v * v;
    xn_s64 n;

    /* The series' terms are lost to the register flow; only the first division by 6 is
       left, and the divide error it can raise. */
    n.lo = (s16)(v >> 1);
    n.hi = v >> 1;
    xn_s64_div(&n, 6);
    return sq * sq;
}

s32 xn_math_angle_to_point(s32 x1, s32 z1, s32 x2, s32 z2)
{
    s32 dx = x2 - x1;
    s32 dz = z2 - z1;
    s32 len, a;
    xn_s64 n;

    /* the length; the low bits of dz << 28 are what the asm leaves in ECX for the root */
    len = xn_math_isqrt(dx * dx + dz * dz, dz << 28);
    if ((u32)len <= 8)
        len = 8;
    xn_s64_set(&n, dz);
    xn_s64_shl(&n, 28);
    a = xn_math_acos(xn_s64_div(&n, len));
    if (dx >= 0)
        a = 0x7FF - a;
    return a;
}

s32 xn_math_fixmul28_v2(s32 a, s32 b)
{
    return xn_fixmul28(a, b);
}

void xn_math_angles_to_vector(s32 a, s32 b, s32 len, xn_vec3 *out)
{
    s32 scale = len << 4;

    a &= XN_ANGLE_MASK;
    b &= XN_ANGLE_MASK;
    out->x = xn_mulhi(xn_fixmul28(xn_sin_table[a], xn_cos_table[b]), scale);
    out->y = xn_mulhi(xn_fixmul28(xn_sin_table[a], xn_sin_table[b]), scale);
    out->z = xn_mulhi(xn_cos_table[a], scale);
}

void xn_math_angles_to_vector_r(xn_regs *r)
{
    xn_vec3 v;

    xn_math_angles_to_vector(r->eax, r->edx, r->ebx, &v);
    r->eax = v.x;
    r->edx = v.y;
    r->ebx = v.z;
}

s32 xn_math_scale_110(s32 a, s32 b, s32 d, s32 *rem)
{
    xn_s64 n;
    s32 q;

    n.lo = xn_mulhi(0x6E - a, b);
    n.hi = 0;
    q = xn_s64_divrem(&n, d, rem);
    return q != 0 ? q : 1;
}

void xn_math_scale_110_r(xn_regs *r)
{
    s32 rem;

    r->eax = xn_math_scale_110(r->eax, r->edx, r->ebx, &rem);
    r->edx = rem;
}

s32 xn_math_mul_sin(s32 a, s32 angle)
{
    return xn_fixmul28(a, xn_sin_table[angle]);
}

void xn_math_yaw_offset_xz(s32 yaw, s32 dist, s32 *x, s32 *z)
{
    yaw &= XN_ANGLE_MASK;
    *z = xn_fixmul28(xn_cos_table[yaw], dist);
    *x = xn_fixmul28(xn_sin_table[yaw], dist);
}

void xn_math_advance_pitch_yaw(s32 pitch, s32 yaw, s32 dist, xn_vec3 *v)
{
    yaw &= XN_ANGLE_MASK;
    pitch &= XN_ANGLE_MASK;
    v->z += xn_fixmul28(xn_cos_table[yaw], dist);
    v->x += xn_fixmul28(xn_sin_table[yaw], dist);
    v->y += xn_fixmul28(xn_sin_table[pitch], dist);
}

s32 xn_math_fixmul28(s32 a, s32 b)
{
    return xn_fixmul28(a, b);
}

s32 xn_math_asin_coarse(s32 v)
{
    s32 i = (v + 0x40000) >> 19;

    if (i < -512)
        i = -512;
    else if (i > 511)
        i = 511;
    return xn_math_asin_table[i];
}

/* The angle whose table value (sine or cosine) is nearest v, searching from the coarse guess
   a: walk the way v lies until two neighbours bracket it, then take the nearer. */
static s32 nearest_angle(const s32 *table, s32 a, s32 v)
{
    s32 lo, hi, d_lo, d_hi, step;

    if (v == table[a])
        return a;
    if (v > table[a]) {
        lo = a;
        hi = (a + 1) & XN_ANGLE_MASK;
        step = 1;
    } else {
        lo = (a - 1) & XN_ANGLE_MASK;
        hi = a;
        step = -1;
    }
    for (;;) {
        if (table[lo] == v)
            return lo;
        d_hi = table[hi] - v;
        d_lo = table[lo] - v;
        if ((d_hi ^ d_lo) < 0)
            break;
        lo = (lo + step) & XN_ANGLE_MASK;
        hi = (hi + step) & XN_ANGLE_MASK;
    }
    if (d_lo < 0)
        d_lo = -d_lo;
    if (d_hi < 0)
        d_hi = -d_hi;
    return d_lo > d_hi ? hi : lo;
}

s32 xn_math_asin(s32 v)
{
    if (v < -XN_ONE28)
        v = -XN_ONE28;
    else if (v > XN_ONE28)
        v = XN_ONE28;
    return nearest_angle(xn_sin_table, xn_math_asin_coarse(v) & XN_ANGLE_MASK, v);
}

s32 xn_math_acos_coarse(s32 v)
{
    return (xn_math_asin_coarse(v) + 0x600) & XN_ANGLE_MASK;
}

s32 xn_math_acos(s32 v)
{
    if (v < -XN_ONE28)
        v = -XN_ONE28;
    else if (v > XN_ONE28)
        v = XN_ONE28;
    return nearest_angle(xn_cos_table, xn_math_acos_coarse(v), v);
}

s32 xn_math_isqrt(s32 v, s32 ecx)
{
    s32 top = xn_bsr(v, ecx) - 1;
    s32 root = 0, t;
    u32 bit;

    if (top > 15)
        top = 15;
    for (bit = 1u << (top & 31); bit != 0; bit >>= 1) {
        t = root + bit;
        if (t * t <= v)
            root = t;
    }
    return root;
}

u32 xn_math_isqrt64(u32 lo, s32 hi)
{
    u32 bit = 0x40000000, root = 0, t;
    xn_s64 sq;

    if (hi == 0) {
        if (lo == 0)
            return 0;
        bit = 1u << ((xn_bsr(lo, 0) - 1) & 31);
    }
    for (; bit != 0; bit >>= 1) {
        t = root + bit;
        xn_u64_mul(&sq, t, t);
        if (sq.hi < hi || sq.hi == hi && sq.lo <= lo)
            root = t;
    }
    return root;
}

s32 xn_math_angle_xy(s32 x, s32 y)
{
    s32 quadrant = 0, t, len;
    xn_s64 n;

    /* fold into the first quadrant: x and y both >= 0 */
    if (x < 0) {
        x = -x;
        if (y < 0) {
            y = -y;
            quadrant = 0x400;
        } else {
            t = x;
            x = y;
            y = t;
            quadrant = 0x600;
        }
    } else if (y < 0) {
        y = -y;
        t = x;
        x = y;
        y = t;
        quadrant = 0x200;
    }
    xn_s64_mul(&n, x, x);
    xn_s64_mac(&n, y, y);
    len = xn_math_isqrt64(n.lo, n.hi);
    xn_s64_set(&n, x);
    xn_s64_shl(&n, 28);
    return (xn_math_acos(xn_s64_div(&n, len)) + quadrant) & XN_ANGLE_MASK;
}

s32 xn_math_plane_y_at(const xn_vec3 *n, const xn_vec3 *c, s32 x, s32 z)
{
    xn_tri_work *w = &xn_math_tri_edges;
    xn_s64 d;

    if (n->y == 0)
        return n->x;
    w->n = *n;
    xn_s64_mul(&d, x - c->x, n->x);
    xn_s64_mac(&d, z - c->z, n->z);
    return c->y - xn_s64_div(&d, n->y);
}

void xn_math_plane_y_at_r(xn_regs *r)
{
    xn_vec3 n;

    n.x = r->eax;
    n.y = r->edx;
    n.z = r->ebx;
    r->eax = xn_math_plane_y_at(&n, (const xn_vec3 *)r->ecx, r->esi, r->edi);
}

/* (a*b - c*d) >> 8 with a 64-bit difference */
static s32 cross8(s32 a, s32 b, s32 c, s32 d)
{
    xn_s64 t;

    xn_s64_mul(&t, a, b);
    xn_s64_msub(&t, c, d);
    return xn_s64_shr(&t, 8);
}

s32 xn_math_triangle_y_at(const xn_vec3 *tri, s32 x, s32 z)
{
    xn_tri_work *w = &xn_math_tri_edges;
    s32 ny;
    xn_s64 h;

    w->e1.x = tri[1].x - tri[0].x;
    w->e1.y = tri[1].y - tri[0].y;
    w->e1.z = tri[1].z - tri[0].z;
    w->e2.x = tri[2].x - tri[1].x;
    w->e2.y = tri[2].y - tri[1].y;
    w->e2.z = tri[2].z - tri[1].z;
    w->q.x = x - tri[0].x;
    w->q.z = z - tri[0].z;
    w->q.y = tri[0].y;
    ny = -cross8(w->e1.z, w->e2.x, w->e1.x, w->e2.z);
    if (ny == 0)
        return 0;                       /* edge-on */
    w->ny = ny;
    w->nq_x = -cross8(w->e1.z, w->q.x, w->e1.x, w->q.z);
    w->nq_z = -cross8(w->q.z, w->e2.x, w->q.x, w->e2.z);
    xn_s64_mul(&h, w->nq_z, w->e1.y);
    xn_s64_mac(&h, w->nq_x, w->e2.y);
    return xn_s64_div(&h, w->ny) + w->q.y;
}

void xn_math_rotate_xz(s32 *x, s32 *z, s32 angle)
{
    s32 x16 = *x << 4, z16 = *z << 4;

    angle &= XN_ANGLE_MASK;
    *x = xn_mulhi(x16, xn_cos_table[angle]) + xn_mulhi(z16, xn_sin_table[angle]);
    *z = xn_mulhi(z16, xn_cos_table[angle]) - xn_mulhi(x16, xn_sin_table[angle]);
}

void xn_math_rotate_xz_r(xn_regs *r)
{
    s32 x = r->eax, z = r->edx;

    xn_math_rotate_xz(&x, &z, r->ebx);
    r->eax = x;
    r->edx = z;
}

s32 xn_math_isqrt_lookup(u32 v)
{
    u32 top, low, shift = 0;

    if (v == 0)
        return 0;
    /* down to 12 bits, by an even shift (half of it comes back on the root) */
    top = xn_bsr(v, 0);
    if (top >= 12) {
        shift = top - 11;
        shift += shift & 1;
        v >>= shift;
    }
    /* take out the trailing zero bits, in pairs */
    low = xn_bsf(v, 0) & ~1u;
    v >>= low;
    return (s32)(((u32)xn_math_sqrt_table[v] << (low / 2 + shift / 2)) >> 2);
}
