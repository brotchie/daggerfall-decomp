/* xmodel.h: XnGine's ARCH3D models (src/engine/model.c). Canonical C: plain prototypes,
   Watcom's own calling convention; docs/xngine_canonical.md.

   What it does
     Preparing a model once it is loaded (older files' vertex offsets, the faces' texture
     axes and plane constants, the radius, the normals, and the checks that end the game on a
     bad file); a placement's angles; the per-frame queue (a model the view does not cull,
     keyed by its distance); and the draw: unless the S-buffer already hides its bounds, its
     object-to-view matrix in the frame's pool, the eye in object space, its faces (back faces
     culled, vertices transformed once per draw, faces clipped, projected and walked into the
     S-buffer, each one that shows a polygon record with its texture and first-span setup),
     its light list and the texture-gradient matrix. Also the game's helpers: the push out of
     a building's face, a model's top.

   Fixed point and units
     a model       struct xn_model: points (xn_vec3, 24.8 world units) and normals (8-bit
                   fraction after prepare) by offsets from its start; faces of 8-byte points,
                   whose vertex is a byte offset (index * 12) into the point list and into the
                   renderer's vertex arrays
     a placement   struct xn_model_handle: the world position, the angles (2048 to a turn),
                   rel_x/y/z (the position seen from the eye, << 8), the frame's matrix slot
                   and light list
     matrices      2.28 rotations; the pool slot is the object-to-view matrix (the view's x and
                   y scales folded in), then the gradient matrix (xn_model_scale_matrix)
     texture axes  per textured face: u/z and v/z gradients in object space (2^25 / |e|^2
                   scaled edges)

   Tables and state (object 2): the model queue (xn_model_queue, _ptr, _count; 200), the
   counts, the matrices of the angle helpers (xn_model_base_matrix, _object_matrix,
   _combined_matrix) and of the model being drawn (xn_model_rot_matrix), the renderer's
   vertex arrays (xn_vert_cam, _screen, _flags), pools and lights (xpipe.h), and the shared
   scratch vector a (preparing leaves a face's edge there: pick_distance is game-visible,
   Q-MODEL-03). The asm passed a draw's handle, eye, matrix, depth row and point list to its
   helpers through operands of their code (smc MODEL-*); here they travel in a struct
   xn_model_draw_state.

   Quirks kept: Q-MODEL-01 (the dead extent swaps its minimum and maximum), Q-MODEL-02 (a
   count of 0 runs 2^32 times), Q-MODEL-03 (preparing leaves an edge in the pick's point),
   Q-MODEL-04 (the occlusion test offsets x in 16 bits), Q-MODEL-05 (the back-face test's last
   add is compared without its wrap). Dropped: Q-MODEL-06 (a model of 1024 or more vertices
   overwrote the vertex-flag clear's code). docs/engine/quirks.md. */
#ifndef XMODEL_H
#define XMODEL_H

#include "xpipe.h"

/* ---- tables and state (object 2) --------------------------------------------------------- */
extern s32 xn_model_angle_or_bits;      /* or-ed into the base angles (xn_model_compose_angles) */
extern xn_mat3 xn_model_base_matrix, xn_model_object_matrix, xn_model_combined_matrix;
extern xn_mat3 xn_model_rot_matrix;     /* the model being drawn */
extern struct xn_sort_pair xn_model_queue[200];     /* the frame's models: {key, handle} */
extern struct xn_sort_pair *xn_model_queue_ptr;
extern s32 xn_model_queue_count, xn_model_drawn_count;
extern char xn_model_msg_too_many_verts[], xn_model_msg_corrupted[];

/* ---- a placement's angles ------------------------------------------------------------------ */

/* The game passes &handle->pad_0c (the instance's "angles"): the base angles are the words at
   +2, +4, +6 of it, the draw angles the dwords at +20h, +24h, +28h. */

