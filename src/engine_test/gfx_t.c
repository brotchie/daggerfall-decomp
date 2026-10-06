/* gfx_t.c: test shims of src/engine/gfx.c (built only by tools/xn_rc.py; see
   src/engine_test/vec_t.c and docs/xngine_canonical.md). Each maps its asm entry's registers
   (config/xngine_abi.csv) to the C call and back. */
#include "xgfx.h"

void xn_gfx_wait_vretrace_start_r(xn_regs *r)
{
    xn_gfx_wait_vretrace_start();
}

void xn_gfx_wait_vretrace_end_r(xn_regs *r)
{
    xn_gfx_wait_vretrace_end();
}

void xn_gfx_wait_vretraces_r(xn_regs *r)
{
    xn_gfx_wait_vretraces(r->eax);
}

void xn_gfx_present_inclusive_r(xn_regs *r)
{
    xn_gfx_present_inclusive(r->eax);
}

void xn_gfx_present_r(xn_regs *r)
{
    xn_gfx_present(r->eax);
}

void xn_gfx_clear_r(xn_regs *r)
{
    xn_gfx_clear(r->eax);
}

void xn_gfx_drv0_present_r(xn_regs *r)
{
    xn_gfx_drv0_present(r->eax);
}

/* colour AL */
void xn_gfx_drv0_clear_r(xn_regs *r)
{
    xn_gfx_drv0_clear((u8)r->eax);
}

void xn_gfx_drv0_shutdown_r(xn_regs *r)
{
    xn_gfx_drv0_shutdown();
}

/* n ECX, src ESI, dst EDI */
void xn_gfx_copy_rows_r(xn_regs *r)
{
    xn_gfx_copy_rows((u8 *)r->edi, (const u8 *)r->esi, r->ecx);
}

void xn_gfx_copy_and_clear_r(xn_regs *r)
{
    xn_gfx_copy_and_clear((u8 *)r->edi, (u8 *)r->esi, r->ecx);
}

void xn_gfx_copy_and_clear_unrolled_r(xn_regs *r)
{
    xn_gfx_copy_and_clear_unrolled((u32 *)r->edi, (u32 *)r->esi, r->ecx);
}

/* mode EAX, present mode DL -> 0 or 1 in EAX */
void xn_gfx_set_mode_r(xn_regs *r)
{
    r->eax = xn_gfx_set_mode(r->eax, (u8)r->edx);
}

void xn_gfx_restore_mode_r(xn_regs *r)
{
    xn_gfx_restore_mode();
}

/* mode EAX -> 0 or 1 in EAX, CF when not set */
void xn_gfx_change_mode_r(xn_regs *r)
{
    r->eax = xn_gfx_change_mode(r->eax);
    XN_SETFLAG(r, XN_CF, r->eax != 0);
}

void xn_gfx_set_mode13_r(xn_regs *r)
{
    xn_gfx_set_mode13();
}

void xn_gfx_build_row_offsets_r(xn_regs *r)
{
    xn_gfx_build_row_offsets();
}

void xn_gfx_set_clip_r(xn_regs *r)
{
    xn_gfx_set_clip(r->eax, r->edx, r->ebx, r->ecx);
}

/* -> row EAX, column EDX */
void xn_gfx_get_cursor_pos_r(xn_regs *r)
{
    s32 row, col;

    xn_gfx_get_cursor_pos(&row, &col);
    r->eax = row;
    r->edx = col;
}

/* row EAX, column EDX */
void xn_gfx_set_cursor_pos_r(xn_regs *r)
{
    xn_gfx_set_cursor_pos(r->eax, r->edx);
}

/* CF when there is no VBE */
void xn_gfx_vesa_init_r(xn_regs *r)
{
    XN_SETFLAG(r, XN_CF, !xn_gfx_vesa_init());
}

void xn_gfx_vesa_free_r(xn_regs *r)
{
    xn_gfx_vesa_free();
}

void xn_gfx_vesa_set_mode_r(xn_regs *r)
{
    XN_SETFLAG(r, XN_CF, !xn_gfx_vesa_set_mode(r->eax));
}

void xn_gfx_vesa_get_mode_info_r(xn_regs *r)
{
    XN_SETFLAG(r, XN_CF, !xn_gfx_vesa_get_mode_info(r->eax));
}

void xn_gfx_vesa_set_display_start_r(xn_regs *r)
{
    xn_gfx_vesa_set_display_start(r->eax, r->edx);
}

void xn_gfx_vesa_set_bank_r(xn_regs *r)
{
    xn_gfx_vesa_set_bank(r->eax);
}

void xn_gfx_vesa_get_bank_r(xn_regs *r)
{
    r->eax = xn_gfx_vesa_get_bank();
}

/* AX BX CX DX for the BIOS; CF when it failed */
void xn_gfx_vesa_int10_r(xn_regs *r)
{
    XN_SETFLAG(r, XN_CF, !xn_gfx_vesa_int10(r->eax, r->ebx, r->ecx, r->edx));
}

void xn_gfx_vesa_present_banked_r(xn_regs *r)
{
    xn_gfx_vesa_present_banked(r->eax);
}

/* x EAX, y EDX, w EBX, h ECX, src ESI */
void xn_gfx_vesa_blit_rect_banked_r(xn_regs *r)
{
    xn_gfx_vesa_blit_rect_banked(r->eax, r->edx, r->ebx, r->ecx, (const u8 *)r->esi);
}
