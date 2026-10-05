/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0003DCF4 */
struct flags5 { unsigned char pad:7; unsigned char b7:1; };
struct flags6 { unsigned char pad:5; unsigned char b5:1; };
extern char text_shadow_colour;
extern char D_0012B508;
extern short D_0012DA44;
extern char *screen_buffer;
extern char D_00170D55[];       /* __FILE__ */
extern char text_buffer[];
extern struct flags5 D_001940D5;
extern struct flags6 D_001940D6;
extern short D_00195F36;
extern short D_00195F38;
extern char *msgbox_next_page;
extern char *D_00199658;
extern short D_00199660;
extern short D_00199662;
extern short D_00199664;
extern short D_00199666;
extern short D_00199668;
extern short D_0019966A;
extern void msgbox_draw_buttons(int);
extern int font_char_width(unsigned char);
extern int font_text_width(char *);
extern void text_draw_shadow(char *, short, short);
extern void text_draw_centred_shadow(char *, short, short);
extern void func_0005F50B(void);
extern void mc_free(void *, char *, int);
extern void mc_memset(char *, int, int, char *, int, int);
extern char *mc_malloc(int, char *, int);
extern void mc_memcpy(char *, char *, int, char *, int, int);
extern void func_0012DB50(short);
extern void func_00144E84(int, int, int, int, char *, int);
extern void func_00144F68(int, int, int, int, char *);
extern void func_00144FB4(int, int, int, int, char *);

