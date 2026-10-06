/* helmet_t.c: test shims of src/engine/helmet.c (built only by tools/xn_rc.py; docs/
   xngine_canonical.md). The drivers' asm entries report success in CF (set: failed) and
   their angles in EAX, EDX, EBX. */
#include "xhelmet.h"
#include "xserial.h"

/* port EAX, driver EDX -> CF when it failed */
void xn_helmet_open_r(xn_regs *r)
{
    XN_SETFLAG(r, XN_CF, !xn_helmet_open(r->eax, r->edx));
}

void xn_helmet_close_r(xn_regs *r)
{
    xn_helmet_close();
}

void xn_helmet_request_r(xn_regs *r)
{
    xn_helmet_request();
}

void xn_helmet_poll_r(xn_regs *r)
{
    xn_helmet_poll();
}

/* the sample in EAX, EDX, EBX, and back */
void xn_helmet_smooth_r(xn_regs *r)
{
    xn_vec3 s;

    s.x = r->eax;
    s.y = r->edx;
    s.z = r->ebx;
    xn_helmet_smooth(&s);
    r->eax = s.x;
    r->edx = s.y;
    r->ebx = s.z;
}

/* ---- driver A ---- */

void xn_helmet_a_open_r(xn_regs *r)
{
    XN_SETFLAG(r, XN_CF, !xn_helmet_a_open(r->eax));
}

void xn_helmet_a_close_r(xn_regs *r)
{
    xn_helmet_a_close();
}

void xn_helmet_a_reset_r(xn_regs *r)
{
    XN_SETFLAG(r, XN_CF, !xn_helmet_a_reset());
}

void xn_helmet_a_send_h_r(xn_regs *r)
{
    XN_SETFLAG(r, XN_CF, !xn_helmet_a_send_h());
}

/* -> EAX and CF: 0 sent, 1 not */
void xn_helmet_a_request_r(xn_regs *r)
{
    r->eax = !xn_helmet_a_request();
    XN_SETFLAG(r, XN_CF, r->eax);
}

void xn_helmet_a_read_text_r(xn_regs *r)
{
    r->eax = xn_helmet_a_read_text();
}

/* -> EAX 0 or -1; CF: the asm's last compare (after a timeout, the count below 6) */
void xn_helmet_a_read_r(xn_regs *r)
{
    r->eax = xn_helmet_a_read();
    XN_SETFLAG(r, XN_CF, r->eax != 0 && xn_serial_rx_count[0] < 6);
}

void xn_helmet_a_fail_stub_r(xn_regs *r)
{
    r->eax = xn_helmet_a_fail_stub();
}

/* byte AL -> EAX and CF: 0 sent, 1 not */
void xn_helmet_a_send_r(xn_regs *r)
{
    r->eax = !xn_helmet_a_send((u8)r->eax);
    XN_SETFLAG(r, XN_CF, r->eax);
}

/* ---- driver B ---- */

/* port EAX -> EAX and CF: 0 open, 1 failed */
void xn_helmet_b_open_r(xn_regs *r)
{
    r->eax = !xn_helmet_b_open(r->eax);
    XN_SETFLAG(r, XN_CF, r->eax);
}

/* (sending a string leaves CF clear: its last compare finds the FFh) */
void xn_helmet_b_close_r(xn_regs *r)
{
    xn_helmet_b_close();
    XN_SETFLAG(r, XN_CF, 0);
}

void xn_helmet_b_reset_r(xn_regs *r)
{
    XN_SETFLAG(r, XN_CF, !xn_helmet_b_reset());
}

void xn_helmet_b_send_bang_r(xn_regs *r)
{
    xn_helmet_b_send_bang();
    XN_SETFLAG(r, XN_CF, 0);
}

void xn_helmet_b_start_r(xn_regs *r)
{
    xn_helmet_b_start();
    XN_SETFLAG(r, XN_CF, 0);
}

