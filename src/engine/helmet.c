/* helmet.c: XnGine's head trackers (canonical C; the interface and the module's documentation
   are in xhelmet.h). */
#include "xhelmet.h"
#include "xserial.h"
#include "xstr.h"
#include "xpc.h"

#define BAUD_19200      6                   /* the UART divisor */
#define LINE_8N1        3

/* ---- the drivers, as the front end calls them ------------------------------------------- */

/* Driver A's read: a sample when a record came, or when the read timed out with more than 6
   bytes waiting (the asm's carry is its last compare: the count below 6) */
static int a_read_sample(xn_vec3 *s)
{
    s32 r = xn_helmet_a_read();

    if (r != 0 && xn_serial_rx_count[0] < 6)
        return 0;
    /* Quirk Q-HELMET-02: A decodes no angles. The asm's front end smooths its EAX (r) and the
       EDX and EBX its own caller left; canonical C smooths 0 for those */
    s->x = r;
    s->y = 0;
    s->z = 0;
    return 1;
}

static void a_request(void)
{
    xn_helmet_a_request();
}

static int c_open(s32 port)
{
    return xn_helmet_c_open();
}

static void c_close(void)
{
    xn_helmet_c_close();
}

/* Quirk Q-HELMET-06: the asm passes its caller's EBX to int 33h 6005h; canonical C 0 */
static int c_read_sample(xn_vec3 *s)
{
    return xn_helmet_c_read(0, s);
}

/* By cfg_helmet / 4 (the asm's tables at 0x153419: open, close, read, request) */
static const xn_helmet_driver drivers[3] = {
    { xn_helmet_a_open, xn_helmet_a_close, a_read_sample, a_request },
    { xn_helmet_b_open, xn_helmet_b_close, xn_helmet_b_read, xn_helmet_b_start },
    { c_open, c_close, c_read_sample, xn_helmet_c_request_noop },
};

#pragma off (unreferenced)

/* ---- the front end ------------------------------------------------------------------------ */

s32 xn_helmet_open(s32 port, s32 driver)
{
    xn_helmet_port = port;
    cfg_helmet = driver;
    if (!drivers[driver >> 2].open(port)) {
        cfg_helmet = -1;
        xn_helmet_active = 0;
        return 0;
    }
    xn_helmet_active = 1;
    return 1;
}

void xn_helmet_close(void)
{
    if (xn_helmet_port == 0 || cfg_helmet < 0)
        return;
    drivers[cfg_helmet >> 2].close();
    xn_serial_close(xn_helmet_port);        /* (driver C too: it has no serial port) */
}

void xn_helmet_request(void)
{
    if (xn_helmet_active == 0 || cfg_helmet < 0)
        return;
    drivers[cfg_helmet >> 2].request();
}

void xn_helmet_poll(void)
{
    xn_vec3 s;

    if (xn_helmet_active == 0 || cfg_helmet == -1)
        return;
    if (!drivers[cfg_helmet >> 2].read(&s))
        return;
    xn_helmet_smooth(&s);
    xn_helmet_pitch = s.x;
    xn_helmet_yaw = s.y;
    xn_helmet_roll = s.z;
}

/* sum / n (Q-SYS-01: 0 for n = 0) */
static s32 div_or0(s32 sum, s32 n)
{
    xn_s64 q;

    xn_s64_set(&q, sum);
    return xn_s64_div_or0(&q, n);
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
    /* Quirk Q-HELMET-01: the sums are kept in the pick's scratch point */
    xn_pick_view_x = 0;
    xn_pick_view_y = 0;
    pick_distance = 0;
    off = (xn_helmet_smoothing - 1) * (s32)sizeof(xn_vec3);
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
        off -= sizeof(xn_vec3);
    } while (off >= 0);
    n = xn_helmet_smoothing << 4;
    s->x = div_or0(xn_pick_view_x, n);
    s->y = div_or0(xn_pick_view_y, n);
    s->z = div_or0(pick_distance, n);
    xn_helmet_ring_pos += sizeof(xn_vec3);
    if (xn_helmet_ring_pos == ((u32)n >> 4) * sizeof(xn_vec3))
        xn_helmet_ring_pos = 0;
}

