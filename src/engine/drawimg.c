/* drawimg.c: XnGine's image blits (canonical C; the interface and the module's documentation are
   in xdraw.h): IMG, CEL and RLE images drawn opaque, transparent, remapped, shaded or mirrored,
   mostly on a fixed 320-byte row step. */
#include "xdraw.h"

extern s32 frame_counter;               /* the game's main-loop count */
extern u8 *icon_image;                  /* the spell icons: 20 a row, 16x16, 320 wide */

/* the screen offset of row y, where a negative y is the negative of row -y's (the RLE
   animations start above the screen) */
static s32 row_start(s32 y)
{
    return y < 0 ? -xn_gfx_row_offset[-y] : xn_gfx_row_offset[y];
}

void xn_draw_copy_rect_stride(const u8 *src, u8 *dst, s32 w, s32 h, s32 src_stride)
{
    u32 rows = h;                       /* Q-DRAW-06 */

    do {
        xn_copy_dwords(dst, src, (u32)w >> 2);
        xn_copy_bytes(dst + (w & ~3), src + (w & ~3), w & 3);
        dst += 320;
        src += src_stride;
    } while (--rows != 0);
}

void xn_draw_copy_rect_stride_bytes(const u8 *src, u8 *dst, s32 w, s32 h, s32 src_stride)
{
    u32 rows = h;                       /* Q-DRAW-06, for both counts */

    do {
        u32 k = 0;

        do {
            dst[k] = src[k];
        } while (++k != (u32)w);
        dst += 320;
        src += src_stride;
    } while (--rows != 0);
}

/* The game's calls of xn_draw_copy_rect_stride_bytes (boundary adapter, Quirk Q-DRAW-05): the
   asm leaves its row count, run down, in ECX, and the game's 7 sites read it: 0. */
void xn_draw_copy_rect_stride_bytes_b(xn_regs *r)
{
    xn_draw_copy_rect_stride_bytes((const u8 *)r->eax, (u8 *)r->edx, r->ebx, r->ecx,
                                   XN_STACK_ARG(r, 0));
    r->ecx = 0;
}

void xn_draw_cif_rle_frame(const xn_rle_group *g, s32 frame, s32 y_add)
{
    const u8 *remap = XN_TABLE256(color_remap);
    const u8 *src = (const u8 *)g + g->frame_offset[frame];
    u8 *row = screen_buffer + g->x + row_start(g->y + y_add);
    u32 rows = g->height;

    do {
        u8 *p = row;
        s32 left = g->width;

        do {
            u8 c = *src++;
            u32 n, k;

            if (c < 0x80) {             /* c + 1 literal pixels */
                n = c + 1;
                left -= n;
                if (p >= screen_buffer)         /* Quirk Q-DRAW-09: >= */
                    for (k = 0; k < n; k++)
                        p[k] = remap[src[k]];
                src += n;
            } else {                    /* (c & 7Fh) + 1 times the next byte; 0 transparent */
                n = (c & 0x7F) + 1;
                left -= n;
                c = *src++;
                if (p > screen_buffer && c != 0)        /* (and > here) */
                    xn_fill_bytes(p, remap[c], n);
            }
            p += n;
        } while (left != 0);
        row += 320;
    } while (--rows != 0);
}

void xn_draw_img_masked_remap(const xn_img *img, s32 y_add)
{
    const u8 *remap = XN_TABLE256(color_remap);
    const u8 *src = img->pixels;
    s32 y = img->y + y_add;
    u32 rows = img->height;
    u8 *row;

    if (y < xn_gfx_clip_top) {
        y -= xn_gfx_clip_top;           /* Quirk Q-DRAW-10: the rows above, a negative row */
        rows += y;
        src -= img->width * y;
    }
    row = screen_buffer + xn_gfx_row_offset[y] + img->x;
    do {
        u32 k = 0;

        do {
            if (*src != 0)
                row[k] = remap[*src];
            src++;
        } while (++k != img->width);
        row += 320;
    } while (--rows != 0);
}

