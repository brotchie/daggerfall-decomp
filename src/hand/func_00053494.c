/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00053494 */
extern char D_0012B508[];
extern char screen_buffer[];
extern char D_00175420[];
extern char skill_names[];
extern char classmaker_buttons[];
extern char D_001854F6[];
extern char D_001855CC[];
extern char D_001855CE[];
extern char D_001855D0[];
extern char D_001855D2[];
extern char text_buffer[];
extern char D_00190D64[];
extern char D_00190D66[];
extern char D_00190D6A[];
extern char text_macro_fnpc[];
extern char text_macro_fa[];
extern char text_macro_fae[];
extern char player_class[];
extern void msgbox_update(void);
extern void classmaker_draw_dagger(void);
extern void text_draw_colored(int, int, int, int, unsigned char);
extern void text_draw_centered_colored(int, int, int, int, unsigned char);
extern int func_000A0DD9();
extern int mc_memcpy();
extern int func_0012B2EB();
extern int func_0012B3ED();
extern int func_00144E84();
extern int func_00144ED8();
extern int func_00144FB4();

int classmaker_draw(short a1)
{
    short l_24;
    short l_18;
    short l_1C;

    func_0012B2EB();
    mc_memcpy(*(int *)screen_buffer, *(int *)text_macro_fae, 64000, (int)D_00175420, 235, 4);
    classmaker_draw_dagger();
    if (*(short *)D_00190D66 & 2) {
        func_00144ED8(44, (int)(short)*(short *)D_00190D6A, (int)(unsigned short)*(short *)(*(char **)text_macro_fa + 4), (int)(unsigned short)*(short *)(*(char **)text_macro_fa + 6), *(int *)text_macro_fnpc, 0);
        *(signed char *)D_00190D66 &= 253;
    }
    func_00144E84(44, (int)(short)*(short *)D_00190D6A, (int)(unsigned short)*(short *)(*(char **)text_macro_fa + 4), (int)(unsigned short)*(short *)(*(char **)text_macro_fa + 6), *(int *)text_macro_fnpc, 0);
    *(signed char *)D_00190D66 |= 2;
    func_00144FB4(44, (int)(short)*(short *)D_00190D6A, (int)(unsigned short)*(short *)(*(char **)text_macro_fa + 4), (int)(unsigned short)*(short *)(*(char **)text_macro_fa + 6), (int)(*(char **)text_macro_fa + 12));
    text_draw_centered_colored(func_000A0DD9((int)(short)*(short *)D_00190D64, (int)text_buffer, 10), (int)(short)(((((int)(unsigned short)*(short *)(*(char **)text_macro_fa + 4)) + 1) >> 1) + 43), (int)(short)((((int)(short)*(short *)D_00190D6A) + (((int)(unsigned short)*(short *)(*(char **)text_macro_fa + 6)) >> 1)) - 3), 145, 141);
    if (a1 != 0)
        text_draw_colored(*(int *)player_class + 28, 110, 5, 145, 141);
    text_draw_centered_colored(func_000A0DD9(*(unsigned char *)(*(char **)player_class + 52), (int)text_buffer, 10), 287, 55, 145, 141);
    *(signed char *)D_0012B508 = 145;
    for (l_24 = 0; l_24 < 12; l_24++) {
        if (*(unsigned char *)(*(char **)player_class + 16 + l_24) < 35)
            text_draw_colored(*(int *)(skill_names + (*(unsigned char *)(*(char **)player_class + 16 + l_24) << 2)), (short)(*(short *)(classmaker_buttons + (l_24 + 2) * 12) + 2), (short)(*(short *)(D_001854F6 + (l_24 + 2) * 12) + 1), 145, 141);
    }
    l_18 = (*(short *)D_001855CC + *(short *)D_001855D0) >> 1;
    l_1C = ((*(short *)D_001855D2 + *(short *)D_001855CE) >> 1) - *(short *)D_001855CE + 3;
    for (l_24 = 0; l_24 < 8; l_24++) {
        text_draw_centered_colored(func_000A0DD9(*(short *)(*(char **)player_class + l_24 * 2 + 58), (int)text_buffer, 10), l_18, (short)(*(short *)(D_001854F6 + (l_24 + 18) * 12) + l_1C), 145, 141);
    }
    msgbox_update();
    func_0012B3ED();
    return 0;
}
