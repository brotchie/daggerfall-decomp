/* song.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern char D_001706B6[];
extern char D_001706BD[];
extern char D_001706CA[];
extern char D_001706CD[];
extern char D_00179EA8[];
extern char text_buffer[];
extern char D_00190CA2[];
extern char D_00190CD4[];
extern char D_00190CD8[];
extern char D_00190CDC[];
extern char D_00190D1F[];
extern char D_00190D20[];
extern char D_00190D21[];
extern char D_00190D22[];
extern char D_00190D64[];
extern char text_rsc_buffer[];
extern char player_character[];
extern char D_00195C44[];

extern int rand_range(int, int);
extern int rand();
extern int srand();
extern int mc_strncpy();
extern int func_000A0ED9(int, int);
extern int mc_sprintf(int, ...);
extern int func_000A1054();
extern int func_000CE790();
extern void parse_rsc_text(int, int, int);
extern void object_free_later(int);
#pragma aux func_000A0ED9 parm routine [];

void func_000209F3(int a1)
{
    int l_1C;
    int l_18;

    if (a1 != 0) return;
    l_1C = rand();
    srand(l_1C);
    parse_rsc_text(850, 0, 0);
    mc_strncpy((int)text_buffer, (int)text_rsc_buffer, 160, (int)D_001706B6, 42);
    parse_rsc_text(851, 0, 0);
    func_000A1054((int)text_buffer, (int)text_rsc_buffer, (int)D_001706B6, 44, 160);
    func_000A0ED9(45, (int)D_001706B6);
    mc_sprintf((int)text_rsc_buffer, (int)D_001706BD, (int)text_buffer, l_1C);
    mc_strncpy(*(int *)D_00195C44 + 50000, (int)text_rsc_buffer, 4, (int)D_001706B6, 46);
    l_1C = 0;
L20AC5:;
    if (l_1C < 26) goto L20AD5;
    goto L20AF6;
L20ACD:;
    l_1C++;
    goto L20AC5;
L20AD5:;
    *(short *)(D_00190D64 + (l_1C * 2)) = rand_range(0, 21) + 900;
    goto L20ACD;
L20AF6:;
    l_1C = rand() % 10;
    l_18 = func_000CE790(*(int *)D_00179EA8, 33, l_1C);
L20B21:;
    if (((int)(unsigned char)*(signed char *)((char *)l_18)) == 33) goto L20B91;
    parse_rsc_text((int)(short)*(short *)(D_00190CA2 + (((int)(unsigned char)*(signed char *)((char *)l_18++)) * 2)), 0, 0);
    func_000A1054(*(int *)D_00195C44 + 50000, (int)text_rsc_buffer, (int)D_001706B6, 56, 4);
    func_000A1054(*(int *)D_00195C44 + 50000, (int)D_001706CA, (int)D_001706B6, 57, 4);
    goto L20B21;
L20B91:;
    func_000A1054(*(int *)D_00195C44 + 50000, (int)D_001706CD, (int)D_001706B6, 59, 4);
}

void func_00020BBB(int a1)
{
{
    int l_1C;

    *(signed char *)D_00190D1F = rand() & -255;
    if (a1 == 0) goto L20BF9;
    *(int *)D_00190CD4 = 0;
    *(signed char *)D_00190D20 = *(signed char *)(*(char **)player_character + 64) & 1;
    goto L20C0F;
L20BF9:;
    *(int *)D_00190CD4 = rand();
    *(signed char *)D_00190D20 = rand() & -255;
L20C0F:;
    *(int *)D_00190CD8 = rand();
    if (*(signed char *)D_00190D20 == 0) goto L20C2B;
    l_1C = 0;
    goto L20C32;
L20C2B:;
    l_1C = 1;
L20C32:;
    *(signed char *)D_00190D21 = *(signed char *)&l_1C;
    *(int *)D_00190CDC = rand();
    *(signed char *)D_00190D22 = 0;
}
}

void crime_remove_monster(int a1)
{
    if (((int)(unsigned char)*(signed char *)((char *)a1)) != 18) return;
    object_free_later(a1);
}
