/* xnsmc.h: the rasteriser's arithmetic helpers (src/engine/smc.c): 64-by-32 divides that do
   not fault, the unsigned high product and the sign of an unwrapped sum. Candidates for
   xngine.h.

   The asm divided where a quotient could overflow (or a divisor be 0) and let XnGine's
   divide-error handler make the result 0 (docs/engine/quirks.md Q-SYS-01); the _or0 forms
   give that 0 without the exception. */
#ifndef XNSMC_H
#define XNSMC_H

#include "xngine.h"

/* (a * b) >> 32, unsigned (the asm's mul: the light shaders' squared distance times the
   falloff) */
u32 xn_umulhi(u32 a, u32 b);
#pragma aux xn_umulhi = "mul edx" parm [eax] [edx] value [edx] modify [eax];

/* (hi:lo) / d, unsigned: 0 when d is 0 or the quotient needs more than 32 bits (hi >= d) */
u32 xn_udiv64_or0(u32 hi, u32 lo, u32 d);

/* (hi:lo) / d, signed and truncated: 0 when d is 0 or the quotient does not fit in 32 bits */
s32 xn_idiv64_or0(s32 hi, u32 lo, s32 d);

/* 1 when a + b is negative in exact arithmetic: the sign the asm's jl / jge test after
   `add a, b` (SF != OF), which differs from the wrapped sum's when the add overflows */
int xn_add_lt0(s32 a, s32 b);

/* The readable C's faulting forms (div ecx, idiv ecx on edx:eax), for code not yet
   canonical: a quotient that does not fit raises XnGine's divide error (0 after it). */
u32 xn_udiv64(u32 hi, u32 lo, u32 d);
#pragma aux xn_udiv64 = "div ecx" parm [edx] [eax] [ecx] value [eax] modify [edx];
s32 xn_idiv64(s32 hi, u32 lo, s32 d);
#pragma aux xn_idiv64 = "idiv ecx" parm [edx] [eax] [ecx] value [eax] modify [edx];

#endif
