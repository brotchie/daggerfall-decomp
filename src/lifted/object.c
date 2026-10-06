/* object.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"
#include "clib.h"

extern char D_00176E44[];
extern char D_00176E4D[];
extern char D_00176E70[];
extern int next_record_id;
extern char scratch_190be4[];
extern signed char scratch_190ce4[];
extern struct record *nonworld_root;
extern struct record *player_object;
extern struct record *location_object;
extern struct record *found_object;
extern struct location *current_location;
extern struct mem_pool object_heap;
extern struct loaded_location loaded_location;
extern iptr object_found_last;
extern int D_001A3F94;
extern int object_move_new;
extern int D_001A9AF8;
extern int D_001A9AFC;
extern int D_001A9B00;
extern int D_001A9B04;
extern int D_001A9B08;
extern iptr object_debug_watch;
extern int object_delete_block_id;
extern struct record *object_search_result;
extern iptr object_search_id;
extern int object_move_old_angles;
extern int D_001A9B20;
extern int D_001A9B24;
extern int object_move_old_z;
extern int object_move_old_x;
extern int object_move_old_y;
extern int object_heap_size;
extern int object_heap_free;
extern short object_count_result;
extern short D_001A9B40;
extern short D_001A9B42;
extern char object_debug_watch_copy[];
extern char potion_ingredient_scroll[];
extern char potion_ingredient_count[];

extern struct location_door *location_find_door(int);
extern iptr mem_pool_alloc(iptr, int);
extern int mem_pool_release(iptr);
extern int object_count_type(struct record *, short);
extern int xn_math_approx_dist2d(int, int, int, int);
extern void unequip_object(struct record *);
extern void fatal_error(char *);
extern void mem_pool_init(iptr, int);
extern void mem_pool_free(iptr);
extern void object_follow_move_cb(struct record *);
struct record *object_free_single(struct record *);
struct record *object_delete(struct record *);
struct record *object_alloc(struct record *, struct record *, int);
struct record *object_create_child(struct record *, struct record *, int);
int object_find(struct record *, iptr (*)(struct record *));
iptr object_find_type_cb(struct record *);
iptr object_find_by_id_cb(struct record *);
struct record *object_find_by_id(struct record *, iptr);
iptr object_random_type_cb(struct record *);
int object_new_id(int);
iptr object_find_quest_cb(struct record *);
void object_free_node(struct record *);
void object_free_children(struct record *);
void object_heap_release(struct record *);
void object_set_position(struct record *, int, int, int, int, int, int);
void object_unlink(struct record *);
void object_insert_after(struct record *, struct record *);
void object_add_child(struct record *, struct record *);
void object_foreach_pre(struct record *, void (*)(struct record *));
void object_foreach_post(struct record *, void (*)(struct record *));
void object_foreach(struct record *, void (*)(struct record *));
void object_delete_block_cb(struct record *);
void func_0008EB25(struct record *);
void object_delete_quest_cb(struct record *);
void object_tree_size_cb(struct record *);
void object_delete_type_cb(struct record *);
#pragma aux mc_set_location parm routine [];

void object_heap_init(void)
{
    if (object_heap_size == 0) {
        if (D_001A3F94 < 13000) {
            object_heap_size = NATIVE_HEAP_BYTES(1280000);
        } else {
            object_heap_size = NATIVE_HEAP_BYTES(2048000);
        }
    }
    object_heap_free = object_heap_size;
    mc_set_location(55, D_00176E44);
    func_000A148C(D_00176E4D, object_heap_size);
    mem_pool_init((iptr)&object_heap, object_heap_size);
    (location_object = object_alloc(0, 0, REC_SIZEOF(struct location)))->type = 1;
    location_object->image = 65535;
    location_object->id = -65535;
    (current_location = &location_object->data.location)->buildings = 0;
    (nonworld_root = object_alloc(0, 0, 0))->type = 39;
    nonworld_root->id = 700;
}

void object_heap_shutdown(void)
{
    mem_pool_free((iptr)&object_heap);
}

void object_free_node(struct record *object)
{
    if (object->twin != 0) object->twin->twin = 0;
    object_unlink(object);
    object_heap_release(object);
}

void object_free_children(struct record *object)
{
    if (object == 0) return;
    object_foreach_post(object->children, object_free_node);
}

struct record *object_free_single(struct record *object)
{
    struct record *next;

    if (object == 0) return 0;
    next = object->next;
    object_free_node(object);
    return next;
}

struct record *object_delete(struct record *object)
{
    struct record *next;

    next = object->next;
    if (object == 0) return 0;
    object_free_children(object);
    object_free_single(object);
    return next;
}

struct record *object_detach(struct record *object)
{
    struct record *parent;

    parent = object->parent;
    if (object == 0) return 0;
    object_unlink(object);
    return parent;
}

struct record *object_alloc(struct record *after, struct record *source, int data_size)
{
    int size;
    struct record *object;

    size = data_size + RECORD_HEADER_SIZE;
    object_heap_free -= size + MEM_BLOCK_HEADER_SIZE;
    object = (struct record *)mem_pool_alloc((iptr)&object_heap, size);
    if (object == 0) fatal_error(D_00176E70);
    if (source != 0) {
        mc_memcpy(object, source, RECORD_LINKS_OFFSET, D_00176E44, 163, 4);
        mc_memcpy(&object->data, &source->data, size - RECORD_HEADER_SIZE, D_00176E44, 164, 4);
        mc_memset(&object->next, 0, RECORD_LINKS_SIZE, D_00176E44, 165, 4);
    } else {
        mc_memset(object, 0, size, D_00176E44, 169, 4);
        object->id = object_new_id(1);
    }
    if (after != 0) object_insert_after(after, object);
    return object;
}

void object_heap_release(struct record *object)
{
    struct mem_block *block;

    block = (struct mem_block *)((char *)object - MEM_BLOCK_HEADER_SIZE);
    object_heap_free += block->size + MEM_BLOCK_HEADER_SIZE;
    mem_pool_release((iptr)object);
}

struct record *object_clone(struct record *object)
{
    struct mem_block *block;
    struct record *clone;
    int size;

    block = (struct mem_block *)((char *)object - MEM_BLOCK_HEADER_SIZE);
    size = block->size;
    clone = object_create_child(object->parent, 0, size - RECORD_HEADER_SIZE);
    mc_memcpy(clone, object, RECORD_LINKS_OFFSET, D_00176E44, 203, 4);
    mc_memcpy(&clone->data, &object->data, size - RECORD_HEADER_SIZE, D_00176E44, 204, 4);
    return clone;
}

struct record *object_create_child(struct record *parent, struct record *source, int data_size)
{
    struct record *child;
    struct record *sibling;

    child = object_alloc(parent->children, source, data_size);
    child->parent = (struct record *)parent;
    sibling = child;
    while (sibling != 0) {
        parent->children = (struct record *)sibling;
        sibling = sibling->prev;
    }
    return child;
}

struct record *object_reparent(struct record *parent, struct record *object)
{
    object_unlink(object);
    object_add_child(parent, object);
    return object;
}

void object_set_position(struct record *object, int x, int y, int z, int angle_x, int yaw, int angle_z)
{
    int unused;

    object_move_old_x = object->x;
    object_move_old_y = object->y;
    object_move_old_z = object->z;
    object_move_old_angles = object->angle_x;
    D_001A9B20 = object->yaw;
    D_001A9B24 = object->angle_z;
    object->x = x;
    object->y = y;
    object->z = z;
    object->angle_x = angle_x;
    object->yaw = yaw;
    object->angle_z = angle_z;
    object_move_new = x;
    D_001A9AF8 = y;
    D_001A9AFC = z;
    D_001A9B00 = angle_x;
    D_001A9B04 = yaw;
    D_001A9B08 = angle_z;
    if (object->type == 43) return;
    object_foreach(object->children, object_follow_move_cb);
}

void object_move_by(struct record *object, int dx, int dy, int dz, int d_angle_x, int d_yaw, int d_angle_z)
{
    object_set_position(object, object->x + dx, object->y + dy, object->z + dz, object->angle_x + d_angle_x, object->yaw + d_yaw, object->angle_z + d_angle_z);
}

void object_unlink(struct record *object)
{
    if (object->parent != 0 && object->parent->children == object) object->parent->children = object->next;
    if (object->prev != 0) object->prev->next = object->next;
    if (object->next != 0) object->next->prev = object->prev;
    object->next = 0;
    object->prev = object->next;
    object->parent = object->prev;
}

void object_insert_after(struct record *after, struct record *object)
{
    object->next = after->next;
    if (after->next != 0) after->next->prev = (struct record *)object;
    after->next = (struct record *)object;
    object->prev = (struct record *)after;
    object->parent = after->parent;
}

void object_add_child(struct record *parent, struct record *child)
{
    if (parent->children != 0) {
        object_insert_after(parent->children, child);
        return;
    }
    parent->children = (struct record *)child;
    child->parent = (struct record *)parent;
    child->prev = 0;
    child->next = child->prev;
}

void object_swap_siblings(struct record *first, struct record *second)
{
    struct record *next;
    struct record *prev;

    if (first->next != second && second->next != first) {
        next = second->next;
        prev = second->prev;
        if (first->prev != 0) first->prev->next = (struct record *)second;
        second->next = first->next;
        second->prev = first->prev;
        if (second->next != 0) second->next->prev = (struct record *)second;
        if (second->prev != 0) second->prev->next = (struct record *)first;
        first->next = (struct record *)next;
        first->prev = (struct record *)prev;
        if (first->next != 0) first->next->prev = (struct record *)first;
    } else {
        if (second->next == first) {
            next = first;
            first = second;
            second = next;
        }
        if (first->prev != 0) first->prev->next = (struct record *)second;
        if (second->next != 0) second->next->prev = (struct record *)first;
        second->prev = first->prev;
        first->next = second->next;
        first->prev = (struct record *)second;
        second->next = (struct record *)first;
    }
    if (first->prev == 0 && first->parent != 0) first->parent->children = (struct record *)first;
    if (second->prev != 0 || second->parent == 0) return;
    second->parent->children = (struct record *)second;
}

void object_foreach_near_player(struct record *object, void (*callback)(struct record *), int max_dist)
{
    while (object != 0) {
        if (xn_math_approx_dist2d(object->x, object->z, player_object->x, player_object->z) < max_dist) {
            callback(object);
            if (object->children != 0 && (object->flags & 1) == 0) object_foreach_near_player(object->children, callback, max_dist);
        }
        object = object->next;
    }
}

void object_foreach_pre(struct record *object, void (*callback)(struct record *))
{
    struct record *next;

    while (object != 0) {
        next = object->next;
        callback(object);
        if (object->children != 0) object_foreach_pre(object->children, callback);
        object = next;
    }
}

void object_foreach_post(struct record *object, void (*callback)(struct record *))
{
    struct record *next;

    while (object != 0) {
        next = object->next;
        if (object->children != 0) object_foreach_post(object->children, callback);
        callback(object);
        object = next;
    }
}

void object_foreach(struct record *object, void (*callback)(struct record *))
{
    struct record *next;

    while (object != 0) {
        next = object->next;
        if (object->children != 0) object_foreach(object->children, callback);
        callback(object);
        object = next;
    }
}

void object_foreach_skip_player(struct record *object, void (*callback)(struct record *))
{
    struct record *next;

    while (object != 0) {
        next = object->next;
        if (object->children != 0 && object->type != 4) object_foreach_skip_player(object->children, callback);
        callback(object);
        object = next;
    }
}

void object_foreach_open(struct record *object, void (*callback)(struct record *))
{
    while (object != 0) {
        callback(object);
        if (object->children != 0 && (object->flags & 1) == 0) object_foreach_open(object->children, callback);
        object = object->next;
    }
}

void object_foreach_until(struct record *object, int (*callback)(struct record *))
{
    struct record *next;

    while (object != 0) {
        next = object->next;
        if (callback(object) == 0 && object->children != 0) object_foreach_until(object->children, callback);
        object = next;
    }
}

int object_find(struct record *object, iptr (*callback)(struct record *))
{
    while (object != 0) {
        if (callback(object) != 0) return 1;
        if (object_find(object->children, callback) != 0) return 1;
        object = object->next;
    }
    return 0;
}

int object_find_open(struct record *object, iptr (*callback)(struct record *))
{
    while (object != 0) {
        if (callback(object) != 0) return 1;
        if (object->children != 0 && (object->flags & 1) == 0) {
            if (object_find(object->children, callback) != 0) return 1;
        }
        object = object->next;
    }
    return 0;
}

void object_find_item_cb(struct record *object)
{
    struct item *item;

    if (((int)(short)D_001A9B42) == (-1) || object->type != 2) return;
    item = &object->data.item;
    if (item->group != D_001A9B40 || item->index != D_001A9B42) return;
    D_001A9B42 = 65535;
    found_object = object;
}

iptr object_find_type_cb(struct record *object)
{
    if ((short)object->type != D_001A9B42) return 0;
    found_object = object;
    return 1;
}

struct record *object_find_type(struct record *root, int type)
{
    found_object = 0;
    D_001A9B42 = type;
    object_find(root, object_find_type_cb);
    return found_object;
}

void object_count_type_cb(struct record *object)
{
    if ((short)object->type != D_001A9B42) return;
    object_count_result++;
}

struct record *object_create_in_block(struct record *parent, int type, int data_size, int image, int pad13)
{
    struct record *object;

    if (parent != 0) {
        object = object_create_child(parent, 0, data_size);
    } else {
        object = object_alloc(0, 0, data_size);
    }
    object->type = type;
    object->image = image;
    object->pad13 = pad13;
    object->id = location_object->id + ((int)(unsigned short)(current_location->object_counter)++);
    if (object->id == (-1016397758)) {
        mc_memcpy(object_debug_watch_copy, object, RECORD_HEADER_SIZE, D_00176E44, 634, 4);
        object_debug_watch = (iptr)object;
    }
    return object;
}

iptr object_find_by_id_cb(struct record *object)
{
    if (object->id == object_search_id) object_search_result = object;
    return (iptr)object_search_result;
}

struct record *object_find_by_id(struct record *root, iptr id)
{
    object_search_id = id;
    object_search_result = 0;
    if (root == 0) {
        object_foreach(location_object, (void (*)())object_find_by_id_cb);
        object_found_last = (iptr)object_search_result;
        if ((iptr)object_search_result != 0) return object_search_result;
        object_foreach(nonworld_root, (void (*)())object_find_by_id_cb);
        object_found_last = (iptr)object_search_result;
        return object_search_result;
    }
    object_find(root, object_find_by_id_cb);
    return object_search_result;
}

iptr object_random_type_cb(struct record *object)
{
    if (object->type != scratch_190ce4[0]) return 0;
    if (*(int *)scratch_190be4 == 0) {
        found_object = object;
        return 1;
    }
    (*(int *)scratch_190be4)--;
    return 0;
}

struct record *object_random_child_of_type(struct record *parent, int type)
{
    if ((*(int *)scratch_190be4 = object_count_type(parent->children, (int)(short)*(short *)&type)) == 0) {
        return 0;
    }
    found_object = 0;
    *(int *)scratch_190be4 = rand() % *(int *)scratch_190be4;
    scratch_190ce4[0] = *(signed char *)&type;
    object_find(parent->children, object_random_type_cb);
    return found_object;
}

void object_delete_block_cb(struct record *object)
{
    if ((object->id & -65536) != object_delete_block_id) return;
    if (object->twin != 0) object->twin->twin = 0;
    object_free_single(object);
}

void object_delete_block(struct record *root, int block_id)
{
    object_delete_block_id = block_id & -65536;
    object_foreach_post(root, object_delete_block_cb);
}

void func_0008EB25(struct record *object)
{
    struct record *twin;

    if (object->twin == 0) return;
    twin = object->twin;
}

void func_0008EB52(void)
{
    object_foreach_pre(location_object, func_0008EB25);
    object_foreach_pre(nonworld_root, func_0008EB25);
}

int object_new_id(int id_high)
{
    int id;

    if (next_record_id >= 63000) next_record_id = 10000;
    while (object_find_by_id(location_object, (id = (id_high << 16) + next_record_id++)) != 0 || object_find_by_id(nonworld_root, id) != 0) {
    }
    return id;
}

void object_delete_quest_cb(struct record *object)
{
    struct location_door *door;

    if (object->quest_id != scratch_190ce4[0]) return;
    if (object->type == 8) {
        if (((int)(unsigned char)(object->data.person.flags & 128)) != 0) {
            object->twin->twin = 0;
            object->twin = 0;
            object->quest_id = 0;
            return;
        }
    }
    if (loaded_location.door_count != 0 && (object->id & -65536) == (location_object->id & -65536)) {
        door = location_find_door(object->id);
        if (door != 0) door->flags &= 0xFFF;
    }
    unequip_object(object);
    object_delete(object);
}

void object_delete_quest_objects(struct record *root, unsigned char quest_id)
{
    scratch_190ce4[0] = quest_id;
    object_foreach_post(root, object_delete_quest_cb);
}

void object_tree_size_cb(struct record *object)
{
    *(int *)scratch_190be4 += ((struct mem_block *)((char *)object - MEM_BLOCK_HEADER_SIZE))->size;
}

int object_tree_size(struct record *root)
{
    *(int *)scratch_190be4 = 0;
    object_foreach(root, object_tree_size_cb);
    return *(int *)scratch_190be4;
}

void object_delete_type_cb(struct record *object)
{
    if (object->type != scratch_190ce4[0]) return;
    object_delete(object);
}

void object_delete_type(struct record *root, unsigned char type)
{
    scratch_190ce4[0] = type;
    object_foreach_post(root, object_delete_type_cb);
}

iptr object_find_quest_cb(struct record *object)
{
    if (object->quest_id == scratch_190ce4[0]) return (iptr)(found_object = object);
    return 0;
}

struct record *object_find_quest(struct record *root, unsigned char quest_id)
{
    scratch_190ce4[0] = quest_id;
    found_object = 0;
    object_find(root, object_find_quest_cb);
    return found_object;
}

int potionmaker_scroll_up(void)
{
    if (*(int *)potion_ingredient_scroll != 0) *(int *)potion_ingredient_scroll -= 3;
    return 0;
}

int potionmaker_scroll_down(void)
{
    if ((*(int *)potion_ingredient_count - *(int *)potion_ingredient_scroll) <= 12) return 0;
    *(int *)potion_ingredient_scroll += 3;
    return 0;
}
