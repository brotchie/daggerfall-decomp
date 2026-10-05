/* img.c: XnGine's image records and decoders as readable C (ximg.h; see xngine.h and
   docs/xngine_readable.md). */
#include "ximg.h"
#include "xdrawhlp.h"

extern u8 xn_pal_ega16_map[16];         /* a palette index per EGA colour */

void xn_img_unpack_rows(void)
{
    const u32 *offset = (const u32 *)xn_draw_scaled_src;
    const u8 *file = xn_draw_scaled_src - 0x1C;     /* the offsets count from the header */
    u8 *dst = scratch_buffer;
    u32 rows = xn_draw_scaled_src_h;

    do {
        u32 at = *offset++;

        if (at & 0x80000000u) {
            xn_img_rle_decode(file + (at & 0x7FFFFFFF), dst);
        } else {
            u32 w = xn_draw_scaled_src_w;

            xn_copy_dwords(dst, file + at, w >> 2);
            xn_copy_bytes(dst + (w & ~3), file + at + (w & ~3), w & 3);
        }
        dst += 256;
    } while (--rows != 0);
}

u32 xn_img_rle_decode(const u8 *src, u8 *dst)
{
    u16 left = *(const u16 *)src;

    src += 2;
    while (left != 0) {
        s16 n = *(const s16 *)src;

        src += 2;
        if (n < 0) {
            u16 count = -n;

            left -= count;
            xn_fill_bytes(dst, *src++, count);
            dst += count;
        } else {
            left -= n;
            xn_copy_bytes(dst, src, n);
            dst += n;
            src += n;
        }
    }
    return 0;
}

xn_rle_group *xn_img_cif_group(const xn_img *cif, s32 group)
{
    const u8 *p = cif->pixels + cif->data_size;
    u32 n;

    for (n = group; n != 0; n--)
        p += ((const xn_rle_group *)p)->size;
    return (xn_rle_group *)p;
}

s32 xn_img_rle_frame_unpack(const xn_rle_group *g, s32 frame, u8 *dst)
{
    const u8 *src = (const u8 *)g + g->frame_offset[frame];
    u32 rows = g->height;

    do {
        u8 *p = dst;
        s32 left = g->width;

        do {
            u8 c = *src++;
            u32 n;

            if (c < 0x80) {             /* c + 1 literal pixels */
                n = c + 1;
                xn_copy_bytes(p, src, n);
                src += n;
            } else {                    /* (c & 7Fh) + 1 times the next byte */
                n = (c & 0x7F) + 1;
                xn_fill_bytes(p, *src++, n);
            }
            left -= n;
            p += n;
        } while (left != 0);
        dst += g->width;
    } while (--rows != 0);
    return 1;
}

const xn_img *xn_img_skip_records(const xn_img *img, s32 k)
{
    u32 n;

    for (n = k; n != 0; n--)
        img = (const xn_img *)(img->pixels + img->data_size);
    return img;
}

void xn_img_remap_colours(xn_img *img)
{
    u8 *p = img->pixels;
    u32 n = img->width * img->height;

    do {
        if (*p != 0)
            *p = xn_pal_ega16_map[*p];
        p++;
    } while (--n != 0);
}
