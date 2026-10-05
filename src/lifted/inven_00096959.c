/* inven.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern int paperdoll_mask;


int inv_paperdoll_slot_at(int x, int y, int unused)
{
    x += -41;
    y += -5;
    return (int)(unsigned char)*(signed char *)((char *)(int)(((char *)paperdoll_mask) + ((y * 125) + x)));
}
