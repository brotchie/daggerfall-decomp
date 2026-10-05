/* xmodel.h: XnGine's ARCH3D models (readable C, model.c; see xngine.h): preparing a model once
   loaded (vertex offsets, texture axes, face planes, radius, normals, checks), the per-frame
   queue (culled by the view, sorted by distance), and the draw: the occlusion test against the
   S-buffer, the object-to-camera matrix, the faces (back faces culled, vertices transformed
   lazily, textures looked up), the light list and the texture-gradient matrix. The draw passes
   its values to its helpers through patched operands (smc/model.md).

   Each declaration keeps the function's asm interface (its row in config/xngine_abi.csv): no
   pragma is Watcom's own convention; a pragma names the registers; NAME_r is the glue for a
   function whose asm callers read several registers or flags. */
#ifndef XMODEL_H
#define XMODEL_H

#include "xpipe.h"

/* The game passes &handle->pad_0c (its instance's angles) to these three: the base angles are
   the words at +2, +4, +6 of it, the draw angles the dwords at +20h, +24h, +28h. */

/* The base angles (pitch, yaw, roll) of the instance at `angles`. */
void xn_model_set_angles(s32 pitch, s32 yaw, s32 roll, s16 *angles);

/* The draw angles of the instance: its base rotation (the base angles | xn_model_angle_or_bits)
   times the object's (pitch, yaw, roll), back into angles; when that fails, the base angles
   (sign-extended). */
void xn_model_compose_angles(s16 *angles, s32 pitch, s32 yaw, s32 roll);

/* The draw angles: the base angles (zero-extended), the yaw plus yaw_offset. */
void xn_model_set_angles_yaw_offset(s16 *angles, s32 yaw_offset);

/* Dead: the mean of the model's vertices (handle->model) into xn_pick_view_x/y and
   pick_distance. (Keeps every register.) */
void xn_model_centroid_to_pick(const struct xn_model_handle *h);
#pragma aux xn_model_centroid_to_pick parm [eax] modify exact [eax];

/* When the player leaves a building: the picked face's normal through the model's rotation
   pushes the player half a unit out (x and z). */
void xn_model_push_player_from_pick(const struct xn_poly *pick);

/* The highest vertex y of the model (at least -100000). */
s32 xn_model_max_y(const struct xn_model *m);

/* Dead: the model's x and z extents (>> 8) to *dx, *dz. The asm starts its minima at -100000
   and maxima at 100000 (swapped): the extents are at least 200000 >> 8 (a bug, kept). (Keeps
   every register.) */
void xn_model_xz_extent(const struct xn_model *m, u32 *dx, u32 *dz);
#pragma aux xn_model_xz_extent parm [eax] [edx] [ebx] modify exact [eax];

/* Prepares a loaded model: older files' vertex offsets (index * 4) times 3; the faces' texture
   axes, plane constants, the radius and the normals (8-bit fraction); point 0's u/v << 4; and
   the checks (24 points a face, offsets multiples of 12 within the vertices), which end the
   program with 'XnGine: Object has too many vertices.' / '... is corrupted'. */
void xn_model_prepare(struct xn_model *m);
void xn_model_prepare_r(xn_regs *r);

/* The texture axes of every textured face (texture >= 100h), for each animation frame (once
   when there are none). */
void xn_model_calc_uv_axes(struct xn_model *m);
#pragma aux xn_model_calc_uv_axes parm [eax] modify exact [eax ecx edx esi];

/* One face's texture axes: e1 = p1 - p0, e2 = p2 - p1 made orthogonal to e1, both scaled by
   2^25 / |e|^2 (64-bit), then u_axis = du1 e1' + du2' e2' and the same for v (du2' the second
   delta less its part along e1). The vectors are kept in the shared scratch (xn_pick_view_x..,
   xn_scratch_vec_b, xn_model_uv_len2, xn_model_uv_t). A degenerate face is left alone. */
void xn_model_calc_face_uv_axes(const struct xn_model *m, const struct xn_model_face *face,
                                struct xn_model_face_data *out);
#pragma aux xn_model_calc_face_uv_axes parm [esi] [ebx] [edi] modify exact [eax edx];

/* Queues the model of a handle for this frame, with its animation frame. */
void xn_model_submit(struct xn_model_handle *h, s32 frame);

/* The handle's position relative to the eye (<< 8); unless the view culls its sphere, an entry
   of the model queue, keyed by its distance less its radius (0 when negative or behind). */
void xn_model_cull_and_queue(struct xn_model_handle *h, u8 frame);
#pragma aux xn_model_cull_and_queue parm [edi] [eax] modify exact [eax ecx edx ebx esi edi];

