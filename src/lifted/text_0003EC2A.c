/* text.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern signed char mouse_buttons;
extern int screen_buffer;
extern char D_00170D55[];
extern short D_00178A08;
extern signed char msgbox_kind;
extern unsigned char D_00196271;
extern signed char D_00196272;
extern signed char game_mode;
extern signed char mouse_buttons_prev;
extern char msgbox_image[];
extern int msgbox_saved_screen;

extern int text_expand_wrap(unsigned short, short, int, int, int);
extern int mc_free();
extern int mc_malloc();
extern int mc_strncpy();
extern int func_000A0DF4();
extern int mc_memcpy();
extern int func_0012B136();
extern int func_0012DB50();
extern void msgbox_render(int, int);
extern void msgbox_wait(void);
extern void mode_push(void);

void msgbox_show_string(int a1, short a2)
{
    int l_24;
    int l_20;
    int l_28;
    int l_18;
    int l_1C;

    if (msgbox_kind != 0) return;
    msgbox_saved_screen = mc_malloc(64000, (int)D_00170D55, 697);
    mc_memcpy(msgbox_saved_screen, screen_buffer, 64000, (int)D_00170D55, 698, 4);
    func_0012DB50(4);
    l_20 = func_000A0DF4(a1);
    l_28 = mc_malloc(l_20 + 16, (int)D_00170D55, 702);
    l_18 = mc_malloc(((l_20 < 4096) ? 8192 : l_20 * 2), (int)D_00170D55, 703);
    l_1C = mc_malloc(((l_20 < 4096) ? 8192 : l_20 * 2), (int)D_00170D55, 704);
    mc_strncpy(l_28, a1, 4, (int)D_00170D55, 705);
    if (((int)(short)a2) == 5) {
        l_24 = 4;
        D_00196271 = 0;
    } else {
        l_24 = 0;
    }
    l_28 = text_expand_wrap((int)(unsigned short)(l_24 | 32770), (int)(short)D_00178A08, l_28, l_18, l_1C);
    msgbox_render(l_28, (int)msgbox_image);
    if (l_28 != 0 && l_28 != (-1751672937)) {
        mc_free(l_28, (int)D_00170D55, 717);
        l_28 = -1751672937;
    }
    msgbox_kind = *(signed char *)&a2;
    mode_push();
    game_mode = 8;
    D_00196272 = 1;
    mouse_buttons_prev = mouse_buttons;
    func_0012B136();
    msgbox_wait();
}
