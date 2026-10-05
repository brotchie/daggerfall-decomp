/* profile.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */


#pragma aux sosconv "*" parm caller [] value [eax] modify [eax ebx ecx edx];
#pragma aux (sosconv) profile_get_string;

int profile_get_string(int a1, int a2, int a3)
{
    int l_14;
    int l_10;

    if (*(int *)((char *)a1 + 164) != 0) {
        l_14 = *(int *)((char *)a1 + 164);
    } else {
        l_14 = *(int *)((char *)a1 + 160);
    }
    if (l_14 == 0) return 0;
    l_10 = 0;
    while (((int)(unsigned char)*(signed char *)((char *)l_14)) == 32) l_14++;
    while (((int)(unsigned char)*(signed char *)((char *)l_14)) != 13 && ((int)(unsigned char)*(signed char *)((char *)l_14)) != 44 && ((unsigned)(a3 - 1)) > l_10) {
        *(signed char *)((char *)(l_10++ + a2)) = *(signed char *)((char *)l_14++);
    }
    *(signed char *)((char *)(a2 + l_10)) = 0;
    if ((a3 - 1) == l_10) {
        while (((int)(unsigned char)*(signed char *)((char *)l_14)) != 13 && ((int)(unsigned char)*(signed char *)((char *)l_14)) != 44) {
            l_14++;
        }
    }
    if (((int)(unsigned char)*(signed char *)((char *)l_14)) == 44) {
        ++l_14;
        *(int *)((char *)a1 + 164) = l_14;
    } else {
        *(int *)((char *)a1 + 164) = l_14;
    }
    return 1;
}
