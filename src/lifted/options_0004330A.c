/* options.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern signed char key_down_esc;
extern int screen_buffer;
extern char D_00170EDB[];
extern char D_00170EE8[];
extern signed char D_00187CA8;
extern signed char D_00196272;
extern signed char game_mode;
extern int options_image;
extern int options_saved_screen;

extern int disk_read_file(int, int);
extern int key_pressed_once(unsigned char);
extern int mc_malloc();
extern int mc_memcpy();
extern void logbook_prune_quests(void);
extern void func_0007EE38(void);

int options_open(short a1)
{
    int l_20;

    if (a1 != 0 || (game_mode == 0 && key_pressed_once(1) != 0)) {
        do {
        } while (key_down_esc != 0);
        func_0007EE38();
        D_00187CA8 = 0;
        game_mode = 7;
        options_image = disk_read_file((int)D_00170EDB, 0);
        D_00196272 = 1;
        options_saved_screen = mc_malloc(64000, (int)D_00170EE8, 120);
        mc_memcpy(options_saved_screen, screen_buffer, 64000, (int)D_00170EE8, 121, 4);
        logbook_prune_quests();
    }
    if (((int)(unsigned char)game_mode) == 7) {
        l_20 = 1;
    } else {
        l_20 = 0;
    }
    return l_20;
}
