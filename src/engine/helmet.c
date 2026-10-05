/* helmet.c: XnGine's head trackers as readable C (xhelmet.h; see xngine.h). */
#include "xhelmet.h"
#include "xserial.h"
#include "xstr.h"
#include "xtimer.h"

/* ---- the front end ------------------------------------------------------------------------ */

/* A driver's routine in one of the slot tables: cfg_helmet is a byte offset into them */
static void (*drv_slot(void (**table)(void), s32 offset))(void)
{
    return *(void (**)(void))((u8 *)table + offset);
}

s32 xn_helmet_open(s32 port, s32 driver)
{
    xn_regs r;

    xn_helmet_port = port;
    cfg_helmet = driver;
    r.eax = port;
    r.edx = driver;
    xn_asmcall(drv_slot(xn_helmet_drv_open, driver), &r);
    if (r.eflags & XN_CF) {
        cfg_helmet = -1;
        xn_helmet_active = 0;
        return 0;
    }
    xn_helmet_active = 1;
    return 1;
}

void xn_helmet_open_r(xn_regs *r)
{
    XN_SETFLAG(r, XN_CF, !xn_helmet_open(r->eax, r->edx));
}

void xn_helmet_close(void)
{
    xn_regs r;

    if (xn_helmet_port == 0 || cfg_helmet < 0)
        return;
    r.eax = xn_helmet_port;
    r.edx = cfg_helmet;
    xn_asmcall(drv_slot(xn_helmet_drv_close, cfg_helmet), &r);
    xn_serial_close(r.eax);                 /* (driver C too: it has no serial port) */
}

void xn_helmet_request(void)
{
    xn_regs r;

    if (xn_helmet_active == 0 || cfg_helmet < 0)
        return;
    r.eax = cfg_helmet;
    xn_asmcall(drv_slot(xn_helmet_drv_request, cfg_helmet), &r);
}

void xn_helmet_poll_r(xn_regs *r)
{
    xn_regs d = *r;
    xn_vec3 s;

    if (xn_helmet_active == 0 || cfg_helmet == -1)
        return;
    d.eax = cfg_helmet;
    xn_asmcall(drv_slot(xn_helmet_drv_read, cfg_helmet), &d);
    if (d.eflags & XN_CF)
        return;
    s.x = d.eax;
    s.y = d.edx;
    s.z = d.ebx;
    xn_helmet_smooth(&s);
    xn_helmet_pitch = s.x;
    xn_helmet_yaw = s.y;
    xn_helmet_roll = s.z;
}

/* sum / n, a 64-bit idiv (n = 0: the divide handler's 0) */
static s32 idiv32(s32 sum, s32 n)
{
    xn_s64 q;

    xn_s64_set(&q, sum);
    return xn_s64_div(&q, n);
}

void xn_helmet_smooth(xn_vec3 *s)
{
    s32 off, d, y, n;
    const xn_vec3 *e;

    *(xn_vec3 *)((u8 *)xn_helmet_ring + xn_helmet_ring_pos) = *s;
    if (xn_helmet_smoothing == 0)
        return;
    if (xn_helmet_smoothing > 7)
        xn_helmet_smoothing = 7;
    xn_pick_view_x = 0;
    xn_pick_view_y = 0;
    pick_distance = 0;
    off = (xn_helmet_smoothing - 1) * 12;
    do {
        e = (const xn_vec3 *)((const u8 *)xn_helmet_ring + off);
        xn_pick_view_x += e->x;
        y = e->y;
        d = y - s->y;
        if (d < 0)
            d = -d;
        if (d > 0x4000)                     /* across the wrap: half a turn nearer */
            y += s->y < 0 ? -0x8000 : 0x8000;
        xn_pick_view_y += y;
        pick_distance += e->z;
        off -= 12;
    } while (off >= 0);
    n = xn_helmet_smoothing << 4;
    s->x = idiv32(xn_pick_view_x, n);
    s->y = idiv32(xn_pick_view_y, n);
    s->z = idiv32(pick_distance, n);
    xn_helmet_ring_pos += 12;
    if (xn_helmet_ring_pos == ((u32)n >> 4) * 12)
        xn_helmet_ring_pos = 0;
}

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

