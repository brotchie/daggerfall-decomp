/* itemmakr.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern char enchant_spell_lists[];
extern char D_0018598A[];
extern char D_00187966[];

extern int enchant_spell_cost(unsigned char);

int enchant_slot_cost(int a1, unsigned char a2, unsigned char a3, int a4)
{
    int l_1C;

    if (a1 != 99) goto L57C14;
    l_1C = *(int *)(D_00187966 + (((int)(unsigned char)a2) << 2));
    if (a3 == 0) goto L57BF5;
    l_1C = l_1C / 100;
L57BF5:;
    if (l_1C != 10) goto L57C09;
    l_1C = l_1C * ((int)(unsigned char)a2);
L57C09:;
    return -(l_1C);
L57C14:;
    if (a1 < 5) goto L57C20;
    if (a1 <= 7) goto L57C22;
L57C20:;
    goto L57C46;
L57C22:;
    return enchant_spell_cost((int)(unsigned char)*(signed char *)((char *)(int)(*(char **)(enchant_spell_lists + (a4 << 2)) + ((int)(unsigned char)a2))));
L57C46:;
    return (int)(short)*(short *)(D_0018598A + (a1 * 2));
}
