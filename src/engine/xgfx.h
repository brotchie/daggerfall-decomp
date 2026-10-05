/* xgfx.h: XnGine's graphics modes and screen (src/engine/gfx.c; see xngine.h): mode 13h and
   VESA (VBE through DPMI real-mode calls, banked windows and linear frame buffers), the back
   buffer screen_buffer, presenting it (copying the clip rows to the screen, optionally
   clearing them), clearing, the clip window and the vertical retrace waits.

   Each declaration keeps the function's asm interface (config/xngine_abi.csv): no pragma is
   Watcom's own convention; a pragma names the registers; NAME_r is the glue for a function
   whose asm callers read several registers or flags. */
#ifndef XGFX_H
#define XGFX_H

#include "xngine.h"
#include "xnstruct.h"
#include "xscreen.h"

/* ---- the graphics state (0x14291B..0x143570, struct xn_gfx_state) ----------------------- */
extern s32 xn_gfx_mode;             /* 13h or a VESA mode */
extern s32 xn_gfx_saved_mode;       /* the BIOS mode found at start, restored at the end */
extern s32 xn_gfx_driver;           /* a byte offset into the driver tables: always 0 */
extern s32 xn_gfx_width, xn_gfx_height, xn_gfx_screen_size;
extern s32 xn_gfx_page_count;       /* linear frame buffer pages (1 for a back buffer) */
/* the clip window, exclusive on the right and bottom: every blit and the 3D view clip to it,
   and present copies (and clear clears) its rows top..bottom - 1 */
extern s32 xn_gfx_clip_left, xn_gfx_clip_top, xn_gfx_clip_right, xn_gfx_clip_bottom;
extern s32 xn_gfx_row_offset[768];  /* y * width */
extern u8 *screen_buffer;           /* the back buffer, 32-aligned (or the LFB page drawn) */
extern u8 *xn_gfx_buffer_base;      /* a second copy of it nothing reads */
extern void *xn_gfx_buffer_alloc;   /* the block allocated for it; 0 for a frame buffer */
extern u8 xn_gfx_present_mode;      /* <= 0 (signed): present does nothing; 1 copy; 3 flip */

/* Code addresses are asm entries the asm calls through these, with registers (xn_asmcall) */
typedef void (*xn_gfx_routine)(void);
extern xn_gfx_routine xn_gfx_copy_clear_fn;   /* copy and clear the back buffer: ecx bytes
                                                 esi -> edi (143B80) */
extern xn_gfx_routine xn_gfx_drv_shutdown[];  /* the driver tables, by xn_gfx_driver bytes */
extern xn_gfx_routine xn_gfx_drv_present[];   /* (eax: keep) */
extern xn_gfx_routine xn_gfx_drv_clear[];     /* (al: colour) */
/* the routine at byte offset xn_gfx_driver of a driver table */
#define XN_GFX_DRIVER_FN(table) (*(xn_gfx_routine *)((u8 *)(table) + xn_gfx_driver))

/* ---- VESA (0x15FA00..0x15FB6D) ---------------------------------------------------------- */
extern u32 xn_gfx_vesa_dos_selector;    /* the 768-byte DOS buffer's selector (0: none) */
extern u8 *xn_gfx_vesa_dos_buffer;      /* and its linear address (VBE info lands there) */
extern struct xn_rm_regs xn_gfx_vesa_rm_regs;   /* DPMI 300h: the real-mode int 10h's registers */
extern u16 xn_gfx_vesa_bank_step;       /* window positions per 64K bank (64 / granularity) */
extern u16 xn_gfx_vesa_bank_table[16];  /* the window position of each 64K bank */
extern xn_gfx_routine xn_gfx_vesa_copy_fn;  /* the row copy xn_gfx_vesa_present_banked uses */
extern u8 xn_gfx_vesa_present;          /* 1 when VBE 1.2 or later was found */
extern s32 xn_gfx_vesa_page;            /* the LFB page being drawn (page flipping) */
extern u8 *xn_gfx_vesa_lfb;             /* the mapped linear frame buffer, 0 if none */
extern s32 xn_gfx_vesa_bank;            /* the bank last set */
extern struct xn_vesa_mode_info xn_gfx_vesa_mode_info;
/* xn_gfx_vesa_blit_rect_banked's self-patched operands (KEEP: the asm keeps them in code) */
extern s32 xn_vesa_blit_dwords, xn_vesa_blit_bytes, xn_vesa_blit_skip;

extern u8 text_shadow_colour;           /* present clears the back buffer to this */
extern u32 xn_colour_fill_table[256];   /* i * 01010101h: a colour in all four bytes */

