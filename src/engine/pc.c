/* pc.c: the PC under XnGine, the platform layer (canonical C; the interface and the module's
   documentation are in xpc.h).

   Each function fills a register file with the service's inputs (the registers it does not
   read left 0), makes the call through glue.asm's xn_intNN and reads the answer back. */
#include "xpc.h"

/* The game's C library (Watcom's, in object 1): protected-mode vectors as far pointers
   (selector:offset: returned in DX:EAX, passed in CX:EBX) */
typedef void (__interrupt __far *far_handler)(void);
far_handler func_000A1272(int intno);                   /* 0xA1272: _dos_getvect */
void func_000A12A6(int intno, far_handler handler);     /* 0xA12A6: the setvect it uses */

/* A register file with eax, ebx, ecx and edx set, the rest 0 */
static void regs(xn_regs *r, u32 eax, u32 ebx, u32 ecx, u32 edx)
{
    r->eax = eax;
    r->ebx = ebx;
    r->ecx = ecx;
    r->edx = edx;
    r->esi = r->edi = r->ebp = 0;
}

/* A far pointer's parts */
typedef union far_parts {
    far_handler fn;
    struct {
        u32 offset;
        u16 selector;
    } p;
} far_parts;

/* ---- interrupt vectors ------------------------------------------------------------------ */

void xn_pc_get_vector(u8 n, u32 *offset, u16 *selector)
{
    far_parts v;

    v.fn = func_000A1272(n);
    *offset = v.p.offset;
    *selector = v.p.selector;
}

void xn_pc_set_vector(u8 n, u32 offset, u16 selector)
{
    far_parts v;

    v.p.offset = offset;
    v.p.selector = selector;
    func_000A12A6(n, v.fn);
}

void xn_pc_install_vector(u8 n, void (*entry)(void))
{
    func_000A12A6(n, (far_handler)entry);       /* (the far pointer takes CS) */
}

/* ---- DPMI ------------------------------------------------------------------------------- */

static int dpmi(xn_regs *r)
{
    xn_int31(r);
    return (r->eflags & XN_CF) == 0;
}

int xn_dpmi_segment_selector(u16 segment, u16 *selector)
{
    xn_regs r;
    int ok;

    regs(&r, 0x0002, segment, 0, 0);
    ok = dpmi(&r);
    *selector = (u16)r.eax;
    return ok;
}

int xn_dpmi_selector_base(u16 selector, u32 *base)
{
    xn_regs r;
    int ok;

    regs(&r, 0x0006, selector, 0, 0);
    ok = dpmi(&r);
    *base = r.ecx << 16 | (r.edx & 0xFFFF);         /* CX:DX */
    return ok;
}

int xn_dpmi_dos_alloc(u16 paragraphs, u16 *segment, u16 *selector)
{
    xn_regs r;

    regs(&r, 0x0100, paragraphs, 0, 0);
    if (!dpmi(&r))
        return 0;
    *segment = (u16)r.eax;
    *selector = (u16)r.edx;
    return 1;
}

int xn_dpmi_dos_free(u16 selector)
{
    xn_regs r;

    regs(&r, 0x0101, 0, 0, selector);
    return dpmi(&r);
}

void xn_dpmi_get_exception(u8 n, u32 *offset, u16 *selector)
{
    xn_regs r;

    regs(&r, 0x0202, n, 0, 0);
    dpmi(&r);
    *offset = r.edx;
    *selector = (u16)r.ecx;
}

void xn_dpmi_set_exception(u8 n, u32 offset, u16 selector)
{
    xn_regs r;

    regs(&r, 0x0203, n, selector, offset);
    dpmi(&r);
}

void xn_dpmi_install_exception(u8 n, void (*entry)(void))
{
    far_parts v;

    v.fn = (far_handler)entry;                  /* (the far pointer takes CS) */
    xn_dpmi_set_exception(n, v.p.offset, v.p.selector);
}

int xn_dpmi_real_int(u8 n, xn_dpmi_rm_regs *rm)
{
    xn_regs r;

    regs(&r, 0x0300, n, 0, 0);                  /* BH = 0 flags, CX = 0 words */
    r.edi = (u32)rm;
    return dpmi(&r);
}

int xn_dpmi_lock(const void *addr, u32 size)
{
    xn_regs r;

    regs(&r, 0x0600, (u32)addr >> 16, (u32)addr & 0xFFFF, 0);  /* BX:CX the address */
    r.esi = size >> 16;                         /* SI:DI the size */
    r.edi = size & 0xFFFF;
    return dpmi(&r);
}

void xn_cw_set_transfer_buffer(u16 segment, u16 selector, u32 size)
{
    xn_regs r;

    regs(&r, 0xFF26, segment, size, selector);
    xn_int31(&r);
}

/* ---- the BIOS ------------------------------------------------------------------------------ */

int xn_bios_key_waiting(u16 *key)
{
    xn_regs r;

    regs(&r, 0x0100, 0, 0, 0);
    xn_int16(&r);
    if (r.eflags & XN_ZF)
        return 0;
    if (key != 0)
        *key = (u16)r.eax;
    return 1;
}

u16 xn_bios_read_key(void)
{
    xn_regs r;

    regs(&r, 0x0000, 0, 0, 0);
    xn_int16(&r);
    return (u16)r.eax;
}

void xn_bios_ps2_sample_rate(u8 rate)
{
    xn_regs r;

    regs(&r, 0xC202, (u32)rate << 8, 0, 0);
    xn_int15(&r);
}

/* ---- the multiplex interrupt ---------------------------------------------------------------- */

void xn_pc_yield(void)
{
    xn_regs r;

    regs(&r, 0x1680, 0, 0, 0);
    xn_int2f(&r);
}

/* ---- the mouse driver ------------------------------------------------------------------------- */

static void mousedrv(xn_regs *r, u16 fn, u16 bx, u16 cx, u16 dx)
{
    regs(r, fn, bx, cx, dx);
    xn_int33(r);
}

void xn_mousedrv_state(u16 *buttons, s16 *x, s16 *y)
{
    xn_regs r;

    mousedrv(&r, 0x03, 0, 0, 0);
    *buttons = (u16)r.ebx;
    *x = (s16)r.ecx;
    *y = (s16)r.edx;
}

void xn_mousedrv_set_position(s16 x, s16 y)
{
    xn_regs r;

    mousedrv(&r, 0x04, 0, x, y);
}

void xn_mousedrv_x_range(s16 lo, s16 hi)
{
    xn_regs r;

    mousedrv(&r, 0x07, 0, lo, hi);
}

void xn_mousedrv_y_range(s16 lo, s16 hi)
{
    xn_regs r;

    mousedrv(&r, 0x08, 0, lo, hi);
}

void xn_mousedrv_motion(s16 *dx, s16 *dy)
{
    xn_regs r;

    mousedrv(&r, 0x0B, 0, 0, 0);
    *dx = (s16)r.ecx;
    *dy = (s16)r.edx;
}

void xn_mousedrv_set_speed(u16 horizontal, u16 vertical, u16 threshold)
{
    xn_regs r;

    mousedrv(&r, 0x1A, horizontal, vertical, threshold);
}

void xn_mousedrv_speed(u16 *horizontal, u16 *vertical, u16 *threshold)
{
    xn_regs r;

    mousedrv(&r, 0x1B, 0, 0, 0);
    *horizontal = (u16)r.ebx;
    *vertical = (u16)r.ecx;
    *threshold = (u16)r.edx;
}
