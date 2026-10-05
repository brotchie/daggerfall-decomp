/* pickbook.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern signed char D_0012B508;
extern signed char game_mode;

extern int key_action_held(int);
extern void show_health_status(void);
extern void msgbox_show_rsc(int, int);

void status_show(short force)
{
    if (force == 0) {
        if (game_mode != 0 || key_action_held(35) == 0) return;
    }
    D_0012B508 = 146;
    msgbox_show_rsc(22, 1);
    show_health_status();
}