/* the asm entries stored as routine pointers */
extern void asm_xn_gfx_copy_rows(void);

/* The game's malloc and free (Watcom's runtime in object 1) */
void *func_000A10A8(u32 size);
void func_000A117E(void *p);
#define game_malloc func_000A10A8
#define game_free   func_000A117E

/* ---- retrace -------------------------------------------------------------------------- */

/* Waits until the vertical retrace has started (3DAh bit 3 set). inventory_frame,
   inpstr_edit and wait_frames_or_input call it, then xn_gfx_wait_vretrace_end. (Glue: the asm
   saves and restores AX and DX, so its callers find every register as it was.) */
void xn_gfx_wait_vretrace_start(void);
void xn_gfx_wait_vretrace_start_r(xn_regs *r);

/* Waits until the vertical retrace is over (3DAh bit 3 clear). */
void xn_gfx_wait_vretrace_end(void);
void xn_gfx_wait_vretrace_end_r(xn_regs *r);

/* Dead: waits for n retraces (2^32 for 0); EAX comes back 0. */
void xn_gfx_wait_vretraces(u32 n);
void xn_gfx_wait_vretraces_r(xn_regs *r);

/* Presents the back buffer (xn_gfx_present) with the clip window one row taller, unless its
   bottom is the 200th row. main and 27 UI loops. */
void xn_gfx_present_inclusive(s32 keep);
#pragma aux xn_gfx_present_inclusive parm [eax] modify exact [eax];

/* Sets mode 13h or a VESA mode (when VBE was found), allocates and clears the 32-aligned
   back buffer (or clears the linear frame buffer's pages and draws into them), and sets the
   full-screen clip window and the row table. 0 when set, 1 when not. init_video passes
   (13h, 1). */
s32 xn_gfx_set_mode(s32 mode, s32 present_mode);
void xn_gfx_set_mode_r(xn_regs *r);            /* asm: eax mode, dl present mode; eax */

/* Shuts the driver down, frees the back buffer and goes back to the BIOS mode found at
   start (if any); frees the VBE DOS buffer. shutdown_video, crash_screen. */
void xn_gfx_restore_mode(void);
void xn_gfx_restore_mode_r(xn_regs *r);        /* asm: every register kept */

/* Dead: switches to another mode at run time, keeping the palette (read from the DAC first):
   frees and reallocates the back buffer as xn_gfx_set_mode does. 0 when set (or already
   set), 1 (and CF) when not. */
s32 xn_gfx_change_mode(s32 mode);
void xn_gfx_change_mode_r(xn_regs *r);

/* Sets mode 13h (remembering the BIOS mode the first time), 320 x 200, the full clip window
   and the row table. */
void xn_gfx_set_mode13(void);

/* Fills xn_gfx_row_offset with y * xn_gfx_width for 768 rows. */
void xn_gfx_build_row_offsets(void);

/* Sets the clip window (right and bottom exclusive). The blits and the game's book and talk
   screens set it. */
void xn_gfx_set_clip(s32 left, s32 top, s32 right, s32 bottom);

/* Clears the back buffer's clip rows to colour through the driver. */
void xn_gfx_clear(s32 colour);
#pragma aux xn_gfx_clear parm [eax] modify exact [eax];

/* Driver 0's clear: fills the rows clip top..bottom - 1 of the back buffer with the colour.
   (ABI override: only xn_gfx_clear calls it, through the driver table, and saves every
   register around the call.) */
void xn_gfx_drv0_clear(u8 colour);
#pragma aux xn_gfx_drv0_clear parm [eax] modify exact [eax ecx edx edi];

/* Shows the back buffer through the driver; keep = 0 also clears it. Each game frame. */
void xn_gfx_present(s32 keep);
#pragma aux xn_gfx_present parm [eax] modify exact [eax];

/* Driver 0's present. Mode 13h: copies the clip rows to the screen at A0000h (keep = 0: with
   the copy-and-clear routine); a banked VESA mode: the same through the 64K window; a linear
   frame buffer (present mode 3): shows the page drawn and draws into the next one, clearing
   it when keep = 0. Nothing when the present mode is <= 0. (ABI override: only
   xn_gfx_present calls it, through the driver table, and saves every register around the
   call.) */
void xn_gfx_drv0_present(s32 keep);
#pragma aux xn_gfx_drv0_present parm [eax] modify exact [eax ecx edx esi edi];

/* Dead BIOS wrappers (int 10h AH=3 and AH=2, page 0): the cursor's row (EAX) and column
   (EDX), and setting them; every other register as the BIOS leaves it. */