/* ---- driver A ----------------------------------------------------------------------------- */

s32 xn_helmet_a_open(s32 port)
{
    s32 sent;

    xn_helmet_a_port = port;
    xn_serial_open(port, 6, 3);             /* 19200 baud, 8N1 */
    xn_helmet_a_reset();
    sent = xn_helmet_a_send('G') == 0;
    xn_helmet_a_angles[0] = 0;
    xn_helmet_a_angles[1] = 0;
    xn_helmet_a_angles[2] = 0;
    return sent;
}

void xn_helmet_a_open_r(xn_regs *r)
{
    XN_SETFLAG(r, XN_CF, !xn_helmet_a_open(r->eax));
}

void xn_helmet_a_close(void)
{
}

s32 xn_helmet_a_reset(void)
{
    return xn_helmet_a_send('R') == 0;
}

void xn_helmet_a_reset_r(xn_regs *r)
{
    XN_SETFLAG(r, XN_CF, !xn_helmet_a_reset());
}

s32 xn_helmet_a_send_h(void)
{
    return xn_helmet_a_send('H') == 0;
}

void xn_helmet_a_send_h_r(xn_regs *r)
{
    XN_SETFLAG(r, XN_CF, !xn_helmet_a_send_h());
}

s32 xn_helmet_a_request(void)
{
    return xn_helmet_a_send('G');
}

void xn_helmet_a_request_r(xn_regs *r)
{
    r->eax = xn_helmet_a_request();
    XN_SETFLAG(r, XN_CF, r->eax);
}

/* Waits for n bytes in port 1's ring, polling it `loops` times; then reads n bytes into the
   record (those the ring has), skipping CR and LF when text; 0, or -1 on the timeout */
static s32 a_read_record(u32 loops, u32 n, s32 text)
{
    char *p = xn_helmet_a_record;
    u8 c;

    for (; loops != 0; loops--)
        if (xn_serial_rx_count[0] == n)
            break;
    if (loops == 0)
        return -1;
    for (; n != 0; n--) {
        if (!xn_serial_rx_get(xn_helmet_a_port, &c))
            continue;
        if (text && (c == '\n' || c == '\r'))
            continue;
        *p++ = c;
    }
    return 0;
}

s32 xn_helmet_a_read_text(void)
{
    return a_read_record(0xFFFFFF, 18, 1);
}

s32 xn_helmet_a_read(void)
{
    return a_read_record(0xFFFF, 6, 0);
}

void xn_helmet_a_read_r(xn_regs *r)
{
    r->eax = xn_helmet_a_read();
    /* the timeout's CF is its last compare's: the count below 6 */
    XN_SETFLAG(r, XN_CF, r->eax != 0 && xn_serial_rx_count[0] < 6);
}

s32 xn_helmet_a_fail_stub(void)
{
    return -1;
}

s32 xn_helmet_a_send(u8 byte)
{
    return xn_serial_send_byte(xn_helmet_a_port, byte);
}

void xn_helmet_a_send_r(xn_regs *r)
{
    r->eax = xn_helmet_a_send((u8)r->eax);
    XN_SETFLAG(r, XN_CF, r->eax);
}

/* ---- driver B ----------------------------------------------------------------------------- */

