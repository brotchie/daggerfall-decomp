/* flat.c: XnGine's flats, the billboard sprites (canonical C; the interface and the module's
   documentation are in xflat.h). */
#include "xflat.h"
#include "xcam.h"
#include "xtex.h"
#include "xmat.h"
#include "xmath.h"
#include "xspan.h"
#include "xlight.h"
#include "xshade.h"                     /* the fog */

/* ---- queueing ------------------------------------------------------------------------------ */

u32 xn_flat_add(s32 x, s32 y, s32 z, u32 image, s32 frame, u32 flags, u32 scale)
{
    return xn_flat_add_body(x, y, z, image, frame, flags, scale);
}

/* a flat record and its sort entry (key -z) */
static struct xn_flat *new_flat(u32 image, s32 frame, u32 flags, const xn_vec3 *view)
{
    struct xn_flat *flat = (struct xn_flat *)xn_render_poly_next++;
    struct xn_sort_pair *pair;

    flat->image = image;
    flat->frame = frame;
    flat->flags = flags;
    flat->view_x = view->x;
    flat->view_y = view->y;
    flat->view_z = view->z;
    flat->object = 0;
    pair = xn_flat_sort_end++;
    pair->value = flat;
    pair->key = -view->z;
    return flat;
}

u32 xn_flat_add_body(s32 x, s32 y, s32 z, u32 image, s32 frame, u32 flags, u32 scale)
{
    struct xn_flat *flat;
    xn_vec3 v;

    v.x = (x - xn_cam_x) << 8;
    v.y = (y - xn_cam_y) << 8;
    v.z = (z - xn_cam_z) << 8;
    xn_mat_transform(&v, &xn_cam_view_matrix);
    if (v.z <= 0 || v.z >= xn_cam_far_z)
        return scale;                   /* Quirk Q-FLAT-02 */
    if (++xn_flat_count > 0x200)
        return scale;
    flat = new_flat(image, frame, flags, &v);
    *(u32 *)&flat->scale = scale;       /* the scale word, the light byte and one more */
    return (u32)flat;
}

void xn_flat_add_view(s32 x, s32 y, s32 z, u32 image)
{
    struct xn_flat *flat;
    xn_vec3 v;

    if (++xn_flat_count > 0x200)
        return;
    v.x = x;
    v.y = y;
    v.z = z;
    flat = new_flat(image, 0, 4, &v);
    *(u32 *)&flat->scale = 0x100;
}

/* ---- the frame ----------------------------------------------------------------------------- */

void xn_flat_begin_frame(void)
{
    const s32 *f = &xn_flat_matrix.m[0][0];
    s32 *p = &xn_flat_proj_matrix.m[0][0];
    int k;

    xn_mat_from_angles(xn_flat_ignore_pitch == 0x64 ? 0 : xn_cam_pitch, 0, 0, &xn_flat_matrix);
    xn_cam_scale_matrix(&xn_flat_matrix, &xn_flat_matrix);
    for (k = 0; k < 3; k++)
        p[k] = xn_mulhi(f[k], xn_cam_flat_scale_x);
    for (k = 3; k < 6; k++)
        p[k] = xn_mulhi(f[k], xn_cam_flat_scale_y);
    for (k = 6; k < 9; k++)
        p[k] = f[k] >> 1;
}

/* A screen axis of the frame's flats: (ax, ay, 0) through the projection matrix (64-bit
   products), y and z / 8 rounded */
static void flat_axis(s32 ax, s32 ay, xn_vec3 *a)
{
    a->x = ax;
    a->y = ay;
    a->z = 0;
    xn_mat_transform_wide(a, &xn_flat_proj_matrix);
    a->y = (a->y + 4) >> 3;
    a->z = (a->z + 4) >> 3;
}

/* ---- the quads ----------------------------------------------------------------------------- */

void xn_flat_rotate_offset(s32 ox, s32 oy, xn_vec3 *out)
{
    const s32 *m = &xn_flat_matrix.m[0][0];

    ox <<= 4;
    oy <<= 4;
    out->x = xn_mulhi(ox, m[0]) + xn_mulhi(oy, m[1]);
    out->y = xn_mulhi(ox, m[3]) + xn_mulhi(oy, m[4]);
    out->z = xn_mulhi(oy, m[7]);
}

/* a corner: the flat's position plus an offset, its outcode merged into the clipper's */
static void corner(struct xn_poly_vertex *v, const struct xn_flat *flat, s32 ox, s32 oy, s32 oz)
{
    v->x = flat->view_x + ox;
    v->y = flat->view_y + oy;
    v->z = flat->view_z + oz;
    v->outcode = (u8)xn_poly_outcode(v->x, v->y, v->z);
    xn_poly_clip_outcode_and &= v->outcode;
    xn_poly_clip_outcode_or |= v->outcode;
}

