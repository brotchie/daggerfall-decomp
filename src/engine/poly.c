/* poly.c: XnGine's polygons (canonical C; the interface and the module's documentation are in
   xpoly.h). */
#include "xpoly.h"
#include "xcam.h"
#include "xflat.h"
#include "xspan.h"
#include "xlight.h"
#include "xrender.h"

extern struct xn_vert_cam xn_vert_cam[1024];        /* indexed by byte offset (index * 12) */
extern struct xn_vert_screen xn_vert_screen[1024];
extern struct xn_vert_flags xn_vert_flags[1024];
extern u32 xn_recip16_table[1024];      /* 0FFFFh / n */
extern u32 xn_recip32_table[1024];      /* 0FFFFFFFFh / n */

/* ---- outcodes --------------------------------------------------------------------------- */

u32 xn_poly_outcode(s32 x, s32 y, s32 z)
{
    u32 code = 0;

    if (z < xn_cam_near_z)
        code |= 0x10;
    if (z > xn_cam_far_z)
        code |= 0x20;
    if (x > z)
        code |= 2;
    if (y > z)
        code |= 4;
    if (x < -z)
        code |= 1;
    if (y < -z)
        code |= 8;
    return code;
}

u32 xn_poly_outcode_eax(s32 x, s32 y, s32 z)
{
    return xn_poly_outcode(x, y, z);
}

/* ---- the clipper ------------------------------------------------------------------------ */

/* the cut's coordinate: from a toward b by hi((b - a) * 2 * t), 32-bit differences */
static s32 lerp(s32 a, s32 b, s32 t)
{
    return xn_mulhi((s32)((u32)(b - a) << 1), t) + a;
}

/* the edge's parameter num / den as a 32-bit fraction: (num << 32) / (2 * den), 0 where the
   divide would overflow */
static s32 cut_at(s32 num, s32 den)
{
    return xn_idiv64_or0(num, 0, (s32)((u32)den << 1));
}

/* a new vertex's outcode, stored and merged into the clipper's OR and AND */
static void finish(struct xn_poly_vertex *v)
{
    u8 code = (u8)xn_poly_outcode(v->x, v->y, v->z);

    v->outcode = code;
    xn_poly_clip_outcode_or |= code;
    xn_poly_clip_outcode_and &= code;
}

void xn_poly_clip_intersect_left(const struct xn_poly_vertex *in,
                                 const struct xn_poly_vertex *out, struct xn_poly_vertex *dst)
{
    s32 t = cut_at(in->x + in->z, (in->x + in->z) - out->x - out->z);

    dst->x = lerp(in->x, out->x, t);
    dst->z = -dst->x;
    dst->y = lerp(in->y, out->y, t);
    finish(dst);
}

void xn_poly_clip_intersect_right(const struct xn_poly_vertex *in,
                                  const struct xn_poly_vertex *out, struct xn_poly_vertex *dst)
{
    s32 t = cut_at(in->x - in->z, (out->z - out->x) + (in->x - in->z));

    dst->x = lerp(in->x, out->x, t);
    dst->z = dst->x;
    dst->y = lerp(in->y, out->y, t);
    finish(dst);
}

void xn_poly_clip_intersect_bottom(const struct xn_poly_vertex *in,
                                   const struct xn_poly_vertex *out, struct xn_poly_vertex *dst)
{
    s32 t = cut_at(in->z - in->y, out->y - in->y - out->z + in->z);

    dst->x = lerp(in->x, out->x, t);
    dst->y = lerp(in->y, out->y, t);
    dst->z = dst->y;
    finish(dst);
}

void xn_poly_clip_intersect_top(const struct xn_poly_vertex *in,
                                const struct xn_poly_vertex *out, struct xn_poly_vertex *dst)
{
    s32 t = cut_at(in->y + in->z, (in->y + in->z) - out->y - out->z);

    dst->x = lerp(in->x, out->x, t);
    dst->y = lerp(in->y, out->y, t);
    dst->z = -dst->y;
    finish(dst);
}

void xn_poly_clip_intersect_near(const struct xn_poly_vertex *in,
                                 const struct xn_poly_vertex *out, struct xn_poly_vertex *dst)
{
    s32 t = cut_at(in->z - xn_cam_near_z, in->z - out->z);

    dst->x = lerp(in->x, out->x, t);
    dst->y = lerp(in->y, out->y, t);
    dst->z = xn_cam_near_z;
    finish(dst);
}

void xn_poly_clip_intersect_far(const struct xn_poly_vertex *in,
                                const struct xn_poly_vertex *out, struct xn_poly_vertex *dst)
{
    s32 t = cut_at(in->z - xn_cam_far_z, in->z - out->z);

    dst->x = lerp(in->x, out->x, t);
    dst->y = lerp(in->y, out->y, t);
    dst->z = xn_cam_far_z;
    finish(dst);
}

