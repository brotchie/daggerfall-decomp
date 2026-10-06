/* xfont.h: XnGine's fonts (src/engine/font.c). Canonical C: plain prototypes, Watcom's own
   calling convention; docs/xngine_canonical.md.

   What it does
     Loads the FONT000n.FNT files into eight slots, selects one, and draws and measures text
     with it: one bitmap glyph per character from '!' (21h) to 7Fh, each a column count and up
     to 16 pixels a row.

   Formats and units
     struct xn_fnt_file (xnstruct.h): the file as loaded whole: the space's width, the rows per
     glyph, then per glyph its width and the offset of its rows; a glyph row is a 16-bit word,
     its most significant bit the leftmost pixel.
     Coordinates are screen pixels; glyphs clip to the clip window (xgfx.h).

   Memory: the text state (where a string started, the selected font, its space width and
   height, the spacing between glyphs and lines) and the slots, in object 2. The game reads
   font_glyphs, font_height, font_space_width, font_char_spacing and xn_font_current.

   Quirks kept: Q-FONT-01 (xn_font_load leaves EBX = 4 to the game), Q-FONT-02 (a glyph's
   left clip shifts by the remaining width plus 16), Q-FONT-03 (a glyph of width 256n draws 256
   columns), Q-FONT-04 (characters 80h-FFh are skipped, '!' measures as a space). Dropped: the
   blitter's two self-patched operands (config/xngine_dropped.csv). docs/engine/quirks.md. */
#ifndef XFONT_H
#define XFONT_H

#include "xngine.h"
#include "xnstruct.h"

/* the text state (struct xn_font_state, as separate globals) */
extern s32 xn_font_text_x0;             /* where the string started: CR returns there */
extern s32 xn_font_text_y0;
extern s32 font_space_width;            /* the selected font's ' ' */
extern s32 font_height;                 /* its rows per glyph */
extern s32 font_char_spacing;           /* pixels between glyphs (1) */
extern s32 xn_font_line_gap;            /* pixels between lines (1) */
extern s32 xn_font_current;             /* the slot selected */
extern struct xn_fnt_file *xn_font_table[8];    /* the loaded fonts by slot */
extern struct xn_fnt_file *font_glyphs; /* the selected font */
extern char xn_font_filename[];         /* 'font????.fnt' */
extern char xn_font_msg_no_font[];      /* 'FONT: Attempt to set non-existing font.$' */

/* No font loaded; glyph spacing and the line gap 1. One game site (init). */
void xn_font_init(void);

/* Loads FONT<number>.FNT (4 digits) whole (xn_dos_load_file: a missing file is fatal) into the
   slot; returns it. Four game sites (init_game_data: fonts 0-3 into slots 1-4); Quirk
   Q-FONT-01: their asm callee left EBX = 4 (the digit count), which the game's next call
   reads: the boundary adapter xn_font_load_b keeps it. */
struct xn_fnt_file *xn_font_load(s32 number, s32 slot);
void xn_font_load_b(xn_regs *r);

/* Selects the font in the slot (its glyphs, space width and height); returns the height. An
   empty slot restores the video mode, removes the keyboard handler, frees the memory, prints
   xn_font_msg_no_font and exits to DOS. 47 game sites. */
s32 xn_font_select(s32 slot);

/* Draws text (NUL-ended) at (x, y) in text_colour with the selected font: ' ' advances by
   the space width, CR returns to the start's x, LF goes down a line (x stays); other
   characters below '!' are skipped, and so are 80h-FFh (Q-FONT-04: a signed compare).
   Returns the x after it. 4 game sites. */
s32 xn_font_draw_string(s32 x, s32 y, const u8 *text);

/* Draws one glyph: w columns of each of font_height 16-bit rows (most significant bit
   leftmost), at (x, y) in text_colour, clipped to the clip window. Quirk Q-FONT-02: after a
   left clip the rows are shifted by the remaining width plus 16 (not by the columns cut), and
   that width is what is drawn: the wrong columns, and 16 blank ones further on (into the
   right clip). Quirk Q-FONT-03: only the low byte of the width counts the columns (a width
   of 256 draws 256, of 0 also 256). */
void xn_font_draw_glyph(s32 x, s32 y, s32 w, const u16 *rows);

/* The width of character c in the selected font; Quirk Q-FONT-04: '!' and below give the space
   width less the glyph spacing. */
s32 xn_font_glyph_width(s32 c);

/* Dead: the width of text with the glyph spacing after every character. */
s32 xn_font_string_width(const u8 *text);

/* Dead: the same for at most n characters (n = 0: none). */
s32 xn_font_string_width_n(const u8 *text, s32 n);

#endif
