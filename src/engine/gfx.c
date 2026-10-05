/* gfx.c: XnGine's graphics modes and screen as readable C (xgfx.h; see xngine.h and
   docs/xngine_readable.md). The game runs in mode 13h: the VESA paths never run in it, but
   are kept as the asm has them. */
#include "xgfx.h"
#include "xpal.h"

#define VGA_STATUS      0x3DA
#define VGA_IN_RETRACE  0x08
#define VGA_MEMORY      ((u8 *)0xA0000)     /* the screen, and the VESA bank window */
#define VGA_WINDOW_END  0xB0000

#define VBE_OK          0x004F
#define VBE_INFO        0x4F00
#define VBE_MODE_INFO   0x4F01
#define VBE_SET_MODE    0x4F02
#define VBE_GET_MODE    0x4F03
#define VBE_WINDOW      0x4F05
#define VBE_DISPLAY     0x4F07
#define VBE_LINEAR      0x4000              /* the set-mode bit for the linear frame buffer */

/* ---- retrace -------------------------------------------------------------------------- */

void xn_gfx_wait_vretrace_start(void)
{
    while (!(xn_inb(VGA_STATUS) & VGA_IN_RETRACE))
        ;
}

void xn_gfx_wait_vretrace_start_r(xn_regs *r)
{
    xn_gfx_wait_vretrace_start();
}

void xn_gfx_wait_vretrace_end(void)
{
    while (xn_inb(VGA_STATUS) & VGA_IN_RETRACE)
        ;
}

void xn_gfx_wait_vretrace_end_r(xn_regs *r)
{
    xn_gfx_wait_vretrace_end();
}

void xn_gfx_wait_vretraces(u32 n)
{
    do {
        xn_gfx_wait_vretrace_start();
        xn_gfx_wait_vretrace_end();
    } while (--n != 0);
}

void xn_gfx_wait_vretraces_r(xn_regs *r)
{
    xn_gfx_wait_vretraces(r->eax);
    r->eax = 0;                 /* the count, run down */
}

/* ---- present and clear ----------------------------------------------------------------- */

void xn_gfx_present_inclusive(s32 keep)
{
    if (xn_gfx_clip_bottom == 200) {
        xn_gfx_present(keep);
        return;
    }
    xn_gfx_clip_bottom++;
    xn_gfx_present(keep);
    xn_gfx_clip_bottom--;
}

void xn_gfx_present(s32 keep)
{
    xn_regs r;

    r.eax = keep;
    r.ebx = xn_gfx_driver;
    xn_asmcall(XN_GFX_DRIVER_FN(xn_gfx_drv_present), &r);
}

void xn_gfx_clear(s32 colour)
{
    xn_regs r;

    r.eax = colour;
    r.ebx = xn_gfx_driver;
    xn_asmcall(XN_GFX_DRIVER_FN(xn_gfx_drv_clear), &r);
}

void xn_gfx_drv0_clear(u8 colour)
{
    u32 fill = xn_colour_fill_table[colour];
    s32 top = xn_gfx_row_offset[xn_gfx_clip_top];
    u32 *p = (u32 *)(screen_buffer + top);
    u32 n = (u32)(xn_gfx_row_offset[xn_gfx_clip_bottom] - top) >> 2;

    for (; n != 0; n--)
        *p++ = fill;
}

