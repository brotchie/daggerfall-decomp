/* pickbook.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern int screen_buffer;
extern char D_00170DE4[];
extern char D_00170DF7[];
extern char D_00170E04[];
extern int D_00184876;
extern signed char D_00187CA8;
extern signed char D_001940D4;
extern signed char D_001940D8;
extern struct record *player_object;
extern int spellshop_icons;
extern int window_image;
extern int D_00195D60;
extern int player_death_timer;
extern unsigned char D_0019626F;
extern signed char D_00196272;
extern signed char game_mode;
extern int spellbook_saved_screen;

extern int spellbook_build_list(void);
extern int key_action_held(int);
extern int sound_play(int, struct record *, int);
extern int disk_read_file(int, int);
extern int mc_malloc();
extern int mc_memcpy();
extern int func_0012DB50();
extern void hud_status_set(int);

int spellbook_open(short a1)
{
    int l_20;

    if (((int)D_0019626F) == 5 && ((int)(unsigned char)game_mode) == 8) {
        return 1;
    }
    if (player_death_timer > 0) return 0;
    if (a1 != 0 || (game_mode == 0 && key_action_held(28) != 0)) {
        if (D_00195D60 != 0) {
            hud_status_set(D_00184876);
            return 0;
        }
        func_0012DB50(4);
        D_001940D8 &= 254;
        if (spellbook_build_list() == 0) return 0;
        D_001940D4 |= 128;
        D_00187CA8 = 0;
        D_001940D8 |= 2;
        spellbook_saved_screen = mc_malloc(64000, (int)D_00170DE4, 101);
        mc_memcpy(spellbook_saved_screen, screen_buffer, 64000, (int)D_00170DE4, 102, 4);
        game_mode = 5;
        window_image = disk_read_file((int)D_00170DF7, 0);
        spellshop_icons = disk_read_file((int)D_00170E04, 0);
        D_00196272 = 1;
        sound_play(237, player_object, 100);
    }
    if (((int)(unsigned char)game_mode) == 5) {
        l_20 = 1;
    } else {
        l_20 = 0;
    }
    return l_20;
}
