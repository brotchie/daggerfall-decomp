/* poly.c: XnGine's polygon functions as readable C (xpoly.h; see xngine.h and
   docs/xngine_readable.md). */
#include "xpoly.h"
#include "xflat.h"

extern struct xn_vert_cam xn_vert_cam[1024];        /* indexed by byte offset (index * 12) */
extern struct xn_vert_screen xn_vert_screen[1024];
extern struct xn_vert_flags xn_vert_flags[1024];
extern struct xn_poly_vertex **xn_poly_ring_a[], **xn_poly_ring_b[];  /* rings by count */
extern u32 xn_recip16_table[1024];      /* 0xFFFF / n */
extern u32 xn_recip32_table[1024];      /* 0xFFFFFFFF / n */
extern struct xn_span *xn_render_span_next;
extern xn_routine xn_render_tmap_span_fns[6];   /* 8-px by kind 0/4/8, then the 16-px ones */

/* patch fields: the projectors' view constants (12A2D0) */
extern s32 xn_poly_flat_sx, xn_poly_flat_cx, xn_poly_flat_sy, xn_poly_flat_cy;
extern s32 xn_poly_face_sx, xn_poly_face_cx, xn_poly_face_sy, xn_poly_face_cy;
extern s32 xn_poly_terrain_sx, xn_poly_terrain_cx, xn_poly_terrain_sy, xn_poly_terrain_cy;
/* the edge walker's state, in its own code */
extern struct xn_poly_vertex **xn_raster_left, **xn_raster_right;
extern s32 xn_raster_dxl, xn_raster_dzl, xn_raster_dxr;
/* the gradients' saved axis x components (the asm reloads them from its code) */
extern s32 xn_grad_u0_a, xn_grad_u0_b, xn_grad_v0_a, xn_grad_v0_b;
/* the textured setup's constants */
extern u8 xn_poly_subdiv_shift;                 /* 18 at 320 wide, else 25 (12A4F0) */
extern s32 xn_poly_setup_sx, xn_poly_setup_sy;  /* the half width and height (12A2D0) */

/* other groups' routines, through their asm entries */
extern void asm_xn_span_insert(void);
extern void asm_xn_light_setup_poly(void);
/* the intersect routines' asm entries: what xn_poly_clip_intersect_fn holds */
extern void asm_xn_poly_clip_intersect_left(void);
extern void asm_xn_poly_clip_intersect_right(void);
extern void asm_xn_poly_clip_intersect_bottom(void);
extern void asm_xn_poly_clip_intersect_top(void);
extern void asm_xn_poly_clip_intersect_near(void);
extern void asm_xn_poly_clip_intersect_far(void);

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

void xn_poly_outcode_r(xn_regs *r)
{
    r->ecx = xn_poly_outcode(r->eax, r->edx, r->ebx);
}

u32 xn_poly_outcode_eax(s32 x, s32 y, s32 z)
{
    return xn_poly_outcode(x, y, z);
}

void xn_poly_outcode_eax_r(xn_regs *r)
{
    r->eax = xn_poly_outcode(r->eax, r->edx, r->ebx);
}

/* ---- the clipper ------------------------------------------------------------------------ */

/* the cut's coordinate: from a toward b by (b - a) * 2 * t / 2^32 */
static s32 lerp(s32 a, s32 b, s32 t)
{
    return xn_mulhi((b - a) * 2, t) + a;
}

/* a new vertex's outcode, kept and merged into the clipper's OR and AND */
static void finish(struct xn_poly_vertex *v)
{
    u8 code = (u8)xn_poly_outcode(v->x, v->y, v->z);

    v->outcode = code;
    xn_poly_clip_outcode_or |= code;
    xn_poly_clip_outcode_and &= code;
}

/* the cut parameter num / den as a 32-bit fraction: (num << 32) / (2 * den), 64-bit idiv */
static s32 cut(s32 num, s32 den)
{
    return xn_sdiv64(num, 0, den * 2);
}

