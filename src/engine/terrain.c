/* terrain.c: XnGine's outdoor ground as readable C (xterrain.h; see xngine.h and
   docs/xngine_readable.md, and docs/engine/smc/terrain.md for the patched fields).

   Camera space is world units << 8 rotated by the eye (the frustum x = +-z, y = +-z). The
   grid is the 32 x 32 vertices around the eye, a vertex per 256 world units, read from the
   world's 256 x 256 layers at the eye's position. */
#include "xterrain.h"
#include "xmat.h"
#include "xmath.h"

/* the vertex arrays at a vertex's byte offset (the asm's numbering) */
static struct xn_vert_cam *cam_at(u32 v)
{
    return (struct xn_vert_cam *)((u8 *)xn_vert_cam + v);
}

static struct xn_vert_screen *screen_at(u32 v)
{
    return (struct xn_vert_screen *)((u8 *)xn_vert_screen + v);
}

static struct xn_vert_flags *flags_at(u32 v)
{
    return (struct xn_vert_flags *)((u8 *)xn_vert_flags + v);
}

/* the camera x and y before the view scales */
static struct xn_terrain_vert_coord *unscaled_x_at(u32 v)
{
    return (struct xn_terrain_vert_coord *)((u8 *)xn_terrain_vert_x + v);
}

static struct xn_terrain_vert_coord *unscaled_y_at(u32 v)
{
    return (struct xn_terrain_vert_coord *)((u8 *)xn_terrain_vert_y + v);
}

/* a vertex for the face planes: camera x and y before the view scales, and camera z */
static void plane_vertex(u32 v, xn_vec3 *p)
{
    p->x = unscaled_x_at(v)->value;
    p->y = unscaled_y_at(v)->value;
    p->z = cam_at(v)->z;
}

/* the 256 x 256 layers' position of the grid's corner: the row from the world's far z edge
   and the column, each 16 squares before the eye's; bytes, so both wrap */
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
    /* the end mark is the first dword of an entry; the next list starts right after it */
    *(s32 *)out = -1;
    xn_render_light_list_next = (struct xn_light_ref *)((u8 *)out + 4);
}

