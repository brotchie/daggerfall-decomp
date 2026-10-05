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
extern void (*grid_visit_func)(struct record *, int);
extern int location_grid_x;
extern int location_grid_z;
extern struct building *object_building(struct record *);
extern int mc_memset();
extern int xn_math_approx_dist2d();
extern int xn_math_approx_hypot();

int building_access_level(struct building *a1)
{
    struct building *l_1C;

    if (a1 == 0) {
        l_1C = object_building(player_object);
    } else {
        l_1C = a1;
    }
    if (l_1C == 0) return 0;
    if (((int)(unsigned char)(l_1C->flags & 1)) != 0 && l_1C->rent_expires > game_minutes) {
        return l_1C->access_level;
    }
    return 0;
}

void building_grant_access(struct building *a1, unsigned char a2, int a3)
{
    struct building *l_18;

    l_18 = a1 ? a1 : object_building(player_object);
    if (a1 == 0) return;
    if (a1->type == 15) return;
    l_18->access_level = a2;
    l_18->flags &= 248;
    l_18->flags |= 1;
    l_18->rent_expires = a3;
}

int func_0007E441(int a1)
{
    int l_44;
    int l_40;
    struct record *l_3C;
    int l_38;
    int l_34;
    int l_30;
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;

    l_3C = player_object->parent;
    while (l_3C->type != 1 && ((int)(unsigned short)(l_3C->flags & 1)) == 0) l_3C = l_3C->parent;
    if (l_3C->type != 43) return 0;
    l_38 = player_object->x - l_3C->x;
    l_34 = player_object->y - l_3C->y;
    l_30 = player_object->z - l_3C->z;
    l_1C = ((a1 == 2) ? 8 : 0);
    l_3C = l_3C->children;
    while (l_3C->type != 43) l_3C = l_3C->next;
    l_44 = (int)RECORD_DATA(l_3C);
    l_40 = *(int *)((char *)l_44 + 13);
    l_28 = 100000;
    l_24 = 0;
    for (l_2C = 0; ((int)(unsigned char)*(signed char *)((char *)l_44 + 2)) > l_2C; l_2C++, (*(char (**)[16])&l_40)++) {
        l_20 = xn_math_approx_hypot(*(int *)((char *)l_40 + 4) - l_34, xn_math_approx_dist2d(l_38, l_30, *(int *)((char *)l_40), *(int *)((char *)l_40 + 8)));
        if (l_20 < l_28) {
            l_28 = l_20;
            l_24 = (int)(unsigned char)(*(int *)((char *)l_40 + 12) >> l_1C);
        }
    }
    return l_24;
}

void dungeon_grid_build(void)
{
    int l_28;
    int l_24;
    struct record *l_20;
    int l_1C;
    int l_18;

    l_20 = location_object->children;
    l_1C = 10000;
    l_18 = 10000;
    mc_memset((int)location_grid, 0, 4096, (int)D_00176A10, 849, 4096);
    while (l_20 != 0) {
        if (l_20->type == 47) {
            if ((l_20->x - location_object->x) < l_1C) l_1C = l_20->x - location_object->x;
            if ((l_20->z - location_object->z) < l_18) l_18 = l_20->z - location_object->z;
        }
        l_20 = l_20->next;
    }
    if (l_1C < 0) {
        l_28 = -(l_1C);
    } else {
        l_28 = 0;
    }
    location_grid_x = l_28;
    if (l_18 < 0) {
        l_24 = -(l_18);
    } else {
        l_24 = 0;
    }
    location_grid_z = l_24;
    l_20 = location_object->children;
    while (l_20 != 0) {
        if (l_20->type == 47) {
            location_grid[(((int)(((char *)location_grid_x) + (l_20->x - location_object->x)) / 1024) + (((int)(((char *)location_grid_z) + (l_20->z - location_object->z)) / 1024) << 5))] = l_20;
        }
        l_20 = l_20->next;
    }
}

void town_grid_build(void)
{
    struct record *l_20;
    int l_1C;
    int l_18;

    l_20 = location_object->children;
    l_1C = 10000;
    l_18 = 10000;
    mc_memset((int)location_grid, 0, 4096, (int)D_00176A10, 880, 4096);
    l_20 = location_object->children;
    while (l_20 != 0) {
        if (l_20->type == 38) {
            location_grid[(((l_20->x - location_object->x) / 4096) + (((l_20->z - location_object->z) / 4096) << 5))] = l_20;
        }
        l_20 = l_20->next;
    }
}

void func_0007E815(struct record *a1, int a2)
{
    int l_24;
    int l_20;
    int l_1C;
    int l_18;
    int l_14;

    l_24 = (a1->x - location_object->x + location_grid_x) / 1024;
    l_20 = (a1->z - location_object->z + location_grid_z) / 1024;
    l_14 = l_24 + (l_20 << 5);
    l_1C = (l_24 & 1) + (l_20 & 1) * 2;
    l_18 = ((l_20 & -2) << 5) + (l_24 & -2);
    if (location_grid[l_18] != 0)
        grid_visit_func(location_grid[l_18]->children, a2);
    if (D_001940E8[l_18] != 0)
        grid_visit_func(D_001940E8[l_18]->children, a2);
    if (D_00194164[l_18] != 0)
        grid_visit_func(D_00194164[l_18]->children, a2);
    if (D_00194168[l_18] != 0)
        grid_visit_func(D_00194168[l_18]->children, a2);
    switch (l_1C) {
    case 0:
        if (l_14 != 0 && cart_overlay_image[l_14] != 0)
            grid_visit_func(cart_overlay_image[l_14]->children, a2);
        if (l_14 > 31 && D_00194064[l_14] != 0)
            grid_visit_func(D_00194064[l_14]->children, a2);
        break;
    case 1:
        if ((l_14 & 31) < 31 && D_001940E8[l_14] != 0)
            grid_visit_func(D_001940E8[l_14]->children, a2);
        if (l_14 > 31 && D_00194064[l_14] != 0)
            grid_visit_func(D_00194064[l_14]->children, a2);
        break;
    case 2:
        if ((l_14 & 31) != 0 && cart_overlay_image[l_14] != 0)
            grid_visit_func(cart_overlay_image[l_14]->children, a2);
        if (l_14 < 992 && D_00194164[l_14] != 0)
            grid_visit_func(D_00194164[l_14]->children, a2);
        break;
    case 3:
        if ((l_14 & 31) < 31 && D_001940E8[l_14] != 0)
            grid_visit_func(D_001940E8[l_14]->children, a2);
        if (l_14 < 992 && D_00194164[l_14] != 0)
            grid_visit_func(D_00194164[l_14]->children, a2);
        break;
    }
}
