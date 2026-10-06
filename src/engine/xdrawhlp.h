/* xdrawhlp.h: what the 2D drawing (draw.c, drawline.c, drawimg.c, drawunr.c, drawscl.c), the
   image decoders (img.c) and the fonts (font.c) share: the screen (xgfx.h), the colour tables
   and work buffers they read, and the byte-copy and fill helpers.

   The helpers are compiler support, like xngine.h's 64-bit products: the engine is built
   without a C library (-zl), and Watcom C32 10.0a turns a loop of dword stores into a call of
   its library's __STOSD, which is not there. Each is one string instruction (rep movsb...),
   with C's meaning (memcpy forwards a byte at a time, memset); listed for xngine.h. */
#ifndef XDRAWHLP_H
#define XDRAWHLP_H

#include "xngine.h"
#include "xgfx.h"
#include "ptrint.h"                     /* uptr: an int that holds an address */

/* ---- colour tables and buffers -------------------------------------------------------------- */

extern u8 *xn_shade_table;              /* 64 rows of 256 colours, darker downwards (SHADE.nnn) */
extern u32 xn_recip16_table[1024];      /* 0xFFFF / i ([0] = 0xFFFF): 16.16 line steps */
extern u8 *big_buffer;                  /* the engine's work buffer (xn_mem_work_block) */
extern u8 *scratch_buffer;              /* the game's work buffer */
extern u8 *color_remap;                 /* the game's current colour remap (a dye): 256 bytes */

/* A 256-byte colour table as the asm indexes it: the colour is loaded into AL of the table's
   address (mov al, [eax]), so the low byte of the table's address is replaced, not added to.
   Quirk Q-DRAW-01: a table that is not 256-aligned is read from the 256 bytes before its start
   (every table the game passes is aligned: the shade rows and the dye remaps). */
#define XN_TABLE256(p) ((const u8 *)((uptr)(p) & ~(uptr)0xFF))

/* Whether a + b <= 0 as the asm's `add a, b; jle` decides it: by the true sum (the CPU's SF != OF
   or ZF), which a sum that overflows 32 bits does not have the sign of. The clips test their
   cuts this way (draw.c). */
int xn_add_le0(s32 a, s32 b);

/* ---- copies and fills (compiler support) --------------------------------------------------- */

#ifndef DAGGER_PORT
/* n bytes forwards, one at a time (an overlap repeats bytes as memcpy's simplest loop does) */
void xn_copy_bytes(u8 *dst, const u8 *src, u32 n);
#pragma aux xn_copy_bytes = "rep movsb" parm [edi] [esi] [ecx] modify exact [edi esi ecx];

/* n dwords */
void xn_copy_dwords(void *dst, const void *src, u32 n);
#pragma aux xn_copy_dwords = "rep movsd" parm [edi] [esi] [ecx] modify exact [edi esi ecx];

/* n bytes of v */
void xn_fill_bytes(u8 *dst, u8 v, u32 n);
#pragma aux xn_fill_bytes = "rep stosb" parm [edi] [eax] [ecx] modify exact [edi ecx];

/* n dwords of v */
void xn_fill_dwords(void *dst, u32 v, u32 n);
#pragma aux xn_fill_dwords = "rep stosd" parm [edi] [eax] [ecx] modify exact [edi ecx];

#else
/* The native build: the same string instructions as loops, forwards (an overlapping copy
   repeats bytes or dwords as rep movs does: not memcpy, not memmove) */
static __inline__ void xn_copy_bytes(u8 *dst, const u8 *src, u32 n)
{
    while (n-- != 0)
        *dst++ = *src++;
}

static __inline__ void xn_copy_dwords(void *dst, const void *src, u32 n)
{
    u8 *d = (u8 *)dst;
    const u8 *s = (const u8 *)src;

    for (; n != 0; n--, d += 4, s += 4) {   /* a dword at a time: unaligned, as movsd */
        u32 v;

        __builtin_memcpy(&v, s, 4);
        __builtin_memcpy(d, &v, 4);
    }
}

static __inline__ void xn_fill_bytes(u8 *dst, u8 v, u32 n)
{
    while (n-- != 0)
        *dst++ = v;
}

static __inline__ void xn_fill_dwords(void *dst, u32 v, u32 n)
{
    u8 *d = (u8 *)dst;

    for (; n != 0; n--, d += 4)
        __builtin_memcpy(d, &v, 4);
}
#endif

#endif
