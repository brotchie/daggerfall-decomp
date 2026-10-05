/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00036233 */
#include "records.h"

extern char D_00170AD2[];
extern unsigned char D_0017A834[];
extern struct record *location_object;
extern char D_001962A1;
extern struct rdb_object_id rdb_object_ids[];
extern char *rdb_data;
extern struct rdb_flat *rdb_flat_resource;
extern struct rdb_light *rdb_light_resource;
extern int D_001995F8;
extern int D_001995FC;
extern struct rdb_file *rdb_loaded_file;
extern struct dungeon_block *rdb_dungeon_block;
extern struct rdb_model *rdb_model_resource;
extern int rdb_object_id_count;
extern void func_000361B7(struct rdb_model *);
extern void rdb_model_id_from_name(struct record *, char *);
extern void fatal_error(char *);
extern int rand_range(int, int);
extern struct record *rmb_make_marker(struct record *, int);
extern struct record *rmb_make_flat(struct record *, short, short, int);
extern struct record *object_free_single(struct record *);
extern struct record *object_create_in_block(struct record *, int, int, int, int);
extern void xn_model_set_angles(int, int, int, int *);
extern struct tex_cache_entry *xn_tex_cache_lookup(int, int, int);
extern void xn_tex_cache_flush(void);

void rdb_create_objects(struct record *quarter, struct rdb_object *rdb_object, int block_index)
{
    int unused1;
    struct record *object;
    struct person *person;
    struct tex_cache_entry *res;
    int unused2;
    int unused3;
    int offset;
    int unused4;
    int height;

    do {
        offset = (char *)rdb_object - rdb_data;
        switch (rdb_object->type) {
        case 1:
            rdb_model_resource = (struct rdb_model *)(rdb_data + rdb_object->resource_offset);
            if (rdb_model_resource->action_offset <= 0)
                func_000361B7(rdb_model_resource);
            object = object_create_in_block(quarter, 6, 62, 0, block_index);
            rdb_model_id_from_name(object, rdb_loaded_file->model_references[rdb_model_resource->model_index].model_id);
            if ((object->image2 == 703 || object->image2 == 704) && (rdb_dungeon_block->x || rdb_dungeon_block->z)) {
                object_free_single(object);
                object = 0;
                break;
            }
            object->wait_state = 0;
            xn_model_set_angles(rdb_model_resource->x_rotation, rdb_model_resource->y_rotation, rdb_model_resource->z_rotation, (int *)object->data.instance.angles);
            if (object->image2 == 550) {
                object->type = 32;
                object->lock_level = D_0017A834[rdb_model_resource->trigger_flag_starting_lock >> 4];
            }
            break;
        case 2:
            rdb_light_resource = (struct rdb_light *)(rdb_data + rdb_object->resource_offset);
            object = object_create_in_block(quarter, 7, 0, rdb_light_resource->image, block_index);
            object->light_radius = rdb_light_resource->radius;
            break;
        case 3:
            rdb_flat_resource = (struct rdb_flat *)(rdb_data + rdb_object->resource_offset);
            if ((rdb_flat_resource->image >> 7) == 199) {
                switch ((rdb_flat_resource->image & 31) - 2) {
                case 14:
                    object = rmb_make_marker(quarter, rdb_flat_resource->image);
                    object->trigger_range = rdb_flat_resource->sound_index;
                    object->mobile_id = rdb_flat_resource->magnitude;
                    object->link_flag = rdb_flat_resource->flags;
                    object->wait_state = rdb_flat_resource->action;
                    break;
                case 13:
                    object = rmb_make_marker(quarter, rdb_flat_resource->image);
                    object->trigger_range = rdb_flat_resource->sound_index;
                    object->mobile_id = rdb_flat_resource->flags;
                    if (object->mobile_id == 0)
                        object->mobile_id = rand_range(1, 6);
                    object->wait_state = rdb_flat_resource->action;
                    break;
                case 8:
                    if (rdb_flat_resource->flags != 0)
                        D_001995F8 = rdb_flat_resource->flags;
                    if (rdb_flat_resource->sound_index != 0)
                        D_001995FC = -(rdb_flat_resource->sound_index << 3);
                    D_001962A1 = rdb_flat_resource->magnitude;
                    if (rdb_dungeon_block->is_starting == 0) {
                        object = 0;
                        break;
                    }
                default:
                    object = rmb_make_marker(quarter, rdb_flat_resource->image);
                    break;
                }
            } else if (rdb_flat_resource->action == 29) {
                object = rmb_make_flat(quarter, rdb_flat_resource->image, rdb_flat_resource->magnitude + (rdb_flat_resource->sound_index << 8), 0);
                person = &object->data.person;
                if (rdb_flat_resource->flags & 16)
                    person->flags |= 32;
                if (rdb_flat_resource->flags & 32)
                    person->flags |= 16;
                person->faction_id = (rdb_flat_resource->sound_index << 8) + rdb_flat_resource->magnitude;
            } else {
                object = object_create_in_block(quarter, 33, 0, rdb_flat_resource->image, block_index);
            }
            break;
        }
        if (object != 0) {
            rdb_object_ids[rdb_object_id_count].offset = offset;
            rdb_object_ids[rdb_object_id_count].id = object->id;
            if (rdb_object_id_count++ > 512)
                fatal_error(D_00170AD2);
            object->x = location_object->x + rdb_object->x + (rdb_dungeon_block->x << 11);
            object->y = location_object->y + rdb_object->y;
            object->z = location_object->z + rdb_object->z + (rdb_dungeon_block->z << 11);
            if (rdb_object->type == 3) {
                res = xn_tex_cache_lookup(object->image >> 7, object->image & 127, 0);
                if (res == 0) {
                    xn_tex_cache_flush();
                    res = xn_tex_cache_lookup(object->image >> 7, object->image & 127, 0);
                }
                height = res->image->height * (res->image->x_scale + 256) / 256;
                object->y += height >> 1;
            }
        }
        rdb_object = (struct rdb_object *)(rdb_data + rdb_object->next);
    } while ((char *)rdb_object - rdb_data > 0);
}
