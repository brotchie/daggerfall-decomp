/* xmat.h: XnGine's matrix functions (src/engine/mat.c; see xngine.h): 3x3 rotations in 2.28
   fixed point from and to angles, matrix times vector, matrix products, and the general
   n x m products the collision code uses.

   Each declaration keeps the function's asm interface (config/xngine_abi.csv): no pragma is
   Watcom's own convention; a pragma names the registers; NAME_r is the glue for a function
   whose asm callers read several registers or flags. */
#ifndef XMAT_H
#define XMAT_H

#include "xngine.h"

extern s32 xn_cam_scale_x;              /* the view matrix's row scales (xn_cam_scale_matrix) */
extern s32 xn_cam_scale_y;
extern xn_vec3 xn_terrain_step_x;       /* xn_mat_scaled_axes' results */
extern xn_vec3 xn_terrain_step_y;
extern xn_vec3 xn_terrain_step_z;
extern xn_vec3 xn_mat_scaled_axes_in;   /* and its input */

/* xn_mat_from_angles' products of sines and cosines (in code bytes, 0x13719F) */
typedef struct xn_mat_angles_tmp {
    s32 sr_sy;              /* sin roll * sin yaw */
    s32 sy_cr;              /* sin yaw * cos roll */
    s32 sp_cy;              /* sin pitch * cos yaw */
} xn_mat_angles_tmp;
extern xn_mat_angles_tmp xn_mat_from_angles_tmp;

/* The general products' row strides and shifts (in code bytes next to them) */
extern s32 xn_mat_int_strides[2];       /* A's row, B's row, in bytes */
extern s32 xn_mat_fixed_strides[2];
extern s32 xn_mat_fixed_shift;
extern s32 xn_mat_fixed64_strides[2];
extern u16 xn_mat_fixed64_shift;

/* The angles of a rotation: pitch the arc sine of m[2][1], yaw that of m[2][0] / cos(pitch)
   (mirrored by m[2][2]'s sign), roll that of -m[0][1] / cos(pitch) (mirrored by m[1][1]'s).
   Returns 0 (all angles 0) when cos(pitch) is 0 or a ratio is out of range; the asm's CF. */
int xn_mat_to_angles(const xn_mat3 *m, s32 *pitch, s32 *yaw, s32 *roll);
void xn_mat_to_angles_r(xn_regs *r);

/* The rotation of pitch, yaw and roll (2048ths of a turn) into m, 2.28 rounded. */
void xn_mat_from_angles(s32 pitch, s32 yaw, s32 roll, xn_mat3 *m);

/* Dead: m = the 3x3 identity with 1.0 = 2^28. (These three keep every register: the route's
   stub keeps EAX, which Watcom's code never keeps.) */
void xn_mat_identity_q28(xn_mat3 *m);
#pragma aux xn_mat_identity_q28 parm [eax] modify exact [eax];

/* Dead: xn_mat_set_scale. */
void xn_mat_set_scale_thunk(xn_mat3 *m);
#pragma aux xn_mat_set_scale_thunk parm [eax] modify exact [eax];

/* Dead: m = the identity scaled by the view scales; the asm stores the matrix's address where
   the y scale belongs (a bug, kept). */
void xn_mat_set_scale(xn_mat3 *m);
#pragma aux xn_mat_set_scale parm [eax] modify exact [eax];

/* *x, *y, *z = m times (*x, *y, *z) (xn_mat_transform) */
void xn_mat_transform_ptr(s32 *x, s32 *y, s32 *z, const xn_mat3 *m);

/* In place: v = m v, each product the high dword of (v << 4) times the 2.28 entry. */
void xn_mat_transform(xn_vec3 *v, const xn_mat3 *m);
void xn_mat_transform_r(xn_regs *r);

/* *x, *y, *z = m^T times (*x, *y, *z) */
void xn_mat_transform_transposed_ptr(s32 *x, s32 *y, s32 *z, const xn_mat3 *m);

