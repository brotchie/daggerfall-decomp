/* poly_t.c: test shims of src/engine/poly.c (built only by tools/xn_rc.py; see
   src/engine_test/vec_t.c and docs/xngine_canonical.md). Each NAME_r maps the registers of
   NAME's asm entry (config/xngine_abi.csv) to the canonical C call and back. What the asm took
   from its callers' patched operands (the walker's edges at the row loop's entry, the flat
   being drawn) and from the clipper's routine and count globals, the shims read where the
   asm's callers left them. */
#include "xflat.h"

extern xn_routine xn_poly_clip_intersect_fn;   /* the asm's clip pass routine (0x15820C) */
extern s8 xn_poly_vertex_count;                 /* the asm walker's count (0x15B984) */
/* the flat being drawn as the asm's draw left it (flat_t.c) */
void xn_flat_t_walk_of_asm(xn_flat_walk *w);
/* the asm walker's edges, operands of its own code */
extern struct xn_poly_vertex **xn_raster_left, **xn_raster_right;
extern s32 xn_raster_dxl, xn_raster_dzl, xn_raster_dxr;
/* the intersect routines' asm entries (what the asm's clip pass routine holds) */
extern void asm_xn_poly_clip_intersect_left(void);
extern void asm_xn_poly_clip_intersect_right(void);
extern void asm_xn_poly_clip_intersect_bottom(void);
extern void asm_xn_poly_clip_intersect_top(void);
extern void asm_xn_poly_clip_intersect_near(void);
extern void asm_xn_poly_clip_intersect_far(void);

/* x, y, z in EAX EDX EBX -> the outcode in ECX */
void xn_poly_outcode_r(xn_regs *r)
{
    r->ecx = xn_poly_outcode(r->eax, r->edx, r->ebx);
}

void xn_poly_outcode_eax_r(xn_regs *r)
{
    r->eax = xn_poly_outcode_eax(r->eax, r->edx, r->ebx);
}

/* in EBX, out ESI, the new vertex at EDI -> its x, y, z in EAX EDX EBX, its outcode in ECX,
   EDI past it (ZF from that add) */
static void cut_regs(xn_clip_cut_fn cut, xn_regs *r)
{
    struct xn_poly_vertex *dst = (struct xn_poly_vertex *)r->edi;

    cut((const struct xn_poly_vertex *)r->ebx, (const struct xn_poly_vertex *)r->esi, dst);
    r->eax = dst->x;
    r->edx = dst->y;
    r->ebx = dst->z;
    r->ecx = dst->outcode;
    r->edi += sizeof(struct xn_poly_vertex);
    XN_SETFLAG(r, XN_ZF, r->edi == 0);
}

void xn_poly_clip_intersect_left_r(xn_regs *r)
{
    cut_regs(xn_poly_clip_intersect_left, r);
}

void xn_poly_clip_intersect_right_r(xn_regs *r)
{
    cut_regs(xn_poly_clip_intersect_right, r);
}

void xn_poly_clip_intersect_bottom_r(xn_regs *r)
{
    cut_regs(xn_poly_clip_intersect_bottom, r);
}

void xn_poly_clip_intersect_top_r(xn_regs *r)
{
    cut_regs(xn_poly_clip_intersect_top, r);
}

void xn_poly_clip_intersect_near_r(xn_regs *r)
{
    cut_regs(xn_poly_clip_intersect_near, r);
}

void xn_poly_clip_intersect_far_r(xn_regs *r)
{
    cut_regs(xn_poly_clip_intersect_far, r);
}

/* the plane bit in EBP; the cut routine is the asm entry in xn_poly_clip_intersect_fn */
void xn_poly_clip_plane_r(xn_regs *r)
{
    xn_routine fn = xn_poly_clip_intersect_fn;
    xn_clip_cut_fn cut = xn_poly_clip_intersect_near;

    if (fn == asm_xn_poly_clip_intersect_left)
        cut = xn_poly_clip_intersect_left;
    else if (fn == asm_xn_poly_clip_intersect_right)
        cut = xn_poly_clip_intersect_right;
    else if (fn == asm_xn_poly_clip_intersect_bottom)
        cut = xn_poly_clip_intersect_bottom;
    else if (fn == asm_xn_poly_clip_intersect_top)
        cut = xn_poly_clip_intersect_top;
    else if (fn == asm_xn_poly_clip_intersect_far)
        cut = xn_poly_clip_intersect_far;
    xn_poly_clip_plane(r->ebp, cut);
}

/* CF set when the polygon is left inside every plane */
void xn_poly_clip_frustum_r(xn_regs *r)
{
    XN_SETFLAG(r, XN_CF, xn_poly_clip_frustum());
}

/* the quad at ESI; the flat as the asm's xn_flat_draw left it */
void xn_poly_project_flat_r(xn_regs *r)
{
    xn_flat_walk w;

    xn_flat_t_walk_of_asm(&w);
    xn_poly_project_flat(&w, (struct xn_poly_vertex *)r->esi);
}

/* the face at ESI, the outcodes in BX -> CF when spans were added */
void xn_poly_project_face_r(xn_regs *r)
{
    XN_SETFLAG(r, XN_CF, xn_poly_project_face((const struct xn_model_face *)r->esi,
                                              r->ebx & 0xFFFF));
}

void xn_poly_clc_stub_r(xn_regs *r)
{
    XN_SETFLAG(r, XN_CF, xn_poly_clc_stub());
}

/* the vertices' bytes in ECX */
void xn_poly_project_terrain_r(xn_regs *r)
{
    xn_poly_project_terrain(r->ecx / sizeof(struct xn_poly_vertex));
}

/* the top row in EAX, the ring place in EBP; the count as the caller left it */
void xn_poly_rasterize_r(xn_regs *r)
{
    xn_poly_rasterize(r->eax, (struct xn_poly_vertex **)r->ebp, (u8)xn_poly_vertex_count);
}

/* the row loop's entry: the row in ESI, the rows in EDI, left and right x in EBP EBX, 1/z in
   ECX; the edges in the walker's operands */
void xn_poly_rasterize_row_loop_r(xn_regs *r)
{
    xn_edge_walk w;

    w.left = xn_raster_left;
    w.right = xn_raster_right;
    w.dxl = xn_raster_dxl;
    w.dzl = xn_raster_dzl;
    w.dxr = xn_raster_dxr;
    w.count = xn_poly_vertex_count;
    w.rows = r->edi;
    w.xl = r->ebp;
    w.xr = r->ebx;
    w.zl = r->ecx;
    xn_poly_rasterize_rows((struct xn_span *)r->esi, &w);
}

/* the matrix in ECX, the axes in EDX, the polygon in EDI */
void xn_poly_tex_gradients_r(xn_regs *r)
{
    xn_poly_tex_gradients((const xn_mat3 *)r->ecx, (const struct xn_model_face_data *)r->edx,
                          (struct xn_poly *)r->edi);
}

/* a span routine's registers: the polygon in EAX, the node in ESI, x - centre in EBX, the
   count in EBP, the first pixel - 1 in EDI */
void xn_poly_setup_textured_r(xn_regs *r)
{
    xn_poly_setup_textured((struct xn_poly *)r->eax, (const struct xn_span *)r->esi, r->ebx,
                           r->ebp, (u8 *)r->edi + 1);
}

void xn_poly_ret_stub_r(xn_regs *r)
{
    (void)r;
    xn_poly_ret_stub();
}
