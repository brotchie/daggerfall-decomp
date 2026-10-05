/* support.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern struct record *detect_target;
extern char D_00195B84[];


void detect_consider_creature(struct record *creature, int distance)
{
    creature->detect_distance = distance;
    if (distance >= *(int *)D_00195B84 || distance >= 2048) return;
    detect_target = creature;
    *(int *)D_00195B84 = distance;
}
