/* xlight.h: XnGine's lights (src/engine/light.c): the frame's light table, the lighting of
   each polygon on its first span, and the per-pixel light shaders. Canonical C: plain
   prototypes, Watcom's own calling convention; docs/xngine_canonical.md.

   What it does
     The game adds the frame's lights (xn_light_add: torches, spells, the sun) to a table of
     32. When a model face or a terrain cell is first drawn, its setup lights it:
       - its base shade row is the face's own shade plus the frame's ambient row;
       - each directional light of its model's light list that faces it adds its intensity
         times the cosine, in whole rows;
       - each point light of the list that is in front of the face plane, near enough and
         strong enough is collected (at most 3; the third ends the list).
     A face lit to the last row is drawn unlit (kind 0); with no point light it is drawn
     through its one shade row (kind 4, the row in the polygon's +14h); with point lights
     it gets a light shader (kind 8): a record of the lights' foot points on the face plane,
     their intensities and falloffs and the base row, which xn_light_shade evaluates at every
     16th pixel of its spans (xspan.h). The asm compiled each shader into machine code (one
     of three templates, patched and copied into big_buffer) and called it; canonical C keeps
     the same numbers in a struct xn_light_shader from a pool of the frame's. The asm's
     machine code still goes into big_buffer, as bytes nothing runs, because the game reads
     big_buffer back through a stale pointer (Q-LIGHT-07).

   Units and formats
     positions      world units; xn_light_to_view moves them to view space << 8; a shader's
                    foot points are view space >> 8 (indices into the squares table:
                    xn_squares_table_mid[k] = k*k << 8).
     intensity      1..32 at add; a point light at the face: (2^32 / d^2) * intensity, kept
                    when above 100h; falloff: 2^30 / d^2, kept when above 80h (d: the
                    light's distance from the face plane, d^2's high dword).
     shade rows     addresses in xn_shade_table (64 rows of 256 colours, 16K-aligned): row k
                    is xn_shade_table + k << 8, the last (k = 63) xn_shade_table_last_row; a
                    shade is a row address plus an 8-bit fraction.
     falloff table  xn_light_falloff: LIGHT.DAT, 32K words, the falloff by scaled squared
                    distance (index < 8000h).
     types          0 point, 4 ignored, 8 directional (byte offsets of the asm's dispatch
                    tables).

   Globals (in object 2, the engine's): xn_light_table, xn_light_next, xn_light_count,
   xn_shade_table (+ _alloc, _last_row), xn_light_falloff (+ _alloc), xn_squares_table_mid.
   The game's: xn_light_ambient (the ambient level, the game writes it). Canonical C's own:
   xn_light_ambient_row (the frame's), the shader pool (the frame's).

   Quirks kept (docs/engine/quirks.md): Q-LIGHT-01 (xn_light_add returns the camera cull's
   leftover, which the game keeps as an object's draw handle), Q-LIGHT-02 (the light count
   goes up before the full test), Q-LIGHT-04 (the squares-table index is not bounded), Q-LIGHT-05 (the
   third point light ends the face's light list), Q-LIGHT-06 (the 1/z slope's inverse is 0
   for a slope of 0: Q-SYS-01), Q-LIGHT-07 (the shaders' bytes in big_buffer reach the
   game). */
#ifndef XLIGHT_H
#define XLIGHT_H

#include "xngine.h"
#include "xnstruct.h"

#define XN_LIGHTS       32              /* slots in xn_light_table (a 33rd ends the scans) */
#define XN_LIGHT_POINTS 3               /* point lights a shader takes */

extern struct xn_light xn_light_table[XN_LIGHTS + 1];
extern struct xn_light *xn_light_next;  /* the next free slot */
extern s32 xn_light_count;              /* lights added this frame (Q-LIGHT-02) */
extern s32 xn_light_ambient;            /* the game's ambient level (row << 8; clamped at use) */
extern u16 *xn_light_falloff;           /* LIGHT.DAT, 32-aligned */
extern u8 *xn_light_falloff_alloc;
extern char xn_light_filename[];        /* "LIGHT.DAT" */
extern char xn_light_msg_no_memory[];   /* "ENGINE: Out of memory for shaders.$" */
extern u8 *xn_shade_table;              /* 64 rows of 256 colours, 16K-aligned */
extern u8 *xn_shade_table_alloc;
extern u8 *xn_shade_table_last_row;     /* xn_shade_table + 3F00h */
extern s32 xn_squares_table[8192];      /* k*k << 8 for k = -4096..4095 (xrender.h) */
#define xn_squares_table_mid (xn_squares_table + 4096)     /* k*k << 8 at index k */
extern u8 *xn_light_code_next;          /* where the next shader's asm image goes (Q-LIGHT-07;
                                           xn_render_begin_frame: big_buffer) */

/* The frame's ambient shade row: xn_shade_table + the ambient level's whole rows (clamped to
   0..3F00h), set by xn_render_begin_frame; the model faces' and the flats' base row. */
