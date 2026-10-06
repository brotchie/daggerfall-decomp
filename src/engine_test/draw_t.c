/* draw_t.c: test shims of src/engine/draw*.c (built only by tools/xn_rc.py; see
   src/engine_test/vec_t.c and docs/xngine_canonical.md). Each maps its asm entry's registers
   (config/xngine_abi.csv) to the C call and back. */
#include "xdraw.h"

/* The unrolled row bodies the asm calls after planting a `ret` in them: the count is where the
   planter put the ret (the first step whose first byte is C3h), the asm's own input */
extern u8 asm_xn_draw_transparent_row[];
extern u8 asm_xn_draw_shaded_row[];

static u32 planted_count(const u8 *body, int stride, int max)
{
    int n;

    for (n = 0; n < max; n++, body += stride)
        if (*body == 0xC3)
            return n;
    return max;
}

/* The scaled image's statics: its row compiler and clip take their inputs here, from
   xn_draw_image_scaled's asm */
extern u16 xn_draw_scaled_flags;
extern s16 xn_draw_scaled_x;
extern s16 xn_draw_scaled_y;
extern u16 xn_draw_scaled_src_w;
extern u8 xn_draw_scaled_x_int;
extern u16 xn_draw_scaled_x_frac;
extern u16 xn_draw_scaled_skip_x;
extern u16 xn_draw_scaled_skip_y;
extern u16 xn_draw_scaled_src_ofs;

/* a blit from the clip routine's registers: x EAX, y EDX, w EBX, h ECX, skip EBP, buf ESI */
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
    r->ebp = b->skip;
    r->esi = (u32)b->buf;
}

static void set_blit(xn_blit *b, u32 x, u32 y, u32 w, u32 h, u32 buf, u32 skip)
{
    b->x = x;
    b->y = y;
    b->w = w;
    b->h = h;
    b->skip = skip;
    b->buf = (u8 *)buf;
}

/* (x1, y1) EAX EDX, (x2, y2) EBX ECX */
static void regs_to_line(const xn_regs *r, xn_line *l)
{
    l->x1 = r->eax;
    l->y1 = r->edx;
    l->x2 = r->ebx;
    l->y2 = r->ecx;
}

static void line_to_regs(const xn_line *l, xn_regs *r)
{
    r->eax = l->x1;
    r->edx = l->y1;
    r->ebx = l->x2;
    r->ecx = l->y2;
}

/* ==== draw.c ================================================================================ */

void xn_draw_fill_rect_r(xn_regs *r)
{
    xn_draw_fill_rect(r->eax, r->edx, r->ebx, r->ecx);
}

/* the blit in its registers; CF when nothing is left */
void xn_draw_clip_rect_r(xn_regs *r)
{
    xn_blit b;
    int ok;

    regs_to_blit(r, &b);
    ok = xn_draw_clip_rect(&b);
    blit_to_regs(&b, r);
    XN_SETFLAG(r, XN_CF, !ok);
}

void xn_draw_get_rect_r(xn_regs *r)
{
    xn_draw_get_rect_b(r);
}

/* dst in EDI, dst_skip in EBP; EAX: the row step or the clip's x */
void xn_draw_get_rect_regs_r(xn_regs *r)
{
    xn_blit b;
    int drawn;

    set_blit(&b, r->eax, r->edx, r->ebx, r->ecx, r->edi, r->ebp);
    drawn = xn_draw_get_blit(&b);
    r->eax = drawn ? xn_gfx_width - b.w : b.x;
}

void xn_draw_put_rect_r(xn_regs *r)
{
    xn_draw_put_rect(r->eax, r->edx, r->ebx, r->ecx, (const u8 *)XN_STACK_ARG(r, 0),
                     XN_STACK_ARG(r, 1));
}

/* src in ESI, src_skip in EBP */
void xn_draw_put_rect_regs_r(xn_regs *r)
{
    xn_draw_put_rect(r->eax, r->edx, r->ebx, r->ecx, (const u8 *)r->esi, r->ebp);
}

