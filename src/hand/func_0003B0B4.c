/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0003B0B4 */
extern char mouse_buttons;
extern short mouse_x;
extern short mouse_y;
extern unsigned char D_0012B508;
extern char *screen_buffer;
extern unsigned char D_00147964;
extern char D_00170B88[];
extern unsigned char D_001940D4;
extern unsigned char D_001940D5;
extern char *D_00195C44;
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
extern int func_000A1023();
extern int func_0012B136();
extern int func_0012B2D3();
extern int func_00143914();

int chargen_popup_choice(short a1, short a2, short a3, char *a4, unsigned char a5, unsigned char a6)
{
    D_00196271 = 0;
    D_001940D4 &= 254;
    while (mouse_buttons != 0)
        func_0012B136();
    D_0012B508 = 146;
    msgbox_button_keys = a5;
    D_00196034 = a6;
    msgbox_button_ids = a2;
    D_00196090 = a3;
    D_00196091 = 0;
    if (a4 != 0) {
        disk_read_file(a4, D_00195C44);
        func_000A1023(screen_buffer, D_00195C44, 64000, D_00170B88, 399, 4);
    } else {
        func_00143914(0);
    }
    D_001940D5 |= 128;
    msgbox_show_rsc(a1, 5);
    D_00147964 &= 254;
    while (D_00196271 == 0) {
        keys_world_actions();
        func_0012B136();
        msgbox_update();
        func_0012B2D3(mouse_x, mouse_y);
        func_000A1023(655360, screen_buffer, 64000, D_00170B88, 412, 4);
    }
    return D_00196271 - 1;
}
