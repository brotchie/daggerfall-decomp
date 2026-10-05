/* sound.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern char D_00186DEC[];
extern char D_0018DD54[];
extern char sound_enabled[];

extern int func_000A1D3C();

void sound_set_volume(int a1)
{
    if (*(signed char *)sound_enabled == 0) return;
    if (*(int *)D_0018DD54 == (-1)) return;
    func_000A1D3C(a1);
    *(int *)D_00186DEC = a1;
}
