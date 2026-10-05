/* support.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern char D_0012B508[];
extern char msgbox_button_keys[];
extern char D_00196034[];
extern char msgbox_button_ids[];
extern char D_00196090[];
extern char D_00196091[];
extern char D_00196271[];

extern void msgbox_show_rsc(int, int);

void msgbox_yes_no_rsc(short a1)
{
    *(signed char *)D_00196271 = 0;
    *(signed char *)msgbox_button_ids = 4;
    *(signed char *)D_00196090 = 5;
    *(signed char *)D_00196091 = 0;
    *(signed char *)msgbox_button_keys = 21;
    *(signed char *)D_00196034 = 49;
    *(signed char *)D_0012B508 = 146;
    msgbox_show_rsc((int)(short)a1, 5);
}
