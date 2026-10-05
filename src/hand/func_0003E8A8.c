/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0003E8A8 */
extern char D_00170D55[];
extern short D_00178A08;
extern int msgbox_next_page;
extern int text_rsc_load(int, int, int);
extern void msgbox_render(int, int);
extern void msgbox_show_more_pages(int);
extern void mc_free(int, char *, int);

int msgbox_render_rsc(short a1, int a2, short a3)
{
    int h;
    int r;

    h = text_rsc_load(a1, (short)(a3 | 0x8002), D_00178A08);
    msgbox_render(h, a2);
    if (msgbox_next_page != 0) {
        msgbox_show_more_pages(a2);
        r = 0;
    } else {
        r = 1;
    }
    if (h != 0 && h != 0x97979797) {
        mc_free(h, D_00170D55, 599);
        h = 0x97979797;
    }
    return r;
}
