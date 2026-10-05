/* profile.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "structs.h"

extern char D_00170129[];

extern int strlen();
extern int mc_memmove();
#pragma aux sosconv "*" parm caller [] value [eax] modify [eax ebx ecx edx];
#pragma aux (sosconv) profile_set_string;

int profile_set_string(struct profile *profile, char *value)
{
    char *cursor;
    char *start;
    int count;
    int new_length;
    int old_length;

    cursor = profile->value;
    if (cursor == 0) return 0;
    old_length = 0;
    while (*cursor == 32) cursor++;
    start = cursor;
    while (*cursor++ != 13) old_length++;
    new_length = strlen(value);
    if (((unsigned)new_length) < old_length) {
        count = (profile->buffer + profile->length - start) - (old_length - new_length);
        mc_memmove(start, (old_length - new_length) + start, count, (int)D_00170129, 870, 4);
        profile->length -= old_length - new_length;
    } else if (((unsigned)new_length) > old_length) {
        if ((new_length - old_length) + profile->length > profile->capacity) {
            return 0;
        }
        count = (profile->buffer + profile->length - start) + (new_length - old_length);
        mc_memmove((new_length - old_length) + start, start, count, (int)D_00170129, 888, 4);
        profile->length += new_length - old_length;
    }
    while (*value != 0) {
        *start++ = *value++;
    }
    profile->flags |= 128;
    return 1;
}
