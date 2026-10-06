/* xctest.h: what the rasteriser's test shims share (group C: span_t.c, shade_t.c, light_t.c,
   render_t.c, rframe_t.c, tmap_t.c; built only by tools/xn_rc.py). The shims map an asm
   entry's registers to the canonical C call; these helpers read the asm's own state where a
   record starts with it. */
#ifndef XCTEST_H
#define XCTEST_H

#include "xngine.h"
#include "xnstruct.h"
#include "xlight.h"

/* The count a planted ret gives one of the asm's unrolled bodies: `max` steps of `stride`
   bytes from `body`; the step whose first byte is C3h (ret), or max when none is (the body
   runs to its own ret). The asm's planters put the ret at step n to draw n pixels. */
int xc_planted_count(const void *body, int stride, int max);

/* A polygon whose +14h may hold the asm's compiled light shader (a copy of one of the three
   templates in big_buffer, as a record made by the asm starts): the shader as canonical C's
   record, decoded from the copy's operands into *tmp, is put in +14h. Returns what +14h held
   (to put back after the call); a canonical record is left as it is. */
const struct xn_light_shader *xc_adopt_shader(struct xn_poly *poly, struct xn_light_shader *tmp);

/* The asm's light-setup scratch (object 2): the point slots the dispatch handlers fill and
   the shade row being accumulated. */
extern s32 xn_light_point_intensity[3], xn_light_point_falloff[3];
extern s32 xn_light_point_x[3], xn_light_point_y[3], xn_light_point_z[3];
extern u8 *xn_light_shade_row;

/* The asm's point slots 0..n-1 as canonical C's points */
void xc_asm_points(struct xn_light_point *pts, int n);

#endif
