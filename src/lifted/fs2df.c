/* fs2df.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"
#include "clib.h"

extern char D_00170AB4[];
extern char D_00170AEC[];
extern char D_00170AF9[];
extern char D_00170B06[];
extern signed char D_001940D8;
extern iptr magic_window_image;
extern iptr window_image;
extern unsigned char D_0019626F;
extern signed char D_00196272;
extern signed char game_mode;
extern struct rdb_object_id rdb_object_ids[];
extern struct link *D_001995E4;
extern char *rdb_data;
extern struct link *D_001995EC;
extern struct rdb_flat *rdb_flat_resource;
extern struct rdb_light *rdb_light_resource;
extern struct rdb_action *rdb_action_resource;
extern struct rdb_file *rdb_loaded_file;
extern struct rdb_model *rdb_model_resource;
extern int rdb_object_id_count;
extern short rdb_link_object_id;
extern iptr spellmaker_settings_image;
extern struct link links[1024];
extern int link_count;

extern int spellmaker_new(void);
extern iptr disk_read_file(char *, iptr);
short rdb_object_id_by_offset(int);
void func_000361B7(struct rdb_model *);
void rdb_build_action_chain(int);
void action_record_add(struct rdb_model *, struct rdb_action *, struct rdb_flat *, unsigned char);
void action_record_add_chained(struct rdb_model *, struct rdb_action *, struct rdb_flat *, unsigned char);
void action_axis_to_translation(struct link *);

void func_000361B7(struct rdb_model *model)
{
    struct rdb_unknown_entry *entry;
    int key;

    key = 0;
    entry = (struct rdb_unknown_entry *)(rdb_data + rdb_loaded_file->object_header.unknown_offset);
    while (entry->key != key) {
        entry = (struct rdb_unknown_entry *)(rdb_data + entry->next);
    }
    model->action_offset = entry->action_offset;
    model->trigger_flag_starting_lock = entry->trigger_flag_starting_lock;
    model->sound_index = entry->sound_index;
}

void rdb_link_actions(struct record *quarter, struct rdb_object *rdb_object, int block_index)
{
    int unused1;
    int unused2;
    int offset;

    do {
        offset = (int)((char *)rdb_object - rdb_data);
        switch (rdb_object->type) {
        case 1:
            if ((rdb_model_resource = (struct rdb_model *)(rdb_data + rdb_object->resource_offset))->action_offset < 0) {
                func_000361B7(rdb_model_resource);
            }
            if (rdb_model_resource->trigger_flag_starting_lock != 0) {
                rdb_action_resource = (struct rdb_action *)(rdb_data + rdb_model_resource->action_offset);
                rdb_link_object_id = rdb_object_id_by_offset(offset);
                rdb_build_action_chain(rdb_object->type);
            }
            break;
        case 2:
            rdb_light_resource = (struct rdb_light *)(rdb_data + rdb_object->resource_offset);
            break;
        case 3:
            if (((rdb_flat_resource = (struct rdb_flat *)(rdb_data + rdb_object->resource_offset))->flags != 0 && rdb_flat_resource->image != 25482) || (rdb_flat_resource->image == 25490 && rdb_flat_resource->magnitude == 70 && rdb_flat_resource->next_object_offset == 16747 && rdb_flat_resource->action == 5)) {
                if (rdb_flat_resource->image != 25488) {
                    rdb_link_object_id = rdb_object_id_by_offset(offset);
                    rdb_build_action_chain(rdb_object->type);
                }
            }
        }
        rdb_object = (struct rdb_object *)(rdb_data + rdb_object->next);
    } while (((char *)rdb_object - rdb_data) > 0);
}

short rdb_object_id_by_offset(int offset)
{
    int i;

    for (i = 0; i < rdb_object_id_count; i++) {
        if (rdb_object_ids[i].offset == offset) return rdb_object_ids[i].id;
    }
    return 0;
}

void rdb_build_action_chain(int resource_type)
{
    struct rdb_object *rdb_object;
    int offset;

    switch ((unsigned)resource_type) {
    case 1:
        action_record_add(rdb_model_resource, rdb_action_resource, 0, 0);
        offset = rdb_action_resource->next_object_offset;
        break;
    case 2:
        action_record_add(0, 0, 0, rdb_light_resource->action);
        offset = rdb_light_resource->next_object_offset;
        break;
    case 3:
        action_record_add(0, 0, rdb_flat_resource, rdb_flat_resource->action);
        offset = rdb_flat_resource->next_object_offset;
    }
    while (offset > 0) {
        rdb_link_object_id = rdb_object_id_by_offset(offset);
        rdb_object = (struct rdb_object *)(rdb_data + offset);
        switch (rdb_object->type) {
        case 1:
            if ((rdb_model_resource = (struct rdb_model *)(rdb_data + rdb_object->resource_offset))->action_offset < 0) {
                func_000361B7(rdb_model_resource);
            }
            rdb_action_resource = (struct rdb_action *)(rdb_data + rdb_model_resource->action_offset);
            action_record_add_chained(rdb_model_resource, rdb_action_resource, 0, 0);
            offset = rdb_action_resource->next_object_offset;
            break;
        case 2:
            action_record_add_chained(0, 0, 0, (rdb_light_resource = (struct rdb_light *)(rdb_data + rdb_object->resource_offset))->action);
            offset = rdb_light_resource->next_object_offset;
            break;
        case 3:
            action_record_add_chained(0, 0, rdb_flat_resource, (rdb_flat_resource = (struct rdb_flat *)(rdb_data + rdb_object->resource_offset))->action);
            offset = rdb_flat_resource->next_object_offset;
        }
    }
}

void action_record_add(struct rdb_model *model, struct rdb_action *model_action, struct rdb_flat *flat, unsigned char action)
{
    D_001995EC = (D_001995E4 = &links[link_count++]);
    mc_memset(D_001995EC, 0, REC_SIZEOF(struct link), D_00170AB4, 447, 4);
    D_001995EC->object_id = rdb_link_object_id;
    if (model_action != 0) {
        D_001995EC->trigger = model->trigger_flag_starting_lock;
        D_001995EC->param = model->sound_index;
        D_001995EC->axis = model_action->axis;
        D_001995EC->duration = model_action->duration;
        D_001995EC->magnitude = model_action->magnitude;
        D_001995EC->action = model_action->action;
    } else if (flat != 0) {
        D_001995EC->trigger = flat->flags;
        D_001995EC->param = flat->sound_index;
        D_001995EC->axis = flat->magnitude;
        D_001995EC->action = action;
    } else {
        D_001995EC->action = action;
    }
    if (D_001995EC->action > 1 && D_001995EC->action < 8) action_axis_to_translation(D_001995EC);
    D_001995EC->chain_count = 0;
}

void action_record_add_chained(struct rdb_model *model, struct rdb_action *model_action, struct rdb_flat *flat, unsigned char action)
{
    D_001995E4->chain_count++;
    D_001995EC = &links[link_count++];
    mc_memset(D_001995EC, 0, REC_SIZEOF(struct link), D_00170AB4, 490, 4);
    D_001995EC->object_id = rdb_link_object_id;
    if (model_action != 0) {
        D_001995EC->trigger = model->trigger_flag_starting_lock;
        D_001995EC->param = model->sound_index;
        D_001995EC->axis = model_action->axis;
        D_001995EC->duration = model_action->duration;
        D_001995EC->magnitude = model_action->magnitude;
        D_001995EC->action = model_action->action;
    } else if (flat != 0) {
        D_001995EC->trigger = flat->flags;
        D_001995EC->param = flat->sound_index;
        D_001995EC->axis = flat->magnitude;
        D_001995EC->action = action;
    } else {
        D_001995EC->action = action;
    }
    if (D_001995EC->action <= 1 || D_001995EC->action >= 8) return;
    action_axis_to_translation(D_001995EC);
}

void action_axis_to_translation(struct link *link)
{
    link->magnitude = link->axis << 3;
    link->axis = ((link->action - 2) ^ 1) + 1;
    link->duration = 50;
    link->action = 1;
}

int spellmaker_open(int opening)
{
    if (((int)D_0019626F) == 2) return 1;
    if (opening != 0) {
        game_mode = 2;
        window_image = disk_read_file(D_00170AEC, 0);
        magic_window_image = disk_read_file(D_00170AF9, 0);
        spellmaker_settings_image = disk_read_file(D_00170B06, 0);
        D_00196272 = 1;
        D_001940D8 |= 1;
        spellmaker_new();
    }
    return ((((int)(unsigned char)game_mode) == 2) ? 1 : 0);
}
