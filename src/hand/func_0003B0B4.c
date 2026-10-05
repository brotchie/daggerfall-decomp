/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0003B0B4 */
extern char mouse_buttons;
extern short mouse_x;
extern short mouse_y;
extern unsigned char D_0012B508;
extern char *screen_buffer;
extern unsigned char xn_mouse_cursor_drawn;
extern char D_00170B88[];
extern unsigned char D_001940D4;
extern unsigned char D_001940D5;
extern char *scratch_buffer;
extern unsigned char msgbox_button_keys;
extern unsigned char D_00196034;
extern unsigned char msgbox_button_ids;
extern unsigned char D_00196090;
extern unsigned char D_00196091;
extern unsigned char D_00196271;
extern void msgbox_show_rsc(int, int);
extern void msgbox_update(void);
extern void keys_world_actions(void);
extern int disk_read_file(char *, char *);
extern int mc_memcpy();
extern int xn_mouse_poll_clamped();
extern int xn_mouse_cursor_move();
extern int xn_gfx_clear();

int chargen_popup_choice(short a1, short a2, short a3, char *a4, unsigned char a5, unsigned char a6)
{
    D_00196271 = 0;
    D_001940D4 &= 254;
    while (mouse_buttons != 0)
        xn_mouse_poll_clamped();
    D_0012B508 = 146;
    msgbox_button_keys = a5;
    D_00196034 = a6;
    msgbox_button_ids = a2;
    D_00196090 = a3;
    D_00196091 = 0;
    if (a4 != 0) {
        disk_read_file(a4, scratch_buffer);
        mc_memcpy(screen_buffer, scratch_buffer, 64000, D_00170B88, 399, 4);
    } else {
        xn_gfx_clear(0);
    }
    D_001940D5 |= 128;
    msgbox_show_rsc(a1, 5);
    xn_mouse_cursor_drawn &= 254;
    while (D_00196271 == 0) {
        keys_world_actions();
        xn_mouse_poll_clamped();
        msgbox_update();
        xn_mouse_cursor_move(mouse_x, mouse_y);
        mc_memcpy(655360, screen_buffer, 64000, D_00170B88, 412, 4);
    }
    return D_00196271 - 1;
}
