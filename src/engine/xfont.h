/* xfont.h: XnGine's fonts (src/engine/font.c; see xngine.h): the FONT000n.FNT files, the
   glyph blitter and the string drawing and measuring the game's text goes through.

   Each declaration keeps the function's asm interface (config/xngine_abi.csv): no pragma is
   Watcom's own convention; a pragma names the registers; NAME_r is the glue for a function
   whose asm callers read several registers or flags. */
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
extern char xn_font_msg_no_font[];      /* 'FONT: Attempt to set non-existing font.' */
/* the glyph blitter's self-patched operands (rule KEEP) */
extern u8 xn_font_glyph_skip;           /* 12DD02: shl eax, SKIP (a byte) */
extern s32 xn_font_glyph_step;          /* 12DD10: add edi, STEP */

/* No font loaded; glyph spacing and the line gap 1. */
void xn_font_init(void);

/* Loads FONT<number>.FNT (4 digits) whole into the slot; returns it. Its asm leaves EBX 4
   (the digit count), which callers read (xn_font_load_r). */
struct xn_fnt_file *xn_font_load(s32 number, s32 slot);
void xn_font_load_r(xn_regs *r);

/* Selects the font in the slot (its glyphs, space width and height); returns the height.
   An empty slot restores the video mode, removes the keyboard handler, frees the memory,
   prints xn_font_msg_no_font and exits to DOS. 47 game sites. */
s32 xn_font_select(s32 slot);
void xn_font_select_r(xn_regs *r);

/* Draws text (NUL-ended) at (x, y) in text_colour with the selected font: ' ' advances by
   the space width, CR returns to the start's x, LF goes down a line (x stays); other
   characters below '!' and those from 80h are skipped. Returns the x after it. */
s32 xn_font_draw_string(s32 x, s32 y, const u8 *text);
void xn_font_draw_string_r(xn_regs *r);

/* Draws one glyph: w pixels of each of font_height 16-bit rows (most significant bit
   leftmost), at (x, y) in text_colour, clipped. Kept bug: after a left clip the asm adds 16
   to the remaining width (not to the columns cut) and shifts the rows by that, so it skips
   the wrong columns and walks 16 blank pixels further (into the right clip). Keeps EAX EBX
   EDX. */
void xn_font_draw_glyph(s32 x, s32 y, s32 w, const u16 *rows);
#pragma aux xn_font_draw_glyph parm [eax] [edx] [ebx] [esi] modify exact [eax ecx esi edi];

/* The width of character c in the selected font: '!' and below (and so anything the string
   functions pass for those) the space width less the glyph spacing. */
s32 xn_font_glyph_width(s32 c);

/* The width of text with the glyph spacing after every character. */
s32 xn_font_string_width(const u8 *text);

/* Dead: the same for at most n characters. */
s32 xn_font_string_width_n(const u8 *text, s32 n);

#endif