void xn_flat_quad_centred(u32 w, u32 h, const struct xn_flat *flat)
{
    struct xn_poly_vertex *q = xn_poly_vertex_buf_a;
    s32 hw = w >> 1, hh = h >> 1;
    xn_vec3 a, b;

    xn_poly_clip_outcode_and = 0xFF;
    xn_poly_clip_outcode_or = 0;
    xn_flat_rotate_offset(-hw, -hh, &a);
    corner(&q[0], flat, a.x, a.y, a.z);
    xn_flat_rotate_offset(hw, -hh, &b);
    corner(&q[1], flat, b.x, b.y, b.z);
    corner(&q[3], flat, -b.x, -b.y, -b.z);
    corner(&q[2], flat, -a.x, -a.y, -a.z);
}

void xn_flat_quad_standing(u32 w, u32 h, const struct xn_flat *flat)
{
    struct xn_poly_vertex *q = xn_poly_vertex_buf_a;
    s32 hw = w >> 1;
    xn_vec3 a, b;

    xn_poly_clip_outcode_and = 0xFF;
    xn_poly_clip_outcode_or = 0;
    xn_flat_rotate_offset(-hw, -(s32)h, &a);
    corner(&q[0], flat, a.x, a.y, a.z);
    xn_flat_rotate_offset(hw, -(s32)h, &b);
    corner(&q[1], flat, b.x, b.y, b.z);
    corner(&q[3], flat, -b.x, 0, 0);    /* the base: at the flat's position, x only */
    corner(&q[2], flat, -a.x, 0, 0);
}

void xn_flat_quad_none(void)
{
}

void xn_flat_quad_none_8(void)
{
}

void xn_flat_quad_none_16(void)
{
}

/* The quad of the flat's kind (flags & 1Fh), w x h view units. Quirk Q-FLAT-03: the kinds
   without a quad leave the clipper's buffer and AND as the last quad left them. Kinds 17-31
   read past the asm's 17-entry table into the flat sort list (Q-FLAT-06, dropped: no caller
   passes them): here they have no quad. */
static void build_quad(const struct xn_flat *flat, u32 w, u32 h)
{
    switch (flat->flags & 0x1F) {
    case 0:
    case 1:
        xn_flat_quad_centred(w, h, flat);
        break;
    case 4:
    case 5:
    case 6:
    case 7:
        xn_flat_quad_standing(w, h, flat);
        break;
    default:
        break;
    }
}

/* ---- drawing ------------------------------------------------------------------------------ */

int xn_flat_draw(struct xn_flat *flat)
{
    struct xn_tex_entry *e;
    struct xn_tex_image *image;
    xn_flat_walk w;
    u32 scale;

    e = xn_tex_cache_lookup(flat->image >> 7, flat->image & 0x7F, flat->frame);
    if (e == 0)
        return 1;
    flat->table = xn_shade_blend_tables[e->blend_index];
    image = e->current;
    scale = (u16)(flat->scale + image->x_scale);
    flat->texels = (u8 *)image + image->data_offset;
    flat->scale = (u16)scale;
    build_quad(flat, image->width * scale, image->height * scale);
    if (xn_poly_clip_outcode_and)
        return 0;
    /* the first corner, for the pick */
    xn_pick_view_x = xn_poly_vertex_buf_a[0].x >> 3;
    xn_pick_view_y = xn_poly_vertex_buf_a[0].y >> 3;
    pick_distance = xn_poly_vertex_buf_a[0].z >> 3;
    xn_flat_drawn_count++;
    flat->span = xn_flat_span_light_setup;
    w.flat = flat;
    w.tex_w = image->width;
    xn_poly_project_flat(&w, xn_poly_vertex_buf_a);
    return 0;
}

