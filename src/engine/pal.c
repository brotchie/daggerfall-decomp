/* pal.c: XnGine's VGA palette (canonical C; the interface and the module's documentation are in
   xpal.h). The DAC takes an index at 3C8h (write) or 3C7h (read), then three components at 3C9h;
   bit 3 of 3DAh is set during the vertical retrace. */
#include "xpal.h"

#define VGA_DAC_READ_INDEX  0x3C7
#define VGA_DAC_WRITE_INDEX 0x3C8
#define VGA_DAC_DATA        0x3C9
#define VGA_STATUS          0x3DA
#define VGA_IN_RETRACE      0x08

/* A fade's 16.16 components and their per-retrace steps (xn_pal_fade_to's working arrays) */
static s32 fade_rgb[768];
static s32 fade_step[768];

/* waits for the start of a vertical retrace: the end of the current one, then the next */
static void wait_retrace_start(void)
{
    while (xn_inb(VGA_STATUS) & VGA_IN_RETRACE)
        ;
    while (!(xn_inb(VGA_STATUS) & VGA_IN_RETRACE))
        ;
}

/* the 256 DAC entries into dst */
static void read_dac(u8 *dst)
{
    int i;

    for (i = 0; i < 256; i++) {
        xn_outb(VGA_DAC_READ_INDEX, (u8)i);
        *dst++ = xn_inb(VGA_DAC_DATA);
        *dst++ = xn_inb(VGA_DAC_DATA);
        *dst++ = xn_inb(VGA_DAC_DATA);
    }
}

/* ---- the DAC --------------------------------------------------------------------------------- */

void xn_pal_set_range_8bit(const u8 *rgb, u16 first, u16 count)
{
    u8 index = (u8)first;
    u32 n = count;                      /* Quirk Q-PAL-01: 0 runs 2^32 times */

    do {
        xn_outb(VGA_DAC_WRITE_INDEX, index++);
        xn_outb(VGA_DAC_DATA, *rgb++ >> 2);
        xn_outb(VGA_DAC_DATA, *rgb++ >> 2);
        xn_outb(VGA_DAC_DATA, *rgb++ >> 2);
    } while (--n != 0);
}

void xn_pal_set_all_8bit(u8 *pal)
{
    int i;

    for (i = 0; i < 768; i++)
        pal[i] >>= 2;
    xn_pal_set(pal);
}

void xn_pal_read_dac(u8 *dst)
{
    read_dac(dst);
}

void xn_pal_set(const u8 *pal)
{
    int i;

    xn_pal_current = (u8 *)pal;
    wait_retrace_start();
    for (i = 0; i < 256; i++) {
        xn_outb(VGA_DAC_WRITE_INDEX, (u8)i);
        xn_outb(VGA_DAC_DATA, *pal++);
        xn_outb(VGA_DAC_DATA, *pal++);
        xn_outb(VGA_DAC_DATA, *pal++);
    }
}

void xn_pal_get(u8 *dst)
{
    read_dac(dst);
}

void xn_pal_stub_empty(void)
{
}

/* ---- fades ---------------------------------------------------------------------------------- */

void xn_pal_fade_to(const u8 *target, s32 steps)
{
    const u8 *cur;
    u32 left;
    int i;

    if (steps == 0)
        return;
    cur = xn_pal_current;
    for (i = 0; i < 768; i++) {
        s32 from = cur[i] << 16;

        fade_rgb[i] = from;
        fade_step[i] = ((target[i] << 16) - from) / steps;     /* |quotient| <= 63 << 16 */
    }
    left = steps;                       /* Quirk Q-PAL-04: a negative count runs on */
    do {
        wait_retrace_start();
        for (i = 0; i < 256; i++) {
            s32 *c = &fade_rgb[i * 3];
            const s32 *d = &fade_step[i * 3];

            xn_pal_fade_write_entry((u8)i, c);
            c[0] += d[0];
            c[1] += d[1];
            c[2] += d[2];
        }
    } while (--left != 0);
    xn_pal_set(target);
}

void xn_pal_fade_to_black(s32 steps)
{
    xn_pal_fade_to(xn_pal_black, steps);
}

void xn_pal_fade_write_entry(u8 index, const s32 *rgb)
{
    xn_outb(VGA_DAC_WRITE_INDEX, index);
    xn_outb(VGA_DAC_DATA, (u8)((u32)rgb[0] >> 16));
    xn_outb(VGA_DAC_DATA, (u8)((u32)rgb[1] >> 16));
    xn_outb(VGA_DAC_DATA, (u8)((u32)rgb[2] >> 16));
}

