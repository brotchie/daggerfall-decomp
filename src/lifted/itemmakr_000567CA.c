/* itemmakr.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern char inventory_containers[];
extern char inv_left_scroll[];
extern char inv_left_container[];
extern signed char inv_tab;


void itemmaker_select_tab(int tab)
{
    tab += -15;
    if (*(int *)(inventory_containers + (tab << 2)) == 0) return;
    inv_tab = *(signed char *)&tab;
    *(int *)inv_left_container = *(int *)(inventory_containers + (((int)(unsigned char)inv_tab) << 2));
    *(int *)inv_left_scroll = 0;
}
