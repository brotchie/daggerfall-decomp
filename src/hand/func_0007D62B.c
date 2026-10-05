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
extern void func_0012B136(void);

void msgbox_choice_rsc(short a1, short a2, short a3, short a4, unsigned char a5, unsigned char a6, unsigned char a7)
{
    D_00196271 = 0;
    D_001940D4 |= 1;
    while (mouse_buttons != 0)
        func_0012B136();
    D_0012B508 = 146;
    msgbox_button_keys = a5;
    D_00196034 = a6;
    D_00196035 = a7;
    msgbox_button_ids = a2;
    D_00196090 = a3;
    D_00196091 = a4;
    msgbox_show_rsc(a1, 5);
}
