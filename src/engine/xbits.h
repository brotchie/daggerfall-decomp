/* xbits.h: XnGine's flag helpers (src/engine/bits.c; see xngine.h). */
#ifndef XBITS_H
#define XBITS_H

#include "xngine.h"

/* *flags &= ~mask when clear, else *flags |= mask. classmaker_set_advantage and
   _disadvantage set and clear the class flag bytes with it. */
void xn_bits_set_or_clear_u8(u8 *flags, u8 mask, s32 clear);

/* The same for a flag word. */
void xn_bits_set_or_clear_u16(u16 *flags, u16 mask, s32 clear);

#endif
