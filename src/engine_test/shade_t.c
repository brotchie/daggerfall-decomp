/* shade_t.c: test shims of src/engine/shade.c (built only by tools/xn_rc.py; see
   src/engine_test/vec_t.c and docs/xngine_canonical.md). Each NAME_r maps the registers of
   NAME's asm entry (config/xngine_abi.csv) to the canonical C call. */
#include "xshade.h"
#include "xctest.h"

/* the fog remap's unrolled asm body, where the fog span plants its ret */
extern u8 asm_xn_shade_fog_pixels[];

void xn_shade_table_63_r(xn_regs *r)
{
    r->eax = (u32)xn_shade_table_63();
}

void xn_shade_init_reserved_r(xn_regs *r)
{
    (void)r;
    xn_shade_init_reserved();
}

void xn_shade_keep_colours_0_255_r(xn_regs *r)
{
    (void)r;
    xn_shade_keep_colours_0_255();
}

void xn_shade_build_translucent_table_r(xn_regs *r)
{
    xn_shade_build_translucent_table((u8 *)r->eax);
}

void xn_shade_load_r(xn_regs *r)
{
    xn_shade_load(r->eax);
}

void xn_shade_load_haze_r(xn_regs *r)
{
    xn_shade_load_haze(r->eax);
}

void xn_shade_set_fog_r(xn_regs *r)
{
    xn_shade_set_fog(r->eax);
}

void xn_shade_fog_span_off_r(xn_regs *r)
{
    (void)r;
}

/* The unrolled remap: ECX = the fog row, EDX = its step, EDI = the first pixel - 100h; the
   count is the step its planter put a ret at (641: none). EAX and ECX come back as the asm
   leaves them: the last lookup's address with the pixel, and the row after the last step. */
void xn_shade_fog_pixels_r(xn_regs *r)
{
    u8 *pix = (u8 *)r->edi + 0x100;
    int n = xc_planted_count(asm_xn_shade_fog_pixels, 18, 641);

    xn_shade_fog_pixels(pix, n, r->ecx, r->edx);
    if (n > 0)
        r->eax = ((r->ecx + (n - 1) * r->edx) & ~0xFFu) | pix[n - 1];
    r->ecx += n * r->edx;
}

/* EBX = the polygon, ESI = the span node, EBP = the count, EDI = the first pixel - 1 */
void xn_shade_fog_span_r(xn_regs *r)
{
    xn_shade_fog_span((const struct xn_poly *)r->ebx, (const struct xn_span *)r->esi, r->ebp,
                      (u8 *)r->edi + 1);
}
