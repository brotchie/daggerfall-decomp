/* xnsmc.h: helpers for the self-modifying, unrolled and generated code of XnGine (the designs
   in docs/engine/smc/index.md, "Helpers to add to xngine.h"). Written by the raster
   group (span, shade, tmap, light); the coordinator may promote them into xngine.h. */
#ifndef XNSMC_H
#define XNSMC_H

#include "xngine.h"

/* (a * b) >> 32, unsigned (mul: the light shaders' squared distance times the falloff) */
u32 xn_umulhi(u32 a, u32 b);
#pragma aux xn_umulhi = "mul edx" parm [eax] [edx] value [edx] modify [eax];

/* (hi:lo) / d, unsigned (div ecx). A quotient that does not fit, or d = 0, raises a divide
   error: XnGine's handler steps over the 2-byte instruction and gives 0 (rule DIV). */
u32 xn_udiv64(u32 hi, u32 lo, u32 d);
#pragma aux xn_udiv64 = "div ecx" parm [edx] [eax] [ecx] value [eax] modify [edx];

/* (hi:lo) / d, signed (idiv ecx); the same divide-error rule */
s32 xn_idiv64(s32 hi, u32 lo, s32 d);
#pragma aux xn_idiv64 = "idiv ecx" parm [edx] [eax] [ecx] value [eax] modify [edx];

/* the bytes of v reversed (bswap eax, written as its bytes) */
u32 xn_bswap(u32 v);
#pragma aux xn_bswap = 0x0F 0xC8 parm [eax] value [eax];

/* whether a + b is negative without the wrap: the sign `jl`/`jge` test after `add a, b`
   (SF != OF), which differs from the wrapped sum's sign when the add overflows */
int xn_add_lt0(s32 a, s32 b);
#pragma aux xn_add_lt0 = "add eax, edx" "setl al" "movzx eax, al" parm [eax] [edx] value [eax];

/* rule KEEP: a function stores one of its own operands only to use it later in the same call;
   the C keeps the value in a local and stores the field once, for the records */
#define XN_KEEP(field, v)   ((field) = (v))

/* a dword into code or data by address (generators patching a template) */
#define XN_PUT32(p, v)      (*(u32 *)(p) = (u32)(v))

/* the asm entry of a function, as a code address (declare `extern void asm_NAME(void);`):
   for routine pointers the asm compares or stores (span routines, the fog hook, the
   frame's sentinel) */
#define XN_ASM(name)        ((void (*)(void))asm_##name)

/* rule RET: the count a planted ret gives an unrolled body of `max` steps of `stride` bytes:
   the step whose first byte is C3h, or max when none is (the body runs to its own ret) */
int xn_planted_count(const void *body, int stride, int max);

#endif
