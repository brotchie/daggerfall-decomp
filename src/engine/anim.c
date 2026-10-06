/* anim.c: XnGine's ASCR animation scripts (canonical C; the interface and the module's
   documentation are in xanim.h). */
#include "xanim.h"
#include "xpc.h"

#define STATE_NONE_ENTRY 0x8000             /* a state table entry: none (use state 0's) */

/* The script's word at pos + k, and the position at an offset from the record's start */
static u16 word_at(xn_ascr_pos pos, s32 k)
{
    return *(const u16 *)(pos + k);
}

static xn_ascr_pos script_at(const struct xn_anim *a, u32 offset)
{
    return a->script + offset;
}

void xn_anim_rand_seed(void)
{
    xn_anim_rand_state = (u16)XN_BIOS_TICKS;
}

s32 xn_anim_reset(struct xn_anim *a)
{
    a->opcode10_byte = 0xFF;
    a->frame_copy = 0;
    a->frame = 0;
    a->events = 0;
    a->wait = 0;
    a->facing = 0;
    a->record_group = 0;
    a->pos = 0;
    return 0;
}

s32 xn_anim_reset_with_rate(struct xn_anim *a, u16 tick_divisor)
{
    xn_anim_reset(a);
    *(volatile u16 *)0x0C = tick_divisor;   /* Quirk Q-ANIM-01: through the reset's 0 */
    return 0;
}

/* Whether a requested state may start now: stopped, waiting (1..127 steps), or at a
   frame-wait or goto opcode */
static int may_start(const struct xn_anim *a)
{
    if (a->pos == 0 || (s8)a->wait > 0)
        return 1;
    return *a->pos == XN_ASCR_FRAME_WAIT || *a->pos == XN_ASCR_GOTO;
}

void xn_anim_update(struct xn_anim *a)
{
    const struct xn_ascr *ascr;
    u32 entry;

    if (a->request != XN_ANIM_NONE && may_start(a)) {
        a->wait = 0;
        a->state = a->request;
        ascr = (const struct xn_ascr *)a->script;
        entry = ascr->state_entry[a->request];
        if (entry == STATE_NONE_ENTRY)
            entry = ascr->state_entry[0];
        a->pos = a->script + entry;
        a->request = XN_ANIM_NONE;
    }
    while (xn_anim_tick(a))
        ;
}

s32 xn_anim_tick(struct xn_anim *a)
{
    u16 step;
    xn_ascr_pos pos;

    if (a->pos == 0)
        return 0;
    /* Quirk Q-SYS-01: a speed of 0 divides by zero: step 0 */
    step = (u16)(a->tick_divisor != 0 ? xn_anim_ticks / a->tick_divisor : 0);
    if (step == a->last_step)
        return 0;
    a->last_step = step;
    if (a->wait == XN_ANIM_NONE)            /* holding */
        return 1;
    if (a->wait != 0) {
        a->wait--;
        return 1;
    }
    pos = a->pos;
    xn_anim_run_opcodes(a, &pos);
    a->pos = pos;
    return 1;
}

/* Runs opcode op at *pos: 1 go on, 0 yield */
static s32 run_opcode(struct xn_anim *a, xn_ascr_pos *pos, s32 op)
{
    switch (op) {
    case XN_ASCR_LOOP_START: return xn_anim_op_loop_start(a, pos);
    case XN_ASCR_LOOP_END: return xn_anim_op_loop_end(a, pos);
    case XN_ASCR_SET_EVENTS: return xn_anim_op_set_events(a, pos);
    case XN_ASCR_FRAME_WAIT: return xn_anim_op_frame_wait(a, pos);
    case XN_ASCR_GOTO: return xn_anim_op_goto(a, pos);
    case XN_ASCR_RESTART: return xn_anim_op_restart(a, pos);
    case XN_ASCR_STOP: return xn_anim_op_stop(a, pos);
    case XN_ASCR_GOTO_RANDOM: return xn_anim_op_goto_random(a, pos);
    case XN_ASCR_GOTO_IF_EVENTS: return xn_anim_op_goto_if_events(a, pos);
    case XN_ASCR_SET_MIRROR: return xn_anim_op_set_mirror(a, pos);
    case XN_ASCR_SET_BYTE13: return xn_anim_op_set_byte13(a, pos);
    case XN_ASCR_SET_RECORD: return xn_anim_op_set_record(a, pos);
    }
    /* Quirk Q-ANIM-05 (dropped): the asm calls what its table holds past its 12 handlers (two
       zeros, then other data) and crashes; canonical C stops the script */
    return xn_anim_op_stop(a, pos);
}

