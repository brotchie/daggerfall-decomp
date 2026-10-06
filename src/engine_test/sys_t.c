/* sys_t.c: test shims of src/engine/sys.c (built only by tools/xn_rc.py; docs/
   xngine_canonical.md). The handlers' vectors reach their C through the build's interrupt
   stubs, in the tests too; their shims only mark them canonical. */
#include "xsys.h"

void xn_sys_crit_error_handler_r(xn_regs *r)
{
    xn_sys_crit_error_handler(r);
}

void xn_sys_install_crit_error_handler_r(xn_regs *r)
{
    xn_sys_install_crit_error_handler();
}

void xn_sys_restore_crit_error_handler_r(xn_regs *r)
{
    xn_sys_restore_crit_error_handler();
}

void xn_sys_yield_r(xn_regs *r)
{
    xn_sys_yield();
}

void xn_sys_set_dos_transfer_buffer_r(xn_regs *r)
{
    xn_sys_set_dos_transfer_buffer();
}

void xn_sys_zero_page_save_r(xn_regs *r)
{
    xn_sys_zero_page_save();
}

void xn_sys_zero_page_check_r(xn_regs *r)
{
    xn_sys_zero_page_check();
}

void xn_sys_install_divide_handler_r(xn_regs *r)
{
    xn_sys_install_divide_handler();
}

void xn_sys_remove_divide_handler_r(xn_regs *r)
{
    xn_sys_remove_divide_handler();
}

void xn_sys_divide_error_handler_r(xn_regs *r)
{
    xn_sys_divide_error_handler(r);
}
