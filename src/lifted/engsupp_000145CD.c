/* engsupp.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern char player_environment[];
extern char D_0017995C[];
extern char D_00179966[];
extern char D_00179970[];
extern char D_0017998E[];
extern char itemmaker_slot_kinds[];
extern char D_00190CE5[];
extern char climate_weathers[];

extern int func_000CE45E();

short texture_archive_for_climate(int a1, int a2)
{
    int l_1C;
    int l_18;

    l_18 = a1 % 100;
    switch (*(unsigned char *)player_environment) {
case 1:
    if (func_000CE45E((int)D_00179970, (int)(short)*(short *)&l_18, 15) == 0) goto L1468B;
    if (l_18 != 74) goto L14647;
    if (a2 > 2) goto L14649;
L14647:;
    goto L14654;
L14649:;
    return a1;
L14654:;
    a1 = l_18 + (((int)(signed char)*(signed char *)itemmaker_slot_kinds) * 100);
    if (((int)(unsigned char)*(signed char *)(climate_weathers + ((int)(signed char)*(signed char *)D_00190CE5))) != 5) goto L14683;
    if (l_18 != 74) goto L14685;
L14683:;
    goto L1468B;
L14685:;
    a1++;
L1468B:;
    return a1;
case 2:
    if (func_000CE45E((int)D_0017998E, (int)(short)*(short *)&l_18, 15) == 0) goto L146E5;
    if (l_18 != 74) goto L146B9;
    if (a2 > 2) goto L146BB;
L146B9:;
    goto L146C6;
L146BB:;
    return a1;
L146C6:;
    a1 = (a1 % 100) + (((int)(signed char)*(signed char *)itemmaker_slot_kinds) * 100);
L146E5:;
    return a1;
case 3:
    if (l_18 != 74) goto L146FC;
    if (a2 > 2) goto L146FE;
L146FC:;
    goto L14709;
L146FE:;
    return a1;
L14709:;
    if (l_18 != 74) goto L14724;
    return l_18 + ((short)*(signed char *)itemmaker_slot_kinds * 100);
L14724:;
    l_1C = func_000CE45E((int)D_0017995C, (int)(short)*(short *)&a1, 5);
    if (l_1C == 0) goto L1475C;
    a1 = (int)(short)*(short *)(D_00179966 + (((l_1C - ((int)D_0017995C)) >> 1) * 2));
    goto L14775;
L1475C:;
    if (a1 != 168) goto L14775;
    a1 = (((int)(signed char)*(signed char *)itemmaker_slot_kinds) * 100) + 68;
L14775:;
    return a1;
default:
    return a1;
}
}
