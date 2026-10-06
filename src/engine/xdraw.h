/* xdraw.h: XnGine's 2D drawing on the back buffer (src/engine/draw.c, drawline.c, drawimg.c,
   drawunr.c, drawscl.c). Canonical C: plain prototypes, Watcom's own calling convention;
   docs/xngine_canonical.md.

   What it does
     Rectangle copies between the screen and a buffer, opaque and transparent image blits
     (clipped), lines and outlines, the scaled inventory images, the RLE weapon, spell and CEL
     animations, the full-screen overlays and the hurt flash: everything the game's interface
     draws on screen_buffer.

   Units and conventions
     Coordinates are back-buffer pixels; the clip window is xgfx.h's (left and top inclusive,
     right and bottom exclusive). Some routines use the screen's row step xn_gfx_width, others
     a fixed 320 (each says which). Colour 0 is transparent where a routine says so; colours
     go through 256-byte tables (shade rows, dye remaps: XN_TABLE256, Q-DRAW-01).
     A buffer's rows are w + skip bytes apart (xn_blit), unless a routine says otherwise.

   Tables read: text_colour, xn_shade_table (row 20 darkens), color_remap, xn_recip16_table,
   xn_colour_fill_table; work buffers scratch_buffer (unpacked images), and the CEL row buffer.

   Quirks kept (docs/engine/quirks.md, Q-DRAW-nn): the registers some blits leave the game
   (Q-DRAW-02, -03, -05: boundary adapters), the clip and line edge cases, the RLE animations'
   off-screen tests, zero counts that run 2^32 times (Q-DRAW-06), and the scaled image's
   strides. Dropped: the generated row code, the planted rets of the unrolled rows, the
   self-patched operands and the asm's scratch (config/xngine_dropped.csv). */
#ifndef XDRAW_H
#define XDRAW_H

#include "xngine.h"
#include "xdrawhlp.h"
#include "ximg.h"

/* ==== rectangles and image blits (draw.c) =================================================== */

/* A rectangle between the screen and a buffer: (x, y, w, h) on the screen; the buffer's rows
   w + skip bytes apart. The clip moves buf and grows skip as it cuts. */
typedef struct xn_blit {
    s32 x, y, w, h;
    s32 skip;
    u8 *buf;
} xn_blit;

/* (x, y, w, h) filled with text_colour; not clipped; h = 0 runs 2^32 rows (Q-DRAW-06). 13 game
   sites: the options' volume bars, the breath meter, list frames, the text cursor... */
void xn_draw_fill_rect(s32 x, s32 y, s32 w, s32 h);

/* Clips b to the clip window in place: rows above and columns left of it move b->buf on (by
   rows of the unclipped w + skip, and by columns), columns cut on either side add to skip.
   Returns 0 when nothing is left; b is then as far as the clip got (Quirk Q-DRAW-04: after a
   failed top clip y is y - clip_top). */
int xn_draw_clip_rect(xn_blit *b);

/* The clipped screen rectangle b into b->buf (rows w + skip apart); 0 when nothing was left
   (b as the clip left it). */
int xn_draw_get_blit(xn_blit *b);

/* b->buf (rows w + skip apart) onto the clipped screen rectangle b; 0 when nothing was left. */
int xn_draw_put_blit(xn_blit *b);

/* The same, colour 0 transparent. */
int xn_draw_put_blit_transparent(xn_blit *b);

/* Copies the clipped screen rectangle (x, y, w, h) into dst, rows w + dst_skip bytes apart:
   the background under a list, the mouse cursor, a message box. 7 game sites; Quirk
   Q-DRAW-02: they get EAX as the asm left it (the boundary adapter xn_draw_get_rect_b). */
void xn_draw_get_rect(s32 x, s32 y, s32 w, s32 h, u8 *dst, s32 dst_skip);
void xn_draw_get_rect_b(xn_regs *r);

/* Copies src (rows w + src_skip bytes apart) to the clipped screen rectangle (x, y, w, h):
   restores what xn_draw_get_rect saved; sheets and icons. 5 game sites. */
void xn_draw_put_rect(s32 x, s32 y, s32 w, s32 h, const u8 *src, s32 src_skip);

/* The opaque image blit: the w x h pixels at (x, y), clipped. 67 game sites and a pointer the
   game holds; Quirk Q-DRAW-03: they get EAX and ECX as the asm left them (xn_draw_image_b). */
void xn_draw_image(s32 x, s32 y, s32 w, s32 h, const u8 *pixels);
void xn_draw_image_b(xn_regs *r);

/* The image blit with colour 0 transparent: the w x h pixels at (x, y), clipped. 33 game sites
   (the mouse cursor, the compass, books, faces, FLC frames...) and a pointer the game holds;
   Quirk Q-DRAW-03 (xn_draw_image_transparent_b). */
