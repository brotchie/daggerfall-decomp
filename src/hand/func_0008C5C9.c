/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0008C5C9 */
#include "ptrint.h"
extern unsigned char D_0012B508;
extern short font_height;
extern short D_00142928;
extern short D_0014292C;
extern char D_00176E2C[];
extern char D_00190B44[];
extern int inpstr_result;
extern iptr inpstr_text;
extern short text_cursor_x;
extern short text_cursor_y;
extern short inpstr_cursor;
extern void text_draw(iptr, unsigned short, unsigned short);
extern int inpstr_read_key(void);
extern int inpstr_handle_key(unsigned char);
extern short inpstr_text_width(iptr, short);
extern void mc_strncpy(iptr, char *, int, char *, int);
extern void xn_draw_line(int, int, int, int);

int inpstr_update(void)
{
    unsigned char c;
    char x;
    char y;

    D_0012B508 += 5;
    D_00142928 = inpstr_text_width(inpstr_text, inpstr_cursor) + text_cursor_x;
    D_0014292C = text_cursor_y;
    {
        int *clk;

        clk = (int *)0x46c;
        if (*clk & 4)
            xn_draw_line(D_00142928, D_0014292C, D_00142928, (short)(D_0014292C + font_height - 1));
    }
    D_0012B508 -= 5;
    text_draw(inpstr_text, text_cursor_x, text_cursor_y);
    c = inpstr_read_key();
    if (c == 0)
        return 0;
    {
        int r;

        r = inpstr_handle_key(c);
        if (r == 0x87654321)
            return 0;
        if (r == 0x8000) {
            mc_strncpy(inpstr_text, D_00190B44, 4, D_00176E2C, 157);
            return 2;
        }
        inpstr_result = r;
        return 1;
    }
}
