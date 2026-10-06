/* terrain.c: XnGine's outdoor ground (canonical C; the interface and the module's
   documentation are in xterrain.h). The asm's patched operands (docs/engine/smc/terrain.md)
   are locals here: the grid walker's position and steps, the cells' archive and span setup;
   the cells' texture origins, which the asm kept in the world's scratch globals, are locals
   passed to the axis routines. */
#include "xterrain.h"
#include "xmat.h"
#include "xmath.h"
#include "xflat.h"
#include "xpoly.h"
#include "xtex.h"

#define OUT_LEFT    0x01                /* outcodes: x < -z */
#define OUT_RIGHT   0x02                /* x > z */
#define OUT_BOTTOM  0x04                /* y > z */
#define OUT_TOP     0x08                /* y < -z */
#define OUT_NEAR    0x10
#define OUT_FAR     0x20
#define TWO_TRIS    0x80                /* the height byte's bit 7: the cell is two triangles */

#define ROW         XN_GRID_SIDE        /* a vertex's neighbour below in the grid */
#define FAR_BEHIND  (-0x12C00)          /* a vertex at or behind this z keeps its position */
#define CELL_UV     0x3E00              /* a cell's step of the texture origins */

/* a * b / d with the 64-bit product, truncated; 0 where the asm's idiv faults (Q-SYS-01) */
static s32 muldiv(s32 a, s32 b, s32 d)
{
    xn_s64 t;

    xn_s64_mul(&t, a, b);
    return xn_s64_div_or0(&t, d);
}

/* a camera-space vertex for the face planes: x and y before the view scales, and z */
static void plane_vertex(u32 v, xn_vec3 *p)
{
    p->x = xn_terrain_vert_x[v].value;
    p->y = xn_terrain_vert_y[v].value;
    p->z = xn_vert_cam[v].z;
}

/* the layers' row and column of the grid's corner: the row from the world's north edge and
   the column, each 16 squares before the eye's; bytes, so both wrap */
static u8 grid_first_row(void)
{
    return (u8)(((u32)(xn_world_size_z - xn_cam_z) >> 8) - 0x10);
}

static u8 grid_first_col(void)
{
    return (u8)((xn_cam_x >> 8) - 0x10);
}

/* ---- the frame -------------------------------------------------------------------------- */

void xn_terrain_draw(void)
{
    xn_mat_scaled_axes(0x1000000, -0x1000000, -0x1000000, &xn_cam_rotation);
    xn_terrain_setup_axes();
    xn_terrain_build_height_table();
    xn_terrain_transform_grid();
    xn_terrain_add_nature_flats();
    if (xn_terrain_draw_cells())
        xn_terrain_build_light_list();
}

void xn_terrain_build_light_list(void)
{
    struct xn_light_ref *out = xn_render_light_list_next;
    struct xn_light *light = xn_light_table;
    s32 n = xn_light_count;
    xn_vec3 dir;

    if (n != 0) {
        if (n > 32)
            n = 32;
        do {                            /* (a negative count runs 2^32 times) */
            if (light->type == 8) {     /* directional */
                dir.x = light->x;
                dir.y = light->y;
                dir.z = light->z;
                xn_mat_transform(&dir, &xn_cam_rotation);
                out->x = dir.x;
                out->y = dir.y;
                out->z = dir.z;
                out->light = light;
                out->intensity = -light->intensity;
                out++;
            }
            light++;
        } while (--n != 0);
    }
    /* the end mark is an entry's light pointer, -1; the next list starts right after it */
    *(iptr *)out = -1;
    xn_render_light_list_next = (struct xn_light_ref *)((u8 *)out + PTR_SIZE);
}

