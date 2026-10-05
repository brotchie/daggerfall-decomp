/* options.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern char key_down_esc[];
extern char screen_buffer[];
extern char D_00170EDB[];
extern char D_00170EE8[];
extern char D_00187CA8[];
extern char D_00196272[];
extern char game_mode[];
extern char options_image[];
extern char options_saved_screen[];

extern int disk_read_file(int, int);
extern int key_pressed_once(unsigned char);
extern int mc_malloc();
extern int mc_memcpy();
extern void logbook_prune_quests(void);
extern void func_0007EE38(void);

int options_open(short a1)
{
    int l_20;

    if (a1 != 0) goto L4333B;
    if (*(signed char *)game_mode != 0) goto L43339;
    if (key_pressed_once(1) != 0) goto L4333B;
L43339:;
    goto L433AB;
L4333B:;
    if (*(signed char *)key_down_esc != 0) goto L4333B;
    func_0007EE38();
    *(signed char *)D_00187CA8 = 0;
    *(signed char *)game_mode = 7;
    *(int *)options_image = disk_read_file((int)D_00170EDB, 0);
    *(signed char *)D_00196272 = 1;
    *(int *)options_saved_screen = mc_malloc(64000, (int)D_00170EE8, 120);
    mc_memcpy(*(int *)options_saved_screen, *(int *)screen_buffer, 64000, (int)D_00170EE8, 121, 4);
    logbook_prune_quests();
L433AB:;
    if (((int)(unsigned char)*(signed char *)game_mode) != 7) goto L433C0;
    l_20 = 1;
    goto L433C7;
L433C0:;
    l_20 = 0;
L433C7:;
    return l_20;
}
