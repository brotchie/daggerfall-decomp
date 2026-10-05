/* xdraw.h: XnGine's 2D drawing on screen_buffer (src/engine/draw*.c; see xngine.h): rectangle
   blits and fills, lines, the scaled inventory images, the RLE weapon and spell animations,
   and the full-screen overlays.

   Each declaration keeps the function's asm interface (config/xngine_abi.csv): no pragma is
   Watcom's own convention; a pragma names the registers; NAME_r is the glue for a function
   whose asm callers read several registers or flags. */
#ifndef XDRAW_H
#define XDRAW_H

#include "xngine.h"
#include "xdrawhlp.h"
#include "ximg.h"

/* ==== rectangles (draw.c) ================================================================== */

/* A rectangle between the screen and a buffer, as the clip routine 144E00 takes it in its
   registers: x, y, w, h on the screen; the buffer's rows w + skip bytes apart. */
typedef struct xn_blit {
    s32 x, y, w, h;         /* EAX EDX EBX ECX */
    s32 skip;               /* EBP */
    u8 *buf;                /* ESI */
} xn_blit;

/* (x, y, w, h) filled with text_colour; not clipped. 13 game sites: the options' volume bars,
   the breath meter, list frames, the text cursor... */
void xn_draw_fill_rect(s32 x, s32 y, s32 w, s32 h);

/* Clips the rectangle to the clip window in place: rows above and columns left of it move the
   buffer pointer on (by rows of the unclipped width + skip, and by columns), columns cut on
   either side add to skip. Returns 0 (the asm's CF) when nothing is left; the rectangle is
   then as far as it got (after a failed top clip, y is y - clip_top, as the asm leaves EDX). */
int xn_draw_clip_rect(xn_blit *b);
void xn_draw_clip_rect_r(xn_regs *r);

/* Copies the clipped screen rectangle (x, y, w, h) into dst, whose rows are w + dst_skip
   bytes apart: the background under a list, the mouse cursor, a message box. A game caller
   reads EAX's upper bits after it: the screen's row step (xn_gfx_width - w) after a copy,
   else the clip's x (xn_draw_get_rect_r). */
void xn_draw_get_rect(s32 x, s32 y, s32 w, s32 h, u8 *dst, s32 dst_skip);
void xn_draw_get_rect_r(xn_regs *r);
void xn_draw_get_rect_regs_r(xn_regs *r);      /* 144E9C: dst in EDI, dst_skip in EBP */

/* Copies src (rows w + src_skip bytes apart) to the clipped screen rectangle (x, y, w, h):
   restores what xn_draw_get_rect saved; sheets and icons. */
void xn_draw_put_rect(s32 x, s32 y, s32 w, s32 h, const u8 *src, s32 src_skip);
void xn_draw_put_rect_regs_r(xn_regs *r);      /* 144EF0: src in ESI, src_skip in EBP */

/* The opaque image blit: the w x h pixels at (x, y), clipped. 67 game sites. */
void xn_draw_image(s32 x, s32 y, s32 w, s32 h, const u8 *pixels);
void xn_draw_image_r(xn_regs *r);

/* xn_draw_image's body: b's buffer (skip 0) to the screen, clipped; returns 0 when nothing was
   left. Its asm callers read EAX ECX EDX EBX after it: the clip's registers, or after a blit
   xn_gfx_width - w, 0, 0 and w (xn_draw_image_regs_r). */
int xn_draw_image_regs(xn_blit *b);
void xn_draw_image_regs_r(xn_regs *r);

/* Dead: the IMG record img drawn opaque at (x, y) (144F28 saves ECX EBP ESI EDI around 144F38,
   which takes w and h from the header and the pixels after it). */
void xn_draw_img_record(s32 x, s32 y, const xn_img *img);
void xn_draw_img_record_r(xn_regs *r);
void xn_draw_img_record_regs_r(xn_regs *r);

/* Dead: a lone ret between functions. */
void xn_draw_unused_ret_144f47(void);
#pragma aux xn_draw_unused_ret_144f47 parm [] modify exact [eax];

/* Dead: the IMG record img drawn at (x, y) with colour 0 transparent (144F48 saves registers
   around 144F58). */
void xn_draw_img_record_transparent(s32 x, s32 y, const xn_img *img);
void xn_draw_img_record_transparent_r(xn_regs *r);
void xn_draw_img_record_transparent_regs_r(xn_regs *r);

