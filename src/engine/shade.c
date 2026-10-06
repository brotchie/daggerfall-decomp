/* shade.c: XnGine's shade table and distance fog (xshade.h). */
#include "xshade.h"
#include "xnsmc.h"
#include "xrframe.h"
#include "xstr.h"
#include "xdos.h"
#include "xmem.h"

#define SHADE_ROWS      64
#define SHADE_ROW       256

u8 *xn_shade_table_63(void)
{
    return xn_shade_table + (SHADE_ROWS - 1) * SHADE_ROW;
}

void xn_shade_init_reserved(void)
{
    u8 *row;
    int k, c;

    for (row = xn_shade_table, k = 0; k < SHADE_ROWS; k++, row += SHADE_ROW) {
        for (c = 0; c < 32; c++)
            row[c] = 0xFF;
        row[255] = 0xFF;
        row[0] = 0;
    }
    for (row = haze_table_1, k = 0; k < SHADE_ROWS; k++, row += SHADE_ROW)
        row[255] = 0x77;
}

void xn_shade_keep_colours_0_255(void)
{
    u8 *row;
    int k;

    for (row = xn_shade_table, k = 0; k < SHADE_ROWS; k++, row += SHADE_ROW) {
        row[255] = 0xFF;
        row[0] = 0;
    }
}

void xn_shade_build_translucent_table(u8 *table)
{
    const u8 *src;
    int c, k;

    for (c = 0; c < SHADE_ROW; c++)
        *table++ = (u8)c;
    for (src = xn_shade_table + 50 * SHADE_ROW, k = 0; k < 13; k++, src -= 2 * SHADE_ROW)
        for (c = 0; c < SHADE_ROW; c++)
            *table++ = src[c];
    for (c = 0; c < 2 * SHADE_ROW; c++)
        *table++ = 0xF5;
}

void xn_shade_load(s32 n)
{
    xn_str_from_int(n, xn_shade_filename + 6, 3);           /* "SHADE.nnn" */
    xn_dos_load_file(xn_shade_filename, xn_shade_table);
    xn_shade_table_last_row = xn_shade_table + (SHADE_ROWS - 1) * SHADE_ROW;
}

void xn_shade_load_haze(s32 n)
{
    u8 *buf;

    xn_str_from_int(n, xn_haze_filename + 5, 3);            /* "HAZE.nnn" */
    buf = (u8 *)(((uptr)func_000A10A8(0x4100) + 0xFF) & ~(uptr)0xFF);
    xn_fog_table_last = (u8 *)xn_dos_load_file(xn_haze_filename, buf) +
                        (SHADE_ROWS - 1) * SHADE_ROW;
}

/* ---- the fog ------------------------------------------------------------------------------ */

void xn_shade_set_fog(s32 start)
{
    if (start != -1) {
        u32 range = (u32)xn_cam_far_z >> 8;     /* the view distance, z >> 8 */

        xn_fog_start = start;
        if ((s32)range > start) {
            xn_fog_step = xn_udiv64_or0(0, 0x3F00, range - start);
            xn_fog_min_inv_z = xn_udiv64_or0(0x100, 0, xn_cam_far_z);
            xn_render_span_hook = xn_shade_fog_span;
            return;
        }
    }
    xn_render_span_hook = xn_shade_fog_span_off;
    xn_fog_start = xn_cam_far_z;        /* Quirk Q-SHADE-01: not >> 8 */
}

void xn_shade_fog_span_off(const struct xn_poly *poly, const struct xn_span *span, s32 n,
                           u8 *pix)
{
    (void)poly;
    (void)span;
    (void)n;
    (void)pix;
}

void xn_shade_fog_pixels(u8 *pix, s32 n, uptr row, s32 step)
{
    s32 k;

    for (k = 0; k < n; k++) {
        pix[k] = *(const u8 *)((row & ~(uptr)0xFF) | pix[k]);
        row += step;
    }
}

/* The fog row of depth inv_z: the fog table's level step * 2^32 / inv_z - start * step
   bytes before its last level, plus that position's fraction (the low byte, which the pixel
   replaces). */
static uptr fog_row(u32 inv_z)
{
    return (uptr)xn_fog_table_last + (u32)xn_fog_start * xn_fog_step -
           xn_udiv64_or0(xn_fog_step, 0, inv_z);
}

void xn_shade_fog_span(const struct xn_poly *poly, const struct xn_span *span, s32 n, u8 *pix)
{
    u32 inv_z = span->inv_z;
    s32 slope = poly->dx_per_inv_z;             /* pixels a unit of 1/z: 2^32 / d(1/z)/dx */
    /* the fog start's 1/z, 2^32 / start (Quirk Q-SHADE-02: 0 for a start of 0 or 1) */
    u32 inv_start = xn_udiv64_or0(1, 0, (u32)xn_fog_start);
    u32 inv_end;
    s32 h;

    if (slope == 0) {                           /* the same depth along the span */
        if ((s32)inv_z >= (s32)inv_start)
            return;                             /* nearer than the fog start */
        xn_shade_fog_pixels(pix, n, fog_row(inv_z), 0);
        return;
    }
    if ((s32)inv_z > (s32)inv_start) {          /* the span starts nearer than the fog */
        if (slope >= 0)
            return;                             /* ... and does not go deeper */
        h = xn_mulhi(slope, inv_z - inv_start); /* -(the pixels before the fog) */
        /* Quirk Q-SHADE-03: n + h <= 0 as the asm's add; jle (unwrapped) */
        if (xn_add_lt0(n, h) || n + h == 0)
            return;
        n += h;
        pix -= h;
        inv_z = inv_start;
    } else if (slope > 0) {                     /* in the fog, and leaving it */
        h = -xn_mulhi(slope, inv_z - inv_start);
        if (n > h)
            n = h;
    }
    if ((s32)inv_z < (s32)xn_fog_min_inv_z)
        inv_z = xn_fog_min_inv_z;
    inv_end = poly->inv_z_dx * n + inv_z;
    if ((s32)inv_end < (s32)xn_fog_min_inv_z)
        inv_end = xn_fog_min_inv_z;
    /* the row steps by the levels' difference / n a pixel (through FFFFFFFFh / n; n is at
       most a screen row) */
    xn_shade_fog_pixels(pix, n, fog_row(inv_z),
                        -xn_mulhi((s32)(xn_udiv64_or0(xn_fog_step, 0, inv_end) -
                                        xn_udiv64_or0(xn_fog_step, 0, inv_z)),
                                  xn_recip32_table[n]));
}
