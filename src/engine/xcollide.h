/* xcollide.h: XnGine's collision tests (src/engine/collide.c: the geometric tests;
   colmodel.c: models, probes and flats). Canonical C: plain prototypes, Watcom's own calling
   convention; docs/xngine_canonical.md.

   What it does
     The game asks whether a segment (a missile's step, a line of sight, a ray) or a probe (a
     mover's set of spheres) meets an ARCH3D model, or whether a segment meets a flat
     (billboard). A model carries collision spheres, each listing the faces near it: a test
     keeps the spheres it meets, merges their face lists, and tests those faces: a segment by
     where it crosses a face's plane, a probe sphere by its distance from the plane and the
     face's edges. The hits go to a list at big_buffer that the game reads. Most of the
     geometric primitives below them are dead code (no caller): they keep their canonical
     form, tested by their records and the differential tests.

   Fixed point and units
     world        world units; a model's position and a probe's spheres
     model space  (world - the model's position) << 8 (24.8), in the model's own axes (the
                  inverse of its rotation, xn_mat_from_angles of its angles)
     normals      a model's normals have 8 fraction bits; the plane tests take them << 8 (16
                  fraction bits); a hit's normal is in world axes with 8 fraction bits
     products     the tests sum 64-bit products and keep 16 fraction bits, rounded (+8000h),
                  unless a function says otherwise; a divide whose quotient does not fit gives
                  0 (Q-SYS-01)
     faces        a face's point slot is a byte offset into the model's points (12 a point);
                  xn_face_edge_tables gives each point count's edges as slot offsets

   Memory
     big_buffer   the hit list (struct xn_collide_hits: the game reads it), and the tests'
                  work areas at + 1000h (the probe spheres that meet a model: pointers),
                  + 1400h (those spheres in model space), + 2000h and + 3000h (the merged face
                  lists, swapped after each merge): game-visible chunks, written as the asm does
     tables       xn_face_edge_tables (by point count), xn_collide_flat_anchor_shift (by a
                  flat's anchor) in object 2, read only
   The working state the asm kept in globals and in its own code (xn_collide_work, the seg
   and sph states, xn_collide_normal, ...) is in locals here (config/xngine_dropped.csv).

   Quirks kept: Q-COLL-01 .. Q-COLL-05, Q-COLL-08, Q-COLL-09; dropped: Q-COLL-06, Q-COLL-07
   (unreachable). docs/engine/quirks.md. */
#ifndef XCOLLIDE_H
#define XCOLLIDE_H

#include "xwshare.h"

extern const s32 *xn_face_edge_tables[25];  /* by point count: point slot offsets (8 a point)
                                               of the edges, the first again at the end */
extern u8 xn_collide_flat_anchor_shift[];   /* by a flat's anchor: [anchor * 2] */

/* ---- planes ------------------------------------------------------------------------------ */

/* A segment p0 -> p1 against the plane through c with normal n (16 fraction bits): -1 unless
   the ends are on opposite sides and the segment is not nearly parallel (n . (p1 - p0),
   rounded >> 16, outside -8..7); else the crossing's t from p0 (16.16) and the crossing
   point to *hit. A vertical segment (the same x and z) uses only the y terms. */
s32 xn_collide_segment_plane(const xn_vec3 *n, const xn_vec3 *c, const xn_vec3 *p0,
                             const xn_vec3 *p1, xn_vec3 *hit);

/* Dead: the vertical case of xn_collide_segment_plane on its own (the direction's y term
   only; the ends' sides still take the whole normal, so for a segment that is not vertical a
   crossing's t may be negative). */
s32 xn_collide_vsegment_plane(const xn_vec3 *n, const xn_vec3 *c, const xn_vec3 *p0,
                              const xn_vec3 *p1, xn_vec3 *hit);

/* Dead: where the line p0 -> p1 meets the plane (c, n), without any test: its t (a line
   parallel to the plane: 0) and the point to *hit. */
s32 xn_collide_line_plane_point(const xn_vec3 *n, const xn_vec3 *c, const xn_vec3 *p0,
                                const xn_vec3 *p1, xn_vec3 *hit);

/* Dead: the same for a vertical line (the direction's y term only). */
s32 xn_collide_vline_plane_point(const xn_vec3 *n, const xn_vec3 *c, const xn_vec3 *p0,
                                 const xn_vec3 *p1, xn_vec3 *hit);

/* Dead: the signed distance of s from the plane (c, n), rounded. Q-COLL-01: its z term uses
   c's y. */
s32 xn_collide_plane_distance(const xn_vec3 *n, const xn_vec3 *c, const xn_vec3 *s);

/* Dead: a point of the line where the planes (n1 through p1) and (n2 through p2) meet: on the
   axis plane the line crosses most squarely, from the 2 x 2 system of the other two axes, to
   *point. Returns 0 (no point) when the planes are nearly parallel (each component of
   n1 x n2 within +-1600h) or the system has no solution. */