void xn_poly_clip_plane(u32 plane, xn_clip_cut_fn cut)
{
    struct xn_poly_vertex *v, *end, *dst, *t;

    xn_poly_clip_outcode_and = 0x7F;
    xn_poly_clip_outcode_or = 0;
    /* close the ring: the first two vertices again after the last */
    end = xn_poly_clip_src_end;
    end[0] = xn_poly_clip_src[0];
    end[1] = xn_poly_clip_src[1];
    end = ++xn_poly_clip_src_end;
    dst = xn_poly_clip_dst;
    for (v = xn_poly_clip_src + 1; v != end; v++) {
        if (v->outcode & plane) {       /* outside: the cuts of its edges to inside neighbours */
            if (!(v[-1].outcode & plane))
                cut(&v[-1], v, dst++);
            if (!(v[1].outcode & plane))
                cut(&v[1], v, dst++);
        } else {
            *dst++ = *v;
            xn_poly_clip_outcode_or |= v->outcode;
            xn_poly_clip_outcode_and &= v->outcode;
        }
    }
    xn_poly_clip_src_end = dst;
    t = xn_poly_clip_src;
    xn_poly_clip_src = xn_poly_clip_dst;
    xn_poly_clip_dst = t;
}

/* One plane of the frustum: 1 when the polygon is now inside every plane, 0 when it is
   outside one, -1 to go on. keep: the outcode bits that stay (those of the planes done go) */
static int clip_against(u32 plane, xn_clip_cut_fn cut, u8 keep)
{
    xn_poly_clip_plane(plane, cut);
    xn_poly_clip_outcode_or &= keep;
    if (!(xn_poly_clip_outcode_or & 0x7F))
        return 1;
    if (xn_poly_clip_outcode_and & 0x7F)
        return 0;
    return -1;
}

int xn_poly_clip_frustum(void)
{
    int k = -1;

    if (xn_poly_clip_outcode_or & 0x10)
        k = clip_against(0x10, xn_poly_clip_intersect_near, 0xFF);
    if (k < 0 && (xn_poly_clip_outcode_or & 0x20))
        k = clip_against(0x20, xn_poly_clip_intersect_far, 0xFF);
    if (k < 0 && (xn_poly_clip_outcode_or & 1))
        k = clip_against(1, xn_poly_clip_intersect_left, 0xFE);
    if (k < 0 && (xn_poly_clip_outcode_or & 4))
        k = clip_against(4, xn_poly_clip_intersect_bottom, 0xFA);
    if (k < 0 && (xn_poly_clip_outcode_or & 2))
        k = clip_against(2, xn_poly_clip_intersect_right, 0xF8);
    if (k < 0 && (xn_poly_clip_outcode_or & 8))
        k = clip_against(8, xn_poly_clip_intersect_top, 0xF0);
    return k != 0;
}

/* ---- the edge walker -------------------------------------------------------------------- */

int xn_walk_left_edge(xn_edge_walk *w)
{
    for (;;) {
        struct xn_poly_vertex *v, *next;
        u16 dy;

        if (--w->count < 0)             /* Quirk Q-POLY-02: a signed byte */
            return 0;
        v = w->left[0];
        next = w->left[1];
        w->left++;
        dy = (u16)(next->y - v->y);             /* the rows: 16-bit */
        w->rows = (w->rows & 0xFFFF0000) | dy;
        if ((s16)next->y <= (s16)v->y)
            continue;
        w->dxl = (next->x - v->x) * (s32)xn_recip16_table[dy];
        w->dzl = xn_mulhi(next->z - v->z, xn_recip32_table[dy]);
        w->xl = v->x << 16;
        w->zl = v->z;
        return 1;
    }
}

int xn_walk_right_edge(xn_edge_walk *w)
{
    for (;;) {
        struct xn_poly_vertex *v, *prev;
        s32 dy;

        if (--w->count < 0)
            return 0;
        v = w->right[0];
        prev = w->right[-1];
        w->right--;
        dy = prev->y - v->y;
        if (dy <= 0)
            continue;
        w->dxr = (prev->x - v->x) * (s32)xn_recip16_table[dy];
        w->rows |= (u32)dy << 16;
        w->xr = v->x << 16;
        return 1;
    }
}

void xn_poly_rasterize_rows(struct xn_span *row, xn_edge_walk *w)
{
    for (;;) {
        s32 left = (u32)w->xl >> 21;
        s32 right = (u32)w->xr >> 21;

        if (right > left)
            xn_span_insert(row, left, right, w->zl);
        row++;
        w->rows -= 0x10001;
        w->xl += w->dxl;
        w->xr += w->dxr;
        w->zl += w->dzl;
        /* a new edge where one side's rows ran out (the left first when both did) */
        if ((w->rows & 0xFFFF) == 0 && !xn_walk_left_edge(w))
            return;
        if ((w->rows & 0xFFFF0000) == 0 && !xn_walk_right_edge(w))
            return;
    }
}

