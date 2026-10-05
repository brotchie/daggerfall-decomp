/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0003EEE0 */
struct save { char hdr[6]; char name[54]; };
extern char mouse_buttons;
extern char *screen_buffer;
extern char D_00170D55[];        /* __FILE__ */
extern char msgbox_kind;
extern char D_00196271;
extern char D_00196272;
extern char game_mode;
extern char mouse_buttons_prev;
extern char msgbox_image[];
extern char *msgbox_saved_screen;
extern int msgbox_render_quest_text(struct save *, short, char *, int);
extern void msgbox_wait(void);
extern void mode_push(void);
extern void func_000A0040(void *, int, int, char *, int, int);
extern void *func_000A00AF(int, char *, int);
extern void func_000A0AD9(char *, char *, int, char *, int);
extern void func_000A1023(char *, char *, int, char *, int, int);
extern void func_0012B136(void);
extern void func_0012DB50(int);

void msgbox_show_qrc_text(char *a1, short a2, short a3)
{
    struct save s;
    int rc;

    if (msgbox_kind != 0)
        return;
    msgbox_saved_screen = func_000A00AF(64000, D_00170D55, 765);
    func_000A1023(msgbox_saved_screen, screen_buffer, 64000, D_00170D55, 766, 4);
    func_000A0040(&s, 0, 60, D_00170D55, 768, 4);
    func_000A0AD9(s.name, a1, 9, D_00170D55, 769);
    func_0012DB50(4);
    if (a3 == 5) {
        D_00196271 = 0;
        rc = msgbox_render_quest_text(&s, a2, msgbox_image, 4);
    } else
        rc = msgbox_render_quest_text(&s, a2, msgbox_image, 0);
    if (rc == 0)
        return;
    msgbox_kind = a3;
    mode_push();
    game_mode = 8;
    D_00196272 = 1;
    mouse_buttons_prev = mouse_buttons;
    func_0012B136();
    msgbox_wait();
}
