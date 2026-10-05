/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0001E854 */
#include "records.h"
extern char D_00170533[];
extern int D_001967EC;
extern int rmb_origin_z;
extern int rmb_origin_y;
extern int rmb_origin_x;
extern char rmb_origin_yaw[];
extern int D_00196804;
extern struct rmb_file *rmb_block;
extern int block_origin_x;
extern int block_origin_z;
extern int stricmp();
extern int xn_terrain_height_at();

void town_block_place_building(struct building *building, int building_index)
{
    int unused1;
    int unused2;

    D_00196804 = rmb_block->positions[building_index].unknown1;
    D_001967EC = rmb_block->positions[building_index].unknown2;
    rmb_origin_x = block_origin_x + rmb_block->positions[building_index].x;
    rmb_origin_z = block_origin_z - rmb_block->positions[building_index].z;
    rmb_origin_y = xn_terrain_height_at(rmb_origin_x, rmb_origin_z) - 6;
    *(int *)rmb_origin_yaw = rmb_block->positions[building_index].yaw;
    if (stricmp(rmb_block->other_names[building_index], D_00170533) != 0) return;
    building->type = 11;
    building->faction_id = 414;
}
