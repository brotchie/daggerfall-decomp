/* objlib.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern int xn_anim_ticks;
extern char D_00176C20[];
extern char D_00176C29[];
extern char D_00176C4F[];
extern char D_00176C6B[];
extern char D_00176C80[];
extern unsigned char player_environment;
extern short D_00187CB4[];
extern char D_00187D30[];
extern signed char D_00187DAC[];
extern char D_00187DC0[];
extern char D_00187DE8[];
extern char D_00187EC8[];
extern signed char text_buffer[];
extern char scratch_190de4[];
extern char D_001910AC[];
extern char frame_counter[];
extern int view_look_yaw;
extern struct building *current_building;
extern struct record *player_object;
extern struct record *location_object;
extern struct location *current_location;
extern struct settings *game_settings;
extern char scratch_buffer[];
extern int sound_last_size;
extern signed char current_region;
extern signed char current_climate;
extern int rmb_origin_z;
extern int rmb_origin_y;
extern int rmb_origin_x;
extern char rmb_origin_yaw[];
extern int town_house_skip_percent;
extern int town_building_counter;
extern struct block *rmb_record_ptr;
extern signed char location_is_port;
extern char rmb_block[];
extern struct model_node model_cache_nodes[];
extern struct sound_cache_entry sound_cache[];
extern int model_heap_free;
extern struct model_node *model_cache_root;
extern int sound_cache_bytes;
extern int D_001A9438;
extern int sound_last_id;
extern int arch3d_bsa;
extern int dagger_snd;
extern char model_heap[];
extern signed char D_001A949C;
extern signed char model_cache_flush_count;

extern int archive_find_record(int, int, int);
extern int archive_record_size(int, int);
extern int archive_read_record(int, int, int);
extern int collide_floor_height(struct record *);
extern int flats_cfg_find(int);
extern int sound_play_at_point(int, int, int, int, int);
extern int mem_pool_alloc(int, int);
extern int mem_pool_release(int);
extern int rand_range(int, int);
extern int model_get(unsigned short, int, int);
extern int flat_table_pick(int);
extern struct record *object_create_child(struct record *, struct record *, int);
extern int dpmi_lock_region(int, int);
extern int dpmi_unlock_region(int, int);
extern int rand();
extern int srand();
extern int mc_free();
extern int mc_memset();
extern int mc_malloc();
extern int mc_set_location(int, int);
extern int mc_sprintf(int, ...);
extern int mc_memcpy();
extern int xn_rand_noise_2d();
extern int xn_math_approx_dist2d();
extern int xn_math_yaw_offset_xz();
extern int xn_model_max_y();
extern int xn_light_add();
extern int xn_model_prepare();
extern int xn_flat_add();
extern void arch3d_apply_climate_textures(int);
extern void fatal_error(int);
extern void mem_pool_init(int, int);
extern void mem_pool_free(int);
extern void rotate_xz(int *, int *, int);
extern void rmb_add_doors(struct record *, int);
extern void rmb_add_people(struct record *, int);
extern void rmb_add_editor_marker(struct record *, struct block_flat *);
extern void object_foreach(struct record *, int);
struct record *rmb_add_subrecord(struct record *);
int model_load(int, int);
int model_cache_add(int);
int flat_random_clutter(int, int);
void rmb_set_building_factions(struct record *);
void model_cache_purge_old(struct model_node *);
void model_cache_flush(struct model_node *);
void model_cache_purge_unused(struct model_node *, int, struct model_node *);
void model_cache_remove_node(struct model_node *, struct model_node *);
void sound_cache_trim(void);
void model_unlink_object_cb(struct record *);
void model_unlink_objects(int);
#pragma aux mc_set_location parm routine [];

struct record *rmb_add_subrecord(struct record *parent)
{
    struct record *object;
    struct block *block;
    struct block_model *model;
    struct block_flat *flat;
    int people;
    struct block_section3 *entry;
    int doors;
    struct flat_cfg *flat_cfg;
    int i;
    int unused;
    int unused2;
    int size;

    size = 17;
    size += rmb_record_ptr->model_count * 66;
    size += rmb_record_ptr->flat_count * 17;
    size += rmb_record_ptr->section3_count << 4;
    if (size == 17) return 0;
    object = object_create_child(parent, 0, size);
    object->type = 43;
    object->x = rmb_origin_x;
    object->z = rmb_origin_z;
    object->y = rmb_origin_y;
    object->yaw = *(short *)rmb_origin_yaw;
    object->pad13 = 32768;
    object->id = location_object->id + ((int)(unsigned short)(current_location->object_counter)++);
    D_001A9438 = object->id;
    block = &object->data.block;
    mc_memcpy((int)block, (int)rmb_record_ptr, size, (int)D_00176C20, 803, 4);
    block->models = (struct block_model *)((int)block + 17);
    model = block->models;
    block->flats = (struct block_flat *)((int)model + (block->model_count * 66));
    flat = block->flats;
    block->section3 = (struct block_section3 *)((int)flat + (block->flat_count * 17));
    entry = block->section3;
    for (i = 0; block->model_count > i; i++, model++) {
        rotate_xz(&model->x, &model->z, *(int *)rmb_origin_yaw);
        model->x += rmb_origin_x;
        model->z += rmb_origin_z;
        model->model = 0;
        if (model->y > 0 && model->id > 10) {
            model->model = (char *)model_get(model->id, model->variant, (((int)(unsigned char)current_climate) << 2) + ((int)(unsigned char)D_001A949C));
            model->y = (-model->y) - (xn_model_max_y((int)model->model) >> 8);
        }
        model->y += rmb_origin_y;
        model->yaw += *(int *)rmb_origin_yaw;
    }
    for (i = 0; block->flat_count > i; i++, flat++) {
        rotate_xz(&flat->x, &flat->z, *(int *)rmb_origin_yaw);
        flat->x += rmb_origin_x;
        flat->z += rmb_origin_z;
        flat->y += rmb_origin_y;
        if ((flat->image >> 7) == 199) {
            rmb_add_editor_marker(parent, flat);
        } else {
            flat_cfg = (struct flat_cfg *)flats_cfg_find(flat->image);
            if ((flat_cfg->flags & 2) != 0 && ((int)(unsigned short)(game_settings->view_flags & 4)) != 0) {
                flat->image = 0;
            }
        }
    }
    for (i = 0; block->section3_count > i; i++, entry++) {
        rotate_xz(&entry->x, &entry->z, *(int *)rmb_origin_yaw);
        entry->x += rmb_origin_x;
        entry->z += rmb_origin_z;
        entry->y += rmb_origin_y;
    }
    people = (int)((char *)rmb_record_ptr + size);
    doors = people + (rmb_record_ptr->people_count * 17);
    rmb_add_people(parent, people);
    rmb_add_doors(parent, doors);
    rmb_record_ptr = (struct block *)(doors + (rmb_record_ptr->door_count * 19));
    return object;
}

void rmb_set_building_factions(struct record *object)
{
    struct person *person;
    struct building *building;
    int unused;

    building = &current_location->buildings[object->image];
    if (building->type != 16) {
        if (building->faction_id == 0) {
            building->faction_id = *(short *)(D_00187D30 + (((int)(unsigned char)current_region) * 2));
        }
        return;
    }
    building->faction_id = ((((int)(unsigned char)(location_is_port & 16)) != 0) ? 852 : 242);
    object = object->children;
    while (object != 0) {
        if (object->type == 8) {
            person = &object->data.person;
            if (person->faction_id == *(short *)(D_00187D30 + (((int)(unsigned char)current_region) * 2))) {
                person->faction_id = D_00187CB4[((int)(unsigned char)current_region)];
            }
            if (person->faction_id == 852 && ((int)(unsigned char)(location_is_port & 16)) == 0) {
                person->faction_id = 242;
            }
        }
        object = object->next;
    }
}

struct record *rmb_add_building(struct record *parent, int building_index)
{
    int value;
    int saved_seed;
    struct record *object;

    rmb_record_ptr = (struct block *)(*(int *)(*(char **)rmb_block + 1475 + (building_index << 2)));
    object = rmb_add_subrecord(parent);
    object->flags = 1;
    object->image2 = building_index;
    value = (int)(unsigned char)*(signed char *)(*(char **)rmb_block + 667 + (building_index * 26));
    switch ((unsigned)value) {
    case 21:
    case 22:
        object->flags |= 8;
        object->image = 65535;
        return object;
    case 17:
    case 18:
    case 19:
    case 20:
        saved_seed = rand();
        srand((int)(short)(short)object->id);
        value = rand();
        srand(saved_seed);
        if (((int)(unsigned short)*(short *)(*(char **)rmb_block + 661 + (building_index * 26))) != 42 && ((int)(unsigned short)*(short *)(*(char **)rmb_block + 661 + (building_index * 26))) != 108) {
            if ((value % 100) <= town_house_skip_percent) {
                object->flags |= 8;
                object->image = 65535;
                return object;
            }
        }
    }
    object->image = town_building_counter++;
    rmb_add_subrecord(object);
    rmb_set_building_factions(object);
    return object;
}

void model_heap_init(int size)
{
    mem_pool_init((int)model_heap, size);
    model_heap_free = size;
    (model_cache_root = model_cache_nodes)->key = 50000;
    model_cache_root->last_frame = 0;
}

void model_heap_free_all(void)
{
    mem_pool_free((int)model_heap);
}

int model_cache_find(int key)
{
    struct model_node *node;
    struct model_node *added;

    node = model_cache_root;
    while (1) {
        if (key == node->key) {
            node->last_frame = *(int *)frame_counter;
            return (int)node->model;
        }
        if (((unsigned)key) < node->key) {
            if (node->left != 0) {
                node = node->left;
            } else {
                node->left = (struct model_node *)model_cache_add(key);
                added = node->left;
                break;
            }
        } else if (node->right != 0) {
            node = node->right;
        } else {
            node->right = (struct model_node *)model_cache_add(key);
            added = node->right;
            break;
        }
    }
    if (added == 0) return 0;
    added->last_frame = *(int *)frame_counter;
    return (int)added->model;
}

int model_load(int slot, int key)
{
    int record;
    int size;
    int model_id;

    model_id = key & 131071;
    record = archive_find_record(arch3d_bsa, (int)text_buffer, model_id);
    size = archive_record_size(arch3d_bsa, record);
    if ((model_cache_nodes[slot].model = (char *)mem_pool_alloc((int)model_heap, size)) == 0) {
        if (model_cache_flush_count == 0) {
            model_cache_purge_old(model_cache_root);
            if ((model_cache_nodes[slot].model = (char *)mem_pool_alloc((int)model_heap, size)) == 0) {
                model_cache_flush(model_cache_root);
                return 0;
            }
        } else {
            model_cache_purge_old(model_cache_root);
            if ((model_cache_nodes[slot].model = (char *)mem_pool_alloc((int)model_heap, size)) == 0) {
                fatal_error((int)D_00176C29);
            }
        }
    }
    if (archive_read_record(arch3d_bsa, record, (int)model_cache_nodes[slot].model) == 0) {
        mc_set_location(1104, (int)D_00176C20);
        mc_sprintf(*(int *)scratch_buffer, (int)D_00176C4F, record);
        fatal_error(*(int *)scratch_buffer);
    }
    model_heap_free -= (size + 1) & -2;
    return (int)model_cache_nodes[slot].model;
}

int model_cache_add(int key)
{
    int slot;
    int saved_seed;
    {
        int in_use3;
        int in_use2;
        int in_use;

        slot = 0;
        for (;;) {
            if (model_cache_nodes[slot].key != 0 || (int)model_cache_nodes[slot].left != 0 || (int)model_cache_nodes[slot].right != 0) {
                in_use = 1;
            } else {
                in_use = 0;
            }
            if (in_use == 0 || slot >= 512) break;
            slot++;
        }
        if (slot == 512) {
            model_cache_purge_old(model_cache_root);
            slot = 0;
            for (;;) {
                if (model_cache_nodes[slot].key != 0 || (int)model_cache_nodes[slot].left != 0 || (int)model_cache_nodes[slot].right != 0) {
                    in_use2 = 1;
                } else {
                    in_use2 = 0;
                }
                if (in_use2 == 0 || slot >= 512) break;
                slot++;
            }
            if (slot == 512) {
                model_cache_flush(model_cache_root);
                slot = 0;
                for (;;) {
                    if (model_cache_nodes[slot].key != 0 || (int)model_cache_nodes[slot].left != 0 || (int)model_cache_nodes[slot].right != 0) {
                        in_use3 = 1;
                    } else {
                        in_use3 = 0;
                    }
                    if (in_use3 == 0 || slot >= 512) break;
                    slot++;
                }
                if (slot == 512) fatal_error((int)D_00176C6B);
            }
        }
        model_cache_nodes[slot].key = key;
        model_cache_nodes[slot].last_frame = *(int *)frame_counter;
        model_cache_nodes[slot].left = (model_cache_nodes[slot].right = 0);
        if (model_load(slot, key) == 0) return 0;
        xn_model_prepare((int)model_cache_nodes[slot].model);
        saved_seed = rand();
        srand(*(int *)(model_cache_nodes[slot].model + 12));
        arch3d_apply_climate_textures((int)model_cache_nodes[slot].model);
        srand(saved_seed);
        return (int)&model_cache_nodes[slot];
    }
}

void model_cache_purge_old(struct model_node *root)
{
    int age;

    age = 200;
    if (model_heap_free > 204800) return;
    while (age > 2 && model_heap_free < 204800) {
        model_cache_purge_unused(0, age, root);
        age >>= 1;
    }
}

void model_cache_flush(struct model_node *root)
{
    int i;

    model_cache_flush_count++;
    for (i = 1; i < 512; i++) {
        if (model_cache_nodes[i].key != 0 && (int)model_cache_nodes[i].model != 0) {
            model_unlink_objects((int)model_cache_nodes[i].model);
            model_heap_free += mem_pool_release((int)model_cache_nodes[i].model);
        }
    }
    root->left = 0;
    root->right = root->left;
    mc_memset((int)&model_cache_nodes[1], 0, 10220, (int)D_00176C20, 1189, 4);
}

void model_cache_purge_unused(struct model_node *parent, int age, struct model_node *node)
{
    int unused;

    if (node->left != 0) model_cache_purge_unused(node, age, node->left);
    if (node->right != 0) model_cache_purge_unused(node, age, node->right);
    if (((unsigned)(*(int *)frame_counter - node->last_frame)) >= age) model_cache_remove_node(parent, node);
    if (parent->left != parent->right || parent->left == 0) return;
    fatal_error((int)D_00176C80);
}

void model_cache_remove_node(struct model_node *parent, struct model_node *node)
{
    char *model;
    struct model_node *successor;
    struct model_node *successor_parent;

    if (parent == 0) return;
    if (node->left == 0) {
        if (node->right == 0) {
            if (parent->right == node) {
                parent->right = 0;
            } else {
                parent->left = 0;
            }
        } else if (parent->right == node) {
            parent->right = node->right;
        } else {
            parent->left = node->right;
        }
        model_unlink_objects((int)node->model);
        model_heap_free += mem_pool_release((int)node->model);
        node->model = 0;
        node->key = 0;
        return;
    }
    if (node->right == 0) {
        if (parent->right == node) {
            parent->right = node->left;
        } else {
            parent->left = node->left;
        }
        model_unlink_objects((int)node->model);
        model_heap_free += mem_pool_release((int)node->model);
        node->model = 0;
        node->key = 0;
        return;
    }
    successor_parent = node;
    successor = node->right;
    while (successor->left != 0) {
        successor_parent = successor;
        successor = successor->left;
    }
    model = node->model;
    node->model = successor->model;
    node->last_frame = successor->last_frame;
    node->key = successor->key;
    successor->model = model;
    model_cache_remove_node(successor_parent, successor);
}

void marker_make_clutter(struct record *marker, struct building *building)
{
    int floor_y;
    int saved_seed;

    saved_seed = rand();
    srand(marker->id & 65535);
    floor_y = collide_floor_height(marker);
    if (floor_y != 100000) marker->y = floor_y;
    marker->image = flat_random_clutter((int)player_environment, current_building->type);
    srand(saved_seed);
}

void marker_make_loot_pile(struct record *marker, struct building *building)
{
    int floor_y;

    floor_y = collide_floor_height(marker);
    if (floor_y != 100000) marker->y = floor_y;
    if (((int)player_environment) != 3) {
        marker->type = 33;
        marker->image = *(short *)(D_00187DC0 + (building->type * 2));
    } else {
        marker->type = 33;
        marker->image = ((unsigned short)(unsigned char)D_00187DAC[rand() % 20]) + 27648;
        marker->pad19 = 1;
    }
    if (marker->image != 0) if (marker->image != 65535) return;
    floor_y++;
}

int sound_cache_load(int id)
{
    int unused;
    int slot;
    int size;
    int record;

    sound_last_id = id;
    for (slot = 0; slot < 256; slot++) {
        if (sound_cache[slot].id == id) {
            sound_last_size = sound_cache[slot].size;
            return (int)sound_cache[slot].data;
        }
    }
    slot = 0;
    while (sound_cache[slot].data != 0) slot++;
    record = archive_find_record(dagger_snd, (int)D_001910AC, id);
    size = archive_record_size(dagger_snd, record);
    sound_cache[slot].last_frame = *(int *)frame_counter;
    sound_cache[slot].id = id;
    sound_cache[slot].size = size;
    sound_cache[slot].data = (char *)mc_malloc(size, (int)D_00176C20, 1338);
    dpmi_lock_region((int)sound_cache[slot].data, size + 4096);
    archive_read_record(dagger_snd, record, (int)sound_cache[slot].data);
    sound_cache_bytes += size;
    sound_cache_trim();
    sound_last_size = size;
    return (int)sound_cache[slot].data;
}

void sound_cache_trim(void)
{
    int oldest_frame;
    int oldest;
    int i;

    if (sound_cache_bytes < 393216) return;
    while (sound_cache_bytes > 262144) {
        oldest = -1;
        oldest_frame = *(int *)frame_counter;
        for (i = 0; i < 256; i++) {
            if (sound_cache[i].data == 0) continue;
            if (oldest_frame > sound_cache[i].last_frame) {
                oldest_frame = sound_cache[i].last_frame;
                oldest = i;
            }
        }
        if (oldest == (-1)) return;
        dpmi_unlock_region((int)sound_cache[oldest].data, sound_cache[oldest].size + 4096);
        if (sound_cache[oldest].data != 0 && (int)sound_cache[oldest].data != (-1751672937)) {
            mc_free((int)sound_cache[oldest].data, (int)D_00176C20, 1375);
            sound_cache[oldest].data = (char *)-1751672937;
        }
        sound_cache[oldest].data = 0;
        sound_cache[oldest].id = -1;
        sound_cache_bytes -= sound_cache[oldest].size;
    }
}

void sound_cache_free_all(void)
{
    int i;

    for (i = 0; i < 256; i++) {
        if ((int)sound_cache[i].data != 0) {
            dpmi_unlock_region((int)sound_cache[i].data, sound_cache[i].size + 1024);
            if ((int)sound_cache[i].data != 0 && (int)sound_cache[i].data != (-1751672937)) {
                mc_free((int)sound_cache[i].data, (int)D_00176C20, 1392);
                sound_cache[i].data = (char *)-1751672937;
            }
            sound_cache[i].data = (char *)0;
            sound_cache[i].id = -1;
        }
    }
    sound_cache_bytes = 0;
}

void flat_animal_sound(int x, int y, int z, int archive, int record)
{
    if (archive != 201) return;
    if (rand() > 100) return;
    if (xn_math_approx_dist2d(x, z, player_object->x, player_object->z) > 768) return;
    switch ((unsigned)record) {
    return;
case 0:
case 1:
    sound_play_at_point(367, x, y, z, 100);
    return;
case 3:
case 4:
    sound_play_at_point(371, x, y, z, 100);
    return;
case 5:
case 6:
    sound_play_at_point(370, x, y, z, 100);
    return;
case 7:
case 8:
    sound_play_at_point(369, x, y, z, 100);
    return;
case 9:
case 10:
    sound_play_at_point(368, x, y, z, 100);
default:;
}
}

void model_unlink_object_cb(struct record *object)
{
    struct block *block;
    struct block_model *model;
    int i;

    switch (object->type) {
    case 6:
    case 32:
        if (*(int *)((char *)object + 71) == *(int *)scratch_190de4) *(int *)((char *)object + 71) = 0;
        return;
    case 43:
        block = &object->data.block;
        model = block->models;
        for (i = 0; block->model_count > i; i++, model++) {
            if ((int)model->model == *(int *)scratch_190de4) {
                model->model = 0;
            }
        }
        return;
    case 56:
        model = (struct block_model *)RECORD_DATA(object);
        for (i = 0; object->model_count > i; i++, model++) {
            if ((int)model->model == *(int *)scratch_190de4) {
                model->model = 0;
            }
        }
    default:;
    }
}

void model_unlink_objects(int model)
{
    *(int *)scratch_190de4 = model;
    object_foreach(location_object, (int)model_unlink_object_cb);
}

int flat_random_clutter(int environment, int building_type)
{
    if (environment == 3) return flat_table_pick((int)D_00187EC8);
    return flat_table_pick(((int)D_00187DE8) + (rand_range(0, 7) * 28));
}

void player_light_draw(void)
{
    int intensity;
    int flicker;
    int dx;
    int dz;
    int jitter_x;
    int jitter_y;
    int jitter_z;

    flicker = xn_rand_noise_2d(player_object->x ^ player_object->z, (xn_anim_ticks / 40) << 6);
    flicker >>= 3;
    flicker = 256 - flicker;
    intensity = (flicker * 192) >> 8;
    jitter_x = xn_rand_noise_2d(player_object->x ^ player_object->z, (xn_anim_ticks / 40) << 6);
    jitter_x >>= 3;
    jitter_y = xn_rand_noise_2d(player_object->x + player_object->z, (xn_anim_ticks / 40) << 6);
    jitter_y >>= 3;
    jitter_z = xn_rand_noise_2d(player_object->x - player_object->z, (xn_anim_ticks / 40) << 6);
    jitter_z >>= 3;
    xn_math_yaw_offset_xz((player_object->yaw + view_look_yaw) & 2047, 192, (int)&dx, (int)&dz);
    xn_light_add((player_object->x + dx) + (jitter_x - 16), (player_object->y - 50) + (jitter_y - 16), (player_object->z + dz) + (jitter_z - 16), 50, intensity, 0);
    xn_flat_add((player_object->x + dx) + (jitter_x - 16), (player_object->y - 50) + (jitter_y - 16), (player_object->z + dz) + (jitter_z - 16), 26883, -1, 1, 400);
}
