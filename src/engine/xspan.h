/* xspan.h: XnGine's span routines (span.c; see xngine.h and docs/engine/smc/span.md).

   A span routine draws one run of pixels of one polygon on one screen row: xn_render_frame
   calls the polygon's +3Ch for each span node, xn_flat_span_emit a flat's. Each divides for
   perspective at the span's start and then every 8 or 16 pixels, and fills the pixels in
   between affinely. The asm unrolls the 8- or 16-pixel blocks and ends the span with an
   unrolled body (the "tail") stopped by a ret it plants; here both are one loop (the *_run
   helpers of span.c), and a tail's asm entry is the RET adapter: its count is where the
   planter put the ret.

   The routines patch operands of their own code to use later in the same call (the block
   loop's end, the steps, the texture origin); the C keeps those values in locals and stores
   each field once (XN_KEEP), because the records see the code bytes. Operands other functions
   patch (the 1/z table's address, the view centre, the lit spans' rays) are read where the
   asm reads them.

   The natural functions take the polygon, the span node, x - the view centre, the pixel count
   and the first pixel; the asm passes eax, esi, ebx, ebp and edi = the first pixel - 1 (the
   glue NAME_r unpacks them). */
#ifndef XSPAN_H
#define XSPAN_H

#include "xngine.h"
#include "xnstruct.h"

extern s32 xn_render_row_y;             /* the row being drawn - the view centre's */
extern s32 xn_cam_dir_x_mid[];          /* the x ray of a column, by x - centre x */
extern s32 xn_recip16_table[];          /* 2^16 / n */
extern s32 xn_recip32_table[];          /* 2^32 / n */
extern u32 xn_colour_fill_table[256];   /* a colour four times (a dword of it) */

/* where a planted ret stops a tail after n pixels (n * the tail's step bytes) */
extern u16 xn_span_solid_lit_tail_offsets[16];  /* 9 n */
extern u16 xn_span_tail_offsets_17[16];         /* 17 n */
extern u16 xn_span_tail_offsets_19[16];         /* 19 n */
extern u16 xn_water_unroll_offsets[];           /* 21 n (the water's table) */
extern u16 xn_span_tail_offsets_23[8];          /* 23 n */
extern u16 xn_span_tail_offsets_25[8];          /* 25 n */
extern u32 xn_tmap_ret_offsets[16];             /* a tmap copy's pixel n */

/* ---- solid colour ----------------------------------------------------------------------- */

/* n pixels of the polygon's colour (+40h: the colour four times). */
void xn_span_solid(struct xn_poly *poly, s32 n, u8 *pix);
void xn_span_solid_r(xn_regs *r);

/* The first span of a solid polygon with a shade row: the colour through the row becomes the
   polygon's colour, xn_span_solid its routine, and the span is drawn. */
void xn_span_solid_shaded_setup(struct xn_poly *poly, s32 n, u8 *pix);
void xn_span_solid_shaded_setup_r(xn_regs *r);

/* A solid polygon lit per pixel by its compiled light shader (+14h): the shade every 16
   pixels (and at the span's end), interpolated in between; the pixel is the colour through
   the shade's row. */
void xn_span_solid_lit(struct xn_poly *poly, const struct xn_span *span, s32 xs, s32 n, u8 *pix);
void xn_span_solid_lit_r(xn_regs *r);
void xn_span_solid_lit_tail_r(xn_regs *r);

/* ---- textured -------------------------------------------------------------------------------- */

/* A textured polygon: texels at u/z, v/z divided by 1/z every 8 pixels (_8) or 16 (_16: its
   setup doubles the polygon's 8-pixel steps once and installs it), wrapped by the texture's
   mask. The _shaded ones map every texel through the polygon's shade row (+14h). */
void xn_span_tex_8(struct xn_poly *poly, const struct xn_span *span, s32 xs, s32 n, u8 *pix);
void xn_span_tex_8_r(xn_regs *r);
void xn_span_tex_16_setup(struct xn_poly *poly, const struct xn_span *span, s32 xs, s32 n,
                          u8 *pix);
void xn_span_tex_16_setup_r(xn_regs *r);
void xn_span_tex_16(struct xn_poly *poly, const struct xn_span *span, s32 xs, s32 n, u8 *pix);
void xn_span_tex_16_r(xn_regs *r);
void xn_span_tex_tail_r(xn_regs *r);

