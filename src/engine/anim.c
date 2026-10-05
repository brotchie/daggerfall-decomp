/* anim.c: XnGine's ASCR animation scripts as readable C (xanim.h; see xngine.h). */
#include "xanim.h"
#include "xtimer.h"

#define ANIM_NONE   0xFF                /* request: none; wait: hold */

/* The script's word at pos + k, and the script position at an offset from its start */
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
    s32 zero = xn_anim_reset(a);

    ((struct xn_anim *)zero)->tick_divisor = tick_divisor;     /* (sic: linear 0Ch) */
    return zero;
}

/* Whether a requested state may start now: stopped, waiting (1..127 steps), or at a
   frame-wait or goto opcode */
static int anim_may_start(const struct xn_anim *a)
{
    if (a->pos == 0 || (s8)a->wait > 0)
        return 1;
    return *a->pos == 3 || *a->pos == 4;
}

void xn_anim_update(struct xn_anim *a)
{
    const struct xn_ascr *ascr;
    u32 entry;

    if (a->request != ANIM_NONE && anim_may_start(a)) {
        a->wait = 0;
        a->state = a->request;
        ascr = (const struct xn_ascr *)a->script;
        entry = ascr->state_entry[a->request];
        if (entry == 0x8000)            /* none: state 0's */
            entry = ascr->state_entry[0];
        a->pos = a->script + entry;
        a->request = ANIM_NONE;
    }
    while (xn_anim_tick(a))
        ;
}

s32 xn_anim_tick(struct xn_anim *a)
{
    xn_s64 ticks;
    u16 step;
    xn_ascr_pos pos;

    if (a->pos == 0)
        return 0;
    ticks.lo = xn_anim_ticks;
    ticks.hi = 0;
    step = (u16)xn_u64_div(&ticks, a->tick_divisor);
    if (step == a->last_step)
        return 0;
    a->last_step = step;
    if (a->wait == ANIM_NONE)           /* holding */
        return 1;
    if (a->wait != 0) {
        a->wait--;
        return 1;
    }
    pos = a->pos;
    xn_anim_run_opcodes(a, &pos);
    a->pos = (u8 *)pos;
    return 1;
}

void xn_anim_tick_r(xn_regs *r)
{
    XN_SETFLAG(r, XN_CF, !xn_anim_tick((struct xn_anim *)r->eax));
}

/* An opcode the table has no handler for: the asm calls whatever the table holds there */
static s32 anim_op_other(struct xn_anim *a, xn_ascr_pos *pos, s32 op)
{
    xn_regs r;

    r.eax = op;
    r.esi = (u32)a;
    r.edi = (u32)*pos;
    xn_asmcall(xn_anim_opcodes[op], &r);
    *pos = (xn_ascr_pos)r.edi;
    return (r.eflags & XN_CF) == 0;
}

/* Runs opcode op at *pos: 1 go on, 0 yield */
static s32 anim_op(struct xn_anim *a, xn_ascr_pos *pos, s32 op)
{
    switch (op) {
    case 0: return xn_anim_op_loop_start(a, pos);
    case 1: return xn_anim_op_loop_end(a, pos);
    case 2: return xn_anim_op_set_events(a, pos);
    case 3: return xn_anim_op_frame_wait(a, pos);
    case 4: return xn_anim_op_goto(a, pos);
    case 5: return xn_anim_op_restart(a, pos);
    case 6: return xn_anim_op_stop(a, pos);
    case 7: return xn_anim_op_goto_random(a, pos);
    case 8: return xn_anim_op_goto_if_events(a, pos);
    case 9: return xn_anim_op_set_mirror(a, pos);
    case 10: return xn_anim_op_set_byte13(a, pos);
    case 11: return xn_anim_op_set_record(a, pos);
    }
    return anim_op_other(a, pos, op);
}

void xn_anim_run_opcodes(struct xn_anim *a, xn_ascr_pos *pos)
{
    s8 b;

    for (;;) {
        b = (s8)**pos;
        if (b < 0) {                    /* a frame */
            (*pos)++;
            a->frame_copy = a->frame = (u16)(-(s16)b - 1);
            return;
        }
        if (!anim_op(a, pos, b))
            return;
    }
}

void xn_anim_run_opcodes_r(xn_regs *r)
{
    xn_ascr_pos pos = (xn_ascr_pos)r->edi;

    xn_anim_run_opcodes((struct xn_anim *)r->esi, &pos);
    r->edi = (u32)pos;
}

/* the opcodes' asm interface: ESI the state, EDI the position, CF set to yield */
static void op_glue(xn_regs *r, s32 (*op)(struct xn_anim *, xn_ascr_pos *))
{
    xn_ascr_pos pos = (xn_ascr_pos)r->edi;

    XN_SETFLAG(r, XN_CF, !op((struct xn_anim *)r->esi, &pos));
    r->edi = (u32)pos;
}

#pragma off (unreferenced)          /* the opcodes share one signature; some use no state */

