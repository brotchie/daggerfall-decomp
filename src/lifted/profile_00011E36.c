/* profile.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */


#pragma aux sosconv "*" parm caller [] value [eax] modify [eax ebx ecx edx];
#pragma aux (sosconv) profile_find_item;

int profile_find_item(int profile, char *item)
{
    char *cursor;
    char *start;
    char *name;
    int offset;
    int found;

    found = 0;
    cursor = *(char **)((char *)profile + 144);
    offset = *(int *)((char *)profile + 148);
    do {
        name = item;
        if (*cursor == *name) {
            start = cursor;
            cursor++;
            name++;
            offset++;
            while (*cursor == *name && ((unsigned)offset) < *(int *)((char *)profile + 136)) {
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
                    *(char **)((char *)profile + 160) = cursor;
                } else {
                    *(int *)((char *)profile + 160) = 0;
                }
                *(char **)((char *)profile + 156) = start;
                *(int *)((char *)profile + 164) = 0;
                found = 1;
            }
        }
        cursor++;
        offset++;
    } while (found == 0 && ((unsigned)offset) < *(int *)((char *)profile + 136) && *cursor != 91);
    return found;
}
