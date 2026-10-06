/* profile.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "structs.h"
#include "clib.h"

extern char D_00170129[];
extern char D_00170133[];
extern char D_00170137[];
extern char D_0017013B[];
extern iptr D_00178848[];

extern int profile_find_section(struct profile *, char *);
#pragma aux sosconv "*" parm caller [] value [eax] modify [eax ebx ecx edx];
#pragma aux (sosconv) profile_find_section;
extern int profile_find_item(struct profile *, char *);
#pragma aux (sosconv) profile_find_item;
extern int profile_get_string(struct profile *, char *, int);
#pragma aux (sosconv) profile_get_string;
extern int profile_set_string(struct profile *, char *);
#pragma aux (sosconv) profile_set_string;
int profile_hex_digit(signed char);
#pragma aux (sosconv) profile_get_raw_line;
#pragma aux (sosconv) profile_get_yes;
#pragma aux (sosconv) profile_get_item_string;
#pragma aux (sosconv) profile_set_yes_no;
#pragma aux (sosconv) profile_delete_section;
#pragma aux (sosconv) profile_add_section;

int profile_get_raw_line(struct profile *profile, char *line, int size)
{
    char *cursor;
    char *end;
    int length;

    cursor = profile->line;
    if (cursor == 0 || *cursor == 91 || *cursor == 13) {
        return 0;
    }
    end = profile->buffer + profile->length;
    length = 0;
    while (*cursor == 32) cursor++;
    while (*cursor != 13 && ((unsigned)(size - 1)) > length) {
        line[length++] = *cursor++;
    }
    line[length] = 0;
    cursor += 2;
    if (cursor >= end) {
        profile->line = 0;
    } else {
        profile->line = cursor;
    }
    return 1;
}

int profile_get_yes(struct profile *profile, char *item)
{
    char value[32];

    if ((short)profile_find_item(profile, item) == 0) return 0;
    if ((short)profile_get_string(profile, value, 32) == 0) return 0;
    if (stricmp(value, D_00170133) == 0) return 1;
    return 0;
}

int profile_get_item_string(struct profile *profile, char *item, char *value, int size)
{
    if ((short)profile_find_item(profile, item) == 0) return 0;
    if ((short)profile_get_string(profile, value, size) == 0) return 0;
    return 1;
}

int profile_set_yes_no(struct profile *profile, char *item, short yes)
{
    if ((short)profile_find_item(profile, item) == 0) return 0;
    if (yes != 0) {
        if ((short)profile_set_string(profile, D_00170137) == 0) return 0;
    } else if ((short)profile_set_string(profile, D_0017013B) == 0) {
        return 0;
    }
    return 1;
}

int profile_delete_section(struct profile *profile, char *section)
{
    char *start;
    char *end;
    int length;

    if ((short)profile_find_section(profile, section) == 0) return 0;
    start = profile->section_header;
    end = profile->buffer + profile->length;
    length = 1;
    while (start[length] != 91 && start + length < end) {
        length++;
    }
    mc_memmove(start, start + length, (int)(end - (start + length)), D_00170129, 1179, 4);
    profile->length -= length;
    profile->flags |= 128;
    return 1;
}

int profile_add_section(struct profile *profile, char *section)
{
    char *cursor;
    int length;

    if ((short)profile_find_section(profile, section) != 0) return 0;
    cursor = profile->buffer + profile->length;
    length = strlen(section) + 6;
    if (profile->length + length > profile->capacity) return 0;
    *cursor++ = 13;
    *cursor++ = 10;
    profile->section_header = cursor;
    *cursor++ = 91;
    while (*section != 0) {
        *cursor++ = *section++;
    }
    *cursor++ = 93;
    *cursor++ = 13;
    *cursor++ = 10;
    profile->line = cursor;
    profile->section = cursor;
    profile->length += length;
    profile->flags |= 128;
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
        value += (int)(profile_hex_digit((signed char)text[i++]) * D_00178848[digits]);
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
