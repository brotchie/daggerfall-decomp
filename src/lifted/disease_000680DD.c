/* disease.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

struct bf8_0_2 { unsigned char f:2; };
extern char player_entity[];
extern char D_00195AA8[];
extern char D_00195B08[];
extern char D_00195B84[];
extern char player_character[];
extern char game_minutes[];
extern char nearest_creature_distance[];
extern char nearest_creature[];
extern char D_001A3AA4[];
extern char D_001A3AA8[];

extern int damage_apply(int, int, int);
extern int player_in_daylight(void);
extern int player_in_temple(void);
extern int rand();
extern void item_damage(int, int);
extern void enchant_extra_spell_points(int, int);
extern void object_foreach(int, int);
extern void item_repair_cb(int);
extern void item_break(int);

void item_enchantment_tick(int a1, int a2, int a3)
{
    int l_10;

    if (a2 == 3) goto L68101;
    if (*(int *)D_00195B08 == 0) goto L68103;
L68101:;
    goto L68108;
L68103:;
    return;
L68108:;
    switch ((unsigned)a2) {
    goto L684C4;
case 3:
    *(int *)D_001A3AA4 = 0;
    enchant_extra_spell_points(a1, a3);
    *(short *)(*(char **)player_character + 143) += *(short *)D_001A3AA4;
    *(int *)D_001A3AA8 += *(int *)D_001A3AA4;
    if (*(int *)D_001A3AA4 == 0) goto L681C1;
    if (a3 >= 7) goto L681C3;
L681C1:;
    goto L681C9;
L681C3:;
    if (a3 <= 10) goto L681CB;
L681C9:;
    goto L681E6;
L681CB:;
    if (*(short *)(*(char **)player_character + 141) < *(short *)(*(char **)player_character + 143)) goto L681E8;
L681E6:;
    goto L681FC;
L681E8:;
    *(short *)(*(char **)player_character + 141) += *(short *)D_00195B08 * 5;
L681FC:;
    if (*(int *)D_00195B08 == 0) goto L6820E;
    if (((struct bf8_0_2 *)&game_minutes)->f == 0) goto L68210;
L6820E:;
    goto L6821F;
L68210:;
    item_damage(*(int *)D_00195AA8, 1);
L6821F:;
    goto L684C4;
case 5:
    switch ((unsigned)a3) {
case 1:
    if (player_in_daylight() == 0) return;
    goto L68256;
case 2:
    if (player_in_daylight() != 0) return;
default:
L68256:;
    *(short *)(*(char **)player_character + 124) += *(short *)D_00195B08;
    if (*(short *)(*(char **)player_character + 124) <= *(short *)(*(char **)player_character + 126)) goto L68290;
    *(short *)(*(char **)player_character + 124) = *(short *)(*(char **)player_character + 126);
    goto L682B6;
L68290:;
    if ((rand() % 10) != 0) goto L682B6;
    item_damage(*(int *)D_00195AA8, 1);
L682B6:;
    goto L684C4;
}
case 17:
    if (a3 != 0) goto L682CA;
    if (player_in_daylight() != 0) goto L682DB;
L682CA:;
    if (a3 == 0) goto L682D9;
    if (player_in_temple() != 0) goto L682DB;
L682D9:;
    goto L682ED;
L682DB:;
    damage_apply(*(int *)player_entity, *(int *)D_00195B08, 0);
L682ED:;
    goto L684C4;
case 16:
    switch ((unsigned)a3) {
case 1:
    if (player_in_daylight() == 0) return;
    goto L68324;
case 2:
    if (player_in_temple() == 0) return;
default:
L68324:;
    if (((int)(unsigned short)*(short *)((char *)a1 + 44)) <= *(int *)D_00195B08) goto L68348;
    *(short *)((char *)a1 + 44) -= *(short *)D_00195B08;
    goto L68352;
L68348:;
    item_break(*(int *)D_00195AA8);
L68352:;
    goto L684C4;
}
case 21:
    switch ((unsigned)a3) {
case 1:
    if (*(int *)D_00195B08 == 0) goto L68394;
    if (((unsigned)(*(int *)game_minutes - *(int *)(*(char **)player_character + 509))) > 1440) goto L68396;
L68394:;
    goto L683A8;
L68396:;
    damage_apply(*(int *)player_entity, *(int *)D_00195B08, 0);
L683A8:;
    goto L683DF;
case 2:
    if (*(int *)D_00195B08 == 0) goto L683CB;
    if (((unsigned)(*(int *)game_minutes - *(int *)(*(char **)player_character + 509))) > 10080) goto L683CD;
L683CB:;
    goto L683DF;
L683CD:;
    damage_apply(*(int *)player_entity, *(int *)D_00195B08, 0);
default:
L683DF:;
    goto L684C4;
}
case 8:
    *(int *)D_00195B84 = *(int *)D_00195B08;
    object_foreach(*(int *)(*(char **)player_entity + 63), (int)item_repair_cb);
    if ((rand() % 10) != 0) goto L68426;
    item_damage(*(int *)D_00195AA8, 1);
L68426:;
    goto L684C4;
case 1:
    item_damage(*(int *)D_00195AA8, 1);
    goto L684C4;
case 6:
    if (a3 != 0) goto L68451;
    if (*(int *)nearest_creature_distance < 128) goto L68453;
L68451:;
    goto L68468;
L68453:;
    if (*(short *)(*(char **)player_character + 124) != *(short *)(*(char **)player_character + 126)) goto L6846A;
L68468:;
    goto L684C4;
L6846A:;
    damage_apply(*(int *)nearest_creature, *(int *)D_00195B08, 0);
    *(short *)(*(char **)player_character + 124) += *(short *)D_00195B08;
    if (*(short *)(*(char **)player_character + 124) <= *(short *)(*(char **)player_character + 126)) goto L684B4;
    *(short *)(*(char **)player_character + 124) = *(short *)(*(char **)player_character + 126);
L684B4:;
    item_damage(*(int *)D_00195AA8, *(int *)D_00195B08);
default:
L684C4:;
    if (*(short *)(*(char **)player_character + 141) >= 0) return;
    *(short *)(*(char **)player_character + 141) = 0;
}
}