void xn_draw_image_r(xn_regs *r)
{
    xn_draw_image_b(r);
}

/* pixels in ESI */
void xn_draw_image_regs_r(xn_regs *r)
{
    xn_blit b;

    set_blit(&b, r->eax, r->edx, r->ebx, r->ecx, r->esi, 0);
    xn_draw_image_regs_out(&b, xn_draw_put_blit(&b), r);
}

/* x EAX, y EDX, the record in EBX */
void xn_draw_img_record_r(xn_regs *r)
{
    xn_draw_img_record(r->eax, r->edx, (const xn_img *)r->ebx);
}

void xn_draw_img_record_regs_r(xn_regs *r)
{
    xn_draw_img_record(r->eax, r->edx, (const xn_img *)r->ebx);
}

void xn_draw_img_record_transparent_r(xn_regs *r)
{
    xn_draw_img_record_transparent(r->eax, r->edx, (const xn_img *)r->ebx);
}

void xn_draw_img_record_transparent_regs_r(xn_regs *r)
{
    xn_draw_img_record_transparent(r->eax, r->edx, (const xn_img *)r->ebx);
}

void xn_draw_unused_ret_144f47_r(xn_regs *r)
{
    xn_draw_unused_ret_144f47();
}

void xn_draw_image_transparent_r(xn_regs *r)
{
    xn_draw_image_transparent_b(r);
}

void xn_draw_unused_ret_144fc7_r(xn_regs *r)
{
    xn_draw_unused_ret_144fc7();
}

/* pixels in ESI */
void xn_draw_image_transparent_regs_r(xn_regs *r)
{
    xn_blit b;

    set_blit(&b, r->eax, r->edx, r->ebx, r->ecx, r->esi, 0);
    xn_draw_image_transparent_regs_out(&b, xn_draw_put_blit_transparent(&b), r);
}

/* the row at ESI + 100h to EDI + 100h (the body's displacements), its length the planted ret's
   step (16 bytes a step, 640 steps); AL the last pixel loaded */
void xn_draw_transparent_row_r(xn_regs *r)
{
    const u8 *src = (const u8 *)r->esi + 0x100;
    u32 n = planted_count(asm_xn_draw_transparent_row, 16, 640);

    xn_draw_transparent_row((u8 *)r->edi + 0x100, src, n);
    if (n > 0)
        r->eax = (r->eax & ~0xFFu) | src[n - 1];
}

/* ==== drawline.c ============================================================================ */

void xn_draw_hline_r(xn_regs *r)
{
    xn_draw_hline(r->eax, r->edx, r->ebx);
}

void xn_draw_vline_keep_regs_r(xn_regs *r)
{
    xn_draw_vline_keep_regs(r->eax, r->edx, r->ebx);
}

/* x EAX, y1 EDX, y2 ECX */
void xn_draw_vline_r(xn_regs *r)
{
    xn_draw_vline(r->eax, r->edx, r->ecx);
}

void xn_draw_line_r(xn_regs *r)
{
    xn_draw_line(r->eax, r->edx, r->ebx, r->ecx);
}

void xn_draw_line_nosave_r(xn_regs *r)
{
    xn_draw_line_nosave(r->eax, r->edx, r->ebx, r->ecx);
}

void xn_draw_line_unclipped_r(xn_regs *r)
{
    xn_draw_line_unclipped(r->eax, r->edx, r->ebx, r->ecx);
}

void xn_draw_line_to_r(xn_regs *r)
{
    xn_draw_line_to(r->eax, r->edx);
}

/* the line in its registers; CF when none of it is inside */
void xn_draw_line_clip_r(xn_regs *r)
{
    xn_line l;
    int ok;

    regs_to_line(r, &l);
    ok = xn_draw_line_clip(&l);
    line_to_regs(&l, r);
    XN_SETFLAG(r, XN_CF, !ok);
}

