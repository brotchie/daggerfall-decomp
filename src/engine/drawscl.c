/* drawscl.c: XnGine's scaled image (xn_draw_image_scaled) and its row compiler as readable C
   (xdraw.h; see xngine.h, docs/xngine_readable.md and docs/engine/smc/draw.md,
   DRAW-SCALED). The compiler still emits the row's machine code into big_buffer (its bytes
   are behaviour the records compare); the C draws the row from the plan built with it. */
#include "xdraw.h"

#define SCALED_COLS 330         /* columns a row can have: at most one per screen column + 1 */

int xn_draw_image_scaled_clip(void)
{
    xn_draw_scaled_skip_x = 0;
    xn_draw_scaled_skip_y = 0;
    if (xn_draw_scaled_x >= (s16)xn_gfx_clip_right)
        return 0;
    if (xn_draw_scaled_x < (s16)xn_gfx_clip_left) {
        xn_draw_scaled_skip_x = (s16)xn_gfx_clip_left - xn_draw_scaled_x;
        xn_draw_scaled_x = xn_gfx_clip_left;
    }
    if (xn_draw_scaled_y >= (s16)xn_gfx_clip_bottom)
        return 0;
    if (xn_draw_scaled_y < (s16)xn_gfx_clip_top) {
        xn_draw_scaled_skip_y = (s16)xn_gfx_clip_top - xn_draw_scaled_y;
        xn_draw_scaled_y = xn_gfx_clip_top;
    }
    return 1;
}

void xn_draw_image_scaled_clip_r(xn_regs *r)
{
    XN_SETFLAG(r, XN_CF, !xn_draw_image_scaled_clip());
}

/* the 8.8 accumulator's next value: the integer part reloaded, the fraction added */
static u16 next_acc(u8 step_int, u16 acc, u16 step_frac)
{
    return (u16)(((u16)step_int << 8 | (acc & 0xFF)) + step_frac);
}

void xn_draw_image_scaled_compile_row(xn_scaled_row *row, u32 loop_hi)
{
    u8 *code = big_buffer;
    u16 cols = xn_draw_scaled_src_w;
    u16 acc = (u16)xn_draw_scaled_x_int << 8 | (u8)xn_draw_scaled_x_frac;
    u32 x = (u16)xn_draw_scaled_x;

    row->n = 0;
    row->remap = (xn_draw_scaled_flags & 0x8000) != 0;
    row->last_jump = 0;
    do {
        s16 count = acc >> 8;

        row->count_left = 0;
        if (count != 0) {
            u8 *je_zero, *je_ff;
            xn_scaled_col *col = row->n < row->cap ? &row->col[row->n] : 0;

            /* mov al, [esi + src_ofs]; or al, al; je; cmp al, 0FFh; je */
            code[0] = 0x8A;
            code[1] = 0x86;
            XN_PUT32(code + 2, xn_draw_scaled_src_ofs);
            code[6] = 0x0A;
            code[7] = 0xC0;
            code[8] = 0x74;
            code += 10;
            je_zero = code;
            code[0] = 0x3C;
            code[1] = 0xFF;
            code[2] = 0x74;
            code += 4;
            je_ff = code;
            if (col) {
                col->src = xn_draw_scaled_src_ofs;
                col->x = (u16)x;
                col->count = 0;
            }
            if ((s16)(x + count) > (s16)xn_gfx_clip_right) {
                /* the window's right edge: the last column, cut */
                cols = 1;
                count -= (s16)(x + count) - (s16)xn_gfx_clip_right;
                if (count <= 0)
                    row->count_left = count;
            }
            if (count > 0) {
                u32 pairs;

                if (row->remap)
                    *code++ = 0xD7;                     /* xlat */
                if (count & 1) {                        /* mov [edi + x], al */
                    code[0] = 0x88;
                    code[1] = 0x87;
                    XN_PUT32(code + 2, x);
                    code += 6;
                    x++;
                    if (col)
                        col->count++;
                }
                if ((count >> 1) != 0) {
                    pairs = (loop_hi << 16) | (count >> 1);   /* (`loop` counts all of ECX) */
                    code[0] = 0x8A;                     /* mov ah, al */
                    code[1] = 0xE0;
                    code += 2;
                    do {                                /* mov [edi + x], ax */
                        code[0] = 0x66;
                        code[1] = 0x89;
                        code[2] = 0x87;
                        XN_PUT32(code + 3, x);
                        code += 7;
                        x += 2;
                        if (col)
                            col->count += 2;
                    } while (--pairs != 0);
                    loop_hi = 0;
                }
            }
            je_ff[-1] = (u8)(code - je_ff);
            je_zero[-1] = (u8)(code - je_zero);
            row->last_jump = code - je_zero;
            row->n++;
        }
        xn_draw_scaled_src_ofs++;
        acc = next_acc(xn_draw_scaled_x_int, acc, xn_draw_scaled_x_frac);
    } while (--cols != 0);
    *code = 0xC3;
    row->acc = acc;
    row->loop_hi = loop_hi;
}