void xn_flat_setup_gradients(xn_flat_walk *w, s32 top_y)
{
    struct xn_flat *flat = w->flat;
    s32 sx = xn_pick_view_x * xn_cam_half_width;       /* the flat's position on the screen */
    s32 sy = xn_pick_view_y * xn_cam_half_height;
    s32 k = xn_udiv64_or0(2, 0, *(u32 *)&flat->scale & 0xFFFF);       /* 2^33 / scale */
    xn_vec3 u, v;
    s32 dy;

    flat_axis(0x40000000, 0, &u);
    flat_axis(0, 0x40000000, &v);
    flat->du_dx = xn_mulhi(u.x, k);
    flat->u_dx = flat->du_dx >> 3;
    flat->u_dy = xn_mulhi(u.y, k);
    flat->u_c = xn_mulhi(u.z, k);
    flat->dv_dx = xn_mulhi(v.x, k);
    flat->u_offset = (sx * flat->u_dx + sy * flat->u_dy + pick_distance * flat->u_c) >> 1;
    flat->v_dx = flat->dv_dx >> 3;
    flat->v_dy = xn_mulhi(v.y, k);
    flat->v_c = xn_mulhi(v.z, k);
    flat->v_offset = sx * flat->v_dx + sy * flat->v_dy + pick_distance * flat->v_c;
    /* the constants at the top row; the walk steps them per row */
    dy = top_y - xn_cam_centre_y;
    w->u_step = flat->u_dy;
    flat->u_c += w->u_step * dy;
    w->v_step = flat->v_dy - 8;
    flat->v_c += w->v_step * dy;
    if (flat->flags & 0x20) {           /* mirrored */
        flat->du_dx = -flat->du_dx;
        flat->u_dx = -flat->u_dx;
        flat->u_offset = -flat->u_offset - (s32)(w->tex_w << 23);
    }
}

/* 1/z of the span `node` at x (from its start, along its polygon's gradient) */
static s32 inv_z_at(const struct xn_span *node, s32 x)
{
    return (x - node->x_start) * node->poly->inv_z_dx + node->inv_z;
}

void xn_flat_span_clip(xn_flat_walk *w, struct xn_span *node, s32 x0, s32 x1, s32 inv_z)
{
    for (;;) {
        node = node->next;
        if ((s16)node->x_end <= (s16)x0)
            continue;                   /* ends before the piece */
        if ((s16)x1 <= (s16)node->x_start) {
            xn_flat_span_emit(w, x0, x1, inv_z);        /* starts after it: the rest shows */
            return;
        }
        if ((s16)x1 < (s16)node->x_end) {
            /* the piece ends over this span: draw what is in front of it */
            if (node->x_start == x0) {
                if (inv_z > node->inv_z)
                    xn_flat_span_emit(w, x0, x1, inv_z);
            } else if (node->x_start < x0) {
                if (inv_z_at(node, x0) < inv_z)
                    xn_flat_span_emit(w, x0, x1, inv_z);
            } else if (inv_z > node->inv_z) {
                xn_flat_span_emit(w, x0, x1, inv_z);
            } else {
                xn_flat_span_emit(w, x0, (x1 & 0xFFFF0000) | node->x_start, inv_z);
            }
            return;
        }
        /* the piece goes on past this span */
        if (node->x_start == x0) {
            if (node->inv_z < inv_z)
                continue;               /* the flat is in front: keep the piece */
        } else if (node->x_start > x0) {
            if (inv_z > node->inv_z)
                continue;
            /* behind the span: draw the gap before it, go on after it */
            xn_flat_span_emit(w, x0, (x1 & 0xFFFF0000) | node->x_start, inv_z);
            x0 = node->x_end;
            if (x0 < x1)
                continue;
            return;
        } else if (inv_z_at(node, x0) < inv_z) {
            continue;
        }
        /* hidden up to the span's end */
        x0 = (x0 & 0xFFFF0000) | node->x_end;
        if (x0 >= x1)
            return;
    }
}

void xn_flat_span_emit(xn_flat_walk *w, s32 x0, s32 x1, s32 inv_z)
{
    struct xn_flat *flat = w->flat;

    flat->span(flat, inv_z, x0, x1 - x0, w->row + x0);  /* the first piece: the light setup */
}

void xn_flat_raster_rows(xn_flat_walk *w, struct xn_span *row)
{
    xn_edge_walk *e = &w->edges;

    for (;;) {
        s32 x0 = (u32)e->xr >> 21;              /* the flat's ring runs the other way round */
        s32 x1 = ((u32)e->xl >> 21) - 1;

        if (x1 > x0)
            xn_flat_span_clip(w, row, x0, x1, e->zl);
        row++;
        w->flat->u_c += w->u_step;
        w->flat->v_c += w->v_step;
        w->row += xn_gfx_width;
        e->rows -= 0x10001;
        e->xl += e->dxl;
        e->xr += e->dxr;
        e->zl += e->dzl;
        if ((e->rows & 0xFFFF) == 0 && !xn_walk_left_edge(e))
            return;
        if ((e->rows & 0xFFFF0000) == 0 && !xn_walk_right_edge(e))
            return;
    }
}

void xn_flat_raster(xn_flat_walk *w, s32 top_y, struct xn_poly_vertex **ring, int n)
{
    xn_edge_walk *e = &w->edges;

    e->left = ring;
    e->right = ring;
    e->count = (s8)n;
    e->rows = 0;
    xn_flat_setup_gradients(w, top_y);
    w->row = screen_buffer + xn_gfx_row_offset[top_y];
    if (!xn_walk_left_edge(e) || !xn_walk_right_edge(e))
        return;
    xn_flat_raster_rows(w, &xn_render_span_rows[top_y]);
}