/* In place: v = m^T v, the inverse rotation. */
void xn_mat_transform_transposed(xn_vec3 *v, const xn_mat3 *m);
void xn_mat_transform_transposed_r(xn_regs *r);

/* Dead: a copy of xn_mat_transform_ptr. */
void xn_mat_transform_ptr_v2(s32 *x, s32 *y, s32 *z, const xn_mat3 *m);

/* In place: v = m v with the full 64-bit products >> 28 (no << 4 first: large v do not
   overflow). */
void xn_mat_transform_wide(xn_vec3 *v, const xn_mat3 *m);
void xn_mat_transform_wide_r(xn_regs *r);

/* Dead: a copy of xn_mat_transform_transposed. */
void xn_mat_transform_transposed_v2(xn_vec3 *v, const xn_mat3 *m);
void xn_mat_transform_transposed_v2_r(xn_regs *r);

/* The view-space steps of the axes scaled by a, b and c: m (a, 0, 0), m (0, b, 0) and
   m (0, 0, c) into xn_terrain_step_x, _y and _z. */
void xn_mat_scaled_axes(s32 a, s32 b, s32 c, const xn_mat3 *m);

/* out = a b (2.28; each entry a 64-bit sum >> 28) */
void xn_mat_multiply(const xn_mat3 *a, const xn_mat3 *b, xn_mat3 *out);

/* Dead: C (rows x bcols) = A (rows x inner) B (inner x bcols), 32-bit integer products. */
void xn_mat_mul_int(s32 bcols, s32 rows, s32 *c, s32 inner, const s32 *a, const s32 *b);
#pragma aux xn_mat_mul_int parm [eax] [edx] [ebx] [ecx] [esi] [edi] \
    modify exact [eax edx ebx esi];

/* *out = the dot product of n consecutive entries of a with a column of b
   (xn_mat_int_strides[1] bytes apart). */
void xn_mat_dot_int(s32 n, s32 *out, const s32 *a, const s32 *b);
#pragma aux xn_mat_dot_int parm [ecx] [ebx] [esi] [edi] modify exact [eax];

/* C = A B with 64-bit sums >> shift (fixed point). The shift arrives in EBP: glue. */
void xn_mat_mul_fixed(s32 bcols, s32 rows, s32 *c, s32 inner, const s32 *a, const s32 *b,
                      s32 shift);
void xn_mat_mul_fixed_r(xn_regs *r);

/* *out = (a row . b column) >> xn_mat_fixed_shift, with a 64-bit sum */
void xn_mat_dot_fixed(s32 n, s32 *out, const s32 *a, const s32 *b);
#pragma aux xn_mat_dot_fixed parm [ecx] [ebx] [esi] [edi] modify exact [eax];

/* Dead: C = A B over 8-byte entries (the low dwords multiplied), 64-bit results >> shift.
   The shift arrives in BP: glue. */
void xn_mat_mul_fixed64(s32 bcols, s32 rows, s32 *c, s32 inner, const s32 *a, const s32 *b,
                        u16 shift);
void xn_mat_mul_fixed64_r(xn_regs *r);

/* the 8-byte entries' dot product: a 64-bit result >> xn_mat_fixed64_shift into out[0..1] */
void xn_mat_dot_fixed64(s32 n, s32 *out, const s32 *a, const s32 *b);
#pragma aux xn_mat_dot_fixed64 parm [ecx] [ebx] [esi] [edi] modify exact [eax];

/* m = the n x n identity with 1 << shift on the diagonal */
void xn_mat_identity(s32 *m, s32 shift, s32 n);

/* Dead: the same over 8-byte entries. */
void xn_mat_identity64(s32 *m, s32 shift, s32 n);
#pragma aux xn_mat_identity64 parm [eax] [edx] [ebx] modify exact [eax edx];

/* dst = src transposed (a copy, then swapped in place) */
void xn_mat_transpose_copy3(const xn_mat3 *src, xn_mat3 *dst);

#endif
