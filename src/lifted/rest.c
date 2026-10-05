/* rest.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

struct bf8_0_1 { unsigned char f:1; };
struct bf8_0_2 { unsigned char f:2; };
struct bf8_3_1 { unsigned char _:3; unsigned char f:1; };
struct bf8_4_1 { unsigned char _:4; unsigned char f:1; };
struct bf8_5_1 { unsigned char _:5; unsigned char f:1; };
extern signed char mouse_buttons;
extern signed char D_0012B508;
extern char D_001760D6[];
extern char D_00176141[];
extern char D_0017614E[];
extern unsigned char player_environment;
extern int D_00185087;
extern signed char D_00187CA8;
extern int D_0018DDE4;
extern signed char D_001940D9;
extern signed char player_motion_flags;
extern int D_00195998;
extern char frame_counter[];
extern struct building *current_building;
extern struct record *player_entity;
extern struct record *player_object;
extern struct record *D_00195AC4;
extern struct building *tavern_building;
extern int creature_count;
extern struct location *current_location;
extern struct character *player_character;
extern int game_minutes;
extern char D_00195CDC[];
extern int D_00195D48;
extern int player_death_timer;
extern unsigned char D_0019626F;
extern signed char D_00196272;
extern signed char game_mode;
extern signed char in_dungeon_water;
extern signed char crime_current;
extern signed char D_00196283;
extern signed char D_00196294;
extern signed char D_0019629B;
extern signed char D_001962A4;
extern int D_001A4A24;
extern int rest_image;

extern int tavern_room_rented(void);
extern int disk_resolve_path(int);
extern struct membership *guild_find_membership_by_kind(unsigned char);
extern int rand_range(int, int);
extern struct building *object_building(struct record *);
extern int mc_free();
extern int mc_memset();
extern int func_000C1500();
extern int func_0012B136();
extern int func_00144D00();
extern void tavern_go_to_room(void);
extern void damage_creature_death(struct record *);
extern void raise_skills(void);
extern void skill_add_uses(int, int);
extern void msgbox_show_string(int, int);
extern void msgbox_show_rsc(int, int);
extern void guards_summon(int);
extern void time_pass_minutes(int);
extern void palette_restore(void);
extern void rest_recover(int);
extern void weapon_load_hand_sprite(struct record *, int);
extern void weapon_free_sprites(void);
void fatigue_add(int);

void rest_close(void)
{
    int l_18;

    if (((int)(unsigned char)game_mode) != 16) return;
    if (rest_image != 0 && rest_image != (-1751672937)) {
        mc_free(rest_image, (int)D_001760D6, 271);
        rest_image = -1751672937;
    }
    D_00196272 = 0;
    game_mode = 0;
    D_0019626F = 16;
    D_00187CA8 = 1;
    D_00196294 = 0;
    D_0019629B = 0;
    D_00195D48 = 10000;
    if (player_character->special_infection_time != 0 && player_character->special_infection != 0 && ((int)(unsigned short)(player_character->flags & 16)) != 0) {
        player_character->flags &= ~0x10;
        while (mouse_buttons != 0) func_0012B136();
        l_18 = disk_resolve_path((int)D_00176141);
        mc_memset(655360, 0, 64000, (int)D_001760D6, 285, 4);
        func_000C1500(l_18, 0, 0, 1);
        mc_memset(655360, 0, 64000, (int)D_001760D6, 287, 4);
        palette_restore();
        D_0019629B = 0;
        D_00195D48 = 10000;
    } else if (player_character->special_infection_time != 0 && player_character->special_infection == 0 && ((int)(unsigned short)(player_character->flags & 16)) != 0) {
        player_character->flags &= ~0x10;
        while (mouse_buttons != 0) func_0012B136();
        l_18 = disk_resolve_path((int)D_0017614E);
        mc_memset(655360, 0, 64000, (int)D_001760D6, 297, 4);
        func_000C1500(l_18, 0, 0, 1);
        mc_memset(655360, 0, 64000, (int)D_001760D6, 299, 4);
        palette_restore();
        D_0019629B = 0;
        D_00195D48 = 10000;
    }
    if (((unsigned)(game_minutes - D_00195998)) <= 360) return;
    D_00195998 = game_minutes;
    raise_skills();
}

void fatigue_update(void)
{
    int l_18;

    if (((int)(unsigned char)game_mode) == 16 || ((int)(unsigned char)game_mode) == 19) {
        return;
    }
    if (D_0018DDE4 != 0 && ((struct bf8_0_2 *)&frame_counter)->f == 0) return;
    l_18 = -11;
    if (in_dungeon_water != 0 && player_character->race != 7) {
        if (rand_range(1, 100) > player_character->skills[17].value) l_18 = -44;
        skill_add_uses(17, 1);
    }
    if (((struct bf8_4_1 *)&D_001940D9)->f != 0) l_18 = -88;
    if (((struct bf8_5_1 *)&player_motion_flags)->f != 0) l_18 = -22;
    fatigue_add(l_18);
}

void fatigue_add(int a1)
{
    int l_1C;
    int l_18;

    if (player_death_timer != 0) return;
    l_18 = player_character->fatigue;
    l_18 += a1;
    l_1C = (player_character->attributes[0] + player_character->attributes[4]) << 6;
    if (l_18 > l_1C) l_18 = l_1C;
    player_character->fatigue = l_18;
    if (l_18 >= 1) return;
    D_0012B508 = 146;
    player_character->fatigue = 0;
    if (creature_count != 0) {
        msgbox_show_rsc(1072, 1);
        damage_creature_death(player_entity);
        return;
    }
    msgbox_show_rsc(1071, 1);
    D_001962A4 = 1;
    time_pass_minutes(60);
    rest_recover((int)player_entity);
    D_001962A4 = 0;
}

void breath_update(void)
{
    struct membership *l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    l_28 = player_character->attributes[4];
    if (D_00187CA8 == 0) return;
    if (l_28 > 100) l_28 = 100;
    if (((int)(unsigned char)D_00196283) != 107) {
        *(int *)D_00195CDC = 0;
        l_24 = 1132;
        D_001A4A24 = *(int *)((char *)l_24);
        return;
    }
    if ((player_character->conditions & 0x80000) != 0) return;
    if (*(int *)D_00195CDC == 0) {
        if ((*(int *)D_00195CDC = l_28 >> 1) > 50) *(int *)D_00195CDC = 50;
        l_2C = guild_find_membership_by_kind(149);
        if (l_2C != 0) *(int *)D_00195CDC += l_2C->rank * 3;
    }
    l_20 = 1132;
    if (((unsigned)(*(int *)((char *)l_20) - D_001A4A24)) > 18) {
        (*(int *)D_00195CDC)--;
        l_1C = 1132;
        if (player_character->race == 7 && ((struct bf8_0_1 *)((char *)l_1C))->f != 0) {
            (*(int *)D_00195CDC)++;
        }
        l_18 = 1132;
        D_001A4A24 = *(int *)((char *)l_18);
    }
    D_0012B508 = 145;
    if (((l_28 >> 3) + 4) > *(int *)D_00195CDC) D_0012B508 = 246;
    if (*(int *)D_00195CDC != 0) {
        func_00144D00(310, (int)(short)(120 - (*(short *)D_00195CDC * 2)), 6, (int)(short)(*(short *)D_00195CDC * 2));
    }
    if (*(int *)D_00195CDC != 0) return;
    damage_creature_death(player_entity);
}

int rest_allowed(void)
{
    if (player_character->ship_owned != 0 && ((unsigned)(((unsigned)(int)D_00195AC4->id) >> 16)) < 1000) {
        return 1;
    }
    switch (player_environment) {
    case 1:
        if (((int)(unsigned short)(short)D_00195AC4->image) == 65535) return 1;
        if (((int)(unsigned char)(signed char)current_location->kind) == 4 || ((int)(unsigned char)(signed char)current_location->kind) == 7 || ((int)(unsigned char)(signed char)current_location->kind) > 9) {
            return 1;
        }
        crime_current = 8;
        guards_summon(1);
        msgbox_show_rsc(17, 1);
        return 0;
    case 2:
        if (((int)(unsigned char)(signed char)current_building->type) == 15) {
            tavern_building = current_building;
            if (tavern_room_rented() == 0) {
                msgbox_show_string(D_00185087, 1);
                return 0;
            }
            tavern_go_to_room();
            return 1;
        }
        if (((int)(unsigned short)(short)current_building->faction_id) == 41 && guild_find_membership_by_kind(2) != 0) {
            tavern_go_to_room();
            return 1;
        }
        if ((int)current_building->id != player_character->house) {
            crime_current = 8;
            guards_summon(1);
            return 0;
        }
    }
    return 1;
}

int rest_room_expired(void)
{
    struct building *l_1C;

    l_1C = object_building(player_object);
    if (l_1C == 0) return 0;
    if (l_1C->type != 15) return 0;
    if ((l_1C->flags & 2) != 0 && game_minutes > l_1C->rent_expires) return 1;
    return 0;
}

void weapon_reload_hand_sprites(void)
{
    weapon_free_sprites();
    weapon_load_hand_sprite(player_character->equipped[21], 1);
    weapon_load_hand_sprite(player_character->equipped[19], 0);
}
