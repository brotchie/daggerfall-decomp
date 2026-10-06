/* camera.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "ptrint.h"
#include "clib.h"

extern signed char key_down_backslash;
extern char D_00170194[];
extern char D_0017019D[];
extern char D_00178A1C[];
extern signed char D_00178A52[];
extern signed char D_00178A53[];
extern signed char D_00178A54[];
extern short screenshot_number;
extern char D_0019645A[];

extern void xn_pal_read_dac(char *);
extern int xn_kbd_flush(void);
void screenshot_save_bmp(char *);
#pragma aux mc_set_location parm routine [];

void screenshot_poll(void)
{
    if (key_down_backslash == 0) return;
    mc_set_location(44, D_00170194);
    mc_sprintf(D_0019645A, D_0017019D, (int)(short)screenshot_number);
    screenshot_save_bmp(D_0019645A);
    while (key_down_backslash != 0) xn_kbd_flush();
}

void screenshot_save_bmp(char *filename)
{
    iptr buffer;
    short i;
    short fd;

    buffer = (iptr)mc_malloc(768, D_00170194, 67);
    xn_pal_read_dac((char *)buffer);
    *(int *)&i = 0;
    for (; ((int)(short)i) < 256; (*(int *)&i)++) {
        D_00178A52[((int)(short)i) << 2] = *(signed char *)((char *)((((int)(short)i) * 3) + buffer) + 2) << 2;
        D_00178A53[((int)(short)i) << 2] = *(signed char *)((char *)((((int)(short)i) * 3) + buffer) + 1) << 2;
        D_00178A54[((int)(short)i) << 2] = *(signed char *)((char *)((((int)(short)i) * 3) + buffer)) << 2;
    }
    screenshot_number++;
    unlink(filename);
    *(int *)&fd = open(filename, 546, 384);
    if (((int)(short)fd) != (-1) && buffer != 0) {
        write((int)(short)fd, D_00178A1C, 1078);
        if (buffer != 0 && buffer != (-1751672937)) {
            mc_free((void *)buffer, D_00170194, 82);
            buffer = -1751672937;
        }
        buffer = 655360;
        buffer += 63680;
        *(int *)&i = 0;
        for (; ((int)(short)i) < 200; (*(int *)&i)++) {
            write((int)(short)fd, (void *)buffer, 320);
            buffer += -320;
        }
        close((int)(short)fd);
        return;
    }
    if (buffer == 0 || buffer == (-1751672937)) return;
    mc_free((void *)buffer, D_00170194, 93);
    buffer = -1751672937;
}