void xn_anim_run_opcodes(struct xn_anim *a, xn_ascr_pos *pos)
{
    s8 b;

    for (;;) {
        b = (s8)**pos;
        if (b < 0) {                        /* a frame */
            (*pos)++;
            a->frame_copy = a->frame = (u16)(-(s16)b - 1);
            return;
        }
        if (!run_opcode(a, pos, b))
            return;
    }
}

#pragma off (unreferenced)                  /* the opcodes share one signature */

s32 xn_anim_op_loop_start(struct xn_anim *a, xn_ascr_pos *pos)
{
    u8 lo = (*pos)[1], hi = (*pos)[2];
    u8 count = lo;

    if (lo != hi)
        count = (u8)xn_anim_rand_range(lo | hi << 8);
    (*pos)[3] = count;                      /* the loop's count lives in the script */
    *pos += 4;
    return 1;
}

s32 xn_anim_op_loop_end(struct xn_anim *a, xn_ascr_pos *pos)
{
    xn_ascr_pos start = script_at(a, word_at(*pos, 1));

    if (--start[3] != 0)
        *pos = start + 4;                   /* the body again */
    else
        *pos += 3;
    return 1;
}

s32 xn_anim_op_set_events(struct xn_anim *a, xn_ascr_pos *pos)
{
    a->events |= word_at(*pos, 1);
    *pos += 3;
    return 1;
}

s32 xn_anim_op_set_byte13(struct xn_anim *a, xn_ascr_pos *pos)
{
    a->opcode10_byte = (*pos)[1];
    *pos += 2;
    return 1;
}

s32 xn_anim_op_set_mirror(struct xn_anim *a, xn_ascr_pos *pos)
{
    a->events &= 0x7FFF;
    if ((*pos)[1] != 0)
        a->events |= 0x8000;
    *pos += 2;
    return 1;
}

s32 xn_anim_op_restart(struct xn_anim *a, xn_ascr_pos *pos)
{
    u32 offset = ((const struct xn_ascr *)a->script)->restart;

    a->pos = (u8 *)offset;                  /* Quirk Q-ANIM-02: the bare offset */
    *pos = (xn_ascr_pos)offset;
    return 1;
}

s32 xn_anim_op_stop(struct xn_anim *a, xn_ascr_pos *pos)
{
    *pos = 0;
    return 0;
}

s32 xn_anim_op_frame_wait(struct xn_anim *a, xn_ascr_pos *pos)
{
    u8 steps = (*pos)[2];

    a->frame_copy = a->frame = (*pos)[1];
    *pos += 3;
    a->wait = steps != 0 ? steps : XN_ANIM_NONE;
    return 0;
}

s32 xn_anim_op_goto(struct xn_anim *a, xn_ascr_pos *pos)
{
    *pos = script_at(a, word_at(*pos, 1));
    return 1;
}

s32 xn_anim_op_goto_random(struct xn_anim *a, xn_ascr_pos *pos)
{
    if ((u8)xn_anim_rand_range(100 << 8) <= (*pos)[1])     /* 0..100 */
        *pos = script_at(a, word_at(*pos, 2));
    else
        *pos += 4;
    return 1;
}

s32 xn_anim_op_goto_if_events(struct xn_anim *a, xn_ascr_pos *pos)
{
    if (a->events & word_at(*pos, 1))
        *pos = script_at(a, word_at(*pos, 3));
    else
        *pos += 5;
    return 1;
}

s32 xn_anim_op_set_record(struct xn_anim *a, xn_ascr_pos *pos)
{
    xn_ascr_pos next = *pos + 2;
    s32 wrapped = next < *pos;              /* Quirk Q-ANIM-03: the add's carry */

    a->record_group = (*pos)[1];
    *pos = next;
    return !wrapped;
}

#pragma on (unreferenced)

u16 xn_anim_rand_range(u16 range)
{
    u8 lo = (u8)range, hi = (u8)(range >> 8);
    u8 span = hi - lo + 1;
    u8 r = (u8)(xn_anim_rand_next() & 0x7F);

    if (span == 0)
        return 0;                           /* Quirk Q-ANIM-04 (Q-SYS-01) */
    return (u8)(lo + r % span);
}

u16 xn_anim_rand_next(void)
{
    xn_anim_rand_state = (u16)(xn_anim_rand_state * 1509 + 41);
    return xn_anim_rand_state;
}