/* The base angles (pitch, yaw, roll) of the instance at `angles`. 3 game sites. */
void xn_model_set_angles(s32 pitch, s32 yaw, s32 roll, s16 *angles);

/* The draw angles of the instance: its base rotation (the base angles | xn_model_angle_or_bits)
   times the object's (pitch, yaw, roll), back into angles; when that fails, the base angles
   (sign-extended). 2 game sites. */
void xn_model_compose_angles(s16 *angles, s32 pitch, s32 yaw, s32 roll);

/* The draw angles: the base angles (zero-extended), the yaw plus yaw_offset. 2 game sites. */
void xn_model_set_angles_yaw_offset(s16 *angles, s32 yaw_offset);

/* ---- the game's helpers ----------------------------------------------------------------- */

/* When the player leaves a building: the picked face's normal through the model's rotation
   (in big_buffer) pushes the player half a unit out along x and z. One game site. */
void xn_model_push_player_from_pick(const struct xn_poly *pick);

/* The highest vertex y of the model (at least -100000). One game site. */
s32 xn_model_max_y(const struct xn_model *m);

/* Dead: the model's x and z extents (>> 8, unsigned) to *dx, *dz. The minima start at -100000
   and the maxima at 100000 (Q-MODEL-01): the extents are at least 200000 >> 8. */
void xn_model_xz_extent(const struct xn_model *m, u32 *dx, u32 *dz);

/* Dead: the mean of the model's vertices (truncated; 0 for a model of no vertices,
   Q-SYS-01) into xn_pick_view_x/_y and pick_distance. _to_pick takes the placement. */
void xn_model_calc_centroid(const struct xn_model *m);
void xn_model_centroid_to_pick(const struct xn_model_handle *h);

/* ---- preparing a model ------------------------------------------------------------------------ */

/* Prepares a loaded model: files before "v2.6" get their vertex offsets (index * 4) times 3,
   and end the game with 'XnGine: Object has too many vertices.' at 1024 vertices or more;
   then the texture axes, plane constants, radius and normals (8-bit fraction); point 0's u/v
   << 4; and the checks: more than 24 points a face ends the game ('... too many vertices'),
   a vertex offset that is not a multiple of 12 or past the vertices too ('... is
   corrupted'). Returns m. One game site (model_get, which goes on with the model in EAX). */
struct xn_model *xn_model_prepare(struct xn_model *m);

/* The texture axes of every textured face (texture >= 100h), for each animation frame (once
   when there are none). */
void xn_model_calc_uv_axes(struct xn_model *m);

/* One face's texture axes: e1 = p1 - p0, and e2 = p2 - p1 less its part along e1, each
   scaled by 2^25 / |e|^2 (64-bit); then u_axis = du1 e1' + du2' e2' and the same for v, du2'
   being the second delta less its part along e1. A degenerate face (|e|^2 >> 8 of 0) is
   left alone. e1 is kept in the shared scratch vector a (Q-MODEL-03). */
void xn_model_calc_face_uv_axes(const struct xn_model *m, const struct xn_model_face *face,
                                struct xn_model_face_data *out);

/* Each face's plane constant (its first point . its normal) and its texture-axis pointer
   (XN_SET_FACE_DATA, xnstruct.h: natively the axes' offset from the face, the file's 4-byte
   slot being too narrow for an address). */
void xn_model_calc_face_planes(struct xn_model *m);

/* The radius: the largest vertex distance from the origin. */
void xn_model_calc_radius(struct xn_model *m);

/* Every face's normal with `bits` fraction bits. */
void xn_model_calc_face_normals(struct xn_model *m, s32 bits);

/* A face's normal: (p1 - p0) x (p2 - p1) normalised to 16.16, then >> (16 - bits) when that
   is positive. p1 - p0 is kept in the shared scratch vector a (Q-MODEL-03). */
void xn_model_calc_face_normal(const struct xn_model *m, const struct xn_model_face *face,
                               xn_vec3 *out, s32 bits);

