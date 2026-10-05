/* profile.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "structs.h"


#pragma aux sosconv "*" parm caller [] value [eax] modify [eax ebx ecx edx];
#pragma aux (sosconv) profile_find_section;

int profile_find_section(struct profile *profile, char *section)
{
    char *cursor;
    char *header;
    char *name;
    int offset;
    int found;

    found = 0;
    cursor = profile->buffer;
    offset = 0;
    do {
        if (*cursor == 91) {
            header = cursor;
            cursor++;
            name = section;
            while (*cursor == *name && offset < profile->length) {
                name++;
                cursor++;
                offset++;
            }
            if (*cursor == 93 && *name == 0) {
                found = 1;
                while (*cursor != 10) cursor++;
                cursor++;
                profile->line = cursor;
                profile->section = cursor;
                profile->section_offset = offset;
                profile->section_header = header;
            }
        }
        cursor++;
        offset++;
    } while (found == 0 && offset < profile->length);
    return found;
}
