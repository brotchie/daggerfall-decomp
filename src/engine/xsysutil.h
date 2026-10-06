/* xsysutil.h: CPU intrinsics the system and input groups' C needs, beside xngine.h's (for the
   coordinator to promote there). Each is one or two instructions, inline (a `#pragma aux`
   code sequence), like xngine.h's 64-bit products: compiler support, not an engine
   interface. */
#ifndef XSYSUTIL_H
#define XSYSUTIL_H

#include "xngine.h"

/* ---- guaranteed tail calls ------------------------------------------------------------- */
/* XnGine's dispatchers jump into the handler (jmp [table + i*4], or push and ret), so the
   handler runs at its caller's stack depth and returns straight to it. A C call would run
   it a frame deeper, and the game can tell: its handlers store the addresses of their locals
   in globals the game reads (click_activate in click_hit). Watcom C32 10.0a never turns an
   indirect call into a jump, so these do: `return xn_tail_jump(fn)` in a function with no
   frame of its own. Each takes fn and the handler's arguments in the registers its
   dispatcher already holds them in, so Watcom needs no other register (a saved one would be
   left on the stack under the handler's return address). */

#ifndef DAGGER_PORT
/* jmp fn: fn() (fn in EAX) */
s32 xn_tail_jump(void *fn);
#pragma aux xn_tail_jump = "jmp eax" parm [eax] aborts modify exact [];

/* fn(1): push fn; EAX = 1; ret (fn in EAX) */
s32 xn_tail_jump_1(void *fn);
#pragma aux xn_tail_jump_1 = "push eax" "mov eax, 1" "ret" parm [eax] aborts modify exact [];

/* fn(a, b, c) with fn in EAX and a, b, c in EDX, EBX, ECX (a dispatcher's second to fourth
   arguments): push fn; EAX, EDX, EBX = a, b, c; ret */
s32 xn_tail_jump3(void *fn, s32 a, s32 b, s32 c);
#pragma aux xn_tail_jump3 = "push eax" "mov eax, edx" "mov edx, ebx" "mov ebx, ecx" "ret" \
    parm [eax] [edx] [ebx] [ecx] aborts modify exact [];

/* ---- the interrupt flag ------------------------------------------------------------------ */
/* XnGine's installers run with interrupts off and then put IF back as it was */
u32 xn_save_flags(void);
#pragma aux xn_save_flags = "pushfd" "pop eax" value [eax] modify exact [eax];
void xn_restore_flags(u32 flags);
#pragma aux xn_restore_flags = "push eax" "popfd" parm [eax] modify exact [];
void xn_cli(void);
#pragma aux xn_cli = "cli" modify exact [];
void xn_sti(void);
#pragma aux xn_sti = "sti" modify exact [];

#else
/* The native build (docs/port.md): a dispatcher's jump is a call (the handler runs a frame
   deeper, which nothing native depends on); the interrupt flag is the virtual PC's interrupt
   lock (port/include/port_vpc.h: cli and sti nest), and IF in the saved flags says whether it
   was free. */
#include "ptrint.h"
void port_cli(void);
void port_sti(void);
int port_cli_depth(void);
static __inline__ s32 xn_tail_jump(void *fn) { return ((s32 (*)(void))fn)(); }
static __inline__ s32 xn_tail_jump_1(void *fn) { return ((s32 (*)(s32))fn)(1); }
static __inline__ s32 xn_tail_jump3(void *fn, iptr a, iptr b, iptr c)
{
    return ((s32 (*)(iptr, iptr, iptr))fn)(a, b, c);
}
static __inline__ u32 xn_save_flags(void) { return port_cli_depth() ? 0 : 0x200; }
static __inline__ void xn_restore_flags(u32 flags)
{
    if (flags & 0x200) {
        while (port_cli_depth() > 0)
            port_sti();
    } else if (port_cli_depth() == 0) {
        port_cli();
    }
}
static __inline__ void xn_cli(void) { port_cli(); }
static __inline__ void xn_sti(void) { port_sti(); }
#endif

#endif