void xn_poly_rasterize(s32 top_y, struct xn_poly_vertex **ring, int n)
{
    xn_edge_walk w;

    w.left = ring;
    w.right = ring;
    w.count = (s8)n;
    w.rows = 0;
    if (!xn_walk_left_edge(&w) || !xn_walk_right_edge(&w))
        return;
    xn_poly_rasterize_rows(&xn_render_span_rows[top_y], &w);
}

/* ---- the projectors ----------------------------------------------------------------------- */

/* Projects the clipped polygon in place and finds its top vertex: the first of the least rows
   (first) or the last of them; *top_y gets that row (from `least`: 7FFFh or 7FFFFFFFh, as
   the asm starts). Returns the vertex count. */
static int project_clipped(s32 least, int first, s32 *top_y, int *top)
{
    struct xn_poly_vertex *v;
    int n = 0;

    *top_y = least;
    *top = 0;
    for (v = xn_poly_clip_src; v != xn_poly_clip_src_end; v++, n++) {
        u32 inv_z;

        xn_cam_screen_point(v->x, v->y, v->z, &v->x, &v->y, &inv_z);
        v->z = inv_z;
        if (first ? v->y < *top_y : v->y <= *top_y) {
            *top_y = v->y;
            *top = n;
        }
    }
    return n;
}

/* The ring of n vertices at vertex `top`, for the buffer the clipper ended in */
static struct xn_poly_vertex **ring_at(int n, int top)
{
    if (xn_poly_clip_src == xn_poly_vertex_buf_a)
        return xn_poly_ring_a[n] + top;
    return xn_poly_ring_b[n] + top;
}

void xn_poly_project_flat(struct xn_flat_walk *w, struct xn_poly_vertex *quad)
{
    s32 top_y;
    int n, top;

    xn_poly_clip_src = quad;
    xn_poly_clip_src_end = quad + 4;
    if (xn_poly_clip_outcode_or) {
        xn_poly_clip_dst = quad + 32;
        if (!xn_poly_clip_frustum())
            return;
    }
    n = project_clipped(0x7FFF, 1, &top_y, &top);
    xn_flat_raster(w, top_y, ring_at(n, top), n);
}

/* The face's vertices inside the view, from their projections (Quirk Q-POLY-01: the asm
   counts the vertices in CL and keeps the ring's byte offset of the vertex in CH, both bytes
   that wrap: a face of 1 point takes 256 more, the ring is that of n mod 64) */
static void walk_projected(const struct xn_model_face *face)
{
    const struct xn_model_face_point *p = face->points;
    u8 n = face->point_count;
    struct xn_poly_vertex *v = xn_poly_vertex_buf_a;
    const struct xn_vert_screen *s = &XN_AT(struct xn_vert_screen, xn_vert_screen, p->vertex);
    u32 c = n + 0x3FF;                  /* the asm's `add ecx, 3FFh` */
    u8 left = (u8)c;                    /* the vertices after the first: n - 1 */
    u8 at = (u8)(c >> 8);               /* the ring's byte offset of the next: 4 */
    u8 top_at = 0;
    s32 top_y;

    v->x = s->sx;
    v->z = s->inv_z;
    v->y = s->sy;
    top_y = v->y;
    do {
        p++;
        v++;
        s = &XN_AT(struct xn_vert_screen, xn_vert_screen, p->vertex);
        v->x = s->sx;
        v->z = s->inv_z;
        v->y = s->sy;
        if (v->y <= top_y) {            /* the last of the top vertices */
            top_y = v->y;
            top_at = at;
        }
        at += 4;
    } while (--left != 0);
    xn_poly_rasterize(top_y, (struct xn_poly_vertex **)((u8 *)xn_poly_ring_a[at >> 2] + top_at),
                      n);
}

