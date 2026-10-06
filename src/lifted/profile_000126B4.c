/* profile.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "structs.h"


extern int profile_find_item(struct profile *, char *);
#pragma aux sosconv "*" parm caller [] value [eax] modify [eax ebx ecx edx];
#pragma aux (sosconv) profile_find_item;
extern int profile_get_number(struct profile *, int *);
#pragma aux (sosconv) profile_get_number;
#pragma aux (sosconv) profile_get_item_number;

int profile_get_item_number(struct profile *profile, char *item, int *number)
{
    if ((short)profile_find_item(profile, item) == 0) return 0;
    if ((short)profile_get_number(profile, number) == 0) return 0;
    return 1;
}