void xn_terrain_add_nature_flats(void)
{
    u32 v = 0, n;
    u8 row, col;
    s32 rows, cols;

    if (xn_world_nature_archive <= 1)
        return;
    row = grid_first_row();
    col = grid_first_col();
    for (rows = XN_GRID_SIDE; rows != 0; rows--, row++, col -= XN_GRID_SIDE) {
        for (cols = XN_GRID_SIDE; cols != 0; cols--, col++, v++) {
            n = xn_world_flat_layer[(u16)(row << 8 | col)] >> 2;
            if (n < 1 || n > 33)
                continue;
            if (xn_vert_flags[v].terrain_outcode & (OUT_NEAR | OUT_FAR))
                continue;
            xn_flat_add_view(xn_vert_cam[v].x, xn_vert_cam[v].y, xn_vert_cam[v].z,
                             ((u32)xn_world_nature_archive << 7 | n) - 1);
        }
    }
}

/* ---- per-frame tables ------------------------------------------------------------------- */

/* v = (v + 80h) >> 8 for each component */
static void round8(xn_vec3 *v)
{
    v->x = (v->x + 0x80) >> 8;
    v->y = (v->y + 0x80) >> 8;
    v->z = (v->z + 0x80) >> 8;
}

/* a camera-space step in texels: step * scale / 2^24 */
static s32 to_texels(s32 step)
{
    return muldiv(step, xn_terrain_tex_scale, 0x1000000);
}

void xn_terrain_setup_axes(void)
{
    xn_terrain_u_axis[0] = to_texels(xn_terrain_step_x.x);
    xn_terrain_u_axis[1] = to_texels(xn_terrain_step_x.y);
    xn_terrain_u_axis[2] = to_texels(xn_terrain_step_x.z);
    xn_terrain_v_axis[0] = to_texels(xn_terrain_step_z.x);
    xn_terrain_v_axis[1] = to_texels(xn_terrain_step_z.y);
    xn_terrain_v_axis[2] = to_texels(xn_terrain_step_z.z);
    round8(&xn_terrain_step_x);
    round8(&xn_terrain_step_y);
    round8(&xn_terrain_step_z);
}

/* (a * b + 80h) >> 8 with a 64-bit product */
static s32 mul_round8(s32 a, s32 b)
{
    xn_s64 t;

    xn_s64_mul(&t, a, b);
    xn_s64_addu(&t, 0x80);
    return xn_s64_shr(&t, 8);
}

void xn_terrain_build_height_table(void)
{
    int i;
    s32 h;

    for (i = 0; i < 128; i++) {
        h = -xn_world_height_scale[i] - xn_cam_y;
        xn_terrain_height_cam_x[i] = xn_terrain_height_cam_x[i + 128] =
            mul_round8(xn_terrain_step_y.x, h);
        xn_terrain_height_cam_y[i] = xn_terrain_height_cam_y[i + 128] =
            mul_round8(xn_terrain_step_y.y, h);
        xn_terrain_height_cam_z[i] = xn_terrain_height_cam_z[i + 128] =
            mul_round8(xn_terrain_step_y.z, h);
    }
}

/* ---- the grid --------------------------------------------------------------------------- */

/* The outcode of a camera-space point: the view planes it is outside of */
static u8 outcode(s32 x, s32 y, s32 z)
{
    u8 c = 0;

    if (z < xn_cam_near_z)
        c |= OUT_NEAR;
    if (z > xn_cam_far_z)
        c |= OUT_FAR;
    if (x > z)
        c |= OUT_RIGHT;
    if (y > z)
        c |= OUT_BOTTOM;
    if (x < -z)
        c |= OUT_LEFT;
    if (y < -z)
        c |= OUT_TOP;
    return c;
}

/* Vertex v at layer position pos, the walker at camera position p: its height's offset from
   p, its camera position and outcode (with the height's bit 7), the projection of a vertex
   inside the view; and the tile and flat bytes. A vertex far behind the eye keeps its old
   position: its outcode is then the height byte | 10h. */
