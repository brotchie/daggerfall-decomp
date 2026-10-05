/* xvid.h: XnGine's VID movie player (src/engine/vid.c; see xngine.h): ANIM*.VID files read
   through a 64000-byte buffer (big_buffer) in chunks: frames (raw, skip/copy deltas, RLE),
   palettes, and 8-bit sound streamed through two locked 4000h buffers to the HMI SOS digital
   driver; a 60 Hz SOS timer callback paces the frames.

   Each declaration keeps the function's asm interface (config/xngine_abi.csv): no pragma is
   Watcom's own convention; a pragma names the registers; NAME_r is the glue for a function
   whose asm callers read several registers or flags.

   EDX: the player never sets the size it gives dpmi_unlock_region (the game's C, which skips
   the unlock when the size is 0) when it unlocks the audio buffers and itself: the call gets
   whatever EDX holds, the leftovers of earlier calls. The C keeps that: the functions on the
   way take and return `edx`, what the asm has in EDX there. */
#ifndef XVID_H
#define XVID_H

#include "xngine.h"
#include "xnstruct.h"
#include "xscreen.h"

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
    void (*done)(void);     /* +5C called when the sample has played (xn_vid_audio_done_cb) */
    u8 pad60[144];
} xn_vid_sos_sample;

extern struct xn_vid_header xn_vid_header;
extern xn_vid_sos_sample xn_vid_sample;
extern struct xn_vid_player xn_vid_state;      /* 0xC10FF */

/* Plays the VID movie at path with its top left at (x, y) and the clip window set to it,
   then sets its last palette. With skippable, a mouse button or key stops it. 0 (and CF)
   when the file is missing or the timer could not be added, else 1. The intro, the death
   video, resting, quests (ANIM*.VID). */
int xn_vid_play(const char *path, s32 x, s32 y, s32 skippable);
void xn_vid_play_r(xn_regs *r);     /* asm: eax path, edx x, ebx y, ecx skippable */

/* Opens a movie: the read buffer, the two locked audio buffers, the 60 Hz timer, the file
   and its header; reads the first 64000 bytes and decodes the first chunk. */
void xn_vid_open(s32 x, s32 y, const char *path);
#pragma aux xn_vid_open parm [eax] [edx] [ebx] modify exact [eax];

/* One pass of the player's loop: while sound streams, waits for the sample to finish
   (restarting it on the other buffer if asked), else for the 60 Hz countdown; stops the
   sound if a frame came while it streamed; then decodes the next chunk. */
void xn_vid_update(u32 edx);
#pragma aux xn_vid_update parm [edx] modify exact [eax];

/* The end of a movie: stops the sound, unlocks and frees the audio buffers, closes the file
   (unless header flag bit 0), sets the done flag and removes the timer. Returns EDX as the
   timer removal leaves it. */
u32 xn_vid_finish(u32 edx);
#pragma aux xn_vid_finish parm [edx] value [edx] modify exact [eax ebx ecx edx esi edi];

/* Decodes the next chunk: 0 a raw frame, 1 a skip/copy delta, 4 the same from a start row,
   3 RLE fills and copies, 2 a palette (to the DAC at once), 7Ch an audio header, 7Dh audio
   data; anything else ends the movie (as does the last frame). After a frame, audio chunks
   that follow are read at once. */
void xn_vid_decode_chunk(u32 edx);
#pragma aux xn_vid_decode_chunk parm [edx] modify exact [eax ebx ecx edx esi edi];

/* Clears the audio state and closes the movie file, if open. */
void xn_vid_close_file(void);
#pragma aux xn_vid_close_file parm [] modify exact [eax ebx ecx esi edi];

/* Moves the unread bytes (p to the buffer's end) to the buffer's start and reads the file
   into the rest. Returns the buffer's start (or p, when it is the start). */
u8 *xn_vid_refill(u8 *p);
void xn_vid_refill_r(xn_regs *r);   /* asm: esi in and out; bx = the file when it read */

/* The first read: 64000 bytes into the buffer, the read pointer at its start. */
void xn_vid_fill(void);
void xn_vid_fill_r(xn_regs *r);     /* asm: edx = the buffer, bx = the file after */

/* Adds a frame's delay and the header's to the 60 Hz countdown; on the first frame after an
   audio header, starts the sound or arms its delay. */
void xn_vid_schedule_frame(u16 delay);
#pragma aux xn_vid_schedule_frame parm [eax] modify exact [eax];

/* Copies an audio chunk (a word length, then the bytes) from p into the next audio buffer,
   refilling the read buffer when it runs out; the buffers alternate. */
void xn_vid_read_audio(u8 *p);
#pragma aux xn_vid_read_audio parm [esi] modify exact [eax ebx ecx edx esi edi];

/* The 60 Hz timer callback: counts the frame countdown down, and the audio delay when it is
   armed. */
void xn_vid_timer_cb(void);
#pragma aux xn_vid_timer_cb parm [] modify exact [eax];

/* The SOS sample-done callback: points the sample at the other audio buffer and flags it;
   if the last one was not taken yet, asks the player to restart the sample instead. */
void xn_vid_audio_done_cb(xn_vid_sos_sample *s);
void xn_vid_audio_done_cb_r(xn_regs *r);    /* asm: the sample on the stack (no pop) */

/* Stops the sound, if any. Returns EDX as the SOS call leaves it (or edx). */
u32 xn_vid_audio_stop(u32 edx);
#pragma aux xn_vid_audio_stop parm [edx] value [edx] modify exact [eax edx];

/* Starts the last audio buffer filled as an 8-bit mono sample at the movie's rate. */
void xn_vid_audio_start(void);
#pragma aux xn_vid_audio_start parm [] modify exact [eax];

#endif
