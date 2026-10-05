/* profile.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "structs.h"


#pragma aux sosconv "*" parm caller [] value [eax] modify [eax ebx ecx edx];
#pragma aux (sosconv) profile_get_string;

int profile_get_string(struct profile *profile, char *value, int size)
{
    char *cursor;
    int length;

    if (profile->next_value != 0) {
        cursor = profile->next_value;
    } else {
        cursor = profile->value;
    }
    if (cursor == 0) return 0;
    length = 0;
    while (*cursor == 32) cursor++;
    while (*cursor != 13 && *cursor != 44 && ((unsigned)(size - 1)) > length) {
        value[length++] = *cursor++;
    }
    value[length] = 0;
    if ((size - 1) == length) {
        while (*cursor != 13 && *cursor != 44) {
            cursor++;
        }
    }
    if (*cursor == 44) {
        ++cursor;
        profile->next_value = cursor;
    } else {
        profile->next_value = cursor;
    }
    return 1;
}
