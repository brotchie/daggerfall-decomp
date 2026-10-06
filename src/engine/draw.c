/* draw.c: XnGine's rectangle copies and image blits (canonical C; the interface and the
   module's documentation are in xdraw.h): the clip of a screen rectangle and the copies between
   the screen and a buffer built on it. */
#include "xdraw.h"

static void set_blit(xn_blit *b, s32 x, s32 y, s32 w, s32 h, const u8 *buf, s32 skip)
{
    b->x = x;
    b->y = y;
    b->w = w;
    b->h = h;
    b->skip = skip;
    b->buf = (u8 *)buf;
}

int xn_add_le0(s32 a, s32 b)
{
    s32 s = (s32)((u32)a + (u32)b);

    if ((a < 0) == (b < 0) && (s < 0) != (a < 0))
        return a < 0;                   /* overflowed: the true sum has a's sign */
    return s <= 0;
}

void xn_draw_fill_rect(s32 x, s32 y, s32 w, s32 h)
{
    u8 *row = XN_SCREEN(x, y);
    u32 rows = h;                       /* Quirk Q-DRAW-06: 0 runs 2^32 times */

    do {
        xn_fill_bytes(row, text_colour, w);
        row += xn_gfx_width;
    } while (--rows != 0);
}

int xn_draw_clip_rect(xn_blit *b)
{
    s32 end, cut;

    if (b->w <= 0 || b->h <= 0 || b->x >= xn_gfx_clip_right || b->y >= xn_gfx_clip_bottom)
        return 0;
    if (b->y < xn_gfx_clip_top) {
        b->y -= xn_gfx_clip_top;                /* minus the rows above the window */
        cut = b->h;
        b->h += b->y;
        if (xn_add_le0(cut, b->y))
            return 0;                           /* Quirk Q-DRAW-04: y stays y - clip_top */
        b->buf -= b->y * (b->w + b->skip);
        b->y = xn_gfx_clip_top;
    }
    if (b->x < xn_gfx_clip_left) {
        b->x -= xn_gfx_clip_left;               /* minus the columns left of it */
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

int xn_draw_get_blit(xn_blit *b)
{
    const u8 *row;
    u8 *dst;
    u32 rows;

    if (!xn_draw_clip_rect(b))
        return 0;
    row = XN_SCREEN(b->x, b->y);
    dst = b->buf;
    rows = b->h;
    do {
        xn_copy_bytes(dst, row, b->w);
        dst += b->w + b->skip;
        row += xn_gfx_width;
    } while (--rows != 0);
    return 1;
}

int xn_draw_put_blit(xn_blit *b)
{
    const u8 *src;
    u8 *row;
    u32 rows;

    if (!xn_draw_clip_rect(b))
        return 0;
    row = XN_SCREEN(b->x, b->y);
    src = b->buf;
    rows = b->h;
    do {
        xn_copy_bytes(row, src, b->w);
        src += b->w + b->skip;
        row += xn_gfx_width;
    } while (--rows != 0);
    return 1;
}

int xn_draw_put_blit_transparent(xn_blit *b)
{
    const u8 *src;
    u8 *row;
    u32 rows;

    if (!xn_draw_clip_rect(b))
        return 0;
    row = XN_SCREEN(b->x, b->y);
    src = b->buf;
    rows = b->h;
    do {
        xn_draw_transparent_row(row, src, b->w);
        src += b->w + b->skip;
        row += xn_gfx_width;
    } while (--rows != 0);
    return 1;
}

void xn_draw_get_rect(s32 x, s32 y, s32 w, s32 h, u8 *dst, s32 dst_skip)
{
    xn_blit b;

    set_blit(&b, x, y, w, h, dst, dst_skip);
    xn_draw_get_blit(&b);
}

/* The game's calls of xn_draw_get_rect (boundary adapter, Quirk Q-DRAW-02): the asm leaves EAX
   = the screen's row step less the width after a copy, else the clip's x; the game reads its
   upper bytes. */
void xn_draw_get_rect_b(xn_regs *r)
{
    xn_blit b;
    int drawn;

    set_blit(&b, r->eax, r->edx, r->ebx, r->ecx, (u8 *)XN_STACK_ARG(r, 0), XN_STACK_ARG(r, 1));
    drawn = xn_draw_get_blit(&b);
    r->eax = drawn ? xn_gfx_width - b.w : b.x;
}

void xn_draw_put_rect(s32 x, s32 y, s32 w, s32 h, const u8 *src, s32 src_skip)
{
    xn_blit b;

    set_blit(&b, x, y, w, h, src, src_skip);
    xn_draw_put_blit(&b);
}

void xn_draw_image(s32 x, s32 y, s32 w, s32 h, const u8 *pixels)
{
    xn_blit b;

    set_blit(&b, x, y, w, h, pixels, 0);
    xn_draw_put_blit(&b);
}

void xn_draw_image_regs_out(const xn_blit *b, int drawn, xn_regs *r)
{
    if (!drawn) {
        r->eax = b->x;
        r->edx = b->y;
        r->ebx = b->w;
        r->ecx = b->h;
        return;
    }
    r->eax = xn_gfx_width - b->w;
    r->ecx = 0;
    r->edx = 0;
    r->ebx = b->w;
}

/* The game's calls of xn_draw_image (boundary adapter, Quirk Q-DRAW-03): the asm's registers */
void xn_draw_image_b(xn_regs *r)
{
    xn_blit b;

    set_blit(&b, r->eax, r->edx, r->ebx, r->ecx, (const u8 *)XN_STACK_ARG(r, 0), 0);
    xn_draw_image_regs_out(&b, xn_draw_put_blit(&b), r);
}

void xn_draw_image_transparent(s32 x, s32 y, s32 w, s32 h, const u8 *pixels)
{
    xn_blit b;

    set_blit(&b, x, y, w, h, pixels, 0);
    xn_draw_put_blit_transparent(&b);
}

void xn_draw_image_transparent_regs_out(const xn_blit *b, int drawn, xn_regs *r)
{
    const u8 *last;

    if (!drawn) {
        r->eax = b->x;
        r->edx = b->y;
        r->ebx = b->w;
        r->ecx = b->h;
        return;
    }
    /* the last pixel the last row loaded: AL, over the clipped x */
    last = b->buf + (b->h - 1) * (b->w + b->skip) + b->w - 1;
    r->eax = (b->x & ~0xFF) | *last;
    r->ecx = 0;
    r->edx = xn_gfx_width;
    r->ebx = b->w << 4;
}

/* The game's calls of xn_draw_image_transparent (boundary adapter, Quirk Q-DRAW-03) */
void xn_draw_image_transparent_b(xn_regs *r)
{
    xn_blit b;

    set_blit(&b, r->eax, r->edx, r->ebx, r->ecx, (const u8 *)XN_STACK_ARG(r, 0), 0);
    xn_draw_image_transparent_regs_out(&b, xn_draw_put_blit_transparent(&b), r);
}

void xn_draw_img_record(s32 x, s32 y, const xn_img *img)
{
    xn_draw_image(x, y, img->width, img->height, img->pixels);
}

void xn_draw_img_record_transparent(s32 x, s32 y, const xn_img *img)
{
    xn_draw_image_transparent(x, y, img->width, img->height, img->pixels);
}

void xn_draw_transparent_row(u8 *dst, const u8 *src, u32 n)
{
    u32 k;

    for (k = 0; k < n; k++)
        if (src[k] != 0)
            dst[k] = src[k];
}

void xn_draw_unused_ret_144f47(void)
{
}

void xn_draw_unused_ret_144fc7(void)
{
}