/* Draws a queued model: unless the S-buffer already hides its bounds (it is then flagged),
   its matrices (rotation, object to camera in the next pool slot), the eye in object space,
   the depth gradient row, the faces, the light list and the texture-gradient matrix. Returns 1
   (the asm's CF) when a texture lookup failed. */
int xn_model_draw(struct xn_model_handle *h);
void xn_model_draw_r(xn_regs *r);

/* An animated model's frame (no caller: Daggerfall's models have none). */
void xn_model_set_frame(struct xn_model *m, s32 frame);
void xn_model_set_frame_regs_r(xn_regs *r);
void xn_model_set_frame_r(xn_regs *r);

/* The faces of the model being drawn: the vertex flags cleared, the point list patched in;
   each front face (by the eye in object space) gets its vertices transformed, unless they are
   all outside one plane its 1/z gradient, its projection into spans, and when it reached the
   screen a polygon record with its texture's cache entry and first-span setup. Returns 1 (CF)
   when a texture lookup failed (the cache is full). */
int xn_model_draw_faces(const struct xn_model *m);
void xn_model_draw_faces_r(xn_regs *r);

/* A face's vertices not yet transformed in this draw: into camera space (the point plus the
   eye, through the model's matrix), their outcodes, and when inside the frustum their
   projections. Returns the outcodes' AND (low byte) and OR (high byte) over the face. */
u32 xn_model_transform_face_verts(const struct xn_model_face *face);
void xn_model_transform_face_verts_r(xn_regs *r);

/* The model's light list (in xn_render_light_list_pool, ended by -1): each light of the frame
   that reaches it (directional lights always), in object space; the handle points at it. */
void xn_model_build_light_list(void);

/* The texture-gradient matrices of the slot: rows 0 and 1 times the view's inverse scales
   (high dwords, doubled) into the light copy (+1C20h), then times 2^36 / focal_x and
   2^33 / focal_y; row 2 copied and doubled. (The asm leaves the last entry in EDX.) */
void xn_model_scale_matrix(void);
void xn_model_scale_matrix_r(xn_regs *r);

/* Each face's plane constant (its first point . its normal) and its texture-axis pointer. */
void xn_model_calc_face_planes(struct xn_model *m);
#pragma aux xn_model_calc_face_planes parm [eax] modify exact [eax];

/* The radius: the largest vertex distance from the origin. */
void xn_model_calc_radius(struct xn_model *m);
#pragma aux xn_model_calc_radius parm [eax] modify exact [eax ecx edx esi];

/* Every face's normal with `bits` fraction bits. */
void xn_model_calc_face_normals(struct xn_model *m, s32 bits);
#pragma aux xn_model_calc_face_normals parm [eax] [edx] modify exact [eax ecx esi];

/* A face's normal: (p1 - p0) x (p2 - p1) (kept in the shared scratch), normalised to 16.16,
   then >> (16 - bits) when that is positive. */
void xn_model_calc_face_normal(const struct xn_model *m, const struct xn_model_face *face,
                               xn_vec3 *out, s32 bits);
#pragma aux xn_model_calc_face_normal parm [eax] [edx] [ebx] [esi] modify exact [eax edx ebx edi];

/* Dead: the mean of the model's vertices into xn_pick_view_x/y and pick_distance. (Keeps every
   register.) */
void xn_model_calc_centroid(const struct xn_model *m);
#pragma aux xn_model_calc_centroid parm [eax] modify exact [eax];

/* Is the model (more than 4 faces) hidden: its sphere's screen bounds (clipped) all covered by
   spans nearer than its centre? Returns 1 (CF). */
int xn_model_is_occluded(const struct xn_model *m, const struct xn_model_handle *h);
void xn_model_is_occluded_r(xn_regs *r);

/* The screen bounds of the model's sphere (the centre through the eye's rotation, +- the
   radius projected) into *x0..*y1 and xn_model_bounds_*, and its centre's 1/z (2^40 / z);
   the centre in view space to *c. Returns 0 (the asm's CF) when the centre is not beyond the
   near plane. */
int xn_model_project_bounds(const struct xn_model *m, const struct xn_model_handle *h,
                            s32 *x0, s32 *y0, s32 *x1, s32 *y1, s32 *inv_z, xn_vec3 *c);
void xn_model_project_bounds_r(xn_regs *r);

/* The first n vertices' done flags set to value (the asm: a body of 1024 stores stopped by a
   ret planted after n). */
void xn_model_clear_vert_flags(struct xn_vert_flags *flags, int n, u8 value);
void xn_model_clear_vert_flags_r(xn_regs *r);

#endif
