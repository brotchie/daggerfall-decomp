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
extern int last_skill_check_minutes;
extern char frame_counter[];
extern struct building *current_building;
extern struct record *player_entity;
extern struct record *player_object;
extern struct record *location_object;
extern struct building *tavern_building;
extern int creature_count;
extern struct location *current_location;
extern struct character *player_character;
extern int game_minutes;
extern char breath_remaining[];
extern int sky_loaded_frame;
extern int player_death_timer;
extern unsigned char D_0019626F;
extern signed char D_00196272;
extern signed char game_mode;
extern signed char in_dungeon_water;
extern signed char crime_current;
extern signed char fog_colour;
extern signed char D_00196294;
extern signed char night_sky_loaded;
extern signed char D_001962A4;
extern int breath_last_tick;
extern char rest_image[];

extern int tavern_room_rented(void);
extern int disk_resolve_path(int);
extern struct membership *guild_find_membership_by_kind(unsigned char);
extern int rand_range(int, int);
extern struct building *object_building(struct record *);
extern int mc_free();
extern int mc_memset();
extern int xn_vid_play();
extern int xn_mouse_poll_clamped();
extern int xn_draw_fill_rect();
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
    int path;

    if (((int)(unsigned char)game_mode) != 16) return;
    if (*(int *)rest_image != 0 && *(int *)rest_image != (-1751672937)) {
        mc_free(*(int *)rest_image, (int)D_001760D6, 271);
        *(int *)rest_image = -1751672937;
    }
    D_00196272 = 0;
    game_mode = 0;
    D_0019626F = 16;
    D_00187CA8 = 1;
    D_00196294 = 0;
    night_sky_loaded = 0;
    sky_loaded_frame = 10000;
    if (player_character->special_infection_time != 0 && player_character->special_infection != 0 && ((int)(unsigned short)(player_character->flags & 16)) != 0) {
        player_character->flags &= ~0x10;
        while (mouse_buttons != 0) xn_mouse_poll_clamped();
        path = disk_resolve_path((int)D_00176141);
        mc_memset(655360, 0, 64000, (int)D_001760D6, 285, 4);
        xn_vid_play(path, 0, 0, 1);
        mc_memset(655360, 0, 64000, (int)D_001760D6, 287, 4);
        palette_restore();
        night_sky_loaded = 0;
        sky_loaded_frame = 10000;
    } else if (player_character->special_infection_time != 0 && player_character->special_infection == 0 && ((int)(unsigned short)(player_character->flags & 16)) != 0) {
        player_character->flags &= ~0x10;
        while (mouse_buttons != 0) xn_mouse_poll_clamped();
        path = disk_resolve_path((int)D_0017614E);
        mc_memset(655360, 0, 64000, (int)D_001760D6, 297, 4);
        xn_vid_play(path, 0, 0, 1);
        mc_memset(655360, 0, 64000, (int)D_001760D6, 299, 4);
        palette_restore();
        night_sky_loaded = 0;
        sky_loaded_frame = 10000;
    }
    if (((unsigned)(game_minutes - last_skill_check_minutes)) <= 360) return;
    last_skill_check_minutes = game_minutes;
    raise_skills();
}

void fatigue_update(void)
{
    int delta;

    if (((int)(unsigned char)game_mode) == 16 || ((int)(unsigned char)game_mode) == 19) {
        return;
    }
    if (D_0018DDE4 != 0 && ((struct bf8_0_2 *)&frame_counter)->f == 0) return;
    delta = -11;
    if (in_dungeon_water != 0 && player_character->race != 7) {
        if (rand_range(1, 100) > player_character->skills[17].value) delta = -44;
        skill_add_uses(17, 1);
    }
    if (((struct bf8_4_1 *)&D_001940D9)->f != 0) delta = -88;
    if (((struct bf8_5_1 *)&player_motion_flags)->f != 0) delta = -22;
    fatigue_add(delta);
}

void fatigue_add(int amount)
{
    int max_fatigue;
    int fatigue;

    if (player_death_timer != 0) return;
    fatigue = player_character->fatigue;
    fatigue += amount;
    max_fatigue = (player_character->attributes[0] + player_character->attributes[4]) << 6;
    if (fatigue > max_fatigue) fatigue = max_fatigue;
    player_character->fatigue = fatigue;
    if (fatigue >= 1) return;
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
    struct membership *membership;
    int endurance;
    int bios_clock;
    int bios_clock2;
    int bios_clock3;
    int bios_clock4;

    endurance = player_character->attributes[4];
    if (D_00187CA8 == 0) return;
    if (endurance > 100) endurance = 100;
    if (((int)(unsigned char)fog_colour) != 107) {
        *(int *)breath_remaining = 0;
        bios_clock = 1132;
        breath_last_tick = *(int *)((char *)bios_clock);
        return;
    }
    if ((player_character->conditions & 0x80000) != 0) return;
    if (*(int *)breath_remaining == 0) {
        if ((*(int *)breath_remaining = endurance >> 1) > 50) *(int *)breath_remaining = 50;
        membership = guild_find_membership_by_kind(149);
        if (membership != 0) *(int *)breath_remaining += membership->rank * 3;
    }
    bios_clock2 = 1132;
    if (((unsigned)(*(int *)((char *)bios_clock2) - breath_last_tick)) > 18) {
        (*(int *)breath_remaining)--;
        bios_clock3 = 1132;
        if (player_character->race == 7 && ((struct bf8_0_1 *)((char *)bios_clock3))->f != 0) {
            (*(int *)breath_remaining)++;
        }
        bios_clock4 = 1132;
        breath_last_tick = *(int *)((char *)bios_clock4);
    }
    D_0012B508 = 145;
    if (((endurance >> 3) + 4) > *(int *)breath_remaining) D_0012B508 = 246;
    if (*(int *)breath_remaining != 0) {
        xn_draw_fill_rect(310, (int)(short)(120 - (*(short *)breath_remaining * 2)), 6, (int)(short)(*(short *)breath_remaining * 2));
    }
    if (*(int *)breath_remaining != 0) return;
    damage_creature_death(player_entity);
}

int rest_allowed(void)
{
    if (player_character->ship_owned != 0 && ((unsigned)(((unsigned)(int)location_object->id) >> 16)) < 1000) {
        return 1;
    }
    switch (player_environment) {
    case 1:
        if (((int)(unsigned short)(short)location_object->image) == 65535) return 1;
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
    struct building *building;

    building = object_building(player_object);
    if (building == 0) return 0;
    if (building->type != 15) return 0;
    if ((building->flags & 2) != 0 && game_minutes > building->rent_expires) return 1;
    return 0;
}

void weapon_reload_hand_sprites(void)
{
    weapon_free_sprites();
    weapon_load_hand_sprite(player_character->equipped[21], 1);
    weapon_load_hand_sprite(player_character->equipped[19], 0);
}