void xn_draw_image_transparent(s32 x, s32 y, s32 w, s32 h, const u8 *pixels);
void xn_draw_image_transparent_b(xn_regs *r);

/* The registers the asm's blits leave (Q-DRAW-03), from the blit as clipped and whether it
   drew: the opaque blit (144F7C): x, y, w, h as clipped when nothing was drawn, else EAX =
   xn_gfx_width - w, ECX = EDX = 0, EBX = w; the transparent one (144FC8): after a blit EAX's
   low byte the last source pixel its row loaded (its other bytes x's), ECX = 0, EDX =
   xn_gfx_width, EBX = 16 w. */
void xn_draw_image_regs_out(const xn_blit *b, int drawn, xn_regs *r);
void xn_draw_image_transparent_regs_out(const xn_blit *b, int drawn, xn_regs *r);

/* Dead: the IMG record img drawn opaque, or with colour 0 transparent, at (x, y). */
void xn_draw_img_record(s32 x, s32 y, const xn_img *img);
void xn_draw_img_record_transparent(s32 x, s32 y, const xn_img *img);

/* One row of the transparent blit: each non-zero pixel of src[0..n) to dst. */
void xn_draw_transparent_row(u8 *dst, const u8 *src, u32 n);

/* Dead: lone rets between functions. */
void xn_draw_unused_ret_144f47(void);
void xn_draw_unused_ret_144fc7(void);

/* ==== lines (drawline.c) =================================================================== */

/* A line's end points, or a rectangle's corners */
typedef struct xn_line {
    s32 x1, y1, x2, y2;
} xn_line;

/* The end point of the last line clipped (xn_draw_line_clip), where xn_draw_line_to starts;
   the game reads and sets them */
extern s32 pen_x;
extern s32 pen_y;

/* The horizontal line [x1, x2) (either order) at row y, clipped, in text_colour (Q-DRAW-07: the
   right end is not drawn). */
void xn_draw_hline(s32 x1, s32 y, s32 x2);

/* The vertical line from row y1 to y2 (either order; Q-DRAW-07: the lower end is not drawn)
   at column x, clipped, in text_colour. */
void xn_draw_vline(s32 x, s32 y1, s32 y2);

/* Dead: the same. */
void xn_draw_vline_keep_regs(s32 x, s32 y1, s32 y2);

/* The line from (x1, y1) to (x2, y2) in text_colour, clipped: the text cursor. 2 game sites. */
void xn_draw_line(s32 x1, s32 y1, s32 x2, s32 y2);

/* The same. */
void xn_draw_line_nosave(s32 x1, s32 y1, s32 x2, s32 y2);

/* A line already clipped: horizontal and vertical ones go to xn_draw_hline and xn_draw_vline;
   otherwise one pixel per step of the major axis, the minor one in 16.16 with a step from the
   reciprocal table. Q-DRAW-07: it stops one pixel short of the end point. */
void xn_draw_line_unclipped(s32 x1, s32 y1, s32 x2, s32 y2);

/* The line from the pen (the last line's end point) to (x, y): the maps, notes, saves. 12 game
   sites. */
void xn_draw_line_to(s32 x, s32 y);

/* Clips the line l to the clip window in place and leaves the pen at its end point: returns 0
   when none of it is inside. The left and right cuts move y along the line, the top and bottom
   ones x. Quirk Q-DRAW-08: a right cut puts x2 at clip_right - 1, a bottom one y2 at
   clip_bottom; a horizontal line on the window's top row is rejected. */
int xn_draw_line_clip(xn_line *l);

/* Clips the rectangle (x1, y1)-(x2, y2) (x1 <= x2, y1 <= y2, right and bottom exclusive) to the
   clip window in place; returns 0 (r unchanged) when it lies outside. */
int xn_draw_clip_rect_xyxy(xn_line *r);

/* Dead: the outline of the rectangle (x1, y1)-(x2, y2) (either order), clipped: its bottom edge
   at the clipped y2, which xn_draw_hline does not draw when it is the window's bottom. */
void xn_draw_rect_outline(s32 x1, s32 y1, s32 x2, s32 y2);

/* Dead: the rectangle (x1, y1)-(x2, y2) (either order) filled with text_colour, clipped
   (Q-DRAW-06: y1 = y2 fills 2^32 rows). */
void xn_draw_fill_rect_clipped(s32 x1, s32 y1, s32 x2, s32 y2);

/* The line from (x1, y1) to (x2, y2), both ends drawn, in text_colour, by Bresenham's steps on
   a 320-byte row step: the list frames. Not clipped. 4 game sites. */
void xn_draw_line_text_colour(s32 x1, s32 y1, s32 x2, s32 y2);

/* ==== images (drawimg.c) =================================================================== */

extern u8 *xn_paperdoll_background;     /* the paperdoll's background copy (the game's) */
extern u8 xn_cel_row_buffer[330];       /* xn_draw_cel_frame's decoded RLE row */

