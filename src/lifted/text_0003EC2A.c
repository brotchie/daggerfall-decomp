/* text.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "ptrint.h"
#include "clib.h"

extern signed char mouse_buttons;
extern iptr screen_buffer;
extern char D_00170D55[];
extern short msgbox_wrap_width;
extern signed char msgbox_kind;
extern unsigned char D_00196271;
extern signed char D_00196272;
extern signed char game_mode;
extern signed char mouse_buttons_prev;
extern char msgbox_image[];
extern char *msgbox_saved_screen;

extern char *text_expand_wrap(unsigned short, short, char *, char *, char *);
extern void xn_mouse_poll_clamped(void);
extern int xn_font_select(int);
extern void msgbox_render(char *, iptr);
extern void msgbox_wait(void);
extern void mode_push(void);

void msgbox_show_string(char *text, short kind)
{
    int flags;
    int length;
    char *copy;
    char *wrap_buf;
    char *expand_buf;

    if (msgbox_kind != 0) return;
    msgbox_saved_screen = mc_malloc(64000, D_00170D55, 697);
    mc_memcpy(msgbox_saved_screen, (void *)screen_buffer, 64000, D_00170D55, 698, 4);
    xn_font_select(4);
    length = strlen(text);
    copy = mc_malloc(length + 16, D_00170D55, 702);
    wrap_buf = mc_malloc(((length < 4096) ? 8192 : length * 2), D_00170D55, 703);
    expand_buf = mc_malloc(((length < 4096) ? 8192 : length * 2), D_00170D55, 704);
    mc_strncpy(copy, text, 4, D_00170D55, 705);
    if (kind == 5) {
        flags = 4;
        D_00196271 = 0;
    } else {
        flags = 0;
    }
    copy = text_expand_wrap((int)(unsigned short)(flags | 32770), (int)(short)msgbox_wrap_width, copy, wrap_buf, expand_buf);
    msgbox_render(copy, (iptr)msgbox_image);
    if (copy != 0 && copy != (char *)0x97979797) {
        mc_free(copy, D_00170D55, 717);
        copy = (char *)0x97979797;
    }
    msgbox_kind = kind;
    mode_push();
    game_mode = 8;
    D_00196272 = 1;
    mouse_buttons_prev = mouse_buttons;
    xn_mouse_poll_clamped();
    msgbox_wait();
}
