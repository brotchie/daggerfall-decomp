/* qinit.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern struct region regions[];
extern char D_00170A64[];
extern char D_00178A10[];
extern char item_group_templates[];
extern signed char D_001940D5;
extern char D_00195984[];
extern struct record *nonworld_root;
extern struct record *quest_root;
extern struct record *camera_object;
extern struct record *player_object;
extern struct record *location_object;
extern struct character *player_character;
extern int game_minutes;
extern struct record *quest_tick_object;
extern int qbn_opcode_arg_counts;
extern signed char current_region;
extern signed char D_00196299;
extern signed char D_001962A3;
extern int faction_count;
extern struct faction *factions;
extern signed char D_001970DC;
extern struct quest *current_quest;
extern struct quest *quest_tick_data;
extern short qbn_record_sizes[];
extern struct membership *guild_membership;

extern struct faction *faction_find(short);
extern int faction_player_related(struct faction *);
extern int quest_section(struct quest *, int);
extern int quest_record(struct quest *, int, int);
extern struct record *func_000310E1(struct record *, struct record *);
extern int quest_init_person(struct qbn_person *);
extern int quest_init_place(struct qbn_place *);
extern int spawn_find_point(struct record *, int, int);
extern int rand_range(int, int);
extern struct record *object_free_single(struct record *);
extern struct record *object_delete(struct record *);
extern struct record *object_create_child(struct record *, struct record *, int);
extern struct record *object_reparent(struct record *, struct record *);
extern struct record *object_find_by_id(struct record *, int);
extern int object_new_id(int);
extern int rand();
extern int mc_strncpy();
extern int mc_memcpy();
extern void item_make_random(unsigned short, struct item *);
extern void item_make(int, int, struct item *);
extern void monster_init(struct record *, int);
extern void monster_init_gear(struct record *);
extern void map_goto_location(int, int, int, int);
struct record *quest_init_item(struct qbn_item *);
struct record *quest_init_foe(struct qbn_foe *);
struct record *quest_record_object(int, char *);
int quest_place_object(struct record *, struct qbn_place *);

struct record *quest_init_item(struct qbn_item *qbn_item)
{
    char *location_data;
    struct record *item;
    struct item *item_data;
    int gold_amount;
    int level;
    int power;
    struct faction *faction;

    location_data = RECORD_DATA(location_object);
    faction = 0;
    if ((qbn_item->flags & 2) != 0) {
        if (qbn_item->index == (-1)) {
            if (current_quest->faction_id != 0) {
                faction = faction_find(current_quest->faction_id);
                if (faction != 0 && faction_player_related(faction) != 0) {
                    level = guild_membership->rank + 1;
                } else {
                    level = (player_character->level / 2) + 1;
                }
            } else {
                level = (player_character->level / 2) + 1;
            }
            if (level > 10) level = 10;
            if (faction != 0) {
                power = faction->power;
            } else {
                power = 50;
            }
            gold_amount = ((power + 50) * ((((int)&*(signed char *)((char *)(regions[(unsigned char)current_region].price_adjustment / 2) + 500)) * rand_range(level * 150, level * 200)) / 1000)) / 100;
        } else {
            gold_amount = rand_range(qbn_item->index, qbn_item->group);
        }
        if (gold_amount < 1) gold_amount = 1;
        item = object_create_child(nonworld_root, 0, 107);
        item->type = 2;
        item->image2 = *(short *)D_00178A10;
        item->flags = 0;
        item->quest_id = (signed char)current_quest->id;
        item->id = object_new_id(700);
        qbn_item->object = item;
        item_data = &item->data.item;
        item_make(28, 0, item_data);
        item_data->value = gold_amount;
        item->image = item_data->dropped_image;
    } else {
        if (qbn_item->group < 0) {
            do {
                qbn_item->group = rand() % 28;
            } while (*(int *)(item_group_templates + (qbn_item->group << 2)) == 0);
        }
        item = object_create_child(nonworld_root, 0, 107);
        item->type = 2;
        item->image2 = *(short *)D_00178A10;
        item->flags = 0;
        item->quest_id = (signed char)current_quest->id;
        item->id = object_new_id(700);
        qbn_item->object = item;
        item_data = &item->data.item;
        item_data->message = 0;
        if (qbn_item->index >= 0) {
            item_make((int)(unsigned short)qbn_item->group, qbn_item->index, item_data);
        } else {
            item_make_random((int)(unsigned short)qbn_item->group, item_data);
            qbn_item->index = item_data->index;
        }
        item->image = item_data->dropped_image;
        if (item_data->group == 9 && item_data->index == 5 && qbn_item->messages[1] != 0) {
            item_data->message = qbn_item->messages[1];
            mc_strncpy(&item_data->name[10], (int)(signed char *)&current_quest->name[0], 4, (int)D_00170A64, 572);
        }
    }
    return item;
}

struct record *quest_init_foe(struct qbn_foe *foe)
{
    struct record *foe_object;
    char *location_data;
    struct character *character;

    foe_object = object_create_child(nonworld_root, 0, 659);
    location_data = RECORD_DATA(location_object);
    foe_object->type = 18;
    foe_object->flags |= 1;
    foe_object->repair_due = rand();
    foe_object->id = object_new_id(700);
    foe->object = foe_object;
    foe_object->image2 = (*(int *)D_00178A10)++;
    foe_object->quest_id = (signed char)current_quest->id;
    monster_init(foe_object, foe->type);
    character = &foe_object->data.character;
    character->team = 1;
    return foe_object;
}

int quest_init_resources(struct quest *quest)
{
    struct qbn_place *qbn_place;
    struct qbn_person *qbn_person;
    struct qbn_item *qbn_item;
    struct qbn_foe *foe;
    struct qbn_op *op;
    struct qbn_arg *arg;
    char *record;
    struct qbn_text_var *text_var;
    int i;
    int pass_count;
    int record_index;
    short arg_index;
    int section;
    short unused;

    pass_count = 0;
    current_quest = quest;
    qbn_person = (struct qbn_person *)((char *)quest + quest->section_offsets[3]);
    D_001970DC = 0;
    for (i = 0; quest->section_counts[3] > i; i++, qbn_person++) {
        qbn_person->object = 0;
        if (quest_init_person(qbn_person) == 0) return 0;
    }
    while (D_001970DC != 0) {
        qbn_person = (struct qbn_person *)((char *)quest + quest->section_offsets[3]);
        D_001970DC = 0;
        for (i = 0; quest->section_counts[3] > i; i++, qbn_person++) {
            if (qbn_person->object != 0) continue;
            if (quest_init_person(qbn_person) == 0) return 0;
        }
        if (pass_count++ > 20) return 0;
    }
    qbn_place = (struct qbn_place *)((char *)quest + quest->section_offsets[4]);
    for (i = 0; quest->section_counts[4] > i; i++, qbn_place++) {
        qbn_place->object = 0;
        if (quest_init_place(qbn_place) == 0) return 0;
    }
    qbn_item = (struct qbn_item *)((char *)quest + quest->section_offsets[0]);
    for (i = 0; quest->section_counts[0] > i; i++, qbn_item++) {
        if ((qbn_item->flags & 2) == 0 && qbn_item->group == 100) {
            mc_memcpy(qbn_item, *(int *)(D_00195984 + (qbn_item->index << 2)), 19, (int)D_00170A64, 660, 4);
        } else {
            qbn_item->object = 0;
            if (quest_init_item(qbn_item) == 0) return 0;
        }
    }
    foe = (struct qbn_foe *)((char *)quest + quest->section_offsets[7]);
    for (i = 0; quest->section_counts[7] > i; i++, foe++) {
        foe->object = 0;
        if (quest_init_foe(foe) == 0) return 0;
    }
    op = (struct qbn_op *)((char *)quest + quest->section_offsets[8]);
    for (i = 0; quest->section_counts[8] > i; i++, op++) {
        arg = op->args;
        op->last_minutes = game_minutes;
        op->arg_count = ((int)(unsigned char)*(signed char *)((char *)(int)(*(char **)&qbn_opcode_arg_counts + op->opcode))) - 48;
        *(int *)&arg_index = 0;
        for (; op->arg_count > *(int *)&arg_index; (*(int *)&arg_index)++, arg++) {
            if ((int)arg->record != 305419896) {
                if (arg->value != (-1) && arg->value != (-2)) {
                    record_index = (int)arg->record & 255;
                    section = (int)arg->record >> 8;
                    record = (char *)quest + quest->section_offsets[section];
                    record += ((int)(short)qbn_record_sizes[section]) * record_index;
                    arg->record = record;
                    arg->object = quest_record_object(section, record);
                } else {
                    arg->record = 0;
                    arg->object = 0;
                }
            } else {
                arg->record = 0;
                arg->object = 0;
            }
        }
    }
    if (quest->text_offset != 0) {
        text_var = (struct qbn_text_var *)((int)quest + quest->text_offset);
        while (text_var->name[0] != 0) {
            text_var->record = (char *)quest_record(quest, (int)(short)((unsigned short)text_var->section), text_var->index);
            text_var++;
        }
    }
    return 1;
}

struct record *quest_record_object(int section, char *record)
{
    switch ((unsigned)section) {
    case 3:
        return ((struct qbn_person *)record)->object;
    case 0:
        return ((struct qbn_item *)record)->object;
    case 4:
        return ((struct qbn_place *)record)->object;
    case 7:
        return ((struct qbn_foe *)record)->object;
    }
    return 0;
}

void qaction_place_foe(struct qbn_op *op, struct qbn_place *place)
{
    struct record *foe_object;

    foe_object = op->args[1].object;
    if (quest_place_object(foe_object, place) == 0) return;
    if (place == 0) D_00196299 = 1;
    monster_init_gear(foe_object);
}

int func_000337AD(struct location_door *door, struct qbn_place *place, struct building *building)
{
    if (place->p2 > (-1)) {
        if (place->p2 >= 17 && place->p2 <= 20 && building->type >= 17 && building->type <= 20) {
            if (place->p3 == (-1)) return (int)(unsigned short)(door->flags & 20480);
            if (place->p3 != 1) {
                return (((((int)(unsigned short)(door->flags & 16384)) != 0) && (((int)(unsigned short)(door->flags & 4096)) == 0)) ? 1 : 0);
            }
            return (int)(unsigned short)(door->flags & 4096);
        }
    }
    if (place->p2 > (-1)) {
        if (building->type == 11 && (short)(building->type) == place->p2 && building->faction_id == 40) {
        } else {
            if (place->p2 == 11) return 0;
            if ((short)(building->type) != place->p2) return 0;
        }
    }
    if (place->p3 == (-1)) return (int)(unsigned short)(door->flags & 20480);
    if (place->p3 != 1) {
        return (((((int)(unsigned short)(door->flags & 16384)) != 0) && (((int)(unsigned short)(door->flags & 4096)) == 0)) ? 1 : 0);
    }
    return (int)(unsigned short)(door->flags & 4096);
}

int quest_place_object(struct record *object, struct qbn_place *place)
{
    struct record *site;
    short saved_faction_id;

    if (place == 0) {
        if (spawn_find_point(object, 512, 1024) != 0) {
            site = object_create_child(player_object->parent, 0, 0);
            site->x = object->x;
            site->y = object->y;
            site->z = object->z;
            func_000310E1(object, site);
            object_free_single(site);
            return object->id;
        }
        return 0;
    }
    if (object->twin != 0) object_delete(object->twin);
    site = object_find_by_id(nonworld_root, place->object->id);
    if (site == 0) return 0;
    object_reparent(site, object);
    object->id = object_new_id(((unsigned)site->id) >> 16);
    site = object->children;
    while (site != 0) {
        site->id = object_new_id(((unsigned)site->parent->id) >> 16);
        site = site->next;
    }
    if (object->type != 2 && object->type != 18) {
        saved_faction_id = object->data.building.faction_id;
        mc_memcpy(&object->data, &place->object->data, 26, (int)D_00170A64, 924, 4);
        object->data.building.faction_id = *(int *)&saved_faction_id;
    }
    if ((((unsigned)object->id) >> 16) == (((unsigned)location_object->id) >> 16)) object = func_000310E1(object, 0);
    return object->id;
}

void qaction_place_item(struct quest *quest, struct qbn_op *op)
{
    struct record *item;

    item = op->args[1].object;
    if (item->twin != 0) {
        object_delete(item->twin);
        item->twin = 0;
    }
    quest_place_object(item, (struct qbn_place *)op->args[2].record);
}

void qaction_place_npc(struct quest *quest, struct qbn_op *op)
{
    int unused1;
    struct record *person;
    int unused2;
    int unused3;

    if (op->args[1].value == (-1)) {
        person = op->args[2].object;
        map_goto_location((int)(unsigned char)current_region, 3, person->image, person->image2);
        person = op->args[2].object->twin;
        if (person != 0) {
            player_object->x = person->x;
            player_object->y = person->y;
            player_object->z = person->z;
            player_object->yaw = camera_object->yaw;
            D_001940D5 |= 2;
        }
        return;
    }
    person = op->args[1].object;
    if (person->type == 65) return;
    if (person->twin != 0) object_delete(person->twin);
    person->twin = 0;
    quest_place_object(person, (struct qbn_place *)op->args[2].record);
}

void qaction_give_item_to_foe(struct quest *quest, struct qbn_op *op)
{
    struct record *item;
    struct record *item_twin;
    struct record *foe_object;

    item = op->args[1].object;
    if (item->twin != 0) {
        object_delete(item->twin);
        item->twin = 0;
    }
    foe_object = op->args[2].object->twin;
    if (foe_object != 0) {
        item_twin = object_create_child(foe_object, 0, 107);
        item_twin->type = 2;
        item_twin->id = object_new_id(((unsigned)location_object->id) >> 16);
        item_twin->image = item->image;
        item_twin->quest_id = (signed char)quest->id;
        item_twin->twin = item;
        item->twin = item_twin;
        mc_memcpy(&item_twin->data, &item->data, 107, (int)D_00170A64, 1008, 4);
    }
    foe_object = op->args[2].object;
    item->id = object_new_id(((unsigned)foe_object->id) >> 16);
    object_reparent(foe_object, item);
}

struct record *quest_find_site_for_building(struct building *building)
{
    struct record *quest_object;
    struct record *next;
    struct record *site;
    struct qbn_place *qbn_place;
    struct qbn_person *qbn_person;
    struct building *person_building;
    int i;

    quest_object = quest_root->children;
    while (quest_object != 0) {
        next = quest_object->next;
        if (quest_object->type == 14) {
            quest_tick_object = quest_object;
            quest_tick_data = (struct quest *)((int)&quest_object->data.quest);
            qbn_place = (struct qbn_place *)quest_section(quest_tick_data, 4);
            for (i = 0; quest_tick_data->section_counts[4] > i; i++, qbn_place++) {
                site = qbn_place->object;
                if ((qbn_place->flags & 64) != 0) {
                    D_001962A3 = 1;
                } else {
                    D_001962A3 = 0;
                }
                if (site != 0 && building->id == site->data.building.id) return site;
            }
            qbn_person = (struct qbn_person *)quest_section(quest_tick_data, 3);
            for (i = 0; quest_tick_data->section_counts[3] > i; i++, qbn_person++) {
                person_building = &qbn_person->object->data.building;
                if (((int)(short)(qbn_person->flags & 16384)) != 0) {
                    D_001962A3 = 1;
                } else {
                    D_001962A3 = 0;
                }
                if (building->id == person_building->id) return qbn_person->object;
            }
        }
        quest_object = next;
    }
    return 0;
}

struct faction *pick_random_of_three(struct faction **choices)
{
    int i;
    int choice_count;

    choice_count = 0;
    for (i = 0; i < 3; i++) {
        if (choices[i] != 0) choice_count++;
    }
    if (choice_count == 0) return 0;
    return choices[rand_range(0, choice_count - 1)];
}

int faction_random_hostile_id(void)
{
    int i;
    int hostile_count;

    i = 0;
    hostile_count = i;
    for (; i < faction_count; i++) {
        if (factions[i].reputation < 0) hostile_count++;
    }
    if (hostile_count == 0) return 0;
    hostile_count = rand_range(0, hostile_count - 1);
    for (i = 0; i < faction_count; i++) {
        if (factions[i].reputation >= 0) continue;
        if (hostile_count == 0) return factions[i].id;
        hostile_count--;
    }
    return 0;
}

int quest_object_in_use(int object_id)
{
    struct record *quest_object;
    struct qbn_place *qbn_place;
    struct qbn_person *qbn_person;
    struct quest *quest;
    int i;

    quest_object = quest_root->children;
    while (quest_object != 0) {
        if (quest_object->type == 14) {
            quest = &quest_object->data.quest;
            qbn_place = (struct qbn_place *)quest_section(quest, 4);
            for (i = 0; quest->section_counts[4] > i; i++, qbn_place++) {
                if (qbn_place->object != 0) {
                    if (qbn_place->object->id == object_id) return 1;
                }
            }
            qbn_person = (struct qbn_person *)quest_section(quest, 3);
            for (i = 0; quest->section_counts[3] > i; i++, qbn_person++) {
                if (qbn_person->object != 0) {
                    if (qbn_person->object->id == object_id) return 1;
                }
            }
        }
        quest_object = quest_object->next;
    }
    return 0;
}
