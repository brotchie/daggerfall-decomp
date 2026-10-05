/* crime.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern int screen_buffer;
extern char D_001706E1[];
extern char D_001706E9[];
extern char D_001706F6[];
extern signed char text_buffer[];
extern int D_00195D48;
extern signed char D_00196294;
extern signed char D_0019629B;
extern signed char D_001962A4;
extern signed char D_001962A5;
extern signed char D_001962B0;

extern int disk_read_file(int, int);
extern int mc_free();
extern int mc_memset();
extern int func_000A0ED9(int, int);
extern int mc_sprintf(int, ...);
extern int mc_memcpy();
extern int func_000CD33A();
extern int func_000CDD81();
extern void screen_shake_stop(void);
extern void court_restore_vitals(void);
extern void time_pass(int);
extern void palette_restore(void);
extern void text_draw_centered_colored(int, int, int, int, unsigned char);
#pragma aux func_000A0ED9 parm routine [];

void prison_serve_sentence(int a1)
{
    int l_1C;
    int l_18;

    D_001962B0 = 1;
    D_001962A4 = 1;
    D_001962A5 = 0;
    D_00196294 = 1;
    l_18 = disk_read_file((int)D_001706E9, 0);
    mc_memset(655360, 0, 64000, (int)D_001706E1, 384, 4);
    for (l_1C = 0; l_1C < 768; l_1C++) {
        *(signed char *)((char *)(l_18 + l_1C) + 64000) <<= 2;
    }
    func_000CD33A(l_18 + 64000, 0, 256);
    for (l_1C = a1; l_1C != 0; l_1C--) {
        time_pass(1440);
        mc_memcpy(screen_buffer, l_18, 64000, (int)D_001706E1, 391, 4);
        func_000A0ED9(392, (int)D_001706E1);
        mc_sprintf((int)text_buffer, (int)D_001706F6, l_1C);
        text_draw_centered_colored((int)text_buffer, 156, 165, 190, 219);
        func_000CDD81(0);
    }
    mc_memset(655360, 0, 64000, (int)D_001706E1, 397, 4);
    mc_memset(screen_buffer, 0, 64000, (int)D_001706E1, 398, 4);
    palette_restore();
    if (l_18 != 0 && l_18 != (-1751672937)) {
        mc_free(l_18, (int)D_001706E1, 400);
        l_18 = -1751672937;
    }
    D_00196294 = 0;
    D_001962A5 = 1;
    D_001962B0 = 0;
    D_00195D48 = 10000;
    D_0019629B = 0;
    D_001962A4 = 0;
    screen_shake_stop();
    court_restore_vitals();
}
