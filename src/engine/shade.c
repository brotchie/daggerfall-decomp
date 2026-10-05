/* shade.c: XnGine's shade functions as readable C (xshade.h; see xngine.h and
   docs/xngine_readable.md): the shade table's builders and the distance fog. */
#include "xshade.h"
#include "xnsmc.h"

/* the asm functions called here (other groups'; their asm entries, routed or not) */
void asm_xn_str_from_int(s32 v, char *dst, s32 digits);    /* digits of v at dst */
#pragma aux asm_xn_str_from_int parm [eax] [edx] [ebx] modify exact [eax edx];
/* the file `name` read whole into buf; returns buf (on an error it exits to DOS) */
u8 *asm_xn_dos_load_file(const char *name, u8 *buf);
#pragma aux asm_xn_dos_load_file parm [eax] [edx] value [eax] modify exact [eax edx];
void *func_000A10A8(u32 size);                              /* the game's malloc */
#define xn_game_malloc func_000A10A8

extern void asm_xn_shade_fog_span(void);
extern void asm_xn_shade_fog_span_off(void);
extern void asm_xn_shade_fog_pixels(void);
extern char xn_haze_filename[];                             /* "HAZE.nnn" */

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
    u8 *last;

    asm_xn_str_from_int(n, xn_shade_filename + 6, 3);       /* "SHADE.nnn" */
    asm_xn_dos_load_file(xn_shade_filename, xn_shade_table);
    last = xn_shade_table + (SHADE_ROWS - 1) * SHADE_ROW;
    xn_shade_table_last_row = last;
    xn_light_t1_max_a = xn_light_t1_max_b = last;
    xn_light_t2_max_a = xn_light_t2_max_b = last;
    xn_light_t3_max_a = xn_light_t3_max_b = last;
}

/* the row's outputs are FS and GS (unchanged): glue, which keeps every register as the
   asm's pushad does */
void xn_shade_load_r(xn_regs *r)
{
    xn_shade_load(r->eax);
}

void xn_shade_load_haze(s32 n)
{
    u8 *buf;

    asm_xn_str_from_int(n, xn_haze_filename + 5, 3);        /* "HAZE.nnn" */
    buf = (u8 *)(((u32)xn_game_malloc(0x4100) + 0xFF) & ~0xFFu);
    xn_fog_table_last = asm_xn_dos_load_file(xn_haze_filename, buf) + (SHADE_ROWS - 1) * SHADE_ROW;
}

/* ---- the fog ------------------------------------------------------------------------------ */

void xn_shade_set_fog(s32 start)
{
    u32 step, inv_start;

    if (start != -1) {
        xn_fog_start = start;
        if ((s32)(xn_cam_far_z >> 8) > start) {
            xn_fog_table_last_a = (u32)xn_fog_table_last;
            xn_fog_table_last_b = xn_fog_table_last;
            step = xn_udiv64(0, 0x3F00, (xn_cam_far_z >> 8) - start);
            xn_fog_step_a = xn_fog_step_b = xn_fog_step_c = xn_fog_step = step;
            inv_start = xn_udiv64(1, 0, start);             /* start 0: a divide error, 0 */
            xn_fog_inv_start_a = xn_fog_inv_start_b = xn_fog_inv_start_c = inv_start;
            xn_fog_start_step_a = xn_fog_start_step_b = start * step;
            xn_fog_min_inv_z = xn_udiv64(0x100, 0, xn_cam_far_z);
            xn_render_span_hook = XN_ASM(xn_shade_fog_span);
            return;
        }
    }
    xn_render_span_hook = XN_ASM(xn_shade_fog_span_off);
    xn_fog_start = xn_cam_far_z;                            /* not >> 8 (as the asm) */
}

void xn_shade_fog_span_off(void)
{
}

/* n pixels remapped through the fog table: row is a level's address with an 8-bit fraction
   in its low byte, which the pixel replaces; it moves by step a pixel. The asm unrolls this
   641 times (14D320) and stops it with a planted ret. */
static void fog_run(u8 *pix, int n, u32 row, s32 step)
{
    int k;

    for (k = 0; k < n; k++) {
        pix[k] = *(const u8 *)((row & ~0xFFu) | pix[k]);
        row += step;
    }
}

/* The asm body's own records: ecx = row, edx = step, edi = the first pixel - 100h; the count
   is where the planter put its ret. EAX and ECX come back as the asm leaves them: the last
   lookup's address with the pixel, and the row after the last step. */
void xn_shade_fog_pixels_r(xn_regs *r)
{
    u8 *pix = (u8 *)r->edi + 0x100;
    int n = xn_planted_count(asm_xn_shade_fog_pixels, 18, 641);

    fog_run(pix, n, r->ecx, r->edx);
    if (n > 0)
        r->eax = ((r->ecx + (n - 1) * r->edx) & ~0xFFu) | pix[n - 1];
    r->ecx += n * r->edx;
}

void xn_shade_fog_span(const struct xn_poly *poly, const struct xn_span *span, s32 n, u8 *pix)
{
    u32 inv_z = span->inv_z;
    s32 slope = poly->dx_per_inv_z;             /* pixels per unit of 1/z, 2^32 / d(1/z)/dx */
    u32 level0, level1, inv_end;
    s32 h;

    if (slope == 0) {                           /* the same depth along the span */
        if ((s32)inv_z >= (s32)xn_fog_inv_start_c)
            return;                             /* nearer than the fog start */
        level0 = xn_udiv64(xn_fog_step_c, 0, inv_z);
        fog_run(pix, n, (u32)xn_fog_table_last_b + (xn_fog_start_step_b - level0), 0);
        return;
    }
    if ((s32)inv_z > (s32)xn_fog_inv_start_a) { /* the span starts nearer than the fog */
        if (slope >= 0)
            return;                             /* ... and does not go deeper */
        h = xn_mulhi(slope, inv_z - xn_fog_inv_start_a);     /* -(pixels before the fog) */
        if (xn_add_lt0(n, h) || n + h == 0)
            return;                             /* n + h <= 0, unwrapped (add; jle) */
        n += h;
        pix -= h;
        inv_z = xn_fog_inv_start_b;
    } else if (slope > 0) {                     /* in the fog, and leaving it */
        h = -xn_mulhi(slope, inv_z - xn_fog_inv_start_a);
        if (n > h)
            n = h;
    }
    if ((s32)inv_z < (s32)xn_fog_min_inv_z)
        inv_z = xn_fog_min_inv_z;
    level0 = xn_udiv64(xn_fog_step_a, 0, inv_z);
    inv_end = poly->inv_z_dx * n + inv_z;
    if ((s32)inv_end < (s32)xn_fog_min_inv_z)
        inv_end = xn_fog_min_inv_z;
    level1 = xn_udiv64(xn_fog_step_b, 0, inv_end);
    /* the level's step is (level1 - level0) / n through the reciprocal table; n is at most a
       screen row, so xn_fog_unroll_offsets[n] (640 entries) always has the asm's ret offset */
    fog_run(pix, n, xn_fog_table_last_a + (xn_fog_start_step_a - level0),
            -xn_mulhi((s32)(level1 - level0), xn_recip32_table[n]));
}

/* asm: ebx = poly, esi = span node, ebp = n, edi = the first pixel - 1 */
void xn_shade_fog_span_r(xn_regs *r)
{
    xn_shade_fog_span((const struct xn_poly *)r->ebx, (const struct xn_span *)r->esi, r->ebp,
                      (u8 *)r->edi + 1);
}
