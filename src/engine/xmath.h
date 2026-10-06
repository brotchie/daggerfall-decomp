/* xmath.h: XnGine's scalar math (src/engine/math.c). Canonical C: plain prototypes, Watcom's
   own calling convention; docs/xngine_canonical.md.

   What it does
     Approximate distances (octagonal: the larger axis plus a fraction of the smaller), 2.28
     fixed-point products, integer square roots (bit by bit in 32 and 64 bits, and a
     table-driven one), arc sines and cosines over the 2048-step tables, the direction from
     one point to another, steps along a yaw and a pitch, and the height of a triangle (the
     terrain's ground under a point).

   Fixed point and units
     angles     2048 steps to a turn (11 bits). Tables are indexed with the angle & 0x7FF
                unless a function says otherwise.
     2.28       sines, cosines and other values in [-1, 1]: 1.0 = 0x10000000 (XN_ONE28).
     world      positions and distances are the game's world units (integers).

   Tables (data in object 2, read only)
     xn_sin_table, xn_cos_table   (xngine.h) sin and cos of k/2048 of a turn in 2.28;
     xn_math_asin_table           asin(i/512) in 2048ths for i = -512..511 (centred at
                                  0x15DC00: index it -512..511);
     xn_math_sqrt_table           4*sqrt(i) for i = 0..4095, a byte each.

   Divisions that the asm let fault (the quotient out of range, or a divisor of 0) give 0, as
   XnGine's divide-error handler made them (Q-SYS-01; xngine.h's _or0 helpers). The quirks
   callers can see are kept; docs/engine/quirks.md has each one (Q-MATH-nn) with its asm. */
#ifndef XMATH_H
#define XMATH_H

#include "xngine.h"

extern s32 xn_math_asin_table[];        /* asin(i/512) in 2048ths, i = -512..511 (centred) */
extern u8 xn_math_sqrt_table[4096];     /* 4*sqrt(i), i = 0..4095 */

/* ---- distances ------------------------------------------------------------------------ */

/* An approximate ground distance between (x1, z1) and (x2, z2): dx + dz - min(dx, dz) / 2,
   i.e. the larger axis distance plus half the smaller (an octagon: exact along an axis, at
   most 12% long).
   46 game sites (sounds, AI ranges). Q-MATH-01: the halving is unsigned. */
s32 xn_math_approx_dist2d(s32 x1, s32 z1, s32 x2, s32 z2);

/* An approximate length of the 2D vector (a, b): the larger magnitude plus a quarter of the
   smaller (at most 3% long, 12% short). 17 game sites. */
s32 xn_math_approx_hypot(s32 a, s32 b);

/* ---- products --------------------------------------------------------------------------- */

/* (a * b) >> 28 with a 64-bit product: a times a 2.28 factor. Four game sites each; the two
   are the same routine twice in the asm. */
s32 xn_math_fixmul28(s32 a, s32 b);
s32 xn_math_fixmul28_v2(s32 a, s32 b);

/* a * sin(angle) >> 28. Q-MATH-05: the angle is not masked (an angle outside 0..2047 reads
   past the table, as the asm did). One game site. */
s32 xn_math_mul_sin(s32 a, s32 angle);

/* ---- square roots ----------------------------------------------------------------------- */

/* The integer square root of v by bit-by-bit trial, from bit 15 at most. v is taken as a
   signed value and the trial squares wrap at 32 bits (Q-MATH-04): for v >= 2^31 (a sum of
   squares that overflowed) the result is the asm's, not a square root. isqrt(0) = 0 (the
   asm's answer for 0 depends on its caller's ECX: xn_math_isqrt_zero, Q-MATH-02). Four game
   sites (through xn_math_isqrt_b). */
s32 xn_math_isqrt(s32 v);

/* What the asm's isqrt returns for 0 when the caller's ECX holds ecx: 0, unless ecx - 1 is
   negative with its low 5 bits >= 16; then a value set by those bits (0xFFFF4AFB for ecx = 0,
   0x7FFF4AFB for ecx = -1) (Q-MATH-02). */
s32 xn_math_isqrt_zero(s32 ecx);

/* The game's calls of xn_math_isqrt (boundary adapter: the game's registers; Q-MATH-02). */
void xn_math_isqrt_b(xn_regs *r);

