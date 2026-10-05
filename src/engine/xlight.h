/* xlight.h: XnGine's lights (light.c; see xngine.h): the frame's light table, the per-polygon
   light setup, and the per-pixel light shaders.

   A polygon lit by point lights gets a small piece of machine code, a shader returning the
   shade at a view-space point: one of three templates (1, 2 or 3 lights) patched in place
   with the lights' positions, intensities and the polygon's base shade row, and copied into a
   pool in big_buffer (xn_light_code_next); its address goes into the polygon's +14h. The
   builders stay byte-exact generators (the records see the template patches and the copies);
   the shaders are evaluated in C from a copy's operands (xn_light_shade_eval, the design's
   option B), so no generated code runs.

   Each declaration keeps the function's asm interface (config/xngine_abi.csv): no pragma is
   Watcom's own convention; a pragma names the registers; NAME_r is the glue. */
#ifndef XLIGHT_H
#define XLIGHT_H

#include "xngine.h"
#include "xnstruct.h"

#define XN_LIGHTS       32              /* slots in xn_light_table (a 33rd ends the scans) */

extern struct xn_light xn_light_table[XN_LIGHTS + 1];
extern struct xn_light *xn_light_next;  /* the next free slot */
extern s32 xn_light_count;              /* lights added this frame (one more when a 33rd was
                                           tried: the count goes up before the test) */
extern s32 xn_light_ambient;            /* the ambient shade row's offset (row << 8) */
extern u16 *xn_light_falloff;           /* light.dat: falloff by squared distance, 32-aligned */
extern u8 *xn_light_falloff_alloc;
extern char xn_light_filename[];        /* "LIGHT.DAT" */
extern u8 *xn_shade_table;
extern u8 *xn_shade_table_alloc;
extern u8 *xn_shade_table_last_row;
extern u8 *xn_light_code_next;          /* where the next shader is copied (big_buffer) */
extern s32 xn_squares_table_mid[];      /* k*k << 8 at index k, k = -4096..4095 */

/* The point lights that reach the polygon being set up, three slots
   (struct xn_light_point_slots), and its shade row being accumulated */
extern s32 xn_light_point_intensity[3];
extern s32 xn_light_point_falloff[3];
extern s32 xn_light_point_x[3], xn_light_point_y[3], xn_light_point_z[3];
extern u8 *xn_light_shade_row;
extern u8 *xn_light_ambient_row;        /* 15BC61: add ebx, AMBIENT_ROW (from
                                           xn_render_begin_frame: xn_shade_table + ambient) */

/* the three shader templates (code in object 2, patched in place, never run there) */
extern u8 xn_light_tmpl_1[0x54], xn_light_tmpl_2[0x8C], xn_light_tmpl_3[0xCC];

/* ---- the frame's lights -------------------------------------------------------------------- */

/* Allocates the shade table (16K-aligned) and the falloff table (32-aligned), loads
   LIGHT.DAT into the latter and writes its address into the six falloff operands of the
   shader templates; empties the light table. Without the memory, shuts the engine down and
   exits to DOS with a message. Keeps every register (pushad). */
void xn_light_init(void);
void xn_light_init_r(xn_regs *r);

/* Frees the shade and falloff tables' blocks (when the shade table was allocated). */
void xn_light_free(void);
void xn_light_free_r(xn_regs *r);

/* Empties the light table: every slot's intensity -1, the next slot the first. (The count is
   left as it is.) */
void xn_light_reset(void);

/* Adds a light of the frame at (x, y, z): intensity 1..32 (clamped), radius (clamped to 512)
   and type (0 point, 4 ignored, 8 directional). A point light whose sphere the camera does
   not see is dropped. Returns what the asm leaves in EAX: x, or the culling test's result. */
s32 xn_light_add(s32 x, s32 y, s32 z, s32 intensity, s32 radius, s32 type);

/* xn_light_add with the radius in EBP and the type in ESI: glue */
s32 xn_light_add_regs(s32 x, s32 y, s32 z, s32 intensity, s32 radius, s32 type);
void xn_light_add_regs_r(xn_regs *r);

