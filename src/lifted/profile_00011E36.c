/* profile.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "structs.h"


#pragma aux sosconv "*" parm caller [] value [eax] modify [eax ebx ecx edx];
#pragma aux (sosconv) profile_find_item;

int profile_find_item(struct profile *profile, char *item)
{
    char *cursor;
    char *start;
    char *name;
    int offset;
    int found;

    found = 0;
    cursor = profile->section;
    offset = profile->section_offset;
    do {
        name = item;
        if (*cursor == *name) {
            start = cursor;
            cursor++;
            name++;
            offset++;
            while (*cursor == *name && offset < profile->length) {
                cursor++;
                name++;
                offset++;
            }
            if (*name == 0) {
                while (*cursor != 61 && *cursor != 13) {
                    cursor++;
                    offset++;
                }
                if (*cursor == 61) {
                    cursor++;
                    offset++;
                    profile->value = cursor;
                } else {
                    profile->value = 0;
                }
                profile->item = start;
                profile->next_value = 0;
                found = 1;
            }
        }
        cursor++;
        offset++;
    } while (found == 0 && offset < profile->length && *cursor != 91);
    return found;
}
