/* weapons.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

struct bf8_5_1 { unsigned char _:5; unsigned char f:1; };
struct bf8_6_1 { unsigned char _:6; unsigned char f:1; };
extern char pick_distance[];
extern char mouse_buttons[];
extern char D_001343C0[];
extern char D_0017615C[];
extern char D_00176166[];
extern char D_00176175[];
extern char D_00176184[];
extern char player_environment[];
extern char D_00185097[];
extern char weapon_cif_by_index[];
extern char weapon_swing_types[];
extern char swing_to_hit_mods[];
extern char swing_damage_mods[];
extern char D_001875AE[];
extern char D_001875B7[];
extern char text_buffer[];
extern struct record *D_00190504[];
extern char D_001940D6[];
extern char D_0019597C[];
extern struct building *current_building;
extern struct record *player_entity;
extern struct record *player_object;
extern char D_00195AB0[];
extern struct record *D_00195AC4;
extern char D_00195AE0[];
extern char creature_count[];
extern char spell_ready_missile[];
extern char hud_bar_image[];
extern char D_00195B84[];
extern struct character *player_character;
extern struct settings *game_settings;
extern char D_00195C48[];
extern char D_00195D5C[];
extern char mouse_motion_x[];
extern char mouse_motion_y[];
extern char weapon_active_hand[];
extern char mouse_buttons_prev[];
extern char crime_current[];
extern char loaded_location[];
extern char collide_flags[];
extern char D_001A4A30[];
extern char D_001A4A38[];
extern char D_001A4A48[];
extern char weapon_hand_cif[];
extern char D_001A4A5C[];
extern char D_001A4A60[];
extern char D_001A4A68[];
extern char D_001A4A70[];
extern char swing_to_hit[];
extern char swing_damage[];

extern int lockpick_door(struct record *);
extern struct record *func_00013A00(struct record *, int, unsigned short);
extern int collide_move_missile(struct record *, int, int);
extern int func_00041347(void);
extern int key_action_held(int);
extern int building_is_open(int);
extern int sound_play(int, struct record *, int);
extern int disk_read_file(int, int);
extern int click_world_face(int);
extern int hud_message_add(int);
extern int rand_range(int, int);
extern int object_free_single(struct record *);
extern struct record *object_create_child(struct record *, int, int);
extern int inv_take_arrow(int);
extern int door_start_swing(struct record *, int);
extern int mc_free();
extern int mc_memset();
extern int func_000A0ED9(int, int);
extern int mc_sprintf(int, ...);
extern int mc_memcpy();
extern int func_000C2000();
extern int func_000C2043();
extern int func_000C2068();
extern int func_000C7FD9();
extern int func_000C7FF4();
extern int func_000C808D();
extern int func_000CB39A();
extern int func_000CE70D();
extern void lock_show_difficulty(int);
extern void damage_resolve_attack(struct record *, struct record *, int);
extern void guards_summon(int);
extern void cast_fire_missile(int);
extern void spell_area_effect(int);
extern void item_make(int, int, struct item *);
extern void links_trigger(struct record *, int);
extern void mem_check_crt_heap(int);
extern void fatigue_add(int);
extern void weapon_reload_hand_sprites(void);
extern void click_item(int, struct record *);
extern void func_00075E7B(int, struct record *);
extern void click_npc(int, struct record *);
extern void click_creature(int, struct record *);
extern void click_door(int, struct record *);
extern void func_0007606C(int, struct record *);
extern void click_interior_model(int, struct record *);
extern void click_corpse(int, struct record *);
extern void func_000763C6(int, struct record *);
extern void click_loot_container(int, struct record *);
extern void func_00076503(int, struct record *);
extern void object_free_later(struct record *);
extern void spell_cast_queued_run(void);
extern void inv_merge_arrows(struct record *, struct record *, int);
struct record *func_00073F5D(int, int, int);
void weapon_fire_arrow(void);
#pragma aux func_000A0ED9 parm routine [];

void weapon_load_hand_sprite(struct record *a1, int a2)
{
    struct item *l_18;
    int l_14;

    if (a1 == 0) goto L72941;
    if (player_character->race <= 8) goto L72982;
L72941:;
    *(int *)(D_001A4A68 + (a2 << 2)) = 0;
    l_14 = ((player_character->race < 9) ? 10 : 11);
    a1 = 0;
    goto L729E8;
L72982:;
    l_18 = &a1->data.item;
    if (l_18->group != 3) goto L72941;
    l_14 = (int)(unsigned char)*(signed char *)(weapon_cif_by_index + l_18->index);
    if (l_14 != 9) goto L729C8;
    *(int *)(D_001A4A68 + (a2 << 2)) = (int)D_001875B7;
    goto L729D8;
L729C8:;
    *(int *)(D_001A4A68 + (a2 << 2)) = 0;
L729D8:;
    *(int *)(D_001A4A60 + (a2 << 2)) = 0;
L729E8:;
    *(int *)(D_001A4A30 + (a2 << 2)) = (int)a1;
    if (a1 == 0) goto L72A09;
    if (l_18->enchantments[0].type != (-1)) goto L72A0B;
L72A09:;
    goto L72A1C;
L72A0B:;
    if (l_18->index != 17) goto L72A1E;
L72A1C:;
    goto L72A2F;
L72A1E:;
    if (l_18->index != 16) goto L72A31;
L72A2F:;
    goto L72A58;
L72A31:;
    func_000A0ED9(85, (int)D_0017615C);
    mc_sprintf((int)text_buffer, (int)D_00176166, l_14);
    goto L72A7D;
L72A58:;
    func_000A0ED9(87, (int)D_0017615C);
    mc_sprintf((int)text_buffer, (int)D_00176175, l_14);
L72A7D:;
    *(int *)(weapon_hand_cif + (a2 << 2)) = disk_read_file((int)text_buffer, 0);
}

void weapon_reload_sprites(void)
{
    weapon_reload_hand_sprites();
    mem_check_crt_heap(704);
}

int weapon_start_swing(int a1)
{
    short l_1C;
    int l_24;
    short l_18;

    if (key_action_held(33) != 0) goto L72AF2;
    return 0;
L72AF2:;
    l_1C = *(short *)mouse_motion_x;
    l_18 = *(short *)mouse_motion_y;
    l_24 = func_000C7FD9(0, 0, (int)(short)l_1C, (int)(short)l_18);
    if (l_24 >= 45) goto L72B2A;
    return 0;
L72B2A:;
    *(int *)(D_001A4A48 + (a1 << 2)) = *(int *)D_001343C0;
    l_24 = func_000C808D(0, 0, (int)(short)l_1C, (int)(short)l_18) >> 7;
    *(int *)(D_001A4A38 + (a1 << 2)) = (int)(unsigned char)*(signed char *)(weapon_swing_types + l_24);
    *(short *)swing_to_hit = *(short *)(swing_to_hit_mods + (l_24 * 2));
    *(short *)swing_damage = *(short *)(swing_damage_mods + (l_24 * 2));
    if (((int)(unsigned char)*(signed char *)(weapon_swing_types + l_24)) != 2) goto L72BBF;
    if (player_character->equipped[((int)(unsigned char)*(signed char *)weapon_active_hand) * 2 + 19] == 0) goto L72BC1;
L72BBF:;
    goto L72BEB;
L72BC1:;
    *(int *)(D_001A4A68 + (((int)(unsigned char)*(signed char *)weapon_active_hand) << 2)) = (int)D_001875AE;
    *(int *)(D_001A4A60 + (((int)(unsigned char)*(signed char *)weapon_active_hand) << 2)) = 0;
    goto L72BFF;
L72BEB:;
    *(int *)(D_001A4A68 + (((int)(unsigned char)*(signed char *)weapon_active_hand) << 2)) = 0;
L72BFF:;
    *(int *)(D_001A4A70 + (a1 << 2)) = 5;
    return 1;
}

void weapon_bow_update(void)
{
    if (((struct bf8_6_1 *)&D_001940D6)->f == 0) return;
    if (*(int *)(D_0019597C + (((int)(unsigned char)*(signed char *)weapon_active_hand) << 2)) != 0) return;
    if (*(int *)(D_001A4A60 + (((int)(unsigned char)*(signed char *)weapon_active_hand) << 2)) <= 200) goto L72CB0;
    *(int *)(D_001A4A60 + (((int)(unsigned char)*(signed char *)weapon_active_hand) << 2)) -= *(int *)D_00195AB0;
    if (*(int *)(D_001A4A60 + (((int)(unsigned char)*(signed char *)weapon_active_hand) << 2)) >= 200) goto L72CAB;
    *(int *)(D_001A4A60 + (((int)(unsigned char)*(signed char *)weapon_active_hand) << 2)) = 0;
L72CAB:;
    return;
L72CB0:;
    if (*(int *)(D_001A4A60 + (((int)(unsigned char)*(signed char *)weapon_active_hand) << 2)) != 200) goto L72CDA;
    *(int *)(D_001A4A60 + (((int)(unsigned char)*(signed char *)weapon_active_hand) << 2)) = 0;
L72CDA:;
    if (((int)(unsigned char)*(signed char *)((char *)(int)(*(char **)(D_001A4A68 + (((int)(unsigned char)*(signed char *)weapon_active_hand) << 2)) + *(int *)(D_001A4A60 + (((int)(unsigned char)*(signed char *)weapon_active_hand) << 2))))) != 3) goto L72D55;
    if (((int)(unsigned char)(*(signed char *)mouse_buttons_prev & 2)) != 0) goto L72D29;
    if (((int)(unsigned char)(*(signed char *)mouse_buttons & 2)) != 0) goto L72D2B;
L72D29:;
    goto L72D50;
L72D2B:;
    sound_play(6, player_object, 100);
    (*(int *)(D_001A4A60 + (((int)(unsigned char)*(signed char *)weapon_active_hand) << 2)))++;
L72D50:;
    goto L72DFE;
L72D55:;
    (*(int *)(D_001A4A60 + (((int)(unsigned char)*(signed char *)weapon_active_hand) << 2)))++;
    if (((int)(unsigned char)*(signed char *)((char *)(*(int *)(D_001A4A68 + (((int)(unsigned char)*(signed char *)weapon_active_hand) << 2)) + *(int *)(D_001A4A60 + (((int)(unsigned char)*(signed char *)weapon_active_hand) << 2))))) != 5) goto L72DA1;
    weapon_fire_arrow();
    fatigue_add(-11);
L72DA1:;
    if (((int)(unsigned char)*(signed char *)((char *)(int)(*(char **)(D_001A4A68 + (((int)(unsigned char)*(signed char *)weapon_active_hand) << 2)) + *(int *)(D_001A4A60 + (((int)(unsigned char)*(signed char *)weapon_active_hand) << 2))))) != 7) goto L72DFE;
    *(int *)(D_001A4A60 + (((int)(unsigned char)*(signed char *)weapon_active_hand) << 2)) = ((100 - player_character->attributes[6]) * 10) + 1000;
    return;
L72DFE:;
    func_000CB39A(*(int *)(weapon_hand_cif + (((int)(unsigned char)*(signed char *)weapon_active_hand) << 2)), (int)(unsigned char)*(signed char *)((char *)(*(int *)(D_001A4A68 + (((int)(unsigned char)*(signed char *)weapon_active_hand) << 2)) + *(int *)(D_001A4A60 + (((int)(unsigned char)*(signed char *)weapon_active_hand) << 2)))), ((((int)(unsigned short)(*(short *)(*(char **)&game_settings) & 1)) != 0) ? 0 : -((int)(unsigned short)*(short *)(*(char **)hud_bar_image + 6))), (int)(unsigned char)*(signed char *)weapon_active_hand);
}

void weapon_fire_arrow(void)
{
    char l_24[16];

    if (inv_take_arrow(1) != 0) goto L739AB;
    hud_message_add((int)D_00176184);
    return;
L739AB:;
    *(int *)((char *)l_24 + 12) = (int)object_create_child(player_object->parent, 0, 107);
    *(signed char *)(*(char **)((char *)l_24 + 12)) = 2;
    *(short *)(*(char **)((char *)l_24 + 12) + 29) = 998;
    *(short *)(*(char **)((char *)l_24 + 12) + 27) = 0;
    item_make(3, 18, (struct item *)(*(int *)((char *)l_24 + 12) + 71));
    *(signed char *)(*(char **)((char *)l_24 + 12) + 120) = 1;
    mc_memset((int)l_24, 0, 12, (int)D_0017615C, 404, 4);
    func_000CE70D(player_object->angle_x, player_object->yaw, 1024, (int)l_24);
    *(int *)l_24 += player_object->x;
    *(int *)((char *)l_24 + 4) += player_object->y;
    *(int *)((char *)l_24 + 8) += player_object->z;
    func_000C2000((int)player_object + 7, (int)l_24, *(int *)((char *)l_24 + 12) + 142);
    *(int *)(*(char **)((char *)l_24 + 12) + 7) = player_object->x;
    *(int *)(*(char **)((char *)l_24 + 12) + 11) = player_object->y - 70;
    if (((int)(unsigned short)(*(short *)(*(char **)&game_settings) & 1)) != 0) goto L73AA6;
    *(int *)(*(char **)((char *)l_24 + 12) + 11) -= 10;
L73AA6:;
    *(int *)(*(char **)((char *)l_24 + 12) + 15) = player_object->z;
    *(short *)(*(char **)((char *)l_24 + 12) + 25) = 1;
    func_000C2043(*(int *)((char *)l_24 + 12) + 142, 160, *(int *)((char *)l_24 + 12) + 7);
}

void func_00073ADF(struct record *a1)
{
{
    char l_24[12];

    mc_memcpy((int)l_24, (char *)a1 + 142, 12, (int)D_0017615C, 426, 12);
    func_000C2068((int)l_24);
    *(short *)((char *)a1 + 23) = (short)*(int *)l_24 & 2047;
    a1->angle_z = (short)*(int *)((char *)l_24 + 4) & 2047;
}
}

int weapon_arrow_update(struct record *a1)
{
    int l_24;
    int l_20;
    struct record *l_1C;
{
    char l_40[12];
    char l_34[12];

    l_20 = func_000C7FF4(player_object->y - a1->y, func_000C7FD9(player_object->x, player_object->z, a1->x, a1->z));
    if (l_20 <= 2048) goto L73BA7;
    object_free_later(a1);
    return 0;
L73BA7:;
    *(int *)l_34 = a1->x;
    *(int *)((char *)l_34 + 4) = a1->y;
    *(int *)((char *)l_34 + 8) = a1->z;
    func_000C2043((char *)a1 + 142, 40, (int)l_34);
    *(int *)l_40 = a1->angle_x;
    *(int *)((char *)l_40 + 4) = a1->yaw;
    *(int *)((char *)l_40 + 8) = 0;
    *(signed char *)collide_flags |= 4;
    l_24 = collide_move_missile(a1, (int)l_34, (int)l_40);
    if (*(short *)((char *)a1 + 25) != 0) goto L73CC8;
    l_20 = func_000C7FF4(player_object->y - a1->y, func_000C7FD9(player_object->x, player_object->z, a1->x, a1->z));
    if (l_20 >= 125) goto L73CA6;
    sound_play(7, player_object, 100);
    damage_resolve_attack(player_entity, player_entity, 19);
    inv_merge_arrows(player_entity, a1, 0);
    object_free_later(a1);
    return 0;
L73CA6:;
    if ((l_24 & 10) == 0) goto L73CC3;
    object_free_later(a1);
    return 0;
L73CC3:;
    goto L73DE5;
L73CC8:;
    if ((l_24 & 2) == 0) goto L73CF4;
    links_trigger(*(struct record **)D_00195C48, 5);
    object_free_later(a1);
    return 0;
L73CF4:;
    if ((l_24 & 8) == 0) goto L73D0E;
    if (((int)(unsigned char)*(signed char *)(*(char **)D_00195C48)) == 18) goto L73D10;
L73D0E:;
    goto L73D5D;
L73D10:;
    sound_play(7, *(struct record **)D_00195C48, 100);
    damage_resolve_attack(player_entity, *(struct record **)D_00195C48, 19);
    inv_merge_arrows(*(struct record **)D_00195C48, a1, 0);
    object_free_later(a1);
    return 0;
L73D5D:;
    l_1C = func_00073F5D(a1->x, a1->y, a1->z);
    if (l_1C != 0) goto L73D8E;
    object_free_later(a1);
    return 0;
L73D8E:;
    if (*(int *)D_00195B84 <= 140) goto L73DA3;
    return 1;
L73DA3:;
    sound_play(7, l_1C, 100);
    damage_resolve_attack(player_entity, l_1C, 19);
    inv_merge_arrows(l_1C, a1, 0);
    object_free_later(a1);
    return 0;
L73DE5:;
    return 1;
}
}

void weapon_monster_arrow(struct record *a1, struct record *a2)
{
    struct record *l_14;

    l_14 = object_create_child(D_00195AC4, 0, 107);
    l_14->type = 2;
    l_14->image2 = 998;
    l_14->image = 0;
    item_make(3, 18, &l_14->data.item);
    l_14->data.item.stack_count = 1;
    func_000C2000(&a1->x, &a2->x, (char *)l_14 + 142);
    *(short *)((char *)l_14 + 23) = 0;
    l_14->angle_z = 0;
    l_14->x = a1->x;
    l_14->y = a1->y - 60;
    l_14->z = a1->z;
    *(short *)((char *)l_14 + 25) = 0;
    *(int *)((char *)l_14 + 43) = 1;
    func_000C2043((char *)l_14 + 142, 160, &l_14->x);
}

void weapon_free_sprites(void)
{
    if (*(int *)weapon_hand_cif == 0) goto L73EFE;
    if (*(int *)weapon_hand_cif != (-1751672937)) goto L73F00;
L73EFE:;
    goto L73F1E;
L73F00:;
    mc_free(*(int *)weapon_hand_cif, (int)D_0017615C, 542);
    *(int *)weapon_hand_cif = -1751672937;
L73F1E:;
    if (*(int *)D_001A4A5C == 0) goto L73F33;
    if (*(int *)D_001A4A5C != (-1751672937)) goto L73F35;
L73F33:;
    return;
L73F35:;
    mc_free(*(int *)D_001A4A5C, (int)D_0017615C, 543);
    *(int *)D_001A4A5C = -1751672937;
}

struct record *func_00073F5D(int a1, int a2, int a3)
{
    int l_20;
    int l_1C;
    int l_18;
    int l_14;

    l_1C = 100000;
    l_20 = 0;
L73F80:;
    if (l_20 < *(int *)creature_count) goto L73F95;
    goto L73FF0;
L73F8D:;
    l_20++;
    goto L73F80;
L73F95:;
    l_14 = func_000C7FF4(D_00190504[l_20]->y - a2, func_000C7FD9(D_00190504[l_20]->x, D_00190504[l_20]->z, a1, a3));
    if (l_14 >= l_1C) goto L73FEE;
    l_1C = l_14;
    l_18 = l_20;
L73FEE:;
    goto L73F8D;
L73FF0:;
    *(int *)D_00195B84 = l_1C;
    if (l_1C != 100000) goto L7400A;
    return 0;
L7400A:;
    return D_00190504[l_18];
}

void func_00074024(int a1, struct record *a2)
{
    int l_14;

    l_14 = *(int *)loaded_location;
    *(int *)D_00195AE0 = a1;
    if (*(int *)spell_ready_missile == 0) goto L740CD;
    if (((int)(unsigned char)*(signed char *)(*(char **)spell_ready_missile + 78)) != 3) goto L740B4;
    *(int *)D_00195D5C = 0;
    *(int *)(*(char **)spell_ready_missile + 7) = player_object->x;
    *(int *)(*(char **)spell_ready_missile + 11) = player_object->y;
    *(int *)(*(char **)spell_ready_missile + 15) = player_object->z;
    spell_area_effect(*(int *)spell_ready_missile);
    spell_cast_queued_run();
    goto L740BE;
L740B4:;
    cast_fire_missile(*(int *)spell_ready_missile);
L740BE:;
    *(int *)spell_ready_missile = 0;
    return;
L740CD:;
    if (*(int *)pick_distance > 128) goto L740E6;
    links_trigger(a2, 2);
L740E6:;
    if (*(int *)pick_distance > 128) goto L740FE;
    if (click_world_face(a1) != 0) goto L74100;
L740FE:;
    goto L74105;
L74100:;
    return;
L74105:;
    if (l_14 != *(int *)loaded_location) return;
    switch (a2->type) {
    return;
case 2:
    click_item(a1, a2);
    return;
case 6:
    func_00075E7B(a1, a2);
    return;
case 8:
    click_npc(a1, a2);
    return;
case 18:
    click_creature(a1, a2);
    return;
case 32:
    click_door(a1, a2);
    return;
case 34:
    func_0007606C(a1, a2);
    return;
case 43:
    click_interior_model(a1, a2);
    return;
case 44:
    click_corpse(a1, a2);
    return;
case 53:
    func_000763C6(a1, a2);
    return;
case 33:
    click_loot_container(a1, a2);
    return;
case 56:
    func_00076503(a1, a2);
default:;
}
}

void func_0007425E(struct record *a1, int a2)
{
    int l_1C;
    int l_18;
    struct record *l_14;

    if (((int)(unsigned short)(a1->flags & 256)) == 0) goto L742EE;
    if (door_start_swing(a1, 1) == 0) goto L742E9;
    a1->flags &= ~0x100;
    if ((player_character->conditions & 0x20) == 0) goto L742CC;
    if (rand_range(1, 100) <= ((int)(unsigned char)(signed char)player_character->lock_open_chance)) goto L742CE;
L742CC:;
    goto L742E9;
L742CE:;
    a1->flags &= ~0x40;
    a1->lock_level = player_character->level;
L742E9:;
    return;
L742EE:;
    if (a1->lock_level == 0) goto L7430D;
    if (((int)(unsigned short)(a1->flags & 64)) == 0) goto L74374;
L7430D:;
    if ((player_character->conditions & 0x20) == 0) goto L7433B;
    if (rand_range(1, 100) <= ((int)(unsigned char)(signed char)player_character->lock_open_chance)) goto L7433D;
L7433B:;
    goto L7435A;
L7433D:;
    a1->flags &= ~0x40;
    a1->lock_level = player_character->level;
    goto L7436F;
L7435A:;
    if (door_start_swing(a1, 0) == 0) goto L7436F;
    a1->flags |= 0x100;
L7436F:;
    return;
L74374:;
    if ((int)current_building->id == player_character->house) goto L743A3;
    if (((int)(unsigned char)*(signed char *)player_environment) != 2) goto L743A1;
    if (building_is_open((int)current_building) != 0) goto L743A3;
L743A1:;
    goto L743B1;
L743A3:;
    if (door_start_swing(a1, 0) != 0) goto L743B3;
L743B1:;
    goto L743BF;
L743B3:;
    a1->flags |= 0x100;
    return;
L743BF:;
    l_14 = func_00013A00(player_entity, ((unsigned)a1->id) >> 16, a1->lock_level);
    if (l_14 != 0) goto L744D5;
    if (a2 != 0) goto L7446E;
    if ((player_character->conditions & 0x40) == 0) goto L7445B;
    l_18 = (int)(unsigned char)(signed char)player_character->lock_open_chance;
    player_character->conditions &= ~0x40;
    if (rand_range(1, 100) > l_18) goto L7443F;
    if (door_start_swing(a1, 0) != 0) goto L74441;
L7443F:;
    goto L7444C;
L74441:;
    a1->flags |= 320;
    goto L74456;
L7444C:;
    hud_message_add(*(int *)D_00185097);
L74456:;
    return;
L7445B:;
    lock_show_difficulty(a1->lock_level);
    goto L744D3;
L7446E:;
    if (lockpick_door(a1) == 0) goto L74491;
    if (door_start_swing(a1, 0) == 0) goto L74491;
    a1->flags |= 320;
L74491:;
    if (func_00041347() != 0) goto L744C2;
    if (rand_range(1, 300) >= (100 - ((int)(short)player_character->skills[16].value))) goto L744D3;
L744C2:;
    *(signed char *)crime_current = 1;
    guards_summon(1);
L744D3:;
    return;
L744D5:;
    if (door_start_swing(a1, 0) == 0) return;
    object_free_single(l_14);
    a1->flags |= 320;
}
