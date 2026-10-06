/* qmisc.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"
#include "clib.h"
#include "portio.h"

extern char D_00170A08[];
extern char D_00170A11[];
extern char D_00170A1A[];
extern char D_00170A22[];
extern char D_00170A4F[];
extern unsigned char player_environment;
extern signed char item_group_tab[];
extern signed char text_buffer[];
extern signed char scratch_190ce4[];
extern char scratch_190d64[];
extern signed char D_001940D5;
extern signed char quest_global_states[];
extern struct record *inventory_containers[];
extern struct record *D_001959DC;
extern struct record *quest_root;
extern struct quest_face quest_faces[];
extern struct record *camera_object;
extern struct record *player_entity;
extern struct record *player_object;
extern struct record *scratch_current_object;
extern struct record *location_object;
extern struct spell *spell_records;
extern int creature_count;
extern struct record *inv_right_container;
extern struct location *current_location;
extern struct character *player_character;
extern int game_minutes;
extern iptr quest_face_images[];
extern iptr D_00195D14;
extern char D_001960D9[];
extern signed char current_region;
extern signed char game_mode;
extern signed char is_daytime;
extern signed char D_0019629E;
extern signed char D_001962A8;
extern int region_location_count;
extern struct loaded_location loaded_location;
extern struct map_location *region_locations;
extern struct record *quest_reward_container;

extern struct faction *faction_find(short);
extern int tavern_open(short);
extern int quest_arg_state(struct qbn_op *, short);
extern void *quest_section(struct quest *, int);
extern void *quest_record(struct quest *, int, int);
extern struct location_door *location_find_door(int);
extern struct flat_cfg *flats_cfg_find(int);
extern iptr hud_message_add(char *);
extern int rand_range(int, int);
extern int location_contains(int, int);
extern struct record *object_delete(struct record *);
extern struct record *object_create_child(struct record *, struct record *, int);
extern struct record *object_reparent(struct record *, struct record *);
extern struct record *object_find_by_id(struct record *, iptr);
extern int object_new_id(int);
extern struct record *object_find_quest(struct record *, unsigned char);
extern void xn_draw_image_transparent(int, int, int, int, char *);
extern void maploads_load_region(int);
extern void tavern_close(void);
extern void func_0002FE02(struct record *);
extern void func_0002FE4B(struct record *);
extern void quest_cast_spell_on_foe_cb(struct record *);
extern void quest_show_message(struct quest *, int);
extern void quest_reward_faction(struct quest *);
extern void guild_join_dark_brotherhood(void);
extern void guild_join_thieves_guild(void);
extern void dungeon_load(int);
extern void map_goto_location(int, int, int, int);
extern void object_free_children(iptr);
extern void object_foreach(struct record *, void (*)());
extern void inv_store_item(struct record *);
struct record *func_000310E1(struct record *, struct record *);
void quest_give_item_to_player(struct record *);
void quest_face_replace_object_id(int, int);
void quest_faces_remove_quest(unsigned char);
void quest_op_done(struct quest *, struct qbn_op *);
#pragma aux mc_set_location parm routine [];

void qaction_op04_give_reward(struct quest *quest, struct qbn_op *op)
{
    struct record *item;
    struct qbn_item *qbn_item;
    int i;
    int j;

    if (strnicmp(quest->name, D_00170A08, 8) == 0) {
        guild_join_dark_brotherhood();
        return;
    }
    if (strnicmp(quest->name, D_00170A11, 8) == 0) {
        guild_join_thieves_guild();
        return;
    }
    if (tavern_open(0) != 0) tavern_close();
    quest_reward_faction(quest);
    if (((int)(unsigned char)game_mode) != 4) {
        object_free_children((*(iptr *)&quest_reward_container = (iptr)D_001960D9));
    } else {
        quest_reward_container = inv_right_container;
    }
    for (i = 1; i < 5; i++) {
        if (op->args[i].value != (-1)) {
            item = op->args[i].object;
            item->flags |= 0x8000;
            if (item != 0) {
                op->args[i].object = 0;
                qbn_item = quest_section(quest, 0);
                for (j = 0; quest->section_counts[0] > j; j++, qbn_item++) {
                    if (qbn_item->object == item) {
                        qbn_item->object = 0;
                        break;
                    }
                }
                object_reparent(quest_reward_container, item);
                item->x = player_object->x;
                item->y = player_object->y;
                item->z = player_object->z;
            } else {
                mc_set_location(332, D_00170A1A);
                mc_sprintf((char *)text_buffer, D_00170A22, (iptr)quest->name, qbn_item->symbol);
                hud_message_add(text_buffer);
            }
        }
    }
}

void func_0003077F(struct record *object, int delete_twin)
{
    struct record *twin;
    struct location_door *door;

    if (object == 0) return;
    twin = object->twin;
    if (loaded_location.door_count != 0 && (object->id & -65536) == (location_object->id & -65536)) {
        door = location_find_door(object->id);
        if (door != 0) door->flags &= 0xFFF;
    }
    object_delete(object);
    if (twin == 0) return;
    if (delete_twin != 0) {
        object_delete(twin);
        return;
    }
    twin->twin = 0;
    twin->quest_id = 0;
}

void quest_give_item_to_player(struct record *quest_item)
{
    struct item *item_data;
    struct record *given_item;
    struct record *container;

    item_data = &quest_item->data.item;
    if (item_data->enchantments[0].type != (-1)) {
        container = D_001959DC;
    } else {
        container = inventory_containers[(int)(unsigned char)item_group_tab[item_data->group]];
    }
    container = object_create_child(container, 0, 0);
    given_item = func_000310E1(quest_item, container);
    object_delete(container);
    inv_store_item(given_item);
}

void qaction_op37_repute_exceeds(struct quest *quest, struct qbn_op *op)
{
    struct record *person;
    int reputation;
    struct faction *faction;
    struct qbn_state *state;
    int exceeds;
    slot16 person_faction;

    if (op->args[1].value == (-1)) return;
    person = op->args[2].object;
    state = (struct qbn_state *)op->args[1].record;
    if (person == 0) return;
    if (person->type == 65) {
        *(int *)&person_faction = (int)(unsigned short)person->faction_id;
    } else if (person->data.building.faction_id != 0) {
        *(int *)&person_faction = person->data.building.faction_id;
    } else {
        return;
    }
    faction = faction_find((int)(short)person_faction);
    reputation = faction->reputation;
    if (faction->social_group < 5) reputation += player_character->reputation[faction->social_group];
    exceeds = ((reputation >= op->args[3].value) ? 1 : 0);
    if (state->is_global != 0) {
        quest_global_states[state->value] = *(signed char *)&exceeds;
        return;
    }
    state->value = *(signed char *)&exceeds;
}

void quest_faces_after_load(void)
{
    int slot;
    int face_index;
    int i;
    struct image *image;

    for (slot = 0; slot < 10; slot++) {
        i = 0;
        if (((int)(unsigned char)(quest_faces[slot].face & 16)) != 0) {
            image = (struct image *)D_00195D14;
        } else {
            image = (struct image *)quest_face_images[(((int)(unsigned char)quest_faces[slot].face) >> 6)];
        }
        face_index = (int)(unsigned char)(quest_faces[slot].face & 15);
        while (i < face_index) {
            image = (struct image *)((char *)image + image->data_size + 12);
            i++;
        }
        quest_faces[slot].image = image;
        if (object_find_quest(quest_root->children, quest_faces[slot].quest_id) == 0) {
            quest_faces_remove_quest(quest_faces[slot].quest_id);
        }
    }
}

void quest_faces_draw(void)
{
    int slot;
    int drawn_count;
    struct image *image;

    if (game_mode != 0) return;
    slot = 0;
    drawn_count = slot;
    for (; slot < 10; slot++) {
        if (quest_faces[slot].object_id == 0) continue;
        image = quest_faces[slot].image;
        xn_draw_image_transparent((drawn_count << 5) + 8, 36, image->width, image->height, image->pixels);
        drawn_count++;
    }
}

void quest_face_remove(int object_id)
{
    int slot;

    for (slot = 0; slot < 10; slot++) {
        if (quest_faces[slot].object_id == object_id) {
            quest_faces[slot].object_id = 0;
            return;
        }
    }
}

void quest_face_replace_object_id(int old_id, int new_id)
{
    int slot;

    for (slot = 0; slot < 10; slot++) {
        if (quest_faces[slot].object_id == old_id) {
            quest_faces[slot].object_id = new_id;
            return;
        }
    }
}

void quest_faces_remove_quest(unsigned char quest_id)
{
    {
        int slot;

        for (slot = 0; slot < 10; slot++) {
            if (quest_faces[slot].object_id == 0) continue;
            if (quest_faces[slot].quest_id == quest_id) {
                quest_faces[slot].object_id = 0;
            }
        }
    }
}

int qcond_op57_item_used(struct quest *quest, struct qbn_op *op)
{
    struct item *item_data;
    struct qbn_state *state;

    item_data = &op->args[2].object->twin->data.item;
    if (op->args[2].object->twin == 0) return 0;
    if (((int)(unsigned short)(item_data->item_flags & 512)) != 0) {
        if (op->args[1].value == (-1)) return 1;
        state = (struct qbn_state *)op->args[1].record;
        if (state->is_global != 0) {
            quest_global_states[state->value] = 1;
        } else {
            state->value = 1;
        }
        return 1;
    }
    return 0;
}

void quest_items_release_on_close(void)
{
    object_foreach(player_entity->children, func_0002FE02);
}

void func_00030F63(struct quest *quest, struct qbn_op *op)
{
    struct qbn_foe *foe;
    struct record *foe_object;
    int restrain;

    foe = quest_record(quest, 7, (short)op->args[1].value);
    foe_object = op->args[1].object;
    *(short *)scratch_190d64 = foe_object->image2;
    restrain = quest_arg_state(op, 2);
    scratch_190ce4[0] = *(signed char *)&restrain;
    if (restrain != 0) {
        foe_object->data.character.flags |= 0x8000;
    } else {
        foe_object->data.character.flags &= ~0x8000;
    }
    object_foreach(location_object, func_0002FE4B);
}

void qaction_op69_cast_spell_on_foe(struct quest *quest, struct qbn_op *op)
{
    int unused;
    struct record *foe_object;
    int spell_index;
    struct record *spell_object;

    foe_object = op->args[1].object;
    *(short *)scratch_190d64 = foe_object->image2;
    spell_index = 0;
    while (spell_records[spell_index].name[0] == 0 || spell_records[spell_index].id != op->args[2].value) spell_index++;
    spell_object = object_create_child(player_object->parent, 0, 89);
    scratch_current_object = spell_object;
    spell_object->type = 9;
    spell_object->caster = player_entity;
    spell_object->id = object_new_id(((unsigned)location_object->id) >> 16);
    mc_memcpy(&spell_object->data.spell, &spell_records[spell_index], 89, D_00170A1A, 679, 4);
    object_foreach(location_object, quest_cast_spell_on_foe_cb);
    object_delete(spell_object);
}

struct record *func_000310E1(struct record *object, struct record *target)
{
    struct record *twin;
    struct record *child_twin;
    struct person *person;
    int unused1;
    struct flat_cfg *flat_cfg;
    int unused2;
    int data_size;
    int old_id;

    twin = 0;
    if (target == 0) {
        if (object->parent->type == 39) return 0;
        target = object->parent;
        target = object_find_by_id(location_object, target->id);
        if (target == 0) return 0;
    }
    old_id = object->id;
    switch (object->type) {
    case 2:
    case 18:
        data_size = RECORD_BLOCK_SIZE(object) - RECORD_HEADER_SIZE;
        twin = object_create_child(target->parent, 0, data_size);
        twin->x = target->x;
        twin->y = target->y;
        twin->z = target->z;
        mc_memcpy(&twin->data, &object->data, data_size, D_00170A1A, 734, 4);
        twin->type = object->type;
        twin->flags = object->flags;
        twin->image = object->image;
        twin->image2 = object->image2;
        object->id = object_new_id(((unsigned)location_object->id) >> 16);
        twin->id = object->id;
        twin->quest_id = object->quest_id;
        quest_face_replace_object_id(old_id, object->id);
        twin->twin = object;
        object->twin = twin;
        object = object->children;
        while (object != 0) {
            child_twin = object_create_child(twin, 0, 107);
            child_twin->x = target->x;
            child_twin->y = target->y;
            child_twin->z = target->z;
            mc_memcpy(&child_twin->data, &object->data, 107, D_00170A1A, 754, 4);
            child_twin->type = object->type;
            child_twin->flags = object->flags;
            child_twin->image = object->image;
            child_twin->image2 = object->image2;
            object->id = object_new_id(((unsigned)location_object->id) >> 16);
            child_twin->id = object->id;
            child_twin->quest_id = object->quest_id;
            child_twin->twin = object;
            object->twin = child_twin;
            object = object->next;
        }
        break;
    case 41:
        switch (target->type) {
        case 34:
            twin = object_create_child(target->parent, 0, 3);
            twin->type = 8;
            twin->x = target->x;
            twin->y = target->y;
            twin->z = target->z;
            object->id = object_new_id(((unsigned)location_object->id) >> 16);
            twin->id = object->id;
            quest_face_replace_object_id(old_id, object->id);
            break;
        case 8:
            twin = target;
            twin->data.person.flags |= 128;
            break;
        case 40:
            target = object_find_by_id(location_object, target->id);
            twin = object_create_child(target->parent, 0, 3);
            twin->type = 8;
            twin->x = target->x;
            twin->y = target->y;
            twin->z = target->z;
            object->id = object_new_id(((unsigned)location_object->id) >> 16);
            twin->id = object->id;
            quest_face_replace_object_id(old_id, object->id);
            break;
        default:
            return 0;
        }
        twin->quest_id = object->quest_id;
        twin->name_seed = object->name_seed;
        person = &twin->data.person;
        if ((person->flags & 128) == 0) {
            person->faction_id = object->data.building.faction_id;
        }
        if (person->faction_id == 0) {
            mc_set_location(819, D_00170A1A);
            func_000A148C(D_00170A4F, 819);
            person->faction_id = 510;
        }
        if (object->image != 0) {
            twin->image = object->image;
        } else {
            twin->image = faction_find((short)person->faction_id)->flats[0];
        }
        flat_cfg = flats_cfg_find(twin->image);
        person->flags |= (((flat_cfg->flags & 1) != 0) ? 16 : 0);
        if (((int)(unsigned short)(object->flags & 4)) != 0) {
            twin->flags |= 4;
        } else {
            twin->flags &= ~0x4;
        }
        twin->twin = object;
        object->twin = twin;
        break;
    case 40:
        twin = object_find_by_id(location_object, object->id);
        if (twin == 0) return 0;
        twin->quest_id = object->quest_id;
        twin->twin = object;
        object->twin = twin;
    }
    if (twin != 0 && ((int)(unsigned short)(object->flags & 2048)) != 0) twin->flags |= 0xA00;
    return twin;
}

void func_00031658(struct quest *quest, struct qbn_op *op, int immediate)
{
    op->flags |= 2;
    if (immediate == 0) {
        if (D_0019629E != 0) return;
        if (creature_count != 0) return;
        if (D_001962A8 != 0) return;
        if (game_mode != 0) return;
        if (player_character->race == 8 && is_daytime != 0) return;
        if (player_character->race != 8 && is_daytime == 0) return;
        if (((int)player_environment) != 1 || location_contains(player_object->x, player_object->z) == 0) {
            return;
        }
        if (current_location->kind == 4 || current_location->kind == 7 || current_location->kind > 9) {
            return;
        }
        if ((game_minutes - op->last_minutes) == 0) return;
        if (rand_range(1, 100) >= 10) goto L3179A;
    }
    op->flags |= 4;
    quest_give_item_to_player(op->args[1].object);
    quest_op_done(quest, op);
L3179A:;
    op->last_minutes = game_minutes;
}

int quest_deliveries_done(struct quest *quest)
{
    int i;
    struct qbn_op *op;

    op = quest_section(quest, 8);
    for (i = 0; quest->section_counts[8] > i; i++, op++) {
        if (op->opcode == 76 && ((int)(short)(op->flags & 2)) != 0 && ((int)(short)(op->flags & 4)) == 0) {
            return 0;
        }
    }
    return 1;
}

void qaction_op83_teleport_pc(struct qbn_op *op)
{
    struct record *destination;
    struct map_location *map_entry;
    char *place;
    int location_index;
    int map_id;
    int region;
    int saved_region;

    saved_region = (int)(unsigned char)current_region;
    place = op->args[1].record;
    region = op->args[2].value;
    map_id = op->args[3].value;
    maploads_load_region(region);
    map_entry = region_locations;
    for (location_index = 0; location_index < region_location_count; location_index++, map_entry++) {
        if ((map_entry->map_id & 1048575) == map_id) {
            maploads_load_region(saved_region);
            map_goto_location(region, 1, location_index, 0);
            dungeon_load(-1);
            destination = op->args[1].object->twin;
            if (destination != 0) {
                player_object->x = destination->x;
                player_object->y = destination->y;
                player_object->z = destination->z;
                player_object->yaw = camera_object->yaw;
                D_001940D5 |= 2;
            }
            op->flags |= 1;
            return;
        }
    }
}

void quest_op_done(struct quest *quest, struct qbn_op *op)
{
    if (((int)(short)(op->flags & 1)) != 0) return;
    op->flags |= 1;
    quest_show_message(quest, op->message);
}
