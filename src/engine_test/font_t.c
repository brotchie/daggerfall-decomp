/* font_t.c: test shims of src/engine/font.c (built only by tools/xn_rc.py; see
   src/engine_test/vec_t.c and docs/xngine_canonical.md). */
#include "xfont.h"

void xn_font_init_r(xn_regs *r)
{
    xn_font_init();
}

/* number EAX, slot EDX; EBX = 4 after (the boundary adapter has it) */
void xn_font_load_r(xn_regs *r)
{
    xn_font_load_b(r);
}

void xn_font_select_r(xn_regs *r)
{
    r->eax = xn_font_select(r->eax);
}

void xn_font_draw_string_r(xn_regs *r)
{
    r->eax = xn_font_draw_string(r->eax, r->edx, (const u8 *)r->ebx);
}

/* x EAX, y EDX, w EBX, rows ESI (EAX EBX ECX EDX kept) */
void xn_font_draw_glyph_r(xn_regs *r)
{
    xn_font_draw_glyph(r->eax, r->edx, r->ebx, (const u16 *)r->esi);
}

void xn_font_glyph_width_r(xn_regs *r)
{
    r->eax = xn_font_glyph_width(r->eax);
}

void xn_font_string_width_r(xn_regs *r)
{
    r->eax = xn_font_string_width((const u8 *)r->eax);
}

void xn_font_string_width_n_r(xn_regs *r)
{
    r->eax = xn_font_string_width_n((const u8 *)r->eax, r->edx);
}
