/* mem.c: XnGine's memory as readable C (xmem.h; see xngine.h). */
#include "xmem.h"
#include "xkbd.h"
#include "xjoy.h"

u32 xn_mem_align_up(u32 p, u32 align)
{
    return (p + align - 1) & ~(align - 1);
}

/* 'SYSTEM:' errors: the message (AX's AH = 9 put into eax), then exit with AX = 4C00h */
static void sys_fatal(u32 eax, const char *msg)
{
    xn_regs d;

    d.eax = (eax & 0xFFFF00FF) | 0x0900;
    d.edx = (u32)msg;
    xn_int21(&d);
    d.eax = (d.eax & 0xFFFF0000) | 0x4C00;
    xn_int21(&d);
}

void xn_mem_init(u32 size)
{
    xn_regs d;
    u8 *block;
    u32 k;

    size += 0x20;
    if ((s32)size < 0x10000)
        size = 0x10000;
    block = func_000A10A8(size);
    if (block == 0) {
        xn_kbd_remove();
        xn_joy_shutdown();
        asm_xn_gfx_restore_mode();
        sys_fatal(0, xn_mem_msg_alloc_failed);
        return;                         /* (not reached) */
    }
    xn_mem_work_block = block;
    big_buffer = (u8 *)(((u32)block + 0x1F) & ~0x1Fu);
    /* DPMI 0002h, segment to selector; the upper half of EAX: what it held before */
    d.eax = ((u32)big_buffer & 0xFFFF0000) | 2;
    d.ebx = 0x40;
    xn_int31(&d);
    xn_sel_bios_data = (u16)d.eax;
    d.eax = (d.eax & 0xFFFF0000) | 2;
    d.ebx = (d.ebx & 0xFFFF0000) | 0xA000;
    xn_int31(&d);
    xn_sel_vga = (u16)d.eax;
    d.eax = (d.eax & 0xFFFF00FF) | 0x5100;  /* the PSP's segment in BX */
    xn_int21(&d);
    xn_sys_psp_addr = (u16)d.ebx << 4;
    xn_sys_cmd_tail = xn_sys_psp_addr + 0x80;
    xn_recip16_table[0] = 0xFFFF;
    for (k = 1; k < 0x3FF; k++)         /* (the last entry, 3FFh, is left as it is) */
        xn_recip16_table[k] = 0xFFFF / k;
    xn_recip32_table[0] = 0xFFFFFFFF;
    for (k = 1; k < 0x3FF; k++)
        xn_recip32_table[k] = 0xFFFFFFFF / k;
    for (k = 0; k < 256; k++)
        xn_colour_fill_table[k] = k * 0x01010101;
    xn_mem_lock_region(xn_code_0A12B8, 0x400);
}

void xn_mem_shutdown(void)
{
    if (xn_mem_work_block != 0)
        func_000A117E(xn_mem_work_block);
}

void xn_mem_lock_region(void *addr, u32 size)
{
    xn_regs d;

    d.eax = 0x600;
    d.ebx = (u32)addr >> 16;
    d.ecx = (u32)addr & 0xFFFF;
    d.esi = size >> 16;
    d.edi = size & 0xFFFF;
    xn_int31(&d);
    if (!(d.eflags & XN_CF))
        return;
    xn_kbd_remove();                    /* (these keep AL and the upper half of EAX) */
    xn_joy_shutdown();
    asm_xn_gfx_restore_mode();
    xn_mem_shutdown();
    sys_fatal(d.eax, xn_mem_msg_lock_failed);
}
