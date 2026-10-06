/* rframe_t.c: test shims of src/engine/rframe.c (built only by tools/xn_rc.py; see
   src/engine_test/vec_t.c and docs/xngine_canonical.md). */
#include "xrframe.h"
#include "xrender.h"
#include "xspan.h"

/* The span routines and setups as the asm's polygons name them: their asm entries. A record
   of the frame starts with the polygons the game's earlier calls made in the asm (the
   terrain's cells), whose +3Ch holds an asm entry; canonical C's polygons hold the C
   function. */
extern void asm_xn_render_span_setup_solid(void), asm_xn_render_span_setup_terrain(void);
extern void asm_xn_render_span_setup_terrain_solid(void), asm_xn_render_span_mark_ends(void);
extern void asm_xn_poly_setup_textured(void);
extern void asm_xn_span_solid(void), asm_xn_span_solid_shaded_setup(void);
extern void asm_xn_span_solid_lit(void), asm_xn_span_tex_8(void), asm_xn_span_tex_16_setup(void);
extern void asm_xn_span_tex_16(void), asm_xn_span_tex_shaded_8(void);
extern void asm_xn_span_tex_shaded_16_setup(void), asm_xn_span_tex_shaded_16(void);
extern void asm_xn_span_tex_lit_setup(void), asm_xn_span_tex_lit(void), asm_xn_span_tex64(void);
extern void asm_xn_span_tex64_shaded(void);

void xn_poly_setup_textured(struct xn_poly *poly, const struct xn_span *span, s32 xs, s32 n,
                            u8 *pix);

static const struct {
    void (*asm_entry)(void);
    xn_span_fn c;
} routines[] = {
    { asm_xn_render_span_setup_solid, xn_render_span_setup_solid },
    { asm_xn_render_span_setup_terrain, xn_render_span_setup_terrain },
    { asm_xn_render_span_setup_terrain_solid, xn_render_span_setup_terrain_solid },
    { asm_xn_render_span_mark_ends, xn_render_span_mark_ends },
    { asm_xn_poly_setup_textured, xn_poly_setup_textured },
    { asm_xn_span_solid, xn_span_solid },
    { asm_xn_span_solid_shaded_setup, xn_span_solid_shaded_setup },
    { asm_xn_span_solid_lit, xn_span_solid_lit },
    { asm_xn_span_tex_8, xn_span_tex_8 },
    { asm_xn_span_tex_16_setup, xn_span_tex_16_setup },
    { asm_xn_span_tex_16, xn_span_tex_16 },
    { asm_xn_span_tex_shaded_8, xn_span_tex_shaded_8 },
    { asm_xn_span_tex_shaded_16_setup, xn_span_tex_shaded_16_setup },
    { asm_xn_span_tex_shaded_16, xn_span_tex_shaded_16 },
    { asm_xn_span_tex_lit_setup, xn_span_tex_lit_setup },
    { asm_xn_span_tex_lit, xn_span_tex_lit },
    { asm_xn_span_tex64, xn_span_tex64 },
    { asm_xn_span_tex64_shaded, xn_span_tex64_shaded },
};

/* the frame's polygons so far: their asm routines as the canonical C ones */
void xc_adopt_polys(void)
{
    struct xn_poly *p;
    unsigned k;

    for (p = xn_render_poly_pool; p < xn_render_poly_next; p++)
        for (k = 0; k < sizeof routines / sizeof routines[0]; k++)
            if ((void (*)(void))p->span_fn == routines[k].asm_entry) {
                p->span_fn = routines[k].c;
                break;
            }
}

/* EAX = the flags -> EAX and CF: 1 when the frame could not be drawn */
void xn_render_frame_r(xn_regs *r)
{
    xc_adopt_polys();
    r->eax = xn_render_frame(r->eax);
    XN_SETFLAG(r, XN_CF, r->eax != 0);
}