static void grid_vertex(u32 v, u16 pos, const xn_vec3 *p)
{
    u32 h = xn_world_height_layer[pos];
    s32 x = p->x - xn_terrain_height_cam_x[h];
    s32 y = p->y - xn_terrain_height_cam_y[h];
    s32 z = p->z - xn_terrain_height_cam_z[h];
    struct xn_vert_cam *c = &xn_vert_cam[v];
    struct xn_vert_screen *s = &xn_vert_screen[v];
    struct xn_vert_flags *f = &xn_vert_flags[v];
    u8 code = (u8)(h | OUT_NEAR);
    xn_s64 n;
    u32 inv_z;

    if (z > FAR_BEHIND) {
        xn_terrain_vert_x[v].value = x;
        xn_terrain_vert_y[v].value = y;
        c->x = xn_mulshr(x, xn_cam_scale_x, 14);
        c->y = xn_mulshr(y, xn_cam_scale_y, 14);
        c->z = z;
        code = (u8)(h & TWO_TRIS) | outcode(c->x, c->y, z);
        if ((code & ~TWO_TRIS) == 0) {
            n.lo = 0;                   /* 2^40 / z */
            n.hi = 0x100;
            inv_z = xn_u64_div_or0(&n, z);
            s->inv_z = inv_z;
            s->sx = (u32)(xn_mulhi(c->x * xn_cam_half_width, inv_z) +
                          (xn_cam_centre_x << 8) + 0x80) >> 3;
            s->sy = (u32)(xn_mulhi(c->y * xn_cam_half_height, inv_z) +
                          (xn_cam_centre_y << 8) + 0x80) >> 8;
        }
    }
    f->terrain_outcode = code;
    f->terrain_tile = xn_world_tile_layer[pos];
    f->terrain_flat = xn_world_flat_layer[pos];
}

void xn_terrain_transform_grid(void)
{
    xn_vec3 row_start, p;
    u32 v = 0, rows, cols;
    u8 row, col;

    /* the grid's corner, 16 squares back and left of the eye's square, in camera space */
    row_start.x = (-(xn_cam_x & 0xFF) - 0x1000) << 8;
    row_start.y = 0;
    row_start.z = ((-xn_cam_z & 0xFF) + 0x1000) << 8;
    xn_mat_transform_wide(&row_start, &xn_cam_rotation);
    row = grid_first_row();
    col = grid_first_col();
    for (rows = XN_GRID_SIDE; rows != 0; rows--) {
        p = row_start;
        for (cols = XN_GRID_SIDE; cols != 0; cols--) {
            grid_vertex(v, (u16)(row << 8 | col), &p);
            p.x += xn_terrain_step_x.x;
            p.y += xn_terrain_step_x.y;
            p.z += xn_terrain_step_x.z;
            v++;
            col++;
        }
        row_start.x += xn_terrain_step_z.x;
        row_start.y += xn_terrain_step_z.y;
        row_start.z += xn_terrain_step_z.z;
        row++;
        col -= XN_GRID_SIDE;
    }
}

/* ---- a cell's planes -------------------------------------------------------------------- */

/* Whether a + b >= 0 for the exact sum (the asm's `add; jge`: SF = OF) */
static int sum_ge0(s32 a, s32 b)
{
    if ((a ^ b) >= 0)                   /* same signs: the sum has their sign */
        return a >= 0;
    return a + b >= 0;
}

/* The plane through vertex v with edges a = P(a1) - P(a0), b = P(b1) - P(b0). The normal is
   (a x b) with each product's high dword, the edges << 4. Back-facing: the plane's value at
   the eye, n . P(v) (the last sum exact), is not negative. */
static int face_plane(u32 v, struct xn_poly *p, u32 a0, u32 a1, u32 b0, u32 b1)
{
    xn_vec3 p0, pa0, pa1, pb0, pb1, a, b;
    s32 nx, ny, nz, d;

    plane_vertex(v, &p0);
    plane_vertex(a0, &pa0);
    plane_vertex(a1, &pa1);
    plane_vertex(b0, &pb0);
    plane_vertex(b1, &pb1);
    a.x = (pa1.x - pa0.x) << 4;
    a.y = (pa1.y - pa0.y) << 4;
    a.z = (pa1.z - pa0.z) << 4;
    b.x = (pb1.x - pb0.x) << 4;
    b.y = (pb1.y - pb0.y) << 4;
    b.z = (pb1.z - pb0.z) << 4;
    /* Quirk Q-TERRAIN-02: the asm keeps its edges in the shared scratch vectors; a.z is
       pick_distance, which the game reads */
    xn_scratch_vecs.a.z = a.z;
    ny = xn_mulhi(b.x, a.z) - xn_mulhi(a.x, b.z);
    nz = xn_mulhi(a.x, b.y) - xn_mulhi(a.y, b.x);
    nx = xn_mulhi(a.y, b.z) - xn_mulhi(a.z, b.y);
    p->ny = ny;
    p->nz = nz;
    p->nx = nx;
    d = nz * p0.z + nx * p0.x;
    if (sum_ge0(d, ny * p0.y))
        return 0;
    d += ny * p0.y;
    p->inv_z_dx = muldiv(nx * 4, xn_cam_inv_focal_x, d);
    xn_span_dzdx = p->inv_z_dx;
    p->light_list = xn_render_light_list_next;
    p->owner = 1;
    return 1;
}

