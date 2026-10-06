/* mem.c: XnGine's memory (canonical C; the interface and the module's documentation are in
   xmem.h). */
#include "xmem.h"
#include "xdos.h"
#include "xpc.h"
#include "xkbd.h"
#include "xjoy.h"
#include "xgfx.h"

#define WORK_MIN        0x10000         /* the work buffer's least size */
#define WORK_ALIGN      0x20

uptr xn_mem_align_up(uptr p, u32 align)
{
    return (p + align - 1) & ~(uptr)(align - 1);
}

/* The 'SYSTEM:' fatal error: msg ('$'-terminated) printed and the program ended */
static void sys_fatal(const char *msg)
{
    xn_dos_print(msg);
    xn_dos_exit(0);
}

void xn_mem_init(u32 size)
{
    u8 *block;
    u32 k;

    size += WORK_ALIGN;
    if ((s32)size < WORK_MIN)
        size = WORK_MIN;
    block = func_000A10A8(size);
    if (block == 0) {
        xn_kbd_remove();
        xn_joy_shutdown();
        xn_gfx_restore_mode();
        sys_fatal(xn_mem_msg_alloc_failed);
        return;                         /* (not reached) */
    }
    xn_mem_work_block = block;
    big_buffer = (u8 *)xn_mem_align_up((uptr)block, WORK_ALIGN);
    xn_dpmi_segment_selector(0x40, &xn_sel_bios_data);
    xn_dpmi_segment_selector(0xA000, &xn_sel_vga);
    xn_sys_psp_addr = (u32)xn_dos_psp() << 4;
    xn_sys_cmd_tail = xn_sys_psp_addr + 0x80;
    /* Quirk Q-MEM-01: entry 3FFh of both tables is left as it is */
    xn_recip16_table[0] = 0xFFFF;
    for (k = 1; k < 0x3FF; k++)
        xn_recip16_table[k] = 0xFFFF / k;
    xn_recip32_table[0] = 0xFFFFFFFF;
    for (k = 1; k < 0x3FF; k++)
        xn_recip32_table[k] = 0xFFFFFFFF / k;
    for (k = 0; k < 256; k++)
        xn_colour_fill_table[k] = k * 0x01010101;
    xn_mem_lock_region(xn_mem_game_lock_start, 0x400);
}

void xn_mem_shutdown(void)
{
    if (xn_mem_work_block != 0)
        func_000A117E(xn_mem_work_block);
}

void xn_mem_lock_region(const void *addr, u32 size)
{
    if (xn_dpmi_lock(addr, size))
        return;
    xn_kbd_remove();
    xn_joy_shutdown();
    xn_gfx_restore_mode();
    xn_mem_shutdown();
    sys_fatal(xn_mem_msg_lock_failed);
}