/* The image blit with colour 0 transparent: the w x h pixels at (x, y), clipped. 33 game sites
   (the mouse cursor, the compass, books, faces, FLC frames...). */
void xn_draw_image_transparent(s32 x, s32 y, s32 w, s32 h, const u8 *pixels);
void xn_draw_image_transparent_r(xn_regs *r);

/* Dead: a lone ret between functions. */
void xn_draw_unused_ret_144fc7(void);
#pragma aux xn_draw_unused_ret_144fc7 parm [] modify exact [eax];

/* xn_draw_image_transparent's body: b's buffer to the screen, clipped, 0 transparent; returns
   0 when nothing was left. The asm plants a `ret` in the unrolled row 14501C at step w (the
   word it overwrites saved and put back) and calls it once per row; its callers read EAX ECX
   EDX EBX after it: the clipped x with the last source pixel in AL, 0, xn_gfx_width and 16 w. */
int xn_draw_image_transparent_regs(xn_blit *b);
void xn_draw_image_transparent_regs_r(xn_regs *r);

/* The unrolled row of xn_draw_image_transparent_regs: the asm's 640 steps, each copying one
   non-zero pixel of the row (ESI + 100h) to the screen (EDI + 100h), stopped by the ret its
   planter put at step w. AL is the last pixel read. Routed through xn_planted_count: the
   planted ret is the count. */
void xn_draw_transparent_row_r(xn_regs *r);

/* ==== lines (drawline.c) =================================================================== */

/* A line's end points, or a rectangle's corners, as the clip routines take them in their
   registers: (x1, y1) in EAX EDX, (x2, y2) in EBX ECX. */
typedef struct xn_line {
    s32 x1, y1, x2, y2;
} xn_line;

/* The end point the last line clipped (xn_draw_line_clip) and the deltas it clipped with */
extern s32 pen_x;
extern s32 pen_y;
extern s32 xn_draw_line_dx;
extern s32 xn_draw_line_dy;

/* The horizontal line [x1, x2) (either order) at row y, clipped, in text_colour. */
void xn_draw_hline(s32 x1, s32 y, s32 x2);

/* Dead: xn_draw_vline from (x, y1) to y2. */
void xn_draw_vline_keep_regs(s32 x, s32 y1, s32 y2);

/* The vertical line from row y1 to y2 (either order; the lower end's row is not drawn) at
   column x, clipped, in text_colour. (Its asm callers need EAX kept: a keep-eax route.) */
void xn_draw_vline(s32 x, s32 y1, s32 y2);
#pragma aux xn_draw_vline parm [eax] [edx] [ecx] modify exact [eax ecx edx];

/* The line from (x1, y1) to (x2, y2) in text_colour, clipped: the text cursor. */
void xn_draw_line(s32 x1, s32 y1, s32 x2, s32 y2);

/* The same (the asm saves no registers). */
void xn_draw_line_nosave(s32 x1, s32 y1, s32 x2, s32 y2);

/* A line already clipped: horizontal and vertical ones go to xn_draw_hline and xn_draw_vline;
   otherwise one pixel per step of the major axis, the minor one in 16.16 with a step from the
   reciprocal table. Like xn_draw_hline and xn_draw_vline, it stops one pixel short of the
   end point. */
void xn_draw_line_unclipped(s32 x1, s32 y1, s32 x2, s32 y2);

/* The line from the pen (the last line's end point) to (x, y): the maps, notes, saves. */
void xn_draw_line_to(s32 x, s32 y);

/* Clips the line l to the clip window in place (the pen is left at its end point; the deltas
   kept in xn_draw_line_dx/dy): returns 0 (the asm's CF) when none of it is inside. The left
   and right cuts move y along the line, the top and bottom ones x. Kept quirks: a right cut
   puts x2 at clip_right - 1, a bottom one y2 at clip_bottom; a horizontal line on the top
   row is rejected. */
int xn_draw_line_clip(xn_line *l);
void xn_draw_line_clip_r(xn_regs *r);

/* Clips the rectangle (x1, y1)-(x2, y2) (x1 <= x2, y1 <= y2, right and bottom exclusive) to
   the clip window in place; returns 0 (the asm's CF; r unchanged) when it lies outside. */
int xn_draw_clip_rect_xyxy(xn_line *r);
void xn_draw_clip_rect_xyxy_r(xn_regs *r);

/* Dead: the outline of the rectangle (x1, y1)-(x2, y2) (either order), clipped: its bottom
   edge is at the clipped y2, which xn_draw_hline does not draw when it is the window's
   bottom. */
