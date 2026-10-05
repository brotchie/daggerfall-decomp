/* sheet.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

struct bf8_2_1 { unsigned char _:2; unsigned char f:1; };
#include "records.h"

extern char D_00170C40[];
extern char D_00170C4D[];
extern char D_00170C5A[];
extern char D_00170C67[];
extern signed char D_00187CA8;
extern char scratch_190be4[];
extern char scratch_190d64[];
extern short D_00190D8C[];
extern signed char D_001940D9;
extern struct record *player_object;
extern char D_00195B5C[];
extern struct character *player_character;
extern int window_image;
extern struct career *player_class;
extern int player_death_timer;
extern short D_00195F3C;
extern short D_00195F3E;
extern short D_00195F40;
extern short D_00195F42;
extern signed char D_0019626C;
extern unsigned char D_0019626F;
extern signed char D_00196272;
extern signed char game_mode;
extern char D_00199638[];

extern int key_action_held(int);
extern int sound_play(int, struct record *, int);
extern int disk_read_file(int, int);
extern int rand_range(int, int);
extern int mc_memcpy();
extern void sheet_place_spinner(int);

int sheet_open(short a1)
{
    int l_20;
    int l_24;

    if (player_death_timer > 0) return 0;
    if (((int)D_0019626F) == 3 && ((int)(unsigned char)game_mode) == 8) {
        return 1;
    }
    if (a1 != 0 || (game_mode == 0 && key_action_held(36) != 0 && player_death_timer == 0)) {
        game_mode = 3;
        window_image = disk_read_file((int)D_00170C40, 0);
        *(int *)D_00199638 = disk_read_file((int)D_00170C4D, 0);
        *(int *)D_00195B5C = disk_read_file((int)D_00170C5A, 0);
        D_00196272 = 1;
        D_00195F40 = 160 - (((int)(unsigned short)*(short *)(*(char **)D_00199638 + 4)) >> 1);
        D_00195F3E = 100 - (((int)(unsigned short)*(short *)(*(char **)D_00199638 + 6)) >> 1);
        D_00195F42 = *(short *)(*(char **)D_00199638 + 4);
        D_00195F3C = *(short *)(*(char **)D_00199638 + 6);
        if (((struct bf8_2_1 *)&D_001940D9)->f != 0) {
            l_20 = (rand_range(player_class->hp_per_level >> 1, player_class->hp_per_level) + (player_character->attributes[4] / 10)) - 5;
            if (l_20 < 1) l_20 = 1;
            player_character->max_health += l_20;
            player_character->max_health_base += l_20;
            sound_play(364, player_object, 100);
        }
        if (((int)(short)a1) == 50) {
            D_001940D9 |= 4;
            *(short *)scratch_190d64 = 30;
        } else {
            *(short *)scratch_190d64 = rand_range(4, 6);
        }
        *(int *)scratch_190be4 = 0;
        mc_memcpy((int)D_00190D8C, (int)(signed char *)&player_character->base_attributes[0], 16, (int)D_00170C67, 97, 4);
        sheet_place_spinner(13);
        D_0019626C = 0;
        D_00187CA8 = 0;
    }
    if (((int)(unsigned char)game_mode) == 3) {
        l_24 = 1;
    } else {
        l_24 = 0;
    }
    return l_24;
}