int xn_terrain_face_plane_a(u32 v, struct xn_poly *p)
{
    return face_plane(v, p, v, v + ROW, v + ROW, v + ROW + 1);
}

int xn_terrain_face_plane_b(u32 v, struct xn_poly *p)
{
    return face_plane(v, p, v, v + 1, v + ROW + 1, v + 1);
}

/* ---- a cell's polygons ------------------------------------------------------------------ */

/* the corners of the polygons, from the cell's first vertex */
static const u32 tri_a_corners[3] = { 0, ROW, ROW + 1 };
static const u32 tri_b_corners[3] = { 0, ROW + 1, 1 };
static const u32 quad_corners[4] = { 0, ROW, ROW + 1, 1 };

/* A polygon inside the view: the corners' projections into the clipper's buffer, the
   rasterizer from the top one (the last of the least rows: the asm takes a corner when it is
   not below the top so far); the polygon record is used. */
static void draw_projected(u32 v, const u32 *corner, int n)
{
    const struct xn_vert_screen *s;
    s32 top_y = 0;
    int k, top = 0;

    for (k = 0; k < n; k++) {
        s = &xn_vert_screen[v + corner[k]];
        xn_poly_vertex_buf_a[k].x = s->sx;
        xn_poly_vertex_buf_a[k].y = s->sy;
        xn_poly_vertex_buf_a[k].z = s->inv_z;
        if (k == 0 || s->sy <= top_y) {
            top_y = s->sy;
            top = k;
        }
    }
    xn_poly_rasterize(top_y, xn_poly_ring_a[n] + top, n);
    xn_render_poly_next++;
}

/* A polygon partly outside the view: the corners in camera space with their outcodes, to the
   clipper; the polygon record is used. */
static void draw_clipped(u32 v, const u32 *corner, int n)
{
    const struct xn_vert_cam *c;
    int k;

    for (k = 0; k < n; k++) {
        c = &xn_vert_cam[v + corner[k]];
        xn_poly_vertex_buf_a[k].x = c->x;
        xn_poly_vertex_buf_a[k].y = c->y;
        xn_poly_vertex_buf_a[k].z = c->z;
        xn_poly_vertex_buf_a[k].outcode = xn_vert_flags[v + corner[k]].terrain_outcode;
    }
    xn_poly_project_terrain(n);
    xn_render_poly_next++;
}

static void set_clip_outcodes(u32 outcodes)
{
    xn_poly_clip_outcode_and = (u8)outcodes;
    xn_poly_clip_outcode_or = (u8)(outcodes >> 8);
}

void xn_terrain_draw_tri_a(u32 v, struct xn_poly *p)
{
    if (xn_terrain_face_plane_a(v, p))
        draw_projected(v, tri_a_corners, 3);
}

void xn_terrain_draw_tri_b(u32 v, struct xn_poly *p)
{
    if (xn_terrain_face_plane_b(v, p))
        draw_projected(v, tri_b_corners, 3);
}

void xn_terrain_draw_tri_a_clipped(u32 v, struct xn_poly *p, u32 outcodes)
{
    set_clip_outcodes(outcodes);
    if (xn_terrain_face_plane_a(v, p))
        draw_clipped(v, tri_a_corners, 3);
}