void xn_draw_fullscreen_overlay_shaded(const u8 *overlay)
{
    const u8 *darken = xn_shade_table + 20 * 256;
    u8 *p = screen_buffer;
    u32 rows = 200, k;

    do {
        for (k = 0; k < 320; k++, p++, overlay++)
            *p = *overlay > 0x0F ? *overlay : darken[*p];
    } while (--rows != 0);
}

void xn_draw_darken_rect(s32 x, s32 y, s32 w, s32 h)
{
    const u8 *darken = XN_TABLE256(xn_shade_table + 20 * 256);
    u8 *row = XN_SCREEN(x, y);
    u32 rows = h;

    do {
        u32 k = 0;

        do {
            row[k] = darken[row[k]];
        } while (++k != (u32)w);
        row += 320;
    } while (--rows != 0);
}

void xn_draw_cel_frame(const xn_cel *cel, s32 x, s32 y)
{
    u8 *row = XN_SCREEN(x, y);
    const u32 *offset = cel->row_offset;
    u32 rows = cel->height;

    if (cel->frames != 1) {
        /* this frame's offsets: frame_counter % frames in 16 bits (Quirk Q-DRAW-11: no frames is
           the asm's divide error, which gives frame 0) */
        u16 frame = cel->frames != 0 ? (u16)((u16)frame_counter % cel->frames) : 0;

        offset = (const u32 *)((const u8 *)offset + (u16)(frame * rows * 4));
    }
    do {
        const u8 *src;
        u16 k;

        if (*offset & 0x80000000u) {
            xn_img_rle_decode((const u8 *)cel + (*offset & 0x7FFFFFFF), xn_cel_row_buffer);
            src = xn_cel_row_buffer;
        } else {
            src = (const u8 *)cel + *offset;
        }
        k = 0;
        do {
            if (src[k] != 0)
                row[k] = src[k];
        } while (++k != cel->width);
        offset++;
        row += 320;
    } while (--rows != 0);
}

void xn_draw_image_drop_shadow(s32 x, s32 y, s32 w, s32 h, const u8 *src)
{
    u8 *row = XN_SCREEN(x, y);
    u32 rows = h;

    do {
        u32 k = 0;

        do {
            if (src[k] != 0) {
                row[k] = src[k];
                row[k + 2 * 320 + 2] = 0x9C;
            }
        } while (++k != (u32)w);
        src += 256;
        row += 320;
    } while (--rows != 0);
}

void xn_draw_spell_icon(s32 x, s32 y, u16 icon)
{
    u8 *dst = XN_SCREEN(x, y);
    const u8 *src = icon_image + (u16)(icon % 20 * 16) + (u16)(icon / 20 * 16 * 320);
    int k;

    for (k = 0; k < 16; k++) {
        xn_copy_dwords(dst, src, 4);
        dst += 320;
        src += 320;
    }
}

void xn_draw_paperdoll_mask(u8 *dst, const u8 *src, s32 w, s32 h, u8 colour)
{
    u32 rows = h;

    do {
        u32 k = 0;

        do {
            if (src[k] != 0)
                dst[k] = colour;
        } while (++k != (u32)w);
        src += 256;
        dst += 125;
    } while (--rows != 0);
}

/* The game's call of xn_draw_paperdoll_mask (boundary adapter: its interface): dst EAX, src
   ECX, w EDX, h EBX, the colour on the stack (the caller pops it) */
void xn_draw_paperdoll_mask_b(xn_regs *r)
{
    xn_draw_paperdoll_mask((u8 *)r->eax, (const u8 *)r->ecx, r->edx, r->ebx,
                           (u8)XN_STACK_ARG(r, 0));
}

