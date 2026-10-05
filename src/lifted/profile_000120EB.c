/* profile.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */


#pragma aux sosconv "*" parm caller [] value [eax] modify [eax ebx ecx edx];
#pragma aux (sosconv) profile_get_string;

int profile_get_string(int a1, int a2, int a3)
{
    int l_14;
    int l_10;

    if (*(int *)((char *)a1 + 164) == 0) goto L12110;
    l_14 = *(int *)((char *)a1 + 164);
    goto L1211C;
L12110:;
    l_14 = *(int *)((char *)a1 + 160);
L1211C:;
    if (l_14 != 0) goto L1212E;
    return 0;
L1212E:;
    l_10 = 0;
L12135:;
    if (((int)(unsigned char)*(signed char *)((char *)l_14)) != 32) goto L1214C;
    l_14++;
    goto L12135;
L1214C:;
    if (((int)(unsigned char)*(signed char *)((char *)l_14)) == 13) goto L1216A;
    if (((int)(unsigned char)*(signed char *)((char *)l_14)) != 44) goto L1216C;
L1216A:;
    goto L12175;
L1216C:;
    if (((unsigned)(a3 - 1)) > l_10) goto L12177;
L12175:;
    goto L1218C;
L12177:;
    *(signed char *)((char *)(l_10++ + a2)) = *(signed char *)((char *)l_14++);
    goto L1214C;
L1218C:;
    *(signed char *)((char *)(a2 + l_10)) = 0;
    if ((a3 - 1) != l_10) goto L121C6;
L1219E:;
    if (((int)(unsigned char)*(signed char *)((char *)l_14)) == 13) goto L121BC;
    if (((int)(unsigned char)*(signed char *)((char *)l_14)) != 44) goto L121BE;
L121BC:;
    goto L121C6;
L121BE:;
    l_14++;
    goto L1219E;
L121C6:;
    if (((int)(unsigned char)*(signed char *)((char *)l_14)) != 44) goto L121E6;
    ++l_14;
    *(int *)((char *)a1 + 164) = l_14;
    goto L121F2;
L121E6:;
    *(int *)((char *)a1 + 164) = l_14;
L121F2:;
    return 1;
}
