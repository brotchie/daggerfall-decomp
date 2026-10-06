/* matched by the real Watcom C32 10.0a (-d2): func_0007E815 has a switch table, which 10.0a
 * aligns to 4 bytes from the start of the code segment, so it is compiled with the run of its
 * unit's functions from building_access_level (the nearest one at a multiple of 4) */
#include "records.h"

extern char D_00176A10[];
extern struct record *location_grid[];         /* 32 x 32 grid of object lists */
extern struct record *D_00194064[];
extern struct record *cart_overlay_image[];
extern struct record *D_001940E8[];
extern struct record *D_00194164[];
extern struct record *D_00194168[];
extern struct record *player_object;
extern struct record *location_object;
extern int game_minutes;
extern void (*grid_visit_func)(struct record *, void (*)());
extern int location_grid_x;
extern int location_grid_z;
extern struct building *object_building(struct record *);
extern int mc_memset();
extern int xn_math_approx_dist2d();
extern int xn_math_approx_hypot();

int building_access_level(struct building *building)
{
    struct building *target;

    if (building == 0) {
        target = object_building(player_object);
    } else {
        target = building;
    }
    if (target == 0) return 0;
    if (((int)(unsigned char)(target->flags & 1)) != 0 && target->rent_expires > game_minutes) {
        return target->access_level;
    }
    return 0;
}

void building_grant_access(struct building *building, unsigned char access_level, int expires)
{
    struct building *target;

    target = building ? building : object_building(player_object);
    if (building == 0) return;
    if (building->type == 15) return;
    target->access_level = access_level;
    target->flags &= 248;
    target->flags |= 1;
    target->rent_expires = expires;
}

int func_0007E441(int mode)
{
    struct block *block;
    struct block_section3 *entry;
    struct record *object;
    int dx;
    int dy;
    int dz;
    int i;
    int best_distance;
    int result;
    int distance;
    int shift;

    object = player_object->parent;
    while (object->type != 1 && ((int)(unsigned short)(object->flags & 1)) == 0) object = object->parent;
    if (object->type != 43) return 0;
    dx = player_object->x - object->x;
    dy = player_object->y - object->y;
    dz = player_object->z - object->z;
    shift = ((mode == 2) ? 8 : 0);
    object = object->children;
    while (object->type != 43) object = object->next;
    block = &object->data.block;
    entry = block->section3;
    best_distance = 100000;
    result = 0;
    for (i = 0; block->section3_count > i; i++, entry++) {
        distance = xn_math_approx_hypot(entry->y - dy, xn_math_approx_dist2d(dx, dz, entry->x, entry->z));
        if (distance < best_distance) {
            best_distance = distance;
            result = (unsigned char)(entry->data >> shift);
        }
    }
    return result;
}

void dungeon_grid_build(void)
{
    int offset_x;
    int offset_z;
    struct record *block;
    int min_x;
    int min_z;

    block = location_object->children;
    min_x = 10000;
    min_z = 10000;
    mc_memset((iptr)location_grid, 0, 4096, (iptr)D_00176A10, 849, 4096);
    while (block != 0) {
        if (block->type == 47) {
            if ((block->x - location_object->x) < min_x) min_x = block->x - location_object->x;
            if ((block->z - location_object->z) < min_z) min_z = block->z - location_object->z;
        }
        block = block->next;
    }
    if (min_x < 0) {
        offset_x = -(min_x);
    } else {
        offset_x = 0;
    }
    location_grid_x = offset_x;
    if (min_z < 0) {
        offset_z = -(min_z);
    } else {
        offset_z = 0;
    }
    location_grid_z = offset_z;
    block = location_object->children;
    while (block != 0) {
        if (block->type == 47) {
            location_grid[(((int)(iptr)(((char *)(iptr)location_grid_x) + (block->x - location_object->x)) / 1024) + (((int)(iptr)(((char *)(iptr)location_grid_z) + (block->z - location_object->z)) / 1024) << 5))] = block;
        }
        block = block->next;
    }
}

void town_grid_build(void)
{
    struct record *block;
    int min_x;
    int min_z;

    block = location_object->children;
    min_x = 10000;
    min_z = 10000;
    mc_memset((iptr)location_grid, 0, 4096, (iptr)D_00176A10, 880, 4096);
    block = location_object->children;
    while (block != 0) {
        if (block->type == 38) {
            location_grid[(((block->x - location_object->x) / 4096) + (((block->z - location_object->z) / 4096) << 5))] = block;
        }
        block = block->next;
    }
}

void func_0007E815(struct record *object, void (*callback)())
{
    int cell_x;
    int cell_z;
    int quadrant;
    int base_cell;
    int cell;

    cell_x = (object->x - location_object->x + location_grid_x) / 1024;
    cell_z = (object->z - location_object->z + location_grid_z) / 1024;
    cell = cell_x + (cell_z << 5);
    quadrant = (cell_x & 1) + (cell_z & 1) * 2;
    base_cell = ((cell_z & -2) << 5) + (cell_x & -2);
    if (location_grid[base_cell] != 0)
        grid_visit_func(location_grid[base_cell]->children, callback);
    if (D_001940E8[base_cell] != 0)
        grid_visit_func(D_001940E8[base_cell]->children, callback);
    if (D_00194164[base_cell] != 0)
        grid_visit_func(D_00194164[base_cell]->children, callback);
    if (D_00194168[base_cell] != 0)
        grid_visit_func(D_00194168[base_cell]->children, callback);
    switch (quadrant) {
    case 0:
        if (cell != 0 && cart_overlay_image[cell] != 0)
            grid_visit_func(cart_overlay_image[cell]->children, callback);
        if (cell > 31 && D_00194064[cell] != 0)
            grid_visit_func(D_00194064[cell]->children, callback);
        break;
    case 1:
        if ((cell & 31) < 31 && D_001940E8[cell] != 0)
            grid_visit_func(D_001940E8[cell]->children, callback);
        if (cell > 31 && D_00194064[cell] != 0)
            grid_visit_func(D_00194064[cell]->children, callback);
        break;
    case 2:
        if ((cell & 31) != 0 && cart_overlay_image[cell] != 0)
            grid_visit_func(cart_overlay_image[cell]->children, callback);
        if (cell < 992 && D_00194164[cell] != 0)
            grid_visit_func(D_00194164[cell]->children, callback);
        break;
    case 3:
        if ((cell & 31) < 31 && D_001940E8[cell] != 0)
            grid_visit_func(D_001940E8[cell]->children, callback);
        if (cell < 992 && D_00194164[cell] != 0)
            grid_visit_func(D_00194164[cell]->children, callback);
        break;
    }
}
