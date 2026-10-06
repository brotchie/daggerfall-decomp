/* crime.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "ptrint.h"
#include "clib.h"
#include "doslow.h"

extern iptr screen_buffer;
extern char D_001706E1[];
extern char D_001706E9[];
extern char D_001706F6[];
extern signed char text_buffer[];
extern int sky_loaded_frame;
extern signed char D_00196294;
extern signed char night_sky_loaded;
extern signed char D_001962A4;
extern signed char D_001962A5;
extern signed char D_001962B0;

extern iptr disk_read_file(char *, iptr);
extern void xn_pal_set_range_8bit(char *, int, int);
extern void xn_gfx_present_inclusive(int);
extern void screen_shake_stop(void);
extern void court_restore_vitals(void);
extern void time_pass(int);
extern void palette_restore(void);
extern void text_draw_centred_coloured(iptr, int, int, int, unsigned char);
#pragma aux mc_set_location parm routine [];

void prison_serve_sentence(int days)
{
    int day;
    iptr image;

    D_001962B0 = 1;
    D_001962A4 = 1;
    D_001962A5 = 0;
    D_00196294 = 1;
    image = disk_read_file(D_001706E9, 0);
    mc_memset((void *)DOS_LOW(0xA0000), 0, 64000, D_001706E1, 384, 4);
    for (day = 0; day < 768; day++) {
        *(signed char *)((char *)(image + day) + 64000) <<= 2;
    }
    xn_pal_set_range_8bit((char *)(image + 64000), 0, 256);
    for (day = days; day != 0; day--) {
        time_pass(1440);
        mc_memcpy((void *)screen_buffer, (void *)image, 64000, D_001706E1, 391, 4);
        mc_set_location(392, D_001706E1);
        mc_sprintf((char *)text_buffer, D_001706F6, day);
        text_draw_centred_coloured((iptr)text_buffer, 156, 165, 190, 219);
        xn_gfx_present_inclusive(0);
    }
    mc_memset((void *)DOS_LOW(0xA0000), 0, 64000, D_001706E1, 397, 4);
    mc_memset((void *)screen_buffer, 0, 64000, D_001706E1, 398, 4);
    palette_restore();
    if (image != 0 && image != (-1751672937)) {
        mc_free((void *)image, D_001706E1, 400);
        image = -1751672937;
    }
    D_00196294 = 0;
    D_001962A5 = 1;
    D_001962B0 = 0;
    sky_loaded_frame = 10000;
    night_sky_loaded = 0;
    D_001962A4 = 0;
    screen_shake_stop();
    court_restore_vitals();
}
