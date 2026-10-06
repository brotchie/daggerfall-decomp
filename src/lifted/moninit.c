/* moninit.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern struct monster_template monster_table[];
extern char D_00176844[];
extern char monster_spells_imp[];
extern char monster_spells_ghost[];
extern char monster_spells_orc_shaman[];
extern char monster_spells_wraith[];
extern char monster_spells_frost_daedra[];
extern char monster_spells_fire_daedra[];
extern char monster_spells_daedroth[];
extern char monster_spells_vampire[];
extern char monster_spells_seducer[];
extern char monster_spells_vampire_ancient[];
extern char monster_spells_daedra_lord[];
extern char monster_spells_lich[];
extern char monster_spells_ancient_lich[];
extern iptr monster_class_spell_lists[];
extern signed char monster_class_map_chance[];
extern signed char wabbajack_creatures[];
extern signed char monster_map_chance[];
extern char D_00190704[];
extern struct record *nonworld_root;
extern struct record *location_object;
extern struct spell *spell_records;
extern struct character *player_character;
extern int save_file_handle;

extern struct record *monster_make_item(struct record *, int, int, int, int, int);
extern int rand_range(int, int);
extern struct record *object_create_child(struct record *, struct record *, int);
extern int object_new_id(int);
extern int rand();
extern int mc_free();
extern int mc_memset();
extern int read();
extern int mc_memcpy();
extern void character_update_armor_values(struct record *);
extern void item_make(int, int, struct item *);
extern void item_damage(struct record *, int);
extern void poison_init_record(struct disease *, int);
extern void monster_init(struct record *, int);
extern void monster_reload_anim_cb(struct record *);
extern void object_foreach(struct record *, void (*)());
void monster_give_spells(struct record *, char *);
void monster_give_equipment(struct record *, struct character *, int);
void monster_poison_weapon(struct record *);

void monster_reload_anims(void)
{
    int slot;

    for (slot = 0; slot < 128; slot++) {
        if (*(int *)(D_00190704 + (slot << 2)) != 0) {
            if (*(int *)(D_00190704 + (slot << 2)) != 0 && *(int *)(D_00190704 + (slot << 2)) != (-1751672937)) {
                mc_free(*(int *)(D_00190704 + (slot << 2)), (iptr)D_00176844, 211);
                *(int *)(D_00190704 + (slot << 2)) = -1751672937;
            }
        }
    }
    object_foreach(location_object, monster_reload_anim_cb);
    object_foreach(nonworld_root, monster_reload_anim_cb);
}

int monster_roll_d8_health(int dice_count, int bonus)
{
    int i;
    int total;

    i = 0;
    total = i;
    for (; i < dice_count; i++) {
        total += rand_range(1, 8);
    }
    return total + bonus;
}

int monster_roll_class_health(int hp_per_level, int bonus, int level)
{
    int i;
    int total;

    i = 0;
    total = i;
    for (; i < level; i++) {
        total += rand_range(1, hp_per_level);
    }
    return total + bonus;
}

void monster_init_gear(struct record *monster)
{
    struct character *monster_char;
    int list_index;

    monster_char = &monster->data.character;
    if (monster_char->race >= 43) {
        monster_give_equipment(monster, monster_char, rand() & 1);
        if (((int)(unsigned short)(monster_table[monster_char->race].flags & 2)) != 0) {
            list_index = monster_char->level / 3;
            if (list_index > 6) list_index = 6;
            monster_give_spells(monster, (char *)monster_class_spell_lists[list_index]);
        }
        return;
    }
    switch (monster_char->race) {
        return;
    case 1:
        monster_give_spells(monster, monster_spells_imp);
        return;
    case 7:
        monster_give_equipment(monster, monster_char, 0);
        return;
    case 8:
        monster_give_equipment(monster, monster_char, 1);
        return;
    case 12:
        monster_give_equipment(monster, monster_char, 1);
        return;
    case 18:
        monster_give_spells(monster, monster_spells_ghost);
        return;
    case 21:
        monster_give_equipment(monster, monster_char, 0);
        monster_give_spells(monster, monster_spells_orc_shaman);
        return;
    case 23:
        monster_give_spells(monster, monster_spells_wraith);
        return;
    case 24:
        monster_give_equipment(monster, monster_char, 2);
        return;
    case 25:
        monster_give_spells(monster, monster_spells_frost_daedra);
        return;
    case 26:
        monster_give_spells(monster, monster_spells_fire_daedra);
        return;
    case 27:
        monster_give_spells(monster, monster_spells_daedroth);
        return;
    case 28:
        monster_give_spells(monster, monster_spells_vampire);
        return;
    case 29:
        monster_give_spells(monster, monster_spells_seducer);
        return;
    case 30:
        monster_give_spells(monster, monster_spells_vampire_ancient);
        return;
    case 31:
        monster_give_spells(monster, monster_spells_daedra_lord);
        return;
    case 32:
        monster_give_spells(monster, monster_spells_lich);
        return;
    case 33:
        monster_give_spells(monster, monster_spells_ancient_lich);
    default:;
    }
}

void monster_give_spells(struct record *monster, char *spell_ids)
{
    int i;
    int spell_index;
    struct record *spellbook;
    struct record *spell;
    struct character *monster_char;

    i = 0;
    monster_char = &monster->data.character;
    monster_char->magicka = (monster_char->max_magicka = (((unsigned short)monster_char->level) * 10) + 100);
    monster_char->skills[22].value = 80;
    monster_char->skills[23].value = 80;
    monster_char->skills[24].value = 80;
    monster_char->skills[25].value = 80;
    monster_char->skills[26].value = 80;
    monster_char->skills[27].value = 80;
    spellbook = object_create_child(monster, 0, 0);
    spellbook->type = 22;
    spellbook->flags = 3;
    spellbook->id = object_new_id(((unsigned)monster->id) >> 16);
    while (((int)(unsigned char)*(signed char *)(spell_ids + i)) != 255) {
        spell_index = 0;
        while ((signed char)spell_records[spell_index].id != *(signed char *)(spell_ids + i)) spell_index++;
        spell = object_create_child(spellbook, 0, 89);
        spell->type = 9;
        spell->flags = 1;
        spell->id = object_new_id(((unsigned)monster->id) >> 16);
        mc_memcpy(&spell->data.spell, &spell_records[spell_index], 89, (iptr)D_00176844, 370, 4);
        i++;
    }
}

void monster_give_equipment(struct record *monster, struct character *monster_char, int tier)
{
    int poison_chance;
    int i;

    mc_memset(monster_char->equipped, 0, 108, (iptr)D_00176844, 400, 108);
    switch ((unsigned)tier) {
    case 0:
        monster_char->equipped[19] = monster_make_item(monster, 3, 5, 7, -1, 100);
        monster_char->equipped[21] = monster_make_item(monster, 2, 7, 8, -1, 50);
        if (monster_char->equipped[21] == 0) monster_char->equipped[21] = monster_make_item(monster, 3, 0, 3, 2, 50);
        monster_char->equipped[12] = monster_make_item(monster, 2, 5, 5, -1, 50);
        monster_char->equipped[13] = monster_make_item(monster, 2, 4, 4, -1, 50);
        monster_char->equipped[15] = monster_make_item(monster, 2, 3, 3, -1, 50);
        monster_char->equipped[18] = monster_make_item(monster, 2, 0, 0, -1, 50);
        monster_char->equipped[23] = monster_make_item(monster, 2, 2, 2, -1, 50);
        monster_char->equipped[26] = monster_make_item(monster, 2, 6, 6, -1, 50);
        character_update_armor_values(monster);
        break;
    case 1:
        monster_char->equipped[19] = monster_make_item(monster, 3, 9, 14, -1, 100);
        monster_char->equipped[12] = monster_make_item(monster, 2, 5, 5, -1, 75);
        monster_char->equipped[13] = monster_make_item(monster, 2, 4, 4, -1, 75);
        monster_char->equipped[15] = monster_make_item(monster, 2, 3, 3, -1, 75);
        monster_char->equipped[18] = monster_make_item(monster, 2, 0, 0, -1, 75);
        monster_char->equipped[23] = monster_make_item(monster, 2, 2, 2, -1, 75);
        monster_char->equipped[26] = monster_make_item(monster, 2, 6, 6, -1, 75);
        character_update_armor_values(monster);
        break;
    case 2:
        monster_char->equipped[19] = monster_make_item(monster, 3, 9, 14, -1, 100);
        monster_char->equipped[12] = monster_make_item(monster, 2, 5, 5, -1, 90);
        monster_char->equipped[13] = monster_make_item(monster, 2, 4, 4, -1, 90);
        monster_char->equipped[15] = monster_make_item(monster, 2, 3, 3, -1, 90);
        monster_char->equipped[18] = monster_make_item(monster, 2, 0, 0, -1, 90);
        monster_char->equipped[23] = monster_make_item(monster, 2, 2, 2, -1, 90);
        monster_char->equipped[26] = monster_make_item(monster, 2, 6, 6, -1, 90);
        character_update_armor_values(monster);
    }
    for (i = 0; i < 7; i++) {
        if (monster_char->armor_values[i] > 50) monster_char->armor_values[i] = 60;
    }
    if (player_character->level < 2) return;
    if (monster_char->mobile_id >= 128 && monster_char->equipped[19] != 0) {
        if (monster_char->mobile_id == 139) {
            poison_chance = 60;
        } else {
            poison_chance = 5;
        }
        if (rand_range(1, 100) < poison_chance) monster_poison_weapon(monster_char->equipped[19]);
        return;
    }
    switch (monster_char->mobile_id) {
    case 7:
    case 8:
    case 12:
        if (rand_range(1, 100) >= 5) return;
        monster_poison_weapon(monster_char->equipped[19]);
    default:;
    }
}

void monster_poison_weapon(struct record *object)
{
    object = object_create_child(object, 0, 47);
    object->id = object_new_id(((unsigned)object->parent->id) >> 16);
    poison_init_record(&object->data.disease, (int)(iptr)&*(signed char *)((char *)(iptr)rand_range(0, 7) + 128));
}

void monster_wabbajack(struct record *item, struct record *target)
{
    int health_change;
    struct character *target_char;

    target_char = &target->data.character;
    if (((int)(unsigned short)(target_char->flags & 4096)) != 0) return;
    health_change = target_char->max_health;
    monster_init(target, (int)(unsigned char)wabbajack_creatures[rand_range(0, 16)]);
    target_char->flags |= 0x1000;
    health_change = target_char->max_health - health_change;
    if (health_change >= 0) return;
    item_damage(item, health_change);
}

void monster_maybe_give_map(struct record *monster, int mobile_id)
{
    struct record *item;
    struct item *item_data;

    if (mobile_id < 128) {
        if (rand_range(1, 100) > ((int)(unsigned char)monster_map_chance[mobile_id])) return;
    } else {
        if (rand_range(1, 100) > ((int)(unsigned char)monster_class_map_chance[mobile_id])) return;
    }
    item = object_create_child(monster, 0, 107);
    item->type = 2;
    item->id = object_new_id(((unsigned)monster->id) >> 16);
    item_data = &item->data.item;
    item_make(27, 8, item_data);
}

int savetree_read_chunk(struct record *buffer)
{
    int size;

    read(save_file_handle, (iptr)&size, 4);
    read(save_file_handle, buffer, size);
    return size;
}
