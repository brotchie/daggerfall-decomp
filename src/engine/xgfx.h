/* xgfx.h: XnGine's graphics modes and screen (src/engine/gfx.c). Canonical C: plain prototypes,
   Watcom's own calling convention; docs/xngine_canonical.md.

   What it does
     Sets the video mode (13h, or a VESA mode when VBE 1.2 or later answers), keeps the back
     buffer every 2D blit and the 3D view draw into (screen_buffer), presents it (copies the
     clip window's rows to the screen, optionally clearing them), clears it, keeps the clip
     window and the row table, and waits for the vertical retrace. Present, clear and
     shutdown go through a display driver (one: driver 0); VESA goes through the video BIOS
     in real mode (DPMI 0300h), with 64K banks or a linear frame buffer.

   Units
     pixels       bytes, one per pixel (256 colours); rows xn_gfx_width bytes apart (320)
     the clip     left and top inclusive, right and bottom exclusive: every blit and the 3D
                  view clip to it; present copies (and clear clears) rows top..bottom - 1
     VESA         banks of 64K, positioned in the window's granularity (KB); pages of a
                  linear frame buffer, xn_gfx_screen_size bytes each

   Memory (object 2, the struct xn_gfx_state at 0x14291B..0x143570, and the VESA state at
   0x15FA00..0x15FB6D): the game reads the clip window, the row table and screen_buffer; the
   rest is the engine's. The game never finds VESA (its paths never run).

   Hardware: the video BIOS (int 10h: mode 13h, the mode found at start, the cursor), VBE
   through DPMI 0300h, DOS memory through DPMI 0100h/0101h/0006h, the frame buffer's mapping
   through DPMI 0800h; port 3DAh (bit 3: in the vertical retrace); VGA memory at A0000h.

   Quirks kept (docs/engine/quirks.md): Q-GFX-01 (present_inclusive's extra row), Q-GFX-02
   (wait_vretraces(0) waits 2^32 times), Q-GFX-03 (the linear frame buffer's address loses its
   low word), Q-GFX-04 (the banked present's first copy runs to the bank's end). */
#ifndef XGFX_H
#define XGFX_H

#include "xngine.h"
#include "xnstruct.h"

/* ---- the screen (the game reads these) ---------------------------------------------------- */
extern u8 *screen_buffer;           /* the back buffer, 32-aligned (or the LFB page drawn) */
extern s32 xn_gfx_width, xn_gfx_height, xn_gfx_screen_size;
extern s32 xn_gfx_clip_left, xn_gfx_clip_top, xn_gfx_clip_right, xn_gfx_clip_bottom;
extern s32 xn_gfx_row_offset[768];  /* y * xn_gfx_width */

/* the address of row y, column x of the back buffer */
#define XN_SCREEN(x, y) (screen_buffer + xn_gfx_row_offset[y] + (x))

/* ---- the engine's graphics state ---------------------------------------------------------- */
extern s32 xn_gfx_mode;             /* 13h or a VESA mode */
extern s32 xn_gfx_saved_mode;       /* the BIOS mode found at start, restored at the end */
extern s32 xn_gfx_driver;           /* the display driver: a byte offset in the asm's driver
                                       tables, 4 per driver; always 0 */
extern s32 xn_gfx_page_count;       /* linear frame buffer pages (1 for a back buffer) */
extern u8 *xn_gfx_buffer_base;      /* a second copy of screen_buffer nothing reads */
extern void *xn_gfx_buffer_alloc;   /* the block allocated for it; 0 for a frame buffer */
extern u8 xn_gfx_present_mode;      /* <= 0 (signed): present does nothing; 1 copy; 3 flip */
extern u8 text_shadow_colour;       /* present clears the back buffer to this (the game's) */
extern u8 text_colour;              /* the colour of text, lines and filled rectangles (the game's) */
extern u32 xn_colour_fill_table[256];   /* i * 01010101h: a colour in all four bytes */