void xn_draw_image_scaled_compile_row_r(xn_regs *r)
{
    xn_scaled_row row;

    row.col = 0;
    row.cap = 0;
    xn_draw_image_scaled_compile_row(&row, r->ecx >> 16);
    if (row.n != 0)
        r->eax = row.last_jump;
    r->ecx = (u32)row.loop_hi << 16 | row.count_left;
    r->edx = (r->edx & 0xFFFF0000u) | xn_draw_scaled_x_frac;
    r->ebx = (r->ebx & 0xFFFF0000u) | row.acc;
    r->ebp &= 0xFFFF0000u;                      /* BP: the column count, spent */
}

/* What the emitted code does for one source row: each column's pixel, unless 0 or FFh,
   (through color_remap) on its screen pixels */
static void run_row(const xn_scaled_row *row, const u8 *src, u8 *dst)
{
    int i;

    for (i = 0; i < row->n; i++) {
        const xn_scaled_col *col = &row->col[i];
        u8 c = src[col->src];
        u32 k;

        if (c == 0 || c == 0xFF)
            continue;
        if (row->remap)
            c = color_remap[c];
        for (k = 0; k < col->count; k++)
            dst[col->x + k] = c;
    }
}

void xn_draw_image_scaled(s32 x, s32 y, s32 w, s32 h, s32 src_w, s32 src_h, s32 flags,
                          const u8 *src)
{
    xn_scaled_col cols[SCALED_COLS];
    xn_scaled_row row;
    s16 q, rem;
    u16 acc, rows;
    u8 *dst;
    const u8 *src_row;

    if ((s16)w <= 2)
        return;
    xn_draw_scaled_x = x;
    xn_draw_scaled_y = y;
    xn_draw_scaled_w = w;
    xn_draw_scaled_h = h;
    xn_draw_scaled_src_w = src_w;
    xn_draw_scaled_src_h = src_h;
    xn_draw_scaled_flags = flags;
    xn_draw_scaled_src = (u8 *)src;
    if (xn_draw_scaled_flags & 8) {
        xn_img_unpack_rows();
        xn_draw_scaled_src = scratch_buffer;
    }
    /* the 8.8 steps: w * 256 / src_w, h * 256 / src_h */
    q = xn_idiv16((u32)(u16)w << 8, (s16)xn_draw_scaled_src_w, &rem);
    xn_draw_scaled_x_int = (u16)q >> 8;
    xn_draw_scaled_x_frac = (u8)q;
    if (xn_draw_scaled_h <= 2)
        return;
    q = xn_idiv16((u32)(u16)xn_draw_scaled_h << 8, (s16)xn_draw_scaled_src_h, &rem);
    xn_draw_scaled_y_int = (u16)q >> 8;
    xn_draw_scaled_y_frac = (u8)q;
    XN_KEEP(xn_draw_scaled_stride, 0x100);
    if (!xn_draw_image_scaled_clip())
        return;
    /* the source rows and columns the clip cut */
    q = xn_idiv16((u32)xn_draw_scaled_skip_y << 8,
                  (s16)((u16)xn_draw_scaled_y_int << 8 | (u8)xn_draw_scaled_y_frac), &rem);
    xn_draw_scaled_src_h -= q;
    if ((s16)xn_draw_scaled_src_h <= 0)
        return;
    xn_draw_scaled_src_ofs = q * xn_draw_scaled_src_w;     /* rows of src_w, not of 100h */
    q = xn_idiv16((u32)xn_draw_scaled_skip_x << 8,
                  (s16)((u16)xn_draw_scaled_x_int << 8 | (u8)xn_draw_scaled_x_frac), &rem);
    xn_draw_scaled_src_w -= q;
    if ((s16)xn_draw_scaled_src_w <= 0)
        return;
    xn_draw_scaled_src_ofs += q;
    XN_KEEP(xn_draw_scaled_bottom, XN_SCREEN(0, xn_gfx_clip_bottom));
    dst = screen_buffer + xn_gfx_row_offset[(u16)xn_draw_scaled_y];
    src_row = xn_draw_scaled_src;
    row.col = cols;
    row.cap = SCALED_COLS;
    xn_draw_image_scaled_compile_row(&row, (u32)h >> 16);
    /* each source row y-step times (the 8.8 accumulator's integer part) */
    rows = xn_draw_scaled_src_h;
    acc = (u16)xn_draw_scaled_y_int << 8 | (u8)xn_draw_scaled_y_frac;
    XN_KEEP(xn_draw_scaled_yint_op, xn_draw_scaled_y_int);
    do {
        u16 n;

        for (n = acc >> 8; n != 0; n--) {
            run_row(&row, src_row, dst);
            dst += 320;
            if (dst >= xn_draw_scaled_bottom)
                return;
        }
        src_row += 0x100;                       /* xn_draw_scaled_stride */
        acc = next_acc(xn_draw_scaled_y_int, acc, xn_draw_scaled_y_frac);
    } while (--rows != 0);
}
