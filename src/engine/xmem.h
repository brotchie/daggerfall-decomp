/* xmem.h: XnGine's memory (src/engine/mem.c; see xngine.h): the work buffer, DPMI locks, the
   reciprocal and colour-fill tables. */
#ifndef XMEM_H
#define XMEM_H

#include "xngine.h"
#include "xsysutil.h"

extern u8 *big_buffer;                      /* the work buffer, 32-byte aligned (0x147954) */
extern u8 *xn_mem_work_block;               /* the block as allocated (0x147958) */
extern u16 xn_sel_bios_data;                /* selector of real-mode segment 40h */
extern u16 xn_sel_vga;                      /* selector of real-mode segment A000h */
extern u32 xn_sys_psp_addr;                 /* the PSP's linear address (nothing reads it) */
extern u32 xn_sys_cmd_tail;                 /* and its command tail's (nor this) */
extern u32 xn_recip16_table[1024];          /* 0xFFFF / i ([0] = 0xFFFF) */
extern u32 xn_recip32_table[1024];          /* 0xFFFFFFFF / i ([0] = 0xFFFFFFFF) */
extern u32 xn_colour_fill_table[256];       /* i * 01010101h: a colour in all four bytes */
extern char xn_mem_msg_alloc_failed[];      /* 'SYSTEM: Unable to allocate workMem.$' */
extern char xn_mem_msg_lock_failed[];       /* 'SYSTEM: Unable to Lock Memory Region.$' */
extern u8 xn_code_0A12B8[];                 /* the game's code that xn_mem_init locks */

/* Group 5's xn_gfx_restore_mode, through its asm entry: it keeps every register */
void asm_xn_gfx_restore_mode(void);
#pragma aux asm_xn_gfx_restore_mode parm [] modify exact [];
/* (p + align - 1) rounded down to a multiple of align (a power of 2). init_game_data and
   color_init_remap_tables align their 256-byte tables (water.tbl, haze.000/001). */
u32 xn_mem_align_up(u32 p, u32 align);

/* init_video (102400): the work buffer (size + 32 bytes, at least 64K, with the game's
   malloc; failure: 'SYSTEM: Unable to allocate workMem.' and exit), selectors for segments
   40h and A000h, the PSP address, the reciprocal and colour-fill tables; then locks 400h
   bytes of the game's code at 0xA12B8. */
void xn_mem_init(u32 size);
#pragma aux xn_mem_init parm [eax] modify exact [eax edx ebx];

/* game_shutdown and the fatal paths: frees the work buffer if there is one. (Its flags are
   free's: see the ABI override.) */
void xn_mem_shutdown(void);
#pragma aux xn_mem_shutdown parm [] modify exact [eax edx];

/* Locks size bytes at addr (DPMI 0600h). On failure: the keyboard, joystick and video put
   back, the work buffer freed, 'SYSTEM: Unable to Lock Memory Region.' and exit. The int 9 and
   divide handlers, the joystick's tick handler and the game code above. */
void xn_mem_lock_region(void *addr, u32 size);
#pragma aux xn_mem_lock_region parm [eax] [edx] modify exact [eax ecx edx ebx esi edi];

#endif
