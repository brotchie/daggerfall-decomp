/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0003E8A8 */
extern char D_00170D55[];
extern short msgbox_wrap_width;
extern int msgbox_next_page;
extern char *text_rsc_load(int, int, int);
extern void msgbox_render(char *, char **);
extern void msgbox_show_more_pages(char **);
extern void mc_free(void *, char *, int);

int msgbox_render_rsc(short text_id, char **image, short flags)
{
    char *text;
    int single_page;

    text = text_rsc_load(text_id, (short)(flags | 0x8002), msgbox_wrap_width);
    msgbox_render(text, image);
    if (msgbox_next_page != 0) {
        msgbox_show_more_pages(image);
        single_page = 0;
    } else {
        single_page = 1;
    }
    if (text != 0 && text != (char *)0x97979797) {
        mc_free(text, D_00170D55, 599);
        text = (char *)0x97979797;
    }
    return single_page;
}