/* ---- driver A ----------------------------------------------------------------------------- */

s32 xn_helmet_a_open(s32 port)
{
    s32 sent;

    xn_helmet_a_port = port;
    xn_serial_open(port, BAUD_19200, LINE_8N1);
    xn_helmet_a_reset();
    sent = xn_helmet_a_send('G');
    xn_helmet_a_angles[0] = 0;
    xn_helmet_a_angles[1] = 0;
    xn_helmet_a_angles[2] = 0;
    return sent;
}

void xn_helmet_a_close(void)
{
}

s32 xn_helmet_a_reset(void)
{
    return xn_helmet_a_send('R');
}

s32 xn_helmet_a_send_h(void)
{
    return xn_helmet_a_send('H');
}

s32 xn_helmet_a_request(void)
{
    return xn_helmet_a_send('G');
}

/* Waits for n bytes in port 1's ring (Quirk Q-HELMET-04: whatever the port), polling it
   `loops` times; then reads n bytes into the record (those the ring has), skipping CR and LF
   when text; 0, or -1 on the timeout */
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

s32 xn_helmet_a_fail_stub(void)
{
    return -1;
}

s32 xn_helmet_a_send(u8 byte)
{
    return xn_serial_send_byte(xn_helmet_a_port, byte) == 0;
}

/* ---- driver B ----------------------------------------------------------------------------- */

s32 xn_helmet_b_open(s32 port)
{
    s32 tries;

    xn_helmet_port = port;
    xn_serial_open(port, BAUD_19200, LINE_8N1);
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
    return 1;
fail:
    xn_serial_close(xn_helmet_port);
    return 0;
}

