/* xmat.h: XnGine's matrices (src/engine/mat.c). Canonical C: plain prototypes, Watcom's own
   calling convention; docs/xngine_canonical.md.

   What it does
     3x3 rotations from and to (pitch, yaw, roll), a rotation times a vector (and its
     transpose: the inverse rotation), rotation products, the view-space steps of the terrain's
     axes, and the general n x m products and identities the collision code uses.

   Fixed point and units
     xn_mat3      m[row][column], 2.28 fixed point (1.0 = 0x10000000); a rotation's rows are
                  the axes of the space it maps to
     angles       2048 steps to a turn; xn_mat_from_angles composes roll, pitch and yaw as the
                  game's objects and camera use them
     transforms   xn_mat_transform takes each product as the high dword of (v << 4) times the
                  2.28 entry (v must fit in 28 bits); xn_mat_transform_wide keeps the 64-bit
                  product >> 28 (any v)
     general      the n x m products take row-major arrays of s32 (or of 8-byte entries, whose
                  low dwords are multiplied, for the 64-bit ones); a stride is in entries;
                  rows, inner and cols must be at least 1 (the asm's loops counted down from
                  them)

   Tables (object 2, read only): xn_sin_table and xn_cos_table (xngine.h).
   The quirks callers can see are kept; docs/engine/quirks.md has each one (Q-MAT-nn). */
#ifndef XMAT_H
#define XMAT_H

#include "xngine.h"
#include "ptrint.h"                     /* iptr: an int that holds an address */

extern s32 xn_cam_scale_x;              /* the view's x and y scales (xcam.h) */
extern s32 xn_cam_scale_y;
extern xn_vec3 xn_terrain_step_x;       /* xn_mat_scaled_axes' results (the terrain reads them) */
extern xn_vec3 xn_terrain_step_y;
extern xn_vec3 xn_terrain_step_z;

/* ---- rotations ------------------------------------------------------------------------------ */

/* The rotation of pitch, yaw and roll (2048ths of a turn) into m, each entry a 2.28 product
   rounded. 12 game sites (objects, the camera). */
void xn_mat_from_angles(s32 pitch, s32 yaw, s32 roll, xn_mat3 *m);

/* The angles of the rotation m: pitch the arc sine of m[2][1]; yaw that of m[2][0] / cos(pitch)
   (mirrored when m[2][2] < 0); roll that of -m[0][1] / cos(pitch) (mirrored when
   m[1][1] < 0). Returns 1, or 0 with all three angles 0 when cos(pitch) is 0 or a ratio is
   above 1. */
int xn_mat_to_angles(const xn_mat3 *m, s32 *pitch, s32 *yaw, s32 *roll);

/* out = a b (each entry the 64-bit sum of three products >> 28). */
void xn_mat_multiply(const xn_mat3 *a, const xn_mat3 *b, xn_mat3 *out);

/* dst = src transposed (the inverse of a rotation). */
void xn_mat_transpose_copy3(const xn_mat3 *src, xn_mat3 *dst);

/* Dead: m = the identity (1.0 = 2^28). */
void xn_mat_identity_q28(xn_mat3 *m);

/* Dead: m = the identity scaled by the view's scales. Q-MAT-01: the asm stores the matrix's
   own address where the y scale belongs (m[1][1]), and m[2][2] stays 1.0. */
void xn_mat_set_scale(xn_mat3 *m);
void xn_mat_set_scale_thunk(xn_mat3 *m);    /* (the same, through one more call) */

/* ---- vectors ------------------------------------------------------------------------------- */

/* In place: v = m v, each product the high dword of (v << 4) times the 2.28 entry. */
void xn_mat_transform(xn_vec3 *v, const xn_mat3 *m);

/* The same on three separate values: (*x, *y, *z) = m (*x, *y, *z). Eight game sites. */
void xn_mat_transform_ptr(s32 *x, s32 *y, s32 *z, const xn_mat3 *m);

/* In place: v = m^T v, the inverse rotation. */
void xn_mat_transform_transposed(xn_vec3 *v, const xn_mat3 *m);

/* The same on three separate values. One game site. */
void xn_mat_transform_transposed_ptr(s32 *x, s32 *y, s32 *z, const xn_mat3 *m);

/* In place: v = m v with the full 64-bit products >> 28 (no << 4 first: large v do not
   overflow). */
void xn_mat_transform_wide(xn_vec3 *v, const xn_mat3 *m);

/* Dead: copies of xn_mat_transform_ptr and xn_mat_transform_transposed. */
void xn_mat_transform_ptr_v2(s32 *x, s32 *y, s32 *z, const xn_mat3 *m);
void xn_mat_transform_transposed_v2(xn_vec3 *v, const xn_mat3 *m);

/* The view-space steps of the axes scaled by a, b and c: m (a, 0, 0), m (0, b, 0) and
   m (0, 0, c) (xn_mat_transform_wide) into xn_terrain_step_x, _y and _z. */
void xn_mat_scaled_axes(s32 a, s32 b, s32 c, const xn_mat3 *m);

/* ---- general products (collision) ------------------------------------------------------------ */

/* The dot product of n consecutive entries of a with a column of b, whose entries are
   b_stride apart: 32-bit products and sum. */
s32 xn_mat_dot_int(s32 n, const s32 *a, const s32 *b, s32 b_stride);

/* Dead: c (rows x cols) = a (rows x inner) b (inner x cols), 32-bit products. */
void xn_mat_mul_int(s32 *c, const s32 *a, const s32 *b, s32 rows, s32 inner, s32 cols);

/* The same dot product with a 64-bit sum, >> shift (0..31). */
s32 xn_mat_dot_fixed(s32 n, const s32 *a, const s32 *b, s32 b_stride, u32 shift);

/* c = a b with 64-bit sums >> shift: fixed-point matrices (collision: 3x3, shift 28). */
void xn_mat_mul_fixed(s32 *c, const s32 *a, const s32 *b, s32 rows, s32 inner, s32 cols,
                      u32 shift);

/* The dot product over 8-byte entries (their low dwords; b's entries are b_stride dwords
   apart): the 64-bit sum >> shift into out[0] (low) and out[1] (high, arithmetic shift). */
void xn_mat_dot_fixed64(s32 n, const s32 *a, const s32 *b, s32 b_stride, u32 shift, s32 out[2]);

/* Dead: c = a b over 8-byte entries, 64-bit results >> shift. */
void xn_mat_mul_fixed64(s32 *c, const s32 *a, const s32 *b, s32 rows, s32 inner, s32 cols,
                        u32 shift);

/* m = the n x n identity with 1 << shift on the diagonal (n at least 1). */
void xn_mat_identity(s32 *m, s32 shift, s32 n);

/* Dead: the same over 8-byte entries. */
void xn_mat_identity64(s32 *m, s32 shift, s32 n);

#endif