void xn_gfx_drv0_present(s32 keep)
{
    s8 how = (s8)xn_gfx_present_mode;
    s32 top, offset;
    xn_regs r;

    if (how <= 0)
        return;
    if (how == 3) {
        /* the linear frame buffer: show the page drawn, draw into the next */
        xn_gfx_vesa_set_display_start(0, xn_gfx_vesa_page * xn_gfx_height);
        if (++xn_gfx_vesa_page == xn_gfx_page_count)
            xn_gfx_vesa_page = 0;
        screen_buffer = xn_gfx_vesa_lfb + xn_gfx_screen_size * xn_gfx_vesa_page;
        if (keep == 0)
            xn_gfx_clear(text_shadow_colour);
        return;
    }
    if ((u32)xn_gfx_mode >= 0x100) {
        xn_gfx_vesa_present_banked(keep);
        return;
    }
    top = xn_gfx_clip_top;
    offset = xn_gfx_row_offset[top];
    if (keep != 0) {
        xn_gfx_copy_rows(VGA_MEMORY + offset, screen_buffer + offset,
                         xn_gfx_row_offset[xn_gfx_clip_bottom - top]);
        return;
    }
    r.ecx = xn_gfx_row_offset[xn_gfx_clip_bottom - top];
    r.esi = (u32)(screen_buffer + offset);
    r.edi = (u32)(VGA_MEMORY + offset);
    r.eax = keep;
    xn_asmcall(xn_gfx_copy_clear_fn, &r);
}

void xn_gfx_copy_rows(u8 *dst, const u8 *src, u32 n)
{
    u32 *d = (u32 *)dst;
    const u32 *s = (const u32 *)src;
    u32 k;

    for (k = n >> 2; k != 0; k--)
        *d++ = *s++;
    dst = (u8 *)d;
    src = (const u8 *)s;
    for (k = n & 3; k != 0; k--)
        *dst++ = *src++;
}

void xn_gfx_copy_rows_r(xn_regs *r)
{
    u32 n = r->ecx;

    xn_gfx_copy_rows((u8 *)r->edi, (const u8 *)r->esi, n);
    r->esi += n;
    r->edi += n;
    r->ecx = 0;
}

void xn_gfx_copy_and_clear(u8 *dst, u8 *src, u32 n)
{
    u32 fill = xn_colour_fill_table[text_shadow_colour];
    u32 *p = (u32 *)src;
    u32 k;

    xn_gfx_copy_rows(dst, src, n);
    for (k = n >> 2; k != 0; k--)
        *p++ = fill;
    src = (u8 *)p;
    for (k = n & 3; k != 0; k--)
        *src++ = (u8)fill;
}

void xn_gfx_copy_and_clear_r(xn_regs *r)
{
    u32 n = r->ecx;

    xn_gfx_copy_and_clear((u8 *)r->edi, (u8 *)r->esi, n);
    r->eax = xn_colour_fill_table[text_shadow_colour];
    r->esi += n;
    r->edi = r->esi;            /* the clear ran over the source */
    r->ecx = 0;
}

void xn_gfx_copy_and_clear_unrolled(u32 *dst, u32 *src, u32 n)
{
    u32 fill = xn_colour_fill_table[text_shadow_colour];
    u32 blocks, k;

    for (blocks = n >> 9; blocks != 0; blocks--, src += 128, dst += 128)
        for (k = 0; k < 128; k++) {
            u32 v = src[k];

            src[k] = fill;
            dst[k] = v;
        }
    if (n & 0x1FF)
        xn_gfx_copy_and_clear((u8 *)dst, (u8 *)src, n & 0x1FF);
}

void xn_gfx_drv0_shutdown(void)
{
}

/* ---- modes ------------------------------------------------------------------------------- */

/* Draws into the linear frame buffer: its pages cleared, page 0 first */
static void use_frame_buffer(void)
{
    u32 *p = (u32 *)xn_gfx_vesa_lfb;
    u32 n;

    screen_buffer = xn_gfx_vesa_lfb;
    xn_gfx_buffer_base = xn_gfx_vesa_lfb;
    xn_gfx_buffer_alloc = 0;
    xn_gfx_page_count = xn_gfx_vesa_mode_info.image_pages;
    for (n = (u32)(xn_gfx_screen_size * xn_gfx_page_count) >> 2; n != 0; n--)
        *p++ = 0;
    xn_gfx_present_mode = 3;
    xn_gfx_vesa_page = 0;
}

