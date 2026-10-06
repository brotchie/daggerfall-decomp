/* xvid.h: XnGine's VID movie player (src/engine/vid.c). Canonical C: plain prototypes, Watcom's
   own calling convention; docs/xngine_canonical.md.

   What it does
     Plays the ANIM*.VID movies (the Bethesda logo and intro, the death video, resting and the
     quests'): the file is read through the 64000-byte work buffer (big_buffer) in chunks of
     frames (raw, skip/copy deltas, RLE), palettes and 8-bit sound; the sound is streamed
     through two locked 4000h buffers to the HMI SOS digital driver, and a 60 Hz SOS timer
     callback paces the frames (or the sound's end does, while it plays).

   Format (struct xn_vid_header, xnstruct.h): a header (frames, width, height, flags, the frame
   delay, the version), then chunks of a type byte: 0 a raw frame, 1 a delta, 4 a delta from a
   start row, 3 RLE, 2 a palette, 7Ch an audio header (delay, rate byte), 7Dh audio data; a
   frame chunk starts with its delay in 60 Hz ticks.

   Memory: the player's state (struct xn_vid_player at 0xC10FF, the header at 0xC1000, the SOS
   sample descriptor at 0xC100F; the engine's own), locked for the interrupt-time callbacks
   with the player's code. The frames go to screen_buffer and xn_gfx_present shows them; the
   palettes go to the DAC.

   Calls out (the boundary's exits): the game's dpmi_lock_region and dpmi_unlock_region, malloc
   and free, sound_timer_add and sound_timer_remove, and SOS's start and stop sample; DOS
   through the DOS layer (xdos.h). Calls in: the SOS timer calls xn_vid_timer_cb, and the SOS
   driver xn_vid_audio_done_cb, through the pointers the player gives them.

   Quirks kept (docs/engine/quirks.md): Q-VID-01 (an audio chunk longer than the rest of the
   read buffer copies over its own start), Q-VID-02 (the frame width's 16-bit row step),
   Q-VID-03 (the movie file is opened unchecked), Q-VID-04 (a raw frame crossing the read
   buffer's end goes on with the file handle as its width). Deviation D-VID-01: the asm's unlocks took
   their size from a leftover register (mostly 0: no unlock); the C unlocks what it locked. */
#ifndef XVID_H
#define XVID_H

#include "xngine.h"
#include "xnstruct.h"

/* HMI SOS's start-sample descriptor (include/structs.h struct sos_sample) with the done
   callback the player sets */
typedef struct xn_vid_sos_sample {
    u8 *data;               /* +00 */
    u8 pad04[8];
    s32 length;             /* +0C */
    s32 length2;            /* +10 the length again */
    u8 pad14[24];
    s32 volume;             /* +2C left << 16 | right */
    s32 loop;               /* +30 */
    s32 rate;               /* +34 */
    s32 bits;               /* +38 */
    s32 channels;           /* +3C */
    s32 format;             /* +40 8000h: 8-bit unsigned */
    s32 pan;                /* +44 8000h: the centre */
    u8 pad48[20];
    void (__cdecl *done)(struct xn_vid_sos_sample *s);  /* +5C when the sample has played */
    u8 pad60[144];
} xn_vid_sos_sample;

extern struct xn_vid_header xn_vid_header;
extern xn_vid_sos_sample xn_vid_sample;
extern struct xn_vid_player xn_vid_state;      /* 0xC10FF */

/* Plays the VID movie at path with its top left at (x, y) and the clip window set to it, then
   sets its last palette. With skippable, a mouse button or key stops it. 0 when the file is
   missing or the timer could not be added, else 1. 8 game sites. */
int xn_vid_play(const char *path, s32 x, s32 y, s32 skippable);

/* Opens a movie: the read buffer (big_buffer), the two locked audio buffers, the 60 Hz timer,
   the file (Q-VID-03) and its header; reads the first 64000 bytes and decodes the first
   chunk. */
void xn_vid_open(s32 x, s32 y, const char *path);

/* One pass of the player's loop: while sound streams, waits for the sample to finish
   (restarting it on the other buffer if asked), else for the 60 Hz countdown; stops the sound
   if a frame came while it streamed; then decodes the next chunk. */
void xn_vid_update(void);

/* The end of a movie: stops the sound, unlocks and frees the audio buffers, closes the file
   (unless header flag bit 0), sets the done flag and removes the timer. */
void xn_vid_finish(void);

/* Decodes the next chunk: a frame (raw, delta, delta from a row, RLE: then the audio chunks
   that follow at once), a palette (to the DAC at once), an audio header or audio data; any
   other type ends the movie, and so does the last frame. Quirk Q-VID-04: the rows of a raw
   frame (no shipped movie has one) after a refill of the read buffer are as wide as the DOS
   file handle. */
void xn_vid_decode_chunk(void);

/* Clears the audio state and closes the movie file, if open. */
void xn_vid_close_file(void);

/* Moves the unread bytes (p to the buffer's end) to the buffer's start and reads the file into
   the rest. Returns the buffer's start (p when it is the start: nothing to do). */
u8 *xn_vid_refill(u8 *p);

/* The first read: 64000 bytes into the buffer, the read pointer at its start. */
void xn_vid_fill(void);

/* Adds a frame's delay and the header's to the 60 Hz countdown; on the first frame after an
   audio header, starts the sound or arms its delay. */
void xn_vid_schedule_frame(u16 delay);

/* Copies an audio chunk (a word length, then the bytes) from p into the next audio buffer,
   refilling the read buffer when it runs out (Q-VID-01); the buffers alternate. */
void xn_vid_read_audio(u8 *p);

/* The 60 Hz timer callback (the SOS timer calls it, a plain Watcom function): counts the frame
   countdown down, and the audio delay when it is armed. */
void xn_vid_timer_cb(void);

/* The SOS sample-done callback (the SOS driver calls it with the sample on the stack, as C's
   cdecl does): points the sample at the other audio buffer and flags it; if the last one was
   not taken yet, asks the player to restart the sample instead. */
void __cdecl xn_vid_audio_done_cb(xn_vid_sos_sample *s);

/* Stops the sound, if any. */
void xn_vid_audio_stop(void);

/* Starts the last audio buffer filled as an 8-bit mono sample at the movie's rate. */
void xn_vid_audio_start(void);

#endif
