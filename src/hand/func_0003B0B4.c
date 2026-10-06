/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0003B0B4 */
#include "ptrint.h"
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
extern iptr disk_read_file(char *, char *);
extern int mc_memcpy();
extern int xn_mouse_poll_clamped();
extern int xn_mouse_cursor_move();
extern int xn_gfx_clear();

int chargen_popup_choice(short text_id, short button1, short button2, char *image, unsigned char key1, unsigned char key2)
{
    D_00196271 = 0;
    D_001940D4 &= 254;
    while (mouse_buttons != 0)
        xn_mouse_poll_clamped();
    D_0012B508 = 146;
    msgbox_button_keys = key1;
    D_00196034 = key2;
    msgbox_button_ids = button1;
    D_00196090 = button2;
    D_00196091 = 0;
    if (image != 0) {
        disk_read_file(image, scratch_buffer);
        mc_memcpy(screen_buffer, scratch_buffer, 64000, D_00170B88, 399, 4);
    } else {
        xn_gfx_clear(0);
    }
    D_001940D5 |= 128;
    msgbox_show_rsc(text_id, 5);
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