int xn_collide_plane_plane_line(const xn_vec3 *n1, const xn_vec3 *p1, const xn_vec3 *n2,
                                const xn_vec3 *p2, xn_vec3 *point);

/* A sphere (centre s, radius r) against the plane (c, n): r - |distance| (not negative: they
   touch); the signed distance to *dist. */
s32 xn_collide_sphere_plane(const xn_vec3 *n, const xn_vec3 *c, const xn_vec3 *s, s32 r,
                            s32 *dist);

/* Dead: the same for a normal in the x-z plane (n's y is not read), and in the y-z plane (n's
   x is not read). */
s32 xn_collide_sphere_plane_xz(const xn_vec3 *n, const xn_vec3 *c, const xn_vec3 *s, s32 r,
                               s32 *dist);
s32 xn_collide_sphere_plane_yz(const xn_vec3 *n, const xn_vec3 *c, const xn_vec3 *s, s32 r,
                               s32 *dist);

/* Dead: the z case without the magnitude: r - (c.z - s.z). */
s32 xn_collide_sphere_plane_z(const xn_vec3 *c, const xn_vec3 *s, s32 r);

/* ---- spheres ----------------------------------------------------------------------------- */

/* A segment p0 -> p1 against a sphere (c, r) in model space (64-bit products, 16 fraction
   bits): 1 when an end is inside; else, for the segment's point nearest the centre (found
   from p1), r^2 - d^2 (not negative: a hit); -1 when that point is beyond the segment's ends
   or the segment has no length. */
s32 xn_collide_segment_sphere_fx(const xn_vec3 *c, s32 r, const xn_vec3 *p0, const xn_vec3 *p1);

/* The same in world units (32-bit products, Q-COLL-09): the bounding-sphere tests. */
s32 xn_collide_segment_sphere(const xn_vec3 *c, s32 r, const xn_vec3 *p0, const xn_vec3 *p1);

/* Dead: the segment's ends and middle against a sphere, by the approximate distance
   (xn_collide_point_in_sphere_approx): the first of the three that is not negative, else the
   middle's. */
s32 xn_collide_segment_sphere_approx(const xn_vec3 *c, s32 r, const xn_vec3 *p0,
                                     const xn_vec3 *p1);

/* Two spheres: the high dword of (r1 + r2)^2 - |c1 - c2|^2 (negative: apart). */
s32 xn_collide_sphere_sphere(const xn_vec3 *c1, s32 r1, const xn_vec3 *c2, s32 r2);

/* Dead: r1 + r2 - the approximate distance (xn_vec_length_approx). */
s32 xn_collide_sphere_sphere_approx(const xn_vec3 *c1, s32 r1, const xn_vec3 *c2, s32 r2);

/* A point p against a sphere (c, r): the high dword of r^2 - |p - c|^2 (negative: outside). */
s32 xn_collide_point_in_sphere(const xn_vec3 *c, s32 r, const xn_vec3 *p);

/* r - the approximate distance from c to p (xn_vec_length_approx). */
s32 xn_collide_point_in_sphere_approx(const xn_vec3 *c, s32 r, const xn_vec3 *p);

/* The point p against an upright cylinder standing on its middle at c (diameter d, height h):
   -1 above or below it, else the high dword of (d/2)^2 - the x-z distance^2. */
s32 xn_collide_point_in_cylinder(const xn_vec3 *p, const xn_vec3 *c, s32 d, s32 h);

/* Dead: whether p - b lies between 0 and the extents e (a box) or e on every axis (a cube),
   by signs: negative inside. */
s32 xn_collide_point_in_box(const xn_vec3 *p, const xn_vec3 *e, const xn_vec3 *b);
s32 xn_collide_point_in_cube(const xn_vec3 *p, s32 e, const xn_vec3 *b);

/* ---- faces ------------------------------------------------------------------------------- */

/* A point p in the plane of a face (its points in model space, its 8-bit normal) against its
   edges: 1 inside (or on); else the high dword of the first edge's (v0 x v1) . normal that is
   negative (v0, v1: the edge's ends less p, the cross product's terms >> 16). */
s32 xn_collide_point_in_face(const xn_vec3 *p, const struct xn_model_face *face,
                             const xn_vec3 *points, const xn_vec3 *normal);

/* Dead: the same with the edges << 8 (the products' high dwords) and the dot product's low
   dword >> 8; 0 outside an edge (on one too), else 1. */
int xn_collide_point_in_face_v2(const xn_vec3 *p, const struct xn_model_face *face,
                                const xn_vec3 *points, const xn_vec3 *normal);

/* Dead: a face's area: the lengths of its fan's cross products (xn_vec_length), halved. */
s32 xn_collide_face_area(const struct xn_model_face *face, const xn_vec3 *points);

