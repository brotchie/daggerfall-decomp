/* font.c: XnGine's fonts (canonical C; the interface and the module's documentation are in
   xfont.h). */
#include "xfont.h"
#include "xgfx.h"
#include "xdos.h"
#include "xdrawhlp.h"

/* other groups' functions (group A's: xstr.h, xdos.h, xkbd.h, xmem.h) */
void xn_str_from_int(s32 v, char *buf, s32 digits);
void *xn_dos_load_file(const char *name, void *dst);
void xn_kbd_remove(void);
void xn_mem_shutdown(void);

void xn_font_init(void)
{
    xn_fill_dwords(xn_font_table, 0, 8 * PTR_SIZE / 4);     /* 8 pointers */
    font_char_spacing = 1;
    xn_font_line_gap = 1;
}

struct xn_fnt_file *xn_font_load(s32 number, s32 slot)
{
    xn_str_from_int(number, xn_font_filename + 4, 4);
    return xn_font_table[slot] = xn_dos_load_file(xn_font_filename, 0);
}

/* The game's calls of xn_font_load (boundary adapter, Quirk Q-FONT-01): the asm left EBX = 4,
   the digit count it gave xn_str_from_int, and init_game_data's next call reads it. */
void xn_font_load_b(xn_regs *r)
{
    r->eax = (uptr)xn_font_load((s32)r->eax, (s32)r->edx);
    r->ebx = 4;
}

s32 xn_font_select(s32 slot)
{
    struct xn_fnt_file *f = xn_font_table[slot];

    if (f == 0) {
        xn_gfx_restore_mode();
        xn_kbd_remove();
        xn_mem_shutdown();
        xn_dos_print(xn_font_msg_no_font);
        xn_dos_exit(0);
        return 0;
    }
    xn_font_current = slot;
    font_glyphs = f;
    font_space_width = f->space_width;
    font_height = f->height;
    return font_height;
}

s32 xn_font_draw_string(s32 x, s32 y, const u8 *text)
{
    u8 c;

    xn_font_text_x0 = x;
    xn_font_text_y0 = y;
    while ((c = *text++) != 0) {
        if ((s8)c >= 0x21) {                    /* Quirk Q-FONT-04: 80h-FFh fall through */
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

void xn_font_draw_glyph(s32 x, s32 y, s32 w, const u16 *rows)
{
    s32 h = font_height, cut, end;
    u32 bits;
    u8 skip = 0x10;             /* bits shifted out of a row before its first pixel */
    u8 colour, n;
    u8 *dst;

    if (x >= xn_gfx_clip_right || y >= xn_gfx_clip_bottom)
        return;
    /* (each cut is tested as the asm's `sub; jle` does: a true compare of the two values) */
    if (y < xn_gfx_clip_top) {
        cut = xn_gfx_clip_top - y;
        if (h <= cut)
            return;
        h -= cut;
        rows += cut;
        y = xn_gfx_clip_top;
    }
    end = y + h;
    if (end > xn_gfx_clip_bottom) {
        cut = end - xn_gfx_clip_bottom;
        if (h <= cut)
            return;
        h -= cut;
    }
    if (x < xn_gfx_clip_left) {
        cut = xn_gfx_clip_left - x;
        if (w <= cut)
            return;
        w -= cut;
        w = (w & ~0xFF) | (u8)(w + 0x10);       /* Quirk Q-FONT-02: 16 more columns, */
        skip = (u8)w;                           /* and the shift is that width */
        x = xn_gfx_clip_left;
    }
    end = x + w;
    if (end > xn_gfx_clip_right) {
        cut = end - xn_gfx_clip_right;
        if (w <= cut)
            return;
        w -= cut;
    }
    dst = XN_SCREEN(x, y);
    colour = text_colour;
    /* A row's bits go to the top of a dword, the leftmost pixel in bit 31. The upper half is
       what the last row left there (x at first): it matters only for a shift under 16, after a
       left-clipped width of 240 or more wrapped (Q-FONT-02). */
    bits = x;
    do {
        bits = (bits & 0xFFFF0000u) | *rows++;
        bits <<= skip & 31;
        n = (u8)w;                              /* Quirk Q-FONT-03: 0 draws 256 */
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
    if (c <= 0x21)                              /* Quirk Q-FONT-04: '!' too */
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
