/* maploads.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern char mouse_buttons[];
extern char D_0017055C[];
extern char tavern_state[];
extern char current_building[];
extern char tavern_building[];
extern char D_0019626F[];
extern char D_00196272[];
extern char game_mode[];
extern char tavern_menu_image[];

extern int disk_read_file(int, int);
extern int func_0012B136();

int tavern_open(short a1)
{
    int l_20;

    if (((int)(unsigned char)*(signed char *)D_0019626F) != 20) goto L1EEAD;
    if (((int)(unsigned char)*(signed char *)game_mode) == 8) goto L1EEAF;
L1EEAD:;
    goto L1EEB8;
L1EEAF:;
    return 1;
L1EEB8:;
    if (a1 == 0) goto L1EEFF;
L1EEBF:;
    if (*(signed char *)mouse_buttons == 0) goto L1EECF;
    func_0012B136();
    goto L1EEBF;
L1EECF:;
    *(signed char *)tavern_state = 0;
    *(int *)tavern_menu_image = disk_read_file((int)D_0017055C, 0);
    *(signed char *)game_mode = 20;
    *(signed char *)D_00196272 = 1;
    *(int *)tavern_building = *(int *)current_building;
L1EEFF:;
    if (((int)(unsigned char)*(signed char *)game_mode) != 20) goto L1EF14;
    l_20 = 1;
    goto L1EF1B;
L1EF14:;
    l_20 = 0;
L1EF1B:;
    return l_20;
}
