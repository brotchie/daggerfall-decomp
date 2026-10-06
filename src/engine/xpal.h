/* xpal.h: XnGine's VGA palette (src/engine/pal.c). Canonical C: plain prototypes, Watcom's own
   calling convention; docs/xngine_canonical.md.

   What it does
     Sets and reads the VGA DAC (256 entries of red, green and blue), waits for the vertical
     retrace before a whole palette goes out, fades from the current palette to another over
     a number of retraces, and finds the palette index nearest to a colour, in RGB or in HSV.

   Formats and units
     palette      768 bytes: 256 RGB triples of 6-bit components (0..63), as the DAC takes
                  them; the "8-bit" calls take 0..255 components and shift each right by 2
     fades        16.16 components and per-retrace steps (one step per vertical retrace)
     HSV          hue 0..17Fh (80h per primary: red 0, green 80h, blue 100h), saturation and
                  value 0..63

   Hardware: ports 3C8h (DAC write index), 3C7h (read index), 3C9h (data), 3DAh bit 3 (in the
   vertical retrace). The DAC is the screen: the game sees every write to it.

   Memory: xn_pal_current (the palette last set: the game reads it, and fades and searches start
   from it), xn_pal_black, the EGA colour table and map (object 2). The fade's working arrays are
   this module's own (pal.c).

   Quirks kept: Q-PAL-01 (a count of 0 writes 2^32 entries), Q-PAL-02 (index 0 is never
   the nearest colour), Q-PAL-03 (signed-byte colour differences), Q-PAL-04 (a negative fade
   length), Q-PAL-05 (HSV to RGB: the asm's wild returns). docs/engine/quirks.md. */
#ifndef XPAL_H
#define XPAL_H

#include "xngine.h"

/* the palette last set (xn_pal_set): fades and the nearest-colour searches start from it; the
   game reads it */
extern u8 *xn_pal_current;
/* 768 zero bytes: a black palette */
extern u8 xn_pal_black[768];
/* the 16 EGA colours as 6-bit RGB triples, and the palette index nearest to each (filled by
   the dead xn_pal_build_ega16_map; xn_img_remap_colours reads it) */
extern u8 xn_pal_ega16_rgb[16 * 3];
extern u8 xn_pal_ega16_map[16];
/* the engine's work buffer (xn_pal_build_ega16_map reads the DAC into it) */
extern u8 *big_buffer;

/* A colour in HSV: hue 0..17Fh, saturation and value 0..63 */
typedef struct xn_pal_hsv {
    u16 h, s, v;
} xn_pal_hsv;

/* A colour of 6-bit components, as words */
typedef struct xn_pal_rgb16 {
    u16 r, g, b;
} xn_pal_rgb16;

/* ---- the DAC --------------------------------------------------------------------------------- */

/* Writes count DAC entries from index first, from 8-bit components (each >> 2). The index wraps
   at 256. Quirk Q-PAL-01: a count of 0 writes 2^32 entries (the asm's `loop` on ECX = 0).
   title_menu, the FLC player, the sky and dungeon loaders, prison_serve_sentence: 9 game
   sites. */
void xn_pal_set_range_8bit(const u8 *rgb, u16 first, u16 count);

/* Turns a palette of 8-bit components into 6-bit ones in place (>> 2) and sets it
   (xn_pal_set). 5 game sites. */
void xn_pal_set_all_8bit(u8 *pal);

/* Reads the 256 DAC entries into dst (768 bytes). screenshot_save_bmp. */
void xn_pal_read_dac(u8 *dst);

/* Sets the palette: remembers it as the current one, waits for the start of a vertical retrace
   (the end of the current one, then the next) and writes all 256 entries. One game site. */
void xn_pal_set(const u8 *pal);

/* Reads the 256 DAC entries into dst (768 bytes). The FLC player saves the palette with it. */
void xn_pal_get(u8 *dst);

/* Dead: does nothing. */
void xn_pal_stub_empty(void);

/* ---- fades ---------------------------------------------------------------------------------- */

/* Fades from the current palette to target over steps vertical retraces (16.16 components, a
   step per retrace), then sets target. 0 steps does nothing; Quirk Q-PAL-04: a negative count
   runs 2^32 - |steps| retraces. play_death_video passes 50. */
void xn_pal_fade_to(const u8 *target, s32 steps);

/* Dead: fades to black over steps retraces. */
void xn_pal_fade_to_black(s32 steps);

/* Writes DAC entry index from three 16.16 components (their integer parts' low bytes). */
void xn_pal_fade_write_entry(u8 index, const s32 *rgb);

/* ---- nearest colours ------------------------------------------------------------------------- */

/* The palette index 1..255 nearest to (r, g, b) in the current palette: the least
   61 dr^2 + 71 dg^2 + 41 db^2 (the first of equals). Quirk Q-PAL-02: index 0 is never chosen;
   Quirk Q-PAL-03: each difference is taken as a signed byte. Only the dead
   xn_pal_build_ega16_map calls it. */
u8 xn_pal_find_nearest(u8 r, u8 g, u8 b);

/* Dead: the palette index 0..255 nearest to the colour (h, s, v), each entry converted to HSV,
   by dh^2 + ds^2 + dv^2 of the differences as signed bytes (Q-PAL-03). */
s32 xn_pal_find_nearest_hsv(u16 h, u16 s, u16 v);

/* The HSV of a colour of 6-bit components: value = the largest, saturation = 64 (max - min) /
   max (at most 63), hue = 64 x (the other two's difference) / (max - min) from the primary
   that is largest (r, then g, then b), plus 180h when negative. Only the dead
   xn_pal_find_nearest_hsv calls it. */
void xn_pal_rgb_to_hsv(u8 r, u8 g, u8 b, xn_pal_hsv *out);

/* Dead and broken: the RGB of an HSV colour, the inverse of xn_pal_rgb_to_hsv (6 sectors of 64
   hue steps; with the hue's fraction f, the levels p = (63 - s) v / 64, q = (63 - s f / 64) v /
   64 and t = (63 - (63 - f) s / 64) v / 64, in 16-bit products). Quirk Q-PAL-05: for a grey and
   hue sectors 0-4 the asm returns into the words it pushed; the C returns the colour. */
void xn_pal_hsv_to_rgb(const xn_pal_hsv *c, xn_pal_rgb16 *out);

/* Dead: reads the DAC into big_buffer and fills xn_pal_ega16_map with the index nearest to each
   EGA colour in it. */
void xn_pal_build_ega16_map(void);

#endif
