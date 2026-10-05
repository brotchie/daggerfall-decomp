/* maploads.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern signed char mouse_buttons;
extern char D_0017055C[];
extern signed char tavern_state;
extern struct building *current_building;
extern char tavern_building[];
extern unsigned char D_0019626F;
extern signed char D_00196272;
extern signed char game_mode;
extern int tavern_menu_image;

extern int disk_read_file(int, int);
extern int func_0012B136();

int tavern_open(short a1)
{
    int l_20;

    if (((int)D_0019626F) == 20 && ((int)(unsigned char)game_mode) == 8) {
        return 1;
    }
    if (a1 != 0) {
        while (mouse_buttons != 0) func_0012B136();
        tavern_state = 0;
        tavern_menu_image = disk_read_file((int)D_0017055C, 0);
        game_mode = 20;
        D_00196272 = 1;
        *(int *)tavern_building = (int)current_building;
    }
    if (((int)(unsigned char)game_mode) == 20) {
        l_20 = 1;
    } else {
        l_20 = 0;
    }
    return l_20;
}
