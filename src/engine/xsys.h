/* xsys.h: XnGine's system services (src/engine/sys.c; see xngine.h): the critical error and
   divide error handlers and their installers, the time-slice yield, CauseWay's DOS transfer
   buffer, and the zero-page check. */
#ifndef XSYS_H
#define XSYS_H

#include "xngine.h"
#include "xsysutil.h"

extern u16 xn_sys_old_int24_sel;            /* the int 24h vector before ours (0xC0400) */
extern u32 xn_sys_old_int24_offset;
extern u8 xn_sys_int24_installed;
extern u16 xn_sys_dos_transfer_selector;    /* the 64K DOS block (0xC5478) */
extern u32 xn_zero_page_copy[256];          /* linear 0..1023 as main started (0xCDE4F) */
extern char xn_msg_bad_zero_page[];         /* 'Bad zero page check (-----).' */
extern u32 xn_sys_old_div_handler_offset;   /* exception 0's handler before ours (0x149D80) */
extern u16 xn_sys_old_div_handler_selector;
extern u8 xn_sys_div_handler_installed;
extern u8 xn_code_14A0E5[];                 /* the end of the divide handler's code */

/* The game's */
extern u32 internal_check_failed;
s32 dpmi_lock_region(void *addr, u32 size);
s32 dpmi_unlock_region(void *addr, u32 size);
u32 fatal_error(const char *message);

/* The handlers' asm entries (their routes take them to the C): what the vectors hold */
void asm_xn_sys_crit_error_handler(void);
void asm_xn_sys_divide_error_handler(void);

/* The protected-mode int 24h (DOS critical error) handler: AL = 1, retry (EAX = 1). */
void xn_sys_crit_error_handler_r(xn_regs *r);

/* main, once: unless done before, locks the handler, saves the int 24h vector and installs
   xn_sys_crit_error_handler (interrupts off meanwhile; IF back as it was). */
void xn_sys_install_crit_error_handler(void);
#pragma aux xn_sys_install_crit_error_handler parm [] modify exact [eax edx];

/* shutdown_video: puts the saved int 24h vector back and unlocks the handler. Kept from the
   asm: xn_sys_int24_installed stays 1. */
void xn_sys_restore_crit_error_handler(void);
#pragma aux xn_sys_restore_crit_error_handler parm [] modify exact [eax ecx edx ebx esi edi];

/* main, every frame: releases the time slice (int 2Fh AX = 1680h, the upper half of EAX as
   the caller left it); the registers as the host leaves them. */
void xn_sys_yield_r(xn_regs *r);

/* init_video: allocates 0FFFh paragraphs of DOS memory (DPMI 0100h), keeps its selector, and
   makes it CauseWay's DOS transfer buffer (int 31h FF26h, 0FFF0h bytes). */
void xn_sys_set_dos_transfer_buffer(void);
#pragma aux xn_sys_set_dos_transfer_buffer parm [] modify exact [eax edx];

/* main, at the start: keeps a copy of linear 0..1023 (the real-mode interrupt vectors). */
void xn_sys_zero_page_save(void);
#pragma aux xn_sys_zero_page_save parm [] modify exact [eax];

/* mem_check_heap: compares linear 0..1023 with the copy, vectors 0, 1 and 8 excepted. On a
   difference: its offset becomes the failed internal check, the copy is put back, and
   fatal_error ('Bad zero page check'). */
void xn_sys_zero_page_check(void);
#pragma aux xn_sys_zero_page_check parm [] modify exact [eax edx];

/* xn_render_init: once, saves exception 0's handler (DPMI 0202h), installs
   xn_sys_divide_error_handler (0203h) and locks its code. */
void xn_sys_install_divide_handler(void);
#pragma aux xn_sys_install_divide_handler parm [] modify exact [eax ecx edx ebx edi];

/* xn_render_shutdown: puts the saved handler back (when there was one). */
void xn_sys_remove_divide_handler(void);
#pragma aux xn_sys_remove_divide_handler parm [] modify exact [eax];

/* A DPMI 0.9 exception frame, as the host far-calls a handler */
typedef struct xn_dpmi_exc_frame {
    u32 ret_eip, ret_cs;                    /* back to the host (retf) */
    u32 error;
    u32 eip, cs, eflags, esp, ss;           /* the faulting instruction's */
} xn_dpmi_exc_frame;

/* XnGine's divide error: the faulting div or idiv is stepped over (2, 3 or 6 bytes by its
   ModRM) and its quotient and remainder (EAX, EDX) become 0. A run of compares in the asm.
   Kept from it: no SIB form (ModRM 34h, 3Ch, 74h, 7Ch, B4h, BCh) is known, so one is stepped
   over as 2 bytes, into the middle of the instruction; and the opcode is not looked at. */
void xn_sys_divide_error_handler(xn_dpmi_exc_frame *f);
void xn_sys_divide_error_handler_r(xn_regs *r);

#endif