s32 xn_helmet_b_open(s32 port)
{
    s32 tries;

    xn_helmet_port = port;
    xn_serial_open(port, 6, 3);
    if (!xn_helmet_b_reset())
        goto fail;
    for (tries = 16;; ) {                   /* the mode, until it says 'O' */
        xn_helmet_b_send_str(xn_helmet_b_str_mode);
        if (xn_helmet_b_wait_ok())
            break;
        if (--tries == 0)
            goto fail;
    }
    xn_helmet_b_yaw_sign = 1;
    if (!xn_helmet_b_request_version())
        goto fail;
    if ((u32)xn_helmet_b_parse_version() >= 1003)
        xn_helmet_b_yaw_sign = -xn_helmet_b_yaw_sign;
    xn_helmet_b_send_bang();
    xn_serial_rx_reset(xn_helmet_port);
    xn_helmet_b_start();
    return 0;
fail:
    xn_serial_close(xn_helmet_port);
    return 1;
}

void xn_helmet_b_open_r(xn_regs *r)
{
    r->eax = xn_helmet_b_open(r->eax);
    XN_SETFLAG(r, XN_CF, r->eax);
}

void xn_helmet_b_close(void)
{
    xn_helmet_b_send_str(xn_helmet_b_str_reset);
}

/* (sending a string leaves CF clear: its last compare finds the FFh) */
void xn_helmet_b_close_r(xn_regs *r)
{
    xn_helmet_b_close();
    XN_SETFLAG(r, XN_CF, 0);
}

s32 xn_helmet_b_reset(void)
{
    s32 tries;

    for (tries = 8; tries != 0; tries--) {
        xn_helmet_b_send_str(xn_helmet_b_str_reset);
        if (xn_helmet_b_wait_ok())
            return 1;
    }
    return 0;
}

void xn_helmet_b_reset_r(xn_regs *r)
{
    XN_SETFLAG(r, XN_CF, !xn_helmet_b_reset());
}

void xn_helmet_b_send_bang(void)
{
    xn_helmet_b_send_str(xn_helmet_b_str_bang);
}

void xn_helmet_b_send_bang_r(xn_regs *r)
{
    xn_helmet_b_send_bang();
    XN_SETFLAG(r, XN_CF, 0);
}

void xn_helmet_b_start(void)
{
    xn_helmet_b_send_str(xn_helmet_b_str_start);
}

void xn_helmet_b_start_r(xn_regs *r)
{
    xn_helmet_b_start();
    XN_SETFLAG(r, XN_CF, 0);
}

s32 xn_helmet_b_request_version(void)
{
    u32 start;

    xn_serial_rx_reset(xn_helmet_port);
    xn_helmet_b_send_str(xn_helmet_b_str_version);
    start = XN_BIOS_TICKS;
    for (;;) {
        if (xn_serial_rx_count[xn_helmet_port - 1] == 60)
            return 1;
        if (XN_BIOS_TICKS - start > 18)
            return 0;
    }
}

void xn_helmet_b_request_version_r(xn_regs *r)
{
    XN_SETFLAG(r, XN_CF, !xn_helmet_b_request_version());
}

void xn_helmet_b_stub(void)
{
}

s32 xn_helmet_b_parse_version(void)
{
    const u8 *p = xn_serial_rx_buffers[xn_helmet_port - 1] - 1;
    const u8 *end = p + 0x200;

    do {
        if (++p == end)
            return 0;
    } while (*p != 'M');
    do {
        if (++p == end)
            return 0;
    } while (*p != 'F');
    return xn_str_to_int((const char *)p + 1) * 1000 + xn_str_to_int((const char *)p + 5);
}

/* Driver B's packet search in the ring (see xhelmet.h): the packet's FFh index, or -1. *mark:
   the byte value the search ended looking for (FFh, or the last bad checksum); *summed:
   whether a checksum was computed (the asm's EDX is then 0) */
static s32 b_find_packet(const u8 *ring, u32 i, u8 *mark, s32 *summed)
{
    s32 left = 0x80;
    u32 j;
    s32 k;
    u8 sum;

    *mark = 0xFF;
    *summed = 0;
    for (;;) {
        if (ring[i] == *mark) {
            sum = *mark;
            j = i;
            for (k = 0; k < 6; k++) {
                j = (j + 1) & 0x1FF;
                sum += ring[j];
            }
            j = (j + 1) & 0x1FF;
            *summed = 1;
            if (sum == ring[j])
                return i;
            *mark = sum;                    /* (sic) and the same byte again */
            continue;
        }
        i = (i - 1) & 0x1FF;
        if (--left == 0)
            return -1;
    }
}

