/* xspan.h: XnGine's span routines and its S-buffer (src/engine/span.c). Canonical C: plain
   prototypes, Watcom's own calling convention; docs/xngine_canonical.md.

   What it does
     The rasteriser draws the frame span by span: a span is one run of pixels of one polygon
     on one screen row. The model faces and the terrain cells go through the S-buffer first:
     xn_span_insert keeps every row's spans sorted and non-overlapping, the nearer polygon
     winning at each pixel, so every screen pixel is drawn once. xn_render_frame then calls
     each span's polygon's routine (its +3Ch, an xn_span_fn) row by row. The flats
     (billboards) are drawn at once, span by span, by xn_flat_span_emit through the flat's
     routine (+3Ch, an xn_flat_span_fn).

     A polygon's routine starts as a setup (xn_render_span_setup) that lights the polygon,
     stores the routine its lighting chooses and runs it. The routines here:
       solid colour     xn_span_solid (the colour), xn_span_solid_lit (lit per pixel);
       textured         xn_span_tex_8 and _16 (perspective-correct every 8 or 16 pixels),
                        xn_span_tex_shaded_8 and _16 (through the polygon's shade row),
                        xn_span_tex_lit (lit per pixel, through the texture mapper);
       terrain          xn_span_tex64 and xn_span_tex64_shaded (64 x 64 textures);
       flats            xn_span_flat_transparent, _transparent_shaded, _lit_fogged and
                        _translucent (texel 0 transparent).
     Each divides for perspective at the span's start and then every 8 or 16 pixels (a
     "block"), and steps affinely in between; the pixels after the last whole block (the
     "tail", fewer than a block) are the *_tail functions, which take their count.

   Units and formats
     inv_z    1/z as the projectors store it: 2^40 / z (z in view units).
     z        from the 1/z table (xn_render_recip_table: entry k = 2^24 / k) at
              (inv_z >> 13) & FFFFh: the polygon spans use entry << 9 (z << 8, for the high
              dword of the products), the terrain and flat spans the entry itself (low
              dwords); the lit spans divide 2^46 / inv_z exactly.
     u/z, v/z the polygon's gradients (+24h..+38h: per screen x, per row, at the centre);
              u = (u/z) * z, the high dword of the product.
     uv       a packed texture coordinate: v in the high half and u in the low half, 8.8
              each; the texel is texels[v_int << 8 | u_int] (textures are 256 bytes a row).
              The lit spans and the texture mapper keep u high and v low (xtmap.h).
     shade    a shade row's address in xn_shade_table (64 rows of 256) with an 8-bit
              fraction in its low byte; the pixel is row[texel] (the low byte replaced).
     xs       the span's first pixel's x minus the view centre's x (polygon spans); x the
              screen column (flat spans); n the pixel count; pix the span's first pixel.

   State read (none is the span routines' own: each call is complete)
     xn_render_recip_table (render), xn_render_row_y (the frame's row - the centre's),
     xn_cam_dir_x_mid / xn_cam_dir_y_mid (the camera's rays per column and row; the 16-pixel
     x step is xn_cam_dir_x_mid[16] = 2^22 / focal x), xn_cam_centre_x, xn_recip16_table and
     xn_recip32_table (FFFFh / n, FFFFFFFFh / n: the tails' steps), xn_colour_fill_table, the
     shade table; the S-buffer: xn_render_span_next, xn_render_poly_next, xn_span_dzdx.

   The asm kept each routine's working values in its own code (patched operands, scratch
   globals) and stopped its unrolled tails with planted rets; canonical C keeps them in
   locals and passes counts (config/xngine_dropped.csv lists what the asm wrote).

   Quirks kept (docs/engine/quirks.md): Q-SPAN-01 (the lit spans' shade carries never reach
   the row's high half), Q-SPAN-02 (the lit textured tail takes the shade at pixel n with the
   1/z of pixel n - 1), Q-SPAN-03 (the S-buffer compares ends as 16-bit and offsets only a
   low word on a split), Q-SPAN-04 (a 1/z of 0 or below 4000h gives a shade at z = 0). */
#ifndef XSPAN_H
#define XSPAN_H

#include "xngine.h"
#include "xnstruct.h"

/* A polygon's span routine: draws pixels pix[0..n-1] of `span` (xs: the first pixel's x - the
   view centre's x). xn_render_frame calls it for every span of the polygon. */
