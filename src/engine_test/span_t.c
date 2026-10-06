/* span_t.c: test shims of src/engine/span.c (built only by tools/xn_rc.py; see
   src/engine_test/vec_t.c and docs/xngine_canonical.md). Each NAME_r maps the registers of
   NAME's asm entry (config/xngine_abi.csv) to the canonical C call.

   The polygon spans' asm entries take EAX = the polygon, ESI = the span node, EBX = x - the
   view centre's x, EBP = the pixel count and EDI = the first pixel - 1; the flat spans' ESI =
   the flat, ECX = 1/z, EBX = the screen x, EBP, EDI. A tail's entry is the asm's unrolled
   body, which its routine stopped with a planted ret: its count is where the ret is. */
#include "xspan.h"
#include "xctest.h"

int xc_planted_count(const void *body, int stride, int max)
{
    const u8 *p = (const u8 *)body;
    int n;

    for (n = 0; n < max; n++, p += stride)
        if (*p == 0xC3)
            return n;
    return max;
}

/* the tails' asm bodies */
extern u8 asm_xn_span_solid_lit_tail[], asm_xn_span_tex_tail[], asm_xn_span_tex_shaded_tail[];
extern u8 asm_xn_span_tex64_tail[], asm_xn_span_tex64_shaded_tail[];
extern u8 asm_xn_span_flat_transparent_tail[], asm_xn_span_flat_transparent_shaded_tail[];
extern u8 asm_xn_span_flat_lit_fogged_tail[], asm_xn_span_flat_translucent_tail[];

#define POLY(r)     ((struct xn_poly *)(r)->eax)
#define SPAN(r)     ((const struct xn_span *)(r)->esi)
#define PIX(r)      ((u8 *)(r)->edi + 1)
#define SPAN_ARGS(r) POLY(r), SPAN(r), (r)->ebx, (r)->ebp, PIX(r)
#define FLAT_ARGS(r) (struct xn_flat *)(r)->esi, (r)->ecx, (r)->ebx, (r)->ebp, PIX(r)

/* a shade row as the tails take it in a register: only bits 8-31 count (never 0 here) */
#define ROW_REG(v)  ((const u8 *)((v) | 1))

void xn_span_solid_r(xn_regs *r)
{
    xn_span_solid(SPAN_ARGS(r));
}

void xn_span_solid_shaded_setup_r(xn_regs *r)
{
    xn_span_solid_shaded_setup(SPAN_ARGS(r));
}

/* the lit routines' records start with the asm's compiled shader in the polygon */
void xn_span_solid_lit_r(xn_regs *r)
{
    struct xn_light_shader s;
    const struct xn_light_shader *was = xc_adopt_shader(POLY(r), &s);

    xn_span_solid_lit(SPAN_ARGS(r));
    POLY(r)->shader = was;
}

/* EAX = the start shade with the colour in AL, ECX = the shade, EBP = its step */
void xn_span_solid_lit_tail_r(xn_regs *r)
{
    xn_span_solid_lit_tail(PIX(r), xc_planted_count(asm_xn_span_solid_lit_tail, 9, 16), r->eax,
                           r->ecx, r->ebp);
}

void xn_span_tex_8_r(xn_regs *r)
{
    xn_span_tex_8(SPAN_ARGS(r));
}

void xn_span_tex_16_setup_r(xn_regs *r)
{
    xn_span_tex_16_setup(SPAN_ARGS(r));
}

void xn_span_tex_16_r(xn_regs *r)
{
    xn_span_tex_16(SPAN_ARGS(r));
}

/* EAX = the mask, ECX = the step, EBX = the coordinate, ESI = the texels */
void xn_span_tex_tail_r(xn_regs *r)
{
    xn_span_tex_tail(PIX(r), xc_planted_count(asm_xn_span_tex_tail, 17, 16), r->ebx, r->ecx,
                     r->eax, (const u8 *)r->esi);
}

void xn_span_tex_shaded_8_r(xn_regs *r)
{
    xn_span_tex_shaded_8(SPAN_ARGS(r));
}

void xn_span_tex_shaded_16_setup_r(xn_regs *r)
{
    xn_span_tex_shaded_16_setup(SPAN_ARGS(r));
}

void xn_span_tex_shaded_16_r(xn_regs *r)
{
    xn_span_tex_shaded_16(SPAN_ARGS(r));
}

/* EAX = the shade row, ECX = the step, EBX = the coordinate, EBP = the mask, ESI = the
   texels */
