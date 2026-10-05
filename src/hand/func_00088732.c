/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00088732 */
#include "records.h"
extern int xn_world_slot_cells[];
extern char *xn_world_height_layer;
extern char *xn_world_flat_layer;
extern int D_00187F30[];
extern signed char text_rsc_buffer[];
extern struct record *player_object;
extern char world_loading;
extern struct map_location *location_here;
extern int terrain_cell_ids[];
extern int terrain_cell_dirty[];
extern int D_001A94C0;
extern void parse_rsc_text(int, int, int);
extern int hud_message_add(char *);
extern struct map_location *region_find_location(int);
extern void location_flatten_terrain(signed char *, signed char *);
extern int xn_world_cell_at();
extern int xn_world_place_nature_flats();
extern int xn_world_mark_nonplanar_quads();

void terrain_update_cells(void)
{
    int i;
    int player_cell;
    struct map_location *saved_location;

    player_cell = xn_world_cell_at(player_object->x, player_object->z);
    saved_location = location_here;
    D_001A94C0 = 4;
    for (i = 0; i < 4; i++) {
        if (terrain_cell_ids[i] != xn_world_slot_cells[i]) {
            terrain_cell_ids[i] = xn_world_slot_cells[i];
            terrain_cell_dirty[i] = 1;
            D_001A94C0 = i;
            if ((location_here = region_find_location(terrain_cell_ids[i])) != 0) {
                if (world_loading == 0 && player_cell == terrain_cell_ids[i]) {
                    switch ((location_here->x_type_flags << 2) >> 27) {
                    case 4:
                    case 7:
                    case 10:
                    case 12:
                        parse_rsc_text(location_here->dungeon_type + 500, 0, 0);
                        hud_message_add(((char *)text_rsc_buffer));
                    }
                }
                location_flatten_terrain(xn_world_height_layer + D_00187F30[D_001A94C0], xn_world_flat_layer + D_00187F30[D_001A94C0]);
                xn_world_mark_nonplanar_quads();
            }
            xn_world_place_nature_flats(i);
        }
    }
    location_here = saved_location;
}
