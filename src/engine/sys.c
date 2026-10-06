/* sys.c: XnGine's system services (canonical C; the interface and the module's documentation
   are in xsys.h). */
#include "xsys.h"
#include "ptrint.h"
#include "doslow.h"
#include "xsysutil.h"
#include "xpc.h"
#include "xmem.h"

#define CRIT_ERROR_LOCK_BYTES   0x1006  /* the int 24h handler's module, from its entry */
#define CRIT_ERROR_RETRY        1       /* int 24h's answers: 0 ignore, 1 retry, 2 abort */

/* ---- the critical error handler --------------------------------------------------------- */

void xn_sys_crit_error_handler(xn_int_frame *f)
{
    f->eax = CRIT_ERROR_RETRY;
}

void xn_sys_install_crit_error_handler(void)
{
    u32 flags = xn_save_flags();

    if (xn_sys_int24_installed != 1) {
        dpmi_lock_region((void *)xn_sys_crit_error_entry, CRIT_ERROR_LOCK_BYTES);
        xn_cli();
        xn_pc_get_vector(0x24, &xn_sys_old_int24_offset, &xn_sys_old_int24_sel);
        xn_pc_install_vector(0x24, xn_sys_crit_error_entry);
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
        xn_pc_set_vector(0x24, xn_sys_old_int24_offset, xn_sys_old_int24_sel);
        xn_sti();
        dpmi_unlock_region((void *)xn_sys_crit_error_entry, CRIT_ERROR_LOCK_BYTES);
        /* Quirk Q-SYS-03: xn_sys_int24_installed is not cleared */
    }
    xn_restore_flags(flags);
}

/* ---- the divide-error handler ------------------------------------------------------------ */

void xn_sys_install_divide_handler(void)
{
    if (xn_sys_div_handler_installed == 1)
        return;
    xn_sys_div_handler_installed = 1;
    xn_dpmi_get_exception(0, &xn_sys_old_div_handler_offset, &xn_sys_old_div_handler_selector);
    xn_dpmi_install_exception(0, xn_sys_divide_error_entry);
    xn_mem_lock_region((void *)xn_sys_divide_error_entry,
                       (u32)(xn_sys_divide_error_end - (u8 *)xn_sys_divide_error_entry));
}

void xn_sys_remove_divide_handler(void)
{
    if (xn_sys_div_handler_installed == 0)
        return;
    xn_sys_div_handler_installed = 0;
    if (xn_sys_old_div_handler_selector != 0)
        xn_dpmi_set_exception(0, xn_sys_old_div_handler_offset,
                              xn_sys_old_div_handler_selector);
}

/* The length of the faulting divide (F6/F7 /6 or /7: div or idiv) by its ModRM byte, as far
   as the asm knows the forms: a disp32 operand 6, [reg + disp8] 3, [reg + disp32] 6, anything
   else 2 (a register operand, or [reg]). Quirk Q-SYS-02: the SIB forms (ModRM 34h, 3Ch, 74h,
   7Ch, B4h, BCh) count as 2, and a register operand's ModRM is not checked to be one. */
static u32 divide_length(u8 modrm)
{
    if (modrm == 0x35 || modrm == 0x3D)
        return 6;
    if (modrm >= 0x70 && modrm <= 0x7F && modrm != 0x74 && modrm != 0x7C)
        return 3;
    if (modrm >= 0xB0 && modrm <= 0xBF && modrm != 0xB4 && modrm != 0xBC)
        return 6;
    return 2;
}

void xn_sys_divide_error_handler(xn_int_frame *f)
{
    /* the host's frame starts above the EFLAGS the entry stub pushed first */
    xn_dpmi_exc_frame *x = (xn_dpmi_exc_frame *)(f->esp + 4);

    x->eip += divide_length(((const u8 *)(uptr)x->eip)[1]);
    f->eax = 0;                         /* Quirk Q-SYS-01: the quotient and remainder are 0 */
    f->edx = 0;
}

/* ---- the rest ------------------------------------------------------------------------------- */

void xn_sys_yield(void)
{
    xn_pc_yield();
}

void xn_sys_set_dos_transfer_buffer(void)
{
    u16 segment, selector;

    if (!xn_dpmi_dos_alloc(0xFFF, &segment, &selector))
        return;
    xn_sys_dos_transfer_selector = selector;
    xn_cw_set_transfer_buffer(segment, selector, 0xFFF0);
}

/* (linear 0: the real-mode interrupt vectors, which CauseWay maps at address 0) */
void xn_sys_zero_page_save(void)
{
    const volatile u32 *zero = (const volatile u32 *)DOS_LOW(0);    /* linear 0 */
    s32 k;

    for (k = 0; k < 256; k++)
        xn_zero_page_copy[k] = zero[k];
}

void xn_sys_zero_page_check(void)
{
    volatile u32 *zero = (volatile u32 *)DOS_LOW(0);
    s32 k;

    for (k = 0; k < 256; k++) {
        if (zero[k] == xn_zero_page_copy[k] || k == 0 || k == 1 || k == 8)
            continue;
        internal_check_failed = 4 * k;
        for (k = 0; k < 256; k++)
            zero[k] = xn_zero_page_copy[k];
        fatal_error(xn_msg_bad_zero_page);
        return;
    }
}

#ifdef DAGGER_PORT
/* DAGGER_PORT: the interrupt entries (as kbd.c's). The virtual PC raises neither: its DOS
   has no critical errors, and arm64 divides by zero to 0 without a trap (Q-SYS-01's answer). */
void xn_sys_crit_error_entry(void)
{
    xn_int_frame f = {0};

    xn_sys_crit_error_handler(&f);
}

void xn_sys_divide_error_entry(void)
{
}

u8 xn_sys_divide_error_end[1];
#endif
