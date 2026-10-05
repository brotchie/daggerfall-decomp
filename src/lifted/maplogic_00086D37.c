/* maplogic.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern int dungeon_water_level;
extern char D_00176C94[];
extern unsigned char player_environment;
extern int D_00187F28;
extern int D_00187F2C;
extern struct record *creature_list[];
extern struct record *camera_object;
extern struct record *player_entity;
extern struct record *player_object;
extern struct record *location_object;
extern int creature_count;
extern struct location *current_location;
extern int D_00195DB8;
extern char saved_player_object[];
extern struct loaded_location loaded_location;
extern signed char model_cache_flush_count;
extern char terrain_cell_ids[];
extern char terrain_cell_dirty[];
extern int terrain_cell_at_player;

extern int object_reparent(struct record *, struct record *);
extern int mc_memset();
extern int mc_memcpy();
extern int xn_world_reload();
extern void automap_free_town_map(void);
extern void automap_save(void);
extern void people_clear(void);
extern void func_00064301(void);
extern void sound_stop_ambient(void);
extern void location_store_objects(void);
extern void location_free(struct loaded_location *);
extern void object_foreach(struct record *, int);
extern void object_delete_block(int, int);
extern void func_0008EB52(void);
extern void inv_assign_item_id(int);

void location_unload(int a1)
{
    {
        int l_20;
        int l_1C;

        if (((int)(unsigned short)*(short *)&a1) == 65535) return;
        func_0008EB52();
        location_store_objects();
        if (location_object->twin != 0) {
            location_object->twin->twin = 0;
            location_object->twin = 0;
        }
        object_reparent(location_object, player_object);
        object_foreach(player_entity->children, (int)inv_assign_item_id);
        people_clear();
        creature_count = 0;
        mc_memset((int)((char *)creature_list), 0, 512, (int)D_00176C94, 450, 512);
        object_delete_block((int)location_object->children, location_object->id);
        l_1C = location_object->id;
        location_object->image = 65535;
        location_object->id = -65535;
        model_cache_flush_count = 1;
        D_00187F2C = 0;
        location_free(&loaded_location);
        mc_memset((int)&loaded_location, 0, 20, (int)D_00176C94, 461, 4);
        mc_memset((int)current_location, 0, 48, (int)D_00176C94, 462, 4);
        automap_save();
        automap_free_town_map();
        if (((int)player_environment) == 3) {
            player_environment = 1;
            dungeon_water_level = 10000;
            terrain_cell_at_player = -1;
            func_00064301();
            xn_world_reload();
            mc_memset((int)terrain_cell_dirty, 0, 16, (int)D_00176C94, 477, 16);
            mc_memset((int)terrain_cell_ids, 0, 16, (int)D_00176C94, 478, 16);
            camera_object->yaw = player_object->yaw;
            if ((l_1C - 65536) == D_00187F28) {
                mc_memcpy((int)player_object, (int)saved_player_object, 55, (int)D_00176C94, 483, 4);
            } else {
                D_00187F2C++;
            }
            D_00195DB8 = 5;
        }
        D_00187F28 = l_1C;
        func_0008EB52();
        sound_stop_ambient();
    }
}
