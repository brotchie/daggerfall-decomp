/* click.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern char D_00176198[];
extern char D_0017627D[];
extern signed char text_buffer[];

extern int disk_open_data(int);
extern int func_0009DEA7();
extern int func_000A00CB();
extern int func_000A0ED9(int, int);
extern int mc_sprintf(int, ...);
#pragma aux func_000A0ED9 parm routine [];

void book_read_header(int a1, int a2)
{
    int l_14;

    func_000A0ED9(586, (int)D_00176198);
    mc_sprintf((int)text_buffer, (int)D_0017627D, a2);
    l_14 = disk_open_data((int)text_buffer);
    func_000A00CB(l_14, a1, 234);
    func_0009DEA7(l_14);
}
