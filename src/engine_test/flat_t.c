/* flat_t.c: test shims of src/engine/flat.c (built only by tools/xn_rc.py; see
   src/engine_test/vec_t.c and docs/xngine_canonical.md). Each NAME_r maps the registers of
   NAME's asm entry (config/xngine_abi.csv) to the canonical C call and back. The asm passed
   the flat being drawn, its row, its row steps and its walker's edges through operands of its
   own code (smc FLAT-*), and its image's width and the walker's count through globals: the
   shims read them there. A flat's span routine in the polygon pool is the asm entry the asm
   stored: the shims that reach it put the C routine there (the pool's span field is
   private). */
#include "xflat.h"
#include "xspan.h"

extern s8 xn_poly_vertex_count;                 /* the asm walker's count */
extern u32 xn_flat_tex_w;                       /* the drawn flat's image width (154C54) */
extern struct xn_flat *xn_flat_emit_flat;       /* 15526F: the flat being drawn */
extern u8 *xn_flat_emit_row;                    /* 155275: its row's first pixel - 1 */
extern s32 xn_flat_row_u_step, xn_flat_row_v_step;     /* 155353 15535D */
extern struct xn_poly_vertex **xn_flat_raster_left, **xn_flat_raster_right;
extern s32 xn_flat_raster_dxl, xn_flat_raster_dzl, xn_flat_raster_dxr;
extern xn_mat3 xn_flat_matrix;
/* the flat span routines' asm entries */
extern void asm_xn_flat_span_light_setup(void);
extern void asm_xn_span_flat_transparent(void);
extern void asm_xn_span_flat_transparent_shaded(void);
extern void asm_xn_span_flat_lit_fogged(void);
extern void asm_xn_span_flat_translucent(void);

/* the C routine of an asm entry the asm stored as the flat's span routine */
static void c_span(struct xn_flat *flat)
{
    void (*fn)(void) = (void (*)(void))flat->span;

    if (fn == asm_xn_flat_span_light_setup)
        flat->span = xn_flat_span_light_setup;
    else if (fn == asm_xn_span_flat_transparent)
        flat->span = (xn_flat_span_fn)xn_span_flat_transparent;
    else if (fn == asm_xn_span_flat_transparent_shaded)
        flat->span = (xn_flat_span_fn)xn_span_flat_transparent_shaded;
    else if (fn == asm_xn_span_flat_lit_fogged)
        flat->span = (xn_flat_span_fn)xn_span_flat_lit_fogged;
    else if (fn == asm_xn_span_flat_translucent)
        flat->span = (xn_flat_span_fn)xn_span_flat_translucent;
}

/* the flat being drawn, as the asm's draw left it (xn_poly_project_flat's shim too) */
void xn_flat_t_walk_of_asm(xn_flat_walk *w)
{
    w->flat = xn_flat_emit_flat;
    w->tex_w = xn_flat_tex_w;
    w->row = xn_flat_emit_row + 1;
    w->u_step = xn_flat_row_u_step;
    w->v_step = xn_flat_row_v_step;
    c_span(w->flat);
}

/* x, y, z, image in EAX EDX EBX ECX; frame, flags, scale on the stack -> EAX */
void xn_flat_add_r(xn_regs *r)
{
    r->eax = xn_flat_add(r->eax, r->edx, r->ebx, r->ecx, XN_STACK_ARG(r, 0), XN_STACK_ARG(r, 1),
                         XN_STACK_ARG(r, 2));
}

/* x, y, z, image in EAX EDX EBX ECX; frame EBP, flags ESI, scale EDI -> EDI */
void xn_flat_add_body_r(xn_regs *r)
{
    r->edi = xn_flat_add_body(r->eax, r->edx, r->ebx, r->ecx, r->ebp, r->esi, r->edi);
}

void xn_flat_add_view_r(xn_regs *r)
{
    xn_flat_add_view(r->eax, r->edx, r->ebx, r->ecx);
}

/* the flat in EDI -> CF when the texture cache failed */
void xn_flat_draw_r(xn_regs *r)
{
    XN_SETFLAG(r, XN_CF, xn_flat_draw((struct xn_flat *)r->edi));
}

/* the quad's last corner (2) and its outcode in EAX EDX EBX ECX, the buffer in ESI, the last
   rotated offset in EBP (the half height, negated, << 4) */
