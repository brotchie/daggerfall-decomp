/* xmath.h: XnGine's math functions (src/engine/math.c; see xngine.h): approximate distances,
   2.28 fixed-point products, square roots, arc sines and cosines over the 2048-step tables,
   angles of points and the height of a triangle.

   Each declaration keeps the function's asm interface (config/xngine_abi.csv): no pragma is
   Watcom's own convention; a pragma names the registers; NAME_r is the glue for a function
   whose asm callers read several registers or flags. */
#ifndef XMATH_H
#define XMATH_H

#include "xngine.h"

/* The arc sine table: asin(i/512) in 2048ths of a turn for i = -512..511 (mid at 0x15DC00) */
extern s32 xn_math_asin_table[];
/* The square-root table: 4*sqrt(i) for i = 0..4095 */
extern u8 xn_math_sqrt_table[4096];

/* The triangle and plane routines' working storage (0x14BB00): the asm keeps its
   intermediate values here, so the C does too (callers can see them) */
typedef struct xn_tri_work {
    xn_vec3 e1;             /* 00 p1 - p0 */
    xn_vec3 e2;             /* 0C p2 - p1 */
    xn_vec3 q;              /* 18 the query point less p0 in x and z; y: p0's */
    s32 unused24[3];        /* 24 */
    s32 ny;                 /* 30 the normal's y (e1 x e2, >> 8) */
    s32 nq_x;               /* 34 the height's terms: -(e1 x q).y >> 8 */
    s32 nq_z;               /* 38 and -(q x e2).y >> 8 */
    xn_vec3 n;              /* 3C xn_math_plane_y_at's normal */
    s32 unused48[6];        /* 48 */
    s32 w_a, w_b;           /* 60 xn_vec_blend's weights, << 10 */
} xn_tri_work;
extern xn_tri_work xn_math_tri_edges;

/* An approximate ground distance between (x1, z1) and (x2, z2): the larger axis distance plus
   half the smaller (an octagon). 46 game sites. */
s32 xn_math_approx_dist2d(s32 x1, s32 z1, s32 x2, s32 z2);

/* An approximate length of (a, b): the larger magnitude plus a quarter of the smaller. */
s32 xn_math_approx_hypot(s32 a, s32 b);

/* Dead: (a - b) / d, unsigned; the remainder to *rem. */
u32 xn_math_diff_div(u32 a, u32 b, u32 d, u32 *rem);
void xn_math_diff_div_r(xn_regs *r);

/* Dead and broken: (v*v)^2. It was meant as an e^x series; what is left of the rest is a
   division that can raise a divide error. */
s32 xn_math_exp_series(s32 v);
#pragma aux xn_math_exp_series parm [edx] value [eax] modify exact [eax];

/* The direction from (x1, z1) to (x2, z2) in 2048ths of a turn: the arc cosine of dz over the
   distance (at least 8), mirrored when dx >= 0. */
s32 xn_math_angle_to_point(s32 x1, s32 z1, s32 x2, s32 z2);

/* (a * b) >> 28: a 2.28 product (the same as xn_math_fixmul28). */
s32 xn_math_fixmul28_v2(s32 a, s32 b);

/* The vector of length len at angles a (from the y axis) and b (around it): (sin a cos b,
   sin a sin b, cos a) * len. */
void xn_math_angles_to_vector(s32 a, s32 b, s32 len, xn_vec3 *out);
void xn_math_angles_to_vector_r(xn_regs *r);

/* Dead: the high dword of (110 - a) * b over d, at least 1; the remainder to *rem. */
s32 xn_math_scale_110(s32 a, s32 b, s32 d, s32 *rem);
void xn_math_scale_110_r(xn_regs *r);

/* a * sin(angle) >> 28 (the angle unmasked, as the asm reads the table) */
s32 xn_math_mul_sin(s32 a, s32 angle);

/* *x = sin(yaw) * dist, *z = cos(yaw) * dist (2.28 products) */
void xn_math_yaw_offset_xz(s32 yaw, s32 dist, s32 *x, s32 *z);
#pragma aux xn_math_yaw_offset_xz parm [eax] [edx] [ebx] [ecx] modify exact [eax edx];

/* v += (sin yaw, sin pitch, cos yaw) * dist: a step along yaw, climbing by pitch */
void xn_math_advance_pitch_yaw(s32 pitch, s32 yaw, s32 dist, xn_vec3 *v);

/* (a * b) >> 28 */
s32 xn_math_fixmul28(s32 a, s32 b);

/* The coarse arc sine of a 2.28 value: a table lookup, no refinement (dead) */
s32 xn_math_asin_coarse(s32 v);

/* The angle (0..2047) whose sine is nearest v (2.28, clamped to +-1) */
s32 xn_math_asin(s32 v);

/* The coarse arc cosine: the coarse arc sine plus three quarters of a turn (dead) */
s32 xn_math_acos_coarse(s32 v);

/* The angle (0..2047) whose cosine is nearest v (2.28, clamped to +-1) */
s32 xn_math_acos(s32 v);

/* The integer square root of v (v < 2^31) by bit-by-bit trial from bit 15 down at most. For
   v = 0 the asm's `bsr` leaves its register as the caller had it: ecx is that value. */
s32 xn_math_isqrt(s32 v, s32 ecx);
#pragma aux xn_math_isqrt parm [eax] [ecx] value [eax] modify exact [eax edx ebx];

/* The integer square root of the 64-bit hi:lo by bit-by-bit trial (the row's ECX input is
   only the asm's `bsr` of a zero lo, which does not change the result) */
u32 xn_math_isqrt64(u32 lo, s32 hi);
#pragma aux xn_math_isqrt64 parm [eax] [edx] value [eax] modify exact [eax edx];

/* Dead: the angle (0..2047) of the vector (x, y): the quadrant, then the arc cosine of its
   first coordinate over the length. */
s32 xn_math_angle_xy(s32 x, s32 y);
#pragma aux xn_math_angle_xy parm [eax] [edx] value [eax] modify exact [eax edx];

/* Dead: the y at (x, z) of the plane through c with normal n; n.x when n.y is 0. */
s32 xn_math_plane_y_at(const xn_vec3 *n, const xn_vec3 *c, s32 x, s32 z);
void xn_math_plane_y_at_r(xn_regs *r);

/* The height at (x, z) of the triangle of the three points at tri (0 when it is edge-on) */
s32 xn_math_triangle_y_at(const xn_vec3 *tri, s32 x, s32 z);

/* Dead: (x, z) rotated by angle: x' = x cos + z sin, z' = z cos - x sin (2.28) */
void xn_math_rotate_xz(s32 *x, s32 *z, s32 angle);
void xn_math_rotate_xz_r(xn_regs *r);

/* The square root of v from the 4096-entry table: v scaled down to 12 bits by an even shift,
   its trailing zero pairs taken out, looked up, scaled back. (The row's ECX input is the
   asm's `bsr` of a zero v, which returns 0 before it.) */
s32 xn_math_isqrt_lookup(u32 v);
#pragma aux xn_math_isqrt_lookup parm [eax] value [eax] modify exact [eax];

#endif
