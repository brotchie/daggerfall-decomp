/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0003B6CF */
struct bf8_2_1 { unsigned char _:2; unsigned char f:1; };
extern char D_0012DA44[];
extern char D_00170C67[];
extern char D_00170C6F[];
extern char D_00170C73[];
extern char D_0017B50C[];
extern char D_0017B5A2[];
extern char D_0017B5A6[];
extern char race_names[];
extern char text_buffer[];
extern char D_001940D9[];
extern char D_00195A08[];
extern char player_entity[];
extern char D_00195B64[];
extern char player_character[];
extern char player_class[];
extern void func_0003C81C(void);
extern int object_weight(int);
extern void text_draw_colored(char *, int, int, int, unsigned char);
extern void text_draw_centered_colored(char *, int, int, int, unsigned char);
extern int gold_total(void);
extern int carry_capacity(void);
extern char *func_000A0DD9(int, char *, int);
extern int func_00144ED8();
#pragma aux func_000A0ED9 parm routine [];
extern int func_000A0ED9(int, char *);
extern int mc_sprintf(char *, char *, ...);

void sheet_draw(void)
{
    int l_34;
    int l_30;
    int l_2C;
    short l_28;
    short l_24;
    short l_20;
    short l_1C;
    short l_18;

    text_draw_colored(*(char **)player_character, 41, 4, 145, 141);
    text_draw_colored(*(char **)player_class + 28, 46, 24, 145, 141);
    l_28 = *(unsigned char *)(*(char **)player_character + 129);
    text_draw_colored(func_000A0DD9(l_28, text_buffer, 10), 45, 34, 145, 141);
    text_draw_colored(*(char **)(race_names + (((int)(unsigned char)*(signed char *)(*(char **)player_character + 67)) << 2)), 41, 14, 145, 141);
    func_000A0ED9(180, D_00170C67);
    mc_sprintf(text_buffer, D_00170C6F, gold_total());
    text_draw_colored(text_buffer, 39, 44, 145, 141);
    func_000A0ED9(183, D_00170C67);
    mc_sprintf(text_buffer, D_00170C73, (int)(short)*(short *)(*(char **)player_character + 124), (int)(short)*(short *)(*(char **)player_character + 126));
    text_draw_centered_colored(text_buffer, 72, 64, 145, 141);
    func_000A0ED9(185, D_00170C67);
    mc_sprintf(text_buffer, D_00170C73, ((int)(unsigned short)*(short *)(*(char **)player_character + 155)) >> 6, ((int)(short)*(short *)(*(char **)player_character + 32)) + ((int)(short)*(short *)(*(char **)player_character + 40)));
    text_draw_centered_colored(text_buffer, 77, 54, 145, 141);
    l_1C = ((*(short *)D_0017B5A2 + *(short *)D_0017B5A6) >> 1) + 1;
    if (((struct bf8_2_1 *)&D_001940D9)->f) l_34 = *(int *)player_character + 48;
    else l_34 = *(int *)player_character + 32;
    for (l_24 = 0; l_24 < 8; l_24++) {
        if (l_24 == 0 && ((struct bf8_2_1 *)&D_001940D9)->f == 0)
            l_18 = *(short *)((char *)(l_24 * 2) + l_34) + *(short *)D_00195A08;
        else
            l_18 = *(short *)((char *)(l_24 * 2) + l_34);
        if (l_18 < *(short *)(*(char **)player_character + 48 + l_24 * 2)) l_20 = 240;
        else if (l_18 > *(short *)(*(char **)player_character + 48 + l_24 * 2)) l_20 = 96;
        else l_20 = 145;
        if (l_24 == 0 && ((struct bf8_2_1 *)&D_001940D9)->f == 0)
            text_draw_centered_colored(func_000A0DD9(*(short *)((char *)(l_24 * 2) + l_34) + *(int *)D_00195A08, text_buffer, 10), l_1C, (short)((*(short *)(D_0017B50C + (l_24 + 13) * 12) - *(short *)D_0012DA44) - 2), l_20, 141);
        else
            text_draw_centered_colored(func_000A0DD9(*(short *)((char *)(l_24 * 2) + l_34), text_buffer, 10), l_1C, (short)((*(short *)(D_0017B50C + (l_24 + 13) * 12) - *(short *)D_0012DA44) - 2), l_20, 141);
    }
    func_00144ED8(192, 1, 125, 197, *(int *)D_00195B64, 0);
    l_2C = object_weight(*(int *)player_entity) >> 2;
    func_000A0ED9(218, D_00170C67);
    mc_sprintf(text_buffer, D_00170C73, l_2C, carry_capacity());
    text_draw_colored(text_buffer, 91, 74, 145, 141);
    func_0003C81C();
}
