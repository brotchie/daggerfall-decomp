/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0003DCF4 */
struct flags5 { unsigned char pad:7; unsigned char b7:1; };
struct flags6 { unsigned char pad:5; unsigned char b5:1; };
extern char text_shadow_colour;
extern char D_0012B508;
extern short font_height;
extern char *screen_buffer;
extern char D_00170D55[];       /* __FILE__ */
extern signed char text_buffer[];
extern struct flags5 D_001940D5;
extern struct flags6 D_001940D6;
extern short text_cursor_x;
extern short text_cursor_y;
extern char *msgbox_next_page;
extern char *msgbox_border_tiles;
extern short msgbox_tile_size;
extern short msgbox_tile_h;
extern short text_format_flags;
extern short msgbox_tile_w;
extern short msgbox_w;
extern short msgbox_h;
extern void msgbox_draw_buttons(short);
extern int font_char_width(unsigned char);
extern int font_text_width(char *);
extern void text_draw_shadow(char *, short, short);
extern void text_draw_centred_shadow(char *, int, int);
extern void painting_draw(void);
extern void mc_free(void *, char *, int);
extern void mc_memset(char *, int, int, char *, int, int);
extern char *mc_malloc(int, char *, int);
extern void mc_memcpy(char *, char *, int, char *, int, int);
extern void xn_font_select(short);
extern void xn_draw_get_rect(int, int, int, int, char *, int);
extern void xn_draw_image(int, int, int, int, char *);
extern void xn_draw_image_transparent(int, int, int, int, char *);

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
    xn_font_select(4);
    h = font_height;
    w = msgbox_w = msgbox_h = 0;
    while ((*p != 0 || p[-1] != 0) && msgbox_next_page == 0) {
        switch (c = *p++) {
        case 0:
            if (w > msgbox_w)
                msgbox_w = w;
            msgbox_h += h;
            h = font_height;
            w = 0;
            if (msgbox_h > 160 && *p != 0)
                msgbox_next_page = p;
            break;
        case 251:
            w = (unsigned char)*p++;
            break;
        case 249:
            xn_font_select((unsigned char)*p++);
            if (h < font_height)
                h = font_height;
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
    xn_font_select(4);
    msgbox_h += 10;
    msgbox_w += 10;
    if (text_format_flags & 4)
        msgbox_h += 20;
    savedcolor = D_0012B508;
    text_shadow_colour = 156;
    D_0012B508 = 145;
    x = 160 - (msgbox_w >> 1) + 5;
    xbase = x;
    y = 100 - (msgbox_h >> 1) + 5;
    ybase = y;
    if (text_format_flags & 4) {
        ybase += -20;
        y += -20;
    }
    save = 0;
    save = mc_malloc(64000, D_00170D55, 353);
    mc_memcpy(save, screen_buffer, 64000, D_00170D55, 354, 4);
    mc_memset(screen_buffer, 0, 64000, D_00170D55, 355, 4);
    ww = msgbox_w + 10;
    cols = (ww + msgbox_tile_w - 1) / msgbox_tile_w;
    if (cols < 2)
        cols = 2;
    hh = msgbox_h + 10;
    rows = (hh + msgbox_tile_h - 1) / msgbox_tile_h;
    if (rows < 2)
        rows = 2;
    x0 = 160 - ((msgbox_w = cols * msgbox_tile_w) >> 1);
    x = x0 + 5 + ((msgbox_w - ww + 10) >> 1);
    xbase = x;
    y0 = 100 - ((msgbox_h = rows * msgbox_tile_h) >> 1);
    y = y0 + 5 + ((msgbox_h - hh + 10) >> 1);
    ybase = y;
    if (D_001940D5.b7) {
        draw = xn_draw_image_transparent;
        D_001940D5.b7 = 0;
    } else {
        draw = xn_draw_image;
    }
    for (n = 0; n < rows; n++) {
        if (n == 0) {
            draw(x0, y0, msgbox_tile_w, msgbox_tile_h, msgbox_border_tiles);
            draw(x0 + msgbox_tile_w * (cols - 1), y0, msgbox_tile_w, msgbox_tile_h, msgbox_tile_size * 2 + msgbox_border_tiles);
            mul = 1;
        } else if (n == rows - 1) {
            draw(x0, y0 + (rows - 1) * msgbox_tile_h, msgbox_tile_w, msgbox_tile_h, msgbox_tile_size * 6 + msgbox_border_tiles);
            draw(x0 + (cols - 1) * msgbox_tile_w, (rows - 1) * msgbox_tile_h + y0, msgbox_tile_w, msgbox_tile_h, (msgbox_tile_size << 3) + msgbox_border_tiles);
            mul = 7;
        } else {
            draw(x0, y0 + n * msgbox_tile_h, msgbox_tile_w, msgbox_tile_h, msgbox_tile_size * 3 + msgbox_border_tiles);
            draw(x0 + (cols - 1) * msgbox_tile_w, n * msgbox_tile_h + y0, msgbox_tile_w, msgbox_tile_h, msgbox_tile_size * 5 + msgbox_border_tiles);
            mul = 4;
        }
        for (k = 1; k < cols - 1; k++)
            draw(x0 + k * msgbox_tile_w, y0 + n * msgbox_tile_h, msgbox_tile_w, msgbox_tile_h, msgbox_tile_size * mul + msgbox_border_tiles);
    }
    D_0012B508 = savedcolor;
    h = font_height;
    n = 0;
    ((char *)text_buffer)[0] = 0;
    mode = 252;
    while ((*q != 0 || q[-1] != 0) && stop == 0) {
        switch (c = *q++) {
        case 0:
            if (mode == 252)
                text_draw_shadow(((char *)text_buffer), x, y);
            else
                text_draw_centred_shadow(((char *)text_buffer), 160, y);
            y += h;
            n = 0;
            h = font_height;
            x = xbase;
            ((char *)text_buffer)[0] = 0;
            if (msgbox_next_page != 0 && msgbox_next_page <= q)
                stop = 1;
            break;
        case 251:
            if (mode == 252)
                text_draw_shadow(((char *)text_buffer), x, y);
            else
                text_draw_centred_shadow(((char *)text_buffer), 160, y);
            x = xbase + (unsigned char)*q++;
            ((char *)text_buffer)[0] = 0;
            n = 0;
            break;
        case 249:
            if (mode == 252)
                text_draw_shadow(((char *)text_buffer), x, y);
            else
                text_draw_centred_shadow(((char *)text_buffer), 160, y);
            x += font_text_width(((char *)text_buffer));
            ((char *)text_buffer)[0] = 0;
            n = 0;
            xn_font_select((unsigned char)*q++);
            if (h < font_height)
                h = font_height;
            break;
        case 248:
            text_cursor_x = x + font_text_width(((char *)text_buffer));
            text_cursor_y = y;
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
                text_draw_shadow(((char *)text_buffer), x, y);
            else
                text_draw_centred_shadow(((char *)text_buffer), 160, y);
            x += font_text_width(((char *)text_buffer));
            ((char *)text_buffer)[0] = 0;
            n = 0;
            D_0012B508 = *q++;
            break;
        default:
            ((char *)text_buffer)[n++] = c;
            ((char *)text_buffer)[n] = 0;
            break;
        }
    }
    if (text_format_flags & 4)
        msgbox_draw_buttons((short)(y + 4));
    if (D_001940D6.b5) {
        D_001940D6.b5 = 0;
        painting_draw();
    }
    *out = mc_malloc(msgbox_h * msgbox_w, D_00170D55, 510);
    xn_draw_get_rect(x0, y0, msgbox_w, msgbox_h, *out, 0);
    if (save != 0) {
        mc_memcpy(screen_buffer, save, 64000, D_00170D55, 515, 4);
        if (save != 0 && save != (char *)0x97979797) {
            mc_free(save, D_00170D55, 516);
            save = (char *)0x97979797;
        }
    }
}
