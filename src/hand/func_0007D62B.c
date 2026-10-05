/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0007D62B */
extern char mouse_buttons;
extern unsigned char D_0012B508;
extern unsigned char D_001940D4;
extern unsigned char msgbox_button_keys;
extern unsigned char D_00196034;
extern unsigned char D_00196035;
extern unsigned char msgbox_button_ids;
extern unsigned char D_00196090;
extern unsigned char D_00196091;
extern char D_00196271;
extern void msgbox_show_rsc(int, int);
extern void xn_mouse_poll_clamped(void);

void msgbox_choice_rsc(short text_id, short button_1, short button_2, short button_3, unsigned char key_1, unsigned char key_2, unsigned char key_3)
{
    D_00196271 = 0;
    D_001940D4 |= 1;
    while (mouse_buttons != 0)
        xn_mouse_poll_clamped();
    D_0012B508 = 146;
    msgbox_button_keys = key_1;
    D_00196034 = key_2;
    D_00196035 = key_3;
    msgbox_button_ids = button_1;
    D_00196090 = button_2;
    D_00196091 = button_3;
    msgbox_show_rsc(text_id, 5);
}
