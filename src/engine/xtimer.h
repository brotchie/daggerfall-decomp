/* xtimer.h: XnGine's timing (src/engine/timer.c; see xngine.h): the BIOS tick count, the
   frame rate, the PIT and two timer callbacks. */
#ifndef XTIMER_H
#define XTIMER_H

#include "xngine.h"

/* The BIOS tick count (18.2 Hz) at 0040:006C, a flat address */
#define XN_BIOS_TICKS (*(volatile u32 *)0x46C)

/* The frame rate (0x12AA04): main sets 23 at the start, xn_timer_fps_update each second */
extern u32 xn_timer_fps;
extern u32 xn_fps_frame_count;          /* frames counted this second (0xCDE06) */
extern u32 xn_fps_last_tick;            /* the BIOS tick of the last update (0xCDE0A) */
extern u32 xn_fps_average;              /* (old + new) / 2 of the rate (0xCDE0E) */

/* The game's: counted by xn_timer_tick_callback (140 Hz), read and cleared by
   frame_ticks_update */
extern volatile s32 timer_tick_count;
/* The game's: what the dead xn_timer_add_25 adds to */
extern s32 frame_timer_ms;

/* The BIOS tick count. damage_collapse_exhausted waits 22 ticks with it. */
u32 xn_timer_bios_ticks(void);

/* Dead: waits until *counter differs from *last, then stores it in *last. */
void xn_timer_wait_change(s32 *last, volatile s32 *counter);
#pragma aux xn_timer_wait_change parm [eax] [edx] modify exact [eax];

/* sound_init_music registers it as a 140 Hz timer callback: counts timer_tick_count. */
void xn_timer_tick_callback(void);
#pragma aux xn_timer_tick_callback parm [] modify exact [eax];

/* Dead: frame_timer_ms += 25, shaped like a timer callback. */
void xn_timer_add_25(void);
#pragma aux xn_timer_add_25 parm [] modify exact [eax];

/* main, every frame: counts the frame; once more than 18 BIOS ticks (a second) have passed,
   the count becomes the frame rate (and is averaged with the last) and starts again. Returns
   the frame rate, its low word. */
u32 xn_timer_fps_update(void);

/* The count of PIT channel 0 (port 40h, low byte then high). Kept from the asm: the command
   it writes first (0B0h to port 43h) programs channel 2, mode 0, not a latch of channel 0. */
u32 xn_timer_read_pit(void);

/* Busy-waits until the BIOS tick count has advanced by n. init_video waits 18 (a second). */
void xn_timer_wait_ticks(u32 n);

#endif
