/* profile.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */


#pragma aux sosconv "*" parm caller [] value [eax] modify [eax ebx ecx edx];
#pragma aux (sosconv) profile_find_section;

int profile_find_section(int profile, char *section)
{
    char *cursor;
    char *header;
    char *name;
    int offset;
    int found;

    found = 0;
    cursor = *(char **)((char *)profile + 132);
    offset = 0;
    do {
        if (*cursor == 91) {
            header = cursor;
            cursor++;
            name = section;
            while (*cursor == *name && ((unsigned)offset) < *(int *)((char *)profile + 136)) {
                name++;
                cursor++;
                offset++;
            }
            if (*cursor == 93 && *name == 0) {
                found = 1;
                while (*cursor != 10) cursor++;
                cursor++;
                *(char **)((char *)profile + 168) = cursor;
                *(char **)((char *)profile + 144) = cursor;
                *(int *)((char *)profile + 148) = offset;
                *(char **)((char *)profile + 152) = header;
            }
        }
        cursor++;
        offset++;
    } while (found == 0 && ((unsigned)offset) < *(int *)((char *)profile + 136));
    return found;
}
