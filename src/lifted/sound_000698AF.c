/* sound.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern int D_00186DEC;
extern int D_0018DD54;
extern signed char sound_enabled;

extern int func_000A1D3C();

void sound_set_volume(int volume)
{
    if (sound_enabled == 0) return;
    if (D_0018DD54 == (-1)) return;
    func_000A1D3C(volume);
    D_00186DEC = volume;
}
