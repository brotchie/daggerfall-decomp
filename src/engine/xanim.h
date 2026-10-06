/* xanim.h: XnGine's ASCR animation scripts (src/engine/anim.c). Canonical C: plain prototypes,
   Watcom's own calling convention; docs/xngine_canonical.md.

   What it does
     Runs the creatures' animation scripts (MONSTER.BSA's ASCR records, struct xn_ascr in
     xnstruct.h): each creature has a state (struct xn_anim) the game sets a request in;
     xn_anim_update starts the requested script when it may, then steps it once per change of
     xn_anim_ticks / tick_divisor. A step counts down a wait, or runs the script to its next
     yield: a byte from 80h up is a frame (and yields), 0..11 are opcodes with operands
     (loops, gotos, random gotos, events, frames with waits).

   The interpreter
     A script position is a pointer into the record (the loops keep their counts in the
     script's own bytes). Each opcode is a function (struct xn_anim *a, xn_ascr_pos *pos) that
     moves *pos past its operands, or to its target, and returns 1 to go on or 0 to yield.
     Offsets in operands are from the record's start.

   Randomness
     A 16-bit LCG of its own (xn_anim_rand_state = state * 1509 + 41), seeded from the BIOS
     tick count; xn_anim_rand_range picks lo + (r & 7Fh) % (hi - lo + 1).

   Globals: xn_anim_rand_state (object 2, 0xC0030), xn_anim_ticks (the engine's animation
   clock, 0x1343C0); the asm's table of opcode handlers (0xC0000) is not used by the C.

   Quirks (docs/engine/quirks.md): Q-ANIM-01 (reset_with_rate stores the speed at linear 0Ch),
   Q-ANIM-02 (restart sets the position to the bare offset), Q-ANIM-03 (opcode 11 yields only
   if the position wraps), Q-ANIM-04 (a range with hi = lo - 1 gives 0), Q-ANIM-05 (an opcode
   from 12 up), Q-SYS-01 (a speed of 0). */
#ifndef XANIM_H
#define XANIM_H

#include "xngine.h"
#include "xnstruct.h"

extern u16 xn_anim_rand_state;              /* the animation LCG's state (0xC0030) */
extern u32 xn_anim_ticks;                   /* the engine's animation clock (0x1343C0) */

/* A position in a script (operands are read, and loop counts written, through it) */
typedef u8 *xn_ascr_pos;

#define XN_ANIM_NONE    0xFF                /* request: none; wait: hold */

/* The opcodes */
#define XN_ASCR_LOOP_START      0           /* (lo, hi, count) */
#define XN_ASCR_LOOP_END        1           /* (word start) */
#define XN_ASCR_SET_EVENTS      2           /* (word bits) */
#define XN_ASCR_FRAME_WAIT      3           /* (frame, steps) */
#define XN_ASCR_GOTO            4           /* (word target) */
#define XN_ASCR_RESTART         5
#define XN_ASCR_STOP            6
#define XN_ASCR_GOTO_RANDOM     7           /* (percent, word target) */
#define XN_ASCR_GOTO_IF_EVENTS  8           /* (word bits, word target) */
#define XN_ASCR_SET_MIRROR      9           /* (byte) */
#define XN_ASCR_SET_BYTE13      10          /* (byte) */
#define XN_ASCR_SET_RECORD      11          /* (byte) */

/* monster_init: xn_anim_rand_state from the BIOS tick count. */
void xn_anim_rand_seed(void);

/* The state stopped: no position, frame 0, no events, no wait, facing and record group 0,
   opcode 10's byte FFh (the request, the speed and the script stay). Returns 0. */
s32 xn_anim_reset(struct xn_anim *a);

/* Dead: xn_anim_reset, then the speed. Q-ANIM-01: the asm stores the speed through the
   reset's result, 0: the word goes to linear 0Ch (the real-mode int 3 vector), not to
   a->tick_divisor. Returns 0. */
s32 xn_anim_reset_with_rate(struct xn_anim *a, u16 tick_divisor);

/* object_draw_cb, per creature: starts the requested state when the script allows it
   (stopped, waiting 1..127 steps, or at a frame-wait or goto opcode): its entry from the
   record's state table (8000h: state 0's); then steps it until it is on the clock's step. */
void xn_anim_update(struct xn_anim *a);

/* One step when xn_anim_ticks / tick_divisor has changed since the last: a wait counted down
   (FFh holds), or the script run to its next yield. Returns 1 when it stepped, 0 when stopped
   or on the same step. A tick_divisor of 0 makes the step 0 (Q-SYS-01). */
s32 xn_anim_tick(struct xn_anim *a);

/* Runs the script from *pos to its next yield: a frame byte b (the frame becomes -(s8)b - 1),
   or an opcode that yields. */
void xn_anim_run_opcodes(struct xn_anim *a, xn_ascr_pos *pos);

/* The opcodes: 1 go on, 0 yield */

/* 0 (lo, hi, count): the loop's count, random in lo..hi (lo when equal), into the script. */
s32 xn_anim_op_loop_start(struct xn_anim *a, xn_ascr_pos *pos);
/* 1 (word start): counts down the count of the loop_start at start; back to its body while
   it is not 0. */
s32 xn_anim_op_loop_end(struct xn_anim *a, xn_ascr_pos *pos);
/* 2 (word bits): events |= bits (1 strike, 2 missile: the game reads and clears them). */
s32 xn_anim_op_set_events(struct xn_anim *a, xn_ascr_pos *pos);
/* 10 (byte): opcode10_byte (nothing reads it). */
s32 xn_anim_op_set_byte13(struct xn_anim *a, xn_ascr_pos *pos);
/* 9 (byte): the mirrored bit (events bit 15) set when the byte is not 0, else cleared. */
s32 xn_anim_op_set_mirror(struct xn_anim *a, xn_ascr_pos *pos);
/* 5: back to state 0's script. Q-ANIM-02: the position becomes the offset itself, not the
   record plus it (dead script data: a stopped script, or a wild one). */
s32 xn_anim_op_restart(struct xn_anim *a, xn_ascr_pos *pos);
/* 6: stops the script (position 0) and yields. */
s32 xn_anim_op_stop(struct xn_anim *a, xn_ascr_pos *pos);
/* 3 (frame, steps): shows frame for that many steps (0: hold, FFh) and yields. */
s32 xn_anim_op_frame_wait(struct xn_anim *a, xn_ascr_pos *pos);
/* 4 (word target): goto. */
s32 xn_anim_op_goto(struct xn_anim *a, xn_ascr_pos *pos);
/* 7 (percent, word target): goto when a random 0..100 is at most percent. */
s32 xn_anim_op_goto_random(struct xn_anim *a, xn_ascr_pos *pos);
/* 8 (word bits, word target): goto when any of the bits is in events. */
s32 xn_anim_op_goto_if_events(struct xn_anim *a, xn_ascr_pos *pos);
/* 11 (byte): the record group (move, attack, idle...). Q-ANIM-03: it goes on unless adding
   2 to the position wraps (the asm returns the add's carry as its yield flag). */
s32 xn_anim_op_set_record(struct xn_anim *a, xn_ascr_pos *pos);

/* A random lo + (r & 7Fh) % (hi - lo + 1), lo the low byte of range and hi the high (8-bit
   arithmetic). Q-ANIM-04: hi = lo - 1 divides by zero: 0. */
u16 xn_anim_rand_range(u16 range);

/* The animation LCG: state = state * 1509 + 41 (16 bits); returns it. */
u16 xn_anim_rand_next(void);

#endif
