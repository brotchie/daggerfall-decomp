/* xsys.h: XnGine's system services (src/engine/sys.c). Canonical C: plain prototypes, Watcom's
   own calling convention; docs/xngine_canonical.md.

   What it does
     The engine's two error handlers and their installers: a protected-mode DOS critical
     error handler (int 24h: every disk error is retried) and a DPMI divide-error handler
     (exception 0: a divide that overflows, or divides by 0, gives 0 and the program goes
     on; Q-SYS-01). Also the time-slice release main calls every frame, CauseWay's DOS
     transfer buffer, and the check of the real-mode interrupt vectors (the zero page) that
     the game's heap check runs.

   The handlers
     A vector holds a handler's interrupt entry (xn_sys_crit_error_entry,
     xn_sys_divide_error_entry): a stub the build makes at the handler's asm entry
     (tools/xn_rc.py isr_stub: pushfd; pushad; the call; popad; popfd; and the asm's own
     return, iretd or retf). It calls the C handler with the interrupted registers as it
     saved them (an xn_int_frame); what the handler changes there is what the interrupted
     code gets back.

   Globals (object 2, the engine's)
     xn_sys_old_int24_offset / _sel, xn_sys_int24_installed      the int 24h vector replaced;
     xn_sys_old_div_handler_offset / _selector, xn_sys_div_handler_installed
                                                                 exception 0's handler;
     xn_sys_dos_transfer_selector   the transfer buffer's selector;
     xn_zero_page_copy              linear 0..1023 as main started.
   The game's: internal_check_failed, fatal_error, dpmi_lock_region, dpmi_unlock_region.

   Quirks (docs/engine/quirks.md): Q-SYS-01 (a division that overflows gives 0), Q-SYS-02 (the
   divide-error handler does not know SIB forms), Q-SYS-03 (the restore leaves the
   critical error handler marked installed). */
#ifndef XSYS_H
#define XSYS_H

#include "xngine.h"

/* An interrupt or exception handler's view of the interrupted code: its registers as the
   handler's entry stub saved them (pushfd; pushad), which it restores when the handler
   returns. Above the saved EFLAGS lies what the CPU or the DPMI host pushed. */
typedef xn_regs xn_int_frame;

/* A DPMI 0.9 exception frame, as the host far-calls an exception handler */
typedef struct xn_dpmi_exc_frame {
    u32 ret_eip, ret_cs;                /* back to the host (retf) */
    u32 error;
    u32 eip, cs, eflags, esp, ss;       /* the faulting instruction's */
} xn_dpmi_exc_frame;

extern u16 xn_sys_old_int24_sel;            /* the int 24h vector before ours (0xC0400) */
extern u32 xn_sys_old_int24_offset;
extern u8 xn_sys_int24_installed;
extern u16 xn_sys_dos_transfer_selector;    /* the 64K DOS block (0xC5478) */
extern u32 xn_zero_page_copy[256];          /* linear 0..1023 as main started (0xCDE4F) */
extern char xn_msg_bad_zero_page[];         /* 'Bad zero page check (-----).' */
extern u32 xn_sys_old_div_handler_offset;   /* exception 0's handler before ours (0x149D80) */
extern u16 xn_sys_old_div_handler_selector;
extern u8 xn_sys_div_handler_installed;

/* The handlers' interrupt entries (the build's stubs at their asm entries: C0500, 149FC8),
   and the end of the divide handler's asm (14A0E5), which its install locks */
extern void xn_sys_crit_error_entry(void);
extern void xn_sys_divide_error_entry(void);
extern u8 xn_sys_divide_error_end[];

/* The game's */
extern u32 internal_check_failed;           /* not 0: fatal_error reports it */
s32 dpmi_lock_region(void *addr, u32 size);
s32 dpmi_unlock_region(void *addr, u32 size);
u32 fatal_error(const char *message);

/* ---- the critical error handler --------------------------------------------------------- */

/* The protected-mode int 24h (DOS critical error) handler: the answer is AL = 1, retry (the
   interrupted code gets EAX = 1). A vector. */
void xn_sys_crit_error_handler(xn_int_frame *f);

/* main, once: unless installed before, locks the handler's entry (1006h bytes: the asm's
   module), saves the int 24h vector and installs xn_sys_crit_error_entry, with interrupts
   off meanwhile (IF back as it was). */
void xn_sys_install_crit_error_handler(void);

/* shutdown_video: puts the saved int 24h vector back (interrupts off meanwhile) and unlocks
   the handler, when it was installed. Q-SYS-03: xn_sys_int24_installed stays 1. */
void xn_sys_restore_crit_error_handler(void);

/* ---- the divide-error handler ------------------------------------------------------------ */

/* xn_render_init: once, saves exception 0's handler (DPMI 0202h), installs
   xn_sys_divide_error_entry (0203h) and locks the handler's asm. */
void xn_sys_install_divide_handler(void);

/* xn_render_shutdown: puts the saved handler back (when there was one). */
void xn_sys_remove_divide_handler(void);

/* XnGine's divide error: the faulting div or idiv is stepped over (2, 3 or 6 bytes by its
   ModRM byte) and its quotient and remainder, EAX and EDX, become 0. The opcode is not
   looked at, nor any SIB byte (Q-SYS-02). The DPMI exception frame lies above the saved
   EFLAGS (f->esp + 4). The asm's 30 run-time blocks (149FD6..14A0CF) are this function's. */
void xn_sys_divide_error_handler(xn_int_frame *f);

/* ---- the rest ------------------------------------------------------------------------------- */

/* main, every frame: releases the time slice (int 2Fh 1680h). */
void xn_sys_yield(void);

/* init_video: allocates 0FFFh paragraphs of DOS memory, keeps its selector, and makes it
   CauseWay's DOS transfer buffer (0FFF0h bytes). Nothing when DOS memory is short. */
void xn_sys_set_dos_transfer_buffer(void);

/* main, at the start: keeps a copy of linear 0..1023 (the real-mode interrupt vectors). */
void xn_sys_zero_page_save(void);

/* mem_check_heap: compares linear 0..1023 with the copy, vectors 0, 1 and 8 (divide,
   single step, timer) excepted. On a difference: its byte offset becomes the failed internal
   check, the copy is put back whole, and fatal_error ('Bad zero page check (-----).'). */
void xn_sys_zero_page_check(void);

#endif