void xn_terrain_draw_tri_b_clipped(u32 v, struct xn_poly *p, u32 outcodes)
{
    set_clip_outcodes(outcodes);
    if (xn_terrain_face_plane_b(v, p))
        draw_clipped(v, tri_b_corners, 3);
}

/* ---- the cells -------------------------------------------------------------------------- */

/* The cell axis routines by the tile's rotation (bits 6-7) and flip: [rotation * 2 + flip].
   Quirk Q-TERRAIN-01: for rotations 2 and 3 both u entries are the same, so those tiles'
   u flip is lost. */
static const xn_terrain_axis_fn u_axis_fns[8] = {
    xn_terrain_u_axis_x, xn_terrain_u_axis_neg_x,
    xn_terrain_u_axis_neg_z, xn_terrain_u_axis_z,
    xn_terrain_u_axis_neg_x, xn_terrain_u_axis_neg_x,
    xn_terrain_u_axis_z, xn_terrain_u_axis_z
};

static const xn_terrain_axis_fn v_axis_fns[8] = {
    xn_terrain_v_axis_z, xn_terrain_v_axis_neg_z,
    xn_terrain_v_axis_x, xn_terrain_v_axis_neg_x,
    xn_terrain_v_axis_neg_z, xn_terrain_v_axis_z,
    xn_terrain_v_axis_neg_x, xn_terrain_v_axis_x
};

/* The next polygon record as cell v's: the span setup, the texture axes of the tile's
   rotation and flips (the flat byte's bits 0, u, and 1, v), the tile's texture (by the
   animation clock) */
static struct xn_poly *cell_poly(u32 v, uptr setup, u32 archive, s32 u0, s32 v0)
{
    struct xn_poly *p = xn_render_poly_next;
    const struct xn_vert_flags *f = &xn_vert_flags[v];
    u32 rot = f->terrain_tile >> 6;

    p->span_fn = (xn_span_fn)setup;
    u_axis_fns[rot * 2 + (f->terrain_flat & 1)](p, u0, v0);
    v_axis_fns[rot * 2 + ((f->terrain_flat & 2) >> 1)](p, u0, v0);
    p->tex = xn_tex_cache_lookup(archive, f->terrain_tile & 0x3F, -1);
    return p;
}

/* the outcodes of the corners so far: their AND (low byte) and OR (high byte), taking a
   corner's outcode byte (with its bit 7) */
static u32 with_corner(u32 outcodes, u32 v)
{
    u8 c = xn_vert_flags[v].terrain_outcode;

    return ((outcodes & 0xFF) & c) | ((outcodes >> 8 & 0xFF | c) << 8);
}

/* every corner outside one plane of the view: nothing to draw */
#define ALL_OUTSIDE(oc)     (((oc) & 0x7F) != 0)
/* some corner outside some plane: clip */
#define SOME_OUTSIDE(oc)    (((oc) >> 8 & 0x7F) != 0)
/* the outcodes for the clipper: the OR without bit 7 */
#define CLIP_OUTCODES(oc)   ((oc) & 0x7FFF)

static void draw_cell(u32 v, uptr setup, u32 archive, s32 u0, s32 v0)
{
    u32 first = xn_vert_flags[v].terrain_outcode & 0x7F;
    u32 oc = first << 8 | first;
    u32 diag;
    struct xn_poly *p;

    if (xn_vert_flags[v].terrain_outcode & TWO_TRIS) {
        /* two triangles, both on the diagonal v .. v + 33 */
        diag = with_corner(oc, v + ROW + 1);
        oc = with_corner(diag, v + ROW);
        if (!ALL_OUTSIDE(oc)) {
            p = cell_poly(v, setup, archive, u0, v0);
            if (SOME_OUTSIDE(oc))
                xn_terrain_draw_tri_a_clipped(v, p, CLIP_OUTCODES(oc));
            else
                xn_terrain_draw_tri_a(v, p);
        }
        oc = with_corner(diag, v + 1);
        if (!ALL_OUTSIDE(oc)) {
            p = cell_poly(v, setup, archive, u0, v0);
            if (SOME_OUTSIDE(oc))
                xn_terrain_draw_tri_b_clipped(v, p, CLIP_OUTCODES(oc));
            else
                xn_terrain_draw_tri_b(v, p);
        }
        return;
    }
    /* one quad: a's plane */
    oc = with_corner(oc, v + ROW);
    oc = with_corner(oc, v + ROW + 1);
    oc = with_corner(oc, v + 1);
    if (ALL_OUTSIDE(oc))
        return;
    p = cell_poly(v, setup, archive, u0, v0);
    if (SOME_OUTSIDE(oc)) {
        set_clip_outcodes(CLIP_OUTCODES(oc));
        if (xn_terrain_face_plane_a(v, p))
            draw_clipped(v, quad_corners, 4);
    } else if (xn_terrain_face_plane_a(v, p))
        draw_projected(v, quad_corners, 4);
}

