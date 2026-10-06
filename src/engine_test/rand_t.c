/* rand_t.c: test shims of src/engine/rand.c (built only by tools/xn_rc.py; docs/
   xngine_canonical.md). */
#include "xrand.h"

void xn_rand_noise_init_r(xn_regs *r)
{
    xn_rand_noise_init();
}

void xn_rand_noise_fade_init_r(xn_regs *r)
{
    xn_rand_noise_fade_init();
}

void xn_rand_noise_2d_r(xn_regs *r)
{
    r->eax = xn_rand_noise_2d(r->eax, r->edx);
}

void xn_rand_noise_empty_r(xn_regs *r)
{
    xn_rand_noise_empty();
}

/* (every register kept) */
void xn_rand_seed_from_ticks_r(xn_regs *r)
{
    xn_rand_seed_from_ticks();
}

void xn_rand_next_r(xn_regs *r)
{
    r->eax = xn_rand_next();
}
