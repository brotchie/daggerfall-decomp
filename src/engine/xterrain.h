/* xterrain.h: XnGine's outdoor ground (src/engine/terrain.c). Canonical C: plain prototypes,
   Watcom's own calling convention; docs/xngine_canonical.md.

   What it does
     Each outdoor frame the ground is the 32 x 32 grid of vertices around the eye, one per
     square of the world's layers (xworld.h), drawn as 31 x 31 textured cells: each cell a
     quad, or two triangles when its first corner's height byte says the square is not
     planar, with the tile's ground texture turned and flipped as its tile byte says, skipped
     when all its corners are outside one plane of the view and clipped when some are. Nature
     flats stand on the vertices that have one. Separately, the ground's height under a point
     (xn_terrain_height_at) serves the game's movement and placement.

   Fixed point and units
     camera space  world units << 8 rotated by the eye (xn_cam_rotation), then x and y scaled
                   by the view (2.14 xn_cam_scale_x/_y): the frustum is x = +-z, y = +-z
     steps         xn_terrain_step_x/_y/_z: a square's step along the world's x, y (height) and z
                   in camera space (xn_mat_scaled_axes: 24.8, then rounded to 16.16 here)
     screen        a projected vertex: x << 5, the row, and 1/z = 2^40 / z
     texture       the ground texture's u and v axes (texels per camera unit), then per screen
                   unit; a cell's texture origin steps by 3E00h a cell
     vertices      numbered 0..1023 (32 a row) in the vertex arrays; a cell's corners are v,
                   v + 1, v + 32 and v + 33 (the asm numbers them by byte offset: v * 12)

   Tables and state
     the terrain's own (object 2): the steps, xn_terrain_tex_scale, the texture axes
     (xn_terrain_u_axis, _v_axis: this frame's), the height steps (xn_terrain_height_cam_x/
     _y/_z[256]: entries 128-255 repeat 0-127), the unscaled camera x and y of each vertex
     (xn_terrain_vert_x/_y), the cell axis routines' tables (xn_terrain_u_axis_fns: Q-TERRAIN-
     01); the renderer's vertex arrays (xn_vert_cam, xn_vert_screen, xn_vert_flags), polygon
     pool (xn_render_poly_next), light lists and clipper buffer; the world's layers, eye and
     archives. Projects with the view globals (xn_cam_half_width/_height, and the centre
     xn_cam_centre_x/_y << 8 + 80h: xn_cam_set_view_window, cam group, sets them).

   Quirks kept: Q-TERRAIN-01 (turned tiles ignore their u flip), Q-TERRAIN-02 (a face plane
   leaves pick_distance). docs/engine/quirks.md. */
#ifndef XTERRAIN_H
#define XTERRAIN_H

#include "xworld.h"

#define XN_GRID_SIDE    32              /* vertices along a side of the grid */

/* ---- the terrain's own data --------------------------------------------------------------- */
extern xn_vec3 xn_terrain_step_x;               /* a square's steps in camera space */
extern xn_vec3 xn_terrain_step_y;
extern xn_vec3 xn_terrain_step_z;
extern s32 xn_terrain_tex_scale;                /* camera units to texels */
extern s32 xn_terrain_u_axis[3];                /* the ground texture's u and v axes */
extern s32 xn_terrain_v_axis[3];
extern s32 xn_terrain_height_cam_x[256];        /* a height byte's step in camera space */
extern s32 xn_terrain_height_cam_y[256];
extern s32 xn_terrain_height_cam_z[256];
extern struct xn_terrain_vert_coord xn_terrain_vert_x[1024];  /* camera x, y before the view */
extern struct xn_terrain_vert_coord xn_terrain_vert_y[1024];  /*   scales */


/* ---- the renderer's (render, poly, light, span groups) ------------------------------------ */
extern struct xn_vert_cam xn_vert_cam[1024];
extern struct xn_vert_screen xn_vert_screen[1024];
extern struct xn_vert_flags xn_vert_flags[1024];
extern struct xn_poly *xn_render_poly_next;
extern struct xn_light_ref *xn_render_light_list_next;
extern struct xn_light xn_light_table[33];
extern s32 xn_light_count;
extern s32 xn_span_dzdx;                        /* the span routines' 1/z step per pixel */
#include "xrender.h"                            /* xn_render_span_setup: the cells' setup */
/* (the clipper's buffer, count and outcodes: xpoly.h) */

/* A cell polygon's texture axis routine: the u (or v) gradient and origin of the polygon,
   from the frame's texture axes and the cell's texture origins u0, v0 */
typedef void (*xn_terrain_axis_fn)(struct xn_poly *p, s32 u0, s32 v0);

/* ---- the frame -------------------------------------------------------------------------- */

/* Draws the outdoor ground: the steps of a square in camera space, the texture axes, the
   height steps, the grid, the nature flats, the cells, and when the ground's texture can be
   had, the light list of its polygons. One game site (each outdoor frame). */
void xn_terrain_draw(void);

