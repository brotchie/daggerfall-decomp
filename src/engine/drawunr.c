/* drawunr.c: XnGine's unrolled 2D loops as readable C (xdraw.h; see xngine.h,
   docs/xngine_readable.md and docs/engine/smc/draw.md): the hurt flash, the dead row
   remaps and pattern fills, and the dead shaded blit with its planted-ret row. */
#include "xdraw.h"

extern u8 xn_draw_shade_row;            /* the shade row of the dead shaded blit (never set) */
extern u8 asm_xn_draw_shaded_row_unrolled[];

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
        /* the asm enters its two-row loop at the second row: this row uses up a pair */
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
    s32 end;

    if (b->x >= xn_gfx_clip_right || b->y >= xn_gfx_clip_bottom)
        return 0;
    if (b->y < xn_gfx_clip_top) {
        b->y -= xn_gfx_clip_top;                /* minus the rows above the window */
        b->h += b->y;
        if (b->h <= 0)
            return 0;
        b->buf -= b->y * b->w;
        b->y = xn_gfx_clip_top;
    }
    if (b->x < xn_gfx_clip_left) {
        b->x -= xn_gfx_clip_left;
        b->w += b->x;
        if (b->w <= 0)
            return 0;
        b->skip -= b->x;
        b->buf -= b->x;
        b->x = xn_gfx_clip_left;
    }
    end = b->y + b->h;
    if (end > xn_gfx_clip_bottom) {
        b->h -= end - xn_gfx_clip_bottom;
        if (b->h <= 0)
            return 0;
    }
    end = b->x + b->w;
    if (end > xn_gfx_clip_right) {
        b->w -= end - xn_gfx_clip_right;
        b->skip += end - xn_gfx_clip_right;
    }
    return 1;
}

void xn_draw_clip_image_rect_r(xn_regs *r)
{
    xn_blit b;
    int ok;

    b.x = r->eax;
    b.y = r->edx;
    b.w = r->ebx;
    b.h = r->ecx;
    b.skip = r->ebp;
    b.buf = (u8 *)r->esi;
    ok = xn_draw_clip_image_rect(&b);
    r->eax = b.x;
    r->edx = b.y;
    r->ebx = b.w;
    r->ecx = b.h;
    r->ebp = b.skip;
    r->esi = (u32)b.buf;
    XN_SETFLAG(r, XN_CF, !ok);
}

void xn_draw_image_shaded(s32 x, s32 y, s32 w, s32 h, const u8 *src)
{
    xn_draw_image_shaded_regs(x, y, w, h, src);
}

void xn_draw_ret_stub(void)
{
}

/* the row: each non-zero pixel of src[0..n) through table to dst; returns the last step's
   colour (0 for a skipped pixel), as AL */
static u8 shaded_run(u8 *dst, const u8 *src, u32 n, const u8 *table)
{
    u32 k;
    u8 c = 0;

    for (k = 0; k < n; k++) {
        c = src[k];
        if (c != 0) {
            c = table[c];
            dst[k] = c;
        }
    }
    return c;
}

/* (The asm's unrolled row has 321 steps; a w over 321 would plant its ret past it.) */
void xn_draw_image_shaded_regs(s32 x, s32 y, s32 w, s32 h, const u8 *src)
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
    table = XN_TABLE256(xn_shade_table + xn_draw_shade_row * 256);
    rows = b.h;
    do {
        shaded_run(row, b.buf, b.w, table);
        b.buf += b.w + b.skip;
        row += xn_gfx_width;
    } while (--rows != 0);
}

void xn_draw_shaded_row_unrolled_r(xn_regs *r)
{
    int n = xn_planted_count(asm_xn_draw_shaded_row_unrolled, 18, 321);
    u8 last = shaded_run((u8 *)r->edi + 0x100, (const u8 *)r->esi + 0x100, n,
                         XN_TABLE256(r->eax));

    if (n > 0)
        r->eax = (r->eax & ~0xFFu) | last;
}
