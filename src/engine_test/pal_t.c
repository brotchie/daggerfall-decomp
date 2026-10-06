/* pal_t.c: test shims of src/engine/pal.c (built only by tools/xn_rc.py; see
   src/engine_test/vec_t.c and docs/xngine_canonical.md). Each maps its asm entry's registers
   (config/xngine_abi.csv) to the C call and back. */
#include "xpal.h"

/* rgb EAX, first DX, count BX */
void xn_pal_set_range_8bit_r(xn_regs *r)
{
    xn_pal_set_range_8bit((const u8 *)r->eax, (u16)r->edx, (u16)r->ebx);
}

void xn_pal_set_all_8bit_r(xn_regs *r)
{
    xn_pal_set_all_8bit((u8 *)r->eax);
}

void xn_pal_read_dac_r(xn_regs *r)
{
    xn_pal_read_dac((u8 *)r->eax);
}

/* r BL, g CL, b DL -> AL */
void xn_pal_find_nearest_r(xn_regs *r)
{
    r->eax = (r->eax & ~0xFFu) | xn_pal_find_nearest((u8)r->ebx, (u8)r->ecx, (u8)r->edx);
}

/* h BX, s CX, v DX -> EAX */
void xn_pal_find_nearest_hsv_r(xn_regs *r)
{
    r->eax = xn_pal_find_nearest_hsv((u16)r->ebx, (u16)r->ecx, (u16)r->edx);
}

/* r BL, g CL, b DL -> h BX, s CX, v DX */
void xn_pal_rgb_to_hsv_r(xn_regs *r)
{
    xn_pal_hsv c;

    xn_pal_rgb_to_hsv((u8)r->ebx, (u8)r->ecx, (u8)r->edx, &c);
    r->ebx = (r->ebx & 0xFFFF0000u) | c.h;
    r->ecx = (r->ecx & 0xFFFF0000u) | c.s;
    r->edx = (r->edx & 0xFFFF0000u) | c.v;
}

/* h BX, s CX, v DX -> r BX, g CX, b DX */
void xn_pal_hsv_to_rgb_r(xn_regs *r)
{
    xn_pal_hsv c;
    xn_pal_rgb16 o;

    c.h = (u16)r->ebx;
    c.s = (u16)r->ecx;
    c.v = (u16)r->edx;
    xn_pal_hsv_to_rgb(&c, &o);
    r->ebx = (r->ebx & 0xFFFF0000u) | o.r;
    r->ecx = (r->ecx & 0xFFFF0000u) | o.g;
    r->edx = (r->edx & 0xFFFF0000u) | o.b;
}

void xn_pal_set_r(xn_regs *r)
{
    xn_pal_set((const u8 *)r->eax);
}

void xn_pal_get_r(xn_regs *r)
{
    xn_pal_get((u8 *)r->eax);
}

void xn_pal_stub_empty_r(xn_regs *r)
{
    xn_pal_stub_empty();
}

void xn_pal_fade_to_black_r(xn_regs *r)
{
    xn_pal_fade_to_black(r->eax);
}

void xn_pal_fade_to_r(xn_regs *r)
{
    xn_pal_fade_to((const u8 *)r->eax, r->edx);
}

/* index CL, rgb ESI */
void xn_pal_fade_write_entry_r(xn_regs *r)
{
    xn_pal_fade_write_entry((u8)r->ecx, (const s32 *)r->esi);
}

void xn_pal_build_ega16_map_r(xn_regs *r)
{
    xn_pal_build_ega16_map();
}
