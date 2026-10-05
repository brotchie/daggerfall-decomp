/* xdrawhlp.h: what the 2D drawing (draw.c, drawline.c, drawimg.c, drawunr.c, drawscl.c),
   the image decoders (img.c) and the fonts (font.c) share: the screen and clip state they read
   (the graphics subsystem's globals), and small helpers that belong in xngine.h (listed for
   the coordinator to promote: XN_KEEP, XN_PUT32, xn_planted_count, xn_copy_bytes,
   xn_fill_bytes, xn_fill_words, xn_fill_dwords). */
#ifndef XDRAWHLP_H
#define XDRAWHLP_H

#include "xngine.h"

/* ---- the screen ------------------------------------------------------------------------ */

extern u8 *screen_buffer;               /* the back buffer every 2D blit writes */
extern s32 xn_gfx_width;                /* its row stride (320) */
extern s32 xn_gfx_row_offset[768];      /* y * xn_gfx_width */
/* the clip rectangle: right and bottom exclusive */
extern s32 xn_gfx_clip_left;
extern s32 xn_gfx_clip_top;
extern s32 xn_gfx_clip_right;
extern s32 xn_gfx_clip_bottom;

extern u8 text_colour;                  /* the colour of text, lines and filled rectangles */
extern u8 *xn_shade_table;              /* 64 rows of 256 colours, darker downwards */
extern u32 xn_colour_fill_table[256];   /* the colour i in all four bytes of a dword */
extern u32 xn_recip16_table[1024];      /* 0xFFFF / i ([0] = 0xFFFF) */

extern u8 *big_buffer;                  /* shared work buffers */
extern u8 *scratch_buffer;
extern u8 *color_remap;                 /* the game's current colour remap (a dye): 256 bytes */

/* A 256-byte colour table as the asm indexes it: the colour goes into AL of the table's
   address (mov al, [eax]), so the table counts as 256-aligned */
#define XN_TABLE256(p) ((const u8 *)((u32)(p) & ~0xFFu))

/* the address of row y, column x of the screen */
#define XN_SCREEN(x, y) (screen_buffer + xn_gfx_row_offset[y] + (x))

/* ---- helpers to promote to xngine.h ------------------------------------------------------ */

/* A self-patched operand: the asm keeps a value of its own call in its code. The C computes
   it in a local and stores the field once, where the asm stores it (the records compare it). */
#ifndef XN_KEEP
#define XN_KEEP(field, v) ((field) = (v))
#endif

/* a dword into a byte stream (generated code) */
#ifndef XN_PUT32
#define XN_PUT32(p, v) (*(u32 *)(p) = (u32)(v))
#endif

/* The count a planted `ret` gives an unrolled body of `max` steps of `stride` bytes: the
   first step whose first byte is C3, or max. For routing a body's asm entry while its
   planter is still asm (the records are taken with the ret planted). */
int xn_planted_count(const void *body, int stride, int max);   /* raster's smc.c */

/* A 16-bit signed divide, as `idiv r/m16` of DX:AX: dividend / d, the remainder to *rem.
   When the quotient does not fit in 16 bits (or d is 0) the asm takes a divide error, after
   which XnGine's handler leaves AX and DX 0: so does this (it divides by 0 to take the same
   exception). */
s16 xn_idiv16(s32 dividend, s32 d, s16 *rem);

/* n bytes, as `rep movsb` (forwards, a byte at a time: overlap behaves as the asm's) */
void xn_copy_bytes(u8 *dst, const u8 *src, u32 n);
#pragma aux xn_copy_bytes = "rep movsb" parm [edi] [esi] [ecx] modify exact [edi esi ecx];

/* n bytes of v, as `rep stosb` */
void xn_fill_bytes(u8 *dst, u8 v, u32 n);
#pragma aux xn_fill_bytes = "rep stosb" parm [edi] [eax] [ecx] modify exact [edi ecx];

/* n words of v, as `rep stosw` */
void xn_fill_words(void *dst, u16 v, u32 n);
#pragma aux xn_fill_words = "rep stosw" parm [edi] [eax] [ecx] modify exact [edi ecx];

/* n dwords of v, as `rep stosd` */
void xn_fill_dwords(void *dst, u32 v, u32 n);
#pragma aux xn_fill_dwords = "rep stosd" parm [edi] [eax] [ecx] modify exact [edi ecx];

/* n dwords, as `rep movsd` */
void xn_copy_dwords(void *dst, const void *src, u32 n);
#pragma aux xn_copy_dwords = "rep movsd" parm [edi] [esi] [ecx] modify exact [edi esi ecx];

#endif