/* The terrain polygons' light list (at xn_render_light_list_next): the directional lights
   among the first 32 of the frame, their directions in camera space and their intensities
   negated; an entry's first dword -1 ends it (a negative light count runs 2^32 times). */
void xn_terrain_build_light_list(void);

/* The nature flats (trees, rocks) of the grid: one for each vertex whose flat number (1-33)
   is set and which is not before the near plane or beyond the far one (none when the
   nature archive is 0 or 1). */
void xn_terrain_add_nature_flats(void);

/* The ground texture's u and v axes (the x and z steps of a square times the texture scale,
   / 2^24; a quotient that does not fit is 0, Q-SYS-01), then the three steps rounded from
   24.8 to 16.16. */
void xn_terrain_setup_axes(void);

/* For the 128 heights: the step from the eye's height to the height in camera space (the y
   step times -(height + eye y), rounded >> 8), twice over. */
void xn_terrain_build_height_table(void);

/* Transforms the 32 x 32 grid around the eye into camera space, a step at a time from the
   grid's corner (16 squares back and left of the eye's square), with each vertex's outcode
   (and its height byte's bit 7), and projects the vertices inside the view; copies each
   vertex's tile and flat bytes. A vertex far behind the eye (z at or below -12C00h) keeps
   its old position. */
void xn_terrain_transform_grid(void);

/* ---- a cell's polygons ---------------------------------------------------------------------- */

/* The plane of cell v's triangle a (v, v + 32, v + 33): its normal (the cross product of two
   edges, each << 4, the products' high dwords) into the polygon p. Returns 0 when it faces
   away from the eye; else sets the polygon's 1/z step per pixel (also xn_span_dzdx; a
   quotient that does not fit is 0, Q-SYS-01), its light list and its owner (1: the
   terrain). Q-TERRAIN-02. */
int xn_terrain_face_plane_a(u32 v, struct xn_poly *p);

/* The same for triangle b (v, v + 1, v + 33). */
int xn_terrain_face_plane_b(u32 v, struct xn_poly *p);

/* Draws cell v's triangle a, all inside the view, as polygon p: its plane, its projected
   corners, the rasterizer from its top corner; the next polygon record. */
void xn_terrain_draw_tri_a(u32 v, struct xn_poly *p);

/* The same for triangle b. */
void xn_terrain_draw_tri_b(u32 v, struct xn_poly *p);

/* Draws cell v's triangle a, partly outside the view: the corners in camera space with their
   outcodes, to the clipper (xn_poly_project_terrain). outcodes: the corners' AND in the low
   byte, their OR in the next, as the clipper takes them. */
void xn_terrain_draw_tri_a_clipped(u32 v, struct xn_poly *p, u32 outcodes);

/* The same for triangle b. */
void xn_terrain_draw_tri_b_clipped(u32 v, struct xn_poly *p, u32 outcodes);

/* Draws the 31 x 31 cells, each one quad or two triangles, skipped when all its corners are
   outside one plane of the view, with its tile's texture turned and flipped. Returns 0,
   drawing nothing, when the ground's texture archive cannot be had. */
int xn_terrain_draw_cells(void);

/* ---- the texture axes ------------------------------------------------------------------- */

/* The texture's origins at the grid's corner (its camera position along the u and v axes:
   to *u0, *v0), and the axes in screen units (x and y divided by the focal lengths << 11, a
   quotient that does not fit 0, Q-SYS-01; z rounded >> 11). */
void xn_terrain_setup_tex_gradients(s32 *u0, s32 *v0);

/* A cell polygon's u gradient and u origin, by its tile's rotation and flip
   (xn_terrain_draw_cells' tables): the texture's u axis and origin u0, its v axis and v0, or
   either negated with the origin mirrored (4000h less it). Each stores the whole origin
   dword over the polygon's u and v origins (the v routine then replaces v's). */
void xn_terrain_u_axis_x(struct xn_poly *p, s32 u0, s32 v0);
void xn_terrain_u_axis_z(struct xn_poly *p, s32 u0, s32 v0);
void xn_terrain_u_axis_neg_x(struct xn_poly *p, s32 u0, s32 v0);
void xn_terrain_u_axis_neg_z(struct xn_poly *p, s32 u0, s32 v0);

/* The same for the v gradient; the v origin is a word. */
void xn_terrain_v_axis_z(struct xn_poly *p, s32 u0, s32 v0);
void xn_terrain_v_axis_x(struct xn_poly *p, s32 u0, s32 v0);
void xn_terrain_v_axis_neg_z(struct xn_poly *p, s32 u0, s32 v0);
void xn_terrain_v_axis_neg_x(struct xn_poly *p, s32 u0, s32 v0);

/* ---- the ground's height ---------------------------------------------------------------- */

/* The ground's height at (x, z) (world units; up is negative): of the square's two triangles
   (split along its diagonal), the one under the point, interpolated (xn_math_triangle_y_at).
   18 game sites. */
s32 xn_terrain_height_at(s32 x, s32 z);

#endif