void xn_terrain_add_nature_flats(void)
{
    u32 v = 0;
    u8 row, col;
    s32 rows, cols;
    u32 n;

    if (xn_world_nature_archive <= 1)
        return;
    row = grid_first_row();
    col = grid_first_col();
    for (rows = 32; rows != 0; rows--, row++, col -= 32) {
        for (cols = 32; cols != 0; cols--, col++, v += XN_GRID_VERT) {
            n = xn_world_flat_layer[(u16)(row << 8 | col)] >> 2;
            if (n < 1 || n > 33)
                continue;
            if (flags_at(v)->terrain_outcode & (0x10 | 0x20))  /* near or far */
                continue;
            xn_flat_add_view(cam_at(v)->x, cam_at(v)->y, cam_at(v)->z,
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
    return xn_muldiv(step, xn_terrain_tex_scale, 0x1000000);
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

/* The outcode of a camera-space point: 1 x < -z, 2 x > z, 4 y > z, 8 y < -z, 10h z before the
   near plane, 20h beyond the far one */
static u8 outcode(s32 x, s32 y, s32 z)
{
    u8 c = 0;

    if (z < xn_cam_near_z)
        c |= 0x10;
    if (z > xn_cam_far_z)
        c |= 0x20;
    if (x > z)
        c |= 2;
    if (y > z)
        c |= 4;
    if (x < -z)
        c |= 1;
    if (y < -z)
        c |= 8;
    return c;
}

/* The vertex at byte offset v, layer position pos: its height's offset from the walker's
   position, its camera position and outcode (with the height's bit 7), the projection of a
   vertex inside the view; and the tile and flat bytes. A vertex far behind the eye (z at or
   below -12C00h) keeps its old position: its outcode is then the height byte | 10h. */
static void grid_vertex(u32 v, u16 pos)
{
    u32 h = xn_world_height_layer[pos];
    s32 x = xn_tgrid_x - xn_terrain_height_cam_x[h];
    s32 y = xn_tgrid_y - xn_terrain_height_cam_y[h];
    s32 z = xn_tgrid_z - xn_terrain_height_cam_z[h];
    struct xn_vert_cam *c = cam_at(v);
    struct xn_vert_screen *s = screen_at(v);
    struct xn_vert_flags *f = flags_at(v);
    u8 code = (u8)(h | 0x10);
    xn_s64 n;
    u32 inv_z;

    if (z > -0x12C00) {
        unscaled_x_at(v)->value = x;
        unscaled_y_at(v)->value = y;
        c->x = xn_mulshr(x, xn_cam_scale_x, 14);
        c->y = xn_mulshr(y, xn_cam_scale_y, 14);
        c->z = z;
        code = (u8)(h & 0x80) | outcode(c->x, c->y, z);
        if ((code & 0x7F) == 0) {
            n.lo = 0;                   /* 2^40 / z */
            n.hi = 0x100;
            inv_z = xn_u64_div(&n, z);
            s->inv_z = inv_z;
            s->sx = (u32)(xn_mulhi(c->x * xn_tgrid_sx, inv_z) + xn_tgrid_cx) >> 3;
            s->sy = (u32)(xn_mulhi(c->y * xn_tgrid_sy, inv_z) + xn_tgrid_cy) >> 8;
        }
    }
    f->terrain_outcode = code;
    f->terrain_tile = xn_world_tile_layer[pos];
    f->terrain_flat = xn_world_flat_layer[pos];
}

void xn_terrain_transform_grid(void)
{
    xn_vec3 corner;
    u32 v = 0;
    u8 row, col;

    /* the grid's corner, 16 squares back and left of the eye's square, in camera space */
    corner.x = (-(xn_cam_x & 0xFF) - 0x1000) << 8;
    corner.y = 0;
    corner.z = ((-xn_cam_z & 0xFF) + 0x1000) << 8;
    xn_mat_transform_wide(&corner, &xn_cam_rotation);
    xn_tgrid_row_x = corner.x;
    xn_tgrid_row_y = corner.y;
    xn_tgrid_row_z = corner.z;
    xn_tgrid_col_dx = xn_terrain_step_x.x;
    xn_tgrid_col_dy = xn_terrain_step_x.y;
    xn_tgrid_col_dz = xn_terrain_step_x.z;
    xn_tgrid_row_dx = xn_terrain_step_z.x;
    xn_tgrid_row_dy = xn_terrain_step_z.y;
    xn_tgrid_row_dz = xn_terrain_step_z.z;
    row = grid_first_row();
    col = grid_first_col();
    xn_tgrid_end = 32 * XN_GRID_ROW;    /* (stored every call) */
    do {
        xn_tgrid_x = xn_tgrid_row_x;
        xn_tgrid_y = xn_tgrid_row_y;
        xn_tgrid_z = xn_tgrid_row_z;
        xn_tgrid_row_end = v + XN_GRID_ROW;
        do {
            grid_vertex(v, (u16)(row << 8 | col));
            xn_tgrid_x += xn_tgrid_col_dx;
            xn_tgrid_y += xn_tgrid_col_dy;
            xn_tgrid_z += xn_tgrid_col_dz;
            v += XN_GRID_VERT;
            col++;
        } while (v != xn_tgrid_row_end);
        xn_tgrid_row_x += xn_tgrid_row_dx;
        xn_tgrid_row_y += xn_tgrid_row_dy;
        xn_tgrid_row_z += xn_tgrid_row_dz;
        row++;
        col -= 32;
    } while (v != xn_tgrid_end);
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
   (a x b) with each product's high dword, the edges << 4; the asm keeps a.y, a.z and b in the
   shared scratch vectors. Back-facing: the plane's value at the eye, n . P(v) (the last sum
   exact), is not negative. */
static int face_plane(u32 v, struct xn_poly *p, u32 a0, u32 a1, u32 b0, u32 b1)
{
    struct xn_scratch *t = &xn_scratch_vecs;
    xn_vec3 p0, pa0, pa1, pb0, pb1;
    s32 ax, nx, ny, nz, d;

    plane_vertex(v, &p0);
    plane_vertex(a0, &pa0);
    plane_vertex(a1, &pa1);
    plane_vertex(b0, &pb0);
    plane_vertex(b1, &pb1);
    ax = (pa1.x - pa0.x) << 4;
    t->a.y = (pa1.y - pa0.y) << 4;
    t->a.z = (pa1.z - pa0.z) << 4;
    t->b.x = (pb1.x - pb0.x) << 4;
    t->b.y = (pb1.y - pb0.y) << 4;
    t->b.z = (pb1.z - pb0.z) << 4;
    ny = xn_mulhi(t->b.x, t->a.z) - xn_mulhi(ax, t->b.z);
    nz = xn_mulhi(ax, t->b.y) - xn_mulhi(t->a.y, t->b.x);
    nx = xn_mulhi(t->a.y, t->b.z) - xn_mulhi(t->a.z, t->b.y);
    p->ny = ny;
    p->nz = nz;
    p->nx = nx;
    d = nz * p0.z + nx * p0.x;
    if (sum_ge0(d, ny * p0.y))
        return 0;
    d += ny * p0.y;
    p->inv_z_dx = xn_muldiv(nx * 4, xn_cam_inv_focal_x, d);
    xn_span_dzdx = p->inv_z_dx;
    p->light_list = xn_render_light_list_next;
    p->owner = 1;
    return 1;
}

int xn_terrain_face_plane_a(u32 v, struct xn_poly *p)
{
    return face_plane(v, p, v, v + XN_GRID_ROW, v + XN_GRID_ROW, v + XN_GRID_ROW + XN_GRID_VERT);
}

void xn_terrain_face_plane_a_r(xn_regs *r)
{
    XN_SETFLAG(r, XN_CF, !xn_terrain_face_plane_a(r->esi, (struct xn_poly *)r->edi));
}

int xn_terrain_face_plane_b(u32 v, struct xn_poly *p)
{
    return face_plane(v, p, v, v + XN_GRID_VERT, v + XN_GRID_ROW + XN_GRID_VERT, v + XN_GRID_VERT);
}

void xn_terrain_face_plane_b_r(xn_regs *r)
{
    XN_SETFLAG(r, XN_CF, !xn_terrain_face_plane_b(r->esi, (struct xn_poly *)r->edi));
}

/* ---- a cell's polygons ------------------------------------------------------------------ */

/* the corners of the polygons, from the cell's first vertex */
static const u32 tri_a_corners[3] = { 0, XN_GRID_ROW, XN_GRID_ROW + XN_GRID_VERT };
static const u32 tri_b_corners[3] = { 0, XN_GRID_ROW + XN_GRID_VERT, XN_GRID_VERT };
static const u32 quad_corners[4] = { 0, XN_GRID_ROW, XN_GRID_ROW + XN_GRID_VERT, XN_GRID_VERT };

/* xn_poly_rasterize (poly group, asm; EBP in): the polygon in xn_poly_vertex_buf_a from its
   top vertex (the ring entry ring, its row y). Its row's inputs edx and ebx are pass-through
   registers it reads only when it has no edge to walk. */
static void rasterize(s32 y, struct xn_poly_vertex **ring)
{
    extern void asm_xn_poly_rasterize(void);
    xn_regs r = { 0 };

    r.eax = y;
    r.ebp = (u32)ring;
    xn_asmcall(asm_xn_poly_rasterize, &r);
}

/* xn_poly_project_terrain (poly group, asm): clips the polygon of the bytes of camera-space
   vertices in xn_poly_vertex_buf_a, projects and rasterizes it (ECX in; its other row inputs
   are registers it sets before use) */
static void project_terrain(u32 bytes)
{
    extern void asm_xn_poly_project_terrain(void);
    xn_regs r = { 0 };

    r.ecx = bytes;
    xn_asmcall(asm_xn_poly_project_terrain, &r);
}

/* A polygon inside the view: the corners' projections into the clipper's buffer, the
   rasterizer from the top one (the last of the least rows: the asm takes a corner when it is
   not below the top so far); the polygon record is used. */
static void draw_projected(u32 v, const u32 *corner, int n)
{
    struct xn_vert_screen *s;
    s32 top_y = 0;
    int k, top = 0;

    xn_poly_vertex_count = (u8)n;
    for (k = 0; k < n; k++) {
        s = screen_at(v + corner[k]);
        xn_poly_vertex_buf_a[k].x = s->sx;
        xn_poly_vertex_buf_a[k].y = s->sy;
        xn_poly_vertex_buf_a[k].z = s->inv_z;
        if (k == 0 || s->sy <= top_y) {
            top_y = s->sy;
            top = k;
        }
    }
    rasterize(top_y, xn_poly_ring_a[n] + top);
    xn_render_poly_next++;
}

/* A polygon partly outside the view: the corners in camera space with their outcodes, to the
   clipper; the polygon record is used. */
static void draw_clipped(u32 v, const u32 *corner, int n)
{
    struct xn_vert_cam *c;
    int k;

    for (k = 0; k < n; k++) {
        c = cam_at(v + corner[k]);
        xn_poly_vertex_buf_a[k].x = c->x;
        xn_poly_vertex_buf_a[k].y = c->y;
        xn_poly_vertex_buf_a[k].z = c->z;
        xn_poly_vertex_buf_a[k].outcode = flags_at(v + corner[k])->terrain_outcode;
    }
    project_terrain(n * sizeof(struct xn_poly_vertex));
    xn_render_poly_next++;
}

static void set_clip_outcodes(u16 outcodes)
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
    set_clip_outcodes((u16)outcodes);
    if (xn_terrain_face_plane_a(v, p))
        draw_clipped(v, tri_a_corners, 3);
}

void xn_terrain_draw_tri_b_clipped(u32 v, struct xn_poly *p, u32 outcodes)
{
    set_clip_outcodes((u16)outcodes);
    if (xn_terrain_face_plane_b(v, p))
        draw_clipped(v, tri_b_corners, 3);
}

/* ---- the cells -------------------------------------------------------------------------- */

/* xn_tex_cache_lookup (tex group, asm): the archive's record (frame -1: by the clock); 0 and
   CF when the archive cannot be loaded. edi: the polygon, as the asm passes it (the row lists
   it, as ebp, as a pass-through of the archive loader). */
static int tex_lookup(u32 archive, u32 record, struct xn_poly *p, struct xn_tex_entry **entry)
{
    extern void asm_xn_tex_cache_lookup(void);
    xn_regs r = { 0 };

    r.eax = archive;
    r.edx = record;
    r.ebx = (u32)-1;
    r.edi = (u32)p;
    xn_asmcall(asm_xn_tex_cache_lookup, &r);
    *entry = (struct xn_tex_entry *)r.eax;
    return !(r.eflags & XN_CF);
}

/* call a cell's axis routine (an asm entry of the tables) for the polygon */
static void call_axis(u32 routine, struct xn_poly *p)
{
    xn_regs r = { 0 };

    r.edi = (u32)p;
    xn_asmcall((void (*)(void))routine, &r);
}

/* The next polygon record as the cell's: the span setup, the texture axes of the tile's
   rotation (bits 6-7) and flips (the flat byte's bits 0 and 1), the tile's texture */
static struct xn_poly *cell_poly(u32 v, u32 setup, u32 archive)
{
    struct xn_poly *p = xn_render_poly_next;
    struct xn_vert_flags *f = flags_at(v);
    u32 rot = f->terrain_tile >> 6;

    p->span_fn = (void (*)(void))setup;
    call_axis(xn_terrain_u_axis_fns[rot * 2 + (f->terrain_flat & 1)], p);
    call_axis(xn_terrain_v_axis_fns[rot * 2 + ((f->terrain_flat & 2) >> 1)], p);
    tex_lookup(archive, f->terrain_tile & 0x3F, p, &p->tex);
    return p;
}

/* the outcodes of the corners so far: their AND (low byte) and OR (high byte), taking a
   corner's outcode byte (with its bit 7) */
static u16 with_corner(u16 outcodes, u32 v)
{
    u8 c = flags_at(v)->terrain_outcode;

    return (u16)(((outcodes & 0xFF) & c) | ((outcodes >> 8 | c) << 8));
}

/* every corner outside one plane of the view: nothing to draw */
#define ALL_OUTSIDE(oc)     (((oc) & 0x7F) != 0)
/* some corner outside some plane: clip */
#define SOME_OUTSIDE(oc)    (((oc) >> 8 & 0x7F) != 0)
/* the outcodes for the clipper: the OR without bit 7 */
#define CLIP_OUTCODES(oc)   ((u16)((oc) & 0x7FFF))

static void draw_cell(u32 v, u32 setup, u32 archive)
{
    u8 first = flags_at(v)->terrain_outcode & 0x7F;
    u16 oc = (u16)(first << 8 | first);
    u16 diag;
    struct xn_poly *p;

    if (flags_at(v)->terrain_outcode & 0x80) {
        /* two triangles, both on the diagonal v .. v + a row + 1 */
        diag = with_corner(oc, v + XN_GRID_ROW + XN_GRID_VERT);
        oc = with_corner(diag, v + XN_GRID_ROW);
        if (!ALL_OUTSIDE(oc)) {
            p = cell_poly(v, setup, archive);
            if (SOME_OUTSIDE(oc))
                xn_terrain_draw_tri_a_clipped(v, p, CLIP_OUTCODES(oc));
            else
                xn_terrain_draw_tri_a(v, p);
        }
        oc = with_corner(diag, v + XN_GRID_VERT);
        if (!ALL_OUTSIDE(oc)) {
            p = cell_poly(v, setup, archive);
            if (SOME_OUTSIDE(oc))
                xn_terrain_draw_tri_b_clipped(v, p, CLIP_OUTCODES(oc));
            else
                xn_terrain_draw_tri_b(v, p);
        }
        return;
    }
    /* one quad: a's plane */
    oc = with_corner(oc, v + XN_GRID_ROW);
    oc = with_corner(oc, v + XN_GRID_ROW + XN_GRID_VERT);
    oc = with_corner(oc, v + XN_GRID_VERT);
    if (ALL_OUTSIDE(oc))
        return;
    p = cell_poly(v, setup, archive);
    if (SOME_OUTSIDE(oc)) {
        set_clip_outcodes(CLIP_OUTCODES(oc));
        if (xn_terrain_face_plane_a(v, p))
            draw_clipped(v, quad_corners, 4);
    } else if (xn_terrain_face_plane_a(v, p))
        draw_projected(v, quad_corners, 4);
}

int xn_terrain_draw_cells(void)
{
    u32 archive, setup;
    struct xn_tex_entry *tex;
    u32 v = 0;
    s32 rows, cols, row_u;

    xn_terrain_setup_tex_gradients();
    archive = xn_world_ground_archive;
    xn_tcells_archive_a = xn_tcells_archive_b = xn_tcells_archive_c = archive;    /* KEEP */
    if (!tex_lookup(archive, 0, 0, &tex))
        return 0;
    setup = xn_render_span_setup_terrain_ptr;
    xn_tcells_setup_a = xn_tcells_setup_b = xn_tcells_setup_c = setup;            /* KEEP */
    for (rows = 31; rows != 0; rows--) {
        row_u = xn_world_scratch_x;
        for (cols = 31; cols != 0; cols--) {
            draw_cell(v, setup, archive);
            v += XN_GRID_VERT;
            xn_world_scratch_x -= 0x3E00;       /* the next cell's texture origin */
        }
        xn_world_scratch_x = row_u;
        v += XN_GRID_VERT;                      /* (the row's last vertex starts no cell) */
        xn_world_scratch_y -= 0x3E00;
    }
    return 1;
}

void xn_terrain_draw_cells_r(xn_regs *r)
{
    XN_SETFLAG(r, XN_CF, !xn_terrain_draw_cells());
}

/* ---- the texture axes ------------------------------------------------------------------- */

/* The high dword of a . v (three 64-bit products), negated, >> 8, + 100h: a texture origin
   at the grid's corner */
static s32 corner_origin(const s32 *axis, const xn_vec3 *v)
{
    xn_s64 t;

    xn_s64_mul(&t, axis[0], v->x);
    xn_s64_mac(&t, axis[1], v->y);
    xn_s64_mac(&t, axis[2], v->z);
    return (-t.hi >> 8) + 0x100;
}

void xn_terrain_setup_tex_gradients(void)
{
    xn_vec3 corner;
    s32 d;

    corner.x = (-(xn_cam_x & 0xFF) - 0x1000) << 18;
    corner.y = 0;
    corner.z = ((-xn_cam_z & 0xFF) + 0x1000) << 18;
    xn_mat_transform_wide(&corner, &xn_cam_rotation);
    xn_world_scratch_x = corner_origin(xn_terrain_u_axis, &corner);
    xn_world_scratch_y = corner_origin(xn_terrain_v_axis, &corner);
    d = xn_cam_focal_x << 11;
    xn_terrain_u_axis[0] = xn_idiv(xn_terrain_u_axis[0], d);
    xn_terrain_v_axis[0] = xn_idiv(xn_terrain_v_axis[0], d);
    d = xn_cam_focal_y << 11;
    xn_terrain_u_axis[1] = xn_idiv(xn_terrain_u_axis[1], d);
    xn_terrain_v_axis[1] = xn_idiv(xn_terrain_v_axis[1], d);
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

/* The u routines store the whole packed origin (u in the low word; the v routine then
   replaces the high word) */
static s32 set_origin(struct xn_poly *p, s32 origin)
{
    p->tex_u0 = (u16)origin;
    p->tex_v0 = (u16)((u32)origin >> 16);
    return origin;
}

static s32 set_v_origin(struct xn_poly *p, s32 origin)
{
    p->tex_v0 = (u16)origin;
    return origin;
}

s32 xn_terrain_u_axis_x(struct xn_poly *p)
{
    set_u(p, xn_terrain_u_axis, 0);
    return set_origin(p, xn_world_scratch_x);
}

s32 xn_terrain_u_axis_z(struct xn_poly *p)
{
    set_u(p, xn_terrain_v_axis, 0);
    return set_origin(p, xn_world_scratch_y);
}

s32 xn_terrain_u_axis_neg_x(struct xn_poly *p)
{
    set_u(p, xn_terrain_u_axis, 1);
    return set_origin(p, 0x4000 - xn_world_scratch_x);
}

s32 xn_terrain_u_axis_neg_z(struct xn_poly *p)
{
    set_u(p, xn_terrain_v_axis, 1);
    return set_origin(p, 0x4000 - xn_world_scratch_y);
}

s32 xn_terrain_v_axis_z(struct xn_poly *p)
{
    set_v(p, xn_terrain_v_axis, 0);
    return set_v_origin(p, xn_world_scratch_y);
}

s32 xn_terrain_v_axis_x(struct xn_poly *p)
{
    set_v(p, xn_terrain_u_axis, 0);
    return set_v_origin(p, xn_world_scratch_x);
}

s32 xn_terrain_v_axis_neg_z(struct xn_poly *p)
{
    set_v(p, xn_terrain_v_axis, 1);
    return set_v_origin(p, 0x4000 - xn_world_scratch_y);
}

s32 xn_terrain_v_axis_neg_x(struct xn_poly *p)
{
    set_v(p, xn_terrain_u_axis, 1);
    return set_v_origin(p, 0x4000 - xn_world_scratch_x);
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
    xn_vec3 *tri = xn_collide_work.ground_tri;
    s32 fx, fz;
    u8 row, col;

    xn_world_scratch_x = x;
    xn_world_scratch_y = xn_world_size_z - z;
    col = (u8)((u32)x >> 8);
    row = (u8)((u32)xn_world_scratch_y >> 8);
    fx = xn_world_scratch_x & 0xFF;
    fz = xn_world_scratch_y & 0xFF;
    /* the square's corners at 0 and 10000h: its first corner, then the triangle's two others
       on the side of the diagonal the point is on */
    tri[0].y = square_height(row, col);
    if (fz - fx > 0) {
        set_point(&tri[1], 0, square_height(row + 1, col), 0x10000);
        set_point(&tri[2], 0x10000, square_height(row + 1, col + 1), 0x10000);
    } else {
        set_point(&tri[2], 0x10000, square_height(row, col + 1), 0);
        set_point(&tri[1], 0x10000, square_height(row + 1, col + 1), 0x10000);
    }
    tri[0].x = 0;
    tri[0].z = 0;
    return -((xn_math_triangle_y_at(tri, fx << 8, fz << 8) + 0x80) >> 8);
}
