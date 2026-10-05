/* pal.c: XnGine's VGA palette functions as readable C (xpal.h; see xngine.h and
   docs/xngine_readable.md). The DAC takes an index at 3C8h (write) or 3C7h (read), then
   three components at 3C9h; bit 3 of 3DAh is set during the vertical retrace. */
#include "xpal.h"

#define VGA_DAC_READ_INDEX  0x3C7
#define VGA_DAC_WRITE_INDEX 0x3C8
#define VGA_DAC_DATA        0x3C9
#define VGA_STATUS          0x3DA
#define VGA_IN_RETRACE      0x08

/* waits for the start of a vertical retrace: the end of the current one, then the next */
static void wait_retrace_start(void)
{
    while (xn_inb(VGA_STATUS) & VGA_IN_RETRACE)
        ;
    while (!(xn_inb(VGA_STATUS) & VGA_IN_RETRACE))
        ;
}

void xn_pal_set_range_8bit(const u8 *rgb, u16 first, u16 count)
{
    u8 index = (u8)first;
    u32 n = count;

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
    int i;

    for (i = 0; i < 256; i++) {
        xn_outb(VGA_DAC_READ_INDEX, (u8)i);
        *dst++ = xn_inb(VGA_DAC_DATA);
        *dst++ = xn_inb(VGA_DAC_DATA);
        *dst++ = xn_inb(VGA_DAC_DATA);
    }
}

u8 xn_pal_find_nearest(u8 r, u8 g, u8 b)
{
    const u8 *p = xn_pal_current + 3;
    s32 i, d, e;

    xn_pal_best_dist = 0x7FFFFFFF;
    for (i = 1; i != 256; i++, p += 3) {
        e = (s8)(r - p[0]);
        d = e * e * 61;
        e = (s8)(g - p[1]);
        d += e * e * 71;
        e = (s8)(b - p[2]);
        d += e * e * 41;
        if (xn_pal_best_dist > d) {
            xn_pal_best_index = i;
            xn_pal_best_dist = d;
        }
    }
    return (u8)xn_pal_best_index;
}

s32 xn_pal_find_nearest_hsv(u16 h, u16 s, u16 v)
{
    const u8 *p = xn_pal_current;
    xn_pal_hsv c;
    s32 i, d, e;

    xn_pal_best_dist = 0x7FFFFFFF;
    xn_pal_hsv_target.h = h;
    xn_pal_hsv_target.s = s;
    xn_pal_hsv_target.v = v;
    for (i = 0; i != 256; i++, p += 3) {
        xn_pal_rgb_to_hsv(p[0], p[1], p[2], &c);
        e = (s8)(xn_pal_hsv_target.h - c.h);
        d = e * e;
        e = (s8)(xn_pal_hsv_target.s - c.s);
        d += e * e;
        e = (s8)(xn_pal_hsv_target.v - c.v);
        d += e * e;
        if (xn_pal_best_dist > d) {
            xn_pal_best_index = i;
            xn_pal_best_dist = d;
        }
    }
    return xn_pal_best_index;
}

void xn_pal_rgb_to_hsv(u8 r, u8 g, u8 b, xn_pal_hsv *out)
{
    xn_pal_hsv_work *w = &xn_pal_hsv_scratch;
    s16 max, min, h, s;

    w->r = r;
    w->g = g;
    w->b = b;
    max = r;
    if (max < g)
        max = g;
    if (max < b)
        max = b;
    w->max = max;
    min = r;
    if (min > g)
        min = g;
    if (min > b)
        min = b;
    w->min = min;

    s = 0;
    if (max != 0) {
        s = (s16)(((max - min) << 6) / max);
        if (s > 63)
            s = 63;
    }
    h = 0;
    if (s != 0) {
        w->delta = max - min;
        if (w->r == (u16)max)
            h = (s16)(((s16)(w->g - w->b) << 6) / (s16)w->delta);
        else if (w->g == (u16)max)
            h = (s16)(((s16)(w->b - w->r) << 6) / (s16)w->delta + 0x80);
        else
            h = (s16)(((s16)(w->r - w->g) << 6) / (s16)w->delta + 0x100);
        if (h < 0)
            h += 0x180;
    }
    out->h = h;
    out->s = s;
    out->v = max;
}