/* ---- nearest colours ------------------------------------------------------------------------- */

/* the index 1..255 of pal nearest to (r, g, b) (xn_pal_find_nearest) */
static u8 nearest_in(const u8 *pal, u8 r, u8 g, u8 b)
{
    const u8 *p = pal + 3;
    s32 best = 0x7FFFFFFF, best_index = 0, i, d, e;

    for (i = 1; i != 256; i++, p += 3) {   /* Quirk Q-PAL-02: from index 1 */
        e = (s8)(r - p[0]);                 /* Quirk Q-PAL-03: as signed bytes */
        d = e * e * 61;
        e = (s8)(g - p[1]);
        d += e * e * 71;
        e = (s8)(b - p[2]);
        d += e * e * 41;
        if (best > d) {
            best_index = i;
            best = d;
        }
    }
    return (u8)best_index;
}

u8 xn_pal_find_nearest(u8 r, u8 g, u8 b)
{
    return nearest_in(xn_pal_current, r, g, b);
}

s32 xn_pal_find_nearest_hsv(u16 h, u16 s, u16 v)
{
    const u8 *p = xn_pal_current;
    xn_pal_hsv c;
    s32 best = 0x7FFFFFFF, best_index = 0, i, d, e;

    for (i = 0; i != 256; i++, p += 3) {
        xn_pal_rgb_to_hsv(p[0], p[1], p[2], &c);
        e = (s8)(h - c.h);                  /* Quirk Q-PAL-03 */
        d = e * e;
        e = (s8)(s - c.s);
        d += e * e;
        e = (s8)(v - c.v);
        d += e * e;
        if (best > d) {
            best_index = i;
            best = d;
        }
    }
    return best_index;
}

void xn_pal_rgb_to_hsv(u8 r, u8 g, u8 b, xn_pal_hsv *out)
{
    s16 max, min, delta, h, s;

    max = r;
    if (max < g)
        max = g;
    if (max < b)
        max = b;
    min = r;
    if (min > g)
        min = g;
    if (min > b)
        min = b;
    s = 0;
    if (max != 0) {
        s = (s16)(((max - min) << 6) / max);
        if (s > 63)
            s = 63;
    }
    h = 0;
    if (s != 0) {
        delta = max - min;
        if (r == max)
            h = (s16)((s16)((g - b) << 6) / delta);
        else if (g == max)
            h = (s16)((s16)((b - r) << 6) / delta + 0x80);
        else
            h = (s16)((s16)((r - g) << 6) / delta + 0x100);
        if (h < 0)
            h += 0x180;
    }
    out->h = h;
    out->s = s;
    out->v = max;
}

void xn_pal_hsv_to_rgb(const xn_pal_hsv *c, xn_pal_rgb16 *out)
{
    u16 v = c->v, s = c->s, f, p, q, t;

    if (s == 0) {
        out->r = out->g = out->b = v;       /* Quirk Q-PAL-05: the asm's ret jumps astray */
        return;
    }
    f = c->h & 0x3F;
    p = (u16)((0x3F - s) * v) >> 6;
    q = (u16)((0x3F - ((u16)(s * f) >> 6)) * v) >> 6;
    t = (u16)((0x3F - ((u16)((0x3F - f) * s) >> 6)) * v) >> 6;
    switch (c->h >> 6) {                    /* (sectors 0-4: Q-PAL-05 too) */
    case 0:
        out->r = v;
        out->g = t;
        out->b = p;
        break;
    case 1:
        out->r = q;
        out->g = v;
        out->b = p;
        break;
    case 2:
        out->r = p;
        out->g = v;
        out->b = t;
        break;
    case 3:
        out->r = p;
        out->g = q;
        out->b = v;
        break;
    case 4:
        out->r = t;
        out->g = p;
        out->b = v;
        break;
    default:
        out->r = v;
        out->g = p;
        out->b = q;
        break;
    }
}

void xn_pal_build_ega16_map(void)
{
    const u8 *ega = xn_pal_ega16_rgb;
    int i;

    read_dac(big_buffer);
    for (i = 0; i < 16; i++, ega += 3)
        xn_pal_ega16_map[i] = nearest_in(big_buffer, ega[0], ega[1], ega[2]);
}
