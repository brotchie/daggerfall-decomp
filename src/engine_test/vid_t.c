/* vid_t.c: test shims of src/engine/vid.c (built only by tools/xn_rc.py; see
   src/engine_test/vec_t.c and docs/xngine_canonical.md). */
#include "xvid.h"

/* path EAX, x EDX, y EBX, skippable ECX -> EAX 1 when played */
void xn_vid_play_r(xn_regs *r)
{
    r->eax = xn_vid_play((const char *)r->eax, r->edx, r->ebx, r->ecx);
}

/* x EAX, y EDX, path EBX */
void xn_vid_open_r(xn_regs *r)
{
    xn_vid_open(r->eax, r->edx, (const char *)r->ebx);
}

void xn_vid_update_r(xn_regs *r)
{
    xn_vid_update();
}

void xn_vid_finish_r(xn_regs *r)
{
    xn_vid_finish();
}

void xn_vid_decode_chunk_r(xn_regs *r)
{
    xn_vid_decode_chunk();
}

void xn_vid_close_file_r(xn_regs *r)
{
    xn_vid_close_file();
}

/* p in and out in ESI */
void xn_vid_refill_r(xn_regs *r)
{
    r->esi = (u32)xn_vid_refill((u8 *)r->esi);
}

void xn_vid_fill_r(xn_regs *r)
{
    xn_vid_fill();
}

/* delay AX */
void xn_vid_schedule_frame_r(xn_regs *r)
{
    xn_vid_schedule_frame((u16)r->eax);
}

/* p in ESI */
void xn_vid_read_audio_r(xn_regs *r)
{
    xn_vid_read_audio((u8 *)r->esi);
}

void xn_vid_timer_cb_r(xn_regs *r)
{
    xn_vid_timer_cb();
}

/* the sample on the stack (the caller pops it) */
void xn_vid_audio_done_cb_r(xn_regs *r)
{
    xn_vid_audio_done_cb((xn_vid_sos_sample *)XN_STACK_ARG(r, 0));
}

void xn_vid_audio_stop_r(xn_regs *r)
{
    xn_vid_audio_stop();
}

void xn_vid_audio_start_r(xn_regs *r)
{
    xn_vid_audio_start();
}
