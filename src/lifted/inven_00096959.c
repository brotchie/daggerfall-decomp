/* inven.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern int paperdoll_mask;


int inv_paperdoll_slot_at(int a1, int a2, int a3)
{
    a1 += -41;
    a2 += -5;
    return (int)(unsigned char)*(signed char *)((char *)(int)(((char *)paperdoll_mask) + ((a2 * 125) + a1)));
}
