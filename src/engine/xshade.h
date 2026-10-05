/* xshade.h: XnGine's shade functions (shade.c; see xngine.h): the 64-row shade table and the
   tables built from it, and the distance fog, a remap of every span beyond the fog start
   through a 64-level fog table.

   Each declaration keeps the function's asm interface (config/xngine_abi.csv): no pragma is
   Watcom's own convention; a pragma names the registers; NAME_r is the glue for a function
   whose asm callers pass registers a prototype cannot take (EBP) or read several. */
#ifndef XSHADE_H
#define XSHADE_H

#include "xngine.h"
#include "xnstruct.h"

/* ---- the shade table -------------------------------------------------------------------- */

extern u8 *xn_shade_table;              /* 64 rows of 256 colours (SHADE.nnn), 16K-aligned */
extern u8 *xn_shade_table_alloc;        /* the block it is in */
extern u8 *xn_shade_table_last_row;     /* xn_shade_table + 3F00h */
extern char xn_shade_filename[];        /* "SHADE.nnn" */
extern u8 *xn_shade_translucent_table;  /* the 16 x 256 table xn_shade_build_translucent_table
                                           fills (xn_shade_blend_tables[1]) */
extern u8 *haze_table_1;                 /* the game's haze.001 table (64 rows of 256) */

/* The light shaders' clamp: the last shade row, in the six MAX operands of the three
   templates (see xlight.h) */
extern u8 *xn_light_t1_max_a, *xn_light_t1_max_b;
extern u8 *xn_light_t2_max_a, *xn_light_t2_max_b;
extern u8 *xn_light_t3_max_a, *xn_light_t3_max_b;

/* The last of the 64 shade rows (the game keeps it as a fog table that changes nothing). */
u8 *xn_shade_table_63(void);

/* At startup, after the haze tables are loaded: in every shade row, colours 0-31 map to FFh
   except colour 0, which stays 0, and colour 255 maps to 255; in every row of the haze.001
   table, colour 255 maps to 77h. */
void xn_shade_init_reserved(void);

/* After a shade table is loaded: colour 0 maps to 0 and colour 255 to 255 in every row. */
void xn_shade_keep_colours_0_255(void);

/* The flats' translucency table at `table`: 16 rows of 256; row 0 the identity, rows 1-13 the
   shade rows 50, 48, ... 26, rows 14 and 15 all F5h. Keeps every register (pushad). */
void xn_shade_build_translucent_table(u8 *table);

/* Loads SHADE.nnn (n as three digits) into the shade table, and writes its last row into the
   light shaders' six clamp operands. Keeps every register (pushad). */
void xn_shade_load(s32 n);
void xn_shade_load_r(xn_regs *r);

/* Dead: loads HAZE.nnn into a new 256-aligned block, and makes its last level the fog
   table's. Keeps every register (pushad). */
void xn_shade_load_haze(s32 n);
#pragma aux xn_shade_load_haze parm [eax] modify exact [eax];

/* ---- the fog ------------------------------------------------------------------------------ */

extern u8 *xn_fog_table_last;           /* the fog table's last 256-byte level */
extern s32 xn_fog_start;                /* in z >> 8 units (the view distance when off) */
extern u32 xn_fog_step;                 /* 3F00h / (far >> 8 - start): table bytes a unit */
extern u32 xn_fog_min_inv_z;            /* 2^40 / far: 1/z is clamped to it */
extern u16 xn_fog_unroll_offsets[640];  /* 18 n: where the planted ret stops the remap */
extern void (*xn_render_span_hook)(void);   /* called after every span: the fog or a ret */
extern u32 xn_cam_far_z;
extern s32 xn_recip32_table[];          /* 2^32 / n */

/* xn_shade_fog_span's operands, written by xn_shade_set_fog (rule PF: each copy of a value is
   written; the fog reads the copy its asm reads) */
extern u32 xn_fog_table_last_a;         /* 1500DF: add ecx, T */
extern u8 *xn_fog_table_last_b;         /* 15011C: lea ecx, [eax + T] */
extern u32 xn_fog_step_a, xn_fog_step_b, xn_fog_step_c;                 /* 3F00h / range */
extern u32 xn_fog_inv_start_a, xn_fog_inv_start_b, xn_fog_inv_start_c;  /* 2^32 / start */
extern u32 xn_fog_start_step_a, xn_fog_start_step_b;                    /* start * step */

/* Sets the fog for the frames to come: start in z >> 8 units, or -1 for none. With a start
   nearer than the view distance, the fog levels' constants go into xn_shade_fog_span's
   operands and the span hook becomes the fog; otherwise the hook is the empty one and the
   fog start is the view distance. */
void xn_shade_set_fog(s32 start);

/* The span hook with no fog: nothing. */
void xn_shade_fog_span_off(void);
#pragma aux xn_shade_fog_span_off parm [] modify exact [eax];

/* The unrolled remap of 641 pixels that xn_shade_fog_span stops with a planted ret: only its
   asm interface (the RET adapter) is C; the remap is fog_run in shade.c. */
void xn_shade_fog_pixels_r(xn_regs *r);

/* Fogs the n pixels of a span from pix (the polygon's, from x_start): the pixels beyond the
   fog start are remapped through the fog level at their depth, interpolated in 1/z from
   both ends of the fogged part. */
void xn_shade_fog_span(const struct xn_poly *poly, const struct xn_span *span, s32 n, u8 *pix);
void xn_shade_fog_span_r(xn_regs *r);

#endif