extern u8 *xn_light_ambient_row;

/* A point light as a shader takes it: its foot point on the face plane (view space >> 8),
   its intensity at the face and its falloff. */
struct xn_light_point {
    s32 x, y, z;
    s32 intensity;
    s32 falloff;
};

/* A polygon's light shader (the polygon's +14h for lighting kind 8). */
struct xn_light_shader {
    s32 nlights;                        /* 1..3 */
    u8 *row;                            /* the base shade row (with the directional lights) */
    struct xn_light_point light[XN_LIGHT_POINTS];
};

/* ---- the frame's lights -------------------------------------------------------------------- */

/* Allocates the shade table (32K from the game's allocator, 16K-aligned) and the falloff
   table (64K + 32, 32-aligned), loads LIGHT.DAT into the latter and empties the light table.
   Without the memory, shuts the engine down and ends the program with a message. One game
   site (the video set-up). */
void xn_light_init(void);

/* Frees the shade and falloff tables' blocks (when the shade table was allocated). */
void xn_light_free(void);

/* Empties the light table: every slot's intensity -1, the next slot the first (the count is
   left: xn_render_begin_frame zeroes it). Three game sites. */
void xn_light_reset(void);

/* Adds a light of the frame at (x, y, z): intensity (none when 0 or less; clamped to 32),
   radius (clamped to 512) and type (0 point, 4 ignored, 8 directional). A point light whose
   sphere (radius intensity / 2 * radius << 6) the camera does not see is not kept. At most 31
   lights a frame (Q-LIGHT-02). Returns x, or for a point light what the camera cull leaves
   (Q-LIGHT-01). Ten game sites. */
s32 xn_light_add(s32 x, s32 y, s32 z, s32 intensity, s32 radius, s32 type);

/* The lights' positions rewritten in view space << 8, in place (for the flats, which light
   in view space). Called by xn_render_draw_flats, after the models. */
void xn_light_to_view(void);

/* ---- the per-polygon lighting -------------------------------------------------------------- */

/* A model face's lighting, on its first span: marks its model drawn (the handle's flag 2),
   stores its 1/z slope's inverse (+60h: 2^32 / +5Ch, 0 for 0, Q-LIGHT-06), and lights it
   from its model's light list as the module header says (Q-LIGHT-05). Returns the lighting
   kind: 0 (lit to the last row), 4 (the row in +14h) or 8 (a shader in +14h). */
s32 xn_light_setup_poly(struct xn_poly *poly);

/* A terrain cell's: the 1/z slope's inverse, the ambient row (none when the ambient is at the
   last row or beyond), and the directional lights of its list (+14h, in view space: their
   directions negated). Returns 0 or 4. */
s32 xn_light_setup_terrain(struct xn_poly *poly);

/* No point light: row becomes the polygon's shade row (+14h); 4. */
s32 xn_light_shade_constant(struct xn_poly *poly, u8 *row);

/* The polygon's shader for n (1..3) point lights over the base row: a record of the frame's
   pool into its +14h, and the asm's compiled form of it at xn_light_code_next (Q-LIGHT-07);
   8. */
s32 xn_light_build_shader(struct xn_poly *poly, u8 *row, const struct xn_light_point *pts,
                          int n);

/* A point light of the face's model's list (ref: in the model's axes): when it is in front
   of the face plane, within its range and strong enough (see the units), *pt gets its
   intensity, falloff and foot point; returns 1 then, else 0. */
int xn_light_add_point(const struct xn_poly *poly, const struct xn_light_ref *ref,
                       struct xn_light_point *pt);

/* A directional light of a model's list: when it faces the polygon, adds its intensity times
   the cosine, in whole rows, to *row. Returns 1 (stop: fully lit) when the row reaches the
   last one. */
int xn_light_add_directional(const struct xn_poly *poly, const struct xn_light_ref *ref,
                             u8 **row);

/* The same for the terrain's lights (in view space, negated, with the cell's normal +50h). */
int xn_light_terrain_add_directional(const struct xn_poly *poly, const struct xn_light_ref *ref,
                                     u8 **row);

/* ---- the shaders ------------------------------------------------------------------------------ */

/* Empties the frame's shader pool (xn_render_begin_frame). */
void xn_light_begin_frame(void);

/* The shade a shader gives at the view-space point of a pixel: its ray (ray_x, ray_y: the
   camera's rays of its column and row) times its z. Each light adds
   (falloff[(d^2 * light falloff) >> 32] * intensity) >> 12 to the base row while that index
   is below 8000h (d^2 from the squares table, Q-LIGHT-04); the sum is clamped to the last
   row. The shade is an address: the row's, its low byte a fraction (iptr). */
iptr xn_light_shade(const struct xn_light_shader *shader, s32 ray_y, s32 ray_x, s32 z);

#endif