void xn_span_tex_shaded_tail_r(xn_regs *r)
{
    xn_span_tex_shaded_tail(PIX(r), xc_planted_count(asm_xn_span_tex_shaded_tail, 19, 16),
                            r->ebx, r->ecx, r->ebp, (const u8 *)r->esi, ROW_REG(r->eax));
}

void xn_span_tex_lit_setup_r(xn_regs *r)
{
    struct xn_light_shader s;
    const struct xn_light_shader *was = xc_adopt_shader(POLY(r), &s);

    xn_span_tex_lit_setup(SPAN_ARGS(r));
    POLY(r)->shader = was;
}

void xn_span_tex_lit_r(xn_regs *r)
{
    struct xn_light_shader s;
    const struct xn_light_shader *was = xc_adopt_shader(POLY(r), &s);

    xn_span_tex_lit(SPAN_ARGS(r));
    POLY(r)->shader = was;
}

void xn_span_tex64_r(xn_regs *r)
{
    xn_span_tex64(SPAN_ARGS(r));
}

/* EAX = the mask (3F3Fh), ECX = the step, EBX = the coordinate, ESI = the texels */
void xn_span_tex64_tail_r(xn_regs *r)
{
    xn_span_tex64_tail(PIX(r), xc_planted_count(asm_xn_span_tex64_tail, 17, 16), r->ebx,
                       r->ecx, r->eax, (const u8 *)r->esi);
}

void xn_span_tex64_shaded_r(xn_regs *r)
{
    xn_span_tex64_shaded(SPAN_ARGS(r));
}

/* EAX = the shade row, EBP = the mask */
void xn_span_tex64_shaded_tail_r(xn_regs *r)
{
    xn_span_tex64_shaded_tail(PIX(r), xc_planted_count(asm_xn_span_tex64_shaded_tail, 19, 16),
                              r->ebx, r->ecx, r->ebp, (const u8 *)r->esi, ROW_REG(r->eax));
}

void xn_span_flat_transparent_r(xn_regs *r)
{
    xn_span_flat_transparent(FLAT_ARGS(r));
}

/* the flat tails: EBX = the coordinate, ECX (or EBP) = the step, ESI = the texels */
void xn_span_flat_transparent_tail_r(xn_regs *r)
{
    xn_span_flat_transparent_tail(PIX(r), xc_planted_count(asm_xn_span_flat_transparent_tail,
                                                           19, 8),
                                  r->ebx, r->ecx, (const u8 *)r->esi);
}

void xn_span_flat_transparent_shaded_r(xn_regs *r)
{
    xn_span_flat_transparent_shaded(FLAT_ARGS(r));
}

/* EAX = the shade row */
void xn_span_flat_transparent_shaded_tail_r(xn_regs *r)
{
    xn_span_flat_transparent_shaded_tail(
        PIX(r), xc_planted_count(asm_xn_span_flat_transparent_shaded_tail, 21, 8), r->ebx,
        r->ecx, (const u8 *)r->esi, ROW_REG(r->eax));
}

void xn_span_flat_lit_fogged_r(xn_regs *r)
{
    xn_span_flat_lit_fogged(FLAT_ARGS(r));
}

/* EAX = the light row, ECX = the fog row, EBP = the step */
void xn_span_flat_lit_fogged_tail_r(xn_regs *r)
{
    xn_span_flat_lit_fogged_tail(PIX(r), xc_planted_count(asm_xn_span_flat_lit_fogged_tail,
                                                          23, 8),
                                 r->ebx, r->ebp, (const u8 *)r->esi, ROW_REG(r->eax),
                                 ROW_REG(r->ecx));
}

void xn_span_flat_translucent_r(xn_regs *r)
{
    xn_span_flat_translucent(FLAT_ARGS(r));
}

/* ECX = the table (with EAX's upper half: the body indexes it with EAX), EBP = the step */
void xn_span_flat_translucent_tail_r(xn_regs *r)
{
    xn_span_flat_translucent_tail(PIX(r), xc_planted_count(asm_xn_span_flat_translucent_tail,
                                                           25, 8),
                                  r->ebx, r->ebp, (const u8 *)r->esi,
                                  (const u8 *)(r->ecx + (r->eax & 0xFFFF0000u)));
}

/* ESI = the row's head, EBP = x0, EBX = x1, ECX = 1/z at x0 */
void xn_span_insert_r(xn_regs *r)
{
    xn_span_insert((struct xn_span *)r->esi, r->ebp, r->ebx, r->ecx);
}