/* The big-endian word at ring index i (wrapping) */
static s16 ring_word(const u8 *ring, u32 i)
{
    return (s16)(ring[i & 0x1FF] << 8 | ring[(i + 1) & 0x1FF]);
}

static s32 b_read(xn_vec3 *angles, u8 *mark, s32 *summed)
{
    const u8 *ring = xn_serial_rx_buffers[xn_helmet_port - 1];
    s32 i = b_find_packet(ring, (xn_serial_rx_tail[xn_helmet_port - 1] - 8) & 0x1FF, mark,
                          summed);

    if (i < 0)
        return 0;
    angles->y = ring_word(ring, i + 1) * xn_helmet_b_yaw_sign;
    angles->x = -ring_word(ring, i + 3);
    angles->z = -ring_word(ring, i + 5);
    return 1;
}

s32 xn_helmet_b_read(xn_vec3 *angles)
{
    u8 mark;
    s32 summed;

    return b_read(angles, &mark, &summed);
}

void xn_helmet_b_read_r(xn_regs *r)
{
    xn_vec3 a;
    u8 mark;
    s32 summed;

    if (b_read(&a, &mark, &summed)) {
        r->eax = a.x;
        r->edx = a.y;
        r->ebx = a.z;
        XN_SETFLAG(r, XN_CF, 0);
        return;
    }
    r->eax = (r->eax & 0xFFFFFF00) | mark;  /* the asm searched in AL */
    if (summed)
        r->edx = 0;                         /* its checksum loop's count */
    XN_SETFLAG(r, XN_CF, 1);
}

void xn_helmet_b_send_str(const u8 *s)
{
    for (;;) {
        while (!(xn_serial_read_lsr(xn_helmet_port) & 0x40))   /* the line idle */
            ;
        if (*s == 0xFF)
            return;
        xn_serial_send_byte(xn_helmet_port, *s++);
    }
}

void xn_helmet_b_send_str_r(xn_regs *r)
{
    xn_helmet_b_send_str((const u8 *)r->eax);
    XN_SETFLAG(r, XN_CF, 0);
}

s32 xn_helmet_b_wait_ok(void)
{
    u8 c = 0;

    if (!xn_helmet_b_wait_rx())
        return 0;
    xn_serial_rx_get(xn_helmet_port, &c);  /* (an empty ring reads as 0) */
    return c == 'O';
}

void xn_helmet_b_wait_ok_r(xn_regs *r)
{
    XN_SETFLAG(r, XN_CF, !xn_helmet_b_wait_ok());
}

s32 xn_helmet_b_wait_rx(void)
{
    u32 start = XN_BIOS_TICKS;

    for (;;) {
        if (xn_serial_rx_count[xn_helmet_port - 1] != 0)
            return 1;
        if (XN_BIOS_TICKS - start > 9)
            return 0;
    }
}

void xn_helmet_b_wait_rx_r(xn_regs *r)
{
    XN_SETFLAG(r, XN_CF, !xn_helmet_b_wait_rx());
}

/* ---- driver C ----------------------------------------------------------------------------- */