/* ---- the light setup ----------------------------------------------------------------------- */

/* the fog row of a flat beyond the fog's start (its depth in whole units past it) */
static u8 *fog_row(const struct xn_flat *flat)
{
    s32 d = (s32)((u32)flat->view_z >> 8) - xn_fog_start;

    return (u8 *)(xn_fog_table_last - d * xn_fog_step);
}

/* beyond the fog's start: the true comparison (the asm's `sub; jle`) */
static int fogged(const struct xn_flat *flat)
{
    return (s32)((u32)flat->view_z >> 8) > xn_fog_start;
}

void xn_flat_span_light_setup(struct xn_flat *flat, u32 inv_z, s32 x, s32 n, u8 *pix)
{
    const u8 *last = xn_shade_table_last_row;
    const struct xn_light *light = xn_light_table;
    s32 k = xn_light_count;
    u8 *row;

    (void)inv_z;                        /* Quirk Q-FLAT-01: the piece is not drawn */
    (void)x;
    (void)n;
    (void)pix;
    if (flat->table != 0) {
        flat->span = (xn_flat_span_fn)xn_span_flat_translucent;
        return;
    }
    row = xn_light_ambient_row + ((*(u32 *)&flat->scale >> 8) & 0xFF00);
    if (row < last && k != 0) {
        if (k > 32)
            k = 32;
        do {
            if (light->type == 8) {
                /* Quirk Q-FLAT-04: a directional light's whole intensity, again and again
                   (the asm jumps back to the same light) until the last row */
                do
                    row += light->intensity << 8;
                while (row < last);
                break;
            } else {
                s32 dx = (light->x - flat->view_x) >> 8;
                s32 dy = (light->y - flat->view_y) >> 8;
                s32 dz = (light->z - flat->view_z) >> 8;
                s32 d2 = dx * dx + dy * dy + dz * dz;

                if (d2 < light->range_sq) {
                    row += xn_muldiv_or0(xn_math_isqrt_lookup(d2) << 16, light->intensity, d2);
                    if (row >= last)
                        break;
                }
            }
            light = (const struct xn_light *)((const u8 *)light + 29);
        } while (--k != 0);
    }
    if (row < last) {
        flat->shade_row = row;
        flat->span = (xn_flat_span_fn)xn_span_flat_transparent_shaded;
        if (fogged(flat)) {
            flat->span = (xn_flat_span_fn)xn_span_flat_lit_fogged;
            flat->table = fog_row(flat);
        }
    } else {                            /* at the last row: unlit */
        flat->span = (xn_flat_span_fn)xn_span_flat_transparent;
        if (fogged(flat)) {
            flat->span = (xn_flat_span_fn)xn_span_flat_transparent_shaded;
            flat->shade_row = fog_row(flat);
        }
    }
}

/* ---- the pick ------------------------------------------------------------------------------ */

struct xn_flat *xn_flat_pick(s32 x, s32 y)
{
    struct xn_sort_pair *pair;
    u32 n;

    xn_scratch_vec_b.x = x;
    xn_scratch_vec_b.y = y;
    if (xn_pick_skip_flats || xn_flat_count == 0)
        return 0;
    n = xn_flat_count;
    pair = &xn_flat_sort_list[n - 1];
    do {                                /* nearest first */
        struct xn_flat *flat = (struct xn_flat *)pair->value;
        /* Quirk Q-FLAT-05: a missing image (the cache full) is read at address 0 */
        struct xn_tex_image *image = xn_tex_cache_lookup_image(flat->image >> 7,
                                                               flat->image & 0x7F);
        u32 scale = *(u32 *)&flat->scale & 0xFFFF;

        build_quad(flat, image->width * scale, image->height * scale);
        if (xn_poly_clip_outcode_and == 0) {
            const struct xn_poly_vertex *q = xn_poly_vertex_buf_a;
            s32 z = q[0].z;
            s32 px = xn_muldiv_or0(xn_scratch_vec_b.x - xn_cam_centre_x, z, xn_cam_focal_x);

            xn_pick_flat_x = px;
            if (px > q[0].x && px < q[1].x) {
                s32 py = xn_muldiv_or0(xn_scratch_vec_b.y - xn_cam_centre_y, z, xn_cam_focal_y);

                xn_pick_flat_y = py;
                if (py > q[0].y && py < q[2].y) {
                    xn_pick_flat_z = (z + 0x80) >> 8;
                    return flat;
                }
            }
        }
        pair--;
    } while (--n != 0);
    return 0;
}
