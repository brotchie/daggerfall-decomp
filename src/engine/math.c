/* math.c: XnGine's scalar math (canonical C; the interface and the module's documentation are
   in xmath.h).

   Angles are 2048 steps to a turn (11 bits; tables index them masked with 0x7FF). Sines,
   cosines and other unit values are 2.28 fixed point (1.0 = 0x10000000). Products that need
   64 bits use xngine.h's helpers (Watcom C32 10.0a has no 64-bit integer type); divisions that
   the asm let fault use the _or0 helpers (arith.c), which give its results without the
   exception. Quirks of the asm that callers can see are kept and marked "Quirk Q-..."
   (docs/engine/quirks.md). */
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
    /* Quirk Q-MATH-01: an unsigned halving (shr), and |most negative| stays negative */
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
    return xn_u64_divrem_or0(&n, d, rem);
}

s32 xn_math_exp_series(s32 v)
{
    s32 sq = v * v;

    /* Quirk Q-MATH-03: the series' terms were lost in the asm; what is left squares v twice
       (its one division, by 6, only ever mattered for the divide error it could raise) */
    return sq * sq;
}

/* The asm's isqrt of 0: its `bsr` leaves the caller's ECX as the top bit (Quirk Q-MATH-02):
   the bit-by-bit search then starts from bit (ecx - 1) (at most 15, the shift count masked to
   5 bits) and accepts every bit whose square's low 32 bits are not above 0. */
s32 xn_math_isqrt_zero(s32 ecx)
{
    s32 top = ecx - 1;
    s32 root = 0, t;
    u32 bit;

    if (top > 15)
        top = 15;
    for (bit = 1u << (top & 31); bit != 0; bit >>= 1) {
        t = root + (s32)bit;
        if (t * t <= 0)
            root = t;
    }
    return root;
}

s32 xn_math_angle_to_point(s32 x1, s32 z1, s32 x2, s32 z2)
{
    s32 dx = x2 - x1;
    s32 dz = z2 - z1;
    s32 sq = dx * dx + dz * dz;         /* 32 bits: far points wrap (Quirk Q-MATH-04) */
    s32 len, a;
    xn_s64 n;

    /* the asm's register for isqrt's top bit holds the low dword of dz << 28 here */
    len = sq != 0 ? xn_math_isqrt(sq) : xn_math_isqrt_zero(dz << 28);
    if ((u32)len <= 8)
        len = 8;
    xn_s64_set(&n, dz);
    xn_s64_shl(&n, 28);
    a = xn_math_acos(xn_s64_div_or0(&n, len));
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

s32 xn_math_scale_110(s32 a, s32 b, s32 d, s32 *rem)
{
    xn_s64 n;
    s32 q;

    n.lo = xn_mulhi(0x6E - a, b);
    n.hi = 0;
    q = xn_s64_divrem_or0(&n, d, rem);
    return q != 0 ? q : 1;
}

s32 xn_math_mul_sin(s32 a, s32 angle)
{
    /* Quirk Q-MATH-05: the angle is not masked */
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

/* v clamped to [-1.0, 1.0] (2.28) */
static s32 clamp_unit(s32 v)
{
    if (v < -XN_ONE28)
        return -XN_ONE28;
    if (v > XN_ONE28)
        return XN_ONE28;
    return v;
}

s32 xn_math_asin(s32 v)
{
    v = clamp_unit(v);
    return nearest_angle(xn_sin_table, xn_math_asin_coarse(v) & XN_ANGLE_MASK, v);
}

s32 xn_math_acos_coarse(s32 v)
{
    return (xn_math_asin_coarse(v) + 0x600) & XN_ANGLE_MASK;
}

s32 xn_math_acos(s32 v)
{
    v = clamp_unit(v);
    return nearest_angle(xn_cos_table, xn_math_acos_coarse(v), v);
}

s32 xn_math_isqrt(s32 v)
{
    s32 root = 0, t, top;
    u32 bit;

    if (v == 0)
        return 0;                       /* (the asm: Q-MATH-02, xn_math_isqrt_zero) */
    top = xn_bsr(v, 0) - 1;
    if (top > 15)
        top = 15;
    /* Quirk Q-MATH-04: the squares are compared as signed 32-bit values (they wrap) */
    for (bit = 1u << (top & 31); bit != 0; bit >>= 1) {
        t = root + (s32)bit;
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
    return (xn_math_acos(xn_s64_div_or0(&n, len)) + quadrant) & XN_ANGLE_MASK;
}

s32 xn_math_plane_y_at(const xn_vec3 *n, const xn_vec3 *c, s32 x, s32 z)
{
    xn_s64 d;

    if (n->y == 0)
        return n->x;                    /* Quirk Q-MATH-06: n.x, not a height */
    xn_collide_normal = *n;
    xn_s64_mul(&d, x - c->x, n->x);
    xn_s64_mac(&d, z - c->z, n->z);
    return c->y - xn_s64_div_or0(&d, n->y);
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
    xn_vec3 e1, e2;
    s32 qx, qz, ny, hx, hz;
    xn_s64 h;

    e1.x = tri[1].x - tri[0].x;
    e1.y = tri[1].y - tri[0].y;
    e1.z = tri[1].z - tri[0].z;
    e2.x = tri[2].x - tri[1].x;
    e2.y = tri[2].y - tri[1].y;
    e2.z = tri[2].z - tri[1].z;
    qx = x - tri[0].x;
    qz = z - tri[0].z;
    ny = -cross8(e1.z, e2.x, e1.x, e2.z);       /* the normal's y, (e1 x e2).y >> 8 */
    if (ny == 0)
        return 0;                               /* edge-on */
    hx = -cross8(e1.z, qx, e1.x, qz);
    hz = -cross8(qz, e2.x, qx, e2.z);
    xn_s64_mul(&h, hz, e1.y);
    xn_s64_mac(&h, hx, e2.y);
    return xn_s64_div_or0(&h, ny) + tri[0].y;
}

void xn_math_rotate_xz(s32 *x, s32 *z, s32 angle)
{
    s32 x16 = *x << 4, z16 = *z << 4;

    angle &= XN_ANGLE_MASK;
    *x = xn_mulhi(x16, xn_cos_table[angle]) + xn_mulhi(z16, xn_sin_table[angle]);
    *z = xn_mulhi(z16, xn_cos_table[angle]) - xn_mulhi(x16, xn_sin_table[angle]);
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

/* ---- the game's calls (boundary adapters) ------------------------------------------------- */

/* The game calls xn_math_isqrt at four sites. The asm's answer for 0 depends on the caller's
   ECX (Q-MATH-02); the game's site 080FFC (intrface.c: the sine of a direction, x * x in ECX)
   reaches 0 when x is 0x10000 and z is 0. This keeps what the game got. */
void xn_math_isqrt_b(xn_regs *r)
{
    r->eax = r->eax != 0 ? xn_math_isqrt(r->eax) : xn_math_isqrt_zero(r->ecx);
}
