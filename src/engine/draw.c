/* draw.c: XnGine's rectangle blits and fills as readable C (xdraw.h; see xngine.h and
   docs/xngine_readable.md): the clip routine 144E00 and the copies between the screen and a
   buffer built on it. */
#include "xdraw.h"

/* the unrolled transparent row, as code bytes (its planter's ret is looked for) */
extern u8 asm_xn_draw_transparent_row[];

s16 xn_idiv16(s32 dividend, s32 d, s16 *rem)
{
    xn_s64 n;
    s32 q, r;

    xn_s64_set(&n, dividend);
    q = xn_s64_divrem(&n, (s16)d, &r);
    if (q != (s16)q)
        q = xn_s64_divrem(&n, 0, &r);   /* the 16-bit quotient overflows: the divide error */
    *rem = (s16)r;
    return (s16)q;
}

/* a rectangle from the registers of an asm caller, and back */
static void regs_to_blit(const xn_regs *r, xn_blit *b)
{
    b->x = r->eax;
    b->y = r->edx;
    b->w = r->ebx;
    b->h = r->ecx;
    b->skip = r->ebp;
    b->buf = (u8 *)r->esi;
}

static void blit_to_regs(const xn_blit *b, xn_regs *r)
{
    r->eax = b->x;
    r->edx = b->y;
    r->ebx = b->w;
    r->ecx = b->h;
}

static void set_blit(xn_blit *b, s32 x, s32 y, s32 w, s32 h, const u8 *buf, s32 skip)
{
    b->x = x;
    b->y = y;
    b->w = w;
    b->h = h;
    b->skip = skip;
    b->buf = (u8 *)buf;
}

void xn_draw_fill_rect(s32 x, s32 y, s32 w, s32 h)
{
    u8 *row = XN_SCREEN(x, y);
    u32 rows = h;

    do {
        xn_fill_bytes(row, text_colour, w);
        row += xn_gfx_width;
    } while (--rows != 0);
}

int xn_draw_clip_rect(xn_blit *b)
{
    s32 end;

    if (b->w <= 0 || b->h <= 0 || b->x >= xn_gfx_clip_right || b->y >= xn_gfx_clip_bottom)
        return 0;
    if (b->y < xn_gfx_clip_top) {
        b->y -= xn_gfx_clip_top;                /* minus the rows above the window */
        b->h += b->y;
        if (b->h <= 0)
            return 0;
        b->buf -= b->y * (b->w + b->skip);
        b->y = xn_gfx_clip_top;
    }
    if (b->x < xn_gfx_clip_left) {
        b->x -= xn_gfx_clip_left;               /* minus the columns left of it */
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

void xn_draw_clip_rect_r(xn_regs *r)
{
    xn_blit b;
    int ok;

    regs_to_blit(r, &b);
    ok = xn_draw_clip_rect(&b);
    blit_to_regs(&b, r);
    r->ebp = b.skip;
    r->esi = (u32)b.buf;
    XN_SETFLAG(r, XN_CF, !ok);
}

/* the screen rectangle b into b's buffer; 0 when nothing was left after the clip */
static int get_rect(xn_blit *b)
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

void xn_draw_get_rect(s32 x, s32 y, s32 w, s32 h, u8 *dst, s32 dst_skip)
{
    xn_blit b;

    set_blit(&b, x, y, w, h, dst, dst_skip);
    get_rect(&b);
}

/* EAX as 144E9C leaves it (one game caller reads its upper bits): after a copy the screen's
   row step, else the clip's x */
static void get_rect_eax(const xn_blit *b, int drawn, xn_regs *r)
{
    r->eax = drawn ? xn_gfx_width - b->w : b->x;
}

void xn_draw_get_rect_r(xn_regs *r)
{
    xn_blit b;

    set_blit(&b, r->eax, r->edx, r->ebx, r->ecx, (u8 *)XN_STACK_ARG(r, 0), XN_STACK_ARG(r, 1));
    get_rect_eax(&b, get_rect(&b), r);
}

void xn_draw_get_rect_regs_r(xn_regs *r)
{
    xn_blit b;

    set_blit(&b, r->eax, r->edx, r->ebx, r->ecx, (u8 *)r->edi, r->ebp);
    get_rect_eax(&b, get_rect(&b), r);
}

void xn_draw_put_rect(s32 x, s32 y, s32 w, s32 h, const u8 *src, s32 src_skip)
{
    xn_blit b;
    u8 *row;

    set_blit(&b, x, y, w, h, src, src_skip);
    if (!xn_draw_clip_rect(&b))
        return;
    row = XN_SCREEN(b.x, b.y);
    do {
        xn_copy_bytes(row, b.buf, b.w);
        b.buf += b.w + b.skip;
        row += xn_gfx_width;
    } while (--b.h != 0);
}

void xn_draw_put_rect_regs_r(xn_regs *r)
{
    xn_draw_put_rect(r->eax, r->edx, r->ebx, r->ecx, (const u8 *)r->esi, r->ebp);
}

int xn_draw_image_regs(xn_blit *b)
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

/* the registers 144F7C leaves its callers: after a blit, its row step, the counts (0) and the
   width; else the clip's */
static void image_regs_out(const xn_blit *b, int drawn, xn_regs *r)
{
    if (!drawn) {
        blit_to_regs(b, r);
        return;
    }
    r->eax = xn_gfx_width - b->w;
    r->ecx = 0;
    r->edx = 0;
    r->ebx = b->w;
}

void xn_draw_image_regs_r(xn_regs *r)
{
    xn_blit b;

    set_blit(&b, r->eax, r->edx, r->ebx, r->ecx, (const u8 *)r->esi, 0);
    image_regs_out(&b, xn_draw_image_regs(&b), r);
}

void xn_draw_image(s32 x, s32 y, s32 w, s32 h, const u8 *pixels)
{
    xn_blit b;

    set_blit(&b, x, y, w, h, pixels, 0);
    xn_draw_image_regs(&b);
}

void xn_draw_image_r(xn_regs *r)
{
    xn_blit b;

    set_blit(&b, r->eax, r->edx, r->ebx, r->ecx, (const u8 *)XN_STACK_ARG(r, 0), 0);
    image_regs_out(&b, xn_draw_image_regs(&b), r);
}

void xn_draw_img_record(s32 x, s32 y, const xn_img *img)
{
    xn_draw_image(x, y, img->width, img->height, img->pixels);
}

void xn_draw_img_record_regs_r(xn_regs *r)
{
    const xn_img *img = (const xn_img *)r->ebx;
    xn_blit b;

    set_blit(&b, r->eax, r->edx, img->width, img->height, img->pixels, 0);
    image_regs_out(&b, xn_draw_image_regs(&b), r);
}

void xn_draw_img_record_r(xn_regs *r)
{
    u32 ecx = r->ecx;

    xn_draw_img_record_regs_r(r);
    r->ecx = ecx;                               /* 144F28 keeps ECX EBP ESI EDI */
}

void xn_draw_unused_ret_144f47(void)
{
}

/* the row: each non-zero pixel of src[0..n) to dst */
static void transparent_run(u8 *dst, const u8 *src, u32 n)
{
    u32 k;

    for (k = 0; k < n; k++)
        if (src[k] != 0)
            dst[k] = src[k];
}

/* (The asm's unrolled row has 640 steps; a w over 640 would plant its ret past it. Clipped
   widths are at most the clip window's, 320.) */
int xn_draw_image_transparent_regs(xn_blit *b)
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
        transparent_run(row, src, b->w);
        src += b->w + b->skip;
        row += xn_gfx_width;
    } while (--rows != 0);
    return 1;
}