void xn_poly_clip_intersect_left(const struct xn_poly_vertex *in,
                                 const struct xn_poly_vertex *out, struct xn_poly_vertex *dst)
{
    s32 t = cut(in->x + in->z, (in->x + in->z) - out->x - out->z);

    dst->x = lerp(in->x, out->x, t);
    dst->z = -dst->x;
    dst->y = lerp(in->y, out->y, t);
    finish(dst);
}

void xn_poly_clip_intersect_right(const struct xn_poly_vertex *in,
                                  const struct xn_poly_vertex *out, struct xn_poly_vertex *dst)
{
    s32 t = cut(in->x - in->z, (out->z - out->x) + (in->x - in->z));

    dst->x = lerp(in->x, out->x, t);
    dst->z = dst->x;
    dst->y = lerp(in->y, out->y, t);
    finish(dst);
}

void xn_poly_clip_intersect_bottom(const struct xn_poly_vertex *in,
                                   const struct xn_poly_vertex *out, struct xn_poly_vertex *dst)
{
    s32 t = cut(in->z - in->y, out->y - in->y - out->z + in->z);

    dst->x = lerp(in->x, out->x, t);
    dst->y = lerp(in->y, out->y, t);
    dst->z = dst->y;
    finish(dst);
}

void xn_poly_clip_intersect_top(const struct xn_poly_vertex *in,
                                const struct xn_poly_vertex *out, struct xn_poly_vertex *dst)
{
    s32 t = cut(in->y + in->z, (in->y + in->z) - out->y - out->z);

    dst->x = lerp(in->x, out->x, t);
    dst->y = lerp(in->y, out->y, t);
    dst->z = -dst->y;
    finish(dst);
}

void xn_poly_clip_intersect_near(const struct xn_poly_vertex *in,
                                 const struct xn_poly_vertex *out, struct xn_poly_vertex *dst)
{
    s32 t = cut(in->z - xn_cam_near_z, in->z - out->z);

    dst->x = lerp(in->x, out->x, t);
    dst->y = lerp(in->y, out->y, t);
    dst->z = xn_cam_near_z;
    finish(dst);
}

void xn_poly_clip_intersect_far(const struct xn_poly_vertex *in,
                                const struct xn_poly_vertex *out, struct xn_poly_vertex *dst)
{
    s32 t = cut(in->z - xn_cam_far_z, in->z - out->z);

    dst->x = lerp(in->x, out->x, t);
    dst->y = lerp(in->y, out->y, t);
    dst->z = xn_cam_far_z;
    finish(dst);
}

/* the asm interface of the six: the new vertex in eax, edx, ebx and its outcode in ecx; edi past
   it (ZF from that add) */
static void intersect_regs(void (*fn)(const struct xn_poly_vertex *,
                                      const struct xn_poly_vertex *, struct xn_poly_vertex *),
                           xn_regs *r)
{
    struct xn_poly_vertex *dst = (struct xn_poly_vertex *)r->edi;

    fn((const struct xn_poly_vertex *)r->ebx, (const struct xn_poly_vertex *)r->esi, dst);
    r->eax = dst->x;
    r->edx = dst->y;
    r->ebx = dst->z;
    r->ecx = dst->outcode;
    r->edi += sizeof(struct xn_poly_vertex);
    XN_SETFLAG(r, XN_ZF, r->edi == 0);
}

void xn_poly_clip_intersect_left_r(xn_regs *r)
{
    intersect_regs(xn_poly_clip_intersect_left, r);
}

void xn_poly_clip_intersect_right_r(xn_regs *r)
{
    intersect_regs(xn_poly_clip_intersect_right, r);
}

void xn_poly_clip_intersect_bottom_r(xn_regs *r)
{
    intersect_regs(xn_poly_clip_intersect_bottom, r);
}

void xn_poly_clip_intersect_top_r(xn_regs *r)
{
    intersect_regs(xn_poly_clip_intersect_top, r);
}

void xn_poly_clip_intersect_near_r(xn_regs *r)
{
    intersect_regs(xn_poly_clip_intersect_near, r);
}

void xn_poly_clip_intersect_far_r(xn_regs *r)
{
    intersect_regs(xn_poly_clip_intersect_far, r);
}

/* Cuts the edge in -> out with the plane of the routine in xn_poly_clip_intersect_fn (an asm
   entry: the C of the six, or the asm itself for anything else) */
