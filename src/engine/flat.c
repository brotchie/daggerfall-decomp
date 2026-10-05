/* flat.c: XnGine's flats (billboard sprites) as readable C (xflat.h; see xngine.h and
   docs/xngine_readable.md). */
#include "xflat.h"
#include "xpoly.h"
#include "xtex.h"
#include "xmat.h"
#include "xmath.h"
#include "xcam.h"

extern struct xn_sort_pair xn_flat_sort_list[512];
extern struct xn_sort_pair *xn_flat_sort_end;
extern s32 xn_flat_count, xn_flat_drawn_count;
extern xn_routine xn_flat_quad_table[17];      /* by flags & 1Fh */
extern u8 *xn_shade_blend_tables[];
extern u32 xn_flat_half_w, xn_flat_half_h;     /* the quad being built */
extern u32 xn_flat_tex_w, xn_flat_tex_h;       /* the flat's image size */
extern u8 xn_flat_ignore_pitch;
extern s32 xn_cam_pitch;
extern xn_mat3 xn_flat_matrix;                 /* the camera pitch, for the corner offsets */
extern xn_mat3 xn_flat_proj_matrix;            /* scaled for the flats' projection */
extern xn_vec3 xn_scratch_vec_b;               /* the pick point (x, y) */
extern s32 xn_pick_flat_x, xn_pick_flat_y, xn_pick_flat_z;
extern u8 xn_pick_skip_flats;
extern s32 xn_render_frame_flags;
/* lights and fog */
extern struct xn_light xn_light_table[33];
extern s32 xn_light_count;
extern s32 xn_shade_table_last_row;            /* the last shade row's address */
extern s32 xn_fog_start, xn_fog_step, xn_fog_table_last;

/* patch fields (smc/flat.md) */
extern struct xn_flat *xn_flat_emit_flat;      /* 15526F: the flat being drawn (154E20) */
extern u8 *xn_flat_emit_row;                   /* 155275: the current row's address - 1 */
extern s32 xn_flat_row_stride;                 /* 155367: the screen width (155415) */
extern s32 *xn_flat_row_u_var, *xn_flat_row_v_var;     /* 15534F 155359: what steps per row */
extern s32 xn_flat_row_u_step, xn_flat_row_v_step;     /* 155353 15535D */
extern s32 xn_flat_grad_sx, xn_flat_grad_sy;   /* 1550ED 1550F7: the half size (12A2D0) */
extern s32 xn_flat_axis_u_x, xn_flat_axis_u_y, xn_flat_axis_u_z;    /* 155110 155120 15512A */
extern s32 xn_flat_axis_v_x, xn_flat_axis_v_y, xn_flat_axis_v_z;    /* 15514C 155163 15516D */
extern s32 xn_flat_ambient_row;                /* 155636 (12A4F0) */
/* the flat walker's fields */
extern struct xn_poly_vertex **xn_flat_raster_left, **xn_flat_raster_right;
extern s32 xn_flat_raster_dxl, xn_flat_raster_dzl, xn_flat_raster_dxr;

/* asm entries stored as data: the span routines this file installs */
extern void asm_xn_flat_span_light_setup(void);
extern void asm_xn_span_flat_transparent(void);
extern void asm_xn_span_flat_transparent_shaded(void);
extern void asm_xn_span_flat_lit_fogged(void);
extern void asm_xn_span_flat_translucent(void);

static const xn_walker flat_walker = {
    &xn_flat_raster_left, &xn_flat_raster_right, &xn_flat_raster_dxl, &xn_flat_raster_dzl,
    &xn_flat_raster_dxr
};

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
        return scale;
    if (++xn_flat_count > 0x200)
        return scale;
    flat = new_flat(image, frame, flags, &v);
    *(u32 *)&flat->scale = scale;       /* the scale word, the light byte and one more */
    return (u32)flat;
}

