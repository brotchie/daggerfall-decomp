/* xterrain.h: XnGine's terrain functions (terrain.c; see xngine.h): the outdoor ground as a
   32 x 32 grid of vertices around the eye, drawn as 31 x 31 textured cells, and the ground's
   height under a point.

   The grid's vertices are numbered by their byte offset in the vertex arrays (12 bytes a
   vertex, 0x180 a row of 32), as the asm and the model faces number them. A cell's corners
   are v, v + 1 (+0Ch), v + a row (+180h) and v + a row + 1 (+18Ch).

   Each declaration keeps the function's asm interface (config/xngine_abi.csv): no pragma is
   Watcom's own convention; a pragma names the registers; NAME_r is the glue for a function
   whose asm callers read flags. */
#ifndef XTERRAIN_H
#define XTERRAIN_H

#include "xworld.h"

#define XN_GRID_ROW     0x180           /* bytes of a grid row: 32 vertices */
#define XN_GRID_VERT    0x0C            /* bytes of a vertex */

/* ---- the terrain's own data ------------------------------------------------------------ */
extern xn_vec3 xn_terrain_step_x;               /* a square's steps along x, y, z in camera */
extern xn_vec3 xn_terrain_step_y;               /*   space (xn_mat_scaled_axes), 24.8 at */
extern xn_vec3 xn_terrain_step_z;               /*   first, then 16.16 */
extern s32 xn_terrain_tex_scale;                /* camera units to texels */
extern s32 xn_terrain_u_axis[3];                /* the ground texture's u and v axes */
extern s32 xn_terrain_v_axis[3];
extern s32 xn_terrain_height_cam_x[256];        /* a height byte's step in camera space; */
extern s32 xn_terrain_height_cam_y[256];        /*   entries 128-255 repeat 0-127 */
extern s32 xn_terrain_height_cam_z[256];
extern struct xn_terrain_vert_coord xn_terrain_vert_x[1024];  /* camera x, y before the view */
extern struct xn_terrain_vert_coord xn_terrain_vert_y[1024];  /*   scales */
extern u32 xn_terrain_u_axis_fns[8];            /* asm entries: [rotation * 2 + flip] */
extern u32 xn_terrain_v_axis_fns[8];

/* xn_terrain_transform_grid's walker: the camera-space position of the current row's first
   vertex and of the current vertex, their steps, and the loop ends, kept in its own code
   (patch fields); the projection's scales and centre, patched by xn_cam_set_view_window */
extern s32 xn_tgrid_row_x, xn_tgrid_row_y, xn_tgrid_row_z;
extern s32 xn_tgrid_x, xn_tgrid_y, xn_tgrid_z;
extern s32 xn_tgrid_col_dx, xn_tgrid_col_dy, xn_tgrid_col_dz;
extern s32 xn_tgrid_row_dx, xn_tgrid_row_dy, xn_tgrid_row_dz;
extern u32 xn_tgrid_row_end, xn_tgrid_end;
extern s32 xn_tgrid_sx, xn_tgrid_cx, xn_tgrid_sy, xn_tgrid_cy;

/* xn_terrain_draw_cells' constants in its code: the ground archive and the span setup, each
   in the three paths (two triangles, a quad) */
extern u32 xn_tcells_archive_a, xn_tcells_archive_b, xn_tcells_archive_c;
extern u32 xn_tcells_setup_a, xn_tcells_setup_b, xn_tcells_setup_c;

/* ---- the renderer (render, poly, light groups) ------------------------------------------- */
extern struct xn_vert_cam xn_vert_cam[1024];
extern struct xn_vert_screen xn_vert_screen[1024];
extern struct xn_vert_flags xn_vert_flags[1024];
extern struct xn_poly *xn_render_poly_next;
extern struct xn_light_ref *xn_render_light_list_next;
extern struct xn_light xn_light_table[33];
extern s32 xn_light_count;
extern s32 xn_cam_scale_x, xn_cam_scale_y;      /* the view's scales, 2.14 */
extern s32 xn_cam_inv_focal_x;
extern s32 xn_span_dzdx;
extern u32 xn_render_span_setup_terrain_ptr;    /* the terrain's span setup routine */
extern struct xn_poly_vertex xn_poly_vertex_buf_a[32];
extern u8 xn_poly_vertex_count;
extern u8 xn_poly_clip_outcode_and, xn_poly_clip_outcode_or;
extern struct xn_poly_vertex **xn_poly_ring_a[];
extern struct xn_collide_scratch xn_collide_work;   /* 0x14A100 */

/* Draws the outdoor ground: the steps of a square in camera space, the texture axes, the
   heights, the grid, the nature flats, the cells, and, when the ground's texture is there,
   the light list of its polygons. (The asm saves every register.) */
void xn_terrain_draw(void);

/* The terrain polygons' light list: the directional lights of the frame (at most the first
   32 lights), their directions in camera space and their intensities negated; -1 ends it. */
void xn_terrain_build_light_list(void);

/* The nature flats (trees, rocks) of the grid: one for each vertex whose flat number (1-33)
   is set and which is not before the near plane or beyond the far one. */
void xn_terrain_add_nature_flats(void);

/* The ground texture's u and v axes (the x and z steps of a square times the texture scale,
   / 2^24), then the three steps rounded from 24.8 to 16.16. */
void xn_terrain_setup_axes(void);

