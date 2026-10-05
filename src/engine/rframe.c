/* rframe.c: xn_render_frame as readable C (xrframe.h). */
#include "xrframe.h"
#include "xnsmc.h"

extern u8 xn_tex_cache_full;
extern s32 xn_render_row_y;
extern s32 xn_gfx_clip_top, xn_cam_centre_y, xn_gfx_width;
extern s32 xn_gfx_row_offset[];
extern u8 *screen_buffer;
extern s32 xn_cam_dir_y_mid[];                  /* the y ray of a row, by row - centre y */
extern struct xn_span xn_render_span_rows[];    /* the rows' list heads (only .next used) */
extern void (*xn_render_span_hook)(void);

/* the lit span routines' y-ray operands, written per row (rule PF) */
extern s32 xn_span_solid_lit_ray_a, xn_span_solid_lit_ray_b, xn_span_solid_lit_ray_c;
extern s32 xn_span_texlit_ray_a, xn_span_texlit_ray_b, xn_span_texlit_ray_c;

/* the asm the frame calls (other groups', or code addresses it compares) */
extern void asm_xn_render_draw_models(void);    /* CF: failed */
extern void asm_xn_render_draw_flats(void);     /* CF: failed */
extern void asm_xn_render_fill_background(void);
extern void asm_xn_render_frame_row_end(void);  /* the sentinel polygon's routine */
void mem_check_heap(s32 checkpoint);            /* the game's heap check (a checkpoint) */

/* calls an asm routine that takes no registers; returns its CF (1: it failed) */
static int call_cf(void (*fn)(void))
{
    xn_regs r;

    r.eax = r.ecx = r.edx = r.ebx = r.ebp = r.esi = r.edi = 0;
    xn_asmcall(fn, &r);
    return (r.eflags & XN_CF) != 0;
}

s32 xn_render_frame(s32 flags)
{
    struct xn_span *head;
    u8 *row;                                    /* the row's first pixel - 1 */
    xn_regs r;

    xn_render_frame_flags = flags;
    if (xn_tex_cache_full || call_cf(asm_xn_render_draw_models))
        return 1;
    mem_check_heap(0x206);
    call_cf(asm_xn_render_fill_background);
    mem_check_heap(0x207);
    xn_render_row_y = xn_gfx_clip_top - xn_cam_centre_y;
    XN_KEEP(xn_render_frame_end_row, -xn_render_row_y);
    row = screen_buffer + xn_gfx_row_offset[xn_gfx_clip_top] - 1;
    XN_KEEP(xn_render_frame_width, xn_gfx_width);
    head = &xn_render_span_rows[xn_gfx_clip_top];
    r.ecx = r.edx = 0;
    do {
        struct xn_span *node, *next;
        s32 ray = xn_cam_dir_y_mid[xn_render_row_y];

        xn_span_solid_lit_ray_a = xn_span_solid_lit_ray_b = xn_span_solid_lit_ray_c = ray;
        xn_span_texlit_ray_a = xn_span_texlit_ray_b = xn_span_texlit_ray_c = ray;
        for (node = head->next; ; node = next) {
            struct xn_poly *poly = node->poly;
            s32 x0 = node->x_start, n = node->x_end - x0;

            next = node->next;                  /* read before the routine runs */
            if (poly->span_fn == XN_ASM(xn_render_frame_row_end))
                break;
            /* the polygon's span routine: eax = poly, esi = node, ebx = x - centre x,
               ebp = n, edi = the first pixel - 1; then the span hook with ebx = poly */
            r.eax = (u32)poly;
            r.esi = (u32)node;
            r.ebx = x0 - xn_render_frame_centre_x;
            r.ebp = n;
            r.edi = (u32)(row + x0);
            xn_asmcall(poly->span_fn, &r);
            r.ebx = (u32)poly;
            r.esi = (u32)node;
            r.ebp = n;
            r.edi = (u32)(row + x0);
            xn_asmcall(xn_render_span_hook, &r);
        }
        row += xn_render_frame_width;
        head++;
    } while (++xn_render_row_y != xn_render_frame_end_row);
    mem_check_heap(0x208);
    return call_cf(asm_xn_render_draw_flats);
}

/* asm: eax = the flags; returns eax and CF (1 failed), every other register kept */
void xn_render_frame_r(xn_regs *r)
{
    r->eax = xn_render_frame(r->eax);
    XN_SETFLAG(r, XN_CF, r->eax != 0);
}