void xn_draw_rect_outline(s32 x1, s32 y1, s32 x2, s32 y2);

/* Dead: the rectangle (x1, y1)-(x2, y2) (either order) filled with text_colour, clipped. */
void xn_draw_fill_rect_clipped(s32 x1, s32 y1, s32 x2, s32 y2);

/* The line from (x1, y1) to (x2, y2), both ends drawn, in text_colour, by Bresenham's
   steps on a 320-byte stride (the list frames). Its state is kept in xn_line_*: the asm jumps
   to its x-major (CE61E) or y-major (CE63F) loop through xn_line_step_routine. (Its ABI row
   counts ESI and EDI as outputs, through that jump; it keeps them: the glue does too.) */
void xn_draw_line_text_colour(s32 x1, s32 y1, s32 x2, s32 y2);
void xn_draw_line_text_colour_r(xn_regs *r);

/* ==== images (drawimg.c) =================================================================== */

/* src (rows src_stride bytes apart) copied as a w x h block into dst, whose rows are 320
   bytes apart (dwords, then the bytes left): the options' joystick picture. */
void xn_draw_copy_rect_stride(const u8 *src, u8 *dst, s32 w, s32 h, s32 src_stride);

/* Frame `frame` of the RLE group g (a weapon CIF) at its own (x, y + y_add), every pixel
   through color_remap: literal runs are opaque, colour-0 repeats transparent. Rows above the
   screen's start are skipped: a literal run whose first pixel is before screen_buffer, a
   repeat whose first pixel is at or before it. Rows are 320 bytes apart. */
void xn_draw_cif_rle_frame(const xn_rle_group *g, s32 frame, s32 y_add);

/* The IMG img at its own (x, y + y_add), colour 0 transparent, others through color_remap,
   clipped at the view's top only. Kept bug: after a top clip it draws from row (y + y_add -
   clip_top), an index before xn_gfx_row_offset, rather than from clip_top. */
void xn_draw_img_masked_remap(const xn_img *img, s32 y_add);

/* The 320x200 overlay over screen_buffer: colours 0-15 darken the screen pixel through shade
   row 20, others replace it. The asm is 320 unrolled steps per row. Keeps every register. */
void xn_draw_fullscreen_overlay_shaded(const u8 *overlay);
#pragma aux xn_draw_fullscreen_overlay_shaded parm [eax] modify exact [eax];

/* The screen rectangle (x, y, w, h) darkened through shade row 20 (the info popup); not
   clipped, rows 320 bytes apart. */
void xn_draw_darken_rect(s32 x, s32 y, s32 w, s32 h);

/* The CEL animation cel at (x, y): its frame frame_counter % frames (frame 0 when it has
   none: the asm's divide error), colour 0 transparent; an RLE row (bit 31 of its offset) is
   decoded into xn_cel_row_buffer first. The class questions' scroll. The asm counts rows in
   ECX with only CX loaded: the C assumes the game's ECX top half, 0. */
void xn_draw_cel_frame(const xn_cel *cel, s32 x, s32 y);

/* The w x h pixels of src (rows 256 bytes apart) at (x, y), colour 0 transparent, each
   opaque pixel also casting colour 9Ch two rows down and two right: the potion maker. */
void xn_draw_image_drop_shadow(s32 x, s32 y, s32 w, s32 h, const u8 *src);

/* The 16x16 spell icon `icon` (column icon % 20, row icon / 20 of the 320-wide icon_image)
   at (x, y). */
void xn_draw_spell_icon(s32 x, s32 y, u16 icon);

/* colour into dst (rows 125 bytes apart) wherever src (rows 256 apart) is not 0: the
   paperdoll's pick mask. The caller pops the colour. */
void xn_draw_paperdoll_mask(u8 *dst, const u8 *src, s32 w, s32 h, u8 colour);
#pragma aux xn_draw_paperdoll_mask parm caller [eax] [ecx] [edx] [ebx] modify exact [eax ecx edx ebx];

/* The row-compressed paperdoll item img (w x h) unpacked into scratch_buffer and drawn at
   (x, y): colour 0 transparent, colour FFh the paperdoll's background
   (xn_paperdoll_background), others through color_remap. */
void xn_draw_paperdoll_item(s32 x, s32 y, s32 w, s32 h, const xn_img *img);

/* The RLE image `frame` of images at its own (x, y + y_add), colour 0 transparent, and again
   mirrored about the screen's centre column (x' = 319 - x): the two casting hands. Kept quirk:
   a literal run skipped above the screen does not move the mirror pointer. */