/* The lights' positions rewritten in view space << 8, in place, for the flats. */
void xn_light_to_view(void);

/* ---- the per-polygon setup ----------------------------------------------------------------- */

/* A model face's lighting: its 1/z slope's inverse into +60h, its model marked as drawn, its
   base shade row (face shade + ambient) lit by the directional lights of the model's light
   list, and the point lights that reach the face plane collected (at most 3). Returns 0 when
   the row runs past the last one (fully lit), 4 with the row in the polygon's +14h (no point
   light), or 8 with a compiled shader there. */
s32 xn_light_setup_poly(struct xn_poly *poly);
#pragma aux xn_light_setup_poly parm [edi] value [eax] modify exact [eax ecx edx ebx esi];

/* The same for a terrain polygon: its directional lights only (from its +14h); 0 or 4. */
s32 xn_light_setup_terrain(struct xn_poly *poly);
#pragma aux xn_light_setup_terrain parm [edi] value [eax] modify exact [eax ecx edx ebx esi];

/* No point light: the polygon's +14h is the shade row; 4. */
s32 xn_light_shade_constant(struct xn_poly *poly);
void xn_light_shade_constant_r(xn_regs *r);

/* Compiles the polygon's shader for its 1, 2 or 3 point lights (the slots): the template's
   operands patched in place, the template copied to xn_light_code_next, the polygon's +14h
   pointed at the copy and its +08h.. holding the falloffs the copy reads. Returns 8. */
s32 xn_light_build_shader(struct xn_poly *poly, int nlights);
s32 xn_light_build_shader_1(struct xn_poly *poly);
s32 xn_light_build_shader_2(struct xn_poly *poly);
s32 xn_light_build_shader_3(struct xn_poly *poly);
void xn_light_build_shader_1_r(xn_regs *r);
void xn_light_build_shader_2_r(xn_regs *r);
void xn_light_build_shader_3_r(xn_regs *r);

/* A point light of a model's list (ref: in the model's axes): when it is in front of the face
   plane, near enough, and strong enough, fills point slot `slot` with its intensity at the
   plane, its falloff, and its foot point on the plane in view space (>> 8); returns 1 then. */
int xn_light_point_reaches(struct xn_poly *poly, const struct xn_light_ref *ref, int slot);
/* xn_light_add_point (15BF75), the point lights' dispatch entry: its asm interface is in
   lglue.asm (a jump into xn_light_build_shader_3 that skips its own frame when the third slot
   fills, which no C or route stub can do), around xn_light_point_reaches. */

/* A directional light of a model's list: when it faces the polygon, adds its intensity times
   the cosine, in whole rows, to the shade row. Returns 1 (stop: fully lit) when the row
   reaches the last one. The terrain's lights come negated, from view space. */
int xn_light_add_directional(const struct xn_poly *poly, const struct xn_light_ref *ref);
void xn_light_add_directional_r(xn_regs *r);
int xn_light_terrain_add_directional(const struct xn_poly *poly, const struct xn_light_ref *ref);
void xn_light_terrain_add_directional_r(xn_regs *r);

/* The dispatch tables' empty entries (type 4, and the terrain's point lights) */
void xn_light_add_type4_noop(void);
#pragma aux xn_light_add_type4_noop parm [] modify exact [eax];
void xn_light_terrain_add_point_noop(void);
#pragma aux xn_light_terrain_add_point_noop parm [] modify exact [eax];
void xn_light_terrain_add_type4_noop(void);
#pragma aux xn_light_terrain_add_type4_noop parm [] modify exact [eax];

/* ---- the shaders ---------------------------------------------------------------------------- */

/* The shade a compiled shader (a copy of one of the templates) gives at a view-space point:
   the ray (ray_x, ray_y) of the pixel times its z. Each point light adds
   (falloff[(d^2 * light falloff) >> 32] * intensity) >> 12 to the base row while that index is
   below 8000h, d^2 from the squares table; the sum is clamped to the last row (the one-light
   shader clamps only when its light added something). Reads the copy's operands; runs no
   code. */
s32 xn_light_shade_eval(const u8 *shader, s32 ray_y, s32 ray_x, s32 z);

#endif