void xn_flat_add_body_r(xn_regs *r)
{
    r->edi = xn_flat_add_body(r->eax, r->edx, r->ebx, r->ecx, r->ebp, r->esi, r->edi);
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

/* ---- drawing ------------------------------------------------------------------------------ */

/* A quad builder from xn_flat_quad_table (an asm entry), as the asm calls it: returns what it
   leaves in EBX (flags & 1Fh going in) */
static u32 build_quad(const struct xn_flat *flat, u32 w, u32 h)
{
    xn_regs r;

    r.eax = w;
    r.edx = h;
    r.ebx = flat->flags & 0x1F;
    r.edi = (u32)flat;
    r.ecx = r.ebp = r.esi = 0;
    xn_asmcall(xn_flat_quad_table[flat->flags & 0x1F], &r);
    return r.ebx;
}

int xn_flat_draw(struct xn_flat *flat)
{
    struct xn_tex_entry *e;
    struct xn_tex_image *image;
    s32 frame = flat->frame;
    u32 scale;

    e = xn_tex_cache_lookup(flat->image >> 7, flat->image & 0x7F, &frame);
    if (e == 0)
        return 1;
    flat->table = xn_shade_blend_tables[e->blend_index];
    image = e->current;
    scale = (u16)(flat->scale + image->x_scale);
    flat->texels = (u8 *)image + image->data_offset;
    flat->scale = (u16)scale;
    xn_flat_tex_h = image->height;
    xn_flat_tex_w = image->width;
    build_quad(flat, image->width * scale, image->height * scale);
    if (xn_poly_clip_outcode_and)
        return 0;
    /* the first corner, for the pick */
    xn_pick_view_x = xn_poly_vertex_buf_a[0].x >> 3;
    xn_pick_view_y = xn_poly_vertex_buf_a[0].y >> 3;
    pick_distance = xn_poly_vertex_buf_a[0].z >> 3;
    xn_flat_drawn_count++;
    xn_flat_emit_flat = flat;
    flat->span = asm_xn_flat_span_light_setup;
    xn_poly_project_flat(xn_poly_vertex_buf_a);
    return 0;
}

void xn_flat_draw_r(xn_regs *r)
{
    XN_SETFLAG(r, XN_CF, xn_flat_draw((struct xn_flat *)r->edi));
}

/* a corner: the flat's position plus an offset, with its outcode merged into the clipper's */
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
    xn_vec3 a, b;

    xn_flat_half_w = w >> 1;
    xn_flat_half_h = h >> 1;
    xn_poly_clip_outcode_and = 0xFF;
    xn_poly_clip_outcode_or = 0;
    xn_flat_rotate_offset(-(s32)xn_flat_half_w, -(s32)xn_flat_half_h, &a);
    corner(&q[0], flat, a.x, a.y, a.z);
    xn_flat_rotate_offset(xn_flat_half_w, -(s32)xn_flat_half_h, &b);
    corner(&q[1], flat, b.x, b.y, b.z);
    corner(&q[3], flat, -b.x, -b.y, -b.z);
    corner(&q[2], flat, -a.x, -a.y, -a.z);
}

void xn_flat_quad_centred_r(xn_regs *r)
{
    struct xn_poly_vertex *last = &xn_poly_vertex_buf_a[2];

    xn_flat_quad_centred(r->eax, r->edx, (const struct xn_flat *)r->edi);
    r->eax = last->x;
    r->edx = last->y;
    r->ebx = last->z;
    r->ecx = last->outcode;
    r->esi = (u32)xn_poly_vertex_buf_a;
    r->ebp = -(s32)xn_flat_half_h << 4;
}

void xn_flat_quad_standing(u32 w, u32 h, const struct xn_flat *flat)
{
    struct xn_poly_vertex *q = xn_poly_vertex_buf_a;
    xn_vec3 a, b;

    xn_flat_half_w = w >> 1;
    xn_flat_half_h = h;
    xn_poly_clip_outcode_and = 0xFF;
    xn_poly_clip_outcode_or = 0;
    xn_flat_rotate_offset(-(s32)xn_flat_half_w, -(s32)xn_flat_half_h, &a);
    corner(&q[0], flat, a.x, a.y, a.z);
    xn_flat_rotate_offset(xn_flat_half_w, -(s32)xn_flat_half_h, &b);
    corner(&q[1], flat, b.x, b.y, b.z);
    corner(&q[3], flat, -b.x, 0, 0);    /* the base: at the flat's position, x only */
    corner(&q[2], flat, -a.x, 0, 0);
}

