/* xvec.h: XnGine's 3D vectors (src/engine/vec.c). Canonical C: plain prototypes, Watcom's own
   calling convention; docs/xngine_canonical.md.

   What it does
     Unit vectors (three precisions), lengths (exact and approximate), cross products and
     blends, the angles of a direction, and steps along a direction. The camera, collision,
     models and the game's movement code use them.

   Fixed point and units
     xn_vec3      x, y, z (world units, or a unit vector in one of the formats below)
     unit 2.14    1.0 = 0x4000: xn_vec_unit_direction, xn_vec_advance, xn_vec_scale_unit14
     unit 16.16   1.0 = 0x10000: xn_vec_normalize
     unit 2.28    1.0 = 0x10000000: xn_vec_normalize_q28, the angle routines' input
     angles       2048 steps to a turn; (pitch, yaw) pairs as xmat.h's rotations use them

   Lengths use 64-bit sums of squares and xn_math_isqrt64 (xmath.h). Divisions that the asm
   let fault give 0 (Q-SYS-01). */
#ifndef XVEC_H
#define XVEC_H

#include "xngine.h"

/* ---- directions and steps --------------------------------------------------------------- */

/* The unit vector (2.14: 1.0 = 0x4000) from `from` to `to` into out; returns out, or 0 (out
   unchanged) when the points are the same. Four game sites. */
xn_vec3 *xn_vec_unit_direction(const xn_vec3 *from, const xn_vec3 *to, xn_vec3 *out);

/* pos += dir * dist (dir a 2.14 unit vector, rounded); returns pos, or 0 (pos unchanged) when
   dist >= 0x10000. Five game sites. */
xn_vec3 *xn_vec_advance(const xn_vec3 *dir, s32 dist, xn_vec3 *pos);

/* v = (v * dist + 0x2000) >> 14: a 2.14 unit vector scaled to the length dist (32-bit
   products). Returns 0 (v unchanged) when dist >= 0x10000, else 1. */
int xn_vec_scale_unit14(xn_vec3 *v, s32 dist);

/* ---- angles ------------------------------------------------------------------------------ */

/* The angles of the direction (x, y, z): normalised, taken to 2.28, then as
   xn_vec_unit_to_angles_regs. Returns the pitch; the yaw to *yaw. The camera. */
s32 xn_vec_dir_to_angles_regs(s32 x, s32 y, s32 z, s32 *yaw);

/* In place: the direction v becomes its angles (pitch, yaw, 0). One game site. */
void xn_vec_dir_to_angles(xn_vec3 *v);

/* The angles of a 2.28 unit vector: the yaw is the arc sine of -x (mirrored to the other
   side when z < 0), the pitch the arc sine of -y / cos(yaw) (+-1.0 when that ratio is not
   below 1, or cos(yaw) is 0). Returns the pitch; the yaw to *yaw. */
s32 xn_vec_unit_to_angles_regs(s32 x, s32 y, s32 z, s32 *yaw);

/* Dead: in place, a 2.28 unit vector becomes its angles (pitch, yaw, 0); returns the pitch. */
s32 xn_vec_unit_to_angles(xn_vec3 *v);

/* ---- lengths and unit vectors -------------------------------------------------------------- */

/* The length of (x, y, z): the square root of the 64-bit sum of squares. */
s32 xn_vec_length(s32 x, s32 y, s32 z);

/* An approximate length of (x, y, z): the largest magnitude plus a quarter of each of the
   other two (at most 6% long, 13% short). Collision's quick sphere tests. */
s32 xn_vec_length_approx(s32 x, s32 y, s32 z);

/* The three terms of that sum, in x, y, z order: the largest axis' magnitude and the others'
   quarters. (The axes are compared as signed values, so an axis of -2^31 never counts as the
   largest.) */
void xn_vec_length_approx_terms(s32 x, s32 y, s32 z, xn_vec3 *terms);

/* In place: v * 65536 / |v|, a 16.16 unit vector; a zero vector stays as it is. */
void xn_vec_normalize(xn_vec3 *v);

/* The same, for the game (three game sites). */
void xn_vec_normalize_ptr(xn_vec3 *v);

/* Dead: in place, v * 2^28 / |v|, a 2.28 unit vector. */
void xn_vec_normalize_q28(xn_vec3 *v);

/* Dead: in place, v scaled to the length 2^shift through a reciprocal:
   (v * (2^(32+shift) / |v|)) >> 32 (0 when the reciprocal does not fit in 32 bits). */
void xn_vec_normalize_shift(xn_vec3 *v, u32 shift);

/* ---- products ------------------------------------------------------------------------------ */

/* out = a x b / 256, each component a 64-bit difference of products, rounded. Model normals. */
void xn_vec_cross(const xn_vec3 *a, const xn_vec3 *b, xn_vec3 *out);

/* Dead: out = (a * wa + b * wb) / 1024, rounded, with 64-bit products. */
void xn_vec_blend(s32 wa, const xn_vec3 *a, s32 wb, const xn_vec3 *b, xn_vec3 *out);

/* Dead: the normal (p1 - p0) x (p2 - p1) / 256 of the triangle tri[0..2] (not rounded). */
void xn_vec_triangle_normal(const xn_vec3 *tri, xn_vec3 *out);

#endif
