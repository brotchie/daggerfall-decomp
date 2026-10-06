/* img_t.c: test shims of src/engine/img.c (built only by tools/xn_rc.py; see
   src/engine_test/vec_t.c and docs/xngine_canonical.md). */
#include "ximg.h"
#include "xdrawhlp.h"

/* The asm's unpacker takes its image from xn_draw_image_scaled's statics, where its callers
   (the scaled image and the paperdoll item) left them, and unpacks into scratch_buffer */
extern u16 xn_draw_scaled_src_w;
extern u16 xn_draw_scaled_src_h;
extern u8 *xn_draw_scaled_src;

void xn_img_unpack_rows_r(xn_regs *r)
{
    xn_img_unpack_rows(xn_draw_scaled_src, xn_draw_scaled_src_w, xn_draw_scaled_src_h,
                       scratch_buffer);
}

/* src EAX, dst EDX */
void xn_img_rle_decode_r(xn_regs *r)
{
    xn_img_rle_decode((const u8 *)r->eax, (u8 *)r->edx);
}

void xn_img_cif_group_r(xn_regs *r)
{
    r->eax = (u32)xn_img_cif_group((const xn_img *)r->eax, r->edx);
}

void xn_img_rle_frame_unpack_r(xn_regs *r)
{
    r->eax = xn_img_rle_frame_unpack((const xn_rle_group *)r->eax, r->edx, (u8 *)r->ebx);
}

void xn_img_skip_records_r(xn_regs *r)
{
    r->eax = (u32)xn_img_skip_records((const xn_img *)r->eax, r->edx);
}

void xn_img_remap_colours_r(xn_regs *r)
{
    xn_img_remap_colours((xn_img *)r->eax);
}
