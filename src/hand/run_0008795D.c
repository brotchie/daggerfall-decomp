/* matched by the real Watcom C32 10.0a (-d2): a run of maplogic from 0x874C0 to 0x8795D, kept together for its switch table's alignment */
#include "records.h"
struct bits { unsigned char b0:1, b1:1, b2:1, b3:1, b4:1, b5:1, b6:1, b7:1; };
struct flags { struct bits f[4]; };
extern int D_000C23C4;
extern int D_000C23C8;
extern int D_000C23CC;
extern char D_00176C94[];
extern unsigned char player_environment;
extern unsigned char D_001940D5;
extern struct record *camera_object;
extern struct record *player_object;
extern struct record *D_00195AC4;
extern struct location *current_location;
extern unsigned char current_region;
extern unsigned char D_00196289;
extern struct map_location *D_00196A9C;
extern char D_001A94A0[];
extern char D_001A94B0[];
extern void region_enter(unsigned char, unsigned char);
extern void automap_load(void);
extern void world_update_location(void);
extern void dungeon_load(int);
extern void location_unload(unsigned short);
extern void building_enter(struct building *);
extern void player_to_nearest_marker(struct record *, int);
extern int mc_memset();
extern int func_000C2FF5();
extern int func_0014B45B(int, int);

void location_place_player_at_edge(unsigned a1)
{
    switch (a1) {
    case 0:
    case 1:
        player_object->x = D_00195AC4->x + (current_location->width << 11);
        player_object->z = D_00195AC4->z - 256;
        player_object->yaw = camera_object->yaw = 0;
        break;
    case 2:
    case 3:
        player_object->x = D_00195AC4->x - 256;
        player_object->z = D_00195AC4->z + (current_location->height << 11);
        player_object->yaw = camera_object->yaw = 512;
        break;
    case 4:
    case 5:
        player_object->x = D_00195AC4->x + (current_location->width << 11);
        player_object->z = D_00195AC4->z + (current_location->height << 12) + 256;
        player_object->yaw = camera_object->yaw = 1024;
        break;
    case 6:
    case 7:
        player_object->x = D_00195AC4->x + (current_location->width << 12) + 256;
        player_object->z = D_00195AC4->z + (current_location->height << 11);
        player_object->yaw = camera_object->yaw = 1536;
        break;
    }
    if (current_location->kind == 0)
        player_to_nearest_marker(D_00195AC4->children, 8);
    player_object->y = func_0014B45B(player_object->x, player_object->z);
}

void map_goto_location(int a1, int a2, int a3, int a4)
{
    struct record *l_C;

    l_C = player_object->parent;
    if (current_region == a1 && player_environment == a2 && D_00195AC4->image == a3) {
        if (player_environment == 2)
            building_enter(&current_location->buildings[a4]);
        else
            player_to_nearest_marker(D_00195AC4, 8);
        return;
    }
    location_unload(D_00195AC4->image);
    player_environment = a2;
    if (current_region != a1)
        region_enter(current_region, a1);
    switch (player_environment) {
    case 1:
        D_000C23C4 = player_object->x = D_00196A9C[a3].x_type_flags & 33554431;
        D_000C23CC = player_object->z = D_00196A9C[a3].y_size & 16777215;
        func_000C2FF5();
        mc_memset(D_001A94B0, 0, 16, D_00176C94, 783, 16);
        mc_memset(D_001A94A0, 0, 16, D_00176C94, 784, 16);
        world_update_location();
        player_object->x = D_00195AC4->x;
        player_object->z = D_00195AC4->z;
        if (current_location->kind == 0)
            player_to_nearest_marker(D_00195AC4->children, 8);
        D_000C23C8 = func_0014B45B(player_object->x, player_object->z);
        player_object->y = D_000C23C8;
        break;
    case 2:
        D_000C23C4 = player_object->x = D_00196A9C[a3].x_type_flags & 33554431;
        D_000C23CC = player_object->z = D_00196A9C[a3].y_size & 16777215;
        func_000C2FF5();
        mc_memset(D_001A94B0, 0, 16, D_00176C94, 803, 16);
        mc_memset(D_001A94A0, 0, 16, D_00176C94, 804, 16);
        world_update_location();
        D_00196289++;
        building_enter(&current_location->buildings[a4]);
        D_00196289--;
        break;
    case 3:
        dungeon_load(a3);
        automap_load();
    }
    D_001940D5 |= 2;
}

int func_0008795D(struct flags *a1, int a2, int a3)
{
    switch (a2) {
    case 0:
        return a1->f[1].b0;
    case 3:
        return a1->f[1].b1;
    case 5:
        return a1->f[1].b3;
    case 6:
        return a1->f[1].b4;
    case 7:
        return a1->f[1].b5;
    case 8:
        return a1->f[1].b6;
    case 10:
        return a1->f[1].b7;
    case 11:
        return a1->f[2].b0;
    case 13:
    case 14:
        switch (a3) {
        case -1:
            return *(unsigned char *)a1 > 0 ? 1 : 0;
        case 0:
        case 26:
            return a1->f[0].b0;
        case 1:
        case 21:
            return a1->f[0].b1;
        case 2:
        case 29:
            return a1->f[0].b2;
        case 3:
        case 27:
            return a1->f[0].b3;
        case 4:
        case 35:
            return a1->f[0].b4;
        case 5:
        case 24:
            return a1->f[0].b5;
        case 6:
        case 33:
            return a1->f[0].b6;
        case 7:
        case 22:
            return a1->f[0].b7;
        }
        break;
    case 25:
    case 26:
    case 27:
    case 28:
    case 29:
    case 30:
    case 31:
    case 32:
    case 33:
    case 34:
    case 35:
        return a1->f[2].b1;
    case 36:
        return a1->f[2].b2;
    case 37:
        return a1->f[2].b3;
    case 38:
        return a1->f[2].b4;
    case 39:
        return a1->f[2].b5;
    case 15:
        return a1->f[2].b7;
    case 12:
        return a1->f[2].b6;
    case 1:
        return a1->f[3].b0;
    }
    return 0;
}
