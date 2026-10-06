/* xbits.h: XnGine's flag helpers (src/engine/bits.c). Canonical C: plain prototypes, Watcom's
   own calling convention; docs/xngine_canonical.md.

   Two helpers the game's class maker uses to set or clear bits of its flag bytes and words
   (its advantages and disadvantages). */
#ifndef XBITS_H
#define XBITS_H

#include "xngine.h"

/* *flags &= ~mask when clear is not 0, else *flags |= mask. classmaker_set_advantage and
   _disadvantage (10 game sites). */
void xn_bits_set_or_clear_u8(u8 *flags, u8 mask, s32 clear);

/* The same for a flag word (12 game sites). */
void xn_bits_set_or_clear_u16(u16 *flags, u16 mask, s32 clear);

#endif
