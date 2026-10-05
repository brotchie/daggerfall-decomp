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

extern int disk_read_file(char *, int);
extern int key_pressed_once(unsigned char);
extern int mc_malloc();
extern int mc_memcpy();
extern void logbook_prune_quests(void);
extern void save_thumbnail_capture(void);

int options_open(short force)
{
    int is_open;

    if (force != 0 || (game_mode == 0 && key_pressed_once(1) != 0)) {
        while (key_down_esc != 0);
        save_thumbnail_capture();
        D_00187CA8 = 0;
        game_mode = 7;
        options_image = disk_read_file(D_00170EDB, 0);
        D_00196272 = 1;
        options_saved_screen = mc_malloc(64000, (int)D_00170EE8, 120);
        mc_memcpy(options_saved_screen, screen_buffer, 64000, (int)D_00170EE8, 121, 4);
        logbook_prune_quests();
    }
    if (((int)(unsigned char)game_mode) == 7) {
        is_open = 1;
    } else {
        is_open = 0;
    }
    return is_open;
}
