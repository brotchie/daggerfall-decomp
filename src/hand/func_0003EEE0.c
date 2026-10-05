/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0003EEE0 */
#include "records.h"

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
extern int msgbox_render_quest_text(struct quest *, short, char *, int);
extern void msgbox_wait(void);
extern void mode_push(void);
extern void mc_memset(void *, int, int, char *, int, int);
extern void *mc_malloc(int, char *, int);
extern void mc_strncpy(char *, char *, int, char *, int);
extern void mc_memcpy(char *, char *, int, char *, int, int);
extern void xn_mouse_poll_clamped(void);
extern void xn_font_select(int);

void msgbox_show_qrc_text(char *name, short message_id, short kind)
{
    struct quest stub;
    int single_page;

    if (msgbox_kind != 0)
        return;
    msgbox_saved_screen = mc_malloc(64000, D_00170D55, 765);
    mc_memcpy(msgbox_saved_screen, screen_buffer, 64000, D_00170D55, 766, 4);
    mc_memset(&stub, 0, 60, D_00170D55, 768, 4);
    mc_strncpy(stub.name, name, 9, D_00170D55, 769);
    xn_font_select(4);
    if (kind == 5) {
        D_00196271 = 0;
        single_page = msgbox_render_quest_text(&stub, message_id, msgbox_image, 4);
    } else
        single_page = msgbox_render_quest_text(&stub, message_id, msgbox_image, 0);
    if (single_page == 0)
        return;
    msgbox_kind = kind;
    mode_push();
    game_mode = 8;
    D_00196272 = 1;
    mouse_buttons_prev = mouse_buttons;
    xn_mouse_poll_clamped();
    msgbox_wait();
}