void msgbox_render(char *text, char **out)
{
    char *p;
    short xbase;
    short w;
    unsigned char mode;
    short y0;
    char *save;
    short ybase;
    short n;
    short k;
    short h;
    short x0;
    unsigned char c;
    unsigned char stop;
    char *q;
    int mul;
    short rows;
    short hh;
    short cols;
    short y;
    unsigned char savedcolor;
    short ww;
    void (*draw)(int, int, int, int, char *);
    short x;

    q = p = text;
    msgbox_next_page = 0;
    stop = 0;
    func_0012DB50(4);
    h = D_0012DA44;
    w = D_00199668 = D_0019966A = 0;
    while ((*p != 0 || p[-1] != 0) && msgbox_next_page == 0) {
        switch (c = *p++) {
        case 0:
            if (w > D_00199668)
                D_00199668 = w;
            D_0019966A += h;
            h = D_0012DA44;
            w = 0;
            if (D_0019966A > 160 && *p != 0)
                msgbox_next_page = p;
            break;
        case 251:
            w = (unsigned char)*p++;
            break;
        case 249:
            func_0012DB50((unsigned char)*p++);
            if (h < D_0012DA44)
                h = D_0012DA44;
            break;
        case 248:
        case 250:
        case 252:
        case 253:
            p++;
            break;
        default:
            w += font_char_width(c);
            break;
        }
    }
    func_0012DB50(4);
    D_0019966A += 10;
    D_00199668 += 10;
    if (D_00199664 & 4)
        D_0019966A += 20;
    savedcolor = D_0012B508;
    text_shadow_colour = 156;
    D_0012B508 = 145;
    x = 160 - (D_00199668 >> 1) + 5;
    xbase = x;
    y = 100 - (D_0019966A >> 1) + 5;
    ybase = y;
    if (D_00199664 & 4) {
        ybase += -20;
        y += -20;
    }
    save = 0;
    save = mc_malloc(64000, D_00170D55, 353);
    mc_memcpy(save, screen_buffer, 64000, D_00170D55, 354, 4);
    mc_memset(screen_buffer, 0, 64000, D_00170D55, 355, 4);
    ww = D_00199668 + 10;
    cols = (ww + D_00199666 - 1) / D_00199666;
    if (cols < 2)
        cols = 2;
    hh = D_0019966A + 10;
    rows = (hh + D_00199662 - 1) / D_00199662;
    if (rows < 2)
        rows = 2;
    x0 = 160 - ((D_00199668 = cols * D_00199666) >> 1);
    x = x0 + 5 + ((D_00199668 - ww + 10) >> 1);
    xbase = x;
    y0 = 100 - ((D_0019966A = rows * D_00199662) >> 1);
    y = y0 + 5 + ((D_0019966A - hh + 10) >> 1);
    ybase = y;
    if (D_001940D5.b7) {
        draw = func_00144FB4;
        D_001940D5.b7 = 0;
    } else {
        draw = func_00144F68;
    }
    for (n = 0; n < rows; n++) {
        if (n == 0) {
            draw(x0, y0, D_00199666, D_00199662, D_00199658);
            draw(x0 + D_00199666 * (cols - 1), y0, D_00199666, D_00199662, D_00199660 * 2 + D_00199658);
            mul = 1;
        } else if (n == rows - 1) {
            draw(x0, y0 + (rows - 1) * D_00199662, D_00199666, D_00199662, D_00199660 * 6 + D_00199658);
            draw(x0 + (cols - 1) * D_00199666, (rows - 1) * D_00199662 + y0, D_00199666, D_00199662, (D_00199660 << 3) + D_00199658);
            mul = 7;
        } else {
            draw(x0, y0 + n * D_00199662, D_00199666, D_00199662, D_00199660 * 3 + D_00199658);
            draw(x0 + (cols - 1) * D_00199666, n * D_00199662 + y0, D_00199666, D_00199662, D_00199660 * 5 + D_00199658);
            mul = 4;
        }
        for (k = 1; k < cols - 1; k++)
            draw(x0 + k * D_00199666, y0 + n * D_00199662, D_00199666, D_00199662, D_00199660 * mul + D_00199658);
    }
    D_0012B508 = savedcolor;
    h = D_0012DA44;
    n = 0;
    text_buffer[0] = 0;
    mode = 252;
    while ((*q != 0 || q[-1] != 0) && stop == 0) {
        switch (c = *q++) {
        case 0:
            if (mode == 252)
                text_draw_shadow(text_buffer, x, y);
            else
                text_draw_centred_shadow(text_buffer, 160, y);
            y += h;
            n = 0;
            h = D_0012DA44;
            x = xbase;
            text_buffer[0] = 0;
            if (msgbox_next_page != 0 && msgbox_next_page <= q)
                stop = 1;
            break;
        case 251:
            if (mode == 252)
                text_draw_shadow(text_buffer, x, y);
            else
                text_draw_centred_shadow(text_buffer, 160, y);
            x = xbase + (unsigned char)*q++;
            text_buffer[0] = 0;
            n = 0;
            break;
        case 249:
            if (mode == 252)
                text_draw_shadow(text_buffer, x, y);
            else
                text_draw_centred_shadow(text_buffer, 160, y);
            x += font_text_width(text_buffer);
            text_buffer[0] = 0;
            n = 0;
            func_0012DB50((unsigned char)*q++);
            if (h < D_0012DA44)
                h = D_0012DA44;
            break;
        case 248:
            D_00195F36 = x + font_text_width(text_buffer);
            D_00195F38 = y;
            q++;
            break;
        case 252:
            mode = 252;
            q++;
            break;
        case 253:
            mode = 253;
            q++;
            break;
        case 250:
            if (mode == 252)
                text_draw_shadow(text_buffer, x, y);
            else
                text_draw_centred_shadow(text_buffer, 160, y);
            x += font_text_width(text_buffer);
            text_buffer[0] = 0;
            n = 0;
            D_0012B508 = *q++;
            break;
        default:
            text_buffer[n++] = c;
            text_buffer[n] = 0;
            break;
        }
    }
    if (D_00199664 & 4)
        msgbox_draw_buttons((short)(y + 4));
    if (D_001940D6.b5) {
        D_001940D6.b5 = 0;
        func_0005F50B();
    }
    *out = mc_malloc(D_0019966A * D_00199668, D_00170D55, 510);
    func_00144E84(x0, y0, D_00199668, D_0019966A, *out, 0);
    if (save != 0) {
        mc_memcpy(screen_buffer, save, 64000, D_00170D55, 515, 4);
        if (save != 0 && save != (char *)0x97979797) {
            mc_free(save, D_00170D55, 516);
            save = (char *)0x97979797;
        }
    }
}