/* A sphere (c, r) against a face's edges (xn_collide_segment_sphere_fx each, the last edge
   back to the first point): 1 on the first that meets it, -1 when none does. */
s32 xn_collide_face_test_edges(const xn_vec3 *c, const struct xn_model_face *face,
                               const xn_vec3 *points, s32 r);

/* Dead: a sphere against a face's corners: -1 when none is inside; else Q-COLL-02: the
   centre's x (the asm pops it back over the test's result). */
s32 xn_collide_face_test_vertices(const xn_vec3 *c, const struct xn_model_face *face,
                                  const xn_vec3 *points, s32 r);

/* Dead (an assembler stub): calls xn_collide_sphere_plane, xn_collide_point_in_face and
   xn_collide_face_test_edges with the registers it is entered with, each fed the one before's
   leftover registers. It has no meaning as C, and does nothing here (Q-COLL-07, dropped). */
void xn_collide_ref_helpers(void);

/* ---- models, probes, flats (colmodel.c) --------------------------------------------------- */

/* A segment start -> end (world units) against the model of handle h: its bounding sphere
   (mode 2 stops there: 0), the collision spheres the segment meets (mode 1 stops at the
   first: 0) and their faces, merged; each face it crosses inside the face's edges is a hit
   (the crossing in world units, the normal in world axes, the face, t / 2). Returns the hit
   list (big_buffer), or -1 for none; Q-COLL-03: 0 (a hit) for a model without collision
   spheres. Nine game sites. */
s32 xn_collide_segment_model(struct xn_model_handle *h, const xn_vec3 *start,
                             const xn_vec3 *end, s32 mode);

/* Dead: two models' bounding spheres only: 0 when they meet, else -1. (The asm has a detailed
   sphere-and-face test after its exits, 14A710, which nothing reaches: Q-COLL-04.) */
s32 xn_collide_model_model(struct xn_model_handle *a, struct xn_model_handle *b, s32 mode);

/* A probe (a set of spheres) against the model of handle h: the probe spheres that meet the
   model's bounding sphere (mode 2 stops there: 0), moved into model space; the collision
   spheres they meet, their faces merged; each face a probe sphere touches inside its edges,
   or on one, is a hit (the face, its normal; t -1). Returns the hit list, or -1; 0 for a
   model without collision spheres (Q-COLL-03). Seven game sites. Q-COLL-06 (dropped): mode
   1 with a hit among the collision spheres unbalances the asm's stack; here it returns 0. */
s32 xn_collide_spheres_model(struct xn_model_handle *h, struct xn_collide_probe *p, s32 mode);

/* Dead: a segment against a probe's spheres (in the probe's axes, without the << 8): 0 when
   one meets it, else -1. */
s32 xn_collide_segment_spheres(struct xn_collide_probe *p, const xn_vec3 *start,
                               const xn_vec3 *end);

/* Dead: a probe against another: a hit for each pair of their spheres that meet (the pair's
   numbers in the hit's face field: a's in the low word, b's in the high); the hit list, or
   -1. Q-COLL-08: the probes' offset is taken in 24.8, their spheres in world units. */
s32 xn_collide_spheres_spheres(struct xn_collide_probe *a, struct xn_collide_probe *b);

/* Dead: a stub: always -1 (its two arguments are not used). */
s32 xn_collide_miss_stk(s32 a, s32 b);

/* A collision test stubbed out: always -1. */
s32 xn_collide_miss(void);

/* A segment start -> end against a flat (billboard) at pos, of its image's size times
   scale / 256 and anchored by flags (bits 1-4): its bounding sphere (mode 2 stops there: 0),
   then the upright plane through its axis facing the segment, and the upright cylinder of
   its size: one hit (Q-COLL-05: its normal has the flat's position added). Returns the hit
   list, or -1. image: archive << 7 | record. */
s32 xn_collide_segment_flat(const xn_vec3 *pos, const xn_vec3 *start, const xn_vec3 *end,
                            u32 image, u32 flags, s32 scale, s32 mode);

/* The same: the game's entry (its last three arguments on the stack). One game site. */
s32 xn_collide_segment_flat_stk(const xn_vec3 *pos, const xn_vec3 *start, const xn_vec3 *end,
                                u32 image, u32 flags, s32 scale, s32 mode);

/* Dead (a tool's): a model's collision spheres: a grid of cells (r * sqrt 2 apart) over its
   points' bounding box, each a sphere of 1.2 r listing the faces it touches, written to out
   (struct xn_model_sphere one after another). Returns the bytes written, -1 when the grid or
   the face lists do not fit in big_buffer; the spheres' count to *count (unset on -1). */
s32 xn_collide_build_model_spheres(struct xn_model *m, s32 r, u8 *out, s32 *count);

#endif