/* the registers 144FC8 leaves its callers: after a blit, the clipped x with the last pixel the
   row loaded in AL, the row count (0), xn_gfx_width and 16 w (the planted ret's offset); else
   the clip's */
static void transparent_regs_out(const xn_blit *b, int drawn, xn_regs *r)
{
    const u8 *last;

    if (!drawn) {
        blit_to_regs(b, r);
        return;
    }
    last = b->buf + (b->h - 1) * (b->w + b->skip) + b->w - 1;
    r->eax = (b->x & ~0xFF) | *last;
    r->ecx = 0;
    r->edx = xn_gfx_width;
    r->ebx = b->w << 4;
}

void xn_draw_image_transparent_regs_r(xn_regs *r)
{
    xn_blit b;

    set_blit(&b, r->eax, r->edx, r->ebx, r->ecx, (const u8 *)r->esi, 0);
    transparent_regs_out(&b, xn_draw_image_transparent_regs(&b), r);
}

void xn_draw_image_transparent(s32 x, s32 y, s32 w, s32 h, const u8 *pixels)
{
    xn_blit b;

    set_blit(&b, x, y, w, h, pixels, 0);
    xn_draw_image_transparent_regs(&b);
}

void xn_draw_image_transparent_r(xn_regs *r)
{
    xn_blit b;

    set_blit(&b, r->eax, r->edx, r->ebx, r->ecx, (const u8 *)XN_STACK_ARG(r, 0), 0);
    transparent_regs_out(&b, xn_draw_image_transparent_regs(&b), r);
}

void xn_draw_img_record_transparent(s32 x, s32 y, const xn_img *img)
{
    xn_draw_image_transparent(x, y, img->width, img->height, img->pixels);
}

void xn_draw_img_record_transparent_regs_r(xn_regs *r)
{
    const xn_img *img = (const xn_img *)r->ebx;
    xn_blit b;

    set_blit(&b, r->eax, r->edx, img->width, img->height, img->pixels, 0);
    transparent_regs_out(&b, xn_draw_image_transparent_regs(&b), r);
}

void xn_draw_img_record_transparent_r(xn_regs *r)
{
    u32 ecx = r->ecx;

    xn_draw_img_record_transparent_regs_r(r);
    r->ecx = ecx;                               /* 144F48 keeps ECX EBP ESI EDI */
}

void xn_draw_unused_ret_144fc7(void)
{
}

void xn_draw_transparent_row_r(xn_regs *r)
{
    const u8 *src = (const u8 *)r->esi + 0x100;         /* the body's displacements */
    int n = xn_planted_count(asm_xn_draw_transparent_row, 16, 640);

    transparent_run((u8 *)r->edi + 0x100, src, n);
    if (n > 0)
        r->eax = (r->eax & ~0xFF) | src[n - 1];          /* AL: the last pixel loaded */
}
