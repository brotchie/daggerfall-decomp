/* xpal.h: XnGine's VGA palette functions (src/engine/pal.c; see xngine.h): setting and reading
   the DAC (ports 3C7h-3C9h) with vertical retrace waits (3DAh), fades, and nearest-colour
   searches in RGB and HSV.

   Each declaration keeps the function's asm interface (config/xngine_abi.csv): no pragma is
   Watcom's own convention; a pragma names the registers; NAME_r is the glue for a function
   whose asm callers read several registers or flags. Where the asm takes BL, CL, DL (or BX..)
   the pragma names the whole registers: Watcom 10.0a ignores byte and word registers in a
   parm list (it falls back to the stack); the u8 and u16 parameters read the low parts.

   A palette is 256 RGB triples of 6-bit components (0..63), as the DAC takes them. */
#ifndef XPAL_H
#define XPAL_H

#include "xngine.h"

/* the palette last set (xn_pal_set): fades and the nearest-colour searches start from it */
extern u8 *xn_pal_current;
/* 768 zero bytes: a black palette */
extern u8 xn_pal_black[768];
/* a fade's 16.16 components and their per-step deltas (xn_pal_fade_to) */
extern s32 xn_pal_fade_rgb[768];
extern s32 xn_pal_fade_step[768];
/* the 16 EGA colours as 6-bit RGB triples, and the palette index nearest to each */
extern u8 xn_pal_ega16_rgb[16 * 3];
extern u8 xn_pal_ega16_map[16];
/* the nearest-colour searches' best distance and index so far (the asm keeps them here) */
extern s32 xn_pal_best_dist;
extern s32 xn_pal_best_index;
/* The scratch buffer every other dead palette routine borrows (12D970 reads the DAC into it) */
extern u8 *big_buffer;

/* A colour in HSV: hue 0..0x17F (0x80 per primary: red 0, green 80h, blue 100h), saturation
   and value 0..63 */
typedef struct xn_pal_hsv {
    u16 h, s, v;
} xn_pal_hsv;

/* xn_pal_find_nearest_hsv's target colour (0x12B80C; xn_pal_rgb_to_hsv and the dead
   xn_pal_hsv_to_rgb use these words as scratch and put them back) */
extern xn_pal_hsv xn_pal_hsv_target;

/* xn_pal_rgb_to_hsv's (and the dead xn_pal_hsv_to_rgb's) working words (0x12B85A): the asm
   keeps its intermediate values here, so the C does too */
typedef struct xn_pal_hsv_work {
    u16 r, g, b;            /* 00 the colour converted */
    u16 unused06[3];        /* 06 */
    u16 f, p, q, t;         /* 0C xn_pal_hsv_to_rgb: the hue's fraction and its three levels */
    u16 max;                /* 14 the largest component (the value) */
    u16 min;                /* 16 the smallest */
    u16 delta;              /* 18 max - min, when the saturation is not 0 */
} xn_pal_hsv_work;
extern xn_pal_hsv_work xn_pal_hsv_scratch;

/* Writes count DAC entries from first, from 8-bit components (each >> 2). The index wraps at
   256, and a count of 0 runs the `loop` 2^32 times, as the asm does. title_menu, the FLC
   player, the sky and dungeon loaders, prison_serve_sentence. */
void xn_pal_set_range_8bit(const u8 *rgb, u16 first, u16 count);

/* Turns a palette of 8-bit components into 6-bit ones in place (>> 2) and sets it. */
void xn_pal_set_all_8bit(u8 *pal);

/* Reads the 256 DAC entries into dst (768 bytes). screenshot_save_bmp. */
void xn_pal_read_dac(u8 *dst);

/* The palette index 1..255 nearest to (r, g, b) in the current palette: the least
   61 dr^2 + 71 dg^2 + 41 db^2, each difference taken as a signed byte. Index 0 is never
   chosen. Only the dead xn_pal_build_ega16_map calls it. */
