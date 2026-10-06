/* mouse_t.c: test shims of src/engine/mouse.c (built only by tools/xn_rc.py; docs/
   xngine_canonical.md). The asm's poll, motion and position calls also left FS = DS for
   their callers, which none reads (config/xngine_dropped.csv). */
#include "xmouse.h"

void xn_mouse_set_range_320x200_r(xn_regs *r)
{
    xn_mouse_set_range_320x200();
}

void xn_mouse_set_range_640x480_r(xn_regs *r)
{
    xn_mouse_set_range_640x480();
}

void xn_mouse_set_sensitivity_r(xn_regs *r)
{
    xn_mouse_set_sensitivity(r->eax, r->edx);
}

void xn_mouse_get_sensitivity_r(xn_regs *r)
{
    xn_mouse_get_sensitivity((u8 *)r->eax, (u8 *)r->edx);
}

/* rate AL */
void xn_mouse_ps2_set_sample_rate_r(xn_regs *r)
{
    xn_mouse_ps2_set_sample_rate((u8)r->eax);
}

void xn_mouse_init_r(xn_regs *r)
{
    xn_mouse_init();
}

void xn_mouse_poll_clamped_r(xn_regs *r)
{
    xn_mouse_poll_clamped();
}

void xn_mouse_poll_r(xn_regs *r)
{
    xn_mouse_poll();
}

/* x AX, y DX */
void xn_mouse_cursor_move_r(xn_regs *r)
{
    xn_mouse_cursor_move((s16)r->eax, (s16)r->edx);
}

void xn_mouse_cursor_erase_r(xn_regs *r)
{
    xn_mouse_cursor_erase();
}

void xn_mouse_cursor_clip_r(xn_regs *r)
{
    xn_mouse_cursor_clip();
}

void xn_mouse_cursor_draw_r(xn_regs *r)
{
    xn_mouse_cursor_draw();
}

/* image EAX, hotspot DX, BX */
void xn_mouse_set_cursor_image_r(xn_regs *r)
{
    xn_mouse_set_cursor_image((const u8 *)r->eax, (s16)r->edx, (s16)r->ebx);
}

void xn_mouse_read_motion_r(xn_regs *r)
{
    xn_mouse_read_motion((s16 *)r->eax, (s16 *)r->edx);
}

void xn_mouse_set_position_r(xn_regs *r)
{
    xn_mouse_set_position(r->eax, r->edx);
}