int xn_terrain_draw_cells(void)
{
    u32 archive;
    uptr setup;                         /* the span setup's address */
    u32 v = 0;
    s32 rows, cols, u0, v0, row_u0;

    xn_terrain_setup_tex_gradients(&u0, &v0);
    archive = xn_world_ground_archive;
    if (xn_tex_cache_lookup(archive, 0, -1) == 0)
        return 0;
    setup = (uptr)xn_render_span_setup(16);
    for (rows = XN_GRID_SIDE - 1; rows != 0; rows--) {
        row_u0 = u0;
        for (cols = XN_GRID_SIDE - 1; cols != 0; cols--) {
            draw_cell(v, setup, archive, u0, v0);
            v++;
            u0 -= CELL_UV;              /* the next cell's texture origin */
        }
        u0 = row_u0;
        v++;                            /* (the row's last vertex starts no cell) */
        v0 -= CELL_UV;
    }
    return 1;
}

/* ---- the texture axes ------------------------------------------------------------------- */

/* The high dword of axis . c (three 64-bit products), negated, >> 8, + 100h: a texture
   origin at the grid's corner */
static s32 corner_origin(const s32 *axis, const xn_vec3 *c)
{
    xn_s64 t;

    xn_s64_mul(&t, axis[0], c->x);
    xn_s64_mac(&t, axis[1], c->y);
    xn_s64_mac(&t, axis[2], c->z);
    return (-t.hi >> 8) + 0x100;
}

/* a / d, truncated; 0 where the asm's idiv faults (d = 0; Q-SYS-01) */
static s32 div_or0(s32 a, s32 d)
{
    xn_s64 n;

    xn_s64_set(&n, a);
    return xn_s64_div_or0(&n, d);
}

void xn_terrain_setup_tex_gradients(s32 *u0, s32 *v0)
{
    xn_vec3 corner;
    s32 d;

    corner.x = (-(xn_cam_x & 0xFF) - 0x1000) << 18;
    corner.y = 0;
    corner.z = ((-xn_cam_z & 0xFF) + 0x1000) << 18;
    xn_mat_transform_wide(&corner, &xn_cam_rotation);
    *u0 = corner_origin(xn_terrain_u_axis, &corner);
    *v0 = corner_origin(xn_terrain_v_axis, &corner);
    d = xn_cam_focal_x << 11;
    xn_terrain_u_axis[0] = div_or0(xn_terrain_u_axis[0], d);
    xn_terrain_v_axis[0] = div_or0(xn_terrain_v_axis[0], d);
    d = xn_cam_focal_y << 11;
    xn_terrain_u_axis[1] = div_or0(xn_terrain_u_axis[1], d);
    xn_terrain_v_axis[1] = div_or0(xn_terrain_v_axis[1], d);
    xn_terrain_u_axis[2] = (xn_terrain_u_axis[2] + 0x400) >> 11;
    xn_terrain_v_axis[2] = (xn_terrain_v_axis[2] + 0x400) >> 11;
}

/* The polygon's u or v gradient: an axis, or the axis negated */
static void set_u(struct xn_poly *p, const s32 *axis, int negate)
{
    p->u_dx = negate ? -axis[0] : axis[0];
    p->u_dy = negate ? -axis[1] : axis[1];
    p->u_c = negate ? -axis[2] : axis[2];
}

