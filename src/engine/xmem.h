/* xmem.h: XnGine's memory (src/engine/mem.c). Canonical C: plain prototypes, Watcom's own
   calling convention; docs/xngine_canonical.md.

   What it does
     The engine's start-up memory: the work buffer every module borrows (big_buffer: the
     world's cell data, the texture unpacker, the scaled-image rows), the selectors of the
     BIOS data area and the VGA memory, the PSP's address, and three tables other modules
     read: 0FFFFh / i and 0FFFFFFFFh / i (the reciprocals the rasteriser multiplies by) and a
     colour in all four bytes of a dword (the fills). Also the DPMI lock every interrupt
     handler's code and data needs, and the work buffer's release.

   Globals (object 2): big_buffer (the work buffer, 32-byte aligned; the game reads it),
   xn_mem_work_block (as allocated), xn_sel_bios_data, xn_sel_vga, xn_sys_psp_addr and
   xn_sys_cmd_tail (nothing reads these two), xn_recip16_table, xn_recip32_table,
   xn_colour_fill_table, and the 'SYSTEM:' messages ('$'-terminated).

   Quirks (docs/engine/quirks.md): Q-MEM-01 (the reciprocal tables' last entry is never
   written). */
#ifndef XMEM_H
#define XMEM_H

#include "xngine.h"
#include "ptrint.h"                     /* uptr: an int that holds an address */

extern u8 *big_buffer;                      /* the work buffer, 32-byte aligned (0x147954) */
extern u8 *xn_mem_work_block;               /* the block as allocated (0x147958) */
extern u16 xn_sel_bios_data;                /* selector of real-mode segment 40h */
extern u16 xn_sel_vga;                      /* selector of real-mode segment A000h */
extern u32 xn_sys_psp_addr;                 /* the PSP's linear address (nothing reads it) */
extern u32 xn_sys_cmd_tail;                 /* and its command tail's (nor this) */
extern u32 xn_recip16_table[1024];          /* 0FFFFh / i ([0] = 0FFFFh) */
extern u32 xn_recip32_table[1024];          /* 0FFFFFFFFh / i ([0] = 0FFFFFFFFh) */
extern u32 xn_colour_fill_table[256];       /* i * 01010101h: a colour in all four bytes */
extern char xn_mem_msg_alloc_failed[];      /* 'SYSTEM: Unable to allocate workMem.$' */
extern char xn_mem_msg_lock_failed[];       /* 'SYSTEM: Unable to Lock Memory Region.$' */
extern u8 xn_mem_game_lock_start[];         /* the game's code the start-up locks (0xA12B8) */

/* The game's allocator (Watcom's runtime in object 1): malloc and free */
void *func_000A10A8(u32 size);
void func_000A117E(void *block);

/* (p + align - 1) rounded down to a multiple of align (a power of 2). init_game_data and
   color_init_remap_tables align their 256-byte tables (water.tbl, haze.000/001): p is an
   address (uptr). */
uptr xn_mem_align_up(uptr p, uptr align);

/* init_video: the work buffer (size + 32 bytes, at least 64K: its start aligned to 32 bytes;
   without the memory: the keyboard, joystick and video put back, 'SYSTEM: Unable to
   allocate workMem.' and the end), selectors for segments 40h and A000h, the PSP's address,
   the reciprocal and colour-fill tables; then locks 400h bytes of the game's code at
   0xA12B8. */
void xn_mem_init(u32 size);

/* game_shutdown and the fatal paths: frees the work buffer if there is one. */
void xn_mem_shutdown(void);

/* Locks size bytes at addr (DPMI 0600h). A failure ends the program: the keyboard, joystick
   and video put back, the work buffer freed, 'SYSTEM: Unable to Lock Memory Region.'. The
   interrupt handlers and their data, and the game's code above. */
void xn_mem_lock_region(const void *addr, u32 size);

#endif
