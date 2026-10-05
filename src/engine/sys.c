/* sys.c: XnGine's system services as readable C (xsys.h; see xngine.h). */
#include "xsys.h"
#include "xmem.h"

void xn_sys_crit_error_handler_r(xn_regs *r)
{
    r->eax = 1;
}

void xn_sys_install_crit_error_handler(void)
{
    u32 flags = xn_save_flags();
    xn_regs v;

    if (xn_sys_int24_installed != 1) {
        dpmi_lock_region(asm_xn_sys_crit_error_handler, 0x1006);
        xn_cli();
        v.eax = 0x24;
        xn_asmcall(func_000A1272, &v);      /* _dos_getvect(24h): DX:EAX */
        xn_sys_old_int24_sel = (u16)v.edx;
        xn_sys_old_int24_offset = v.eax;
        func_000A12A6(0x24, asm_xn_sys_crit_error_handler, xn_cs());
        xn_sys_int24_installed = 1;
        xn_sti();
    }
    xn_restore_flags(flags);
}

void xn_sys_restore_crit_error_handler(void)
{
    u32 flags = xn_save_flags();

    if (xn_sys_int24_installed != 0) {
        xn_cli();
        func_000A12A6(0x24, (void *)xn_sys_old_int24_offset, xn_sys_old_int24_sel);
        xn_sti();
        dpmi_unlock_region(asm_xn_sys_crit_error_handler, 0x1006);
    }
    xn_restore_flags(flags);
}

void xn_sys_yield_r(xn_regs *r)
{
    r->eax = (r->eax & 0xFFFF0000) | 0x1680;
    xn_int2f(r);
}

void xn_sys_set_dos_transfer_buffer(void)
{
    xn_regs d;

    d.eax = 0x100;
    d.ebx = 0xFFF;
    xn_int31(&d);
    if (d.eflags & XN_CF)
        return;
    xn_sys_dos_transfer_selector = (u16)d.edx;
    d.ebx = (d.ebx & 0xFFFF0000) | (u16)d.eax;     /* its real-mode segment */
    d.ecx = 0xFFF0;
    d.eax = (d.eax & 0xFFFF0000) | 0xFF26;
    xn_int31(&d);
}

void xn_sys_zero_page_save(void)
{
    const volatile u32 *zero = 0;
    s32 k;

    for (k = 0; k < 256; k++)
        xn_zero_page_copy[k] = zero[k];
}

void xn_sys_zero_page_check(void)
{
    volatile u32 *zero = 0;
    s32 k;

    for (k = 0; k < 256; k++) {
        if (zero[k] == xn_zero_page_copy[k] || k == 0 || k == 1 || k == 8)
            continue;                       /* (the divide, single step and timer vectors) */
        internal_check_failed = 4 * k;
        for (k = 0; k < 256; k++)
            zero[k] = xn_zero_page_copy[k];
        fatal_error(xn_msg_bad_zero_page);
        return;
    }
}

void xn_sys_install_divide_handler(void)
{
    xn_regs d;

    if (xn_sys_div_handler_installed == 1)
        return;
    xn_sys_div_handler_installed = 1;
    d.eax = 0x202;                          /* get exception 0's handler: CX:EDX */
    d.ebx = 0;
    xn_int31(&d);
    xn_sys_old_div_handler_selector = (u16)d.ecx;
    xn_sys_old_div_handler_offset = d.edx;
    d.eax = 0x203;                          /* set it */
    d.ebx = 0;
    d.ecx = (d.ecx & 0xFFFF0000) | xn_cs();
    d.edx = (u32)asm_xn_sys_divide_error_handler;
    xn_int31(&d);
    xn_mem_lock_region(asm_xn_sys_divide_error_handler,
                       xn_code_14A0E5 - (u8 *)asm_xn_sys_divide_error_handler);
}

void xn_sys_remove_divide_handler(void)
{
    xn_regs d;

    if (xn_sys_div_handler_installed == 0)
        return;
    xn_sys_div_handler_installed = 0;
    if (xn_sys_old_div_handler_selector == 0)
        return;
    d.eax = 0x203;
    d.ebx = 0;
    d.ecx = xn_sys_old_div_handler_selector;
    d.edx = xn_sys_old_div_handler_offset;
    xn_int31(&d);
}

/* The length of the faulting divide by its ModRM byte, as far as the asm knows forms */
static u32 divide_length(u8 modrm)
{
    if (modrm == 0x35 || modrm == 0x3D)                     /* [disp32] */
        return 6;
    if (modrm >= 0x70 && modrm <= 0x7F && modrm != 0x74 && modrm != 0x7C)
        return 3;                                           /* [reg + disp8] */
    if (modrm >= 0xB0 && modrm <= 0xBF && modrm != 0xB4 && modrm != 0xBC)
        return 6;                                           /* [reg + disp32] */
    return 2;                                               /* a register (or SIB: wrong) */
}

void xn_sys_divide_error_handler(xn_dpmi_exc_frame *f)
{
    f->eip += divide_length(((u8 *)f->eip)[1]);
}

void xn_sys_divide_error_handler_r(xn_regs *r)
{
    /* the stub's pushfd sits on the frame: it starts 4 bytes above the saved ESP */
    xn_sys_divide_error_handler((xn_dpmi_exc_frame *)(r->esp + 4));
    r->eax = 0;
    r->edx = 0;
}
