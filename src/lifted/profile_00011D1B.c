/* profile.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */


#pragma aux sosconv "*" parm caller [] value [eax] modify [eax ebx ecx edx];
#pragma aux (sosconv) profile_find_section;

int profile_find_section(int a1, int a2)
{
    int l_20;
    int l_1C;
    int l_18;
    int l_14;
    int l_10;

    l_10 = 0;
    l_20 = *(int *)((char *)a1 + 132);
    l_14 = 0;
    do {
        if (((int)(unsigned char)*(signed char *)((char *)l_20)) == 91) {
            l_1C = l_20;
            l_20++;
            l_18 = a2;
            while (*(signed char *)((char *)l_20) == *(signed char *)((char *)l_18) && ((unsigned)l_14) < *(int *)((char *)a1 + 136)) {
                l_18++;
                l_20++;
                l_14++;
            }
            if (((int)(unsigned char)*(signed char *)((char *)l_20)) == 93 && *(signed char *)((char *)l_18) == 0) {
                l_10 = 1;
                while (((int)(unsigned char)*(signed char *)((char *)l_20)) != 10) l_20++;
                l_20++;
                *(int *)((char *)a1 + 168) = l_20;
                *(int *)((char *)a1 + 144) = l_20;
                *(int *)((char *)a1 + 148) = l_14;
                *(int *)((char *)a1 + 152) = l_1C;
            }
        }
        l_20++;
        l_14++;
    } while (l_10 == 0 && ((unsigned)l_14) < *(int *)((char *)a1 + 136));
    return l_10;
}