/* src (rows src_stride bytes apart) copied as a w x h block into dst, whose rows are 320 bytes
   apart (dwords, then the bytes left): the options' joystick picture. */
void xn_draw_copy_rect_stride(const u8 *src, u8 *dst, s32 w, s32 h, s32 src_stride);

/* The same a byte at a time: the inventory's paperdoll, the spell icons. 7 game sites; Quirk
   Q-DRAW-05: they get ECX = 0, the asm's spent row count (xn_draw_copy_rect_stride_bytes_b). */
void xn_draw_copy_rect_stride_bytes(const u8 *src, u8 *dst, s32 w, s32 h, s32 src_stride);
void xn_draw_copy_rect_stride_bytes_b(xn_regs *r);

/* Frame `frame` of the RLE group g (a weapon CIF) at its own (x, y + y_add), every pixel through
   color_remap (Q-DRAW-01): literal runs opaque, colour-0 repeats transparent. Rows 320 bytes
   apart. Quirk Q-DRAW-09: rows above the screen's start are skipped run by run: a literal run
   whose first pixel is before screen_buffer, a repeat whose first pixel is at or before it.
   4 game sites. */
void xn_draw_cif_rle_frame(const xn_rle_group *g, s32 frame, s32 y_add);

/* The IMG img at its own (x, y + y_add), colour 0 transparent, others through color_remap,
   clipped at the view's top only; rows 320 apart. Quirk Q-DRAW-10: after a top clip it draws
   from row (y + y_add - clip_top), an index before xn_gfx_row_offset, rather than from
   clip_top. One game site. */
void xn_draw_img_masked_remap(const xn_img *img, s32 y_add);

/* The 320x200 overlay over screen_buffer: overlay colours 0-15 darken the screen pixel through
   shade row 20, others replace it. 9 game sites (windows over the 3D view). */
void xn_draw_fullscreen_overlay_shaded(const u8 *overlay);

/* The screen rectangle (x, y, w, h) darkened through shade row 20 (the info popup); not
   clipped, rows 320 bytes apart. One game site. */
void xn_draw_darken_rect(s32 x, s32 y, s32 w, s32 h);

/* The CEL animation cel at (x, y): its frame frame_counter % frames, in 16 bits (Quirk
   Q-DRAW-11: frames = 0 is the asm's divide error: frame 0), colour 0 transparent; an RLE row
   (bit 31 of its offset) is decoded into xn_cel_row_buffer first. Rows 320 apart. The class
   questions' scroll (one game site). */
void xn_draw_cel_frame(const xn_cel *cel, s32 x, s32 y);

/* The w x h pixels of src (rows 256 bytes apart) at (x, y), colour 0 transparent, each opaque
   pixel also casting colour 9Ch two rows down and two right: the potion maker. 2 game sites. */
void xn_draw_image_drop_shadow(s32 x, s32 y, s32 w, s32 h, const u8 *src);

/* The 16x16 spell icon `icon` (column icon % 20, row icon / 20 of the 320-wide icon_image) at
   (x, y). 4 game sites. */
void xn_draw_spell_icon(s32 x, s32 y, u16 icon);

/* colour into dst (rows 125 bytes apart) wherever src (rows 256 apart) is not 0: the paperdoll's
   pick mask. The game's one site passes dst, src, w, h in EAX ECX EDX EBX and colour on the
   stack, which it pops itself: the boundary adapter xn_draw_paperdoll_mask_b takes that
   interface. */
void xn_draw_paperdoll_mask(u8 *dst, const u8 *src, s32 w, s32 h, u8 colour);
void xn_draw_paperdoll_mask_b(xn_regs *r);

/* The row-compressed paperdoll item image (w x h; its row table, ximg.h) unpacked into scratch_buffer (xn_img_unpack_rows)
   and drawn at (x, y): colour 0 transparent, colour FFh the paperdoll's background
   (xn_paperdoll_background), others through color_remap; rows 320 apart. One game site. */
void xn_draw_paperdoll_item(s32 x, s32 y, s32 w, s32 h, const u8 *image);

/* The RLE image `frame` of images at its own (x, y + y_add), colour 0 transparent, and again
   mirrored about the screen's centre column (x' = 319 - x): the two casting hands. Quirk
   Q-DRAW-09: a literal run skipped above the screen does not move the mirror pointer. 2 game
   sites. */
void xn_draw_cast_anim_mirrored(const xn_img *images, s32 frame, s32 y_add);

/* The h x w pixels of src at the screen's top left, colour 0 transparent: the people map. */
void xn_draw_image_masked_at_origin(s32 h, s32 w, const u8 *src);

