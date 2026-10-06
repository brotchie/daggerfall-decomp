/* drawscl.c: XnGine's scaled image (canonical C; the interface and the module's documentation
   are in xdraw.h). The asm compiled each row into machine code at big_buffer (one load, test and
   store sequence per source column) and called it once per screen row
   (docs/engine/smc/draw.md, DRAW-SCALED); the C computes the same columns once and draws them. */
#include "xdraw.h"

/* The columns of the row being drawn: the row's plan, at most one per screen column */
static xn_scaled_col row_cols[XN_SCALED_COLS];

/* n / d as the asm's `idiv r16` of DX:AX = n: the 16-bit quotient, or 0 when it does not fit
   in 16 bits or d is 0 (the divide error XnGine's handler turns into 0: Q-SYS-01) */
static s16 idiv16_or0(s32 n, s16 d)
{
    s32 q;

    if (d == 0)
        return 0;
    q = n / d;
    if (q < -32768 || q > 32767)
        return 0;
    return (s16)q;
}

/* an 8.8 step: v * 256 / d (`movzx dx, ah; shl ax, 8; idiv d`: v's low word, shifted) */
static s16 step88(u16 v, s16 d)
{
    return idiv16_or0((s32)v << 8, d);
}

int xn_draw_image_scaled_clip(s16 *x, s16 *y, u16 *skip_x, u16 *skip_y)
{
    *skip_x = 0;
    *skip_y = 0;
    if (*x >= (s16)xn_gfx_clip_right)
        return 0;
    if (*x < (s16)xn_gfx_clip_left) {
        *skip_x = (s16)xn_gfx_clip_left - *x;
        *x = (s16)xn_gfx_clip_left;
    }
    if (*y >= (s16)xn_gfx_clip_bottom)
        return 0;
    if (*y < (s16)xn_gfx_clip_top) {
        *skip_y = (s16)xn_gfx_clip_top - *y;
        *y = (s16)xn_gfx_clip_top;
    }
    return 1;
}

int xn_draw_image_scaled_row(xn_scaled_col *cols, u32 x, u16 src_ofs, u16 src_w, u8 step_int,
                             u8 step_frac)
{
    u16 acc = (u16)step_int << 8 | step_frac;   /* 8.8: the integer part is this column's */
    u16 left = src_w;
    int n = 0;

    do {
        s16 count = acc >> 8;

        if (count != 0) {
            if ((s16)(x + count) > (s16)xn_gfx_clip_right) {
                /* the window's right edge: this column, cut, is the row's last (`sub; jle`:
                   no pixels when the column is no wider than the cut) */
                s16 over = (s16)((s16)(x + count) - (s16)xn_gfx_clip_right);

                left = 1;
                count = count <= over ? 0 : count - over;
            }
            if (count > 0 && n < XN_SCALED_COLS) {
                cols[n].src = src_ofs;
                cols[n].count = count;
                cols[n].x = x;
                n++;
                x += count;
            }
        }
        src_ofs++;
        acc = (u16)(((u16)step_int << 8 | (acc & 0xFF)) + step_frac);
    } while (--left != 0);
    return n;
}

/* One screen row of the image: each column's source pixel, unless 0 or FFh, (through
   color_remap with flag 8000h) on its screen pixels */
static void draw_row(const xn_scaled_col *cols, int n, const u8 *src, u8 *dst, int remap)
{
    int i;

    for (i = 0; i < n; i++) {
        u8 c = src[cols[i].src];
        u32 k;

        if (c == 0 || c == 0xFF)
            continue;
        if (remap)
            c = color_remap[c];
        for (k = 0; k < cols[i].count; k++)
            dst[cols[i].x + k] = c;
    }
}

void xn_draw_image_scaled(s32 x, s32 y, s32 w, s32 h, s32 src_w, s32 src_h, s32 flags,
                          const u8 *src)
{
    s16 sx = (s16)x, sy = (s16)y, step_x, step_y, q;
    u16 skip_x, skip_y, src_ofs, rows, acc, cw = (u16)src_w, ch = (u16)src_h;
    u8 *dst, *bottom;
    int ncols;

    if ((s16)w <= 2)                    /* Quirk Q-DRAW-15 */
        return;
    if (flags & 8) {
        xn_img_unpack_rows(src, cw, ch, scratch_buffer);
        src = scratch_buffer;
    }
    step_x = step88((u16)w, (s16)cw);
    if ((s16)h <= 2)                    /* (the image unpacked all the same) */
        return;
    step_y = step88((u16)h, (s16)ch);
    if (!xn_draw_image_scaled_clip(&sx, &sy, &skip_x, &skip_y))
        return;
    /* the source rows and columns the clip cut (16-bit: `sub; jle` compares as signed) */
    q = step88(skip_y, step_y);
    if ((s16)ch <= q)
        return;
    ch -= q;
    src_ofs = (u16)(q * cw);            /* Quirk Q-DRAW-16: rows of src_w, not of 100h */
    q = step88(skip_x, step_x);
    if ((s16)cw <= q)
        return;
    cw -= q;
    src_ofs += q;
    bottom = XN_SCREEN(0, xn_gfx_clip_bottom);
    dst = screen_buffer + xn_gfx_row_offset[(u16)sy];
    ncols = xn_draw_image_scaled_row(row_cols, (u16)sx, src_ofs, cw, (u8)(step_x >> 8),
                                     (u8)step_x);
    /* each source row y-step times: the integer part of the 8.8 accumulator */
    acc = (u16)step_y;
    for (rows = ch; rows != 0; rows--) {
        u16 n;

        for (n = acc >> 8; n != 0; n--) {
            draw_row(row_cols, ncols, src, dst, (flags & 0x8000) != 0);
            dst += 320;                 /* (Q-DRAW-16: not xn_gfx_width) */
            if (dst >= bottom)
                return;
        }
        src += 0x100;                   /* Q-DRAW-16 */
        acc = (u16)(((u16)step_y & 0xFF00 | (acc & 0xFF)) + (u8)step_y);
    }
}