/* Allocates the back buffer (32-aligned) and clears it. 0 when there is no memory. */
static int alloc_back_buffer(void)
{
    u8 *block;
    u32 *p;
    u32 n;

    xn_gfx_screen_size = xn_gfx_width * xn_gfx_height;
    block = game_malloc(xn_gfx_screen_size + 32);
    if (block == 0)
        return 0;
    xn_gfx_buffer_alloc = block;
    screen_buffer = (u8 *)(((u32)block + 31) & ~31u);
    xn_gfx_buffer_base = screen_buffer;
    p = (u32 *)screen_buffer;
    for (n = (u32)xn_gfx_screen_size >> 2; n != 0; n--)
        *p++ = 0;
    xn_gfx_page_count = 1;
    return 1;
}

s32 xn_gfx_set_mode(s32 mode, s32 present_mode)
{
    int linear = 0;

    xn_gfx_present_mode = (u8)present_mode;
    if (xn_gfx_vesa_init() && mode != 0x13) {
        if (!xn_gfx_vesa_set_mode(mode))
            return 1;
        linear = xn_gfx_vesa_lfb != 0;
    } else {
        if (mode != 0x13)
            return 1;
        xn_gfx_set_mode13();
    }
    if (linear)
        use_frame_buffer();
    else if (!alloc_back_buffer())
        return 1;
    xn_gfx_set_clip(0, 0, xn_gfx_width, xn_gfx_height);
    xn_gfx_build_row_offsets();
    xn_gfx_driver = 0;
    return 0;
}

void xn_gfx_set_mode_r(xn_regs *r)
{
    r->eax = xn_gfx_set_mode(r->eax, (u8)r->edx);
}

void xn_gfx_restore_mode(void)
{
    s32 mode;
    xn_regs r;

    xn_asmcall(XN_GFX_DRIVER_FN(xn_gfx_drv_shutdown), &r);
    if (xn_gfx_buffer_alloc != 0)
        game_free(xn_gfx_buffer_alloc);
    mode = xn_gfx_saved_mode;
    if (mode == 0)
        return;
    if ((u32)mode >= 0x100) {
        xn_gfx_vesa_set_mode(mode);
    } else {
        r.eax = mode;           /* AH = 0: set the mode */
        xn_int10(&r);
    }
    xn_gfx_vesa_free();
}

void xn_gfx_restore_mode_r(xn_regs *r)
{
    xn_gfx_restore_mode();
}

s32 xn_gfx_change_mode(s32 mode)
{
    int linear = 0;

    if (xn_gfx_mode == mode)
        return 0;
    xn_gfx_mode = mode;
    if (xn_pal_current != 0)
        xn_pal_get(xn_pal_current);
    if (xn_gfx_buffer_alloc != 0)
        game_free(xn_gfx_buffer_alloc);    /* (not forgotten: a failure below leaves it) */
    if (xn_gfx_mode == 0x13) {
        xn_gfx_set_mode13();
    } else {
        if (!xn_gfx_vesa_present || !xn_gfx_vesa_set_mode(xn_gfx_mode))
            return 1;
        linear = xn_gfx_vesa_lfb != 0;
    }
    if (linear) {
        use_frame_buffer();
    } else {
        if (!alloc_back_buffer())
            return 1;
        xn_gfx_present_mode = 1;
    }
    xn_gfx_set_clip(0, 0, xn_gfx_width, xn_gfx_height);
    xn_gfx_build_row_offsets();
    if (xn_pal_current != 0)
        xn_pal_set(xn_pal_current);
    return 0;
}

void xn_gfx_change_mode_r(xn_regs *r)
{
    r->eax = xn_gfx_change_mode(r->eax);
    XN_SETFLAG(r, XN_CF, r->eax != 0);
}

void xn_gfx_set_mode13(void)
{
    xn_regs r;

    if (xn_gfx_saved_mode == 0) {
        r.eax = 0x0F00;         /* get the mode */
        xn_int10(&r);
        xn_gfx_saved_mode = r.eax & 0xFF;
    }
    xn_gfx_mode = 0x13;
    r.eax = 0x13;
    xn_int10(&r);
    xn_gfx_width = 320;
    xn_gfx_height = 200;
    xn_gfx_screen_size = 64000;
    xn_gfx_set_clip(0, 0, xn_gfx_width, xn_gfx_height);
    xn_gfx_build_row_offsets();
}

