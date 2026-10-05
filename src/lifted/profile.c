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

int profile_get_raw_line(int a1, int a2, int a3)
{
    int l_18;
    int l_14;
    int l_10;

    l_18 = *(int *)((char *)a1 + 168);
    if (l_18 == 0 || ((int)(unsigned char)*(signed char *)((char *)l_18)) == 91 || ((int)(unsigned char)*(signed char *)((char *)l_18)) == 13) {
        return 0;
    }
    l_14 = (int)(*(char **)((char *)a1 + 132) + *(int *)((char *)a1 + 136));
    l_10 = 0;
    while (((int)(unsigned char)*(signed char *)((char *)l_18)) == 32) l_18++;
    while (((int)(unsigned char)*(signed char *)((char *)l_18)) != 13 && ((unsigned)(a3 - 1)) > l_10) {
        *(signed char *)((char *)(l_10++ + a2)) = *(signed char *)((char *)l_18++);
    }
    *(signed char *)((char *)(a2 + l_10)) = 0;
    l_18 += 2;
    if (((unsigned)l_18) >= l_14) {
        *(int *)((char *)a1 + 168) = 0;
    } else {
        *(int *)((char *)a1 + 168) = l_18;
    }
    return 1;
}

int profile_get_yes(int a1, int a2)
{
    char l_2C[32];

    if ((short)profile_find_item(a1, a2) == 0) return 0;
    if ((short)profile_get_string(a1, (int)l_2C, 32) == 0) return 0;
    if (stricmp((int)l_2C, (int)D_00170133) == 0) return 1;
    return 0;
}

int profile_get_item_string(int a1, int a2, int a3, int a4)
{
    if ((short)profile_find_item(a1, a2) == 0) return 0;
    if ((short)profile_get_string(a1, a3, a4) == 0) return 0;
    return 1;
}

int profile_set_yes_no(int a1, int a2, short a3)
{
    if ((short)profile_find_item(a1, a2) == 0) return 0;
    if (a3 != 0) {
        if ((short)profile_set_string(a1, (int)D_00170137) == 0) return 0;
    } else if ((short)profile_set_string(a1, (int)D_0017013B) == 0) {
        return 0;
    }
    return 1;
}

int profile_delete_section(int a1, int a2)
{
    int l_18;
    int l_14;
    int l_10;

    if ((short)profile_find_section(a1, a2) == 0) return 0;
    l_18 = *(int *)((char *)a1 + 152);
    l_14 = (int)(*(char **)((char *)a1 + 132) + *(int *)((char *)a1 + 136));
    l_10 = 1;
    while (((int)(unsigned char)*(signed char *)((char *)(l_18 + l_10))) != 91 && ((unsigned)(l_18 + l_10)) < l_14) {
        l_10++;
    }
    mc_memmove(l_18, l_18 + l_10, l_14 - (l_18 + l_10), (int)D_00170129, 1179, 4);
    *(int *)((char *)a1 + 136) -= l_10;
    *(signed char *)((char *)a1 + 1) |= 128;
    return 1;
}

int profile_add_section(int a1, int a2)
{
    int l_14;
    int l_10;

    if ((short)profile_find_section(a1, a2) != 0) return 0;
    l_14 = (int)(*(char **)((char *)a1 + 132) + *(int *)((char *)a1 + 136));
    l_10 = strlen(a2) + 6;
    if (((unsigned)(*(int *)((char *)a1 + 136) + l_10)) > *(int *)((char *)a1 + 140)) return 0;
    *(signed char *)((char *)l_14++) = 13;
    *(signed char *)((char *)l_14++) = 10;
    *(int *)((char *)a1 + 152) = l_14;
    *(signed char *)((char *)l_14++) = 91;
    while (*(signed char *)((char *)a2) != 0) {
        *(signed char *)((char *)l_14++) = *(signed char *)((char *)a2++);
    }
    *(signed char *)((char *)l_14++) = 93;
    *(signed char *)((char *)l_14++) = 13;
    *(signed char *)((char *)l_14++) = 10;
    *(int *)((char *)a1 + 168) = l_14;
    *(int *)((char *)a1 + 144) = l_14;
    *(int *)((char *)a1 + 136) += l_10;
    *(signed char *)((char *)a1 + 1) |= 128;
    return 1;
}

int profile_hex_to_int(int a1)
{
    int l_28;
    int l_24;
    int l_20;
    int l_1C;

    l_28 = 0;
    l_24 = strlen(a1);
    l_1C = 0;
    do {
        l_28 += profile_hex_digit((int)(signed char)*(signed char *)((char *)(l_1C++ + a1))) * D_00178848[l_24];
        l_24--;
    } while (((unsigned)l_24) > 0);
    return l_28;
}

int profile_hex_digit(signed char a1)
{
    int l_20;

    for (l_20 = 0; ((unsigned)l_20) < 16; l_20++) {
        if (((int)(unsigned char)*(signed char *)((char *)(D_00178848[0] + l_20))) == toupper((int)(signed char)a1)) {
            return l_20;
        }
    }
    return -1;
}
