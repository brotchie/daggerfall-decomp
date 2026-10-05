/* xmouse.h: XnGine's mouse (src/engine/mouse.c; see xngine.h): int 33h polling with clamps and
   double clicks, the driver's ranges and sensitivity, and a 16x16 software cursor drawn into
   the back buffer. The state is the game's (mouse_x...) and XnGine's (xn_mouse_*), in one
   block at 0x12AC00 (struct xn_mouse_state in xnstruct.h).

   Each declaration keeps the function's asm interface (config/xngine_abi.csv): no pragma is
   Watcom's own convention; a pragma names the registers; NAME_r is the glue for a function
   whose asm callers read several registers or flags, or (here) an int 33h wrapper whose
   registers go to the driver as its caller left them. */
#ifndef XMOUSE_H
#define XMOUSE_H

#include "xngine.h"
#include "xsysutil.h"

extern u8 mouse_buttons;                    /* 1 left, 2 right */
extern u8 xn_mouse_buttons_prev;            /* at the previous poll */
extern u16 mouse_double_click;              /* 1 left, 2 right (the asm writes a word) */
extern s16 mouse_x, mouse_y;
extern s16 mouse_x_min, mouse_x_max, mouse_y_min, mouse_y_max;
extern s32 xn_mouse_cursor_x, xn_mouse_cursor_y;    /* the cursor's clipped top left */
extern u8 xn_mouse_cursor_under[256];       /* the pixels under the cursor */
extern u8 xn_mouse_cursor_clipped[256];     /* its visible part, rows packed */
extern u8 xn_mouse_cursor_image[256];       /* 16x16, colour 0 transparent */
extern s16 xn_mouse_cursor_w, xn_mouse_cursor_h;    /* its visible size */
extern s16 xn_mouse_hotspot_x, xn_mouse_hotspot_y;
extern s32 xn_mouse_under_x, xn_mouse_under_y;      /* where `under` was saved */
extern u32 xn_mouse_left_press_tick, xn_mouse_right_press_tick;
extern s16 xn_mouse_left_press_x, xn_mouse_left_press_y;
extern s16 xn_mouse_right_press_x, xn_mouse_right_press_y;
extern u16 xn_mouse_cursor_drawn;           /* bit 0: drawn, background saved (0x147964) */
extern u32 xn_gfx_width, xn_gfx_height;     /* the screen (group 5's) */

/* Group 1's register-interface blits, through their asm entries (with xn_asmcall) */
void asm_xn_draw_get_rect_regs(void);
void asm_xn_draw_put_rect_regs(void);
void asm_xn_draw_image_transparent_regs(void);

/* init_video: the mouse's range 0..310 x 0..194 (int 33h 7 and 8). The driver gets AX, CX
   and DX with the upper halves as the caller left them; every register is kept. */
void xn_mouse_set_range_320x200_r(xn_regs *r);

/* Dead: the range 0..620 x 0..460. */
void xn_mouse_set_range_640x480_r(xn_regs *r);

/* The driver's sensitivity (int 33h 1Ah): horizontal, vertical, double-speed threshold 50.
   The options screen (6 x the setting), init_game_data, shutdown_free_all. */
void xn_mouse_set_sensitivity(s32 horizontal, s32 vertical);

/* The driver's sensitivity (int 33h 1Bh) to *horizontal and *vertical (bytes). */
void xn_mouse_get_sensitivity(u8 *horizontal, u8 *vertical);
#pragma aux xn_mouse_get_sensitivity parm [eax] [edx] modify exact [eax edx];

/* Dead: the PS/2 pointing device's sample rate (int 15h C202h, BH = rate). */
void xn_mouse_ps2_set_sample_rate(u8 rate);
#pragma aux xn_mouse_ps2_set_sample_rate parm [eax] modify exact [eax];

/* init_video: the driver's range 10..630 x 10..460 and the pointer at (10, 10). */
void xn_mouse_init(void);
#pragma aux xn_mouse_init parm [] modify exact [eax ebx];

/* Every menu loop (107 game sites): xn_mouse_poll, then mouse_x and mouse_y clamped to
   mouse_x_min..max and mouse_y_min..max (signed). */
void xn_mouse_poll_clamped(void);
#pragma aux xn_mouse_poll_clamped parm [] modify exact [eax];

/* int 33h 3: the buttons and position; a new left or right press within 8 BIOS ticks and 4
   pixels of the last one sets mouse_double_click bit 0 or 1. FS = DS after it. */
void xn_mouse_poll(void);
#pragma aux xn_mouse_poll parm [] modify exact [eax ecx edx ebx esi edi];

/* Moves the cursor to (x, y): mouse_x and mouse_y, the cursor erased and drawn again. */
void xn_mouse_cursor_move(s16 x, s16 y);
#pragma aux xn_mouse_cursor_move parm [eax] [edx] modify exact [eax edx];

/* Puts the saved background back over the cursor, when it is drawn. */
void xn_mouse_cursor_erase(void);
#pragma aux xn_mouse_cursor_erase parm [] modify exact [eax edx];

/* The cursor's rectangle: the mouse position less the hotspot, clipped to the screen; the
   visible part of the image packed into xn_mouse_cursor_clipped. Kept from the asm: the x
   hotspot is taken off y too (the game's hotspot is 0, 0). */
void xn_mouse_cursor_clip(void);

/* Draws the cursor (when it is not drawn): clipped, the background under it saved, the image
   drawn with colour 0 transparent. */
void xn_mouse_cursor_draw(void);
#pragma aux xn_mouse_cursor_draw parm [] modify exact [eax];

/* intrface_init: the cursor image (16x16 bytes) and its hotspot. */
void xn_mouse_set_cursor_image(const u8 *image, s16 hot_x, s16 hot_y);

/* main, every frame: the motion counters since the last call (int 33h 0Bh) to *mx and *my
   (either may be 0). The driver's EAX: AX = 0Bh, the upper half the mx pointer's (the asm
   sets AX only). FS = DS after it. */
void xn_mouse_read_motion(s16 *mx, s16 *my);
#pragma aux xn_mouse_read_motion parm [eax] [edx] modify exact [eax edx];

/* The pointer to (x, y) (int 33h 4). FS = DS after it. */
void xn_mouse_set_position(s32 x, s32 y);
#pragma aux xn_mouse_set_position parm [eax] [edx] modify exact [eax ecx edx ebx esi edi];

#endif
