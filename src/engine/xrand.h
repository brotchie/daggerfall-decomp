/* xrand.h: XnGine's random numbers (src/engine/rand.c; see xngine.h): a small LCG and the
   smooth value noise the game flickers its lights with. */
#ifndef XRAND_H
#define XRAND_H

#include "xngine.h"

/* The LCG's state (0x153800): starts at 1; the world code seeds it per cell */
extern u32 xn_rand_seed;

/* The noise lattice (0xC4100): 256 random values 0..255, the same 256 again, and the first
   once more, so that an index up to 512 needs no mask */
extern s32 xn_noise_values[513];

/* The smoothstep weights (0xC4904): xn_noise_fade_table[t + 256] for t = -256..255 */
extern s32 xn_noise_fade_table[512];

/* init_game_data: the fade table, then the lattice from the LCG seeded with 1. */
void xn_rand_noise_init(void);

/* The fade table: (768 - 2t) * t * t / 65536, rounded, for t = -256..255: the smoothstep
   3t^2 - 2t^3 with 256 = 1.0. */
void xn_rand_noise_fade_init(void);

/* Smooth 2D value noise 0..255 at (x, y), 8.8 fixed point: the lattice hashed at the integer
   parts, blended by the fade of the fractions. object_draw_cb and player_light_draw make the
   lights flicker with it. */
s32 xn_rand_noise_2d(u32 x, u32 y);

/* Dead: a lone ret after the noise. */
void xn_rand_noise_empty(void);
#pragma aux xn_rand_noise_empty parm [] modify exact [eax];

/* Dead: seeds the LCG from the BIOS tick count, (ticks & 0FFFh) + 1. Kept from the asm: it
   reads 0040:006C as the flat address 40006Ch, not 46Ch. */
void xn_rand_seed_from_ticks(void);
#pragma aux xn_rand_seed_from_ticks parm [] modify exact [eax];

/* The LCG: seed = seed * 797 mod 4099 (16-bit mul and div: the low word of the seed); returns
   the new seed. The world and terrain code, the snow and the stars. */
u32 xn_rand_next(void);

#endif
