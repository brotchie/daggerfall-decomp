/* drawunr.c: XnGine's 2D loops the asm unrolled (canonical C; the interface and the module's
   documentation are in xdraw.h; docs/engine/smc/draw.md has the asm's forms): the hurt flash,
   the dead row remaps and pattern fills, and the dead shaded blit. */
#include "xdraw.h"

/* every other pixel of a 320-pixel row, from `first` */
static void checker_row(u8 *row, int first)
{
    int k;

    for (k = first; k < 320; k += 2)
        row[k] = 0xF6;
}

void xn_draw_view_checkerboard(void)
{
    u8 *row = XN_SCREEN(0, xn_gfx_clip_top);
    u32 rows = xn_gfx_clip_bottom - xn_gfx_clip_top;
    u32 pairs = rows >> 1;

    if (rows & 1) {
        /* Quirk Q-DRAW-13: the asm enters its two-row loop at the second row: this row uses up
           a pair (and with 1 row, the count runs from 0) */
        checker_row(row, 1);
        row += 320;
        if (--pairs == 0)
            return;
    }
    do {
        checker_row(row, 0);
        row += 320;
        checker_row(row, 1);
        row += 320;
    } while (--pairs != 0);
}

void xn_draw_remap_rows_320(const u8 *src, u8 *dst, const u8 *table, s32 rows)
{
    u32 n = rows;

    table = XN_TABLE256(table);
    do {
        int k;

        for (k = 0; k < 320; k++)
            dst[k] = table[src[k]];
        src += 320;
        dst += 320;
    } while (--n != 0);
}

void xn_draw_fill_rows_pattern8(const u8 *src, u8 *dst, s32 rows)
{
    u32 n = rows;

    do {
        const u32 *pattern = (const u32 *)src;
        u32 *d = (u32 *)dst;
        int k;

        for (k = 0; k < 40; k++) {
            d[2 * k] = pattern[0];
            d[2 * k + 1] = pattern[1];
        }
        src += 8;
        dst += 320;
    } while (--n != 0);
}

int xn_draw_clip_image_rect(xn_blit *b)
{
    s32 end, cut;

    if (b->x >= xn_gfx_clip_right || b->y >= xn_gfx_clip_bottom)
        return 0;
    if (b->y < xn_gfx_clip_top) {
        b->y -= xn_gfx_clip_top;                /* minus the rows above the window */
        cut = b->h;
        b->h += b->y;
        if (xn_add_le0(cut, b->y))
            return 0;
        b->buf -= b->y * b->w;
        b->y = xn_gfx_clip_top;
    }
    if (b->x < xn_gfx_clip_left) {
        b->x -= xn_gfx_clip_left;
        cut = b->w;
        b->w += b->x;
        if (xn_add_le0(cut, b->x))
            return 0;
        b->skip -= b->x;
        b->buf -= b->x;
        b->x = xn_gfx_clip_left;
    }
    end = b->y + b->h;
    if (end > xn_gfx_clip_bottom) {
        cut = end - xn_gfx_clip_bottom;
        if (b->h <= cut) {                      /* (`sub; jle`: a true compare) */
            b->h -= cut;
            return 0;
        }
        b->h -= cut;
    }
    end = b->x + b->w;
    if (end > xn_gfx_clip_right) {
        b->w -= end - xn_gfx_clip_right;
        b->skip += end - xn_gfx_clip_right;
    }
    return 1;
}

void xn_draw_shaded_row(u8 *dst, const u8 *src, u32 n, const u8 *table)
{
    u32 k;

    for (k = 0; k < n; k++)
        if (src[k] != 0)
            dst[k] = table[src[k]];
}

void xn_draw_image_shaded(s32 x, s32 y, s32 w, s32 h, const u8 *src)
{
    xn_blit b;
    const u8 *table;
    u8 *row;
    u32 rows;

    b.x = x;
    b.y = y;
    b.w = w;
    b.h = h;
    b.skip = 0;
    b.buf = (u8 *)src;
    if (!xn_draw_clip_image_rect(&b))
        return;
    row = XN_SCREEN(b.x, b.y);
    table = XN_TABLE256(xn_shade_table + xn_draw_shade_row * 256);     /* Quirk Q-DRAW-14 */
    rows = b.h;
    do {
        xn_draw_shaded_row(row, b.buf, b.w, table);
        b.buf += b.w + b.skip;
        row += xn_gfx_width;
    } while (--rows != 0);
}

void xn_draw_ret_stub(void)
{
}