/* ---- the frame's queue ----------------------------------------------------------------------- */

/* Queues the model of a placement for this frame, with its animation frame. 8 game sites. */
void xn_model_submit(struct xn_model_handle *h, s32 frame);

/* The placement's position relative to the eye (<< 8, into rel_x/y/z); unless the view culls
   its sphere (xn_cam_cull_sphere), an entry of the model queue (the 200th and later are not
   queued), keyed by its distance less its radius (0 when that is negative or the centre is
   behind the eye). */
void xn_model_cull_and_queue(struct xn_model_handle *h, u8 frame);

/* ---- drawing --------------------------------------------------------------------------------- */

/* Draws a queued model: unless the S-buffer already hides its bounds (it is then flagged,
   flags bit 0), its rotation, the object-to-view matrix in the pool's next slot, the eye in
   object space (into rel_x/y/z), the depth row, the faces, the light list and the gradient
   matrix (the slot is then used). Returns 1 when a texture lookup failed (the cache is full;
   the slot is not used). xn_render_draw_models. */
int xn_model_draw(struct xn_model_handle *h);

/* The model's animation frame: the frame's point, normal and face-data offsets into the
   header (frame clamped to 0 .. frame_count - 1), then the plane constants. Never run:
   Daggerfall's models have no frames (frame_count 0 in all of ARCH3D.BSA). */
void xn_model_set_frame(struct xn_model *m, s32 frame);

/* The faces of the model being drawn (s: its handle, eye, matrix and depth row; the point list
   goes into s->points): the vertex flags cleared; each front face (the eye on the normal's
   side of its plane, Q-MODEL-05) has its vertices transformed, and unless they are all outside
   one plane, its 1/z gradient, its projection into spans, and when it reached the screen a
   polygon record with its texture's cache entry and the render mode's setup. Returns 1 when a
   texture lookup failed. */
int xn_model_draw_faces(const struct xn_model *m, struct xn_model_draw_state *s);

/* A face's vertices not yet transformed in this draw: into view space (the point plus the eye
   through s->matrix), their outcodes, and when inside the view their projections. Returns the
   outcodes' AND (low byte) and OR (high byte) over the face's vertices. */
u32 xn_model_transform_face_verts(const struct xn_model_face *face,
                                  const struct xn_model_draw_state *s);

/* The first n vertices' done flags set to value (n at most 1024: the asm's body of 1024
   stores, Q-MODEL-06). */
void xn_model_clear_vert_flags(struct xn_vert_flags *flags, int n, u8 value);

/* The model's light list (from xn_render_light_list_next, ended by -1; the handle's lights
   point at it): each light of the frame, up to the first of intensity 0 or less, that reaches
   the model's sphere (directional lights always), in object space. */
void xn_model_build_light_list(struct xn_model_handle *h);

/* The slot's gradient matrices: rows 0 and 1 times the view's inverse scales (high dwords,
   doubled) into the light copy (+1C20h), then times 2^36 / focal_x and 2^33 / focal_y; row 2
   copied and doubled. */
void xn_model_scale_matrix(struct xn_model_matrix_slot *slot);

/* Is the model (more than 4 faces) hidden: its sphere's screen bounds (clipped) all covered
   by spans nearer than its centre (Q-MODEL-04)? */
int xn_model_is_occluded(const struct xn_model *m, const struct xn_model_handle *h);

/* The screen bounds of the model's sphere: the centre through the eye's rotation, +- the
   radius projected (xn_cam_project) about the view centre, into *x0..*y1, and the centre's
   1/z (2^40 / z) into *inv_z; the centre in view space to *c. Returns 0 when the centre is
   not beyond the near plane. */
int xn_model_project_bounds(const struct xn_model *m, const struct xn_model_handle *h,
                            s32 *x0, s32 *y0, s32 *x1, s32 *y1, s32 *inv_z, xn_vec3 *c);

#endif
