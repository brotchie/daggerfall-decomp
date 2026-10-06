/* intro.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "ptrint.h"
#include "clib.h"

extern signed char mouse_buttons;
extern iptr screen_buffer;
extern char D_00170B7B[];
extern char D_00170B88[];
extern char D_00170C1B[];
extern char D_00170C28[];
extern char D_00170C35[];

extern iptr disk_resolve_path(iptr);
extern int xn_vid_play(char *, int, int, int);
extern int xn_mouse_poll_clamped(void);
extern void starting_equipment_give(void);
extern void palette_restore(void);

void intro_play_logo(void)
{
    int unused1;
    int unused2;
    int unused3;
    int unused4;
    int unused5;
    int unused6;
    int unused7;
    int unused8;
    int unused9;
    int unused10;
    int unused11;
    int unused12;
    iptr path;

    while (mouse_buttons != 0) xn_mouse_poll_clamped();
    path = disk_resolve_path((iptr)D_00170B7B);
    xn_vid_play((char *)path, 0, 0, 1);
    mc_memset((void *)655360, 0, 64000, D_00170B88, 34, 4);
    mc_memset((void *)screen_buffer, 0, 64000, D_00170B88, 35, 4);
    while (mouse_buttons != 0) xn_mouse_poll_clamped();
    mc_memset((void *)655360, 0, 64000, D_00170B88, 39, 4);
    palette_restore();
}

void intro_play_movie(void)
{
    iptr path;

    mc_memset((void *)655360, 0, 64000, D_00170B88, 358, 4);
    while (mouse_buttons != 0) xn_mouse_poll_clamped();
    path = disk_resolve_path((iptr)D_00170C1B);
    xn_vid_play((char *)path, 0, 0, 1);
    mc_memset((void *)655360, 0, 64000, D_00170B88, 363, 4);
    mc_memset((void *)screen_buffer, 0, 64000, D_00170B88, 364, 4);
    while (mouse_buttons != 0) xn_mouse_poll_clamped();
    path = disk_resolve_path((iptr)D_00170C28);
    xn_vid_play((char *)path, 0, 0, 1);
    mc_memset((void *)655360, 0, 64000, D_00170B88, 369, 4);
    mc_memset((void *)screen_buffer, 0, 64000, D_00170B88, 370, 4);
    while (mouse_buttons != 0) xn_mouse_poll_clamped();
    path = disk_resolve_path((iptr)D_00170C35);
    xn_vid_play((char *)path, 32, 0, 1);
    mc_memset((void *)655360, 0, 64000, D_00170B88, 375, 4);
    mc_memset((void *)screen_buffer, 0, 64000, D_00170B88, 376, 4);
    palette_restore();
}

void chargen_give_starting_equipment(void)
{
    starting_equipment_give();
}
