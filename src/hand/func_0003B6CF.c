/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0003B6CF */
#include "records.h"

struct bf8_2_1 { unsigned char _:2; unsigned char f:1; };
extern short D_0012DA44;
extern char D_00170C67[];
extern char D_00170C6F[];
extern char D_00170C73[];
extern char D_0017B50C[];
extern short D_0017B5A2;
extern short D_0017B5A6;
extern char race_names[];
extern signed char text_buffer[];
extern signed char D_001940D9;
extern char D_00195A08[];
extern struct record *player_entity;
extern int D_00195B64;
extern struct character *player_character;
extern struct career *player_class;
extern void func_0003C81C(void);
extern int object_weight(struct record *);
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
    short *l_34;
    int l_30;
    int l_2C;
    short l_28;
    short l_24;
    short l_20;
    short l_1C;
    short l_18;

    text_draw_colored(player_character->name, 41, 4, 145, 141);
    text_draw_colored(player_class->name, 46, 24, 145, 141);
    l_28 = player_character->level;
    text_draw_colored(func_000A0DD9(l_28, ((char *)text_buffer), 10), 45, 34, 145, 141);
    text_draw_colored(*(char **)(race_names + (player_character->race << 2)), 41, 14, 145, 141);
    func_000A0ED9(180, D_00170C67);
    mc_sprintf(((char *)text_buffer), D_00170C6F, gold_total());
    text_draw_colored(((char *)text_buffer), 39, 44, 145, 141);
    func_000A0ED9(183, D_00170C67);
    mc_sprintf(((char *)text_buffer), D_00170C73, player_character->health, player_character->max_health);
    text_draw_centered_colored(((char *)text_buffer), 72, 64, 145, 141);
    func_000A0ED9(185, D_00170C67);
    mc_sprintf(((char *)text_buffer), D_00170C73, player_character->fatigue >> 6, player_character->attributes[0] + player_character->attributes[4]);
    text_draw_centered_colored(((char *)text_buffer), 77, 54, 145, 141);
    l_1C = ((D_0017B5A2 + D_0017B5A6) >> 1) + 1;
    if (((struct bf8_2_1 *)&D_001940D9)->f) l_34 = player_character->base_attributes;
    else l_34 = player_character->attributes;
    for (l_24 = 0; l_24 < 8; l_24++) {
        if (l_24 == 0 && ((struct bf8_2_1 *)&D_001940D9)->f == 0)
            l_18 = l_34[l_24] + *(short *)D_00195A08;
        else
            l_18 = l_34[l_24];
        if (l_18 < player_character->base_attributes[l_24]) l_20 = 240;
        else if (l_18 > player_character->base_attributes[l_24]) l_20 = 96;
        else l_20 = 145;
        if (l_24 == 0 && ((struct bf8_2_1 *)&D_001940D9)->f == 0)
            text_draw_centered_colored(func_000A0DD9(l_34[l_24] + *(int *)D_00195A08, ((char *)text_buffer), 10), l_1C, (short)((*(short *)(D_0017B50C + (l_24 + 13) * 12) - D_0012DA44) - 2), l_20, 141);
        else
            text_draw_centered_colored(func_000A0DD9(l_34[l_24], ((char *)text_buffer), 10), l_1C, (short)((*(short *)(D_0017B50C + (l_24 + 13) * 12) - D_0012DA44) - 2), l_20, 141);
    }
    func_00144ED8(192, 1, 125, 197, D_00195B64, 0);
    l_2C = object_weight(player_entity) >> 2;
    func_000A0ED9(218, D_00170C67);
    mc_sprintf(((char *)text_buffer), D_00170C73, l_2C, carry_capacity());
    text_draw_colored(((char *)text_buffer), 91, 74, 145, 141);
    func_0003C81C();
}
