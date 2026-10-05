/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0004D402 */
extern signed char mouse_buttons;
extern short mouse_x;
extern short mouse_y;
extern signed char D_0012B508;
extern short D_0012DA44;
extern signed char key_down_esc;
extern signed char key_down_enter;
extern short D_00142928;
extern short D_0014292C;
extern char D_00174FAC[];
extern char note_buttons[];
extern char D_0018514A[];
extern char D_0018514C[];
extern char D_0018514E[];
extern char D_00185150[];
extern char D_001851F0[];
extern char D_001851FF[];
extern signed char D_00185201[];
extern signed char text_buffer[];
extern signed char text_rsc_buffer[];
extern signed char D_001940D5;
extern char inpstr_result[];
extern int window_image;
extern unsigned short D_00195F38;
extern signed char game_mode;
extern signed char mouse_buttons_prev;
extern short D_001997B0;
extern short D_001997B2;
extern short D_001997B4;
extern short D_001997B6;
extern int D_001997C0;
extern int D_001997C8;
extern char note_font[];
extern char D_001997DE[];
extern short D_001997E0;
extern short D_001997E2;
extern short D_001997E6;
extern signed char note_tool;
extern signed char D_001997EB;
extern signed char D_001997EC;
extern int func_0004D1E6(int);
extern int note_close(void);
extern void func_0004DABF(void);
extern void func_0004DD12(void);
extern void note_add_text(int);
extern void func_0004E2AB(int);
extern void func_0004E30F(void);
extern void func_0004E360(void);
extern void note_find(void);
extern void func_0004EB71(void);
extern void func_0004EC7A(void);
extern void text_draw_centered_colored(int, int, int, int, unsigned char);
extern int key_pressed_once(unsigned char);
extern void inpstr_begin_text(int, short);
extern int inpstr_update(void);
extern int mc_memset();
extern int func_000A0DD9();
extern int func_000CB552();
extern int func_0012DB50();
extern int func_00144F68();
extern int func_00144FB4();
extern int func_001532B4();

void func_0004D402(void)
{
    int l_20;
    int l_1C;
    int l_18;

    if (func_0004D1E6(0) == 0) return;
    func_000CB552(window_image);
    func_0012DB50(4);
    func_00144F68(182, 176, 44, 9, (int)(*(char **)&D_001997C0 + (((int)(unsigned char)note_tool) * 396)));
    func_00144F68(226, 176, 44, 9, (int)(*(char **)&D_001997C0 + ((((int)(short)*(short *)note_font) * 396) + 1584)));
    func_00144FB4((((int)(unsigned char)D_001997EB) << 3) + 53, 185, 5, 3, (int)D_001851F0);
    D_0012B508 = 145;
    text_draw_centered_colored(func_000A0DD9((((int)(short)D_001997E2) + 1), (int)text_buffer, 10), 303, 4, 145, 141);
    note_find();
    func_0004DD12();
    if (((int)(unsigned char)D_001997EC) == 1) {
        D_0012B508 = *(signed char *)D_001851FF;
        func_0012DB50((int)(short)((unsigned short)(unsigned char)D_00185201[(int)(short)*(short *)note_font]));
        if (inpstr_update() != 0) {
            if (text_rsc_buffer[0] != 0 && ((int)(short)(*(short *)D_001997DE & 32)) == 0) {
                note_add_text((int)text_rsc_buffer);
            }
            if (text_rsc_buffer[0] == 0 && ((int)(short)(*(short *)D_001997DE & 32)) != 0) {
                *(signed char *)(((char *)D_001997C8) + 6) |= 64;
                func_0004EC7A();
            }
            *(signed char *)D_001997DE &= 223;
            if (key_down_enter != 0 && text_rsc_buffer[0] != 0) {
                D_001997E0 += D_0012DA44;
                D_00195F38 = D_001997E0;
                mc_memset((int)text_rsc_buffer, 0, 81, (int)D_00174FAC, 137, 2048);
                inpstr_begin_text((int)text_rsc_buffer, 79);
            } else {
                D_001940D5 &= 251;
                D_001997EC = 0;
            }
        }
    } else if (((int)(unsigned char)D_001997EC) == 2) {
        D_0012B508 = *(signed char *)D_001851FF;
        D_00142928 = D_001997E6;
        D_0014292C = D_001997E0;
        func_001532B4((int)(short)mouse_x, (int)(short)mouse_y);
    } else if (((int)(unsigned char)D_001997EC) == 3) {
        if (((int)(unsigned char)(mouse_buttons & 1)) != 0) {
            D_00142928 = D_001997B0;
            D_0014292C = D_001997B2;
            func_001532B4((int)(short)mouse_x, (int)(short)D_001997B2);
            func_001532B4((int)(short)mouse_x, (int)(short)mouse_y);
            func_001532B4((int)(short)D_001997B0, (int)(short)mouse_y);
            func_001532B4((int)(short)D_001997B0, (int)(short)D_001997B2);
        } else {
            D_001997B4 = mouse_x;
            D_001997B6 = mouse_y;
            if (D_001997B0 > D_001997B4) {
                D_001997B0 ^= D_001997B4;
                D_001997B4 ^= D_001997B0;
                D_001997B0 ^= D_001997B4;
            }
            if (D_001997B2 > D_001997B6) {
                D_001997B2 ^= D_001997B6;
                D_001997B6 ^= D_001997B2;
                D_001997B2 ^= D_001997B6;
            }
            func_0004EB71();
            D_001997EC = 0;
        }
    } else if (((int)(unsigned char)D_001997EC) == 4) {
        if (((int)(unsigned char)game_mode) != 8) {
            D_001997EC = 0;
            func_0004E2AB((int)(short)(*(short *)inpstr_result - 1));
        }
    }
    D_0012B508 = 146;
    if (mouse_buttons == 0 || (mouse_buttons != 0 && mouse_buttons_prev != 0)) return;
    if (key_pressed_once(72) != 0 || key_pressed_once(73) != 0) func_0004E30F();
    if (key_pressed_once(80) != 0 || key_pressed_once(81) != 0) func_0004E360();
    if (((int)(unsigned char)(mouse_buttons & 1)) != 0 && ((int)(short)mouse_y) > 10 && ((int)(short)mouse_y) < 173) {
        func_0004DABF();
    } else if (D_001997EC == 0 && ((int)(unsigned char)(mouse_buttons & 1)) != 0) {
        for (l_1C = 0; ((int)(short)*(short *)&l_1C) < 14; l_1C++) {
            if (mouse_x > *(short *)(note_buttons + (((int)(short)*(short *)&l_1C) * 12)) && mouse_x < *(short *)(D_0018514C + (((int)(short)*(short *)&l_1C) * 12)) && mouse_y > *(short *)(D_0018514A + (((int)(short)*(short *)&l_1C) * 12)) && mouse_y < *(short *)(D_0018514E + (((int)(short)*(short *)&l_1C) * 12))) {
                ((int (*)())(*(int *)(D_00185150 + (((int)(short)*(short *)&l_1C) * 12))))();
            }
        }
    }
    if (key_down_esc == 0) return;
    note_close();
}
