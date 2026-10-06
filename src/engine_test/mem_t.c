/* mem_t.c: test shims of src/engine/mem.c (built only by tools/xn_rc.py; docs/
   xngine_canonical.md). */
#include "xmem.h"

void xn_mem_align_up_r(xn_regs *r)
{
    r->eax = xn_mem_align_up(r->eax, r->edx);
}

/* size EAX; every register kept */
void xn_mem_init_r(xn_regs *r)
{
    xn_mem_init(r->eax);
}

void xn_mem_shutdown_r(xn_regs *r)
{
    xn_mem_shutdown();
}

/* addr EAX, size EDX; every register kept */
void xn_mem_lock_region_r(xn_regs *r)
{
    xn_mem_lock_region((const void *)r->eax, r->edx);
}
