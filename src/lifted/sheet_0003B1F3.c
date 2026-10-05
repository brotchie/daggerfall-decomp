/* sheet.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

#include "records.h"
#include "bitfield.h"

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
extern struct image *D_00195B5C;
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
extern struct image *D_00199638;

extern int key_action_held(int);
extern int sound_play(int, struct record *, int);
extern int disk_read_file(char *, int);
extern int rand_range(int, int);
extern int mc_memcpy();
extern void sheet_place_spinner(int);

int sheet_open(short opening)
{
    int hp_gain;
    int result;

    if (player_death_timer > 0) return 0;
    if (((int)D_0019626F) == 3 && ((int)(unsigned char)game_mode) == 8) {
        return 1;
    }
    if (opening != 0 || (game_mode == 0 && key_action_held(36) != 0 && player_death_timer == 0)) {
        game_mode = 3;
        window_image = disk_read_file(D_00170C40, 0);
        D_00199638 = (struct image *)disk_read_file(D_00170C4D, 0);
        D_00195B5C = (struct image *)disk_read_file(D_00170C5A, 0);
        D_00196272 = 1;
        D_00195F40 = 160 - (D_00199638->width >> 1);
        D_00195F3E = 100 - (D_00199638->height >> 1);
        D_00195F42 = D_00199638->width;
        D_00195F3C = D_00199638->height;
        if (((struct bf8_2_1 *)&D_001940D9)->f != 0) {
            hp_gain = (rand_range(player_class->hp_per_level >> 1, player_class->hp_per_level) + (player_character->attributes[4] / 10)) - 5;
            if (hp_gain < 1) hp_gain = 1;
            player_character->max_health += hp_gain;
            player_character->max_health_base += hp_gain;
            sound_play(364, player_object, 100);
        }
        if (((int)(short)opening) == 50) {
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
        result = 1;
    } else {
        result = 0;
    }
    return result;
}
