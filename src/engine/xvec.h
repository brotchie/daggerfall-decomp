/* xvec.h: XnGine's vector functions (src/engine/vec.c; see xngine.h): unit vectors and
   lengths, cross products, the angles of a direction, steps along a direction.

   Each declaration keeps the function's asm interface (config/xngine_abi.csv): no pragma is
   Watcom's own convention; a pragma names the registers; NAME_r is the glue for a function
   whose asm callers read several registers or flags. */
#ifndef XVEC_H
#define XVEC_H

#include "xngine.h"

/* xn_vec_unit_to_angles_regs keeps its input here (0xC1F00) */
extern xn_vec3 xn_vec_angles_in;

/* The unit vector (1.0 = 0x4000) from `from` to `to` into out; returns out, or 0 when the
   points are the same (out unchanged). */
xn_vec3 *xn_vec_unit_direction(const xn_vec3 *from, const xn_vec3 *to, xn_vec3 *out);

/* pos += dir * dist (dir a unit vector, 1.0 = 0x4000); returns pos, or 0 (pos unchanged)
   when dist >= 0x10000. */
xn_vec3 *xn_vec_advance(const xn_vec3 *dir, s32 dist, xn_vec3 *pos);

/* In place: a direction (x, y, z) becomes its angles (pitch, yaw, 0). */
void xn_vec_dir_to_angles(xn_vec3 *v);

/* The angles of a direction: normalised and taken to 2.28, then as
   xn_vec_unit_to_angles_regs. Returns the pitch; the yaw to *yaw. */
s32 xn_vec_dir_to_angles_regs(s32 x, s32 y, s32 z, s32 *yaw);
void xn_vec_dir_to_angles_regs_r(xn_regs *r);

/* Dead: in place, a 2.28 unit vector becomes its angles (pitch, yaw, 0); returns the pitch. */
s32 xn_vec_unit_to_angles(xn_vec3 *v);

/* The angles of a 2.28 unit vector: the yaw is the arc sine of -x (mirrored when z < 0), the
   pitch the arc sine of -y over cos(yaw) (+-1 when that is out of range). Returns the
   pitch; the yaw to *yaw. */
s32 xn_vec_unit_to_angles_regs(s32 x, s32 y, s32 z, s32 *yaw);
void xn_vec_unit_to_angles_regs_r(xn_regs *r);

/* v = (v * dist + 0x2000) >> 14: a unit vector (1.0 = 0x4000) scaled to a length. Returns 0
   (v unchanged) when dist >= 0x10000; the asm's CF says so. */
int xn_vec_scale_unit14(xn_vec3 *v, s32 dist);
void xn_vec_scale_unit14_r(xn_regs *r);

/* Dead: out = (a * wa + b * wb) / 1024, rounded, with 64-bit products. */
void xn_vec_blend(s32 wa, const xn_vec3 *a, s32 wb, const xn_vec3 *b, xn_vec3 *out);
void xn_vec_blend_r(xn_regs *r);

/* out = a x b / 256, rounded, with 64-bit products. */
void xn_vec_cross(const xn_vec3 *a, const xn_vec3 *b, xn_vec3 *out);
void xn_vec_cross_r(xn_regs *r);

/* The length of (x, y, z): the square root of the 64-bit sum of squares. */
s32 xn_vec_length(s32 x, s32 y, s32 z);

/* An approximate length: the largest magnitude plus a quarter of the other two. terms: the
   three terms of that sum, in x, y, z order (the asm leaves them in its registers). */
s32 xn_vec_length_approx(s32 x, s32 y, s32 z, xn_vec3 *terms);
void xn_vec_length_approx_r(xn_regs *r);

/* In place: v as a 16.16 unit vector (xn_vec_normalize). */
void xn_vec_normalize_ptr(xn_vec3 *v);

/* v * 65536 / |v|: a 16.16 unit vector; a zero vector stays as it is. */
void xn_vec_normalize(xn_vec3 *v);
void xn_vec_normalize_r(xn_regs *r);

/* Dead: v * 2^28 / |v|: a 2.28 unit vector. */
void xn_vec_normalize_q28(xn_vec3 *v);
void xn_vec_normalize_q28_r(xn_regs *r);

/* Dead: v scaled to the length 2^shift through a reciprocal: (v * (2^(32+shift) / |v|)) >> 32. */
void xn_vec_normalize_shift(xn_vec3 *v, u32 shift);
void xn_vec_normalize_shift_r(xn_regs *r);

/* Dead: the normal (p1 - p0) x (p2 - p1) / 256 of the triangle tri[0..2] (the edges kept in
   xn_math_tri_edges). */
void xn_vec_triangle_normal(const xn_vec3 *tri, xn_vec3 *out);
void xn_vec_triangle_normal_r(xn_regs *r);

#endif
