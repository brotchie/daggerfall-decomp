/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0007CD98 */
extern unsigned char mouse_buttons;
extern signed char text_buffer[];
extern unsigned char D_00196272;
extern unsigned char D_00196275;
extern unsigned char mouse_buttons_prev;
extern char *info_popup_text;
extern short font_text_width(char *);
extern void text_draw(char *, short, short);
extern void func_000CD0F1(int, int, int, int);
extern char *func_000CE300(char *, char *);

void info_popup_update(void)
{
    char *p;
    short i;
    short n;
    short x;
    short y;
    short w;

    n = 0;
    i = 0;
    w = 0;
    if (info_popup_text == 0)
        return;
    p = info_popup_text;
    do {
        p = func_000CE300(((char *)text_buffer), p);
        i = font_text_width(((char *)text_buffer));
        if (i > w)
            w = i;
        n++;
    } while (p[-1] != 0);
    w = (w + 10) / 2;
    func_000CD0F1(160 - w, 100 - n * 5 - 5, w * 2, n * 10 + 10);
    p = info_popup_text;
    x = 160 - w + 5;
    y = 100 - n * 5;
    for (i = 0; i < n; i++, y += 10) {
        p = func_000CE300(((char *)text_buffer), p);
        text_draw(((char *)text_buffer), x, y);
    }
    if (D_00196272 == 1 && (mouse_buttons & 1) == 0 && (mouse_buttons_prev & 1) != 0) {
        D_00196272 = D_00196275;
        info_popup_text = 0;
    }
}
