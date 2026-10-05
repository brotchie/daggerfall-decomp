/* kludge.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"
#include "bitfield.h"

extern signed char D_0012B508;
extern int xn_tex_archives[];
extern signed char key_down_minus;
extern signed char key_down_equals;
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
extern signed char regions[];
extern signed char text_buffer[];
extern char D_001903BA[];
extern signed char scratch_190ce4[];
extern signed char scratch_190ce5;
extern char arena2_path[];
extern char arena2_cd_path[];
extern signed char D_001940D4;
extern signed char D_001940D6;
extern signed char D_001940D9;
extern signed char D_001940DA;
extern struct record *inventory_containers[];
extern struct record *D_001959DC;
extern struct record *D_001959E0;
extern struct record *D_001959E4;
extern struct record *player_entity;
extern struct record *location_object;
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
extern signed char forced_material;
extern int faction_count;
extern struct faction *factions;
extern int loaded_location_door_count;
extern int loaded_location_doors;
extern int D_00199634;
extern struct record *D_00199714;
extern int D_00199718;
extern struct record *D_00199724;
extern signed char kludge_menu_active;
extern signed char cfg_gender;

extern struct faction *faction_find(short);
extern int list_popup_poll(void);
extern int sheet_open(short);
extern struct record *kludge_add_random_item(struct record *, int);
extern int item_add_random_to_container(struct record *, int);
extern int disk_read_file(char *, int);
extern struct membership *guild_find_membership_by_kind(unsigned char);
extern int hud_message_add(char *);
extern int rand_range(int, int);
extern struct record *object_free_single(struct record *);
extern struct record *object_delete(struct record *);
extern struct record *object_create_child(struct record *, struct record *, int);
extern struct record *object_reparent(struct record *, struct record *);
extern struct record *object_find_item(struct record *, short, short);
extern int object_new_id(int);
extern int func_0009DA1C(int, int);
extern int printf(int, ...);
extern int rand();
extern int srand();
extern int open(int, ...);
extern int close();
extern int mc_memset();
extern int read();
extern int mc_strncpy();
extern int mc_set_location(int, int);
extern int mc_sprintf(int, ...);
extern int mc_memcpy();
extern int func_000A148C(int, ...);
extern int fprintf(int, ...);
extern int xn_math_approx_dist2d();
extern int xn_spell_kludge_menu_dispatch();
extern void pickpocket_attempt(struct record *);
extern void rumor_show_local(void);
extern void player_refresh_paperdoll(void);
extern void text_draw(int, int, int);
extern void item_make_in_range(unsigned short, int, int, struct item *);
extern void item_make(int, int, struct item *);
extern void item_make_artifact(struct item *, int);
extern void guild_join_dark_brotherhood(void);
extern void monster_init(struct record *, int);
extern void list_popup_open(int);
extern void object_free_children(struct record *);
extern void object_foreach(struct record *, void (*)());
extern void inv_equip_item(struct record *);
extern void inv_store_item(struct record *);
extern void inv_create_wagon(void);
extern void inv_add_arrows(struct record *, int);
struct record *item_add_to_container(struct record *, int, int, int);
int class_has_magic_skill(struct career *);
void kludge_make_test_character(int);
void starting_spells_give(struct record *);
void kludge_find_door_cb(struct record *);
#pragma aux func_0009DA1C parm routine [];
#pragma aux mc_set_location parm routine [];

void kludge_print_build(int to_console)
{
    int fd;
    int build;

    mc_set_location(78, (int)D_00171044);
    mc_sprintf((int)text_buffer, (int)D_0017104D, (int)arena2_path);
    fd = open((int)text_buffer, 512);
    if (fd < 0) {
        mc_set_location(81, (int)D_00171044);
        mc_sprintf((int)text_buffer, (int)D_0017104D, (int)arena2_cd_path);
        fd = open((int)text_buffer, 512);
        if (fd < 0) return;
    }
    read(fd, (int)text_buffer, 80);
    fprintf((int)D_001903BA, (int)D_0017105A, (int)&build);
    if (to_console != 0) {
        func_0009DA1C(90, (int)D_00171044);
        printf((int)D_0017105D, build);
    } else {
        mc_set_location(92, (int)D_00171044);
        func_000A148C((int)D_00171062, build);
    }
    close(fd);
}

void kludge_find_creature_cb(struct record *object)
{
    if (object->type != 18) return;
    D_00199724 = object;
}

void kludge_menu_open(void)
{
    list_popup_open((int)D_0017BB01);
    kludge_menu_active = 1;
    game_mode = 200;
    D_00196272 = 1;
}

void kludge_menu_update(void)
{
    int row;

    if (kludge_menu_active == 0) return;
    if (((struct bf8_2_1 *)&D_001940D4)->f == 0 || (row = list_popup_poll()) <= (-1)) return;
    kludge_menu_active = 0;
    game_mode = 0;
    D_00196272 = 0;
    xn_spell_kludge_menu_dispatch(row);
}

void kludge_show_rumor(void)
{
    rumor_show_local();
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

void kludge_make_test_character_with_spells(int full)
{
    int unused1;
    struct record *object;
    int unused2;
    short i;

    kludge_make_test_character(full);
    *(int *)&i = 0;
    for (; ((int)(short)i) < spell_record_count; (*(int *)&i)++) {
        if (spell_records[(int)(short)i].name[0] == 0) continue;
        object = object_find_item(player_entity->children, 27, 0);
        object = object_create_child(object, 0, 89);
        object->type = 9;
        object->id = object_new_id(100);
        mc_memcpy(&object->data.spell, &spell_records[(short)i], 89, (int)D_00171044, 165, 4);
    }
}

void kludge_make_test_character(int full)
{
    struct record *object;
    int unused1;
    int unused2;
    int unused3;
    int unused4;
    int unused5;
    struct item *item;
    int i;
    int saved_seed;

    saved_seed = rand();
    srand(12345);
    D_001940D9 |= 8;
    if (cfg_gender != 0 && full != 0) player_character->flags = 1;
    if (((int)(unsigned short)(player_character->flags & 1)) != 0) {
        *(int *)clothing_gender_group = 12;
    } else {
        *(int *)clothing_gender_group = 6;
    }
    player_character->gold = 5000;
    player_character->level = 2;
    if (full != 0) {
        disk_read_file(D_00171077, (int)player_class);
        player_class->forbidden_equipment = 0;
        player_class->forbidden_materials = 0;
        mc_strncpy(player_character->name, (int)D_00171083, 32, (int)D_00171044, 197);
        mc_memcpy(player_character->attributes, (int)D_0017BAE2, 16, (int)D_00171044, 198, 16);
        mc_memcpy(player_character->base_attributes, player_character->attributes, 16, (int)D_00171044, 199, 16);
        player_character->min_metal_to_hit = 0;
        for (i = 0; i < 35; i++) {
            player_character->skills[i].value = rand_range(10, 60);
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
    for (i = 0; i < 35; i++) {
        player_character->skills[i].pad4 = player_character->skills[i].value;
    }
    *(int *)current_region_data = (int)regions;
    object = object_create_child(D_001959E0, 0, 107);
    object->image2 = 202;
    object->type = 2;
    item = &object->data.item;
    item_make_in_range((int)(unsigned short)*(short *)clothing_gender_group, 10, 11, item);
    object = object_create_child(D_001959E0, 0, 107);
    object->image2 = 203;
    object->type = 2;
    item = &object->data.item;
    item_make_in_range((int)(unsigned short)*(short *)clothing_gender_group, 0, 4, item);
    item_add_to_container(D_001959E0, *(int *)clothing_gender_group, 0, 1);
    item_add_to_container(D_001959E0, *(int *)clothing_gender_group, 17, 1);
    item_add_to_container(D_001959E0, *(int *)clothing_gender_group, 9, 0);
    item_add_to_container(D_001959E0, *(int *)clothing_gender_group, 10, 0);
    item_add_to_container(D_001959E0, 23, 1, 0);
    object = item_add_to_container(player_entity, 23, 0, 0);
    inv_create_wagon();
    item_add_random_to_container(inventory_containers[0], 2);
    item_add_random_to_container(inventory_containers[0], 2);
    item_add_random_to_container(inventory_containers[0], 2);
    item_add_random_to_container(inventory_containers[0], 2);
    item_add_to_container(inventory_containers[0], 3, 7, 0);
    forced_material = 1;
    item_add_to_container(inventory_containers[0], 3, 1, 0);
    item_add_to_container(inventory_containers[0], 3, 17, 0);
    item_add_to_container(inventory_containers[0], 2, 5, 0);
    item_add_random_to_container(D_001959E0, *(int *)clothing_gender_group);
    inv_add_arrows(player_entity, 250);
    kludge_add_random_item(D_001959DC, 4);
    kludge_add_random_item(D_001959DC, 4);
    kludge_add_random_item(D_001959E4, 20);
    kludge_add_random_item(D_001959E4, 20);
    kludge_add_random_item(D_001959E4, 17);
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
    object = object_find_item(player_entity->children, 27, 1);
    object = object_create_child(object, 0, 0);
    object->type = 20;
    object->flags = 3;
    object->image = 31;
    object = object_find_item(player_entity->children, 27, 2);
    item = &object->data.item;
    item->value = 10000;
    object = object_create_child(location_object, 0, 107);
    object->type = 2;
    object->flags |= 1;
    item_make(1, 1, &object->data.item);
    object->data.item.value = (int)(unsigned short)*(short *)D_00180B42;
    inv_store_item(object);
    object = object_create_child(object, 0, 109);
    object->type = 31;
    mc_memcpy(&object->data.potion_recipe, (int)potion_recipes, 109, (int)D_00171044, 289, 4);
    object = object_create_child(D_001959DC, 0, 107);
    object->type = 2;
    item = &object->data.item;
    item_make_artifact(item, 7);
    object = object_create_child(D_001959DC, 0, 107);
    object->type = 2;
    item = &object->data.item;
    item_make_artifact(item, 8);
    object = object_create_child(D_001959DC, 0, 107);
    object->type = 2;
    item = &object->data.item;
    item_make_artifact(item, 2);
    object = object_create_child(D_001959DC, 0, 107);
    object->type = 2;
    item = &object->data.item;
    item_make_artifact(item, 10);
    object = object_create_child(D_001959DC, 0, 107);
    object->type = 2;
    item = &object->data.item;
    item_make_artifact(item, (int)(unsigned char)cfg_artifact);
    player_refresh_paperdoll();
    D_001940D9 &= 247;
    srand(saved_seed);
}

void starting_equipment_give(void)
{
    int unused1;
    int unused2;
    struct record *weapon;

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
        forced_material = D_0017BABD[D_00199634 * 2] + 1;
        weapon = item_add_to_container(inventory_containers[0], 3, (int)(unsigned char)D_0017BABC[D_00199634 * 2], 0);
        weapon->data.item.color = weapon->data.item.material + 16;
        weapon->data.item.value = 15;
    } else if (((int)(unsigned char)D_0017BABC[D_00199634 * 2]) == 254) {
        weapon = item_add_to_container(inventory_containers[0], 3, 17, 0);
        forced_material = 1;
        weapon = item_add_to_container(inventory_containers[0], 3, 14, 0);
        inv_add_arrows(player_entity, 24);
    }
    item_add_to_container(D_001959DC, 27, 0, 0);
    starting_spells_give(object_find_item(player_entity->children, 27, 0));
    player_refresh_paperdoll();
    D_001940D9 &= 247;
}

struct record *item_add_to_container(struct record *container, int group, int item_index, int equip)
{
    struct record *object;
    struct record *bottle;
    struct item *item;

    object = object_create_child(container, 0, 107);
    object->type = 2;
    item = &object->data.item;
    item_make((int)(unsigned short)*(short *)&group, item_index, item);
    if ((item->item_flags & 8) != 0) {
        bottle = object_create_child(container, 0, 107);
        bottle->type = 2;
        object_reparent(bottle, object);
        item = &bottle->data.item;
        item_make(1, 1, item);
        if (equip != 0) inv_equip_item(bottle);
        return bottle;
    }
    if (equip != 0) inv_equip_item(object);
    return object;
}

void kludge_toggle_world_position(void)
{
    D_001940D6 ^= 1;
}

void kludge_toggle_second_compass(void)
{
    D_001940D6 ^= 8;
}

void kludge_pick_pockets(void)
{
    struct record *creature;
    int monster_id;

    scratch_190ce4[0] = 1;
    player_character->skills[15].value = 80;
    creature = object_create_child(location_object, 0, 659);
    creature->type = 18;
    monster_id = rand() % 20;
    scratch_190ce4[0] = 2;
    scratch_190ce5 = *(signed char *)&monster_id;
    monster_init(creature, monster_id);
    scratch_190ce4[0] = 3;
    mc_set_location(448, (int)D_00171044);
    mc_sprintf((int)text_buffer, (int)D_00171096, *(int *)(monster_names + (monster_id << 2)));
    hud_message_add(text_buffer);
    scratch_190ce4[0] = 4;
    pickpocket_attempt(creature);
    scratch_190ce4[0] = 5;
    object_free_children(creature);
    scratch_190ce4[0] = 6;
    object_free_single(creature);
    scratch_190ce4[0] = 7;
}

void kludge_toggle_quest_debug(void)
{
    D_001940DA ^= 2;
    if (((struct bf8_1_1 *)&D_001940DA)->f == 0) return;
    hud_message_add(D_001710A7);
}

void kludge_remove_all_items(void)
{
    struct record *object;

    mc_memset(player_character->equipped, 0, 108, (int)D_00171044, 469, 108);
    object = player_entity->children;
    while (object != 0) object = object_delete(object);
}

void kludge_show_memory(void)
{
    int i;
    int row;
    int total;
    int block;

    D_0012B508 = 146;
    i = 0;
    row = i;
    total = row;
    for (; i < 512; i++) {
        if (xn_tex_archives[i] == 0) continue;
        block = xn_tex_archives[i] - 22;
        mc_set_location(487, (int)D_00171044);
        mc_sprintf((int)text_buffer, (int)D_001710BA, *(int *)((char *)block + 8), xn_tex_archives[i] + 2);
        text_draw((int)text_buffer, (row / 25) * 160, (row % 25) << 3);
        row++;
        total += *(int *)((char *)block + 8);
    }
    D_0012B508 = 245;
    mc_set_location(494, (int)D_00171044);
    mc_sprintf((int)text_buffer, (int)D_001710C3, total);
    text_draw((int)text_buffer, (row / 25) * 160, (row % 25) << 3);
}

int location_find_door(int id)
{
    short door;
    int i;
    short id_low;

    *(int *)&id_low = id;
    *(int *)&door = loaded_location_doors;
    for (i = 0; i < loaded_location_door_count; i++, (*(char (**)[6])&(*(int *)&door))++) {
        if ((short)*(int *)&id_low == *(short *)(*(char **)&door + 4)) return *(int *)&door;
    }
    return 0;
}

int class_has_magic_skill(struct career *career)
{
    int i;
    int j;

    for (i = 0; i < 12; i++) {
        for (j = 0; j < 6; j++) {
            if ((signed char)career->skills[i] == D_0017BB39[j]) return 1;
        }
    }
    return 0;
}

void cheat_raise_reputation(void)
{
    struct faction *faction;
    int i;

    faction = factions;
    hud_message_add(D_001710D6);
    for (i = 0; i < faction_count; i++, faction++) {
        if (faction->reputation <= 90) faction->reputation += 10;
    }
    key_down_minus = 0;
}

void cheat_raise_skills(void)
{
    int i;

    hud_message_add(D_001710EB);
    for (i = 0; i < 35; i++) {
        if (player_character->skills[i].value != 100) player_character->skills[i].value++;
    }
    key_down_equals = 0;
}

void starting_spells_give(struct record *spellbook)
{
    int i;
    int record_index;
    int class_id;
    struct record *spell;

    i = 0;
    if (D_00199634 > 6 && D_00199634 != 18) return;
    if (D_00199634 == 18 && class_has_magic_skill(player_class) == 0) return;
    if (D_00199634 == 18) {
        class_id = 1;
    } else {
        class_id = D_00199634;
    }
    while (((int)(unsigned char)D_0017BB3F[(class_id * 6) + i]) != 255) {
        spell = object_create_child(spellbook, 0, 89);
        spell->type = 9;
        spell->id = object_new_id(100);
        record_index = 0;
        while ((signed char)spell_records[record_index].id != D_0017BB3F[(class_id * 6) + i]) {
            record_index++;
        }
        mc_memcpy(&spell->data.spell, &spell_records[record_index], 89, (int)D_00171044, 582, 4);
        i++;
    }
}

void kludge_find_door_cb(struct record *object)
{
    int distance;

    if (object->type != 32) return;
    if (object->y < (-1300) || object->y > (-1200)) return;
    distance = xn_math_approx_dist2d(object->x, object->z, location_object->x + 664, location_object->z + 2035);
    if (distance >= D_00199718) return;
    D_00199714 = object;
    D_00199718 = distance;
}

struct record *kludge_find_door(void)
{
    D_00199718 = 50000;
    D_00199714 = 0;
    object_foreach(location_object, kludge_find_door_cb);
    return D_00199714;
}