static void quad_regs(xn_regs *r, s32 hh)
{
    const struct xn_poly_vertex *last = &xn_poly_vertex_buf_a[2];

    r->eax = last->x;
    r->edx = last->y;
    r->ebx = last->z;
    r->ecx = last->outcode;
    r->esi = (u32)xn_poly_vertex_buf_a;
    r->ebp = (u32)-hh << 4;
}

/* w EAX, h EDX, the flat EDI */
void xn_flat_quad_centred_r(xn_regs *r)
{
    xn_flat_quad_centred(r->eax, r->edx, (const struct xn_flat *)r->edi);
    quad_regs(r, r->edx >> 1);
}

void xn_flat_quad_standing_r(xn_regs *r)
{
    xn_flat_quad_standing(r->eax, r->edx, (const struct xn_flat *)r->edi);
    quad_regs(r, r->edx);
}

void xn_flat_quad_none_r(xn_regs *r)
{
    (void)r;
    xn_flat_quad_none();
}

void xn_flat_quad_none_8_r(xn_regs *r)
{
    (void)r;
    xn_flat_quad_none_8();
}

void xn_flat_quad_none_16_r(xn_regs *r)
{
    (void)r;
    xn_flat_quad_none_16();
}

/* the top row in EAX (kept) */
void xn_flat_setup_gradients_r(xn_regs *r)
{
    xn_flat_walk w;

    xn_flat_t_walk_of_asm(&w);
    xn_flat_setup_gradients(&w, r->eax);
}

/* the row head in ESI, x0 EBX, x1 EBP, 1/z ECX */
void xn_flat_span_clip_r(xn_regs *r)
{
    xn_flat_walk w;

    xn_flat_t_walk_of_asm(&w);
    xn_flat_span_clip(&w, (struct xn_span *)r->esi, r->ebx, r->ebp, r->ecx);
}

void xn_flat_span_emit_r(xn_regs *r)
{
    xn_flat_walk w;

    xn_flat_t_walk_of_asm(&w);
    xn_flat_span_emit(&w, r->ebx, r->ebp, r->ecx);
}

/* the top row in EAX, the ring place in EBP; the count as xn_poly_project_flat left it */
void xn_flat_raster_r(xn_regs *r)
{
    xn_flat_walk w;

    xn_flat_t_walk_of_asm(&w);
    xn_flat_raster(&w, r->eax, (struct xn_poly_vertex **)r->ebp, (u8)xn_poly_vertex_count);
}

/* the row loop's entry: the row in ESI, the rows in EDI, the forward and backward x in EBP
   EBX, 1/z in ECX; the edges in the walker's operands */
void xn_flat_raster_rows_r(xn_regs *r)
{
    xn_flat_walk w;

    xn_flat_t_walk_of_asm(&w);
    w.edges.left = xn_flat_raster_left;
    w.edges.right = xn_flat_raster_right;
    w.edges.dxl = xn_flat_raster_dxl;
    w.edges.dzl = xn_flat_raster_dzl;
    w.edges.dxr = xn_flat_raster_dxr;
    w.edges.count = xn_poly_vertex_count;
    w.edges.rows = r->edi;
    w.edges.xl = r->ebp;
    w.edges.xr = r->ebx;
    w.edges.zl = r->ecx;
    xn_flat_raster_rows(&w, (struct xn_span *)r->esi);
}

/* the offsets in ECX EBP -> the corner offset in EAX EDX EBX, EBP << 4 */
void xn_flat_rotate_offset_r(xn_regs *r)
{
    xn_vec3 v;

    xn_flat_rotate_offset(r->ecx, r->ebp, &v);
    r->eax = v.x;
    r->edx = v.y;
    r->ebx = v.z;
    r->ebp <<= 4;
}

/* -> the flat matrix's address in ESI */
void xn_flat_begin_frame_r(xn_regs *r)
{
    xn_flat_begin_frame();
    r->esi = (u32)&xn_flat_matrix;
}

/* x EAX, y EDX (EBX: a frame the C no longer needs) -> the flat in EAX */
void xn_flat_pick_r(xn_regs *r)
{
    r->eax = (u32)xn_flat_pick(r->eax, r->edx);
}

/* a flat span routine's registers: the flat in ESI */
void xn_flat_span_light_setup_r(xn_regs *r)
{
    xn_flat_span_light_setup((struct xn_flat *)r->esi, r->ecx, r->ebx, r->ebp, (u8 *)r->edi + 1);
}
