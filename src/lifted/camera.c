/* camera.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern signed char key_down_backslash;
extern char D_00170194[];
extern char D_0017019D[];
extern char D_00178A1C[];
extern signed char D_00178A52[];
extern signed char D_00178A53[];
extern signed char D_00178A54[];
extern short screenshot_number;
extern char D_0019645A[];

extern int open(int, ...);
extern int func_0009DEA7();
extern int mc_free();
extern int mc_malloc();
extern int write();
extern int func_000A0ED9(int, int);
extern int mc_sprintf(int, ...);
extern int unlink();
extern int func_000CE758();
extern int func_00142790();
void screenshot_save_bmp(int);
#pragma aux func_000A0ED9 parm routine [];

void screenshot_poll(void)
{
    if (key_down_backslash == 0) return;
    func_000A0ED9(44, (int)D_00170194);
    mc_sprintf((int)D_0019645A, (int)D_0017019D, (int)(short)screenshot_number);
    screenshot_save_bmp((int)D_0019645A);
    while (key_down_backslash != 0) func_00142790();
}

void screenshot_save_bmp(int a1)
{
    int l_20;
    short l_1C;
    short l_18;

    l_20 = mc_malloc(768, (int)D_00170194, 67);
    func_000CE758(l_20);
    *(int *)&l_1C = 0;
    for (; ((int)(short)l_1C) < 256; (*(int *)&l_1C)++) {
        D_00178A52[((int)(short)l_1C) << 2] = *(signed char *)((char *)((((int)(short)l_1C) * 3) + l_20) + 2) << 2;
        D_00178A53[((int)(short)l_1C) << 2] = *(signed char *)((char *)((((int)(short)l_1C) * 3) + l_20) + 1) << 2;
        D_00178A54[((int)(short)l_1C) << 2] = *(signed char *)((char *)((((int)(short)l_1C) * 3) + l_20)) << 2;
    }
    (screenshot_number)++;
    unlink(a1);
    *(int *)&l_18 = open(a1, 546, 384);
    if (((int)(short)l_18) != (-1) && l_20 != 0) {
        write((int)(short)l_18, (int)D_00178A1C, 1078);
        if (l_20 != 0 && l_20 != (-1751672937)) {
            mc_free(l_20, (int)D_00170194, 82);
            l_20 = -1751672937;
        }
        l_20 = 655360;
        l_20 += 63680;
        *(int *)&l_1C = 0;
        for (; ((int)(short)l_1C) < 200; (*(int *)&l_1C)++) {
            write((int)(short)l_18, l_20, 320);
            l_20 += -320;
        }
        func_0009DEA7((int)(short)l_18);
        return;
    }
    if (l_20 == 0 || l_20 == (-1751672937)) return;
    mc_free(l_20, (int)D_00170194, 93);
    l_20 = -1751672937;
}