void xn_gfx_build_row_offsets(void)
{
    s32 offset = 0;
    int y;

    for (y = 0; y < 768; y++) {
        xn_gfx_row_offset[y] = offset;
        offset += xn_gfx_width;
    }
}

void xn_gfx_set_clip(s32 left, s32 top, s32 right, s32 bottom)
{
    xn_gfx_clip_left = left;
    xn_gfx_clip_top = top;
    xn_gfx_clip_right = right;
    xn_gfx_clip_bottom = bottom;
}

/* ---- BIOS cursor (dead) ----------------------------------------------------------------- */

void xn_gfx_get_cursor_pos_r(xn_regs *r)
{
    r->eax = (r->eax & ~0xFF00u) | 0x0300;
    r->ebx = 0;
    xn_int10(r);
    r->eax = (r->edx >> 8) & 0xFF;
    r->edx &= 0xFF;
}

void xn_gfx_set_cursor_pos_r(xn_regs *r)
{
    r->edx += r->eax << 8;      /* DH = row, DL = column */
    r->eax = ((r->eax << 8) & ~0xFF00u) | 0x0200;
    r->ebx = 0;
    xn_int10(r);
}

/* ---- VESA ------------------------------------------------------------------------------ */

int xn_gfx_vesa_int10(u32 ax, u32 bx, u32 cx, u32 dx)
{
    struct xn_rm_regs *rm = &xn_gfx_vesa_rm_regs;
    xn_regs r;

    rm->eax = ax;
    rm->ebx = bx;
    rm->ecx = cx;
    rm->edx = dx;
    r.eax = 0x300;              /* simulate a real-mode interrupt */
    r.ebx = 0x10;
    r.ecx = 0;
    r.edi = (u32)rm;
    xn_int31(&r);
    if (r.eflags & XN_CF)
        return 0;
    return (u16)rm->eax == VBE_OK;
}

void xn_gfx_vesa_int10_r(xn_regs *r)
{
    XN_SETFLAG(r, XN_CF, !xn_gfx_vesa_int10(r->eax, r->ebx, r->ecx, r->edx));
}

int xn_gfx_vesa_init(void)
{
    const u8 *info;
    xn_regs r;

    xn_gfx_vesa_present = 0;
    r.eax = 0x100;              /* allocate DOS memory: 30h paragraphs */
    r.ebx = 0x30;
    xn_int31(&r);
    if (r.eflags & XN_CF)
        return 0;
    xn_gfx_vesa_rm_regs.ds = (u16)r.eax;
    xn_gfx_vesa_rm_regs.es = (u16)r.eax;
    xn_gfx_vesa_dos_selector = (u16)r.edx;
    r.ebx = xn_gfx_vesa_dos_selector;
    r.eax = 6;                  /* the selector's base address */
    xn_int31(&r);
    ((u16 *)&xn_gfx_vesa_dos_buffer)[0] = (u16)r.edx;
    ((u16 *)&xn_gfx_vesa_dos_buffer)[1] = (u16)r.ecx;
    if (xn_gfx_vesa_int10(VBE_INFO, r.ebx, r.ecx, r.edx)) {
        info = xn_gfx_vesa_dos_buffer;
        if (*(const u32 *)info == 0x41534556 && *(const u16 *)(info + 4) >= 0x102) {
            xn_gfx_vesa_present = 1;            /* "VESA", version 1.2 or later */
            return 1;
        }
    }
    r.edx = xn_gfx_vesa_dos_selector;
    r.eax = 0x101;              /* free the DOS memory */
    xn_int31(&r);
    xn_gfx_vesa_dos_selector = 0;
    return 0;
}

