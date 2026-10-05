/* font.c: XnGine's fonts as readable C (xfont.h; see xngine.h, docs/xngine_readable.md and
   docs/engine/smc/font.md, FONT-GLYPH). */
#include "xfont.h"
#include "xdrawhlp.h"

/* other groups' functions, still asm: their interfaces (config/xngine_abi.csv) */
void xn_str_from_int(s32 v, char *buf, s32 digits);
#pragma aux xn_str_from_int parm [eax] [edx] [ebx] modify exact [eax edx];
void *xn_dos_load_file(const char *name, void *dst);
#pragma aux xn_dos_load_file parm [eax] [edx] value [eax] modify exact [eax ecx edx ebx esi edi];
void xn_gfx_restore_mode(void);
#pragma aux xn_gfx_restore_mode parm [] modify exact [eax];
void xn_kbd_remove(void);
#pragma aux xn_kbd_remove parm [] modify exact [eax];
void xn_mem_shutdown(void);
#pragma aux xn_mem_shutdown parm [] modify exact [eax edx];

void xn_font_init(void)
{
    xn_fill_dwords(xn_font_table, 0, 8);
    font_char_spacing = 1;
    xn_font_line_gap = 1;
}

struct xn_fnt_file *xn_font_load(s32 number, s32 slot)
{
    xn_str_from_int(number, xn_font_filename + 4, 4);
    return xn_font_table[slot] = xn_dos_load_file(xn_font_filename, 0);
}

void xn_font_load_r(xn_regs *r)
{
    xn_font_load(r->eax, r->edx);
    r->ebx = 4;                 /* the digit count it gave xn_str_from_int */
}

s32 xn_font_select(s32 slot)
{
    struct xn_fnt_file *f = xn_font_table[slot];

    if (f == 0) {
        xn_regs r;

        xn_gfx_restore_mode();
        xn_kbd_remove();
        xn_mem_shutdown();
        r.eax = ((u32)slot & ~0xFF00u) | 0x0900;        /* AH 9: print the message */
        r.edx = (u32)xn_font_msg_no_font;
        xn_int21(&r);
        r.eax = (r.eax & 0xFFFF0000u) | 0x4C00;         /* exit to DOS */
        xn_int21(&r);
        return 0;
    }
    xn_font_current = slot;
    font_glyphs = f;
    font_space_width = f->space_width;
    font_height = f->height;
    return font_height;
}

void xn_font_select_r(xn_regs *r)
{
    r->eax = xn_font_select(r->eax);
}

s32 xn_font_draw_string(s32 x, s32 y, const u8 *text)
{
    u8 c;

    xn_font_text_x0 = x;
    xn_font_text_y0 = y;
    while ((c = *text++) != 0) {
        if ((s8)c >= 0x21) {
            const struct xn_fnt_glyph *g = &font_glyphs->glyphs[c - 0x21];

            xn_font_draw_glyph(x, y, g->width, (const u16 *)((u8 *)font_glyphs + g->offset));
            x += g->width + font_char_spacing;
        } else if (c == ' ') {
            x += font_space_width;
        } else if (c == '\r') {
            x = xn_font_text_x0;
        } else if (c == '\n') {
            y += font_height + xn_font_line_gap;
        }
    }
    return x;
}

void xn_font_draw_string_r(xn_regs *r)
{
    r->eax = xn_font_draw_string(r->eax, r->edx, (const u8 *)r->ebx);
}

void xn_font_draw_glyph(s32 x, s32 y, s32 w, const u16 *rows)
{
    s32 h = font_height, cut;
    u32 bits;                   /* EAX: a row's bits, shifted out at the top */
    u8 skip = 0x10;             /* bits shifted out before the first pixel */
    u8 colour, n;
    u8 *dst;

    if (x >= xn_gfx_clip_right || y >= xn_gfx_clip_bottom)
        return;
    if (y < xn_gfx_clip_top) {
        cut = xn_gfx_clip_top - y;
        h -= cut;
        if (h <= 0)
            return;
        rows += cut;
        y = xn_gfx_clip_top;
    }
    cut = y + h - xn_gfx_clip_bottom;
    if (cut > 0) {
        h -= cut;
        if (h <= 0)
            return;
    }
    XN_KEEP(xn_font_glyph_skip, skip);
    if (x < xn_gfx_clip_left) {
        cut = xn_gfx_clip_left - x;
        w -= cut;
        if (w <= 0)
            return;
        w = (w & ~0xFF) | (u8)(w + 0x10);       /* add bl, 10h: the bug (see xfont.h) */
        skip = (u8)w;
        XN_KEEP(xn_font_glyph_skip, skip);
        x = xn_gfx_clip_left;
    }
    cut = x + w - xn_gfx_clip_right;
    if (cut > 0) {
        w -= cut;
        if (w <= 0)
            return;
    }
    dst = XN_SCREEN(x, y);
    XN_KEEP(xn_font_glyph_step, xn_gfx_width - w);
    colour = text_colour;
    bits = x;
    do {
        bits = (bits & 0xFFFF0000u) | *rows++;  /* mov ax, row: the top half stays */
        bits <<= skip & 31;
        n = (u8)w;                              /* the low byte of w: 0 draws 256 */
        do {
            if (bits & 0x80000000u)
                *dst = colour;
            bits <<= 1;
            dst++;
        } while (--n != 0);
        dst += xn_gfx_width - w;
    } while (--h != 0);
}

s32 xn_font_glyph_width(s32 c)
{
    if (c <= 0x21)
        return font_space_width - font_char_spacing;
    return font_glyphs->glyphs[c - 0x21].width;
}

s32 xn_font_string_width(const u8 *text)
{
    s32 w = 0;

    for (; *text != 0; text++)
        w += xn_font_glyph_width(*text) + font_char_spacing;
    return w;
}

s32 xn_font_string_width_n(const u8 *text, s32 n)
{
    s32 w = 0;
    u32 left = n;

    if (left == 0)
        return 0;
    do {
        u8 c = *text++;

        if (c == 0)
            break;
        w += xn_font_glyph_width(c) + font_char_spacing;
    } while (--left != 0);
    return w;
}
