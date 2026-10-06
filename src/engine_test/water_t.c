/* water_t.c: test shims of src/engine/water.c (built only by tools/xn_rc.py; see
   src/engine_test/vec_t.c and docs/xngine_canonical.md). Each NAME_r maps the registers of
   NAME's asm entry (config/xngine_abi.csv) to the canonical C call. */
#include "xwater.h"

/* the asm's state the shims read, where its callers left it: the row's jitter pointer
   (xn_water_draw sets it 400h bytes before the row's entries: pixel x's jitter is
   [0x100 + x]), and the unrolled run's code, where the span plants its `ret` */
extern s32 *xn_water_row_jitter;
extern u8 asm_xn_water_span_unrolled[];

#define WATER_STEP  21          /* bytes of a step of the asm's unrolled run */
#define WATER_STEPS 641

void xn_water_init_r(xn_regs *r)
{
    (void)r;
    xn_water_init();
}

/* -> CF when the line cannot be seen (the ends, which the asm leaves in xn_water_line_p0
   and _p1, are the canonical C's locals) */
void xn_water_clip_extent_r(xn_regs *r)
{
    struct xn_poly_vertex p0, p1;

    XN_SETFLAG(r, XN_CF, !xn_water_clip_extent(&p0, &p1));
}

void xn_water_draw_r(xn_regs *r)
{
    (void)r;
    xn_water_draw();
}

/* x0 EBX, x1 EBP, the row - 100h EDI; the jitter from xn_water_row_jitter */
void xn_water_span_r(xn_regs *r)
{
    xn_water_span((u8 *)r->edi + 0x100, xn_water_row_jitter + 0x100, r->ebx, r->ebp);
}

/* The run's asm entry: pixels at EDI + 100h, jitters at ESI + 400h, the tint table in EAX
   (its low byte replaced by each pixel); the count is the step its caller planted a `ret` at
   (641: none). AL: the last pixel written. */
void xn_water_span_unrolled_r(xn_regs *r)
{
    u8 *pix = (u8 *)r->edi + 0x100;
    s32 n;

    for (n = 0; n < WATER_STEPS; n++)
        if (asm_xn_water_span_unrolled[n * WATER_STEP] == 0xC3)
            break;
    xn_water_span_unrolled(pix, (const s32 *)(r->esi + 0x400), n,
                           (const u8 *)(r->eax & ~0xFFu));
    if (n > 0)
        r->eax = (r->eax & ~0xFFu) | pix[n - 1];
}

void xn_water_stub_ret_r(xn_regs *r)
{
    (void)r;
    xn_water_stub_ret();
}
