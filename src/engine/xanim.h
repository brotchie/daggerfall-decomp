/* xanim.h: XnGine's ASCR animation scripts (src/engine/anim.c; see xngine.h): the creature
   animation state (struct xn_anim, xnstruct.h), stepped once per change of
   xn_anim_ticks / tick_divisor by a small interpreter: a script byte from 80h up is a frame
   (and yields), 0..11 are opcodes with operands (loops, gotos, random gotos, events, waits).

   Each declaration keeps the function's asm interface (config/xngine_abi.csv): no pragma is
   Watcom's own convention; a pragma names the registers; NAME_r is the glue for a function
   whose asm callers read several registers or flags. The opcodes' asm interface: ESI the
   state, EDI the script position (advanced), CF set to yield. */
#ifndef XANIM_H
#define XANIM_H

#include "xngine.h"
#include "xnstruct.h"
#include "xsysutil.h"

extern u16 xn_anim_rand_state;              /* the animation LCG's state (0xC0030) */
extern u32 xn_anim_ticks;                   /* the engine's animation clock (0x1343C0) */
/* The opcode handlers' asm entries, by opcode (0xC0000; 12 of them, then two zeros) */
extern void (*xn_anim_opcodes[14])(void);

/* A script position, and the opcodes' result: 1 go on, 0 yield (the asm's CF) */
typedef const u8 *xn_ascr_pos;

/* xn_anim_rand_state from the BIOS tick count. */
void xn_anim_rand_seed(void);

/* The state stopped: no position, frame 0, no events, no wait, facing and record group 0,
   opcode 10's byte FFh (the request, the speed and the script stay). Returns 0 (the asm
   leaves EAX so). */
s32 xn_anim_reset(struct xn_anim *a);

/* Dead: xn_anim_reset, then the speed. Kept from the asm: it stores the speed through EAX
   after the reset, which leaves EAX = 0: the word goes to linear 0Ch (the real-mode int 3
   vector), not to a->tick_divisor. Returns 0. */
s32 xn_anim_reset_with_rate(struct xn_anim *a, u16 tick_divisor);
#pragma aux xn_anim_reset_with_rate parm [eax] [edx] value [eax] modify exact [eax];

/* object_draw_cb, per creature: starts the requested state when the script allows it
   (stopped, waiting, or at a frame-wait or goto opcode), then steps it. */
void xn_anim_update(struct xn_anim *a);
#pragma aux xn_anim_update parm [eax] modify exact [eax ecx esi];

/* One step when xn_anim_ticks / tick_divisor has changed since the last: a wait counted
   down, or the script run to its next yield. Returns 1 when it stepped, 0 when stopped or
   on the same step (the asm's CF). A tick_divisor of 0 divides by zero: the handler's 0. */
s32 xn_anim_tick(struct xn_anim *a);
void xn_anim_tick_r(xn_regs *r);

/* Runs the script from *pos to its next yield: a frame byte b (the frame becomes -(s8)b - 1),
   or an opcode that yields. */
void xn_anim_run_opcodes(struct xn_anim *a, xn_ascr_pos *pos);
void xn_anim_run_opcodes_r(xn_regs *r);

/* The opcodes (operands after the opcode byte; offsets from the script's start) */

/* 0 (lo, hi, count): a loop's count, random in lo..hi (lo when equal), into the script. */
s32 xn_anim_op_loop_start(struct xn_anim *a, xn_ascr_pos *pos);
void xn_anim_op_loop_start_r(xn_regs *r);
/* 1 (word start): counts down the count of the loop_start at start; back to its body while
   it is not 0. */
s32 xn_anim_op_loop_end(struct xn_anim *a, xn_ascr_pos *pos);
void xn_anim_op_loop_end_r(xn_regs *r);
/* 2 (word bits): events |= bits (1 strike, 2 missile: the game reads and clears them). */
s32 xn_anim_op_set_events(struct xn_anim *a, xn_ascr_pos *pos);
void xn_anim_op_set_events_r(xn_regs *r);
/* 10 (byte): opcode10_byte (nothing reads it). */
s32 xn_anim_op_set_byte13(struct xn_anim *a, xn_ascr_pos *pos);
void xn_anim_op_set_byte13_r(xn_regs *r);
/* 9 (byte): the mirrored bit (events bit 15) set when the byte is not 0, else cleared. */
s32 xn_anim_op_set_mirror(struct xn_anim *a, xn_ascr_pos *pos);
void xn_anim_op_set_mirror_r(xn_regs *r);
/* 5: back to state 0's script. Kept from the asm (a bug of dead script data): the position
   becomes the offset itself, not the script plus it. */
s32 xn_anim_op_restart(struct xn_anim *a, xn_ascr_pos *pos);
void xn_anim_op_restart_r(xn_regs *r);
/* 6: stops the script (position 0) and yields. */
s32 xn_anim_op_stop(struct xn_anim *a, xn_ascr_pos *pos);
void xn_anim_op_stop_r(xn_regs *r);
/* 3 (frame, steps): shows frame for that many steps (0: hold, FFh) and yields. */
s32 xn_anim_op_frame_wait(struct xn_anim *a, xn_ascr_pos *pos);
void xn_anim_op_frame_wait_r(xn_regs *r);
/* 4 (word target): goto. */
s32 xn_anim_op_goto(struct xn_anim *a, xn_ascr_pos *pos);
void xn_anim_op_goto_r(xn_regs *r);
/* 7 (percent, word target): goto when a random 0..100 is at most percent. */
s32 xn_anim_op_goto_random(struct xn_anim *a, xn_ascr_pos *pos);
void xn_anim_op_goto_random_r(xn_regs *r);
/* 8 (word bits, word target): goto when any of the bits is in events. */
s32 xn_anim_op_goto_if_events(struct xn_anim *a, xn_ascr_pos *pos);
void xn_anim_op_goto_if_events_r(xn_regs *r);
/* 11 (byte): the record group (move, attack, idle...). Kept from the asm: it does not clear
   CF, which is the carry of the position's add: it goes on unless the pointer wraps. */
s32 xn_anim_op_set_record(struct xn_anim *a, xn_ascr_pos *pos);
void xn_anim_op_set_record_r(xn_regs *r);

/* A random lo + (r & 7Fh) % (hi - lo + 1): lo in the low byte of range, hi in the high
   (8-bit arithmetic); hi = lo - 1 divides by zero, and the handler's EDX = 0 takes lo with
   it: 0. */
u16 xn_anim_rand_range(u16 range);
#pragma aux xn_anim_rand_range parm [eax] value [ax] modify exact [eax];

/* The animation LCG: state = state * 1509 + 41 (16-bit); returns it. */
u16 xn_anim_rand_next(void);
#pragma aux xn_anim_rand_next parm [] value [ax] modify exact [eax];

#endif
