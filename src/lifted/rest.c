/* rest.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

struct bf8_0_1 { unsigned char f:1; };
struct bf8_0_2 { unsigned char f:2; };
struct bf8_3_1 { unsigned char _:3; unsigned char f:1; };
struct bf8_4_1 { unsigned char _:4; unsigned char f:1; };
struct bf8_5_1 { unsigned char _:5; unsigned char f:1; };
extern char mouse_buttons[];
extern char D_0012B508[];
extern char D_001760D6[];
extern char D_00176141[];
extern char D_0017614E[];
extern char player_environment[];
extern char D_00185087[];
extern char D_00187CA8[];
extern char D_0018DDE4[];
extern char D_001940D9[];
extern char player_motion_flags[];
extern char D_00195998[];
extern char frame_counter[];
extern struct building *current_building;
extern struct record *player_entity;
extern struct record *player_object;
extern struct record *D_00195AC4;
extern struct building *tavern_building;
extern char creature_count[];
extern struct location *current_location;
extern struct character *player_character;
extern char game_minutes[];
extern char D_00195CDC[];
extern char D_00195D48[];
extern char player_death_timer[];
extern char D_0019626F[];
extern char D_00196272[];
extern char game_mode[];
extern char in_dungeon_water[];
extern char crime_current[];
extern char D_00196283[];
extern char D_00196294[];
extern char D_0019629B[];
extern char D_001962A4[];
extern char D_001A4A24[];
extern char rest_image[];

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

    if (((int)(unsigned char)*(signed char *)game_mode) != 16) return;
    if (*(int *)rest_image == 0) goto L7215E;
    if (*(int *)rest_image != (-1751672937)) goto L72160;
L7215E:;
    goto L7217E;
L72160:;
    mc_free(*(int *)rest_image, (int)D_001760D6, 271);
    *(int *)rest_image = -1751672937;
L7217E:;
    *(signed char *)D_00196272 = 0;
    *(signed char *)game_mode = 0;
    *(signed char *)D_0019626F = 16;
    *(signed char *)D_00187CA8 = 1;
    *(signed char *)D_00196294 = 0;
    *(signed char *)D_0019629B = 0;
    *(int *)D_00195D48 = 10000;
    if (player_character->special_infection_time == 0) goto L721CC;
    if (player_character->special_infection != 0) goto L721CE;
L721CC:;
    goto L721E5;
L721CE:;
    if (((int)(unsigned short)(player_character->flags & 16)) != 0) goto L721EA;
L721E5:;
    goto L72276;
L721EA:;
    player_character->flags &= ~0x10;
L721F3:;
    if (*(signed char *)mouse_buttons == 0) goto L72203;
    func_0012B136();
    goto L721F3;
L72203:;
    l_18 = disk_resolve_path((int)D_00176141);
    mc_memset(655360, 0, 64000, (int)D_001760D6, 285, 4);
    func_000C1500(l_18, 0, 0, 1);
    mc_memset(655360, 0, 64000, (int)D_001760D6, 287, 4);
    palette_restore();
    *(signed char *)D_0019629B = 0;
    *(int *)D_00195D48 = 10000;
    goto L72335;
L72276:;
    if (player_character->special_infection_time == 0) goto L72290;
    if (player_character->special_infection == 0) goto L72292;
L72290:;
    goto L722A9;
L72292:;
    if (((int)(unsigned short)(player_character->flags & 16)) != 0) goto L722AE;
L722A9:;
    goto L72335;
L722AE:;
    player_character->flags &= ~0x10;
L722B7:;
    if (*(signed char *)mouse_buttons == 0) goto L722C7;
    func_0012B136();
    goto L722B7;
L722C7:;
    l_18 = disk_resolve_path((int)D_0017614E);
    mc_memset(655360, 0, 64000, (int)D_001760D6, 297, 4);
    func_000C1500(l_18, 0, 0, 1);
    mc_memset(655360, 0, 64000, (int)D_001760D6, 299, 4);
    palette_restore();
    *(signed char *)D_0019629B = 0;
    *(int *)D_00195D48 = 10000;
L72335:;
    if (((unsigned)(*(int *)game_minutes - *(int *)D_00195998)) <= 360) return;
    *(int *)D_00195998 = *(int *)game_minutes;
    raise_skills();
}

void fatigue_update(void)
{
    int l_18;

    if (((int)(unsigned char)*(signed char *)game_mode) == 16) goto L72386;
    if (((int)(unsigned char)*(signed char *)game_mode) != 19) goto L7238B;
L72386:;
    return;
L7238B:;
    if (*(int *)D_0018DDE4 == 0) goto L7239D;
    if (((struct bf8_0_2 *)&frame_counter)->f == 0) goto L7239F;
L7239D:;
    goto L723A4;
L7239F:;
    return;
L723A4:;
    l_18 = -11;
    if (*(signed char *)in_dungeon_water == 0) goto L723C6;
    if (player_character->race != 7) goto L723C8;
L723C6:;
    goto L723FD;
L723C8:;
    if (rand_range(1, 100) <= player_character->skills[17].value) goto L723EE;
    l_18 = -44;
L723EE:;
    skill_add_uses(17, 1);
L723FD:;
    if (((struct bf8_4_1 *)&D_001940D9)->f == 0) goto L7240D;
    l_18 = -88;
L7240D:;
    if (((struct bf8_5_1 *)&player_motion_flags)->f == 0) goto L7241D;
    l_18 = -22;
L7241D:;
    fatigue_add(l_18);
}

void fatigue_add(int a1)
{
    int l_1C;
    int l_18;

    if (*(int *)player_death_timer != 0) return;
    l_18 = player_character->fatigue;
    l_18 += a1;
    l_1C = (player_character->attributes[0] + player_character->attributes[4]) << 6;
    if (l_18 <= l_1C) goto L7248D;
    l_18 = l_1C;
L7248D:;
    player_character->fatigue = l_18;
    if (l_18 >= 1) return;
    *(signed char *)D_0012B508 = 146;
    player_character->fatigue = 0;
    if (*(int *)creature_count == 0) goto L724DC;
    msgbox_show_rsc(1072, 1);
    damage_creature_death(player_entity);
    return;
L724DC:;
    msgbox_show_rsc(1071, 1);
    *(signed char *)D_001962A4 = 1;
    time_pass_minutes(60);
    rest_recover((int)player_entity);
    *(signed char *)D_001962A4 = 0;
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
    if (*(signed char *)D_00187CA8 == 0) return;
    if (l_28 <= 100) goto L7254B;
    l_28 = 100;
L7254B:;
    if (((int)(unsigned char)*(signed char *)D_00196283) == 107) goto L72577;
    *(int *)D_00195CDC = 0;
    l_24 = 1132;
    *(int *)D_001A4A24 = *(int *)((char *)l_24);
    return;
L72577:;
    if ((player_character->conditions & 0x80000) != 0) return;
    if (*(int *)D_00195CDC != 0) goto L725D5;
    if ((*(int *)D_00195CDC = l_28 >> 1) <= 50) goto L725AF;
    *(int *)D_00195CDC = 50;
L725AF:;
    l_2C = guild_find_membership_by_kind(149);
    if (l_2C == 0) goto L725D5;
    *(int *)D_00195CDC += l_2C->rank * 3;
L725D5:;
    l_20 = 1132;
    if (((unsigned)(*(int *)((char *)l_20) - *(int *)D_001A4A24)) <= 18) goto L7262C;
    (*(int *)D_00195CDC)--;
    l_1C = 1132;
    if (player_character->race != 7) goto L72613;
    if (((struct bf8_0_1 *)((char *)l_1C))->f != 0) goto L72615;
L72613:;
    goto L7261B;
L72615:;
    (*(int *)D_00195CDC)++;
L7261B:;
    l_18 = 1132;
    *(int *)D_001A4A24 = *(int *)((char *)l_18);
L7262C:;
    *(signed char *)D_0012B508 = 145;
    if (((l_28 >> 3) + 4) <= *(int *)D_00195CDC) goto L7264B;
    *(signed char *)D_0012B508 = 246;
L7264B:;
    if (*(int *)D_00195CDC == 0) goto L72682;
    func_00144D00(310, (int)(short)(120 - (*(short *)D_00195CDC * 2)), 6, (int)(short)(*(short *)D_00195CDC * 2));
L72682:;
    if (*(int *)D_00195CDC != 0) return;
    damage_creature_death(player_entity);
}

int rest_allowed(void)
{
    if (player_character->ship_owned == 0) goto L726CA;
    if (((unsigned)(((unsigned)(int)D_00195AC4->id) >> 16)) < 1000) goto L726CC;
L726CA:;
    goto L726D8;
L726CC:;
    return 1;
L726D8:;
    switch (*(unsigned char *)player_environment) {
case 1:
    if (((int)(unsigned short)(short)D_00195AC4->image) != 65535) goto L72721;
    return 1;
L72721:;
    if (((int)(unsigned char)(signed char)current_location->kind) == 4) goto L72745;
    if (((int)(unsigned char)(signed char)current_location->kind) != 7) goto L72747;
L72745:;
    goto L72759;
L72747:;
    if (((int)(unsigned char)(signed char)current_location->kind) <= 9) goto L72765;
L72759:;
    return 1;
L72765:;
    *(signed char *)crime_current = 8;
    guards_summon(1);
    msgbox_show_rsc(17, 1);
    return 0;
case 2:
    if (((int)(unsigned char)(signed char)current_building->type) != 15) goto L727DF;
    tavern_building = current_building;
    if (tavern_room_rented() != 0) goto L727D1;
    msgbox_show_string(*(int *)D_00185087, 1);
    return 0;
L727D1:;
    tavern_go_to_room();
    return 1;
L727DF:;
    if (((int)(unsigned short)(short)current_building->faction_id) != 41) goto L72800;
    if (guild_find_membership_by_kind(2) != 0) goto L72802;
L72800:;
    goto L72810;
L72802:;
    tavern_go_to_room();
    return 1;
L72810:;
    if ((int)current_building->id == player_character->house) goto L7283D;
    *(signed char *)crime_current = 8;
    guards_summon(1);
    return 0;
default:
L7283D:;
    return 1;
}
}

int rest_room_expired(void)
{
    struct building *l_1C;

    l_1C = object_building(player_object);
    if (l_1C != 0) goto L7287B;
    return 0;
L7287B:;
    if (l_1C->type == 15) goto L72894;
    return 0;
L72894:;
    if ((l_1C->flags & 2) == 0) goto L728B3;
    if (*(int *)game_minutes > l_1C->rent_expires) goto L728B5;
L728B3:;
    goto L728BE;
L728B5:;
    return 1;
L728BE:;
    return 0;
}

void weapon_reload_hand_sprites(void)
{
    weapon_free_sprites();
    weapon_load_hand_sprite(player_character->equipped[21], 1);
    weapon_load_hand_sprite(player_character->equipped[19], 0);
}