void xn_flat_quad_standing_r(xn_regs *r)
{
    struct xn_poly_vertex *last = &xn_poly_vertex_buf_a[2];

    xn_flat_quad_standing(r->eax, r->edx, (const struct xn_flat *)r->edi);
    r->eax = last->x;
    r->edx = last->y;
    r->ebx = last->z;
    r->ecx = last->outcode;
    r->esi = (u32)xn_poly_vertex_buf_a;
    r->ebp = -(s32)xn_flat_half_h << 4;
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

void xn_flat_setup_gradients(s32 top_y)
{
    struct xn_flat *flat = xn_flat_emit_flat;          /* the patch field, read as data */
    s32 sx = xn_pick_view_x * xn_flat_grad_sx;          /* the flat's position on the screen */
    s32 sy = xn_pick_view_y * xn_flat_grad_sy;
    s32 k = xn_udiv64(2, 0, *(u32 *)&flat->scale & 0xFFFF);    /* 2^33 / scale */
    s32 dy;

    flat->du_dx = xn_mulhi(xn_flat_axis_u_x, k);
    flat->u_dx = flat->du_dx >> 3;
    flat->u_dy = xn_mulhi(xn_flat_axis_u_y, k);
    flat->u_c = xn_mulhi(xn_flat_axis_u_z, k);
    flat->dv_dx = xn_mulhi(xn_flat_axis_v_x, k);
    flat->u_offset = (sx * flat->u_dx + sy * flat->u_dy + pick_distance * flat->u_c) >> 1;
    flat->v_dx = flat->dv_dx >> 3;
    flat->v_dy = xn_mulhi(xn_flat_axis_v_y, k);
    flat->v_c = xn_mulhi(xn_flat_axis_v_z, k);
    flat->v_offset = sx * flat->v_dx + sy * flat->v_dy + pick_distance * flat->v_c;
    /* the constants at the top row; the walker steps them per row */
    dy = top_y - xn_cam_centre_y;
    xn_flat_row_u_step = flat->u_dy;
    flat->u_c += flat->u_dy * dy;
    xn_flat_row_u_var = &flat->u_c;
    xn_flat_row_v_step = flat->v_dy - 8;
    flat->v_c += xn_flat_row_v_step * dy;
    xn_flat_row_v_var = &flat->v_c;
    if (flat->flags & 0x20) {           /* mirrored */
        flat->du_dx = -flat->du_dx;
        flat->u_dx = -flat->u_dx;
        flat->u_offset = -flat->u_offset - (xn_flat_tex_w << 23);
    }
}

/* 1/z of the span `node` at x (from its start, along its polygon's gradient) */
static s32 inv_z_at(const struct xn_span *node, s32 x)
{
    return (x - node->x_start) * node->poly->inv_z_dx + node->inv_z;
}

void xn_flat_span_clip(struct xn_span *node, s32 x0, s32 x1, s32 inv_z)
{
    for (;;) {
        node = node->next;
        if ((s16)node->x_end <= (s16)x0)
            continue;                   /* ends before the piece */
        if ((s16)x1 <= (s16)node->x_start) {
            xn_flat_span_emit(x0, x1, inv_z);   /* starts after it: the rest is visible */
            return;
        }
        if ((s16)x1 < (s16)node->x_end) {
            /* the piece ends over this span: draw what is in front of it */
            if (node->x_start == x0) {
                if (inv_z > node->inv_z)
                    xn_flat_span_emit(x0, x1, inv_z);
            } else if (node->x_start < x0) {
                if (inv_z_at(node, x0) < inv_z)
                    xn_flat_span_emit(x0, x1, inv_z);
            } else if (inv_z > node->inv_z) {
                xn_flat_span_emit(x0, x1, inv_z);
            } else {
                xn_flat_span_emit(x0, (x1 & 0xFFFF0000) | node->x_start, inv_z);
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
            xn_flat_span_emit(x0, (x1 & 0xFFFF0000) | node->x_start, inv_z);
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

void xn_flat_span_clip_r(xn_regs *r)
{
    xn_flat_span_clip((struct xn_span *)r->esi, r->ebx, r->ebp, r->ecx);
}

void xn_flat_span_emit(s32 x0, s32 x1, s32 inv_z)
{
    struct xn_flat *flat = xn_flat_emit_flat;
    xn_regs r;

    r.ecx = inv_z;
    r.ebx = x0;
    r.ebp = x1 - x0;
    r.esi = (u32)flat;
    r.edi = (u32)(xn_flat_emit_row + x0);
    r.eax = r.edx = 0;
    xn_asmcall(flat->span, &r);         /* the first piece: the light setup, which draws nothing */
}

void xn_flat_span_emit_r(xn_regs *r)
{
    xn_flat_span_emit(r->ebx, r->ebp, r->ecx);
}

/* The walk from a row: each row's piece from the right edge's x to the left edge's x - 1
   (the flat's ring runs the other way round), cut against the world; then the flat's u/v
   constants, the row address and the edges step */
static void flat_walk(struct xn_span *row, u32 rows, s32 xl, s32 xr, s32 zl)
{
    for (;;) {
        s32 x0 = (u32)xr >> 21;
        s32 x1 = ((u32)xl >> 21) - 1;

        if (x1 > x0)
            xn_flat_span_clip(row, x0, x1, zl);
        row++;
        *xn_flat_row_u_var += xn_flat_row_u_step;
        *xn_flat_row_v_var += xn_flat_row_v_step;
        xn_flat_emit_row += xn_flat_row_stride;
        rows -= 0x10001;
        xl += xn_flat_raster_dxl;
        xr += xn_flat_raster_dxr;
        zl += xn_flat_raster_dzl;
        if ((rows & 0xFFFF) == 0 && !xn_walk_left_edge(&flat_walker, &rows, &xl, &zl))
            return;
        if ((rows & 0xFFFF0000) == 0 && !xn_walk_right_edge(&flat_walker, &rows, &xr))
            return;
    }
}

void xn_flat_raster(s32 top_y, struct xn_poly_vertex **ring)
{
    u32 rows = 0;
    s32 xl, xr, zl;

    xn_flat_raster_left = ring;
    xn_flat_raster_right = ring;
    xn_flat_setup_gradients(top_y);
    xn_flat_emit_row = screen_buffer + xn_gfx_row_offset[top_y] - 1;
    if (!xn_walk_left_edge(&flat_walker, &rows, &xl, &zl))
        return;
    if (!xn_walk_right_edge(&flat_walker, &rows, &xr))
        return;
    flat_walk(&xn_render_span_rows[top_y], rows, xl, xr, zl);
}

void xn_flat_raster_r(xn_regs *r)
{
    xn_flat_raster(r->eax, (struct xn_poly_vertex **)r->ebp);
}

void xn_flat_raster_rows_r(xn_regs *r)
{
    flat_walk((struct xn_span *)r->esi, r->edi, r->ebp, r->ebx, r->ecx);
}

void xn_flat_rotate_offset(s32 ox, s32 oy, xn_vec3 *out)
{
    const s32 *m = &xn_flat_matrix.m[0][0];

    ox <<= 4;
    oy <<= 4;
    out->x = xn_mulhi(ox, m[0]) + xn_mulhi(oy, m[1]);
    out->y = xn_mulhi(ox, m[3]) + xn_mulhi(oy, m[4]);
    out->z = xn_mulhi(oy, m[7]);
}

void xn_flat_rotate_offset_r(xn_regs *r)
{
    xn_vec3 v;

    xn_flat_rotate_offset(r->ecx, r->ebp, &v);
    r->eax = v.x;
    r->edx = v.y;
    r->ebx = v.z;
    r->ebp <<= 4;
}

/* the flat axis `a` (2^30 on one axis) through the projection matrix, y and z / 8 rounded */
static void flat_axis(s32 ax, s32 ay, s32 *x, s32 *y, s32 *z)
{
    xn_vec3 v;

    v.x = ax;
    v.y = ay;
    v.z = 0;
    xn_mat_transform_wide(&v, &xn_flat_proj_matrix);
    *x = v.x;
    *y = (v.y + 4) >> 3;
    *z = (v.z + 4) >> 3;
}

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
    xn_flat_row_stride = xn_gfx_width;
    flat_axis(0x40000000, 0, &xn_flat_axis_u_x, &xn_flat_axis_u_y, &xn_flat_axis_u_z);
    flat_axis(0, 0x40000000, &xn_flat_axis_v_x, &xn_flat_axis_v_y, &xn_flat_axis_v_z);
}

void xn_flat_begin_frame_r(xn_regs *r)
{
    xn_flat_begin_frame();
    r->esi = (u32)&xn_flat_matrix;
}

void *xn_flat_pick(s32 x, s32 y, s32 frame)
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
        struct xn_tex_image *image;
        u32 scale;

        image = xn_tex_cache_lookup_image(flat->image >> 7, flat->image & 0x7F, frame);
        scale = *(u32 *)&flat->scale & 0xFFFF;
        /* the quad builder's EBX is the next lookup's frame (an asm leftover) */
        frame = build_quad(flat, image->width * scale, image->height * scale);
        if (xn_poly_clip_outcode_and == 0) {
            const struct xn_poly_vertex *q = xn_poly_vertex_buf_a;
            s32 z = q[0].z;
            s32 px = xn_muldiv(xn_scratch_vec_b.x - xn_cam_centre_x, z, xn_cam_focal_x);

            xn_pick_flat_x = px;
            if (px > q[0].x && px < q[1].x) {
                s32 py = xn_muldiv(xn_scratch_vec_b.y - xn_cam_centre_y, z, xn_cam_focal_y);

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

void xn_flat_span_light_setup(struct xn_flat *flat)
{
    s32 row;
    const struct xn_light *light = xn_light_table;
    s32 n = xn_light_count;
    s32 fog;

    if (flat->table != 0) {
        flat->span = asm_xn_span_flat_translucent;
        return;
    }
    row = ((*(u32 *)&flat->scale >> 8) & 0xFF00) + xn_flat_ambient_row;
    if (row < xn_shade_table_last_row && n != 0) {
        if (n > 32)
            n = 32;
        do {
            if (light->type == 8) {
                /* directional: its intensity in whole rows, again and again (the asm jumps
                   back to the same light) until the last row */
                do
                    row += light->intensity << 8;
                while (row < xn_shade_table_last_row);
                break;
            } else {
                s32 dx = (light->x - flat->view_x) >> 8;
                s32 dy = (light->y - flat->view_y) >> 8;
                s32 dz = (light->z - flat->view_z) >> 8;
                s32 d2 = dx * dx + dy * dy + dz * dz;

                if (d2 < light->range_sq) {
                    xn_s64 t;

                    xn_s64_mul(&t, xn_math_isqrt_lookup(d2) << 16, light->intensity);
                    row += xn_s64_div(&t, d2);
                    if (row >= xn_shade_table_last_row)
                        break;
                }
            }
            light = (const struct xn_light *)((const u8 *)light + 29);
        } while (--n != 0);
    }
    fog = (s32)((u32)flat->view_z >> 8) - xn_fog_start;
    if (row < xn_shade_table_last_row) {
        flat->shade_row = (u8 *)row;
        flat->span = asm_xn_span_flat_transparent_shaded;
        if ((s32)((u32)flat->view_z >> 8) > xn_fog_start) {
            flat->span = asm_xn_span_flat_lit_fogged;
            flat->table = (u8 *)(xn_fog_table_last - fog * xn_fog_step);
        }
    } else {                            /* at the last row: unlit */
        flat->span = asm_xn_span_flat_transparent;
        if ((s32)((u32)flat->view_z >> 8) > xn_fog_start) {
            flat->span = asm_xn_span_flat_transparent_shaded;
            flat->shade_row = (u8 *)(xn_fog_table_last - fog * xn_fog_step);
        }
    }
}
