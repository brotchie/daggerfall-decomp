/* xcollide.h: XnGine's collision tests (collide.c: the geometric tests; colmodel.c: models,
   probes and flats; see xngine.h). A segment or a probe (a set of spheres) against an ARCH3D
   model's collision spheres and faces, or against a flat; the hits go to a list in
   big_buffer.

   Units: world units; model space is world units << 8 (24.8), in the model's own axes; normals
   with 16 fraction bits (the model's 8-bit normals << 8) unless said otherwise.

   Each declaration keeps the function's asm interface (config/xngine_abi.csv): no pragma is
   Watcom's own convention; a pragma names the registers; NAME_r is the glue for a function
   whose asm callers read several registers or flags. Many of the helpers leave intermediate
   values in registers their row lists as outputs (read by the dead helpers that chain them);
   their glue computes those leftovers as the asm leaves them, and says so. */
#ifndef XCOLLIDE_H
#define XCOLLIDE_H

#include "xwshare.h"

/* ---- shared state ------------------------------------------------------------------------ */
extern struct xn_collide_scratch xn_collide_work;       /* 0x14A100 */
extern struct xn_collide_seg_state xn_collide_seg;      /* 0x14A68F: the segment test's */
extern struct xn_collide_sph_state xn_collide_sph;      /* 0x14AF0C: the probe test's */
extern xn_vec3 xn_collide_normal;       /* the plane tests' normal (14BB3C) */
extern xn_vec3 xn_collide_mid;          /* 14BB24: the approximate segment test's middle; the
                                           plane-plane test's plane constants */
extern xn_vec3 xn_collide_vec_a;        /* the segment-sphere tests' centre less an end */
extern xn_vec3 xn_collide_vec_b;        /*   and the segment, end to start */
extern xn_vec3 xn_collide_edge_v0;      /* the face tests' edge, from the test point */
extern xn_vec3 xn_collide_edge_v1;
extern xn_vec3 xn_collide_test_point;
extern const xn_vec3 *xn_collide_face_normal_ptr;
extern const xn_vec3 *xn_collide_face_points;
extern const xn_vec3 *xn_collide_edge_points;
extern struct xn_model_face_point *xn_collide_edge_first;
extern const s32 *xn_face_edge_tables[25];  /* by point count: point slot offsets (8 a point)
                                               of the edges, the first again at the end */
extern u8 xn_collide_flat_anchor_shift[];   /* by a flat's anchor: [anchor * 2] */
extern s32 xn_collide_flat_height;          /* the flat's size, world units (in code) */
extern s32 xn_collide_flat_width;
extern const xn_vec3 *xn_collide_vertex_points;     /* 15CEDE's point list (in code) */
extern const xn_vec3 *xn_collide_area_points;       /* 15CD33's (in code) */
extern s32 xn_collide_area_last;
extern struct xn_collide_hits *xn_collide_ss_hits;  /* 14B017's list (in code) */
extern struct xn_collide_hit *xn_collide_ss_hit_next;

/* the dead plane-plane intersection's arguments and axis (in its code) */
extern s32 xn_collide_ppl_axis;
extern const xn_vec3 *xn_collide_ppl_n1, *xn_collide_ppl_p1, *xn_collide_ppl_n2, *xn_collide_ppl_p2;

#pragma pack(1)
/* the dead detailed model-model test's lists (in code after 14A6C0's exit, 0x14AA66) */
struct xn_collide_detail_state {
    struct xn_model_sphere **a_list;    /* +00 A's spheres that meet B's bounding sphere */
    struct xn_model_sphere **a_next;
    s32 a_count;
    struct xn_model_sphere **b_list;    /* +0C B's that meet A's */
    struct xn_model_sphere **b_next;
    s32 b_count;
    s32 pair_count;                     /* +18 the pairs that meet */
    struct xn_model_sphere **pairs;     /* +1C two pointers each: A's, B's */
    struct xn_model_sphere **pairs_next;
    struct xn_model *model;             /* +24 A's */
    xn_vec3 *normals;                   /* +28 */
};
/* the dead sphere builder's state (in code after its exit, 0x14BA7D) */
struct xn_collide_build_state {
    xn_vec3 *points;                    /* +00 */
    xn_vec3 *normals;                   /* +04 the face's normal (advanced per face) */
    struct xn_model_face *face;         /* +08 */
    s32 face_index;                     /* +0C */
    s32 faces_left;                     /* +10 */
    s32 *next_node;                     /* +14 the cells' face lists: {next, face, normal4} */
};
#pragma pack()
extern struct xn_collide_detail_state xn_collide_detail;
extern struct xn_collide_build_state xn_collide_build;