void xn_gfx_vesa_init_r(xn_regs *r)
{
    XN_SETFLAG(r, XN_CF, !xn_gfx_vesa_init());
}

void xn_gfx_vesa_free(void)
{
    xn_regs r;

    if (xn_gfx_vesa_dos_selector != 0) {
        r.eax = 0x101;
        r.edx = xn_gfx_vesa_dos_selector;
        xn_int31(&r);
    }
}

int xn_gfx_vesa_set_mode(s32 mode)
{
    struct xn_vesa_mode_info *info = &xn_gfx_vesa_mode_info;
    u32 phys, size;
    xn_regs r;

    if (xn_gfx_saved_mode == 0) {
        xn_gfx_vesa_int10(VBE_GET_MODE, 0, 0, 0);
        xn_gfx_saved_mode = xn_gfx_vesa_rm_regs.ebx & 0xFF;
    }
    xn_gfx_mode = mode;
    if (!xn_gfx_vesa_get_mode_info(mode))
        goto fail;
    *info = *(const struct xn_vesa_mode_info *)xn_gfx_vesa_dos_buffer;
    if (!xn_gfx_vesa_int10(VBE_SET_MODE, info->phys_base != 0 ? xn_gfx_mode | VBE_LINEAR
                                                               : xn_gfx_mode, 0, 0))
        goto fail;
    xn_gfx_width = info->x_resolution;
    xn_gfx_height = info->y_resolution;
    xn_gfx_screen_size = xn_gfx_width * xn_gfx_height;
    if (info->phys_base != 0) {
        phys = info->phys_base;
        info->image_pages++;    /* the number of pages less one: now the count */
        size = xn_gfx_screen_size * info->image_pages;
        r.eax = 0x800;          /* map the physical frame buffer */
        r.ebx = phys >> 16;
        r.ecx = phys & 0xFFFF;
        r.esi = size >> 16;
        r.edi = size & 0xFFFF;
        xn_int31(&r);
        /* The linear address is BX:CX; the asm shifts BX up and then copies the (cleared) BX
           into CX instead of CX into BX, so CX is lost. Kept. */
        xn_gfx_vesa_lfb = (r.eflags & XN_CF) ? 0 : (u8 *)(r.ebx << 16);
    }
    return 1;
fail:
    xn_gfx_saved_mode = 0;
    return 0;
}

void xn_gfx_vesa_set_mode_r(xn_regs *r)
{
    XN_SETFLAG(r, XN_CF, !xn_gfx_vesa_set_mode(r->eax));
}

int xn_gfx_vesa_get_mode_info(s32 mode)
{
    u32 granularity, step;
    xn_s64 sixty_four;
    u16 position;
    int k;

    if (!xn_gfx_vesa_int10(VBE_MODE_INFO, 0, mode, 0))
        return 0;
    granularity = ((const struct xn_vesa_mode_info *)xn_gfx_vesa_dos_buffer)->win_granularity;
    if (granularity == 0)
        return 0;
    sixty_four.lo = 64;         /* 64K in units of the granularity (KB) */
    sixty_four.hi = 0;
    step = xn_u64_div(&sixty_four, granularity);
    if (step == 0)
        return 0;
    xn_gfx_vesa_bank_step = (u16)step;
    position = 0;
    for (k = 0; k < 16; k++, position += (u16)step)
        xn_gfx_vesa_bank_table[k] = position;
    return 1;
}

void xn_gfx_vesa_get_mode_info_r(xn_regs *r)
{
    XN_SETFLAG(r, XN_CF, !xn_gfx_vesa_get_mode_info(r->eax));
}

void xn_gfx_vesa_set_display_start(s32 x, s32 y)
{
    xn_gfx_vesa_int10(VBE_DISPLAY, 0, x, y);
}

