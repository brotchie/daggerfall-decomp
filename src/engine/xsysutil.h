/* xsysutil.h: helpers the system group's C needs that belong in xngine.h (for the
   coordinator to promote). One-instruction inline functions, like xngine.h's. */
#ifndef XSYSUTIL_H
#define XSYSUTIL_H

#include "xngine.h"

/* Tail jumps. The asm dispatchers jump into a handler (jmp [table + i*4], or push and ret),
   so the handler runs on its caller's stack and returns straight to it. A C call would put
   one more frame under the handler, and the game's handlers can tell: click_activate stores
   the address of a local in click_hit. The function using one must have no frame of its own
   at that point (no locals, nothing saved: its pragma lets it change what it needs). */

/* jmp fn (EAX: fn) */
s32 xn_tail_jump(void *fn);
#pragma aux xn_tail_jump = "jmp eax" parm [eax] aborts modify exact [];

/* jmp fn with EAX = a (EDX: fn) */
s32 xn_tail_jump1(void *fn, s32 a);
#pragma aux xn_tail_jump1 = "jmp edx" parm [edx] [eax] aborts modify exact [];

/* jmp fn with EAX, EDX, EBX = a, b, c (ECX: fn) */
s32 xn_tail_jump3(void *fn, s32 a, s32 b, s32 c);
#pragma aux xn_tail_jump3 = "jmp ecx" parm [ecx] [eax] [edx] [ebx] aborts modify exact [];

/* FS = DS (push ds; pop fs): the asm's mouse code leaves FS so for its callers */
void xn_fs_from_ds(void);
#pragma aux xn_fs_from_ds = "push ds" "pop fs";

/* The flags: pushfd / popfd, cli / sti (the asm's handler installers keep IF as it was) */
u32 xn_save_flags(void);
#pragma aux xn_save_flags = "pushfd" "pop eax" value [eax] modify exact [eax];
void xn_restore_flags(u32 flags);
#pragma aux xn_restore_flags = "push eax" "popfd" parm [eax] modify exact [];
void xn_cli(void);
#pragma aux xn_cli = "cli" modify exact [];
void xn_sti(void);
#pragma aux xn_sti = "sti" modify exact [];

/* CS: the selector a handler is installed with */
u16 xn_cs(void);
#pragma aux xn_cs = "mov ax, cs" value [ax] modify exact [eax];

/* (n / d) | (n % d) << 8: an 8-bit divide (div with a byte register, as the asm's div dh)
   of a 16-bit n; after a divide error (d = 0) XnGine's handler leaves EAX and EDX 0 */
u16 xn_divb(u16 n, u8 d);
#pragma aux xn_divb = "div dl" parm [eax] [edx] value [ax] modify exact [eax edx];

/* The game's C library (Watcom's) */
/* _dos_getvect (0xA1272): the vector in DX:EAX (call it with xn_asmcall) */
void func_000A1272(void);
/* _dos_setvect (0xA12A6): vector intno = CX:EBX */
void func_000A12A6(u32 intno, void *handler, u32 sel);
#pragma aux func_000A12A6 parm [eax] [ebx] [ecx] modify [eax ebx ecx edx];
void *func_000A10A8(u32 size);              /* malloc */
void func_000A117E(void *block);            /* free */

#endif