void xn_pal_rgb_to_hsv_r(xn_regs *r)
{
    xn_pal_hsv c;

    xn_pal_rgb_to_hsv((u8)r->ebx, (u8)r->ecx, (u8)r->edx, &c);
    r->ebx = (r->ebx & 0xFFFF0000) | c.h;
    r->ecx = (r->ecx & 0xFFFF0000) | c.s;
    r->edx = (r->edx & 0xFFFF0000) | c.v;
}

void xn_pal_hsv_to_rgb(const xn_pal_hsv *c, xn_pal_rgb16 *out)
{
    xn_pal_hsv_work *w = &xn_pal_hsv_scratch;
    u16 v = c->v, s = c->s;

    if (s == 0) {
        out->r = out->g = out->b = v;           /* (the asm's ret jumps astray here) */
        return;
    }
    w->f = c->h & 0x3F;
    w->p = (u16)((0x3F - s) * v) >> 6;
    w->q = (u16)((0x3F - ((u16)(s * w->f) >> 6)) * v) >> 6;
    w->t = (u16)((0x3F - ((u16)((0x3F - w->f) * s) >> 6)) * v) >> 6;
    switch (c->h >> 6) {                        /* (sectors 0-4: and here) */
    case 0:
        out->r = v;
        out->g = w->t;
        out->b = w->p;
        break;
    case 1:
        out->r = w->q;
        out->g = v;
        out->b = w->p;
        break;
    case 2:
        out->r = w->p;
        out->g = v;
        out->b = w->t;
        break;
    case 3:
        out->r = w->p;
        out->g = w->q;
        out->b = v;
        break;
    case 4:
        out->r = w->t;
        out->g = w->p;
        out->b = v;
        break;
    default:
        out->r = v;
        out->g = w->p;
        out->b = w->q;
        break;
    }
}

void xn_pal_hsv_to_rgb_r(xn_regs *r)
{
    xn_pal_hsv c;
    xn_pal_rgb16 o;

    c.h = (u16)r->ebx;
    c.s = (u16)r->ecx;
    c.v = (u16)r->edx;
    xn_pal_hsv_to_rgb(&c, &o);
    r->ebx = (r->ebx & 0xFFFF0000) | o.r;
    r->ecx = (r->ecx & 0xFFFF0000) | o.g;
    r->edx = (r->edx & 0xFFFF0000) | o.b;
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
    int i;

    for (i = 0; i < 256; i++) {
        xn_outb(VGA_DAC_READ_INDEX, (u8)i);
        *dst++ = xn_inb(VGA_DAC_DATA);
        *dst++ = xn_inb(VGA_DAC_DATA);
        *dst++ = xn_inb(VGA_DAC_DATA);
    }
}

void xn_pal_stub_empty(void)
{
}

void xn_pal_fade_to_black(s32 steps)
{
    xn_pal_fade_to(xn_pal_black, steps);
}

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

        xn_pal_fade_rgb[i] = from;
        xn_pal_fade_step[i] = ((target[i] << 16) - from) / steps;      /* cdq; idiv ebx */
    }
    left = steps;       /* dec ebx; jne: a negative count runs 2^32 - |steps| times */
    do {
        wait_retrace_start();
        for (i = 0; i < 256; i++) {
            s32 *c = &xn_pal_fade_rgb[i * 3];
            const s32 *d = &xn_pal_fade_step[i * 3];

            xn_pal_fade_write_entry((u8)i, c);
            c[0] += d[0];
            c[1] += d[1];
            c[2] += d[2];
        }
    } while (--left != 0);
    xn_pal_set(target);
}

void xn_pal_fade_write_entry(u8 index, const s32 *rgb)
{
    xn_outb(VGA_DAC_WRITE_INDEX, index);
    xn_outb(VGA_DAC_DATA, (u8)((u32)rgb[0] >> 16));
    xn_outb(VGA_DAC_DATA, (u8)((u32)rgb[1] >> 16));
    xn_outb(VGA_DAC_DATA, (u8)((u32)rgb[2] >> 16));
}

void xn_pal_build_ega16_map(void)
{
    u8 *saved = xn_pal_current;
    const u8 *ega = xn_pal_ega16_rgb;
    int i;

    xn_pal_current = big_buffer;
    xn_pal_get(big_buffer);
    for (i = 0; i < 16; i++, ega += 3)
        xn_pal_ega16_map[i] = xn_pal_find_nearest(ega[0], ega[1], ega[2]);
    xn_pal_current = saved;
}