typedef void (*xn_span_fn)(struct xn_poly *poly, const struct xn_span *span, s32 xs, s32 n,
                           u8 *pix);

/* A flat's span routine: pixels pix[0..n-1] of screen column x on, at 1/z inv_z (the flat's
   one depth). xn_flat_span_emit calls it. */
typedef void (*xn_flat_span_fn)(struct xn_flat *flat, u32 inv_z, s32 x, s32 n, u8 *pix);

extern s32 xn_render_row_y;             /* the row being drawn - the view centre's (render) */
extern s32 xn_cam_dir_x_mid[];          /* the camera's x ray of column x - centre x */
extern s32 xn_cam_dir_y_mid[];          /* the y ray of row y - centre y */
extern s32 xn_cam_centre_x;
extern s32 *xn_render_recip_table;      /* 2^24 / k, k = 0..65536 (xn_render_init) */
extern u32 xn_recip16_table[1024];      /* FFFFh / n (xmem.h) */
extern u32 xn_recip32_table[1024];      /* FFFFFFFFh / n */
extern u32 xn_colour_fill_table[256];   /* a colour in all four bytes */

/* ---- solid colour ----------------------------------------------------------------------- */

/* n pixels of the polygon's colour (+40h: the colour four times). */
void xn_span_solid(struct xn_poly *poly, const struct xn_span *span, s32 xs, s32 n, u8 *pix);

/* The first span of a solid polygon with one shade row (lighting kind 4): its colour becomes
   the colour through the row (+14h), xn_span_solid its routine, and the span is drawn. */
void xn_span_solid_shaded_setup(struct xn_poly *poly, const struct xn_span *span, s32 xs,
                                s32 n, u8 *pix);

/* A solid polygon lit per pixel by its light shader (+14h, kind 8): the shade at the span's
   start, every 16 pixels and at the end (xn_light_shade at the pixel's view-space point),
   interpolated in between, one shade per pixel pair in the 16-pixel blocks; the pixel is
   the colour through the shade's row. Q-SPAN-01, Q-SPAN-04. */
void xn_span_solid_lit(struct xn_poly *poly, const struct xn_span *span, s32 xs, s32 n,
                       u8 *pix);

/* The lit span's last n pixels (n < 16): the colour through a shade that starts at `shade`
   and moves by `step` a pixel. base: the start shade with the colour in its low byte; only
   the shade's bits 8-15 move the address (Q-SPAN-01). Both are addresses (uptr). */
void xn_span_solid_lit_tail(u8 *pix, s32 n, uptr base, uptr shade, s32 step);

/* ---- textured ------------------------------------------------------------------------------ */

/* A textured polygon with no lighting (kind 0) or one shade row (+14h, kind 4, the _shaded
   ones): texels at (u/z) * z, (v/z) * z, the divide every 8 pixels (_8) or 16 (_16),
   stepped in between and wrapped by the texture's mask (+4Ch) after every step. */
void xn_span_tex_8(struct xn_poly *poly, const struct xn_span *span, s32 xs, s32 n, u8 *pix);
void xn_span_tex_shaded_8(struct xn_poly *poly, const struct xn_span *span, s32 xs, s32 n,
                          u8 *pix);

/* The 16-pixel routines' first span: the polygon's 8-pixel steps (+50h..+58h) doubled, the
   16-pixel routine stored as its routine and run. */
void xn_span_tex_16_setup(struct xn_poly *poly, const struct xn_span *span, s32 xs, s32 n,
                          u8 *pix);
void xn_span_tex_16(struct xn_poly *poly, const struct xn_span *span, s32 xs, s32 n, u8 *pix);
void xn_span_tex_shaded_16_setup(struct xn_poly *poly, const struct xn_span *span, s32 xs,
                                 s32 n, u8 *pix);
void xn_span_tex_shaded_16(struct xn_poly *poly, const struct xn_span *span, s32 xs, s32 n,
                           u8 *pix);

/* The textured spans' last n pixels: texels[uv's texel] (through `row` when it is not 0),
   uv += step then & mask after each pixel. */
void xn_span_tex_tail(u8 *pix, s32 n, u32 uv, u32 step, u32 mask, const u8 *texels);
void xn_span_tex_shaded_tail(u8 *pix, s32 n, u32 uv, u32 step, u32 mask, const u8 *texels,
                             const u8 *row);

