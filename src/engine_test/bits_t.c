/* bits_t.c: test shims of src/engine/bits.c (built only by tools/xn_rc.py; docs/
   xngine_canonical.md). */
#include "xbits.h"

/* flags EAX, mask DL, clear EBX */
void xn_bits_set_or_clear_u8_r(xn_regs *r)
{
    xn_bits_set_or_clear_u8((u8 *)r->eax, (u8)r->edx, r->ebx);
}

/* flags EAX, mask DX, clear EBX */
void xn_bits_set_or_clear_u16_r(xn_regs *r)
{
    xn_bits_set_or_clear_u16((u16 *)r->eax, (u16)r->edx, r->ebx);
}
