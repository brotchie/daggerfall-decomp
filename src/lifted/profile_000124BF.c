/* profile.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern char D_00170129[];

extern int strlen();
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
    if (l_20 == 0) return 0;
    l_10 = 0;
    while (((int)(unsigned char)*(signed char *)((char *)l_20)) == 32) l_20++;
    l_1C = l_20;
    while (((int)(unsigned char)*(signed char *)((char *)l_20++)) != 13) l_10++;
    l_14 = strlen(a2);
    if (((unsigned)l_14) < l_10) {
        l_18 = ((int)(*(char **)((char *)a1 + 132) + *(int *)((char *)a1 + 136)) - l_1C) - (l_10 - l_14);
        mc_memmove(l_1C, (l_10 - l_14) + l_1C, l_18, (int)D_00170129, 870, 4);
        *(int *)((char *)a1 + 136) -= l_10 - l_14;
    } else if (((unsigned)l_14) > l_10) {
        if (((unsigned)((l_14 - l_10) + *(int *)((char *)a1 + 136))) > *(int *)((char *)a1 + 140)) {
            return 0;
        }
        l_18 = ((int)(*(char **)((char *)a1 + 132) + *(int *)((char *)a1 + 136)) - l_1C) + (l_14 - l_10);
        mc_memmove((l_14 - l_10) + l_1C, l_1C, l_18, (int)D_00170129, 888, 4);
        *(int *)((char *)a1 + 136) += l_14 - l_10;
    }
    while (*(signed char *)((char *)a2) != 0) {
        *(signed char *)((char *)l_1C++) = *(signed char *)((char *)a2++);
    }
    *(signed char *)((char *)a1 + 1) |= 128;
    return 1;
}
