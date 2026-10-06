/* matched by the real Watcom C32 10.0a (-d2): a run of click.c from 0x00077960 to 0x00077B9E, kept together for its switch table's alignment */
#include "records.h"
#include "clib.h"
extern char D_001766F9[];
extern char D_001767E4[];
extern char D_001767EE[];
extern char D_001767FA[];
extern char D_00176807[];
extern char D_00176813[];
extern char D_0017681A[];
extern char D_00176823[];
extern char D_0017682A[];
extern char D_00176833[];
extern char D_0017683A[];
extern char D_001789E8[];
extern char D_001789F0[];
extern unsigned char player_environment;
extern char D_00187CA8;
extern struct building *current_building;
extern struct record *player_object;
extern struct record *location_object;
extern struct location *current_location;
extern unsigned int game_minutes;
extern int trespassing;
extern unsigned char climate_weathers[];
extern unsigned short D_00195F5E;
extern char in_knightly_order_hall;
extern unsigned char current_region;
extern char is_daytime;
extern char D_001962A1;
extern char music_uses_fm;
extern char **D_001A4FA0;
extern char **D_001A4FA4;
extern char **D_001A4FA8;
extern char **D_001A4FAC;
extern char **D_001A4FB0;
extern char **D_001A4FB4;
extern int climate_category(void);
extern void music_play(char *);
extern char *music_dungeon_song(void);
extern int location_contains(int, int);
extern int D_0018767C[];
extern char *D_001876AC[];
extern char *D_001876E8[];
extern char *D_00187734[];
extern char *D_00187750[];
extern char *D_00187770[];
extern char *D_00187784[];
extern char *D_001877A0[];
extern char *D_001877DC[];
extern char *D_00187828[];
extern char *D_00187844[];
extern char *D_00187864[];
extern char *D_00187878[];
extern int player_motion_flags;
extern int frame_ticks;
extern struct character *player_character;
extern struct settings *game_settings;
extern int head_bob_offset;
extern char in_dungeon_water;
extern char D_0019628E;
extern int D_001A4FD0;
extern int D_001A4FD8;
extern int spawn_point_wilderness(struct record *);
extern int spawn_point_town(struct record *, int, int);
extern int spawn_point_building(struct record *);
extern int spawn_point_dungeon(struct record *, int, int);

int spawn_find_point(struct record *object, int min_distance, int max_distance)
{
    int r;

    switch (player_environment) {
    case 1:
        if (location_object->image == 0xffff)
            r = spawn_point_wilderness(object);
        else
            r = spawn_point_town(object, min_distance, max_distance);
        break;
    case 2:
        r = spawn_point_building(object);
        break;
    case 3:
        r = spawn_point_dungeon(object, min_distance, max_distance);
        break;
    }
    if (r == 0)
        object->x = object->y = object->z = 0;
    return r;
}

void head_bob_update(void)
{
    if (!(game_settings->view_flags & 2) || (player_motion_flags & 0x20) || in_dungeon_water || (player_character->conditions & 8)) {
        head_bob_offset = 0;
        return;
    }
    if (D_0019628E)
        D_001A4FD0 += frame_ticks;
    D_001A4FD0 = D_001A4FD0 % 1000;
    if (D_001A4FD8 >= D_001A4FD0 && !D_0019628E)
        D_001A4FD0 = 0;
    D_001A4FD8 = D_001A4FD0;
    head_bob_offset = D_0018767C[D_001A4FD0 / 100];
}

void music_select_tables(void)
{
    if (music_uses_fm) {
        D_001A4FB0 = D_001877A0;
        D_001A4FA8 = D_001877DC;
        D_001A4FAC = D_00187828;
        D_001A4FA4 = D_00187844;
        D_001A4FB4 = D_00187864;
        D_001A4FA0 = D_00187878;
        return;
    }
    D_001A4FB0 = D_001876AC;
    D_001A4FA8 = D_001876E8;
    D_001A4FAC = D_00187734;
    D_001A4FA4 = D_00187750;
    D_001A4FB4 = D_00187770;
    D_001A4FA0 = D_00187784;
}