/* A textured polygon lit per pixel (kind 8): every 16 pixels the light shader's shade and
   the perspective divide, and the texture mapper (xn_tmap_draw) in between. Its first span
   (the setup) doubles the steps and swaps the halves of the packed texture origin (+18h: u
   high, as the mapper takes it). Q-SPAN-02, Q-SPAN-04. */
void xn_span_tex_lit_setup(struct xn_poly *poly, const struct xn_span *span, s32 xs, s32 n,
                           u8 *pix);
void xn_span_tex_lit(struct xn_poly *poly, const struct xn_span *span, s32 xs, s32 n, u8 *pix);

/* ---- the terrain's 64 x 64 textures ------------------------------------------------------- */

/* A terrain cell (kind 0, or 4 through the shade row): divided every 16 pixels with the
   products' low dwords and exactly at the span's end; the offset wrapped by 3F3Fh, the
   coordinate itself never wrapped. Its setup (xn_render_span_setup_terrain) makes the steps
   16-pixel ones. */
void xn_span_tex64(struct xn_poly *poly, const struct xn_span *span, s32 xs, s32 n, u8 *pix);
void xn_span_tex64_shaded(struct xn_poly *poly, const struct xn_span *span, s32 xs, s32 n,
                          u8 *pix);

/* Their last n pixels: texels[uv's texel & mask] (through row when not 0), uv += step. */
void xn_span_tex64_tail(u8 *pix, s32 n, u32 uv, u32 step, u32 mask, const u8 *texels);
void xn_span_tex64_shaded_tail(u8 *pix, s32 n, u32 uv, u32 step, u32 mask, const u8 *texels,
                               const u8 *row);

/* ---- flats (billboards) ------------------------------------------------------------------- */

/* A span of a flat: one divide (the 1/z table at inv_z >> 13), then an exact 8-pixel DDA;
   texel 0 is transparent. Plain; through the flat's shade row (+10h); through the light row
   and then the fog row (+4Ch); or blended with the screen through a translucency table
   (+4Ch: table[texel << 8 | pixel]). xn_span_flat_translucent never ran in play. */
void xn_span_flat_transparent(struct xn_flat *flat, u32 inv_z, s32 x, s32 n, u8 *pix);
void xn_span_flat_transparent_shaded(struct xn_flat *flat, u32 inv_z, s32 x, s32 n, u8 *pix);
void xn_span_flat_lit_fogged(struct xn_flat *flat, u32 inv_z, s32 x, s32 n, u8 *pix);
void xn_span_flat_translucent(struct xn_flat *flat, u32 inv_z, s32 x, s32 n, u8 *pix);

/* Their last n pixels (n < 8): texels[uv's texel], uv += step; texel 0 left out. row, fog:
   the shade and fog rows; table: the translucency table. */
void xn_span_flat_transparent_tail(u8 *pix, s32 n, u32 uv, u32 step, const u8 *texels);
void xn_span_flat_transparent_shaded_tail(u8 *pix, s32 n, u32 uv, u32 step, const u8 *texels,
                                          const u8 *row);
void xn_span_flat_lit_fogged_tail(u8 *pix, s32 n, u32 uv, u32 step, const u8 *texels,
                                  const u8 *row, const u8 *fog);
void xn_span_flat_translucent_tail(u8 *pix, s32 n, u32 uv, u32 step, const u8 *texels,
                                   const u8 *table);

/* ---- the S-buffer ---------------------------------------------------------------------------- */

extern s32 xn_span_dzdx;                /* d(1/z)/dx of the polygon being built (the model and
                                           terrain code set it with its +5Ch) */
extern struct xn_span *xn_render_span_next;     /* the next free span node */
extern struct xn_poly *xn_render_poly_next;     /* the polygon being built */

/* Puts the span [x0, x1) of the polygon being built, 1/z inv_z at x0, into the row whose
   list head is `head`: where it overlaps a span, the nearer one at each pixel wins (by 1/z,
   then by the slope on a tie at a shared start); spans are trimmed, split or dropped, and
   the new pieces take nodes from xn_render_span_next. The rasteriser (xn_poly_rasterize)
   calls it for every row of a polygon. Q-SPAN-03. */
void xn_span_insert(struct xn_span *head, s32 x0, s32 x1, s32 inv_z);

#endif
