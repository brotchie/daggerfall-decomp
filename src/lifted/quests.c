/* quests.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"
#include "bitfield.h"
#include "clib.h"

extern char disk_last_file_size[];
extern char D_00174F47[];
extern char D_00174F57[];
extern char D_00174F5C[];
extern char D_00174F71[];
extern char D_00174F8F[];
extern unsigned char player_environment;
extern char material_to_hit[];
extern char D_00185128[];
extern signed char D_0018512E[];
extern signed char body_part_armor_slots[];
extern signed char text_buffer[];
extern char scratch_190be4[];
extern char arena2_path[];
extern char arena2_cd_path[];
extern struct record *nonworld_root;
extern struct record *quest_root;
extern struct building *current_building;
extern struct record *location_object;
extern struct record *found_object;
extern char D_00195B84[];
extern char D_00195B85[];
extern struct character *player_character;
extern int game_minutes;
extern char *scratch_buffer;
extern struct record *quest_tick_object;
extern iptr quest_potential_questor;
extern short D_00195F68;
extern char D_001961F5[];
extern signed char D_00196282;
extern struct quest *current_quest;
extern iptr quest_debug_data;
extern signed char D_001997AE;
extern char D_001A5BE4[];

extern struct faction *faction_find(short);
extern void *quest_section(struct quest *, int);
extern struct record *func_000310E1(struct record *, struct record *);
extern int quest_init_resources(struct quest *);
extern int quest_offer_prompt(struct quest *);
extern int quest_pick_file(unsigned char, unsigned char, unsigned char, unsigned char, unsigned char);
extern iptr disk_read_file(char *, iptr);
extern int rand_range(int, int);
extern struct building *object_building(struct record *);
extern struct record *object_create_child(struct record *, struct record *, int);
extern int object_find(struct record *, iptr (*)());
extern struct record *object_find_by_id(struct record *, iptr);
extern void faction_change_reputation(struct faction *, int);
extern void rumor_add_quest(struct quest *, int, int, int);
extern void quest_timers_start_all(void);
extern void msgbox_show_rsc(int, int);
extern void quest_end(struct quest *);
extern void fatal_error(char *);
extern void logbook_prune_quests(void);
extern void object_foreach(struct record *, void (*)());
extern void object_foreach_open(struct record *, void (*)());
iptr quest_start(char *);
iptr quest_match_person_cb(struct record *);
int quest_free_id(void);
int quest_is_active(char *);
int item_armor_value(struct item *);
int armor_value_for_part(struct record *, int);
void func_0004C588(struct record *);
void func_0004C759(void);
void quest_mark_giver_cb(struct record *);
void quest_pick_for_npc(struct record *);
void quest_count_givers_cb(struct record *);
#pragma aux mc_set_location parm routine [];

iptr quest_start(char *file_name)
{
    struct record *quest_object;
    struct quest *quest;
    iptr file_data;
    int handle;

    if (file_name[0] == 0) return 0;
    if (stricmp(file_name, D_00174F5C) == 0) return 0;
    if (quest_is_active(file_name) != 0) return 0;
    mc_set_location(172, D_00174F47);
    mc_sprintf((char *)text_buffer, D_00174F57, (iptr)arena2_path, file_name);
    handle = open((char *)text_buffer, 512);
    if (handle < 0) {
        mc_set_location(177, D_00174F47);
        mc_sprintf((char *)text_buffer, D_00174F57, (iptr)arena2_cd_path, file_name);
        handle = open((char *)text_buffer, 512);
        if (handle < 0) return 0;
    }
    close(handle);
    file_data = disk_read_file(file_name, 0);
    logbook_prune_quests();
    quest_object = object_create_child(quest_root, 0, *(int *)disk_last_file_size);
    quest_object->type = 14;
    quest_object->flags = 3;
    quest = &quest_object->data.quest;
    mc_memcpy(quest, (void *)file_data, (int)(short)*(short *)disk_last_file_size, D_00174F47, 193, 4);
    if (file_data != 0 && file_data != (-1751672937)) {
        mc_free((void *)file_data, D_00174F47, 194);
        file_data = -1751672937;
    }
    quest_object->quest_id = (quest->id = quest_free_id());
    func_000A14E8(quest->name, file_name, 8, D_00174F47, 197, 9);
    if (((int)(short)D_00195F68) == 240) {
        D_00195F68 = current_building->faction_id;
    }
    if (toupper((int)(unsigned char)quest->name[5]) == 89) {
        quest->faction_id = D_00195F68;
    } else {
        quest->faction_id = 0;
    }
    current_quest = quest;
    quest_tick_object = quest_object;
    if (quest_init_resources(quest) == 0) {
        quest_end(quest);
        msgbox_show_rsc(600, 1);
        return 0;
    }
    quest_timers_start_all();
    if (tolower((int)(unsigned char)quest->name[0]) != 115 && quest_offer_prompt(quest) == 0) {
        quest_end(quest);
        return 0;
    }
    logbook_prune_quests();
    func_0004C759();
    quest_debug_data = (iptr)current_quest;
    rumor_add_quest(current_quest, 1005, 0, 4);
    return (iptr)quest;
}

void quest_start_pending(void)
{
    if (*(signed char *)D_001961F5 == 0) return;
    quest_start(D_001961F5);
    *(signed char *)D_001961F5 = 0;
}

int quest_file_list_add(char *file_name, int file_count)
{
    int i;
    char *entry;

    entry = scratch_buffer;
    for (i = 0; i < file_count; i++) {
        if (stricmp(entry, file_name) == 0) return file_count;
        entry += strlen(entry) + 1;
    }
    mc_strncpy(entry, file_name, 4, D_00174F47, 383);
    return file_count + 1;
}

iptr quest_match_person_cb(struct record *object)
{
    if (object->type != 8) return 0;
    if (object->data.person.faction_id == *(int *)scratch_190be4) {
        found_object = object;
        return 1;
    }
    return 0;
}

void func_0004C588(struct record *object)
{
    struct record *twin;

    if (object->twin != 0) return;
    if (object->type == 41 && (((unsigned)object->id) >> 16) == 800) {
        *(int *)scratch_190be4 = object->data.building.faction_id;
        found_object = 0;
        object_find(location_object, quest_match_person_cb);
        if (found_object == 0) return;
        if (found_object->twin != 0) {
            mc_set_location(422, D_00174F47);
            mc_sprintf((char *)text_buffer, D_00174F71, (int)(unsigned short)(short)found_object->image);
            fatal_error(text_buffer);
        }
        object->id = found_object->id;
        mc_memcpy(&object->x, &found_object->x, 12, D_00174F47, 427, 4);
        found_object->quest_id = object->quest_id;
        object->twin = found_object;
        found_object->twin = object;
        found_object->data.person.flags |= 128;
        if (((int)(unsigned short)(object->flags & 2048)) != 0) object->flags |= 0x200;
        return;
    }
    if ((location_object->id >> 16) != (((unsigned)object->id) >> 16)) return;
    twin = object_find_by_id(location_object, object->id);
    if (twin == 0 || object->type != twin->type) {
        func_000310E1(object, twin);
        return;
    }
    twin->quest_id = object->quest_id;
    twin->twin = object;
    object->twin = twin;
    if (((int)(unsigned short)(object->flags & 2048)) == 0) return;
    twin->flags |= 0xA00;
}

void func_0004C759(void)
{
    D_00196282 = 0;
    object_foreach_open(nonworld_root, func_0004C588);
}

int quest_free_id(void)
{
    struct record *quest_object;
    struct quest *quest;
    char *id_used;
    int id;

    quest_object = quest_root->children;
    id_used = scratch_buffer;
    mc_memset(id_used, 0, 256, D_00174F47, 478, 4);
    while (quest_object != 0) {
        quest = &quest_object->data.quest;
        id_used[quest->id]++;
        quest_object = quest_object->next;
    }
    for (id = 1; id < 256; id++) {
        if (id_used[id] == 0) return id;
    }
    fatal_error(D_00174F8F);
    return 0;
}

void qaction_op46_hide_npc(struct quest *quest, struct qbn_op *op)
{
    struct record *npc;

    npc = op->args[1].object;
    npc->flags |= 0xA00;
    if (npc->twin == 0) return;
    npc->twin->flags |= 0xA00;
}

void func_0004C874(struct quest *quest, struct qbn_op *op)
{
}

void qaction_op48_restore_npc(struct quest *quest, struct qbn_op *op)
{
    struct record *npc;

    npc = op->args[1].object;
    npc->flags &= ~0xA00;
    if (npc->twin == 0) return;
    npc->twin->flags &= ~0xA00;
}

void func_0004C8CF(struct quest *quest, struct qbn_op *op)
{
}

void quest_mark_giver_cb(struct record *object)
{
    struct faction *faction;
    struct faction *top_faction;
    struct person *person;
    struct building *building;

    if (object->type != 8) return;
    building = object_building(object);
    if (building != 0 && building->type == 11) return;
    if (building != 0 && building->type == 14) return;
    if (((int)player_environment) != 3) {
        if (building == 0 || building->type >= 16 || building->type == 1) return;
    }
    person = &object->data.person;
    if (person->faction_id == 0) return;
    faction = faction_find((short)person->faction_id);
    top_faction = faction;
    while (top_faction->parent != 0) top_faction = top_faction->parent;
    if (top_faction->id == 108 || top_faction->id == 42 || top_faction->id == 40 || top_faction->id == 41) return;
    if (faction->type != 2 && faction->type != 12 && faction->type != 4) return;
    if (faction->type == 4) {
        if ((rand() % 100) > 50) return;
    } else {
        if ((rand() % 100) > 25) return;
    }
    person->flags |= 128;
}

void quest_mark_givers(void)
{
    int saved_seed;

    saved_seed = rand();
    if (((int)player_environment) == 3) {
        if ((((unsigned)location_object->id) >> 16) != 50027 && (((unsigned)location_object->id) >> 16) != 50029 && (((unsigned)location_object->id) >> 16) != 50033) {
            return;
        }
    }
    srand(((unsigned)game_minutes) / 1440);
    object_foreach(location_object, quest_mark_giver_cb);
    srand(saved_seed);
}

int npc_is_quest_giver(struct record *npc)
{
    return (int)(unsigned char)(npc->data.person.flags & 128);
}

void quest_pick_for_npc(struct record *npc)
{
    struct faction *faction;

    if (npc == 0) return;
    faction = faction_find((int)(short)npc->data.person.faction_id);
    if (faction->type == 4 && faction->id != 407) {
        quest_pick_file(82, 0, 48, 67, player_character->level);
        return;
    }
    if (((int)player_environment) == 3) return;
    quest_pick_file(65, 75, 48, 67, player_character->level);
}

void quest_count_givers_cb(struct record *object)
{
    if (object->type != 8 || *(int *)D_00195B84 == 0) return;
    if (((int)(unsigned char)(object->data.person.flags & 128)) == 0) return;
    if (((struct bf8_7_1 *)&D_00195B85)->f != 0) {
        (*(int *)D_00195B84)++;
    } else {
        (*(int *)D_00195B84)--;
    }
    if (*(int *)D_00195B84 != 0) return;
    found_object = object;
}

iptr quest_find_potential_questor(void)
{
    *(int *)D_00195B84 = 32768;
    object_foreach(location_object, quest_count_givers_cb);
    if (*(int *)D_00195B84 == 32768) return 0;
    *(int *)D_00195B84 = rand_range(0, (*(int *)D_00195B84 & 32767) - 1) + 1;
    found_object = 0;
    object_foreach(location_object, quest_count_givers_cb);
    quest_potential_questor = (iptr)found_object;
    return (iptr)found_object;
}

int func_0004CD10(struct record *npc)
{
    int saved_seed;

    saved_seed = rand();
    srand(npc->id);
    *(signed char *)D_001961F5 = 0;
    quest_pick_for_npc(npc);
    srand(saved_seed);
    if (*(signed char *)D_001961F5 != 0) {
        quest_potential_questor = (iptr)npc;
        *(signed char *)D_001961F5 = 0;
        return 1;
    }
    return 0;
}

int func_0004CD80(struct person *person)
{
    if (D_001997AE != 0) return 0;
    if (person->faction_id != 510) return 0;
    D_001997AE = 1;
    quest_start(D_001A5BE4);
    return 1;
}

void quest_reward_faction(struct quest *quest)
{
    struct faction *faction;

    if (quest->faction_id == 0) return;
    faction = faction_find(quest->faction_id);
    faction_change_reputation(faction, 5);
}

void quest_add_questor_rumor(struct quest *quest, int rewarded)
{
    struct qbn_person *qbn_person;
    int i;

    qbn_person = quest_section(quest, 3);
    for (i = 0; quest->section_counts[3] > i; i++, qbn_person++) {
        if (qbn_person->kind == 21) {
            rumor_add_quest(quest, ((rewarded != 0) ? 1008 : 1009), qbn_person->object->id, 2);
            return;
        }
    }
}

int quest_is_active(char *name)
{
    struct record *quest_object;
    struct quest *quest;

    quest_object = quest_root->children;
    while (quest_object != 0) {
        if (quest_object->type == 14) {
            quest = &quest_object->data.quest;
            if (strnicmp(quest->name, name, 8) == 0) return 1;
        }
        quest_object = quest_object->next;
    }
    return 0;
}

int item_armor_value(struct item *item)
{
    int bonus;

    bonus = 0;
    if (item->armor_type == 2) {
        return (((int)(short)*(short *)(material_to_hit + (item->material * 2))) + bonus) + 45;
    }
    return bonus + ((int)(short)*(short *)(D_00185128 + (item->armor_type * 2)));
}

int armor_value_for_part(struct record *object, int part)
{
    struct character *character;
    int armor;
    int shield_index;
    int i;
    int enchantment_index;
    int adjusted;
    struct record *shield;
    struct item *item;

    adjusted = 0;
    armor = 100;
    character = &object->data.character;
    if (character->equipped[(int)(unsigned char)body_part_armor_slots[part]] != 0) {
        armor -= item_armor_value(&character->equipped[(int)(unsigned char)body_part_armor_slots[part]]->data.item);
    }
    shield = character->equipped[EQUIP_LEFT_HAND];
    if (shield != 0) {
        item = &shield->data.item;
        if (item->group == 2) {
            shield_index = item->index - 7;
            if (shield_index >= 0 && shield_index <= 3) {
                for (i = 0; i < 4; i++) {
                    if (((int)(unsigned char)D_0018512E[(shield_index << 2) + i]) == part) {
                        armor -= (shield_index + 1) * 5;
                        break;
                    }
                }
            }
        }
    }
    for (i = 0; i < 27; i++) {
        if (character->equipped[i] != 0) {
            item = &character->equipped[i]->data.item;
            if (item->enchantments[0].type == (-1)) continue;
            enchantment_index = 0;
            do {
                if (item->enchantments[enchantment_index].type == (-1) || enchantment_index >= 10) break;
                if (item->enchantments[enchantment_index].type == 12) {
                    armor += -25;
                    adjusted = 1;
                } else if (item->enchantments[enchantment_index].type == 24) {
                    armor += 25;
                    adjusted = 1;
                }
                enchantment_index++;
            } while (adjusted == 0);
        }
        if (adjusted != 0) break;
    }
    if (armor > 100) return 100;
    if (armor < (-100)) return -100;
    return armor;
}

void character_update_armor_values(struct record *object)
{
    int part;
    struct character *character;

    character = &object->data.character;
    for (part = 0; part < 7; part++) {
        character->armor_values[part] = armor_value_for_part(object, part);
    }
}
