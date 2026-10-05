/* matched by the real Watcom C32 10.0a (-d2): a run of maplogic from 0x874C0 to 0x8795D, kept together for its switch table's alignment */
#include "records.h"
struct bits { unsigned char b0:1, b1:1, b2:1, b3:1, b4:1, b5:1, b6:1, b7:1; };
struct flags { struct bits f[4]; };
extern int xn_cam_x;
extern int xn_cam_y;
extern int xn_cam_z;
extern char D_00176C94[];
extern unsigned char player_environment;
extern unsigned char D_001940D5;
extern struct record *camera_object;
extern struct record *player_object;
extern struct record *location_object;
extern struct location *current_location;
extern unsigned char current_region;
extern unsigned char world_loading;
extern struct map_location *region_locations;
extern char terrain_cell_ids[];
extern char terrain_cell_dirty[];
extern void region_enter(unsigned char, unsigned char);
extern void automap_load(void);
extern void world_update_location(void);
extern void dungeon_load(int);
extern void location_unload(int);
extern void building_enter(struct building *);
extern int player_to_nearest_marker(struct record *, int);
extern int mc_memset();
extern int xn_world_reload();
extern int xn_terrain_height_at(int, int);

void location_place_player_at_edge(unsigned edge)
{
    switch (edge) {
    case 0:
    case 1:
        player_object->x = location_object->x + (current_location->width << 11);
        player_object->z = location_object->z - 256;
        player_object->yaw = camera_object->yaw = 0;
        break;
    case 2:
    case 3:
        player_object->x = location_object->x - 256;
        player_object->z = location_object->z + (current_location->height << 11);
        player_object->yaw = camera_object->yaw = 512;
        break;
    case 4:
    case 5:
        player_object->x = location_object->x + (current_location->width << 11);
        player_object->z = location_object->z + (current_location->height << 12) + 256;
        player_object->yaw = camera_object->yaw = 1024;
        break;
    case 6:
    case 7:
        player_object->x = location_object->x + (current_location->width << 12) + 256;
        player_object->z = location_object->z + (current_location->height << 11);
        player_object->yaw = camera_object->yaw = 1536;
        break;
    }
    if (current_location->kind == 0)
        player_to_nearest_marker(location_object->children, 8);
    player_object->y = xn_terrain_height_at(player_object->x, player_object->z);
}

void map_goto_location(int region, int environment, int location, int building)
{
    struct record *old_parent;

    old_parent = player_object->parent;
    if (current_region == region && player_environment == environment && location_object->image == location) {
        if (player_environment == 2)
            building_enter(&current_location->buildings[building]);
        else
            player_to_nearest_marker(location_object, 8);
        return;
    }
    location_unload(location_object->image);
    player_environment = environment;
    if (current_region != region)
        region_enter(current_region, region);
    switch (player_environment) {
    case 1:
        xn_cam_x = player_object->x = region_locations[location].x_type_flags & 33554431;
        xn_cam_z = player_object->z = region_locations[location].z_size & 16777215;
        xn_world_reload();
        mc_memset(terrain_cell_dirty, 0, 16, D_00176C94, 783, 16);
        mc_memset(terrain_cell_ids, 0, 16, D_00176C94, 784, 16);
        world_update_location();
        player_object->x = location_object->x;
        player_object->z = location_object->z;
        if (current_location->kind == 0)
            player_to_nearest_marker(location_object->children, 8);
        xn_cam_y = xn_terrain_height_at(player_object->x, player_object->z);
        player_object->y = xn_cam_y;
        break;
    case 2:
        xn_cam_x = player_object->x = region_locations[location].x_type_flags & 33554431;
        xn_cam_z = player_object->z = region_locations[location].z_size & 16777215;
        xn_world_reload();
        mc_memset(terrain_cell_dirty, 0, 16, D_00176C94, 803, 16);
        mc_memset(terrain_cell_ids, 0, 16, D_00176C94, 804, 16);
        world_update_location();
        world_loading++;
        building_enter(&current_location->buildings[building]);
        world_loading--;
        break;
    case 3:
        dungeon_load(location);
        automap_load();
    }
    D_001940D5 |= 2;
}

int location_has_service(struct flags *flags, int service, int subtype)
{
    switch (service) {
    case 0:
        return flags->f[1].b0;
    case 3:
        return flags->f[1].b1;
    case 5:
        return flags->f[1].b3;
    case 6:
        return flags->f[1].b4;
    case 7:
        return flags->f[1].b5;
    case 8:
        return flags->f[1].b6;
    case 10:
        return flags->f[1].b7;
    case 11:
        return flags->f[2].b0;
    case 13:
    case 14:
        switch (subtype) {
        case -1:
            return *(unsigned char *)flags > 0 ? 1 : 0;
        case 0:
        case 26:
            return flags->f[0].b0;
        case 1:
        case 21:
            return flags->f[0].b1;
        case 2:
        case 29:
            return flags->f[0].b2;
        case 3:
        case 27:
            return flags->f[0].b3;
        case 4:
        case 35:
            return flags->f[0].b4;
        case 5:
        case 24:
            return flags->f[0].b5;
        case 6:
        case 33:
            return flags->f[0].b6;
        case 7:
        case 22:
            return flags->f[0].b7;
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
        return flags->f[2].b1;
    case 36:
        return flags->f[2].b2;
    case 37:
        return flags->f[2].b3;
    case 38:
        return flags->f[2].b4;
    case 39:
        return flags->f[2].b5;
    case 15:
        return flags->f[2].b7;
    case 12:
        return flags->f[2].b6;
    case 1:
        return flags->f[3].b0;
    }
    return 0;
}
