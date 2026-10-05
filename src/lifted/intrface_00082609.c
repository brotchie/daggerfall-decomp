/* intrface.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */



int climb_angle_ok(int wall_cos)
{
    return (((wall_cos == 0) || ((wall_cos > 250) && (wall_cos < 265))) ? 1 : 0);
}