void xn_draw_cast_anim_mirrored(const xn_img *images, s32 frame, s32 y_add);

/* The h x w pixels of src at the screen's top left, colour 0 transparent: the people map. */
void xn_draw_image_masked_at_origin(s32 h, s32 w, const u8 *src);

/* Dead: when (x, y, w, h) lies inside the clip window (and not touching its right or bottom
   edge), each screen pixel under a non-zero byte of mask (w bytes a row) remapped through
   table. The caller pops the stack arguments. */
void xn_draw_remap_masked_rect(s32 x, s32 y, s32 w, s32 h, const u8 *mask, const u8 *table);
#pragma aux xn_draw_remap_masked_rect parm caller [eax] [edx] [ebx] [ecx] modify exact [eax ecx edx ebx];

/* The n bytes of src as a 4n-wide, 4-row block at dst (rows 320 apart): the travel map's
   zoom. */
void xn_draw_zoom4x(const u8 *src, u8 *dst, s32 n);

/* dst[i] = F4h wherever src[i] == value, for count bytes: the travel map's region. */
void xn_draw_mark_matching(const u8 *src, u8 *dst, u8 value, s32 count);

/* src (rows src_stride apart) copied byte by byte as a w x h block into dst (rows 320
   apart): the inventory's paperdoll, the spell icons. Returns 0, the asm's row count in ECX,
   which callers read. */
s32 xn_draw_copy_rect_stride_bytes(const u8 *src, u8 *dst, s32 w, s32 h, s32 src_stride);
#pragma aux xn_draw_copy_rect_stride_bytes parm routine [eax] [edx] [ebx] [ecx] value [ecx] modify exact [eax ecx edx ebx];

/* Dead: the w x h block at buf (rows w + skip apart) remapped in place through table. */
void xn_draw_remap_rect(u8 *buf, s32 skip, s32 w, s32 h, const u8 *table);
void xn_draw_remap_rect_regs_r(xn_regs *r);    /* 147858: table in EBP; leaves the last colour
                                                  in AL and 0 in ECX */

/* ==== unrolled bodies (drawunr.c) ========================================================== */

/* The hurt flash: colour F6h on every other pixel of the view's rows (clip_top to
   clip_bottom, 320 wide), the pattern shifted by one each row. The asm unrolls two rows and
   enters at the second for an odd row count; kept: an odd count draws one row fewer
   (3 rows draw 1), and 1 row loops 2^32 times. */
void xn_draw_view_checkerboard(void);

/* Dead: rows of 320 bytes of src copied to dst through table (320 unrolled steps a row).
   Keeps every register. */
void xn_draw_remap_rows_320(const u8 *src, u8 *dst, const u8 *table, s32 rows);
#pragma aux xn_draw_remap_rows_320 parm [eax] [edx] [ebx] [ecx] modify exact [eax];

/* Dead: each of `rows` 320-byte rows of dst filled with the next 8 bytes of src repeated 40
   times (unrolled). */
void xn_draw_fill_rows_pattern8(const u8 *src, u8 *dst, s32 rows);

/* Clips the rectangle like xn_draw_clip_rect, but without its checks of w and h, and the rows
   above the window skip w (not w + skip) bytes each. Dead chain (only 0xC8395). */
int xn_draw_clip_image_rect(xn_blit *b);
void xn_draw_clip_image_rect_r(xn_regs *r);

/* Dead: xn_draw_image_shaded_regs with the pixels on the stack. */
void xn_draw_image_shaded(s32 x, s32 y, s32 w, s32 h, const u8 *src);

/* Dead: a lone ret between functions. */
void xn_draw_ret_stub(void);
#pragma aux xn_draw_ret_stub parm [] modify exact [eax];

/* Dead chain: the w x h pixels at (x, y), clipped (xn_draw_clip_image_rect), colour 0
   transparent, others through shade row xn_draw_shade_row (a byte nothing writes: row 0).
   The asm plants a ret in the unrolled row 0xC83F6 at step w (the word saved and put back). */
void xn_draw_image_shaded_regs(s32 x, s32 y, s32 w, s32 h, const u8 *src);
#pragma aux xn_draw_image_shaded_regs parm [eax] [edx] [ebx] [ecx] [esi] modify exact [eax ecx edx ebx esi edi];