int xn_poly_project_face(const struct xn_model_face *face, u32 codes)
{
    const struct xn_model_face_point *p = face->points;
    struct xn_span *spans = xn_render_span_next;
    struct xn_poly_vertex *v;
    u8 k = face->point_count;
    s32 top_y;
    int n, top;

    if ((codes & 0xFF00) == 0) {
        walk_projected(face);
        return (u32)spans < (u32)xn_render_span_next;
    }
    xn_poly_clip_outcode_and = (u8)codes;
    xn_poly_clip_outcode_or = (u8)(codes >> 8);
    xn_poly_clip_src = xn_poly_vertex_buf_a;
    xn_poly_clip_dst = xn_poly_vertex_buf_b;
    v = xn_poly_vertex_buf_a;
    do {                                /* the camera-space vertices and their outcodes */
        const struct xn_vert_cam *c = &XN_AT(struct xn_vert_cam, xn_vert_cam, p->vertex);

        v->outcode = XN_AT(struct xn_vert_flags, xn_vert_flags, p->vertex).outcode;
        v->x = c->x;
        v->y = c->y;
        v->z = c->z;
        v++;
        p++;
    } while (--k != 0);
    xn_poly_clip_src_end = v;
    if (!xn_poly_clip_frustum())
        return 0;
    n = project_clipped(0x7FFFFFFF, 0, &top_y, &top);
    if ((u8)n >= 0x1C)                  /* Quirk Q-POLY-03 */
        return 0;
    xn_poly_rasterize(top_y, ring_at(n, top), n);
    return (u32)spans < (u32)xn_render_span_next;
}

void xn_poly_project_terrain(int n)
{
    s32 top_y;
    int top;

    xn_poly_clip_src = xn_poly_vertex_buf_a;
    xn_poly_clip_dst = xn_poly_vertex_buf_b;
    xn_poly_clip_src_end = xn_poly_vertex_buf_a + n;
    if (!xn_poly_clip_frustum())
        return;
    n = project_clipped(0x7FFFFFFF, 0, &top_y, &top);
    xn_poly_rasterize(top_y, ring_at(n, top), n);
}

/* ---- the textured model face --------------------------------------------------------------- */

/* hi(a.x * m0) + hi(a.y * m1) + hi(a.z * m2): three high dwords added, not a 64-bit sum */
static s32 row_dot(const s32 *m, const xn_vec3 *a)
{
    return xn_mulhi(a->x, m[0]) + xn_mulhi(a->y, m[1]) + xn_mulhi(a->z, m[2]);
}

void xn_poly_tex_gradients(const xn_mat3 *m, const struct xn_model_face_data *axes,
                           struct xn_poly *poly)
{
    poly->u_step = row_dot(m->m[0], &axes->u_axis);
    poly->u_dx = poly->u_step >> 3;
    poly->u_dy = row_dot(m->m[1], &axes->u_axis);
    poly->u_c = row_dot(m->m[2], &axes->u_axis);
    poly->v_step = row_dot(m->m[0], &axes->v_axis);
    poly->v_dx = poly->v_step >> 3;
    poly->v_dy = row_dot(m->m[1], &axes->v_axis);
    poly->v_c = row_dot(m->m[2], &axes->v_axis);
}

/* a * sx + b * sy + c * z with 64-bit products and sum */
static void sum3(xn_s64 *t, s32 a, s32 sx, s32 b, s32 sy, s32 c, s32 z)
{
    xn_s64_mul(t, a, sx);
    xn_s64_mac(t, b, sy);
    xn_s64_mac(t, c, z);
}

void xn_poly_setup_textured(struct xn_poly *poly, const struct xn_span *span, s32 xs, s32 n,
                            u8 *pix)
{
    struct xn_tex_image *image = poly->tex->current;
    s32 kind, sx, sy;
    u32 d, u, v;
    xn_s64 t;

    poly->wrap_mask = image->wrap_mask;
    poly->texels = (u8 *)image + image->data_offset;
    kind = xn_light_setup_poly(poly);
    /* the 16-pixel routines when 1/z changes slowly across the screen (abs as `neg`: the
       least s32 stays itself; the shift unsigned) */
    d = poly->inv_z_dx;
    if ((s32)d < 0)
        d = 0u - d;
    if ((d >> (xn_gfx_width == 320 ? 18 : 25)) == 0)
        kind += 12;
    poly->span_fn = xn_render_tmap_span_fns[kind / 4];
    xn_poly_tex_gradients((const xn_mat3 *)poly->handle->matrix, poly->face->points[2].data,
                          poly);
    /* the gradients at the face's first vertex: the texture origin, u in 6.26, v in 22.10 */
    sx = poly->cam_x * xn_cam_half_width;
    sy = poly->cam_y * xn_cam_half_height;
    sum3(&t, poly->u_dx, sx, poly->u_dy, sy, poly->u_c, poly->cam_z);
    u = xn_s64_shr(&t, 26);
    sum3(&t, poly->v_dx, sx, poly->v_dy, sy, poly->v_c, poly->cam_z);
    v = xn_s64_shr(&t, 10);
    /* the face's packed u/v less (v's high word | u's low word) */
    *(u32 *)&poly->tex_u0 = poly->face->points[0].uv_packed - ((v & 0xFFFF0000) | (u & 0xFFFF));
    poly->span_fn(poly, span, xs, n, pix);
}

/* ---- stubs ------------------------------------------------------------------------------- */

int xn_poly_clc_stub(void)
{
    return 0;
}

void xn_poly_ret_stub(void)
{
}
