/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00096CCF */
#include "records.h"

extern char D_0017704C[];
extern struct record *D_00195AC4;
extern struct spell *spell_records;
extern char D_00195B44[];
extern struct character *player_character;
extern char game_minutes[];
extern int spell_cost(int, int);
extern int cast_item_spell_at();
extern void item_damage(int, int);
extern int object_create_child(int, int, int);
extern int mc_memcpy();

struct S89 { char p[73]; unsigned char f; char q[15]; };
struct E4 { short t; short v; };

void item_apply_equip_effects(int a1, int a2)
{
    int l_24;
    int l_20;
    int l_1C;
    int l_18;
    int l_14;

    l_24 = 0;
    l_1C = a1 + 71;
    l_14 = 0;
L96CF9:;
    if (l_24 >= 10) goto L96D11;
    if (((int)(short)*(short *)((char *)((l_24 << 2) + l_1C) + 67)) != (-1)) goto L96D16;
L96D11:;
    goto L96E9B;
L96D16:;
    switch (*(unsigned short *)((char *)((l_24 << 2) + l_1C) + 67)) {
case 1:
    l_20 = 0;
L96D71:;
    if ((*(struct S89 **)((char *)&spell_records))[l_20].f == ((struct E4 *)(l_1C + 67))[l_24].v) goto L96D98;
    l_20++;
    goto L96D71;
L96D98:;
    l_18 = object_create_child((int)D_00195AC4, 0, 89);
    *(signed char *)((char *)l_18) = 9;
    *(short *)((char *)l_18 + 21) = 3;
    mc_memcpy(l_18 + 71, (int)((char *)spell_records + (l_20 * 89)), 89, (int)D_0017704C, 2092, 4);
    l_14 = l_18 + 71;
    *(signed char *)((char *)l_14 + 72) = *(signed char *)&a2 + 200;
    l_20 = 0;
L96DFD:;
    if (l_20 < 3) goto L96E0D;
    goto L96E4C;
L96E05:;
    l_20++;
    goto L96DFD;
L96E0D:;
    if (((int)(unsigned char)*(signed char *)((char *)((l_20 * 2) + l_14))) == 255) goto L96E4A;
    *(signed char *)((char *)((l_20 * 3) + l_14) + 14) = 255;
    *(signed char *)((char *)((l_20 * 3) + l_14) + 15) = 0;
    *(signed char *)((char *)((l_20 * 3) + l_14) + 16) = 0;
L96E4A:;
    goto L96E05;
L96E4C:;
    cast_item_spell_at(l_18);
    goto L96E90;
case 5:
    *(int *)D_00195B44 = *(int *)game_minutes;
    goto L96E90;
case 9:
    player_character->conditions |= 0x200;
    goto L96E90;
case 10:
    player_character->skills[(int)(short)*(short *)((char *)((l_24 << 2) + l_1C) + 69)].value += 15;
default:
L96E90:;
    l_24++;
    goto L96CF9;
L96E9B:;
    if (l_24 == 0) goto L96EA7;
    if (l_14 != 0) goto L96EA9;
L96EA7:;
    return;
L96EA9:;
    item_damage(a1, spell_cost(l_14, (int)player_character));
}
}
