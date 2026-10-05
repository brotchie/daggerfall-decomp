/* bits.c: XnGine's flag helpers as readable C (xbits.h; see xngine.h). */
#include "xbits.h"

void xn_bits_set_or_clear_u8(u8 *flags, u8 mask, s32 clear)
{
    if (clear)
        *flags &= ~mask;
    else
        *flags |= mask;
}

void xn_bits_set_or_clear_u16(u16 *flags, u16 mask, s32 clear)
{
    if (clear)
        *flags &= ~mask;
    else
        *flags |= mask;
}