static void set_v(struct xn_poly *p, const s32 *axis, int negate)
{
    p->v_dx = negate ? -axis[0] : axis[0];
    p->v_dy = negate ? -axis[1] : axis[1];
    p->v_c = negate ? -axis[2] : axis[2];
}

/* the u routines store the whole origin dword (u in the low word; the v routine replaces the
   high word) */
static void set_origin(struct xn_poly *p, s32 origin)
{
    p->tex_u0 = (u16)origin;
    p->tex_v0 = (u16)((u32)origin >> 16);
}

void xn_terrain_u_axis_x(struct xn_poly *p, s32 u0, s32 v0)
{
    (void)v0;
    set_u(p, xn_terrain_u_axis, 0);
    set_origin(p, u0);
}

void xn_terrain_u_axis_z(struct xn_poly *p, s32 u0, s32 v0)
{
    (void)u0;
    set_u(p, xn_terrain_v_axis, 0);
    set_origin(p, v0);
}

void xn_terrain_u_axis_neg_x(struct xn_poly *p, s32 u0, s32 v0)
{
    (void)v0;
    set_u(p, xn_terrain_u_axis, 1);
    set_origin(p, 0x4000 - u0);
}

void xn_terrain_u_axis_neg_z(struct xn_poly *p, s32 u0, s32 v0)
{
    (void)u0;
    set_u(p, xn_terrain_v_axis, 1);
    set_origin(p, 0x4000 - v0);
}

void xn_terrain_v_axis_z(struct xn_poly *p, s32 u0, s32 v0)
{
    (void)u0;
    set_v(p, xn_terrain_v_axis, 0);
    p->tex_v0 = (u16)v0;
}

void xn_terrain_v_axis_x(struct xn_poly *p, s32 u0, s32 v0)
{
    (void)v0;
    set_v(p, xn_terrain_u_axis, 0);
    p->tex_v0 = (u16)u0;
}

void xn_terrain_v_axis_neg_z(struct xn_poly *p, s32 u0, s32 v0)
{
    (void)u0;
    set_v(p, xn_terrain_v_axis, 1);
    p->tex_v0 = (u16)(0x4000 - v0);
}

void xn_terrain_v_axis_neg_x(struct xn_poly *p, s32 u0, s32 v0)
{
    (void)v0;
    set_v(p, xn_terrain_u_axis, 1);
    p->tex_v0 = (u16)(0x4000 - u0);
}

/* ---- the ground's height ---------------------------------------------------------------- */

/* a height byte's height (bit 7 off), << 8 */
static s32 square_height(u8 row, u8 col)
{
    return xn_world_height_scale[xn_world_height_layer[(u16)(row << 8 | col)] & 0x7F] << 8;
}

static void set_point(xn_vec3 *p, s32 x, s32 y, s32 z)
{
    p->x = x;
    p->y = y;
    p->z = z;
}

s32 xn_terrain_height_at(s32 x, s32 z)
{
    xn_vec3 tri[3];
    s32 fx, fz, zz = xn_world_size_z - z;
    u8 row = (u8)((u32)zz >> 8), col = (u8)((u32)x >> 8);

    fx = x & 0xFF;
    fz = zz & 0xFF;
    /* the square's corners at 0 and 10000h: its first corner, then the triangle's two others
       on the side of the diagonal the point is on */
    set_point(&tri[0], 0, square_height(row, col), 0);
    if (fz - fx > 0) {
        set_point(&tri[1], 0, square_height(row + 1, col), 0x10000);
        set_point(&tri[2], 0x10000, square_height(row + 1, col + 1), 0x10000);
    } else {
        set_point(&tri[2], 0x10000, square_height(row, col + 1), 0);
        set_point(&tri[1], 0x10000, square_height(row + 1, col + 1), 0x10000);
    }
    return -((xn_math_triangle_y_at(tri, fx << 8, fz << 8) + 0x80) >> 8);
}
