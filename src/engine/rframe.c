/* rframe.c: XnGine's frame loop (xrframe.h). */
#include "xrframe.h"
#include "xrender.h"
#include "xtmap.h"

/* the game's heap check, with the caller's checkpoint number (an exit of the boundary) */
void mem_check_heap(s32 checkpoint);

xn_span_hook_fn xn_render_span_hook;

s32 xn_render_frame(s32 flags)
{
    struct xn_span *head;
    u8 *row;                                    /* the row's first pixel */
    s32 end_row;

    xn_render_frame_flags = flags;
    if (xn_tex_cache_full || xn_render_draw_models())
        return 1;
    mem_check_heap(0x206);
    xn_render_fill_background();
    mem_check_heap(0x207);
    /* Quirk Q-RENDER-05: the rows from the clip top to the row as far below the view centre
       as the clip top is above it */
    xn_render_row_y = xn_gfx_clip_top - xn_cam_centre_y;
    end_row = -xn_render_row_y;
    row = screen_buffer + xn_gfx_row_offset[xn_gfx_clip_top];
    head = &xn_render_span_rows[xn_gfx_clip_top];
    do {
        const struct xn_span *node, *next;

        /* the row's spans up to the sentinel, which is its own next */
        for (node = head->next; node != node->next; node = next) {
            struct xn_poly *poly = node->poly;
            s32 x0 = node->x_start, n = node->x_end - x0;

            next = node->next;                  /* read before the routine runs */
            poly->span_fn(poly, node, x0 - xn_cam_centre_x, n, row + x0);
            xn_render_span_hook(poly, node, n, row + x0);
        }
        row += xn_gfx_width;
        head++;
    } while (++xn_render_row_y != end_row);
    mem_check_heap(0x208);
    return xn_render_draw_flats();
}
