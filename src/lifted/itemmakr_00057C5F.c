/* itemmakr.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern char D_0018598A[];
extern char D_00187966[];

extern int enchant_spell_cost(unsigned char);

int enchant_value_slot_cost(int a1, unsigned char a2, unsigned char a3, int a4)
{
    int l_1C;

    if (a1 != 99) goto L57CC5;
    l_1C = *(int *)(D_00187966 + (((int)(unsigned char)a2) << 2));
    if (a3 == 0) goto L57CA6;
    l_1C = l_1C / 100;
L57CA6:;
    if (l_1C != 10) goto L57CBA;
    l_1C = l_1C * ((int)(unsigned char)a2);
L57CBA:;
    return -(l_1C);
L57CC5:;
    if (a1 < 5) goto L57CD1;
    if (a1 <= 7) goto L57CD3;
L57CD1:;
    goto L57CE2;
L57CD3:;
    return enchant_spell_cost((int)(unsigned char)a2);
L57CE2:;
    return (int)(short)*(short *)(D_0018598A + (a1 * 2));
}