/* ---- other groups (asm or pilot C) ------------------------------------------------------- */
/* the TEXTURE image header of archive's record (tex group, asm): +4 width, +6 height */
u8 *xn_tex_cache_lookup_image(u32 archive, u32 record, s32 frame);
#pragma aux xn_tex_cache_lookup_image parm [eax] [edx] [ebx] value [eax] modify exact [eax ecx edx ebx];

/* ---- planes ------------------------------------------------------------------------------ */

/* A segment p0 -> p1 against the plane through c with normal n (16.16): -1 unless the ends are
   on opposite sides and the segment is not nearly parallel (n . (p1 - p0) within -8..7),
   else t (16.16 from p0) and the crossing point to *hit. A vertical segment (the same x and z)
   uses only the y terms. The normal is kept in xn_collide_normal. */
s32 xn_collide_segment_plane(const xn_vec3 *n, const xn_vec3 *c, const xn_vec3 *p0,
                             const xn_vec3 *p1, xn_vec3 *hit);
void xn_collide_segment_plane_r(xn_regs *r);

/* Dead: the vertical case of xn_collide_segment_plane on its own. */
s32 xn_collide_vsegment_plane(const xn_vec3 *n, const xn_vec3 *c, const xn_vec3 *p0,
                              const xn_vec3 *p1, xn_vec3 *hit);
void xn_collide_vsegment_plane_r(xn_regs *r);

/* Dead: where the line p0 -> p1 meets the plane (c, n), without any test: t to the return, the
   point to *hit (a divide error when the line is parallel: t = 0). */
s32 xn_collide_line_plane_point(const xn_vec3 *n, const xn_vec3 *c, const xn_vec3 *p0,
                                const xn_vec3 *p1, xn_vec3 *hit);
void xn_collide_line_plane_point_r(xn_regs *r);

/* Dead: the same for a vertical line (the y terms of the direction only). */
s32 xn_collide_vline_plane_point(const xn_vec3 *n, const xn_vec3 *c, const xn_vec3 *p0,
                                 const xn_vec3 *p1, xn_vec3 *hit);
void xn_collide_vline_plane_point_r(xn_regs *r);

/* Dead: the distance of s from the plane (c, n), rounded; its z term uses c's y (a bug). */
s32 xn_collide_plane_distance(s32 nx, s32 ny, s32 nz, const xn_vec3 *c, const xn_vec3 *s);
#pragma aux xn_collide_plane_distance parm [eax] [edx] [ebx] [ecx] [esi] value [eax] \
    modify exact [eax edx ebx];

/* Dead: a point of the line where the planes (n1 through p1) and (n2 through p2) meet: on the
   axis plane the line crosses most, by the 2 x 2 system of the other two axes. Returns 0
   (the point undefined: the asm's leftovers) when the planes are nearly parallel. */
int xn_collide_plane_plane_line(const xn_vec3 *n1, const xn_vec3 *p1, const xn_vec3 *n2,
                                const xn_vec3 *p2, xn_vec3 *point);
void xn_collide_plane_plane_line_r(xn_regs *r);

/* A sphere (centre s, radius r) against the plane (c, n): r - |distance| (not negative: they
   touch); the signed distance to *dist. */
s32 xn_collide_sphere_plane(const xn_vec3 *n, const xn_vec3 *c, const xn_vec3 *s, s32 r,
                            s32 *dist);
void xn_collide_sphere_plane_r(xn_regs *r);

/* Dead: the same for a normal in the x-z plane, and in the y-z plane. */
s32 xn_collide_sphere_plane_xz(s32 nx, s32 nz, const xn_vec3 *c, const xn_vec3 *s, s32 r,
                               s32 *dist);
void xn_collide_sphere_plane_xz_r(xn_regs *r);
s32 xn_collide_sphere_plane_yz(s32 ny, s32 nz, const xn_vec3 *c, const xn_vec3 *s, s32 r,
                               s32 *dist);
void xn_collide_sphere_plane_yz_r(xn_regs *r);

/* Dead: the z case without the magnitude: r + s.z - c.z. */
s32 xn_collide_sphere_plane_z(const xn_vec3 *c, const xn_vec3 *s, s32 r);
#pragma aux xn_collide_sphere_plane_z parm [ecx] [esi] [edi] value [eax] modify exact [eax];

/* ---- spheres ----------------------------------------------------------------------------- */

/* A segment p0 -> p1 against a sphere in model space (64-bit products, 16 fraction bits): 1
   when an end is inside; else r^2 - d^2 for the point of the segment nearest the centre
   (seen from p1), not negative on a hit; -1 when that point is outside the segment. */