/* Dead: when (x, y, w, h) lies inside the clip window (Quirk Q-DRAW-12: and does not touch its
   right or bottom edge), each screen pixel under a non-zero byte of mask (w bytes a row)
   remapped through table. The caller pops the two stack arguments. */
void xn_draw_remap_masked_rect(s32 x, s32 y, s32 w, s32 h, const u8 *mask, const u8 *table);

/* The n bytes of src as a 4n-wide, 4-row block at dst (rows 320 apart): the travel map's
   zoom. */
void xn_draw_zoom4x(const u8 *src, u8 *dst, s32 n);

/* dst[i] = F4h wherever src[i] == value, for count bytes: the travel map's region. */
void xn_draw_mark_matching(const u8 *src, u8 *dst, u8 value, s32 count);

/* Dead: the w x h block at buf (rows w + skip apart) remapped in place through table. */
void xn_draw_remap_rect(u8 *buf, s32 skip, s32 w, s32 h, const u8 *table);

/* ==== unrolled bodies (drawunr.c) ========================================================== */

extern u8 xn_draw_shade_row;            /* the dead shaded blit's shade row (never written) */

/* The hurt flash: colour F6h on every other pixel of the view's rows (clip_top to clip_bottom,
   320 wide), the pattern shifted by one each row. Quirk Q-DRAW-13: an odd row count draws one
   row fewer (3 rows draw 1), and 1 row runs 2^32 pairs. One game site. */
void xn_draw_view_checkerboard(void);

/* Dead: rows of 320 bytes of src copied to dst through table. */
void xn_draw_remap_rows_320(const u8 *src, u8 *dst, const u8 *table, s32 rows);

/* Dead: each of `rows` 320-byte rows of dst filled with the next 8 bytes of src repeated 40
   times. */
void xn_draw_fill_rows_pattern8(const u8 *src, u8 *dst, s32 rows);

/* Clips b like xn_draw_clip_rect, but without its checks of w and h, and rows above the window
   skip w (not w + skip) bytes each. Dead chain (only xn_draw_image_shaded). */
int xn_draw_clip_image_rect(xn_blit *b);

/* Dead: the w x h pixels at (x, y), clipped (xn_draw_clip_image_rect), colour 0 transparent,
   others through shade row xn_draw_shade_row (Quirk Q-DRAW-14: a byte nothing writes: row 0). */
void xn_draw_image_shaded(s32 x, s32 y, s32 w, s32 h, const u8 *src);

/* One row of it: each non-zero pixel of src[0..n) through table to dst. */
void xn_draw_shaded_row(u8 *dst, const u8 *src, u32 n, const u8 *table);

/* Dead: a lone ret between functions. */
void xn_draw_ret_stub(void);

/* ==== the scaled image (drawscl.c) ========================================================= */

/* One column of a scaled row: the source pixel's offset in its row and the screen pixels
   [x, x + count) it covers */
typedef struct xn_scaled_col {
    u16 src;
    u16 count;
    u32 x;
} xn_scaled_col;

/* The most columns a scaled row can have: one per screen column of the widest mode, and one */
#define XN_SCALED_COLS 1025

/* The image src (src_w x src_h; flag 8: row-compressed, unpacked into scratch_buffer first)
   drawn scaled to w x h at (x, y), clipped, colours 0 and FFh transparent, others through
   color_remap with flag 8000h: the inventory and paperdoll items, the cart. 4 game sites. The
   steps are 8.8 (w * 256 / src_w, h * 256 / src_h, 16-bit divides: Q-SYS-01); each source row
   is drawn y-step times, each source pixel x-step times. Quirks Q-DRAW-15 (w or h of 2 or less
   draws nothing; the image is still unpacked when only h is), Q-DRAW-16 (the source rows are
   100h apart but the top clip skips rows of src_w; destination rows are 320 apart). */
void xn_draw_image_scaled(s32 x, s32 y, s32 w, s32 h, s32 src_w, s32 src_h, s32 flags,
                          const u8 *src);

/* Clips the scaled image's top left (*x, *y) to the clip window (16-bit compares): the columns
   and rows cut to *skip_x and *skip_y (0 when none). Returns 0 when the image starts right of
   or below the window. */
int xn_draw_image_scaled_clip(s16 *x, s16 *y, u16 *skip_x, u16 *skip_y);

/* The columns of one scaled row (the asm compiled them into machine code at big_buffer): for
   each of src_w source columns from offset src_ofs, the integer part of the 8.8 accumulator
   (the step step_int.step_frac added per column) screen pixels from x on; a column with none is
   skipped; at the clip window's right edge the column is cut and the row ends. Fills cols (at
   most XN_SCALED_COLS) with the columns that have pixels; returns how many. src_w of 0 means
   65536 (a 16-bit count). */
int xn_draw_image_scaled_row(xn_scaled_col *cols, u32 x, u16 src_ofs, u16 src_w, u8 step_int,
                             u8 step_frac);

#endif
