/* xtimer.h: XnGine's timing (src/engine/timer.c). Canonical C: plain prototypes, Watcom's own
   calling convention; docs/xngine_canonical.md.

   What it does
     The BIOS tick count (18.2 a second: XN_BIOS_TICKS, xpc.h) and waits on it, the frame
     rate main shows, a read of the PIT, and the 140 Hz callback the game hands the sound
     library's timer (the SOS timer service calls it through a pointer: a plain void(void)
     function, Watcom's convention; the movie player's xn_vid_timer_cb is the other).

   Globals (object 2): xn_timer_fps (main sets 23 at the start), xn_fps_frame_count,
   xn_fps_last_tick, xn_fps_average. The game's: timer_tick_count (counted at 140 Hz, read and
   cleared by frame_ticks_update), frame_timer_ms.

   Quirks (docs/engine/quirks.md): Q-TIMER-01 (the PIT read programs channel 2 instead of
   latching channel 0). */
#ifndef XTIMER_H
#define XTIMER_H

#include "xngine.h"
#include "xpc.h"

extern u32 xn_timer_fps;                /* the frame rate (0x12AA04) */
extern u32 xn_fps_frame_count;          /* frames counted this second (0xCDE06) */
extern u32 xn_fps_last_tick;            /* the BIOS tick of the last update (0xCDE0A) */
extern u32 xn_fps_average;              /* (old + new) / 2 of the rate (0xCDE0E) */

/* The game's */
extern volatile s32 timer_tick_count;
extern s32 frame_timer_ms;

/* The BIOS tick count. damage_collapse_exhausted waits 22 ticks with it (2 game sites). */
u32 xn_timer_bios_ticks(void);

/* Dead: waits until *counter differs from *last, then stores it in *last. */
void xn_timer_wait_change(s32 *last, volatile s32 *counter);

/* The sound library's 140 Hz timer callback (sound_init_music hands it over): counts
   timer_tick_count. */
void xn_timer_tick_callback(void);

/* Dead: frame_timer_ms += 25, shaped like a timer callback. */
void xn_timer_add_25(void);

/* main, every frame: counts the frame; once more than 18 BIOS ticks (a second) have passed
   since the last update, the count becomes the frame rate (averaged with the last into
   xn_fps_average) and starts again. Returns the frame rate's low word. */
u32 xn_timer_fps_update(void);

/* The count of PIT channel 0 (port 40h, low byte then high; 2 game sites). Q-TIMER-01: the
   command it writes first (0B0h to port 43h) programs channel 2, mode 0, instead of latching
   channel 0. */
u32 xn_timer_read_pit(void);

/* Busy-waits until the BIOS tick count has advanced by n. init_video waits 18 (a second). */
void xn_timer_wait_ticks(u32 n);

#endif