void xn_draw_paperdoll_item(s32 x, s32 y, s32 w, s32 h, const u8 *image)
{
    const u8 *remap = XN_TABLE256(color_remap);
    const u8 *src = scratch_buffer;
    u8 *row = XN_SCREEN(x, y);
    u32 rows = h;

    xn_img_unpack_rows(image, (u16)w, (u16)h, scratch_buffer);
    do {
        u32 k = 0;

        do {
            u8 c = src[k];

            if (c == 0xFF)
                row[k] = xn_paperdoll_background[row + k - screen_buffer];
            else if (c != 0)
                row[k] = remap[c];
        } while (++k != (u32)w);
        row += 320;
        src += 256;
    } while (--rows != 0);
}

void xn_draw_cast_anim_mirrored(const xn_img *images, s32 frame, s32 y_add)
{
    const xn_img *img = xn_img_skip_records(images, frame);
    const u8 *src = img->pixels;
    s32 mirror = 319 - 2 * img->x;      /* from a pixel to its mirror image */
    u8 *row = screen_buffer + img->x + row_start(img->y + y_add);
    u32 rows = img->height;

    do {
        u8 *p = row, *m = row + mirror;
        s32 left = img->width;

        do {
            u8 c = *src++;
            u32 n, k;

            if (c < 0x80) {             /* c + 1 pixels, 0 transparent */
                n = c + 1;
                left -= n;
                if (p + n > screen_buffer) {
                    for (k = 0; k < n; k++, p++, m--)
                        if (src[k] != 0)
                            *p = *m = src[k];
                } else {
                    p += n;             /* Quirk Q-DRAW-09: the mirror pointer stays */
                }
                src += n;
            } else {                    /* (c & 7Fh) + 1 times the next byte */
                n = (c & 0x7F) + 1;
                left -= n;
                c = *src++;
                if (c != 0 && p > screen_buffer) {
                    xn_fill_bytes(p, c, n);
                    for (k = 0; k < n; k++)
                        m[-(s32)k] = c;
                }
                p += n;
                m -= n;
            }
        } while (left != 0);
        row += 320;
    } while (--rows != 0);
}

void xn_draw_image_masked_at_origin(s32 h, s32 w, const u8 *src)
{
    u8 *row = screen_buffer;
    u32 rows = h;

    if (h == 0 || w == 0)
        return;
    do {
        u32 k = 0;

        do {
            if (*src != 0)
                row[k] = *src;
            src++;
        } while (++k != (u32)w);
        row += 320;
    } while (--rows != 0);
}

void xn_draw_remap_masked_rect(s32 x, s32 y, s32 w, s32 h, const u8 *mask, const u8 *table)
{
    u8 *row;
    u32 rows = h;

    if (x < xn_gfx_clip_left || x + w >= xn_gfx_clip_right ||      /* Quirk Q-DRAW-12: >= */
        y < xn_gfx_clip_top || y + h >= xn_gfx_clip_bottom)
        return;
    table = XN_TABLE256(table);
    row = XN_SCREEN(x, y);
    do {
        u32 k = 0;

        do {
            if (*mask != 0)
                row[k] = table[row[k]];
            mask++;
        } while (++k != (u32)w);
        row += 320;
    } while (--rows != 0);
}

void xn_draw_zoom4x(const u8 *src, u8 *dst, s32 n)
{
    int r;

    for (r = 0; r < 4; r++) {
        u32 k = 0;
        u8 *p = dst;

        do {
            xn_fill_bytes(p, src[k], 4);
            p += 4;
        } while (++k != (u32)n);
        dst += 320;
    }
}

void xn_draw_mark_matching(const u8 *src, u8 *dst, u8 value, s32 count)
{
    u32 k = 0;

    do {
        if (src[k] == value)
            dst[k] = 0xF4;
    } while (++k != (u32)count);
}

void xn_draw_remap_rect(u8 *buf, s32 skip, s32 w, s32 h, const u8 *table)
{
    u32 rows = h;

    table = XN_TABLE256(table);
    do {
        u32 k = 0;

        do {
            *buf = table[*buf];
            buf++;
        } while (++k != (u32)w);
        buf += skip;
    } while (--rows != 0);
}
