/* support.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern signed char D_0012B508;
extern signed char msgbox_button_keys;
extern signed char D_00196034;
extern signed char msgbox_button_ids;
extern signed char D_00196090;
extern signed char D_00196091;
extern unsigned char D_00196271;

extern void msgbox_show_rsc(int, int);

void msgbox_yes_no_rsc(short a1)
{
    D_00196271 = 0;
    msgbox_button_ids = 4;
    D_00196090 = 5;
    D_00196091 = 0;
    msgbox_button_keys = 21;
    D_00196034 = 49;
    D_0012B508 = 146;
    msgbox_show_rsc((int)(short)a1, 5);
}
