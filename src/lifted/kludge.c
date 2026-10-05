/* kludge.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

struct bf8_1_1 { unsigned char _:1; unsigned char f:1; };
struct bf8_2_1 { unsigned char _:2; unsigned char f:1; };
extern signed char D_0012B508;
extern int D_00132F6C[];
extern signed char D_00142314;
extern signed char D_00142315;
extern char D_00171044[];
extern char D_0017104D[];
extern char D_0017105A[];
extern char D_0017105D[];
extern char D_00171062[];
extern char D_00171077[];
extern char D_00171083[];
extern char D_00171096[];
extern char D_001710A7[];
extern char D_001710BA[];
extern char D_001710C3[];
extern char D_001710D6[];
extern char D_001710EB[];
extern signed char D_0017BABC[];
extern signed char D_0017BABD[];
extern char D_0017BAE2[];
extern char D_0017BB01[];
extern signed char D_0017BB39[];
extern signed char D_0017BB3F[];
extern char potion_recipes[];
extern char D_00180B42[];
extern char monster_names[];
extern signed char region_event_values[];
extern signed char text_buffer[];
extern char D_001903BA[];
extern signed char itemmaker_slot_kinds[];
extern signed char D_00190CE5;
extern char D_001917E4[];
extern char D_00191834[];
extern signed char D_001940D4;
extern signed char D_001940D6;
extern signed char D_001940D9;
extern signed char D_001940DA;
extern struct record *inventory_containers[];
extern struct record *D_001959DC;
extern struct record *D_001959E0;
extern struct record *D_001959E4;
extern struct record *player_entity;
extern struct record *D_00195AC4;
extern struct spell *spell_records;
extern char clothing_gender_group[];
extern char current_region_data[];
extern struct character *player_character;
extern struct career *player_class;
extern int game_minutes;
extern int spell_record_count;
extern signed char D_00196272;
extern signed char game_mode;
extern signed char cfg_artifact;
extern signed char D_001962AB;
extern int faction_count;
extern struct faction *factions;
extern int loaded_location_door_count;
extern int loaded_location_doors;
extern int D_00199634;
extern struct record *D_00199714;
extern int D_00199718;
extern struct record *D_00199724;
extern signed char D_00199728;
extern signed char cfg_gender;

extern struct faction *faction_find(short);
extern int spells_list_poll(void);
extern int sheet_open(int);
extern struct record *func_0004596E(struct record *, unsigned short);
extern int func_00045AED(struct record *, int);
extern int disk_read_file(int, int);
extern struct membership *guild_find_membership_by_kind(unsigned char);
extern int hud_message_add(int);
extern int rand_range(int, int);
extern int object_free_single(struct record *);
extern struct record *object_delete(struct record *);
extern struct record *object_create_child(struct record *, int, int);
extern int object_reparent(struct record *, struct record *);
extern struct record *object_find_item(struct record *, int, int);
extern int object_new_id(int);
extern int func_0009DA1C(int, int);
extern int printf(int, ...);
extern int rand();
extern int srand();
extern int open(int, ...);
extern int func_0009DEA7();
extern int mc_memset();
extern int func_000A00CB();
extern int mc_strncpy();
extern int func_000A0ED9(int, int);
extern int mc_sprintf(int, ...);
extern int mc_memcpy();
extern int func_000A148C(int, ...);
extern int fprintf(int, ...);
extern int func_000C7FD9();
extern int func_000CAE1C();
extern void pickpocket_attempt(struct record *);
extern void func_0001D739(void);
extern void player_refresh_paperdoll(void);
extern void text_draw(int, int, int);
extern void func_0005E37F(unsigned short, int, int, struct item *);
extern void item_make(int, int, struct item *);
extern void item_make_artifact(struct item *, int);
extern void guild_join_dark_brotherhood(void);
extern void monster_init(struct record *, int);
extern void picklist_open(int);
extern void object_free_children(struct record *);
extern void object_foreach(struct record *, int);
extern void inv_equip_item(struct record *);
extern void inv_store_item(struct record *);
extern void inv_create_wagon(void);
extern void inv_add_arrows(struct record *, unsigned char);
struct record *item_add_to_container(struct record *, int, int, int);
int class_has_magic_skill(struct career *);
void kludge_make_test_character(int);
void starting_spells_give(struct record *);
void func_00046114(struct record *);
#pragma aux func_0009DA1C parm routine [];
#pragma aux func_000A0ED9 parm routine [];

void kludge_print_build(int a1)
{
    int l_1C;
    int l_18;

    func_000A0ED9(78, (int)D_00171044);
    mc_sprintf((int)text_buffer, (int)D_0017104D, (int)D_001917E4);
    l_1C = open((int)text_buffer, 512);
    if (l_1C < 0) {
        func_000A0ED9(81, (int)D_00171044);
        mc_sprintf((int)text_buffer, (int)D_0017104D, (int)D_00191834);
        l_1C = open((int)text_buffer, 512);
        if (l_1C < 0) return;
    }
    func_000A00CB(l_1C, (int)text_buffer, 80);
    fprintf((int)D_001903BA, (int)D_0017105A, (int)&l_18);
    if (a1 != 0) {
        func_0009DA1C(90, (int)D_00171044);
        printf((int)D_0017105D, l_18);
    } else {
        func_000A0ED9(92, (int)D_00171044);
        func_000A148C((int)D_00171062, l_18);
    }
    func_0009DEA7(l_1C);
}

void func_00044E20(struct record *a1)
{
    if (a1->type != 18) return;
    D_00199724 = a1;
}

void kludge_menu_open(void)
{
    picklist_open((int)D_0017BB01);
    D_00199728 = 1;
    game_mode = 200;
    D_00196272 = 1;
}

void kludge_menu_update(void)
{
    int l_18;

    if (D_00199728 == 0) return;
    if (((struct bf8_2_1 *)&D_001940D4)->f == 0 || (l_18 = spells_list_poll()) <= (-1)) return;
    D_00199728 = 0;
    game_mode = 0;
    D_00196272 = 0;
    func_000CAE1C(l_18);
}

void show_rumor(void)
{
    func_0001D739();
}

void kludge_good_merchant_rep(void)
{
    player_character->reputation[1] = (rand() % 50) + 10;
}

void kludge_jump_month(void)
{
    game_minutes += 43200;
}

void kludge_advance_level(void)
{
    D_001940D9 |= 4;
    sheet_open(1);
}

void func_00044F85(int a1)
{
    int l_24;
    struct record *l_20;
    int l_1C;
    short l_18;

    kludge_make_test_character(a1);
    *(int *)&l_18 = 0;
    for (; ((int)(short)l_18) < spell_record_count; (*(int *)&l_18)++) {
        if (spell_records[(int)(short)l_18].name[0] == 0) continue;
        l_20 = object_find_item(player_entity->children, 27, 0);
        l_20 = object_create_child(l_20, 0, 89);
        l_20->type = 9;
        l_20->id = object_new_id(100);
        mc_memcpy(&l_20->data.spell, &spell_records[(short)l_18], 89, (int)D_00171044, 165, 4);
    }
}

void kludge_make_test_character(int a1)
{
    struct record *l_38;
    int l_34;
    int l_30;
    int l_2C;
    int l_28;
    int l_24;
    struct item *l_20;
    int l_1C;
    int l_18;

    l_18 = rand();
    srand(12345);
    D_001940D9 |= 8;
    if (cfg_gender != 0 && a1 != 0) player_character->flags = 1;
    if (((int)(unsigned short)(player_character->flags & 1)) != 0) {
        *(int *)clothing_gender_group = 12;
    } else {
        *(int *)clothing_gender_group = 6;
    }
    player_character->gold = 5000;
    player_character->level = 2;
    if (a1 != 0) {
        disk_read_file((int)D_00171077, (int)player_class);
        player_class->forbidden_equipment = 0;
        player_class->forbidden_materials = 0;
        mc_strncpy(player_character->name, (int)D_00171083, 32, (int)D_00171044, 197);
        mc_memcpy(player_character->attributes, (int)D_0017BAE2, 16, (int)D_00171044, 198, 16);
        mc_memcpy(player_character->base_attributes, player_character->attributes, 16, (int)D_00171044, 199, 16);
        player_character->min_metal_to_hit = 0;
        for (l_1C = 0; l_1C < 35; l_1C++) {
            player_character->skills[l_1C].value = rand_range(10, 60);
        }
        player_character->skills[22].value = 30;
        player_character->skills[23].value = 30;
        player_character->skills[24].value = 30;
        player_character->skills[25].value = 30;
        player_character->skills[26].value = 30;
        player_character->skills[27].value = 30;
    }
    guild_join_dark_brotherhood();
    faction_find(108)->reputation = 99;
    guild_find_membership_by_kind(0)->rank = 9;
    for (l_1C = 0; l_1C < 35; l_1C++) {
        player_character->skills[l_1C].pad4 = player_character->skills[l_1C].value;
    }
    *(int *)current_region_data = (int)region_event_values;
    l_38 = object_create_child(D_001959E0, 0, 107);
    l_38->image2 = 202;
    l_38->type = 2;
    l_20 = &l_38->data.item;
    func_0005E37F((int)(unsigned short)*(short *)clothing_gender_group, 10, 11, l_20);
    l_38 = object_create_child(D_001959E0, 0, 107);
    l_38->image2 = 203;
    l_38->type = 2;
    l_20 = &l_38->data.item;
    func_0005E37F((int)(unsigned short)*(short *)clothing_gender_group, 0, 4, l_20);
    item_add_to_container(D_001959E0, *(int *)clothing_gender_group, 0, 1);
    item_add_to_container(D_001959E0, *(int *)clothing_gender_group, 17, 1);
    item_add_to_container(D_001959E0, *(int *)clothing_gender_group, 9, 0);
    item_add_to_container(D_001959E0, *(int *)clothing_gender_group, 10, 0);
    item_add_to_container(D_001959E0, 23, 1, 0);
    l_38 = item_add_to_container(player_entity, 23, 0, 0);
    inv_create_wagon();
    func_00045AED(inventory_containers[0], 2);
    func_00045AED(inventory_containers[0], 2);
    func_00045AED(inventory_containers[0], 2);
    func_00045AED(inventory_containers[0], 2);
    item_add_to_container(inventory_containers[0], 3, 7, 0);
    D_001962AB = 1;
    item_add_to_container(inventory_containers[0], 3, 1, 0);
    item_add_to_container(inventory_containers[0], 3, 17, 0);
    item_add_to_container(inventory_containers[0], 2, 5, 0);
    func_00045AED(D_001959E0, *(int *)clothing_gender_group);
    inv_add_arrows(player_entity, 250);
    func_0004596E(D_001959DC, 4);
    func_0004596E(D_001959DC, 4);
    func_0004596E(D_001959E4, 20);
    func_0004596E(D_001959E4, 20);
    func_0004596E(D_001959E4, 17);
    item_add_to_container(D_001959E4, 21, 4, 0);
    item_add_to_container(D_001959E4, 17, 1, 0);
    item_add_to_container(D_001959E0, 25, 2, 0);
    item_add_to_container(D_001959E0, 27, 4, 0);
    item_add_to_container(D_001959DC, 27, 0, 0);
    item_add_to_container(D_001959E0, 27, 1, 0);
    item_add_to_container(D_001959E0, 27, 2, 0);
    item_add_to_container(D_001959E0, 7, 0, 0);
    item_add_to_container(D_001959E0, 7, 0, 0);
    item_add_to_container(D_001959E0, 7, 0, 0);
    item_add_to_container(D_001959E0, 7, 0, 0);
    item_add_to_container(D_001959E0, 7, 0, 0);
    item_add_to_container(D_001959E0, 13, 0, 0);
    l_38 = object_find_item(player_entity->children, 27, 1);
    l_38 = object_create_child(l_38, 0, 0);
    l_38->type = 20;
    l_38->flags = 3;
    l_38->image = 31;
    l_38 = object_find_item(player_entity->children, 27, 2);
    l_20 = &l_38->data.item;
    l_20->value = 10000;
    l_38 = object_create_child(D_00195AC4, 0, 107);
    l_38->type = 2;
    l_38->flags |= 1;
    item_make(1, 1, &l_38->data.item);
    l_38->data.item.value = (int)(unsigned short)*(short *)D_00180B42;
    inv_store_item(l_38);
    l_38 = object_create_child(l_38, 0, 109);
    l_38->type = 31;
    mc_memcpy(&l_38->data.potion_recipe, (int)potion_recipes, 109, (int)D_00171044, 289, 4);
    l_38 = object_create_child(D_001959DC, 0, 107);
    l_38->type = 2;
    l_20 = &l_38->data.item;
    item_make_artifact(l_20, 7);
    l_38 = object_create_child(D_001959DC, 0, 107);
    l_38->type = 2;
    l_20 = &l_38->data.item;
    item_make_artifact(l_20, 8);
    l_38 = object_create_child(D_001959DC, 0, 107);
    l_38->type = 2;
    l_20 = &l_38->data.item;
    item_make_artifact(l_20, 2);
    l_38 = object_create_child(D_001959DC, 0, 107);
    l_38->type = 2;
    l_20 = &l_38->data.item;
    item_make_artifact(l_20, 10);
    l_38 = object_create_child(D_001959DC, 0, 107);
    l_38->type = 2;
    l_20 = &l_38->data.item;
    item_make_artifact(l_20, (int)(unsigned char)cfg_artifact);
    player_refresh_paperdoll();
    D_001940D9 &= 247;
    srand(l_18);
}

void starting_equipment_give(void)
{
    int l_20;
    int l_1C;
    struct record *l_18;

    D_001940D9 |= 8;
    if (((int)(unsigned short)(player_character->flags & 1)) != 0) {
        *(int *)clothing_gender_group = 12;
    } else {
        *(int *)clothing_gender_group = 6;
    }
    if (((int)(unsigned short)(player_character->flags & 1)) != 0) {
        item_add_to_container(D_001959E0, *(int *)clothing_gender_group, 8, 1);
    } else {
        item_add_to_container(D_001959E0, *(int *)clothing_gender_group, 10, 1);
    }
    item_add_to_container(D_001959E0, *(int *)clothing_gender_group, 24, 1);
    if (((int)(unsigned char)D_0017BABC[D_00199634 * 2]) != 255 && ((int)(unsigned char)D_0017BABC[D_00199634 * 2]) != 254) {
        D_001962AB = D_0017BABD[D_00199634 * 2] + 1;
        l_18 = item_add_to_container(inventory_containers[0], 3, (int)(unsigned char)D_0017BABC[D_00199634 * 2], 0);
        l_18->data.item.color = l_18->data.item.material + 16;
        l_18->data.item.value = 15;
    } else if (((int)(unsigned char)D_0017BABC[D_00199634 * 2]) == 254) {
        l_18 = item_add_to_container(inventory_containers[0], 3, 17, 0);
        D_001962AB = 1;
        l_18 = item_add_to_container(inventory_containers[0], 3, 14, 0);
        inv_add_arrows(player_entity, 24);
    }
    item_add_to_container(D_001959DC, 27, 0, 0);
    starting_spells_give(object_find_item(player_entity->children, 27, 0));
    player_refresh_paperdoll();
    D_001940D9 &= 247;
}

struct record *item_add_to_container(struct record *a1, int a2, int a3, int a4)
{
    struct record *l_18;
    struct record *l_14;
    struct item *l_10;

    l_18 = object_create_child(a1, 0, 107);
    l_18->type = 2;
    l_10 = &l_18->data.item;
    item_make((int)(unsigned short)*(short *)&a2, a3, l_10);
    if ((l_10->item_flags & 8) != 0) {
        l_14 = object_create_child(a1, 0, 107);
        l_14->type = 2;
        object_reparent(l_14, l_18);
        l_10 = &l_14->data.item;
        item_make(1, 1, l_10);
        if (a4 != 0) inv_equip_item(l_14);
        return l_14;
    }
    if (a4 != 0) inv_equip_item(l_18);
    return l_18;
}

void kludge_toggle_world_position(void)
{
    D_001940D6 ^= 1;
}

void kludge_toggle_second_compass(void)
{
    D_001940D6 ^= 8;
}

void func_00045B7F(void)
{
    struct record *l_1C;
    int l_18;

    itemmaker_slot_kinds[0] = 1;
    player_character->skills[15].value = 80;
    l_1C = object_create_child(D_00195AC4, 0, 659);
    l_1C->type = 18;
    l_18 = rand() % 20;
    itemmaker_slot_kinds[0] = 2;
    D_00190CE5 = *(signed char *)&l_18;
    monster_init(l_1C, l_18);
    itemmaker_slot_kinds[0] = 3;
    func_000A0ED9(448, (int)D_00171044);
    mc_sprintf((int)text_buffer, (int)D_00171096, *(int *)(monster_names + (l_18 << 2)));
    hud_message_add((int)text_buffer);
    itemmaker_slot_kinds[0] = 4;
    pickpocket_attempt(l_1C);
    itemmaker_slot_kinds[0] = 5;
    object_free_children(l_1C);
    itemmaker_slot_kinds[0] = 6;
    object_free_single(l_1C);
    itemmaker_slot_kinds[0] = 7;
}

void kludge_toggle_quest_debug(void)
{
    D_001940DA ^= 2;
    if (((struct bf8_1_1 *)&D_001940DA)->f == 0) return;
    hud_message_add((int)D_001710A7);
}

void func_00045C9D(void)
{
    struct record *l_18;

    mc_memset(player_character->equipped, 0, 108, (int)D_00171044, 469, 108);
    l_18 = player_entity->children;
    while (l_18 != 0) l_18 = object_delete(l_18);
}

void kludge_show_memory(void)
{
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    D_0012B508 = 146;
    l_24 = 0;
    l_20 = l_24;
    l_1C = l_20;
    for (; l_24 < 512; l_24++) {
        if (D_00132F6C[l_24] == 0) continue;
        l_18 = D_00132F6C[l_24] - 22;
        func_000A0ED9(487, (int)D_00171044);
        mc_sprintf((int)text_buffer, (int)D_001710BA, *(int *)((char *)l_18 + 8), D_00132F6C[l_24] + 2);
        text_draw((int)text_buffer, (l_20 / 25) * 160, (l_20 % 25) << 3);
        l_20++;
        l_1C += *(int *)((char *)l_18 + 8);
    }
    D_0012B508 = 245;
    func_000A0ED9(494, (int)D_00171044);
    mc_sprintf((int)text_buffer, (int)D_001710C3, l_1C);
    text_draw((int)text_buffer, (l_20 / 25) * 160, (l_20 % 25) << 3);
}

int func_00045E45(int a1)
{
    short l_1C;
    int l_24;
    short l_18;

    *(int *)&l_18 = a1;
    *(int *)&l_1C = loaded_location_doors;
    for (l_24 = 0; l_24 < loaded_location_door_count; l_24++, (*(char (**)[6])&(*(int *)&l_1C))++) {
        if ((short)*(int *)&l_18 == *(short *)(*(char **)&l_1C + 4)) return *(int *)&l_1C;
    }
    return 0;
}

int class_has_magic_skill(struct career *a1)
{
    int l_20;
    int l_1C;

    for (l_20 = 0; l_20 < 12; l_20++) {
        for (l_1C = 0; l_1C < 6; l_1C++) {
            if ((signed char)a1->skills[l_20] == D_0017BB39[l_1C]) return 1;
        }
    }
    return 0;
}

void cheat_raise_reputation(void)
{
    struct faction *l_1C;
    int l_18;

    l_1C = factions;
    hud_message_add((int)D_001710D6);
    for (l_18 = 0; l_18 < faction_count; l_18++, l_1C++) {
        if (l_1C->reputation <= 90) l_1C->reputation += 10;
    }
    D_00142314 = 0;
}

void cheat_raise_skills(void)
{
    int l_18;

    hud_message_add((int)D_001710EB);
    for (l_18 = 0; l_18 < 35; l_18++) {
        if (player_character->skills[l_18].value != 100) player_character->skills[l_18].value++;
    }
    D_00142315 = 0;
}

void starting_spells_give(struct record *a1)
{
    int l_24;
    int l_20;
    int l_1C;
    struct record *l_18;

    l_24 = 0;
    if (D_00199634 > 6 && D_00199634 != 18) return;
    if (D_00199634 == 18 && class_has_magic_skill(player_class) == 0) return;
    if (D_00199634 == 18) {
        l_1C = 1;
    } else {
        l_1C = D_00199634;
    }
    while (((int)(unsigned char)D_0017BB3F[(l_1C * 6) + l_24]) != 255) {
        l_18 = object_create_child(a1, 0, 89);
        l_18->type = 9;
        l_18->id = object_new_id(100);
        l_20 = 0;
        while ((signed char)spell_records[l_20].id != D_0017BB3F[(l_1C * 6) + l_24]) {
            l_20++;
        }
        mc_memcpy(&l_18->data.spell, &spell_records[l_20], 89, (int)D_00171044, 582, 4);
        l_24++;
    }
}

void func_00046114(struct record *a1)
{
    int l_18;

    if (a1->type != 32) return;
    if (a1->y < (-1300) || a1->y > (-1200)) return;
    l_18 = func_000C7FD9(a1->x, a1->z, D_00195AC4->x + 664, D_00195AC4->z + 2035);
    if (l_18 >= D_00199718) return;
    D_00199714 = a1;
    D_00199718 = l_18;
}

int func_000461A3(void)
{
    D_00199718 = 50000;
    D_00199714 = 0;
    object_foreach(D_00195AC4, (int)func_00046114);
    return (int)D_00199714;
}