static void intersect(const struct xn_poly_vertex *in, const struct xn_poly_vertex *out,
                      struct xn_poly_vertex *dst)
{
    xn_routine fn = xn_poly_clip_intersect_fn;
    xn_regs r;

    if (fn == asm_xn_poly_clip_intersect_left)
        xn_poly_clip_intersect_left(in, out, dst);
    else if (fn == asm_xn_poly_clip_intersect_right)
        xn_poly_clip_intersect_right(in, out, dst);
    else if (fn == asm_xn_poly_clip_intersect_bottom)
        xn_poly_clip_intersect_bottom(in, out, dst);
    else if (fn == asm_xn_poly_clip_intersect_top)
        xn_poly_clip_intersect_top(in, out, dst);
    else if (fn == asm_xn_poly_clip_intersect_near)
        xn_poly_clip_intersect_near(in, out, dst);
    else if (fn == asm_xn_poly_clip_intersect_far)
        xn_poly_clip_intersect_far(in, out, dst);
    else {
        r.ebx = (u32)in;
        r.esi = (u32)out;
        r.edi = (u32)dst;
        r.eax = r.ecx = r.edx = r.ebp = 0;
        xn_asmcall(fn, &r);
    }
}

void xn_poly_clip_plane(u32 plane)
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
        /* (the asm tests the plane against the outcode byte with the upper bits of EAX; the
           plane bits are all below 100h) */
        if (v->outcode & plane) {       /* outside: the cuts of its edges to inside neighbours */
            if (!(v[-1].outcode & (u8)plane))
                intersect(&v[-1], v, dst++);
            if (!(v[1].outcode & (u8)plane))
                intersect(&v[1], v, dst++);
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

void xn_poly_clip_plane_r(xn_regs *r)
{
    xn_poly_clip_plane(r->ebp);
}

/* One plane of the frustum: 1 when the polygon is now inside every plane, 0 when it is
   outside one, -1 to go on. keep: the outcode bits that stay (the planes done are cleared) */
static int clip_against(u32 plane, xn_routine fn, u8 keep)
{
    xn_poly_clip_intersect_fn = fn;
    xn_poly_clip_plane(plane);
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
        k = clip_against(0x10, asm_xn_poly_clip_intersect_near, 0xFF);
    if (k < 0 && (xn_poly_clip_outcode_or & 0x20))
        k = clip_against(0x20, asm_xn_poly_clip_intersect_far, 0xFF);
    if (k < 0 && (xn_poly_clip_outcode_or & 1))
        k = clip_against(1, asm_xn_poly_clip_intersect_left, 0xFE);
    if (k < 0 && (xn_poly_clip_outcode_or & 4))
        k = clip_against(4, asm_xn_poly_clip_intersect_bottom, 0xFA);
    if (k < 0 && (xn_poly_clip_outcode_or & 2))
        k = clip_against(2, asm_xn_poly_clip_intersect_right, 0xF8);
    if (k < 0 && (xn_poly_clip_outcode_or & 8))
        k = clip_against(8, asm_xn_poly_clip_intersect_top, 0xF0);
    return k != 0;
}

void xn_poly_clip_frustum_r(xn_regs *r)
{
    XN_SETFLAG(r, XN_CF, xn_poly_clip_frustum());
}

/* ---- projection --------------------------------------------------------------------------- */

/* In place: v from camera space to the screen with the view constants (half size, centre as
   24.8 + 0.5): 1/z = 2^40 / z; x = (hi(x * sx * 1/z) + cx) >> 3, the screen x with 5 fraction
   bits; y = (hi(y * sy * 1/z) + cy) >> 8, the row. The products x * sx are 32-bit; the shifts
   are unsigned. */
static void project(struct xn_poly_vertex *v, s32 sx, s32 cx, s32 sy, s32 cy)
{
    u32 inv_z = xn_udiv64(0x100, 0, v->z);

    v->z = inv_z;
    v->x = (u32)(xn_mulhi(v->x * sx, inv_z) + cx) >> 3;
    v->y = (u32)(xn_mulhi(v->y * sy, inv_z) + cy) >> 8;
}

/* The clipped polygon's ring of vertex pointers at vertex `top` (rings by count, for the buffer
   the clipper ended in), and its count for the walker */
static struct xn_poly_vertex **ring_at(u32 n, s32 top)
{
    xn_poly_vertex_count = (s8)n;
    if (xn_poly_clip_src == xn_poly_vertex_buf_a)
        return xn_poly_ring_a[n] + top;
    return xn_poly_ring_b[n] + top;
}

void xn_poly_project_flat(struct xn_poly_vertex *quad)
{
    struct xn_poly_vertex *v;
    s32 top_y = 0x7FFF;
    s32 top = 0;
    u32 n = 0;

    xn_poly_clip_src = quad;
    xn_poly_clip_src_end = quad + 4;
    if (xn_poly_clip_outcode_or) {
        xn_poly_clip_dst = quad + 32;
        if (!xn_poly_clip_frustum())
            return;
    }
    for (v = xn_poly_clip_src; v != xn_poly_clip_src_end; v++, n++) {
        project(v, xn_poly_flat_sx, xn_poly_flat_cx, xn_poly_flat_sy, xn_poly_flat_cy);
        if (v->y < top_y) {             /* the first of the top vertices */
            top_y = v->y;
            top = n;
        }
    }
    xn_flat_raster(top_y, ring_at(n, top));
}

int xn_poly_project_face(const struct xn_model_face *face, u32 codes)
{
    const struct xn_model_face_point *p = face->points;
    struct xn_span *spans = xn_render_span_next;
    struct xn_poly_vertex *v;
    u8 n = face->point_count;
    s32 top_y = 0x7FFFFFFF;
    s32 top = 0;

    if ((codes & 0xFF00) == 0) {
        /* every vertex inside: the projected ones. The asm counts in CL and keeps the ring's
           byte offset of the vertex in CH (from cx = n + 3FFh: n - 1 more vertices, offset 4,
           both bytes that wrap) and the top vertex's offset in DL */
        u32 c = n + 0x3FF;
        u8 left = (u8)c;
        u8 at = (u8)(c >> 8);
        u8 top_at = 0;

        xn_poly_vertex_count = n;
        v = xn_poly_vertex_buf_a;
        v->x = XN_AT(struct xn_vert_screen, xn_vert_screen, p->vertex).sx;
        v->z = XN_AT(struct xn_vert_screen, xn_vert_screen, p->vertex).inv_z;
        v->y = XN_AT(struct xn_vert_screen, xn_vert_screen, p->vertex).sy;
        top_y = v->y;
        do {                            /* dec cl; jne: a 1-vertex face runs 256 times */
            const struct xn_vert_screen *s;

            p++;
            v++;
            s = &XN_AT(struct xn_vert_screen, xn_vert_screen, p->vertex);
            v->x = s->sx;
            v->z = s->inv_z;
            v->y = s->sy;
            if (v->y <= top_y) {        /* the last of the top vertices */
                top_y = v->y;
                top_at = at;
            }
            at += 4;
        } while (--left != 0);
        xn_poly_rasterize(top_y, (struct xn_poly_vertex **)((u8 *)xn_poly_ring_a[at >> 2] +
                                                             top_at));
        return xn_render_span_next != spans;
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
    } while (--n != 0);
    xn_poly_clip_src_end = v;
    if (!xn_poly_clip_frustum())
        return 0;
    n = 0;
    for (v = xn_poly_clip_src; v != xn_poly_clip_src_end; v++, n++) {
        project(v, xn_poly_face_sx, xn_poly_face_cx, xn_poly_face_sy, xn_poly_face_cy);
        if (v->y <= top_y) {
            top_y = v->y;
            top = n;
        }
    }
    if (n >= 0x1C)
        return 0;
    xn_poly_rasterize(top_y, ring_at(n, top));
    return xn_render_span_next != spans;
}

void xn_poly_project_face_r(xn_regs *r)
{
    XN_SETFLAG(r, XN_CF, xn_poly_project_face((const struct xn_model_face *)r->esi, r->ebx));
}

void xn_poly_clc_stub_r(xn_regs *r)
{
    XN_SETFLAG(r, XN_CF, 0);
}

void xn_poly_project_terrain(u32 bytes)
{
    struct xn_poly_vertex *v;
    s32 top_y = 0x7FFFFFFF;
    s32 top = 0;
    u32 n = 0;

    xn_poly_clip_src = xn_poly_vertex_buf_a;
    xn_poly_clip_dst = xn_poly_vertex_buf_b;
    xn_poly_clip_src_end = (struct xn_poly_vertex *)((u8 *)xn_poly_vertex_buf_a + bytes);
    if (!xn_poly_clip_frustum())
        return;
    for (v = xn_poly_clip_src; v != xn_poly_clip_src_end; v++, n++) {
        project(v, xn_poly_terrain_sx, xn_poly_terrain_cx, xn_poly_terrain_sy,
                xn_poly_terrain_cy);
        if (v->y <= top_y) {
            top_y = v->y;
            top = n;
        }
    }
    xn_poly_rasterize(top_y, ring_at(n, top));
}

/* ---- the edge walker ------------------------------------------------------------------------ */

/* xn_span_insert (asm, rasteriser group): the span left..right of a row, 1/z at its left */
static void span_insert(struct xn_span *row, s32 left, s32 right, s32 inv_z, u32 rows)
{
    xn_regs r;

    r.esi = (u32)row;
    r.ebp = left;
    r.ebx = right;
    r.ecx = inv_z;
    r.edi = rows;
    r.eax = r.edx = 0;
    xn_asmcall(asm_xn_span_insert, &r);
}

/* the polygon walker's state (its code's fields) */
static const xn_walker poly_walker = {
    &xn_raster_left, &xn_raster_right, &xn_raster_dxl, &xn_raster_dzl, &xn_raster_dxr
};

int xn_walk_left_edge(const xn_walker *w, u32 *rows, s32 *xl, s32 *zl)
{
    for (;;) {
        struct xn_poly_vertex *v, *next;
        u16 dy;

        if (--xn_poly_vertex_count < 0)
            return 0;
        v = (*w->left)[0];
        next = (*w->left)[1];
        (*w->left)++;
        dy = (u16)(next->y - v->y);             /* the rows: 16-bit */
        *rows = (*rows & 0xFFFF0000) | dy;
        if ((s16)next->y <= (s16)v->y)
            continue;
        *w->dxl = (next->x - v->x) * (s32)xn_recip16_table[dy];
        *w->dzl = xn_mulhi(next->z - v->z, xn_recip32_table[dy]);
        *xl = v->x << 16;
        *zl = v->z;
        return 1;
    }
}

int xn_walk_right_edge(const xn_walker *w, u32 *rows, s32 *xr)
{
    for (;;) {
        struct xn_poly_vertex *v, *prev;
        s32 dy;

        if (--xn_poly_vertex_count < 0)
            return 0;
        v = (*w->right)[0];
        prev = (*w->right)[-1];
        (*w->right)--;
        if (prev->y <= v->y)
            continue;
        dy = prev->y - v->y;
        *w->dxr = (prev->x - v->x) * (s32)xn_recip16_table[dy];
        *rows |= (u32)dy << 16;
        *xr = v->x << 16;
        return 1;
    }
}

/* The walk from a row: each row's span (when its right end is right of its left end, in whole
   pixels), then the next row; a new edge whenever one side's rows run out (the left first when
   both do). rows: the left edge's remaining rows in the low word, the right's in the high. */
static void walk(struct xn_span *row, u32 rows, s32 xl, s32 xr, s32 zl)
{
    for (;;) {
        s32 left = (u32)xl >> 21;
        s32 right = (u32)xr >> 21;

        if (right > left)
            span_insert(row, left, right, zl, rows);
        row++;
        rows -= 0x10001;
        xl += xn_raster_dxl;
        xr += xn_raster_dxr;
        zl += xn_raster_dzl;
        if ((rows & 0xFFFF) == 0 && !xn_walk_left_edge(&poly_walker, &rows, &xl, &zl))
            return;
        if ((rows & 0xFFFF0000) == 0 && !xn_walk_right_edge(&poly_walker, &rows, &xr))
            return;
    }
}

void xn_poly_rasterize(s32 top_y, struct xn_poly_vertex **ring)
{
    u32 rows = 0;
    s32 xl, xr, zl;

    xn_raster_left = ring;
    xn_raster_right = ring;
    if (!xn_walk_left_edge(&poly_walker, &rows, &xl, &zl))
        return;
    if (!xn_walk_right_edge(&poly_walker, &rows, &xr))
        return;
    walk(&xn_render_span_rows[top_y], rows, xl, xr, zl);
}

void xn_poly_rasterize_r(xn_regs *r)
{
    xn_poly_rasterize(r->eax, (struct xn_poly_vertex **)r->ebp);
}

void xn_poly_rasterize_row_loop_r(xn_regs *r)
{
    walk((struct xn_span *)r->esi, r->edi, r->ebp, r->ebx, r->ecx);
}

/* ---- the textured setup --------------------------------------------------------------------- */

/* hi(a.x * m0) + hi(a.y * m1) + hi(a.z * m2): three high dwords added, not a 64-bit sum */
static s32 row_dot(const s32 *m, const xn_vec3 *a)
{
    return xn_mulhi(a->x, m[0]) + xn_mulhi(a->y, m[1]) + xn_mulhi(a->z, m[2]);
}

void xn_poly_tex_gradients(const xn_mat3 *m, const struct xn_model_face_data *axes,
                           struct xn_poly *poly)
{
    XN_KEEP(xn_grad_u0_a, axes->u_axis.x);
    XN_KEEP(xn_grad_u0_b, axes->u_axis.x);
    poly->u_step = row_dot(m->m[0], &axes->u_axis);
    poly->u_dx = poly->u_step >> 3;
    poly->u_dy = row_dot(m->m[1], &axes->u_axis);
    poly->u_c = row_dot(m->m[2], &axes->u_axis);
    XN_KEEP(xn_grad_v0_a, axes->v_axis.x);
    XN_KEEP(xn_grad_v0_b, axes->v_axis.x);
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

void xn_poly_setup_textured_r(xn_regs *r)
{
    struct xn_poly *poly = (struct xn_poly *)r->eax;
    struct xn_tex_image *image = poly->tex->current;
    s32 kind;
    u32 d;
    s32 sx, sy;
    u32 u, v;
    xn_s64 t;

    poly->tmap = image->tmap;
    poly->wrap_mask = image->wrap_mask;
    poly->texels = (u8 *)image + image->data_offset;
    kind = xn_call_light_setup(asm_xn_light_setup_poly, poly, r);
    /* 16-pixel spans when 1/z changes slowly across the screen */
    d = poly->inv_z_dx;
    if ((s32)d < 0)
        d = -d;
    if ((d >> (xn_poly_subdiv_shift & 31)) == 0)
        kind += 12;
    poly->span_fn = XN_AT(xn_routine, xn_render_tmap_span_fns, kind);
    xn_poly_tex_gradients((const xn_mat3 *)poly->handle->matrix, poly->face->points[2].data,
                          poly);
    /* the gradients at the face's first vertex: the texture origin, u in 6.26, v in 22.10 */
    sx = poly->cam_x * xn_poly_setup_sx;
    sy = poly->cam_y * xn_poly_setup_sy;
    sum3(&t, poly->u_dx, sx, poly->u_dy, sy, poly->u_c, poly->cam_z);
    u = xn_s64_shr(&t, 26);
    sum3(&t, poly->v_dx, sx, poly->v_dy, sy, poly->v_c, poly->cam_z);
    v = xn_s64_shr(&t, 10);
    /* the face's packed u/v less (v's high word | u's low word) */
    *(u32 *)&poly->tex_u0 = poly->face->points[0].uv_packed - ((v & 0xFFFF0000) | (u & 0xFFFF));
    /* the asm's `jmp [eax+3Ch]` with its registers then */
    r->ecx = sy;
    r->edx = *(u32 *)&poly->tex_u0;
    xn_asmcall(poly->span_fn, r);
}

void xn_poly_ret_stub(void)
{
}
