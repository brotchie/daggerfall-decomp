/* drawimg.c: XnGine's image blits as readable C (xdraw.h; see xngine.h and
   docs/xngine_readable.md): IMG, CEL and RLE images drawn opaque, transparent, remapped,
   shaded or mirrored, mostly on a fixed 320-byte stride. */
#include "xdraw.h"

extern s32 frame_counter;               /* the game's main-loop count */
extern u8 *icon_image;                  /* the spell icons: 20 a row, 16x16, 320 wide */
extern u8 *xn_paperdoll_background;     /* the paperdoll's background copy */
extern u8 xn_cel_row_buffer[330];       /* xn_draw_cel_frame's decoded RLE row */

/* the screen offset of row y, where a negative y is the negative of row -y's (the RLE
   animations start above the screen) */
static s32 row_start(s32 y)
{
    return y < 0 ? -xn_gfx_row_offset[-y] : xn_gfx_row_offset[y];
}

void xn_draw_copy_rect_stride(const u8 *src, u8 *dst, s32 w, s32 h, s32 src_stride)
{
    u32 rows = h;

    do {
        xn_copy_dwords(dst, src, (u32)w >> 2);
        xn_copy_bytes(dst + (w & ~3), src + (w & ~3), w & 3);
        dst += 320;
        src += src_stride;
    } while (--rows != 0);
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
                if (p >= screen_buffer)
                    for (k = 0; k < n; k++)
                        p[k] = remap[src[k]];
                src += n;
            } else {                    /* (c & 7Fh) + 1 times the next byte; 0 transparent */
                n = (c & 0x7F) + 1;
                left -= n;
                c = *src++;
                if (p > screen_buffer && c != 0)
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
        y -= xn_gfx_clip_top;           /* the rows above the view, negative: the bug's row */
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
        /* this frame's offsets: frame_counter % frames (a divide error, frame 0, for none),
           in 16 bits as the asm counts */
        xn_s64 count;
        u32 frame;

        count.lo = (u16)frame_counter;
        count.hi = 0;
        xn_u64_divrem(&count, cel->frames, &frame);
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

void xn_draw_paperdoll_item(s32 x, s32 y, s32 w, s32 h, const xn_img *img)
{
    const u8 *remap;
    const u8 *src;
    u8 *row = XN_SCREEN(x, y);
    u32 rows = h;

    xn_draw_scaled_src_w = w;
    xn_draw_scaled_src_h = h;
    xn_draw_scaled_src = (u8 *)img;
    xn_img_unpack_rows();
    src = scratch_buffer;
    remap = XN_TABLE256(color_remap);
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
                    p += n;             /* (the mirror pointer stays) */
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

    if (x < xn_gfx_clip_left || x + w >= xn_gfx_clip_right ||
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

s32 xn_draw_copy_rect_stride_bytes(const u8 *src, u8 *dst, s32 w, s32 h, s32 src_stride)
{
    u32 rows = h;

    do {
        u32 k = 0;

        do {
            dst[k] = src[k];
        } while (++k != (u32)w);
        dst += 320;
        src += src_stride;
    } while (--rows != 0);
    return 0;
}

/* the block remapped; the last colour written */
static u8 remap_rect(u8 *buf, s32 skip, s32 w, s32 h, const u8 *table)
{
    u32 rows = h;
    u8 c;

    table = XN_TABLE256(table);
    do {
        u32 k = 0;

        do {
            c = table[*buf];
            *buf++ = c;
        } while (++k != (u32)w);
        buf += skip;
    } while (--rows != 0);
    return c;
}

void xn_draw_remap_rect(u8 *buf, s32 skip, s32 w, s32 h, const u8 *table)
{
    remap_rect(buf, skip, w, h, table);
}

void xn_draw_remap_rect_regs_r(xn_regs *r)
{
    u8 last = remap_rect((u8 *)r->eax, r->edx, r->ebx, r->ecx, (const u8 *)r->ebp);

    r->eax = (r->ebp & ~0xFFu) | last;
    r->ecx = 0;
}