void xn_helmet_b_close(void)
{
    xn_helmet_b_send_str(xn_helmet_b_str_reset);
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

void xn_helmet_b_send_bang(void)
{
    xn_helmet_b_send_str(xn_helmet_b_str_bang);
}

void xn_helmet_b_start(void)
{
    xn_helmet_b_send_str(xn_helmet_b_str_start);
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

void xn_helmet_b_stub(void)
{
}

s32 xn_helmet_b_parse_version(void)
{
    /* Quirk Q-HELMET-05: from the ring's start, 511 bytes */
    const u8 *p = xn_serial_rx_buffers[xn_helmet_port - 1] - 1;
    const u8 *end = p + XN_SERIAL_RING;

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

s32 xn_helmet_b_find_packet(const u8 *ring, u32 start, u8 *mark, s32 *summed)
{
    s32 left = 0x80;
    u32 i = start, j;
    s32 k;
    u8 sum;

    *mark = 0xFF;
    *summed = 0;
    for (;;) {
        if (ring[i] == *mark) {
            sum = *mark;
            j = i;
            for (k = 0; k < 6; k++) {
                j = (j + 1) & (XN_SERIAL_RING - 1);
                sum += ring[j];
            }
            j = (j + 1) & (XN_SERIAL_RING - 1);
            *summed = 1;
            if (sum == ring[j])
                return i;
            *mark = sum;                    /* Quirk Q-HELMET-03: the same byte again */
            continue;
        }
        i = (i - 1) & (XN_SERIAL_RING - 1);
        if (--left == 0)
            return -1;
    }
}

/* The big-endian word at ring index i (wrapping) */
static s16 ring_word(const u8 *ring, u32 i)
{
    return (s16)(ring[i & (XN_SERIAL_RING - 1)] << 8 | ring[(i + 1) & (XN_SERIAL_RING - 1)]);
}

s32 xn_helmet_b_read(xn_vec3 *angles)
{
    const u8 *ring = xn_serial_rx_buffers[xn_helmet_port - 1];
    u32 start = (xn_serial_rx_tail[xn_helmet_port - 1] - 8) & (XN_SERIAL_RING - 1);
    u8 mark;
    s32 summed;
    s32 i = xn_helmet_b_find_packet(ring, start, &mark, &summed);

    if (i < 0)
        return 0;
    angles->y = ring_word(ring, i + 1) * xn_helmet_b_yaw_sign;
    angles->x = -ring_word(ring, i + 3);
    angles->z = -ring_word(ring, i + 5);
    return 1;
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

s32 xn_helmet_b_wait_ok(void)
{
    u8 c = 0;

    if (!xn_helmet_b_wait_rx())
        return 0;
    xn_serial_rx_get(xn_helmet_port, &c);  /* (an empty ring reads as 0) */
    return c == 'O';
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

/* ---- driver C ----------------------------------------------------------------------------- */

#define TRACKER_DETECT  0x607F
#define TRACKER_GET     0x6003              /* the configuration into the DOS block */
#define TRACKER_SET     0x6004
#define TRACKER_READ    0x6005

s32 xn_helmet_c_open(void)
{
    xn_dpmi_rm_regs *rm = &xn_helmet_c_rm_regs;
    u16 segment, selector;
    u32 base, len;
    u8 *p;

    xn_helmet_c_int33(TRACKER_DETECT, 0, 0, 0);
    if ((u16)rm->eax != 0x7F60 || (u8)(rm->ecx >> 8) != 1 || ((u16)rm->edx | (u16)rm->ebx))
        return 0;
    if (!xn_dpmi_dos_alloc(0x80, &segment, &selector))
        return 0;
    rm->ds = rm->es = segment;
    xn_helmet_c_dos_selector = selector;
    xn_dpmi_selector_base(selector, &base);
    xn_helmet_c_dos_buffer = (u8 *)base;
    /* (BX: the selector, as the asm's EBX still holds it) */
    xn_helmet_c_int33(TRACKER_GET, selector, 0, 0);
    if ((u16)rm->eax != 0)
        return 0;
    for (p = xn_helmet_c_dos_buffer;; p += len) {   /* records: length, type */
        len = p[0];
        if (p[1] == 6)
            break;
        if (p[1] == 0)
            return 0;
    }
    p[0x0E] = 0x8A;
    p[0x16] = 0x88;
    p[0x1E] = 0x88;
    p += len;
    p[0x16] = 0x88;
    p[0x1E] = 0x88;
    xn_helmet_c_int33(TRACKER_SET, rm->ebx, 0, 0);  /* (BX: the driver's last answer) */
    return (u16)rm->eax == 0;
}

s32 xn_helmet_c_close(void)
{
    if (xn_helmet_c_dos_selector == 0)
        return 0;
    return !xn_dpmi_dos_free((u16)xn_helmet_c_dos_selector);
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

s32 xn_helmet_c_read(u32 bx, xn_vec3 *angles)
{
    const s16 *a = (const s16 *)xn_helmet_c_dos_buffer;
    xn_dpmi_rm_regs *rm = &xn_helmet_c_rm_regs;

    xn_helmet_c_int33(TRACKER_READ, bx, 0, 0);
    if ((u16)rm->eax != 0 || rm->ecx == 0)
        return 0;
    angles->y = -(a[0] >> 1);
    angles->x = -(a[1] >> 1);
    angles->z = a[2] >> 1;
    return 1;
}

void xn_helmet_c_int33(u32 eax, u32 ebx, u32 ecx, u32 edx)
{
    xn_helmet_c_rm_regs.eax = eax;
    xn_helmet_c_rm_regs.ebx = ebx;
    xn_helmet_c_rm_regs.ecx = ecx;
    xn_helmet_c_rm_regs.edx = edx;
    xn_dpmi_real_int(0x33, &xn_helmet_c_rm_regs);
}
