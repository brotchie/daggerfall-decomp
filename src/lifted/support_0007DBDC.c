/* support.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern struct record *detect_target;
extern char D_00195B84[];


void detect_consider_creature(struct record *a1, int a2)
{
    *(short *)((char *)a1 + 23) = a2;
    if (a2 >= *(int *)D_00195B84) goto L7DC0D;
    if (a2 < 2048) goto L7DC0F;
L7DC0D:;
    return;
L7DC0F:;
    detect_target = a1;
    *(int *)D_00195B84 = a2;
}
