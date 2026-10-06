/* xtmap.h: XnGine's texture mapper (src/engine/tmap.c). Canonical C: plain prototypes,
   Watcom's own calling convention; docs/xngine_canonical.md.

   What it does
     The lit textured spans (xn_span_tex_lit) draw 16 pixels at a time through the texture
     mapper: texels at a packed coordinate that steps every pixel, wrapped by the texture's
     size masks, each through the shade row of its pixel pair. The asm compiled one copy of
     the mapper per texture (the engine's messages call them "Shaders": the template
     xn_tmap_template patched with the texture's mask and texels, copied into a slot of a
     768-slot pool in the game's heap) and ran the copies; canonical C is one routine that
     takes the texels and the mask (xn_tmap_draw).

     The pool stays as the texture cache sees it: xn_tmap_pool_alloc takes its block from
     the game's allocator, xn_tmap_compile hands each texture record its slot's address (the
     record keeps it, in the texture heap the game reads) and fails, setting
     xn_tex_cache_full, once the pool is full; nothing is written into the slots.

   Units and formats
     uv       u in the high half and v in the low half, 8.8 each (the lit spans' order);
              the texel is texels[(v_int & v mask) << 8 | (u_int & u mask)].
     mask     v mask << 8 | u mask: each the texture's size - 1 (xn_tex_size_mask).
     shade    a shade row's address with an 8-bit fraction; one row per pixel pair.

   Globals (in object 2, the engine's): xn_tmap_pool_block, xn_tmap_pool, xn_tmap_pool_count,
   xn_tex_cache_full (the texture cache's), xn_tmap_msg_no_memory.

   Quirks kept (docs/engine/quirks.md): Q-TMAP-01 (the count goes up before the full test:
   the 768th compile fails and slot 767 is never handed out). */
#ifndef XTMAP_H
#define XTMAP_H

#include "xngine.h"
#include "xnstruct.h"

#define XN_TMAP_SLOTS       768         /* slots in the pool */
#define XN_TMAP_SLOT_BYTES  418         /* the asm's compiled copy: 416 bytes of code, ret, 0 */

extern u8 *xn_tmap_pool_block;          /* the pool's block (game malloc) */
extern u8 *xn_tmap_pool;                /* 32-aligned in it: the slots */
extern u32 xn_tmap_pool_count;          /* slots handed out (Q-TMAP-01) */
extern u8 xn_tex_cache_full;            /* set when a texture could not be placed */
extern char xn_tmap_msg_no_memory[];    /* "XnGine: Out of memory for Shaders.$" */

/* Allocates the pool (768 slots of 418 bytes, 32-aligned) from the game's allocator and
   empties it; without the memory, shuts the engine down and ends the program with a
   message. The texture cache's start-up. */
void xn_tmap_pool_alloc(void);

/* Frees the pool's block, if any. */
void xn_tmap_pool_free(void);

/* A texture record's mapper handle: the next slot's address, or 0 with xn_tex_cache_full set
   when the pool is full (Q-TMAP-01). entry and mask: the record and its size masks (the
   asm's copy baked them in; canonical C's mapper takes them from the polygon). */
void *xn_tmap_compile(const struct xn_tex_entry *entry, u32 mask);

/* The asm rewrote a copy's texel bases when its texture's frame moved; canonical C's mapper
   reads the polygon's texels, so there is nothing to do. */
void xn_tmap_rebase(void *handle, u8 *texels);

/* Empties the pool (the texture cache's flush). */
void xn_tmap_pool_reset(void);

/* Draws pix[0..n-1] (n <= 16): pixel k is texels[uv's texel & mask] through the shade row of
   its pair (shade, then shade + shade_step for pixels 2-3, ...); uv += step every pixel.
   shade: a row's address, its low byte a fraction (uptr). */
void xn_tmap_draw(u8 *pix, s32 n, u32 uv, u32 step, uptr shade, s32 shade_step,
                  const u8 *texels, u32 mask);

#endif