/* For the 128 heights: the step from the eye's height to the height, in camera space (the y
   step times -(height + eye y), rounded), twice over. */
void xn_terrain_build_height_table(void);

/* Transforms the 32 x 32 grid around the eye into camera space (incrementally, from the grid's
   corner), with each vertex's outcode, and projects the vertices inside the view; copies
   each vertex's tile and flat bytes. */
void xn_terrain_transform_grid(void);

/* The plane of the cell's triangle (v, v + a row, v + a row + 1): its normal (the cross
   product of two edges, << 4) into the polygon. Returns 0 when it faces away from the eye;
   else sets the polygon's 1/z step per pixel (also xn_span_dzdx), light list and owner (1:
   the terrain). The asm's CF is the 0. */
int xn_terrain_face_plane_a(u32 v, struct xn_poly *p);
void xn_terrain_face_plane_a_r(xn_regs *r);

/* The same for the other triangle (v, v + 1, v + a row + 1). */
int xn_terrain_face_plane_b(u32 v, struct xn_poly *p);
void xn_terrain_face_plane_b_r(xn_regs *r);

/* Draws the cell's triangle a, all inside the view: its plane, its projected corners, the
   rasterizer from the top corner; the next polygon record. */
void xn_terrain_draw_tri_a(u32 v, struct xn_poly *p);
#pragma aux xn_terrain_draw_tri_a parm [esi] [edi] modify exact [eax ebx ecx edx edi];

/* The same for triangle b. */
void xn_terrain_draw_tri_b(u32 v, struct xn_poly *p);
#pragma aux xn_terrain_draw_tri_b parm [esi] [edi] modify exact [eax ebx ecx edx edi];

/* Draws the cell's triangle a, partly outside the view: the corners in camera space, with
   their outcodes, to the clipper and projector. outcodes: the corners' AND in the low byte,
   their OR in the next (the row's bx; [bx] as a pragma parameter does not compile right). */
void xn_terrain_draw_tri_a_clipped(u32 v, struct xn_poly *p, u32 outcodes);
#pragma aux xn_terrain_draw_tri_a_clipped parm [esi] [edi] [ebx] \
    modify exact [eax ebx ecx edx edi];

/* The same for triangle b. */
void xn_terrain_draw_tri_b_clipped(u32 v, struct xn_poly *p, u32 outcodes);
#pragma aux xn_terrain_draw_tri_b_clipped parm [esi] [edi] [ebx] \
    modify exact [eax ebx ecx edx edi];

/* Draws the 31 x 31 cells: each one quad or two triangles (the first corner's height bit 7),
   skipped when all its corners are outside one plane of the view, with its tile's texture
   turned and flipped. Returns 0, drawing nothing, when the ground's texture archive cannot be
   had (the asm's CF). */
int xn_terrain_draw_cells(void);
void xn_terrain_draw_cells_r(xn_regs *r);

/* The texture's origin at the grid's corner (the corner's camera position along the u and v
   axes) and the axes in screen units (x, y divided by the focal lengths << 11; z rounded
   down 11 bits). */
void xn_terrain_setup_tex_gradients(void);

/* A cell polygon's u axis and u origin by the tile's rotation and flip: the texture's u axis,
   its v axis, or either negated with the origin mirrored (4000h - origin). Each returns the
   origin it stored (the asm leaves it in ECX) and keeps every other register (EAX through the
   route's stub). */
s32 xn_terrain_u_axis_x(struct xn_poly *p);
#pragma aux xn_terrain_u_axis_x parm [edi] value [ecx] modify exact [eax ecx];
s32 xn_terrain_u_axis_z(struct xn_poly *p);
#pragma aux xn_terrain_u_axis_z parm [edi] value [ecx] modify exact [eax ecx];
s32 xn_terrain_u_axis_neg_x(struct xn_poly *p);
#pragma aux xn_terrain_u_axis_neg_x parm [edi] value [ecx] modify exact [eax ecx];
s32 xn_terrain_u_axis_neg_z(struct xn_poly *p);
#pragma aux xn_terrain_u_axis_neg_z parm [edi] value [ecx] modify exact [eax ecx];

/* The same for the v axis; the v origin is a word (the high half of the packed origin). */
s32 xn_terrain_v_axis_z(struct xn_poly *p);
#pragma aux xn_terrain_v_axis_z parm [edi] value [ecx] modify exact [eax ecx];
s32 xn_terrain_v_axis_x(struct xn_poly *p);
#pragma aux xn_terrain_v_axis_x parm [edi] value [ecx] modify exact [eax ecx];
s32 xn_terrain_v_axis_neg_z(struct xn_poly *p);
#pragma aux xn_terrain_v_axis_neg_z parm [edi] value [ecx] modify exact [eax ecx];
s32 xn_terrain_v_axis_neg_x(struct xn_poly *p);
#pragma aux xn_terrain_v_axis_neg_x parm [edi] value [ecx] modify exact [eax ecx];

/* The ground's height at (x, z) (world units, up negative): the square's two triangles
   (split along its diagonal), the one under the point interpolated (in xn_collide_work's
   ground triangle). 18 game sites. (The row's inputs bh and ebx.u are the asm's `mov bl, ..;
   and ebx, 7Fh`, which reads none of them.) */
s32 xn_terrain_height_at(s32 x, s32 z);
#pragma aux xn_terrain_height_at parm [eax] [edx] value [eax] modify exact [eax edx];

#endif