void xn_draw_clip_rect_xyxy_r(xn_regs *r)
{
    xn_line l;
    int ok;

    regs_to_line(r, &l);
    ok = xn_draw_clip_rect_xyxy(&l);
    line_to_regs(&l, r);
    XN_SETFLAG(r, XN_CF, !ok);
}

void xn_draw_rect_outline_r(xn_regs *r)
{
    xn_draw_rect_outline(r->eax, r->edx, r->ebx, r->ecx);
}

void xn_draw_fill_rect_clipped_r(xn_regs *r)
{
    xn_draw_fill_rect_clipped(r->eax, r->edx, r->ebx, r->ecx);
}

void xn_draw_line_text_colour_r(xn_regs *r)
{
    xn_draw_line_text_colour(r->eax, r->edx, r->ebx, r->ecx);
}

/* ==== drawimg.c ============================================================================= */

void xn_draw_copy_rect_stride_r(xn_regs *r)
{
    xn_draw_copy_rect_stride((const u8 *)r->eax, (u8 *)r->edx, r->ebx, r->ecx,
                             XN_STACK_ARG(r, 0));
}

void xn_draw_copy_rect_stride_bytes_r(xn_regs *r)
{
    xn_draw_copy_rect_stride_bytes_b(r);
}

void xn_draw_cif_rle_frame_r(xn_regs *r)
{
    xn_draw_cif_rle_frame((const xn_rle_group *)r->eax, r->edx, r->ebx);
}

void xn_draw_img_masked_remap_r(xn_regs *r)
{
    xn_draw_img_masked_remap((const xn_img *)r->eax, r->edx);
}

void xn_draw_fullscreen_overlay_shaded_r(xn_regs *r)
{
    xn_draw_fullscreen_overlay_shaded((const u8 *)r->eax);
}

void xn_draw_darken_rect_r(xn_regs *r)
{
    xn_draw_darken_rect(r->eax, r->edx, r->ebx, r->ecx);
}

void xn_draw_cel_frame_r(xn_regs *r)
{
    xn_draw_cel_frame((const xn_cel *)r->eax, r->edx, r->ebx);
}

void xn_draw_image_drop_shadow_r(xn_regs *r)
{
    xn_draw_image_drop_shadow(r->eax, r->edx, r->ebx, r->ecx, (const u8 *)XN_STACK_ARG(r, 0));
}

/* icon in BX */
void xn_draw_spell_icon_r(xn_regs *r)
{
    xn_draw_spell_icon(r->eax, r->edx, (u16)r->ebx);
}

void xn_draw_paperdoll_mask_r(xn_regs *r)
{
    xn_draw_paperdoll_mask_b(r);
}

void xn_draw_paperdoll_item_r(xn_regs *r)
{
    xn_draw_paperdoll_item(r->eax, r->edx, r->ebx, r->ecx, (const u8 *)XN_STACK_ARG(r, 0));
}

void xn_draw_cast_anim_mirrored_r(xn_regs *r)
{
    xn_draw_cast_anim_mirrored((const xn_img *)r->eax, r->edx, r->ebx);
}

void xn_draw_image_masked_at_origin_r(xn_regs *r)
{
    xn_draw_image_masked_at_origin(r->eax, r->edx, (const u8 *)r->ebx);
}

/* mask and table on the stack (the caller pops them) */
void xn_draw_remap_masked_rect_r(xn_regs *r)
{
    xn_draw_remap_masked_rect(r->eax, r->edx, r->ebx, r->ecx, (const u8 *)XN_STACK_ARG(r, 0),
                              (const u8 *)XN_STACK_ARG(r, 1));
}

void xn_draw_zoom4x_r(xn_regs *r)
{
    xn_draw_zoom4x((const u8 *)r->eax, (u8 *)r->edx, r->ebx);
}

/* src EAX, dst EDX, value BL, count ECX */
void xn_draw_mark_matching_r(xn_regs *r)
{
    xn_draw_mark_matching((const u8 *)r->eax, (u8 *)r->edx, (u8)r->ebx, r->ecx);
}

