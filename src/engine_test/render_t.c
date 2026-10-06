/* render_t.c: test shims of src/engine/render.c (built only by tools/xn_rc.py; see
   src/engine_test/vec_t.c and docs/xngine_canonical.md). Each NAME_r maps the registers of
   NAME's asm entry (config/xngine_abi.csv) to the canonical C call. */
#include "xrender.h"

void xn_render_init_r(xn_regs *r)
{
    (void)r;
    xn_render_init();
}

void xn_render_shutdown_r(xn_regs *r)
{
    (void)r;
    xn_render_shutdown();
}

void xn_render_set_mode_r(xn_regs *r)
{
    xn_render_set_mode(r->eax);
}

void xn_render_begin_frame_r(xn_regs *r)
{
    (void)r;
    xn_render_begin_frame();
}

/* x, y in EAX, EDX (the asm passed its caller's EBX on to the flat pick, which only kept it
   for the texture cache's unpack key: nothing reads it) */
void xn_render_pick_r(xn_regs *r)
{
    r->eax = (u32)xn_render_pick(r->eax, r->edx);
}

/* the setups' entries: EAX = the polygon, ESI = the span node, EBX = x - centre x, EBP = the
   count, EDI = the first pixel - 1 */
#define SPAN_ARGS(r) (struct xn_poly *)(r)->eax, (const struct xn_span *)(r)->esi, (r)->ebx, \
                     (r)->ebp, (u8 *)(r)->edi + 1

void xn_render_span_setup_solid_r(xn_regs *r)
{
    xn_render_span_setup_solid(SPAN_ARGS(r));
}

void xn_render_span_setup_terrain_r(xn_regs *r)
{
    xn_render_span_setup_terrain(SPAN_ARGS(r));
}

void xn_render_span_setup_terrain_solid_r(xn_regs *r)
{
    xn_render_span_setup_terrain_solid(SPAN_ARGS(r));
}

/* AL: the colour (text_colour) */
void xn_render_span_mark_ends_r(xn_regs *r)
{
    extern u8 text_colour;

    xn_render_span_mark_ends(SPAN_ARGS(r));
    r->eax = (r->eax & ~0xFFu) | text_colour;
}

/* CF: a draw failed */
void xn_render_draw_models_r(xn_regs *r)
{
    XN_SETFLAG(r, XN_CF, xn_render_draw_models());
}

void xn_render_draw_flats_r(xn_regs *r)
{
    XN_SETFLAG(r, XN_CF, xn_render_draw_flats());
}

void xn_render_fill_background_r(xn_regs *r)
{
    (void)r;
    xn_render_fill_background();
}

/* EAX = the pairs, EDX = the first pair's byte offset, EBX = the last's */
void xn_render_sort_pairs_saveregs_r(xn_regs *r)
{
    xn_render_sort_pairs((struct xn_sort_pair *)r->eax, r->edx, r->ebx);
}

void xn_render_sort_pairs_r(xn_regs *r)
{
    xn_render_sort_pairs((struct xn_sort_pair *)r->eax, r->edx, r->ebx);
}

/* -> ESI = where the scan stopped */
void xn_render_sort_pairs_range_r(xn_regs *r)
{
    r->esi = xn_render_sort_pairs_range((struct xn_sort_pair *)r->eax, r->edx, r->ebx);
}