/* VESA */
extern u32 xn_gfx_vesa_dos_selector;    /* the 768-byte DOS buffer's selector (0: none) */
extern u8 *xn_gfx_vesa_dos_buffer;      /* and its linear address (VBE answers land there) */
extern struct xn_rm_regs xn_gfx_vesa_rm_regs;   /* DPMI 300h: the real-mode int 10h's registers */
extern u16 xn_gfx_vesa_bank_step;       /* window positions per 64K bank (64 / granularity) */
extern u16 xn_gfx_vesa_bank_table[16];  /* the window position of each 64K bank */
extern u8 xn_gfx_vesa_present;          /* 1 when VBE 1.2 or later was found */
extern s32 xn_gfx_vesa_page;            /* the LFB page being drawn (page flipping) */
extern u8 *xn_gfx_vesa_lfb;             /* the mapped linear frame buffer, 0 if none */
extern s32 xn_gfx_vesa_bank;            /* the bank last set */
extern struct xn_vesa_mode_info xn_gfx_vesa_mode_info;

/* The game's allocator (Watcom's runtime in object 1: the boundary's exits) */
void *func_000A10A8(u32 size);
void func_000A117E(void *p);
#define game_malloc func_000A10A8
#define game_free   func_000A117E

/* ---- retrace -------------------------------------------------------------------------------- */

/* Waits until the vertical retrace has started (3DAh bit 3 set). inventory_frame, inpstr_edit
   and wait_frames_or_input call it, then xn_gfx_wait_vretrace_end. */
void xn_gfx_wait_vretrace_start(void);

/* Waits until the vertical retrace is over (3DAh bit 3 clear). 3 game sites. */
void xn_gfx_wait_vretrace_end(void);

/* Dead: waits for n retraces (Quirk Q-GFX-02: n = 0 waits 2^32). */
void xn_gfx_wait_vretraces(u32 n);

/* ---- present and clear ---------------------------------------------------------------------- */

/* Presents the back buffer (xn_gfx_present). Quirk Q-GFX-01: with the clip window one row
   taller, unless its bottom is the 200th row: the row under the window goes out too. main and
   27 UI loops. */
void xn_gfx_present_inclusive(s32 keep);

/* Shows the back buffer through the driver; keep = 0 also clears it to text_shadow_colour. */
void xn_gfx_present(s32 keep);

/* Clears the back buffer's clip rows to colour through the driver. 5 game sites. */
void xn_gfx_clear(s32 colour);

/* Driver 0's present. Mode 13h: copies the clip rows to the screen at A0000h (keep = 0: and
   clears them, through the copy-and-clear routine); a banked VESA mode: the same through the
   64K window; a linear frame buffer (present mode 3): shows the page drawn and draws into the
   next one, clearing it when keep = 0. Nothing when the present mode is <= 0. */
void xn_gfx_drv0_present(s32 keep);

/* Driver 0's clear: fills the rows clip top..bottom - 1 of the back buffer with the colour. */
void xn_gfx_drv0_clear(u8 colour);

/* Driver 0's shutdown: nothing. */
void xn_gfx_drv0_shutdown(void);

/* Copies n bytes from src to dst (dwords, then the rest). */
void xn_gfx_copy_rows(u8 *dst, const u8 *src, u32 n);

/* Copies n bytes from src to dst and fills the source with the clear colour text_shadow_colour
   (dwords, then the rest): present's copy that clears. */
void xn_gfx_copy_and_clear(u8 *dst, u8 *src, u32 n);

/* Dead: the same, 512 bytes at a time, each dword read, cleared and written in turn; the rest
   by xn_gfx_copy_and_clear (only a dead benchmark at 143A5E would select it). */
void xn_gfx_copy_and_clear_unrolled(u32 *dst, u32 *src, u32 n);

/* ---- modes ---------------------------------------------------------------------------------- */