s32 xn_anim_op_loop_start(struct xn_anim *a, xn_ascr_pos *pos)
{
    u8 lo = (*pos)[1], hi = (*pos)[2];
    u8 count = lo;

    if (lo != hi)
        count = (u8)xn_anim_rand_range(lo | hi << 8);
    ((u8 *)*pos)[3] = count;            /* the loop's count lives in the script */
    *pos += 4;
    return 1;
}

void xn_anim_op_loop_start_r(xn_regs *r)
{
    op_glue(r, xn_anim_op_loop_start);
}

s32 xn_anim_op_loop_end(struct xn_anim *a, xn_ascr_pos *pos)
{
    u8 *start = (u8 *)script_at(a, word_at(*pos, 1));

    if (--start[3] != 0)
        *pos = start + 4;               /* the body again */
    else
        *pos += 3;
    return 1;
}

void xn_anim_op_loop_end_r(xn_regs *r)
{
    op_glue(r, xn_anim_op_loop_end);
}

s32 xn_anim_op_set_events(struct xn_anim *a, xn_ascr_pos *pos)
{
    a->events |= word_at(*pos, 1);
    *pos += 3;
    return 1;
}

void xn_anim_op_set_events_r(xn_regs *r)
{
    op_glue(r, xn_anim_op_set_events);
}

s32 xn_anim_op_set_byte13(struct xn_anim *a, xn_ascr_pos *pos)
{
    a->opcode10_byte = (*pos)[1];
    *pos += 2;
    return 1;
}

void xn_anim_op_set_byte13_r(xn_regs *r)
{
    op_glue(r, xn_anim_op_set_byte13);
}

s32 xn_anim_op_set_mirror(struct xn_anim *a, xn_ascr_pos *pos)
{
    a->events &= 0x7FFF;
    if ((*pos)[1] != 0)
        a->events |= 0x8000;
    *pos += 2;
    return 1;
}

void xn_anim_op_set_mirror_r(xn_regs *r)
{
    op_glue(r, xn_anim_op_set_mirror);
}

s32 xn_anim_op_restart(struct xn_anim *a, xn_ascr_pos *pos)
{
    u32 offset = ((const struct xn_ascr *)a->script)->restart;

    a->pos = (u8 *)offset;              /* (sic: the offset, not script + offset) */
    *pos = (xn_ascr_pos)offset;
    return 1;
}

void xn_anim_op_restart_r(xn_regs *r)
{
    op_glue(r, xn_anim_op_restart);
}

s32 xn_anim_op_stop(struct xn_anim *a, xn_ascr_pos *pos)
{
    *pos = 0;
    return 0;
}

void xn_anim_op_stop_r(xn_regs *r)
{
    op_glue(r, xn_anim_op_stop);
}

s32 xn_anim_op_frame_wait(struct xn_anim *a, xn_ascr_pos *pos)
{
    u8 steps = (*pos)[2];

    a->frame_copy = a->frame = (*pos)[1];
    *pos += 3;
    a->wait = steps != 0 ? steps : ANIM_NONE;
    return 0;
}

void xn_anim_op_frame_wait_r(xn_regs *r)
{
    op_glue(r, xn_anim_op_frame_wait);
}

s32 xn_anim_op_goto(struct xn_anim *a, xn_ascr_pos *pos)
{
    *pos = script_at(a, word_at(*pos, 1));
    return 1;
}

void xn_anim_op_goto_r(xn_regs *r)
{
    op_glue(r, xn_anim_op_goto);
}

s32 xn_anim_op_goto_random(struct xn_anim *a, xn_ascr_pos *pos)
{
    if ((u8)xn_anim_rand_range(100 << 8) <= (*pos)[1])  /* 0..100 */
        *pos = script_at(a, word_at(*pos, 2));
    else
        *pos += 4;
    return 1;
}

void xn_anim_op_goto_random_r(xn_regs *r)
{
    op_glue(r, xn_anim_op_goto_random);
}

s32 xn_anim_op_goto_if_events(struct xn_anim *a, xn_ascr_pos *pos)
{
    if (a->events & word_at(*pos, 1))
        *pos = script_at(a, word_at(*pos, 3));
    else
        *pos += 5;
    return 1;
}

void xn_anim_op_goto_if_events_r(xn_regs *r)
{
    op_glue(r, xn_anim_op_goto_if_events);
}

s32 xn_anim_op_set_record(struct xn_anim *a, xn_ascr_pos *pos)
{
    xn_ascr_pos next = *pos + 2;
    s32 wrapped = next < *pos;

    a->record_group = (*pos)[1];
    *pos = next;
    return !wrapped;
}

void xn_anim_op_set_record_r(xn_regs *r)
{
    op_glue(r, xn_anim_op_set_record);
}

#pragma on (unreferenced)

u16 xn_anim_rand_range(u16 range)
{
    u8 lo = (u8)range, hi = (u8)(range >> 8);
    u8 span = hi - lo + 1;
    u16 qr = xn_divb(xn_anim_rand_next() & 0x7F, span);    /* AH: the remainder */

    if (span == 0)
        return 0;                       /* the divide error (see xanim.h) */
    return (u8)(lo + (qr >> 8));
}

u16 xn_anim_rand_next(void)
{
    xn_anim_rand_state = (u16)(xn_anim_rand_state * 1509 + 41);
    return xn_anim_rand_state;
}
