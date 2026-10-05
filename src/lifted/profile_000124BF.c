/* profile.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern char D_00170129[];

extern int func_000A0DF4();
extern int mc_memmove();
#pragma aux sosconv "*" parm caller [] value [eax] modify [eax ebx ecx edx];
#pragma aux (sosconv) profile_set_string;

int profile_set_string(int a1, int a2)
{
    int l_20;
    int l_1C;
    int l_18;
    int l_14;
    int l_10;

    l_20 = *(int *)((char *)a1 + 160);
    if (l_20 != 0) goto L124E8;
    return 0;
L124E8:;
    l_10 = 0;
L124EF:;
    if (((int)(unsigned char)*(signed char *)((char *)l_20)) != 32) goto L12506;
    l_20++;
    goto L124EF;
L12506:;
    l_1C = l_20;
L1250C:;
    if (((int)(unsigned char)*(signed char *)((char *)l_20++)) == 13) goto L12526;
    l_10++;
    goto L1250C;
L12526:;
    l_14 = func_000A0DF4(a2);
    if (((unsigned)l_14) >= l_10) goto L1258F;
    l_18 = ((int)(*(char **)((char *)a1 + 132) + *(int *)((char *)a1 + 136)) - l_1C) - (l_10 - l_14);
    mc_memmove(l_1C, (l_10 - l_14) + l_1C, l_18, (int)D_00170129, 870, 4);
    *(int *)((char *)a1 + 136) -= l_10 - l_14;
    goto L1260E;
L1258F:;
    if (((unsigned)l_14) <= l_10) goto L1260E;
    if (((unsigned)((l_14 - l_10) + *(int *)((char *)a1 + 136))) <= *(int *)((char *)a1 + 140)) goto L125BD;
    return 0;
L125BD:;
    l_18 = ((int)(*(char **)((char *)a1 + 132) + *(int *)((char *)a1 + 136)) - l_1C) + (l_14 - l_10);
    mc_memmove((l_14 - l_10) + l_1C, l_1C, l_18, (int)D_00170129, 888, 4);
    *(int *)((char *)a1 + 136) += l_14 - l_10;
L1260E:;
    if (*(signed char *)((char *)a2) == 0) goto L12628;
    *(signed char *)((char *)l_1C++) = *(signed char *)((char *)a2++);
    goto L1260E;
L12628:;
    *(signed char *)((char *)a1 + 1) |= 128;
    return 1;
}
