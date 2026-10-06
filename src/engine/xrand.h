/* xrand.h: XnGine's random numbers (src/engine/rand.c). Canonical C: plain prototypes,
   Watcom's own calling convention; docs/xngine_canonical.md.

   What it does
     A small linear congruential generator (seed = seed * 797 mod 4099: the world's terrain
     and nature, the sky's stars, rain and snow, the water) and a smooth 2D value noise the
     game flickers its lights with (a lattice of 256 random bytes blended by a smoothstep).

   Fixed point
     The noise takes 8.8 fixed-point coordinates (the integer part picks the lattice cell, the
     fraction the blend) and gives 0..255. The fade table is the smoothstep 3t^2 - 2t^3 with
     256 = 1.0.

   Globals (object 2): xn_rand_seed (the generator's state; the world seeds it per cell),
   xn_noise_values (the lattice: 256 values, the same 256 again and the first once more, so an
   index up to 512 needs no mask), xn_noise_fade_table.

   Quirks (docs/engine/quirks.md): Q-RAND-01 (the tick seed reads the BIOS tick count at the
   wrong address). */
#ifndef XRAND_H
#define XRAND_H

#include "xngine.h"

/* The generator's state (0x153800): starts at 1 */
extern u32 xn_rand_seed;

/* The noise lattice (0xC4100): 256 random values 0..255, the same 256 again, the first once
   more */
extern s32 xn_noise_values[513];

/* The smoothstep weights (0xC4904): xn_noise_fade_table[t + 256] for t = -256..255 */
extern s32 xn_noise_fade_table[512];

/* init_game_data: the fade table, then the lattice from the generator seeded with 1. */
void xn_rand_noise_init(void);

/* The fade table: (768 - 2t) * t * t / 65536, rounded, for t = -256..255. */
void xn_rand_noise_fade_init(void);

/* Smooth 2D value noise 0..255 at (x, y), 8.8 fixed point: the lattice hashed at the integer
   parts (x's not masked: the sums are), blended by the fade of the fractions (arithmetic
   shifts). object_draw_cb and player_light_draw make the lights flicker with it (6 game
   sites). */
s32 xn_rand_noise_2d(u32 x, u32 y);

/* Dead: an empty function after the noise. */
void xn_rand_noise_empty(void);

/* Dead: seeds the generator from the BIOS tick count, (ticks & 0FFFh) + 1. Q-RAND-01: it reads
   the flat address 40006Ch (0040:006C taken as linear), not 46Ch. */
void xn_rand_seed_from_ticks(void);

/* The generator: seed = (seed's low word) * 797 mod 4099; returns the new seed (0..4098). The
   world and terrain code, the snow, the rain and the stars. */
u32 xn_rand_next(void);

#endif