/* Sets mode 13h, or a VESA mode when VBE was found (any mode but 13h), allocates and clears the
   32-aligned back buffer (or clears the linear frame buffer's pages and draws into them), and
   sets the full-screen clip window and the row table; present_mode as given (a linear frame
   buffer: 3). 0 when set, 1 when not. init_video passes (13h, 1). */
s32 xn_gfx_set_mode(s32 mode, u8 present_mode);

/* Shuts the driver down, frees the back buffer and goes back to the BIOS mode found at start
   (if any), then frees the VBE DOS buffer. shutdown_video, crash_screen, and every fatal exit. */
void xn_gfx_restore_mode(void);

/* Dead: switches to another mode at run time, keeping the palette (read from the DAC first):
   frees and reallocates the back buffer as xn_gfx_set_mode does. 0 when set (or already set),
   1 when not. */
s32 xn_gfx_change_mode(s32 mode);

/* Sets mode 13h (remembering the BIOS mode the first time), 320 x 200, the full clip window
   and the row table. */
void xn_gfx_set_mode13(void);

/* Fills xn_gfx_row_offset with y * xn_gfx_width for 768 rows. */
void xn_gfx_build_row_offsets(void);

/* Sets the clip window (right and bottom exclusive). */
void xn_gfx_set_clip(s32 left, s32 top, s32 right, s32 bottom);

/* Dead BIOS calls (int 10h AH=3, AH=2; page 0): the text cursor's row and column, and setting
   them (the column plus the row times 256 in DX). */
void xn_gfx_get_cursor_pos(s32 *row, s32 *col);
void xn_gfx_set_cursor_pos(s32 row, s32 col);

/* ---- VESA ----------------------------------------------------------------------------------- */

/* Looks for VBE 1.2 or later: allocates the 768-byte DOS buffer for the BIOS's answers and asks
   for the controller information (4F00h). 1 (and xn_gfx_vesa_present) when found; the buffer
   is freed again when not. */
int xn_gfx_vesa_init(void);

/* Frees the VBE DOS buffer, if any. */
void xn_gfx_vesa_free(void);

/* Sets a VESA mode: remembers the BIOS mode the first time (4F03h), reads the mode's
   information (and its bank table), sets it (with the linear frame buffer bit when it has one),
   sets the screen size and maps the frame buffer's pages (Q-GFX-03). 1 when set; 0 (and the
   saved mode forgotten) when not. */
int xn_gfx_vesa_set_mode(s32 mode);

/* Reads the mode's ModeInfoBlock (4F01h) into the DOS buffer and builds the bank table from its
   window granularity. 1 when it could. */
int xn_gfx_vesa_get_mode_info(s32 mode);

/* Shows the frame buffer from pixel x of line y (4F07h): the page flip. */
void xn_gfx_vesa_set_display_start(s32 x, s32 y);

/* Moves windows A and B to 64K bank `bank` (4F05h), unless it is the bank last set. */
void xn_gfx_vesa_set_bank(s32 bank);

/* Dead: the bank window A shows (4F05h BX=100h), as an index of the bank table (16 when it is
   not in it). */
s32 xn_gfx_vesa_get_bank(void);

/* The video BIOS (int 10h) in real mode through DPMI 300h, with AX BX CX DX. 1 when the call
   worked and the VBE status (AX) is 004Fh. */
int xn_gfx_vesa_int10(u32 ax, u32 bx, u32 cx, u32 dx);

/* The banked VESA present: copies the clip rows of the back buffer to video memory through the
   64K window at A0000h, switching banks as it goes (keep = 0: copy and clear). Q-GFX-04. */
void xn_gfx_vesa_present_banked(s32 keep);

/* Dead: copies a w x h block of pixels from src to (x, y) on a banked VESA screen, splitting
   rows across bank ends. */
void xn_gfx_vesa_blit_rect_banked(s32 x, s32 y, u32 w, u32 h, const u8 *src);

#endif