void xn_span_tex_shaded_8(struct xn_poly *poly, const struct xn_span *span, s32 xs, s32 n,
                          u8 *pix);
void xn_span_tex_shaded_8_r(xn_regs *r);
void xn_span_tex_shaded_16_setup(struct xn_poly *poly, const struct xn_span *span, s32 xs,
                                 s32 n, u8 *pix);
void xn_span_tex_shaded_16_setup_r(xn_regs *r);
void xn_span_tex_shaded_16(struct xn_poly *poly, const struct xn_span *span, s32 xs, s32 n,
                           u8 *pix);
void xn_span_tex_shaded_16_r(xn_regs *r);
void xn_span_tex_shaded_tail_r(xn_regs *r);

/* A textured polygon lit per pixel: every 16 pixels the light shader gives the shade and the
   texture's compiled mapper (xn_tmap_run) draws the pixels in between. Its setup doubles the
   steps, swaps the halves of the packed texture origin (u high, as the mapper wants) and
   installs it. */
void xn_span_tex_lit_setup(struct xn_poly *poly, const struct xn_span *span, s32 xs, s32 n,
                           u8 *pix);
void xn_span_tex_lit_setup_r(xn_regs *r);
void xn_span_tex_lit(struct xn_poly *poly, const struct xn_span *span, s32 xs, s32 n, u8 *pix);
void xn_span_tex_lit_r(xn_regs *r);

/* The terrain's 64 x 64 textures (the offset wrapped by 3F3Fh), divided every 16 pixels with
   low-dword products, and exactly at the span's end; _shaded through the shade row. */
void xn_span_tex64(struct xn_poly *poly, const struct xn_span *span, s32 xs, s32 n, u8 *pix);
void xn_span_tex64_r(xn_regs *r);
void xn_span_tex64_tail_r(xn_regs *r);
void xn_span_tex64_shaded(struct xn_poly *poly, const struct xn_span *span, s32 xs, s32 n,
                          u8 *pix);
void xn_span_tex64_shaded_r(xn_regs *r);
void xn_span_tex64_shaded_tail_r(xn_regs *r);

/* ---- flats (billboards) ----------------------------------------------------------------------- */

/* A span of a flat: one divide (inv_z: its 1/z), then an exact 8-pixel DDA; texel 0 is
   transparent. Plain; through the flat's shade row (+10h); through the light row and then the
   fog row (+4Ch); or blended with the screen through a translucency table
   (table[texel << 8 | pixel]). x is the screen column. */
void xn_span_flat_transparent(const struct xn_flat *flat, u32 inv_z, s32 x, s32 n, u8 *pix);
void xn_span_flat_transparent_r(xn_regs *r);
void xn_span_flat_transparent_tail_r(xn_regs *r);
void xn_span_flat_transparent_shaded(const struct xn_flat *flat, u32 inv_z, s32 x, s32 n,
                                     u8 *pix);
void xn_span_flat_transparent_shaded_r(xn_regs *r);
void xn_span_flat_transparent_shaded_tail_r(xn_regs *r);
void xn_span_flat_lit_fogged(const struct xn_flat *flat, u32 inv_z, s32 x, s32 n, u8 *pix);
void xn_span_flat_lit_fogged_r(xn_regs *r);
void xn_span_flat_lit_fogged_tail_r(xn_regs *r);
void xn_span_flat_translucent(const struct xn_flat *flat, u32 inv_z, s32 x, s32 n, u8 *pix);
void xn_span_flat_translucent_r(xn_regs *r);
void xn_span_flat_translucent_tail_r(xn_regs *r);

/* ---- the S-buffer ------------------------------------------------------------------------------ */

extern s32 xn_span_dzdx;                /* d(1/z)/dx of the polygon being rasterized */
extern struct xn_span *xn_render_span_next;     /* the next free span node */
extern struct xn_poly *xn_render_poly_next;     /* the polygon being built */

/* Puts the span [x0, x1) of the polygon being built, 1/z inv_z at x0, into a row's sorted
   list after `prev` (the row's head): where it overlaps a span, the nearer one at each pixel
   wins (1/z, then the slope on a tie at the start); spans are trimmed, split or dropped, and
   the new pieces take nodes from xn_render_span_next. */
void xn_span_insert(struct xn_span *prev, s32 x0, s32 x1, s32 inv_z);
void xn_span_insert_r(xn_regs *r);

#endif