u8 xn_pal_find_nearest(u8 r, u8 g, u8 b);
#pragma aux xn_pal_find_nearest parm [ebx] [ecx] [edx] value [al] modify exact [eax ebx ecx edx];

/* Dead: the palette index 0..255 nearest to the colour (h, s, v), each entry converted to HSV,
   by dh^2 + ds^2 + dv^2 of the differences as signed bytes. (ABI override: the asm also leaves
   the last entry's HSV in BX CX DX.) */
s32 xn_pal_find_nearest_hsv(u16 h, u16 s, u16 v);
#pragma aux xn_pal_find_nearest_hsv parm [ebx] [ecx] [edx] value [eax] \
    modify exact [eax ebx ecx edx];

/* The HSV of a colour of 6-bit components: value = the largest, saturation = 64 (max - min) /
   max (at most 63), hue = 64 x (the other two's difference) / (max - min) from the primary
   that is largest, plus 0x180 when negative. Only the dead xn_pal_find_nearest_hsv calls it. */
void xn_pal_rgb_to_hsv(u8 r, u8 g, u8 b, xn_pal_hsv *out);
void xn_pal_rgb_to_hsv_r(xn_regs *r);      /* asm: bl cl dl in, bx cx dx out */

/* A colour of 6-bit components, as words */
typedef struct xn_pal_rgb16 {
    u16 r, g, b;
} xn_pal_rgb16;

/* Dead and broken: the RGB of an HSV colour, the inverse of xn_pal_rgb_to_hsv (6 sectors of
   64 hue steps; with the hue's fraction f, the levels p = (63 - s) v / 64,
   q = (63 - s f / 64) v / 64 and t = (63 - (63 - f) s / 64) v / 64, in 16-bit products).
   Only hue sectors 5 and up come back: for a grey (s = 0) and sectors 0-4 the asm returns
   before popping the three words it pushed, so its `ret` jumps to the address they make (the
   old 12B810h and 12B80Eh words) with the stack 2 bytes off: it never returns to its caller
   (no caller exists). The C returns the colour from every path; a wild jump into data is no
   behaviour C can have, and nothing can record it. (ABI override: the RGB in BX CX DX; EAX
   holds the last level computed.) */
void xn_pal_hsv_to_rgb(const xn_pal_hsv *c, xn_pal_rgb16 *out);
void xn_pal_hsv_to_rgb_r(xn_regs *r);      /* asm: h s v in bx cx dx, r g b out */

/* Sets the palette: remembers it as the current one, waits for the start of a vertical
   retrace and writes all 256 DAC entries. */
void xn_pal_set(const u8 *pal);

/* Reads the 256 DAC entries into dst (768 bytes). The FLC player saves the palette with it. */
void xn_pal_get(u8 *dst);

/* Dead: does nothing (pushad; popad; ret). */
void xn_pal_stub_empty(void);
#pragma aux xn_pal_stub_empty parm [] modify exact [eax];

/* Dead: fades to black over steps retraces (ABI override: the asm leaves the black palette's
   address in EAX). */
void xn_pal_fade_to_black(s32 steps);

/* Fades from the current palette to target over steps vertical retraces, in 16.16 steps per
   component, then sets target. 0 steps does nothing. play_death_video passes 50. */
void xn_pal_fade_to(const u8 *target, s32 steps);
#pragma aux xn_pal_fade_to parm [eax] [edx] modify exact [eax edx];

/* Writes DAC entry index from three 16.16 components (their integer parts). */
void xn_pal_fade_write_entry(u8 index, const s32 *rgb);
#pragma aux xn_pal_fade_write_entry parm [ecx] [esi] modify exact [eax edx];

/* Dead: reads the DAC into big_buffer and fills xn_pal_ega16_map with the index nearest to
   each EGA colour in it. */
void xn_pal_build_ega16_map(void);
#pragma aux xn_pal_build_ega16_map parm [] modify exact [eax];

#endif
