/* rand.c: XnGine's random numbers (canonical C; the interface and the module's documentation
   are in xrand.h). */
#include "xrand.h"

#define RAND_MUL        797
#define RAND_MOD        4099

void xn_rand_noise_init(void)
{
    s32 k, v;

    xn_rand_noise_fade_init();
    xn_rand_seed = 1;
    for (k = 0; k < 256; k++) {
        v = xn_rand_next() & 0xFF;      /* (the asm then halves values above 255: none are) */
        xn_noise_values[k] = v;
        xn_noise_values[256 + k] = v;
    }
    xn_noise_values[512] = xn_noise_values[0];
}

void xn_rand_noise_fade_init(void)
{
    s32 t;

    for (t = -256; t < 256; t++)
        xn_noise_fade_table[t + 256] = ((0x300 - 2 * t) * (t * t) + 0x8000) >> 16;
}

/* a + (b - a) * w / 256, with the asm's arithmetic shift */
static s32 lerp8(s32 a, s32 b, s32 w)
{
    return ((b - a) * w >> 8) + a;
}

s32 xn_rand_noise_2d(u32 x, u32 y)
{
    s32 fx = xn_noise_fade_table[(x & 0xFF) + 256];
    s32 fy = xn_noise_fade_table[(y & 0xFF) + 256];
    u32 ix = x >> 8;
    u32 iy = (y >> 8) & 0xFF;
    u32 h0 = (ix + xn_noise_values[iy]) & 0xFF;
    u32 h1 = (ix + xn_noise_values[iy + 1]) & 0xFF;
    s32 top = lerp8(xn_noise_values[h0], xn_noise_values[h0 + 1], fx);
    s32 bottom = lerp8(xn_noise_values[h1], xn_noise_values[h1 + 1], fx);

    return lerp8(top, bottom, fy);
}

void xn_rand_noise_empty(void)
{
}

void xn_rand_seed_from_ticks(void)
{
    /* Quirk Q-RAND-01: 0040:006C as a flat address (the tick count is at 46Ch) */
    xn_rand_seed = (*(volatile u32 *)0x40006C & 0xFFF) + 1;
}

u32 xn_rand_next(void)
{
    xn_rand_seed = (xn_rand_seed & 0xFFFF) * RAND_MUL % RAND_MOD;
    return xn_rand_seed;
}
