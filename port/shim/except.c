/* except.c: MemCheck's processor-exception hook (docs/state.md, "The library region": the
   XXDEF.C exception dumper). FALL.EXE keeps the handler in 0x18929C (by default 0xA2D9E);
   0xBAE85, the DPMI exception handler, saves the registers and calls it before chaining to
   the extender's handler. main installs crash_screen (jmem.c), which ends by calling the
   default.

   The native build installs no exception handlers: registration only keeps the handler. */
#include <stdio.h>

typedef void (*mc_exception_handler)(void);

void func_000A2D9E(void);

static mc_exception_handler handler = func_000A2D9E;

/* mc_set_exception_handler (0x9DB3F): sets the handler, returns the previous one. While
   MemCheck is active (0x188A44 bit 0, never set here) it also hooks the DPMI exceptions
   (0xBAF6D) or, given NULL, unhooks them (0xBAE22). */
mc_exception_handler func_0009DB3F(mc_exception_handler h)
{
    mc_exception_handler old = handler;

    handler = h;
    return old;
}

/* MemCheck's default exception report (0xA2D9E, XXDEF.C): "MemCheck detected an EXCEPTION at
   %04X:%08X. Error code: %04Xh", CS:IP, where the last checked call was, and a stack trace,
   all through mc_debugf and MemCheck's output (silent while MemCheck is inactive); then,
   always, printf("%s\n", "\nChaining to default exception handler... \n"). There are no
   registers to report natively. */
void func_000A2D9E(void)
{
    printf("%s\n", "\nChaining to default exception handler... \n");
}
