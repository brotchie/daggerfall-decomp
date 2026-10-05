/* profile.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */


extern int profile_find_item(int, ...);
extern int profile_get_number(int, ...);
#pragma aux sosconv "*" parm caller [] value [eax] modify [eax ebx ecx edx];
#pragma aux (sosconv) profile_get_item_number;

int profile_get_item_number(int a1, int a2, int a3)
{
    if ((short)profile_find_item(a1, a2) == 0) return 0;
    if ((short)profile_get_number(a1, a3) == 0) return 0;
    return 1;
}