void xn_gfx_get_cursor_pos_r(xn_regs *r);
void xn_gfx_set_cursor_pos_r(xn_regs *r);

/* Copies n bytes of the back buffer from src to dst (dwords, then the rest) and fills the
   source with the clear colour text_shadow_colour. Called through xn_gfx_copy_clear_fn. */
void xn_gfx_copy_and_clear(u8 *dst, u8 *src, u32 n);
void xn_gfx_copy_and_clear_r(xn_regs *r);     /* asm: ecx esi edi in; esi edi advanced */

/* Dead: the same, 512 bytes at a time, each dword read, cleared and written in turn; the rest
   by xn_gfx_copy_and_clear. (ABI override: no caller.) */
void xn_gfx_copy_and_clear_unrolled(u32 *dst, u32 *src, u32 n);
#pragma aux xn_gfx_copy_and_clear_unrolled parm [edi] [esi] [ecx] \
    modify exact [eax ecx edx esi edi];

/* Copies n bytes from src to dst (dwords, then the rest): present's copy without clearing. */
void xn_gfx_copy_rows(u8 *dst, const u8 *src, u32 n);
void xn_gfx_copy_rows_r(xn_regs *r);          /* asm: ecx esi edi in; esi edi advanced */

/* Driver 0's shutdown: nothing. */
void xn_gfx_drv0_shutdown(void);
#pragma aux xn_gfx_drv0_shutdown parm [] modify exact [eax];

/* Looks for VBE 1.2 or later: allocates the 768-byte DOS buffer for the BIOS's answers and
   asks for the controller information (4F00h). 1 (and xn_gfx_vesa_present) when found; the
   buffer is freed again when not. set_mode calls it; the game never finds VESA. */
int xn_gfx_vesa_init(void);
void xn_gfx_vesa_init_r(xn_regs *r);          /* asm: CF when not found */

/* Frees the VBE DOS buffer. */
void xn_gfx_vesa_free(void);

/* Sets a VESA mode: remembers the BIOS mode the first time (4F03h), reads the mode's
   information (and its bank table), sets it (with the linear frame buffer bit when it has
   one), sets the screen size and maps the frame buffer's pages. 1 when set. */
int xn_gfx_vesa_set_mode(s32 mode);
void xn_gfx_vesa_set_mode_r(xn_regs *r);      /* asm: CF when not set */

/* Reads the mode's ModeInfoBlock (4F01h) into the DOS buffer and builds the bank table from
   its window granularity. 1 when it could. */
int xn_gfx_vesa_get_mode_info(s32 mode);
void xn_gfx_vesa_get_mode_info_r(xn_regs *r); /* asm: CF when not */

/* Shows the frame buffer from pixel x of line y (4F07h). The page flip. */
void xn_gfx_vesa_set_display_start(s32 x, s32 y);
#pragma aux xn_gfx_vesa_set_display_start parm [eax] [edx] modify exact [eax];

/* Moves windows A and B to 64K bank `bank` (4F05h), unless it is the bank last set. */
void xn_gfx_vesa_set_bank(s32 bank);
#pragma aux xn_gfx_vesa_set_bank parm [eax] modify exact [eax];

/* Dead: the bank window A shows (4F05h BX=100h), as an index of the bank table (16 when it
   is not in it). */
s32 xn_gfx_vesa_get_bank(void);
#pragma aux xn_gfx_vesa_get_bank parm [] value [eax] modify exact [eax];

/* The video BIOS (int 10h) in real mode through DPMI 300h, with AX BX CX DX. 1 when the call
   worked and the VBE status (AX) is 004Fh. */
int xn_gfx_vesa_int10(u32 ax, u32 bx, u32 cx, u32 dx);
void xn_gfx_vesa_int10_r(xn_regs *r);         /* asm: eax ebx ecx edx in; CF when it failed */

/* The banked VESA present: copies the clip rows of the back buffer to video memory through
   the 64K window at A0000h, switching banks as it goes (keep = 0: copy and clear). */
void xn_gfx_vesa_present_banked(s32 keep);
#pragma aux xn_gfx_vesa_present_banked parm [eax] modify exact [eax];

/* Dead: copies a w x h block of pixels from src to (x, y) on a banked VESA screen, splitting
   rows across bank ends. (ABI override: no caller.) */
void xn_gfx_vesa_blit_rect_banked(s32 x, s32 y, u32 w, u32 h, const u8 *src);
#pragma aux xn_gfx_vesa_blit_rect_banked parm [eax] [edx] [ebx] [ecx] [esi] \
    modify exact [eax ecx edx esi edi];

#endif