void music_choose_song(void)
{
    int seed;
    int climate;
    int idx;
    char *p;

    if (D_00187CA8 == 0)
        return;
    seed = rand();
    if (player_environment == 3) {
        if (D_001962A1) {
            music_play(D_001A4FA8[10]);
        } else {
            p = music_dungeon_song();
            if (p) {
                music_play(p);
            } else {
                srand((current_region << 8) ^ location_object->image);
                music_play(D_001A4FB0[rand() % 15]);
            }
        }
    } else if (player_environment == 1) {
        climate = climate_category();
        srand(game_minutes / 1440);
        if (is_daytime == 0) {
            music_play(D_001A4FA0[rand() % 7]);
        } else if (!location_contains(player_object->x, player_object->z) || (current_location->kind != 4 && current_location->kind <= 9 ? 1 : 0)) {
            switch (climate_weathers[climate]) {
            case 0:
                music_play(D_001A4FA8[rand() % 7]);
                break;
            case 1:
                music_play(D_001A4FA8[rand() % 9]);
                break;
            case 2:
            case 3:
            case 6:
                music_play(D_001A4FA8[rand() % 5 + 7]);
                break;
            case 4:
                music_play(D_001A4FA8[rand() % 3 + 12]);
                break;
            case 5:
                music_play(D_001A4FA8[rand() % 3 + 15]);
                break;
            }
        } else if (location_contains(player_object->x, player_object->z) && (current_location->kind == 4 || current_location->kind >= 9)) {
            music_play(D_001A4FA0[rand() % 7]);
        } else {
            switch (climate_weathers[climate]) {
            case 0:
                music_play(D_001A4FA8[rand() % 7]);
                break;
            case 1:
                music_play(D_001A4FA8[rand() % 9]);
                break;
            case 2:
            case 3:
            case 6:
                music_play(D_001A4FA8[rand() % 5 + 7]);
                break;
            case 4:
                music_play(D_001A4FA8[rand() % 3 + 12]);
                break;
            case 5:
                music_play(D_001A4FA8[rand() % 3 + 15]);
                break;
            }
        }
    } else {
        srand(D_00195F5E);
        if (trespassing) {
            music_play(D_001A4FAC[rand() % 7]);
            srand(seed);
            return;
        }
        switch (current_building->type) {
        case 0:
        case 2:
        case 3:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 12:
        case 13:
            music_play(music_uses_fm == 0 ? D_001767E4 : D_001766F9);
            break;
        case 11:
            if (current_building->faction_id == 40) {
                if (rand() & 1)
                    music_play(music_uses_fm == 0 ? D_001767EE : D_001767FA);
                else
                    music_play(music_uses_fm == 0 ? D_00176807 : D_001767FA);
            } else {
                music_play(music_uses_fm == 0 ? D_00176813 : D_0017681A);
            }
            break;
        case 14:
            if (in_knightly_order_hall) {
                music_play(music_uses_fm == 0 ? D_00176823 : D_0017682A);
            } else {
                p = memchr(D_001789E8, current_building->faction_id, 8);
                idx = (int)(p - D_001789E8);
                if (p == 0) {
                    p = memchr(D_001789F0, current_building->faction_id, 8);
                    if (p == 0) {
                        music_play(music_uses_fm == 0 ? D_00176823 : D_0017682A);
                        break;
                    }
                    idx = (int)(p - D_001789F0);
                }
                music_play(D_001A4FA4[idx]);
            }
            break;
        case 15:
            music_play(D_001A4FB4[game_minutes / 1440 % 5]);
            break;
        case 16:
            /* a random pick from one choice: the code generator folds `% 1` to 0 and drops the
             * then-branch, but its ?: temp keeps the frame slot at [ebp-0x50] */
            if (rand() % 1)
                music_play(music_uses_fm == 0 ? D_00176833 : D_0017683A);
            else
                music_play(music_uses_fm == 0 ? D_00176833 : D_0017683A);
            break;
        default:
            music_play(music_uses_fm == 0 ? D_00176813 : D_0017681A);
            break;
        }
    }
    srand(seed);
}
