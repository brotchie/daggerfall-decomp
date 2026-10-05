/* sosez.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern char D_001700D5[];
extern char D_001700DD[];
extern char D_001700E5[];
extern char D_001700EE[];
extern char D_001700F9[];
extern char D_00170103[];
extern char D_0017010D[];
extern char D_0018DC38[];
extern char D_0018DC94[];
extern char D_0018DC98[];
extern char D_0018DC9C[];
extern int D_0018DD54;
extern int D_0018DD5C;
extern char D_0018DD64[];
extern char D_0018DD86[];

extern int profile_open(int, ...);
extern int profile_close(int, ...);
extern int profile_find_section(int, ...);
extern int profile_get_item_number(int, ...);
extern int mc_memset();
#pragma aux sosconv "*" parm caller [] value [eax] modify [eax ebx ecx edx];
#pragma aux (sosconv) sos_read_settings;

int sos_read_settings(char *path)
{
    char profile[176];

    mc_memset((int)D_0018DC38, 0, 268, (int)D_001700D5, 217, 4);
    mc_memset((int)D_0018DD64, 0, 46, (int)D_001700D5, 218, 4);
    if ((short)profile_open((int)profile, path) == 0) return 0;
    if ((short)profile_find_section((int)profile, (int)D_001700DD) == 0) {
        profile_close((int)profile);
        return 0;
    }
    *(int *)((char *)profile + 172) = profile_get_item_number((int)profile, (int)D_001700E5, (int)&D_0018DD5C);
    *(int *)((char *)profile + 172) = profile_get_item_number((int)profile, (int)D_001700EE, (int)D_0018DC94);
    *(int *)((char *)profile + 172) = profile_get_item_number((int)profile, (int)D_001700F9, (int)D_0018DC9C);
    *(int *)((char *)profile + 172) = profile_get_item_number((int)profile, (int)D_00170103, (int)D_0018DC98);
    if (*(short *)((char *)profile + 172) == 0) {
        profile_close((int)profile);
        return 0;
    }
    if ((short)profile_find_section((int)profile, (int)D_0017010D) == 0) {
        profile_close((int)profile);
        return 0;
    }
    *(int *)((char *)profile + 172) = profile_get_item_number((int)profile, (int)D_001700E5, (int)&D_0018DD54);
    *(int *)((char *)profile + 172) = profile_get_item_number((int)profile, (int)D_001700EE, (int)D_0018DD86);
    if (*(short *)((char *)profile + 172) == 0) {
        profile_close((int)profile);
        return 0;
    }
    profile_close((int)profile);
    return 1;
}
