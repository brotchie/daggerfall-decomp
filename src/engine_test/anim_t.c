/* anim_t.c: test shims of src/engine/anim.c (built only by tools/xn_rc.py; docs/
   xngine_canonical.md). The opcodes' asm entries take the state in ESI and the position in
   EDI (moved on), and set CF to yield. */
#include "xanim.h"

void xn_anim_rand_seed_r(xn_regs *r)
{
    xn_anim_rand_seed();
}

void xn_anim_reset_r(xn_regs *r)
{
    r->eax = xn_anim_reset((struct xn_anim *)r->eax);
}

/* a EAX, speed DX -> EAX */
void xn_anim_reset_with_rate_r(xn_regs *r)
{
    r->eax = xn_anim_reset_with_rate((struct xn_anim *)r->eax, (u16)r->edx);
}

void xn_anim_update_r(xn_regs *r)
{
    xn_anim_update((struct xn_anim *)r->eax);
}

/* a EAX -> CF when it did not step */
void xn_anim_tick_r(xn_regs *r)
{
    XN_SETFLAG(r, XN_CF, !xn_anim_tick((struct xn_anim *)r->eax));
}

/* a ESI, position EDI -> EDI */
void xn_anim_run_opcodes_r(xn_regs *r)
{
    xn_ascr_pos pos = (xn_ascr_pos)r->edi;

    xn_anim_run_opcodes((struct xn_anim *)r->esi, &pos);
    r->edi = (u32)pos;
}

/* an opcode's asm entry: a ESI, position EDI -> EDI, CF set to yield */
static void op(xn_regs *r, s32 (*fn)(struct xn_anim *, xn_ascr_pos *))
{
    xn_ascr_pos pos = (xn_ascr_pos)r->edi;

    XN_SETFLAG(r, XN_CF, !fn((struct xn_anim *)r->esi, &pos));
    r->edi = (u32)pos;
}

void xn_anim_op_loop_start_r(xn_regs *r) { op(r, xn_anim_op_loop_start); }
void xn_anim_op_loop_end_r(xn_regs *r) { op(r, xn_anim_op_loop_end); }
void xn_anim_op_set_events_r(xn_regs *r) { op(r, xn_anim_op_set_events); }
void xn_anim_op_set_byte13_r(xn_regs *r) { op(r, xn_anim_op_set_byte13); }
void xn_anim_op_set_mirror_r(xn_regs *r) { op(r, xn_anim_op_set_mirror); }
void xn_anim_op_restart_r(xn_regs *r) { op(r, xn_anim_op_restart); }
void xn_anim_op_stop_r(xn_regs *r) { op(r, xn_anim_op_stop); }
void xn_anim_op_frame_wait_r(xn_regs *r) { op(r, xn_anim_op_frame_wait); }
void xn_anim_op_goto_r(xn_regs *r) { op(r, xn_anim_op_goto); }
void xn_anim_op_goto_random_r(xn_regs *r) { op(r, xn_anim_op_goto_random); }
void xn_anim_op_goto_if_events_r(xn_regs *r) { op(r, xn_anim_op_goto_if_events); }
void xn_anim_op_set_record_r(xn_regs *r) { op(r, xn_anim_op_set_record); }

/* range AX -> AX (DX kept) */
void xn_anim_rand_range_r(xn_regs *r)
{
    r->eax = (r->eax & 0xFFFF0000) | xn_anim_rand_range((u16)r->eax);
}

void xn_anim_rand_next_r(xn_regs *r)
{
    r->eax = (r->eax & 0xFFFF0000) | xn_anim_rand_next();
}