void xn_gfx_vesa_set_bank(s32 bank)
{
    if (xn_gfx_vesa_bank == bank)
        return;
    xn_gfx_vesa_bank = bank;
    xn_gfx_vesa_int10(VBE_WINDOW, 0, 0, xn_gfx_vesa_bank_table[bank]);     /* window A */
    xn_gfx_vesa_int10(VBE_WINDOW, 1, 0, xn_gfx_vesa_bank_table[bank]);     /* window B */
}

s32 xn_gfx_vesa_get_bank(void)
{
    u16 position;
    s32 k;

    xn_gfx_vesa_int10(VBE_WINDOW, 0x100, 0, 0);     /* get window A's position */
    position = (u16)xn_gfx_vesa_rm_regs.edx;
    for (k = 0; k < 16; k++)
        if (xn_gfx_vesa_bank_table[k] == position)
            break;
    return k;
}

/* Calls a row copy routine (asm entry: ecx bytes esi -> edi) and returns where it left the
   source: the banked present carries the source on from one bank to the next */
static u32 call_copy(xn_gfx_routine copy, u32 n, u32 src, u8 *dst)
{
    xn_regs r;

    r.ecx = n;
    r.esi = src;
    r.edi = (u32)dst;
    xn_asmcall(copy, &r);
    return r.esi;
}

void xn_gfx_vesa_present_banked(s32 keep)
{
    s32 offset, left, bank;
    u32 src, first;

    xn_gfx_vesa_copy_fn = keep != 0 ? asm_xn_gfx_copy_rows : xn_gfx_copy_clear_fn;
    offset = xn_gfx_row_offset[xn_gfx_clip_top];
    left = xn_gfx_row_offset[xn_gfx_clip_bottom - xn_gfx_clip_top];
    bank = (u32)offset >> 16;
    xn_gfx_vesa_set_bank(bank);
    /* the first copy runs to the end of the bank, whatever is left: the view must go past
       it, or `left` goes negative and the last copy is ~4 GB long (kept) */
    first = 0x10000 - (offset & 0xFFFF);
    src = call_copy(xn_gfx_vesa_copy_fn, first, (u32)(screen_buffer + offset),
                    VGA_MEMORY + (offset & 0xFFFF));
    left -= first;
    for (bank++; left > 0x10000; bank++) {
        xn_gfx_vesa_set_bank(bank);
        src = call_copy(xn_gfx_vesa_copy_fn, 0x10000, src, VGA_MEMORY);
        left -= 0x10000;
    }
    xn_gfx_vesa_set_bank(bank);
    call_copy(xn_gfx_vesa_copy_fn, left, src, VGA_MEMORY);
}

void xn_gfx_vesa_blit_rect_banked(s32 x, s32 y, u32 w, u32 h, const u8 *src)
{
    u32 offset = xn_gfx_row_offset[y] + x;
    s32 bank = offset >> 16;
    u8 *dst = VGA_MEMORY + (offset & 0xFFFF);
    u32 skip = xn_gfx_width - w;
    u32 k, first;

    xn_gfx_vesa_set_bank(bank);
    XN_KEEP(xn_vesa_blit_skip, skip);
    XN_KEEP(xn_vesa_blit_dwords, w >> 2);
    XN_KEEP(xn_vesa_blit_bytes, w & 3);
    do {
        if ((u32)dst + w >= VGA_WINDOW_END) {
            /* the row crosses the window's end: the rest of it in the next bank */
            first = 0x10000 - ((u32)dst & 0xFFFF);
            for (k = first; k != 0; k--)
                *dst++ = *src++;
            xn_gfx_vesa_set_bank(++bank);
            dst = VGA_MEMORY;
            for (k = w - first; k != 0; k--)
                *dst++ = *src++;
        } else {
            for (k = w >> 2; k != 0; k--, dst += 4, src += 4)
                *(u32 *)dst = *(const u32 *)src;
            for (k = w & 3; k != 0; k--)
                *dst++ = *src++;
        }
        dst += skip;
        if ((u32)dst >= VGA_WINDOW_END) {
            xn_gfx_vesa_set_bank(++bank);
            dst -= 0x10000;
        }
    } while (--h != 0);
}
