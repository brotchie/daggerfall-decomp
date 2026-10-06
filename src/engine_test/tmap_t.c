/* tmap_t.c: test shims of src/engine/tmap.c (built only by tools/xn_rc.py; see
   src/engine_test/vec_t.c and docs/xngine_canonical.md). Each NAME_r maps the registers of
   NAME's asm entry (config/xngine_abi.csv) to the canonical C call. */
#include "xtmap.h"

void xn_tmap_pool_alloc_r(xn_regs *r)
{
    (void)r;
    xn_tmap_pool_alloc();
}

void xn_tmap_pool_free_r(xn_regs *r)
{
    (void)r;
    xn_tmap_pool_free();
}

/* EAX = the texture record's entry, EDX = its size masks -> EAX = the handle, CF when the
   pool is full */
void xn_tmap_compile_r(xn_regs *r)
{
    void *h = xn_tmap_compile((const struct xn_tex_entry *)r->eax, r->edx);

    r->eax = (u32)h;
    XN_SETFLAG(r, XN_CF, h == 0);
}

void xn_tmap_rebase_r(xn_regs *r)
{
    xn_tmap_rebase((void *)r->eax, (u8 *)r->edx);
}

void xn_tmap_pool_reset_r(xn_regs *r)
{
    (void)r;
    xn_tmap_pool_reset();
}
