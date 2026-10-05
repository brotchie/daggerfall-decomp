/* profile.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern char D_00170129[];
extern char D_00170133[];
extern char D_00170137[];
extern char D_0017013B[];
extern int D_00178848[];

extern int profile_find_section(int, ...);
extern int profile_find_item(int, ...);
extern int profile_get_string(int, ...);
extern int profile_set_string(int, ...);
extern int strlen();
extern int mc_memmove();
extern int stricmp();
extern int toupper();
int profile_hex_digit(signed char);
#pragma aux sosconv "*" parm caller [] value [eax] modify [eax ebx ecx edx];
#pragma aux (sosconv) profile_get_raw_line;
#pragma aux (sosconv) profile_get_yes;
#pragma aux (sosconv) profile_get_item_string;
#pragma aux (sosconv) profile_set_yes_no;
#pragma aux (sosconv) profile_delete_section;
#pragma aux (sosconv) profile_add_section;

int profile_get_raw_line(int profile, char *line, int size)
{
    char *cursor;
    char *end;
    int length;

    cursor = *(char **)((char *)profile + 168);
    if (cursor == 0 || *cursor == 91 || *cursor == 13) {
        return 0;
    }
    end = *(char **)((char *)profile + 132) + *(int *)((char *)profile + 136);
    length = 0;
    while (*cursor == 32) cursor++;
    while (*cursor != 13 && ((unsigned)(size - 1)) > length) {
        line[length++] = *cursor++;
    }
    line[length] = 0;
    cursor += 2;
    if (cursor >= end) {
        *(int *)((char *)profile + 168) = 0;
    } else {
        *(char **)((char *)profile + 168) = cursor;
    }
    return 1;
}

int profile_get_yes(int profile, char *item)
{
    char value[32];

    if ((short)profile_find_item(profile, item) == 0) return 0;
    if ((short)profile_get_string(profile, (int)value, 32) == 0) return 0;
    if (stricmp((int)value, (int)D_00170133) == 0) return 1;
    return 0;
}

int profile_get_item_string(int profile, char *item, char *value, int size)
{
    if ((short)profile_find_item(profile, item) == 0) return 0;
    if ((short)profile_get_string(profile, value, size) == 0) return 0;
    return 1;
}

int profile_set_yes_no(int profile, char *item, short yes)
{
    if ((short)profile_find_item(profile, item) == 0) return 0;
    if (yes != 0) {
        if ((short)profile_set_string(profile, (int)D_00170137) == 0) return 0;
    } else if ((short)profile_set_string(profile, (int)D_0017013B) == 0) {
        return 0;
    }
    return 1;
}

int profile_delete_section(int profile, char *section)
{
    char *start;
    char *end;
    int length;

    if ((short)profile_find_section(profile, section) == 0) return 0;
    start = *(char **)((char *)profile + 152);
    end = *(char **)((char *)profile + 132) + *(int *)((char *)profile + 136);
    length = 1;
    while (start[length] != 91 && start + length < end) {
        length++;
    }
    mc_memmove(start, start + length, end - (start + length), (int)D_00170129, 1179, 4);
    *(int *)((char *)profile + 136) -= length;
    *(signed char *)((char *)profile + 1) |= 128;
    return 1;
}

int profile_add_section(int profile, char *section)
{
    char *cursor;
    int length;

    if ((short)profile_find_section(profile, section) != 0) return 0;
    cursor = *(char **)((char *)profile + 132) + *(int *)((char *)profile + 136);
    length = strlen(section) + 6;
    if (((unsigned)(*(int *)((char *)profile + 136) + length)) > *(int *)((char *)profile + 140)) return 0;
    *cursor++ = 13;
    *cursor++ = 10;
    *(char **)((char *)profile + 152) = cursor;
    *cursor++ = 91;
    while (*section != 0) {
        *cursor++ = *section++;
    }
    *cursor++ = 93;
    *cursor++ = 13;
    *cursor++ = 10;
    *(char **)((char *)profile + 168) = cursor;
    *(char **)((char *)profile + 144) = cursor;
    *(int *)((char *)profile + 136) += length;
    *(signed char *)((char *)profile + 1) |= 128;
    return 1;
}

int profile_hex_to_int(char *text)
{
    int value;
    int digits;
    int unused;
    int i;

    value = 0;
    digits = strlen(text);
    i = 0;
    do {
        value += profile_hex_digit((signed char)text[i++]) * D_00178848[digits];
        digits--;
    } while (((unsigned)digits) > 0);
    return value;
}

int profile_hex_digit(signed char digit)
{
    int i;

    for (i = 0; ((unsigned)i) < 16; i++) {
        if (((int)(unsigned char)*(signed char *)((char *)(D_00178848[0] + i))) == toupper((int)(signed char)digit)) {
            return i;
        }
    }
    return -1;
}