void xn_helmet_b_request_version_r(xn_regs *r)
{
    XN_SETFLAG(r, XN_CF, !xn_helmet_b_request_version());
}

void xn_helmet_b_stub_r(xn_regs *r)
{
    xn_helmet_b_stub();
}

void xn_helmet_b_parse_version_r(xn_regs *r)
{
    r->eax = xn_helmet_b_parse_version();
}

/* -> the angles in EAX, EDX, EBX, CF clear; or CF set with AL the value the search ended
   looking for and EDX 0 when it checked a candidate (its checksum loop's count) */
void xn_helmet_b_read_r(xn_regs *r)
{
    const u8 *ring = xn_serial_rx_buffers[xn_helmet_port - 1];
    u8 mark;
    s32 summed;
    xn_vec3 a;

    if (xn_helmet_b_read(&a)) {
        r->eax = a.x;
        r->edx = a.y;
        r->ebx = a.z;
        XN_SETFLAG(r, XN_CF, 0);
        return;
    }
    xn_helmet_b_find_packet(ring, (xn_serial_rx_tail[xn_helmet_port - 1] - 8) & 0x1FF, &mark,
                            &summed);
    r->eax = (r->eax & 0xFFFFFF00) | mark;
    if (summed)
        r->edx = 0;
    XN_SETFLAG(r, XN_CF, 1);
}

/* string EAX -> CF clear */
void xn_helmet_b_send_str_r(xn_regs *r)
{
    xn_helmet_b_send_str((const u8 *)r->eax);
    XN_SETFLAG(r, XN_CF, 0);
}

void xn_helmet_b_wait_ok_r(xn_regs *r)
{
    XN_SETFLAG(r, XN_CF, !xn_helmet_b_wait_ok());
}

void xn_helmet_b_wait_rx_r(xn_regs *r)
{
    XN_SETFLAG(r, XN_CF, !xn_helmet_b_wait_rx());
}

/* ---- driver C ---- */

/* -> EAX and CF: 0 open, 1 failed */
void xn_helmet_c_open_r(xn_regs *r)
{
    r->eax = !xn_helmet_c_open();
    XN_SETFLAG(r, XN_CF, r->eax);
}

void xn_helmet_c_close_r(xn_regs *r)
{
    XN_SETFLAG(r, XN_CF, xn_helmet_c_close());
}

void xn_helmet_c_reset_noop_r(xn_regs *r)
{
    xn_helmet_c_reset_noop();
}

void xn_helmet_c_request_noop_r(xn_regs *r)
{
    xn_helmet_c_request_noop();
}

void xn_helmet_c_cmd2_noop_r(xn_regs *r)
{
    xn_helmet_c_cmd2_noop();
}

/* BX for the driver; -> EAX ECX EDX EBX as the driver answered, then on success the angles
   in EAX EDX EBX and ESI the DOS block; CF on failure */
void xn_helmet_c_read_r(xn_regs *r)
{
    xn_vec3 a;
    s32 ok = xn_helmet_c_read(r->ebx, &a);

    r->eax = xn_helmet_c_rm_regs.eax;
    r->ebx = xn_helmet_c_rm_regs.ebx;
    r->ecx = xn_helmet_c_rm_regs.ecx;
    r->edx = xn_helmet_c_rm_regs.edx;
    if (ok) {
        r->esi = (u32)xn_helmet_c_dos_buffer;
        r->eax = a.x;
        r->edx = a.y;
        r->ebx = a.z;
    }
    XN_SETFLAG(r, XN_CF, !ok);
}

/* EAX EBX ECX EDX in; the driver's answer back in them */
void xn_helmet_c_int33_r(xn_regs *r)
{
    xn_helmet_c_int33(r->eax, r->ebx, r->ecx, r->edx);
    r->eax = xn_helmet_c_rm_regs.eax;
    r->ebx = xn_helmet_c_rm_regs.ebx;
    r->ecx = xn_helmet_c_rm_regs.ecx;
    r->edx = xn_helmet_c_rm_regs.edx;
}
