/* xtmap.h: XnGine's texture-mapper code generator (tmap.c; see xngine.h).

   XnGine compiles one copy of a 16-pixel texture-mapping loop per texture (the engine's
   messages call them "Shaders"): the template xn_tmap_template (15C300, 8 two-pixel steps,
   struct xn_tmap_copy) with the texture's wrap mask and texel base patched into every fetch,
   copied into a slot of a 768-slot heap pool. xn_span_tex_lit runs them. The generator stays
   byte-exact (the template's in-place patches and the slot copies are what the records see);
   the runner is C that reads the copy's mask and texel fields (the design's option B). */
#ifndef XTMAP_H
#define XTMAP_H

#include "xngine.h"
#include "xnstruct.h"

#define XN_TMAP_SLOTS   768             /* slots in the pool; the 768th compile fails */

extern struct xn_tmap_copy xn_tmap_template;    /* 15C300: code, patched in place */
extern u8 *xn_tmap_pool_block;          /* the pool's block (game malloc) */
extern u8 *xn_tmap_pool;                /* 32-aligned in it: the slots */
extern u8 *xn_tmap_pool_end;            /* pool + 10000h; never read */
extern u32 xn_tmap_pool_count;          /* slots used */
extern u8 xn_tex_cache_full;            /* set when a texture could not be placed */
extern char xn_tmap_msg_no_memory[];    /* "XnGine: Out of memory for Shaders.$" */
extern u32 xn_tmap_ret_offsets[16];     /* 0, 28, 52, 80...: pixel n's code in a copy */

/* Allocates the pool (768 slots of 418 bytes, 32-aligned) with the game's malloc, and
   empties it; without the memory, shuts the engine down and exits to DOS with a message.
   Keeps every register (pushad). */
void xn_tmap_pool_alloc(void);
void xn_tmap_pool_alloc_r(xn_regs *r);

/* Frees the pool's block, if any. Keeps every register (pushad). */
void xn_tmap_pool_free(void);
void xn_tmap_pool_free_r(xn_regs *r);

/* Compiles a mapper for a texture record (its directory entry): patches the template's 8
   steps with the wrap mask and the record's texels, and copies it into the next slot.
   Returns the copy, or 0 (the asm's CF) with xn_tex_cache_full set when the pool is full.
   The count goes up before the test and is not put back: the 768th compile and every later
   one fail, and slot 767 is never used. */
struct xn_tmap_copy *xn_tmap_compile(const struct xn_tex_entry *entry, u32 mask);
void xn_tmap_compile_r(xn_regs *r);

/* A copy's 16 texel bases rewritten for a texture that moved in the heap (not its masks). */
void xn_tmap_rebase(struct xn_tmap_copy *copy, u8 *texels);

/* Empties the pool (the texture cache's flush). */
void xn_tmap_pool_reset(void);

/* What a copy computes, for n of its 16 pixels (n = 16, or 1..15 where the asm plants a ret
   at xn_tmap_ret_offsets[n]): pixel k at pix[k] is the texel at uv (u in the high half, v
   in the low half, 8.8 each; masked after the bytes are swapped), through the shade row of
   its pixel pair (shade, with an 8-bit fraction, steps by shade_step every two pixels); uv
   steps by step every pixel. Reads each step's own mask and texel fields of the copy and
   writes no code (the design's option B). */
void xn_tmap_run(const struct xn_tmap_copy *copy, u8 *pix, int n, u32 uv, u32 step, u32 shade,
                 s32 shade_step);

#endif