s32 xn_collide_segment_sphere_fx(const xn_vec3 *c, s32 r, const xn_vec3 *p0, const xn_vec3 *p1);
void xn_collide_segment_sphere_fx_r(xn_regs *r);

/* The same in world units (32-bit products): the bounding-sphere tests. */
s32 xn_collide_segment_sphere(const xn_vec3 *c, s32 r, const xn_vec3 *p0, const xn_vec3 *p1);
void xn_collide_segment_sphere_r(xn_regs *r);

/* Dead: the segment's ends and middle against a sphere, by the approximate distance: the
   first not negative of the three tests (the middle's otherwise). */
s32 xn_collide_segment_sphere_approx(s32 cx, s32 cy, s32 cz, s32 r, const xn_vec3 *p0,
                                     const xn_vec3 *p1);
#pragma aux xn_collide_segment_sphere_approx parm [eax] [edx] [ebx] [ecx] [esi] [edi] \
    value [eax] modify exact [eax edx ebx esi];

/* Two spheres: the high dword of (r1 + r2)^2 - d^2 (negative: apart). */
s32 xn_collide_sphere_sphere(const xn_vec3 *c1, s32 r1, const xn_vec3 *c2, s32 r2);
void xn_collide_sphere_sphere_r(xn_regs *r);

/* Dead: r1 + r2 - the approximate distance. */
s32 xn_collide_sphere_sphere_approx(s32 cx, s32 cy, s32 cz, s32 r1, const xn_vec3 *c2, s32 r2);
#pragma aux xn_collide_sphere_sphere_approx parm [eax] [edx] [ebx] [ecx] [esi] [edi] \
    value [eax] modify exact [eax edx ebx];

/* A point p against a sphere: the high dword of r^2 - |p - c|^2 (negative: outside). */
s32 xn_collide_point_in_sphere(s32 cx, s32 cy, s32 cz, s32 r, const xn_vec3 *p);
#pragma aux xn_collide_point_in_sphere parm [eax] [edx] [ebx] [ecx] [esi] value [eax] \
    modify exact [eax edx ebx];

/* r - the approximate distance from c to p. */
s32 xn_collide_point_in_sphere_approx(const xn_vec3 *c, s32 r, const xn_vec3 *p);
void xn_collide_point_in_sphere_approx_r(xn_regs *r);

/* The point (x, y, z) against an upright cylinder standing on its middle at c (diameter d,
   height h): -1 above or below it, else the high dword of (d/2)^2 - the x-z distance^2. */
s32 xn_collide_point_in_cylinder(s32 x, s32 y, s32 z, const xn_vec3 *c, s32 d, s32 h);
#pragma aux xn_collide_point_in_cylinder parm [eax] [edx] [ebx] [ecx] [esi] [edi] value [eax] \
    modify exact [eax ecx edx esi edi];

/* Dead: whether p - b lies between 0 and the extents e on every axis, by signs (negative:
   inside). */
s32 xn_collide_point_in_box(s32 x, s32 y, s32 z, const xn_vec3 *e, const xn_vec3 *b);
#pragma aux xn_collide_point_in_box parm [eax] [edx] [ebx] [ecx] [esi] value [eax] \
    modify exact [eax edx ebx];
s32 xn_collide_point_in_cube(s32 x, s32 y, s32 z, s32 e, const xn_vec3 *b);
#pragma aux xn_collide_point_in_cube parm [eax] [edx] [ebx] [ecx] [esi] value [eax] \
    modify exact [eax edx ebx];

/* ---- faces -------------------------------------------------------------------------------- */

/* A point in the plane of a face against its edges: 1 inside (or on); else the high dword of
   the first edge's (v0 x v1) . normal that is negative. */
s32 xn_collide_point_in_face(const xn_vec3 *p, const struct xn_model_face *face,
                             const xn_vec3 *points, const xn_vec3 *normal);
void xn_collide_point_in_face_r(xn_regs *r);

/* Dead: the same with 8 more fraction bits and an 8-bit normal; returns 0 (CF) outside. */
int xn_collide_point_in_face_v2(const xn_vec3 *p, const struct xn_model_face *face,
                                const xn_vec3 *points, const xn_vec3 *normal);
void xn_collide_point_in_face_v2_r(xn_regs *r);

/* Dead: a face's area: the lengths of its fan's cross products, halved. */
s32 xn_collide_face_area(const struct xn_model_face *face, const xn_vec3 *points);
#pragma aux xn_collide_face_area parm [eax] [edx] value [eax] modify exact [eax edx];

/* A sphere (c, r) against a face's edges (xn_collide_segment_sphere_fx each): 1 on the first
   that meets it, -1 none. */
