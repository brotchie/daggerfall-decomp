/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0007CD98 */
extern unsigned char mouse_buttons;
extern signed char text_buffer[];
extern unsigned char D_00196272;
extern unsigned char D_00196275;
extern unsigned char mouse_buttons_prev;
extern char *info_popup_text;
extern short font_text_width(char *);
extern void text_draw(char *, short, short);
extern void xn_draw_darken_rect(int, int, int, int);
extern char *xn_str_copy_line(char *, char *);

void info_popup_update(void)
{
    char *text;
    short i;
    short line_count;
    short x;
    short y;
    short max_width;

    line_count = 0;
    i = 0;
    max_width = 0;
    if (info_popup_text == 0)
        return;
    text = info_popup_text;
    do {
        text = xn_str_copy_line(((char *)text_buffer), text);
        i = font_text_width(((char *)text_buffer));
        if (i > max_width)
            max_width = i;
        line_count++;
    } while (text[-1] != 0);
    max_width = (max_width + 10) / 2;
    xn_draw_darken_rect(160 - max_width, 100 - line_count * 5 - 5, max_width * 2, line_count * 10 + 10);
    text = info_popup_text;
    x = 160 - max_width + 5;
    y = 100 - line_count * 5;
    for (i = 0; i < line_count; i++, y += 10) {
        text = xn_str_copy_line(((char *)text_buffer), text);
        text_draw(((char *)text_buffer), x, y);
    }
    if (D_00196272 == 1 && (mouse_buttons & 1) == 0 && (mouse_buttons_prev & 1) != 0) {
        D_00196272 = D_00196275;
        info_popup_text = 0;
    }
}