void xn_draw_remap_rect_r(xn_regs *r)
{
    xn_draw_remap_rect((u8 *)r->eax, r->edx, r->ebx, r->ecx, (const u8 *)XN_STACK_ARG(r, 0));
}

/* the table in EBP */
void xn_draw_remap_rect_regs_r(xn_regs *r)
{
    xn_draw_remap_rect((u8 *)r->eax, r->edx, r->ebx, r->ecx, (const u8 *)r->ebp);
}

/* ==== drawunr.c ============================================================================= */

void xn_draw_view_checkerboard_r(xn_regs *r)
{
    xn_draw_view_checkerboard();
}

void xn_draw_remap_rows_320_r(xn_regs *r)
{
    xn_draw_remap_rows_320((const u8 *)r->eax, (u8 *)r->edx, (const u8 *)r->ebx, r->ecx);
}

void xn_draw_fill_rows_pattern8_r(xn_regs *r)
{
    xn_draw_fill_rows_pattern8((const u8 *)r->eax, (u8 *)r->edx, r->ebx);
}

void xn_draw_clip_image_rect_r(xn_regs *r)
{
    xn_blit b;
    int ok;

    regs_to_blit(r, &b);
    ok = xn_draw_clip_image_rect(&b);
    blit_to_regs(&b, r);
    XN_SETFLAG(r, XN_CF, !ok);
}

void xn_draw_image_shaded_r(xn_regs *r)
{
    xn_draw_image_shaded(r->eax, r->edx, r->ebx, r->ecx, (const u8 *)XN_STACK_ARG(r, 0));
}

void xn_draw_ret_stub_r(xn_regs *r)
{
    xn_draw_ret_stub();
}

/* src in ESI */
void xn_draw_image_shaded_regs_r(xn_regs *r)
{
    xn_draw_image_shaded(r->eax, r->edx, r->ebx, r->ecx, (const u8 *)r->esi);
}

/* the row at ESI + 100h through the table in EAX to EDI + 100h, its length the planted ret's
   step (18 bytes a step, 321 steps); AL the last step's colour (0 when it skipped) */
void xn_draw_shaded_row_r(xn_regs *r)
{
    const u8 *src = (const u8 *)r->esi + 0x100;
    const u8 *table = XN_TABLE256(r->eax);
    u32 n = planted_count(asm_xn_draw_shaded_row, 18, 321);

    xn_draw_shaded_row((u8 *)r->edi + 0x100, src, n, table);
    if (n > 0)
        r->eax = (r->eax & ~0xFFu) | (src[n - 1] != 0 ? table[src[n - 1]] : 0);
}

/* ==== drawscl.c ============================================================================= */

/* x EAX (only AX), y EDX, w EBX, h ECX; src_w, src_h, flags and src on the stack */
void xn_draw_image_scaled_r(xn_regs *r)
{
    xn_draw_image_scaled(r->eax, r->edx, r->ebx, r->ecx, XN_STACK_ARG(r, 0), XN_STACK_ARG(r, 1),
                         XN_STACK_ARG(r, 2), (const u8 *)XN_STACK_ARG(r, 3));
}

/* the image's x and y, and the cuts, in the statics; CF when it starts outside */
void xn_draw_image_scaled_clip_r(xn_regs *r)
{
    int ok = xn_draw_image_scaled_clip(&xn_draw_scaled_x, &xn_draw_scaled_y,
                                       &xn_draw_scaled_skip_x, &xn_draw_scaled_skip_y);

    XN_SETFLAG(r, XN_CF, !ok);
}

/* the row's inputs from the statics */
void xn_draw_image_scaled_row_r(xn_regs *r)
{
    static xn_scaled_col cols[XN_SCALED_COLS];

    xn_draw_image_scaled_row(cols, (u16)xn_draw_scaled_x, xn_draw_scaled_src_ofs,
                             xn_draw_scaled_src_w, xn_draw_scaled_x_int,
                             (u8)xn_draw_scaled_x_frac);
}