/* The integer square root of the unsigned 64-bit value hi:lo by bit-by-bit trial (from bit 30
   when hi is not 0). */
u32 xn_math_isqrt64(u32 lo, s32 hi);

/* A fast square root of v from the 4096-entry table: v scaled down to 12 bits by an even
   shift, its trailing zero bits taken out in pairs, looked up, scaled back; the table's 4x is
   divided out at the end (truncated: the table's 8 bits, about 0.5% for large v). */
s32 xn_math_isqrt_lookup(u32 v);

/* ---- angles ----------------------------------------------------------------------------- */

/* The arc sine of v (2.28): the angle 0..2047 whose sine is nearest v, v clamped to +-1. The
   coarse guess comes from xn_math_asin_table, then a walk along xn_sin_table. */
s32 xn_math_asin(s32 v);

/* The arc cosine of v (2.28): the angle 0..2047 whose cosine is nearest v, v clamped. */
s32 xn_math_acos(s32 v);

/* Dead: the table's coarse arc sine of a 2.28 value (no refinement), and that plus three
   quarters of a turn as a coarse arc cosine. */
s32 xn_math_asin_coarse(s32 v);
s32 xn_math_acos_coarse(s32 v);

/* The direction from (x1, z1) to (x2, z2) in 2048ths of a turn: the arc cosine of dz over
   the distance (at least 8), mirrored when dx >= 0. 20 game sites (facing, AI). Q-MATH-04:
   the squared distance wraps at 32 bits for far points; Q-MATH-02 for coincident ones. */
s32 xn_math_angle_to_point(s32 x1, s32 z1, s32 x2, s32 z2);

/* Dead: the angle (0..2047) of the vector (x, y): folded into the first quadrant, the arc
   cosine of its first coordinate over its length. */
s32 xn_math_angle_xy(s32 x, s32 y);

/* ---- steps along angles ------------------------------------------------------------------ */

/* *x = sin(yaw) * dist, *z = cos(yaw) * dist (2.28 products): the ground offset of a step of
   dist along yaw. 14 game sites. */
void xn_math_yaw_offset_xz(s32 yaw, s32 dist, s32 *x, s32 *z);

/* v += (sin yaw, sin pitch, cos yaw) * dist: a step along yaw that also climbs by pitch (not
   a unit vector: the ground step is not shortened). Four game sites. */
void xn_math_advance_pitch_yaw(s32 pitch, s32 yaw, s32 dist, xn_vec3 *v);

/* The vector of length len at angles a (from the y axis) and b (around it): (sin a cos b,
   sin a sin b, cos a) * len, each product the high dword of (len << 4) times a 2.28 value.
   The sky's stars. */
void xn_math_angles_to_vector(s32 a, s32 b, s32 len, xn_vec3 *out);

/* Dead: (x, z) rotated by angle: x' = x cos + z sin, z' = z cos - x sin (2.28, the high dwords
   of the products of the values << 4). */
void xn_math_rotate_xz(s32 *x, s32 *z, s32 angle);

/* ---- planes and triangles ---------------------------------------------------------------- */

/* The height at (x, z) of the triangle tri[0..2] (the terrain's ground): from the normal
   (e1 x e2) and the point's offset, all >> 8 with 64-bit products; 0 when the triangle is
   edge-on (its normal's y is 0). */
s32 xn_math_triangle_y_at(const xn_vec3 *tri, s32 x, s32 z);

/* Dead: the y at (x, z) of the plane through c with normal n, which it leaves in
   xn_collide_normal (the collision tests' shared normal). Q-MATH-06: when n.y is 0 it returns
   n.x. */
s32 xn_math_plane_y_at(const xn_vec3 *n, const xn_vec3 *c, s32 x, s32 z);

/* ---- dead and broken ---------------------------------------------------------------------- */

/* Dead: (a - b) / d, unsigned; the remainder to *rem. */
u32 xn_math_diff_div(u32 a, u32 b, u32 d, u32 *rem);

/* Dead and broken (Q-MATH-03): (v*v)^2. It was meant as an e^x series. */
s32 xn_math_exp_series(s32 v);

/* Dead: the high dword of (110 - a) * b over d (at least 1); the remainder to *rem. */
s32 xn_math_scale_110(s32 a, s32 b, s32 d, s32 *rem);

#endif