/* The unrolled row of xn_draw_image_shaded_regs: 321 steps, each a pixel of ESI + 100h to
   EDI + 100h through the 256-byte table in EAX, colour 0 skipped, stopped by the planted
   ret. AL is the last step's colour. Routed through xn_planted_count. */
void xn_draw_shaded_row_unrolled_r(xn_regs *r);

/* ==== the scaled image (drawscl.c) ========================================================= */

/* xn_draw_image_scaled's state: its arguments and steps as the asm keeps them (words and
   bytes), all written by every call (names.csv) */
extern u16 xn_draw_scaled_flags;        /* 8: row-compressed; 8000h: through color_remap */
extern s16 xn_draw_scaled_x;            /* the destination, clipped */
extern s16 xn_draw_scaled_y;
extern s16 xn_draw_scaled_w;            /* its size */
extern s16 xn_draw_scaled_h;
extern u8 xn_draw_scaled_x_int;         /* the 8.8 steps: destination pixels per source pixel */
extern u8 xn_draw_scaled_y_int;
extern u16 xn_draw_scaled_x_frac;       /* (a byte in a word) */
extern u16 xn_draw_scaled_y_frac;
extern u16 xn_draw_scaled_skip_x;       /* destination columns and rows the clip cut */
extern u16 xn_draw_scaled_skip_y;
extern u16 xn_draw_scaled_src_ofs;      /* the source offset of the next column */
/* self-patched operands of xn_draw_image_scaled (rule KEEP) */
extern u8 *xn_draw_scaled_bottom;       /* C08AD: the screen's end (row clip_bottom) */
extern s32 xn_draw_scaled_stride;       /* C08B9: the source rows' stride, always 100h */
extern u8 xn_draw_scaled_yint_op;       /* C08BE: the y step's integer part (a byte) */

/* One column of a scaled row: the source pixel's offset in its row, and the screen pixels
   [x, x + count) it covers */
typedef struct xn_scaled_col {
    u16 src;
    u16 x;
    u32 count;
} xn_scaled_col;

/* A scaled row as the row compiler emits it: its columns (when col is given, up to cap), and
   what the asm leaves in its registers (for its glue) */
typedef struct xn_scaled_row {
    xn_scaled_col *col;
    int cap;
    int n;                  /* columns emitted */
    int remap;              /* colours through color_remap (flag 8000h: an xlat) */
    u32 last_jump;          /* EAX: the last column's first je displacement (0 if none) */
    u16 count_left;         /* CX: the last column's count after a right clip to <= 0, else 0 */
    u16 acc;                /* BX: the column accumulator */
    u16 loop_hi;            /* ECX's top half when it is done */
} xn_scaled_row;

/* The image src (src_w x src_h; flag 8: row-compressed, unpacked into scratch_buffer
   first) drawn scaled to w x h at (x, y), clipped, colours 0 and FFh transparent, others
   through color_remap with flag 8000h: the inventory and paperdoll items, the cart. The
   steps are 8.8 (w * 256 / src_w); the row compiler emits the row's code into big_buffer, and
   each source row is drawn y-step times. Kept: the source rows are 100h apart but the top
   clip skips rows of src_w; destination rows are 320 apart; w or h of 2 or less draws
   nothing. A scale of 34 or more wraps the emitted je displacements, which the C's row does
   not follow (no game image is scaled that far). */
void xn_draw_image_scaled(s32 x, s32 y, s32 w, s32 h, s32 src_w, s32 src_h, s32 flags,
                          const u8 *src);

/* Clips xn_draw_scaled_x/y to the clip window (16-bit compares), the columns and rows cut
   to xn_draw_scaled_skip_x/y; returns 0 (the asm's CF) when the image starts right of or
   below it. */
int xn_draw_image_scaled_clip(void);
void xn_draw_image_scaled_clip_r(xn_regs *r);

/* The row compiler: writes into big_buffer the machine code of one scaled row, per source
   column with a non-zero count (the x accumulator's integer part):
       mov al, [esi + src_ofs]; or al, al; je end; cmp al, 0FFh; je end; [xlat;]
       [mov [edi + x], al;] [mov ah, al; mov [edi + x], ax ...]; end:
   cut at clip_right, then a ret; src_ofs counts up per source column. Fills row (the same
   columns as a plan). loop_hi: ECX's top half at the call, which the asm's `loop` counts
   with (0 from the game: the top half of xn_draw_image_scaled's h). */
void xn_draw_image_scaled_compile_row(xn_scaled_row *row, u32 loop_hi);
void xn_draw_image_scaled_compile_row_r(xn_regs *r);

#endif