s32 xn_collide_face_test_edges(const xn_vec3 *c, const struct xn_model_face *face,
                               const xn_vec3 *points, s32 r);
void xn_collide_face_test_edges_r(xn_regs *r);

/* Dead: a sphere against a face's corners: -1 when none is inside; else the asm returns the
   centre's x (it pops it back over the test's result: a bug, kept). */
s32 xn_collide_face_test_vertices(s32 cx, s32 cy, s32 cz, const struct xn_model_face *face,
                                  const xn_vec3 *points, s32 r);
#pragma aux xn_collide_face_test_vertices parm [eax] [edx] [ebx] [ecx] [esi] [edi] value [eax] \
    modify exact [eax ecx esi edi];

/* ---- models, probes, flats (colmodel.c) --------------------------------------------------- */

/* A segment start -> end against a model: its bounding sphere (mode 2 stops there: 0), the
   collision spheres it meets (mode 1 stops at the first: 0) and their faces, merged; each face
   it crosses inside its edges is a hit (point and normal in world space). Returns the hit list
   (big_buffer), or -1 for none; 0 also for a model without collision spheres. 9 game sites. */
s32 xn_collide_segment_model(struct xn_model_handle *h, const xn_vec3 *start,
                             const xn_vec3 *end, s32 mode);

/* Dead: two models' bounding spheres only: 0 when they meet, else -1 (the detailed test that
   follows in the code is never reached). */
s32 xn_collide_model_model(struct xn_model_handle *a, struct xn_model_handle *b, s32 mode);
#pragma aux xn_collide_model_model parm [eax] [edx] [ebx] value [eax] modify exact [eax edx ebx];

/* A probe (a set of spheres) against a model: the probe spheres that meet its bounding sphere
   (mode 2 stops there: 0), moved into model space; the collision spheres they meet, their
   faces merged; each face a probe sphere touches inside its edges, or on one, is a hit (the
   face and its normal). Returns the hit list, or -1. 7 game sites. (Mode 1 with a hit among
   the collision spheres unbalances the asm's stack: see the C.) */
s32 xn_collide_spheres_model(struct xn_model_handle *h, struct xn_collide_probe *p, s32 mode);
void xn_collide_spheres_model_r(xn_regs *r);

/* Dead: a segment against a probe's spheres: 0 when one meets it, else -1. */
s32 xn_collide_segment_spheres(struct xn_collide_probe *p, const xn_vec3 *start,
                               const xn_vec3 *end);
#pragma aux xn_collide_segment_spheres parm [eax] [edx] [ebx] value [eax] \
    modify exact [eax edx ebx];

/* Dead: a probe against another: a hit for each pair of spheres that meet (their numbers);
   the hit list, or -1. */
s32 xn_collide_spheres_spheres(struct xn_collide_probe *a, struct xn_collide_probe *b);
#pragma aux xn_collide_spheres_spheres parm [eax] [edx] value [eax] modify exact [eax edx];

/* Dead: a stub: always -1 (its two stack arguments are loaded and dropped). */
s32 xn_collide_miss_stk(s32 a, s32 b);
#pragma aux xn_collide_miss_stk parm routine [] value [eax] modify exact [eax];

/* A collision test stubbed out: always -1. */
s32 xn_collide_miss(void);

/* Dead: a stub that chains three helpers' asm interfaces (the registers flow from one to the
   next). */
void xn_collide_ref_helpers_r(xn_regs *r);

/* A segment against a flat (billboard) at pos, of the image's size times scale / 256 and
   anchored by flags: its bounding sphere (mode 2 stops there: 0), then the upright plane
   through its axis facing the segment and the cylinder of its size: one hit. Returns the hit
   list, or -1. The game's interface: stack arguments. */
s32 xn_collide_segment_flat_stk(const xn_vec3 *pos, const xn_vec3 *start, const xn_vec3 *end,
                                u32 image, u32 flags, s32 scale, s32 mode);

/* The same with the registers (EBP: the mode). */
s32 xn_collide_segment_flat(const xn_vec3 *pos, const xn_vec3 *start, const xn_vec3 *end,
                            u32 image, u32 flags, s32 scale, s32 mode);
void xn_collide_segment_flat_r(xn_regs *r);

/* Dead (a tool's): a model's collision spheres: a grid of cells over its points, each a sphere
   of 1.2 r listing the faces it touches, written to out. Returns the bytes written (-1 when
   the grid or the lists do not fit in big_buffer); the spheres' count to *count. */
s32 xn_collide_build_model_spheres(struct xn_model *m, s32 r, u8 *out, s32 *count);
void xn_collide_build_model_spheres_r(xn_regs *r);

#endif