s32 xn_helmet_c_open(void)
{
    xn_dpmi_rm_regs *rm = &xn_helmet_c_rm_regs;
    xn_regs d;
    u8 *p;
    u32 sel, len;

    xn_helmet_c_int33(0x607F, 0, 0, 0);     /* is the tracker there? */
    if ((u16)rm->eax != 0x7F60 || (u8)(rm->ecx >> 8) != 1 || ((u16)rm->edx | (u16)rm->ebx))
        return 1;
    d.eax = 0x100;                          /* a DOS block of 80h paragraphs */
    d.ebx = 0x80;
    xn_int31(&d);
    if (d.eflags & XN_CF)
        return 1;
    rm->ds = (u16)d.eax;
    rm->es = (u16)d.eax;
    sel = (u16)d.edx;
    xn_helmet_c_dos_selector = sel;
    d.ebx = sel;                            /* its linear base: CX:DX */
    d.eax = 6;
    xn_int31(&d);
    xn_helmet_c_dos_buffer = (u8 *)(d.ecx << 16 | (u16)d.edx);
    xn_helmet_c_int33(0x6003, d.ebx, 0, 0); /* the configuration into it */
    if ((u16)rm->eax != 0)
        return 1;
    for (p = xn_helmet_c_dos_buffer;; p += len) {   /* records: length, type */
        len = p[0];
        if (p[1] == 6)
            break;
        if (p[1] == 0)
            return 1;
    }
    p[0x0E] = 0x8A;
    p[0x16] = 0x88;
    p[0x1E] = 0x88;
    p += len;
    p[0x16] = 0x88;
    p[0x1E] = 0x88;
    xn_helmet_c_int33(0x6004, rm->ebx, 0, 0);   /* and back */
    if ((u16)rm->eax != 0)
        return 1;
    return 0;
}

void xn_helmet_c_open_r(xn_regs *r)
{
    r->eax = xn_helmet_c_open();
    XN_SETFLAG(r, XN_CF, r->eax);
}

s32 xn_helmet_c_close(void)
{
    xn_regs d;

    if (xn_helmet_c_dos_selector == 0)
        return 0;
    d.eax = 0x101;
    d.edx = xn_helmet_c_dos_selector;
    xn_int31(&d);
    return (d.eflags & XN_CF) != 0;
}

void xn_helmet_c_close_r(xn_regs *r)
{
    XN_SETFLAG(r, XN_CF, xn_helmet_c_close());
}

void xn_helmet_c_reset_noop(void)
{
}

void xn_helmet_c_request_noop(void)
{
}

void xn_helmet_c_cmd2_noop(void)
{
}

void xn_helmet_c_read_r(xn_regs *r)
{
    const s16 *a = (const s16 *)xn_helmet_c_dos_buffer;
    xn_dpmi_rm_regs *rm = &xn_helmet_c_rm_regs;

    xn_helmet_c_int33(0x6005, r->ebx, 0, 0);
    r->eax = rm->eax;
    r->ebx = rm->ebx;
    r->ecx = rm->ecx;
    r->edx = rm->edx;
    if ((u16)r->eax != 0 || r->ecx == 0) {
        XN_SETFLAG(r, XN_CF, 1);
        return;
    }
    r->esi = (u32)a;
    r->edx = -(a[0] >> 1);
    r->eax = -(a[1] >> 1);
    r->ebx = a[2] >> 1;
    XN_SETFLAG(r, XN_CF, 0);
}

void xn_helmet_c_int33(u32 eax, u32 ebx, u32 ecx, u32 edx)
{
    xn_regs d;

    xn_helmet_c_rm_regs.eax = eax;
    xn_helmet_c_rm_regs.ebx = ebx;
    xn_helmet_c_rm_regs.ecx = ecx;
    xn_helmet_c_rm_regs.edx = edx;
    d.eax = 0x300;                          /* simulate a real-mode interrupt */
    d.ebx = 0x33;
    d.ecx = 0;
    d.edi = (u32)&xn_helmet_c_rm_regs;
    xn_int31(&d);
}

void xn_helmet_c_int33_r(xn_regs *r)
{
    xn_helmet_c_int33(r->eax, r->ebx, r->ecx, r->edx);
    r->eax = xn_helmet_c_rm_regs.eax;
    r->ebx = xn_helmet_c_rm_regs.ebx;
    r->ecx = xn_helmet_c_rm_regs.ecx;
    r->edx = xn_helmet_c_rm_regs.edx;
}
