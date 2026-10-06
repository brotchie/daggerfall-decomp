/* xmouse.h: XnGine's mouse (src/engine/mouse.c). Canonical C: plain prototypes, Watcom's own
   calling convention; docs/xngine_canonical.md.

   What it does
     Polls the mouse driver (xpc.h's int 33h calls) for the buttons and the position, clamps
     the position to the game's window, finds double clicks, sets the driver's range and
     sensitivity, and keeps a 16 x 16 software cursor drawn into the back buffer: the pixels
     under it saved, the image drawn with colour 0 transparent (group B's draw functions).

   Units
     positions   screen pixels (16-bit signed); the driver's range 0..310 x 0..194 in the
                 game's 320 x 200 mode.
     buttons     bit 0 left, bit 1 right.
     time        BIOS ticks: a double click is a second press within 8 ticks (0.44 s) and 4
                 pixels of the first.

   The state (object 2, one block at 0x12AC00: struct xn_mouse_state in xnstruct.h): the
   game's mouse_buttons, mouse_double_click, mouse_x, mouse_y and the clamp window
   mouse_x_min..mouse_y_max; XnGine's xn_mouse_* (the previous buttons, the cursor's
   rectangle, the saved background, its clipped image, the image and hotspot, the last presses).
   xn_mouse_cursor_drawn (0x147964): bit 0 while the cursor is drawn.

   Quirks (docs/engine/quirks.md): Q-MOUSE-01 (the cursor's y takes the x hotspot), Q-MOUSE-02
   (16-bit distances: -32768 counts as near). */
#ifndef XMOUSE_H
#define XMOUSE_H

#include "xngine.h"

extern u8 mouse_buttons;                    /* 1 left, 2 right */
extern u8 xn_mouse_buttons_prev;            /* at the previous poll */
extern u16 mouse_double_click;              /* 1 left, 2 right */
extern s16 mouse_x, mouse_y;
extern s16 mouse_x_min, mouse_x_max, mouse_y_min, mouse_y_max;
extern s32 xn_mouse_cursor_x, xn_mouse_cursor_y;    /* the cursor's clipped top left */
extern u8 xn_mouse_cursor_under[256];       /* the pixels under the cursor */
extern u8 xn_mouse_cursor_clipped[256];     /* its visible part, rows packed */
extern u8 xn_mouse_cursor_image[256];       /* 16 x 16, colour 0 transparent */
extern s16 xn_mouse_cursor_w, xn_mouse_cursor_h;    /* its visible size */
extern s16 xn_mouse_hotspot_x, xn_mouse_hotspot_y;
extern s32 xn_mouse_under_x, xn_mouse_under_y;      /* where `under` was saved */
extern u32 xn_mouse_left_press_tick, xn_mouse_right_press_tick;
extern s16 xn_mouse_left_press_x, xn_mouse_left_press_y;
extern s16 xn_mouse_right_press_x, xn_mouse_right_press_y;
extern u16 xn_mouse_cursor_drawn;           /* bit 0: drawn, background saved */
extern s32 xn_gfx_width, xn_gfx_height;     /* the screen (group B's) */

#define XN_MOUSE_LEFT   1
#define XN_MOUSE_RIGHT  2

/* ---- the driver ---------------------------------------------------------------------------- */

/* init_video: the driver's range 0..310 x 0..194. */
void xn_mouse_set_range_320x200(void);

/* Dead: the range 0..620 x 0..460. */
void xn_mouse_set_range_640x480(void);

/* init_video: the driver's range 10..630 x 10..460 and the pointer at (10, 10). */
void xn_mouse_init(void);

/* The driver's sensitivity: horizontal and vertical (the options screen gives 6 x its
   setting), double-speed threshold 50. The options, init_game_data, shutdown_free_all. */
void xn_mouse_set_sensitivity(s32 horizontal, s32 vertical);

/* The driver's sensitivity to *horizontal and *vertical (their low bytes). */
void xn_mouse_get_sensitivity(u8 *horizontal, u8 *vertical);

/* Dead: the PS/2 pointing device's sample rate. */
void xn_mouse_ps2_set_sample_rate(u8 rate);

/* main, every frame: the motion since the last call, in mickeys, to *mx and *my (either may
   be 0). */
void xn_mouse_read_motion(s16 *mx, s16 *my);

/* The driver's pointer to (x, y). */
void xn_mouse_set_position(s32 x, s32 y);

/* ---- polling ------------------------------------------------------------------------------- */

/* The buttons and the position from the driver; mouse_double_click is set for a button
   pressed now (not at the last poll) within 8 BIOS ticks and 4 pixels of its last press
   (Q-MOUSE-02), and cleared otherwise; each press is remembered. */
void xn_mouse_poll(void);

/* Every menu loop (107 game sites) and the movie player: xn_mouse_poll, then mouse_x and
   mouse_y clamped to mouse_x_min..max and mouse_y_min..max (signed). */
void xn_mouse_poll_clamped(void);

/* ---- the cursor --------------------------------------------------------------------------- */

/* Moves the cursor to (x, y): mouse_x and mouse_y, the cursor erased and drawn again
   (14 game sites). */
void xn_mouse_cursor_move(s16 x, s16 y);

/* Puts the saved background back over the cursor, when it is drawn. */
void xn_mouse_cursor_erase(void);

/* The cursor's rectangle: the mouse position less the hotspot, clipped to the screen; the
   visible part of the image packed into xn_mouse_cursor_clipped. Q-MOUSE-01: the x hotspot is
   taken off y too (the game's hotspot is 0, 0). */
void xn_mouse_cursor_clip(void);

/* Draws the cursor (when it is not drawn): clipped, the background under it saved, the image
   drawn with colour 0 transparent. */
void xn_mouse_cursor_draw(void);

/* intrface_init: the cursor image (16 x 16 bytes) and its hotspot. */
void xn_mouse_set_cursor_image(const u8 *image, s16 hot_x, s16 hot_y);

#endif
