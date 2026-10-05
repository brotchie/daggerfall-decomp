/* support.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern char D_00195AC4[];
extern char current_location[];

extern int object_find_by_id(int, int);
extern void town_map_note_building(int, int);

void func_0007F0F3(int a1)
{
    int l_20;
    int l_1C;
    int l_18;

    if (a1 == 0) return;
    l_1C = *(int *)(*(char **)current_location + 43);
    l_20 = 0;
L7F120:;
    if (((int)(unsigned short)*(short *)(*(char **)current_location + 41)) > l_20) goto L7F144;
    return;
L7F135:;
    l_20++;
    (*(char (**)[26])&l_1C)++;
    goto L7F120;
L7F144:;
    if (((int)(unsigned short)*(short *)((char *)l_1C + 18)) != a1) goto L7F179;
    l_18 = object_find_by_id(*(int *)D_00195AC4, *(int *)((char *)l_1C + 20));
    if (l_18 == 0) goto L7F179;
    town_map_note_building(l_18, l_1C);
L7F179:;
    goto L7F135;
}
