/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0001E854 */
#include "records.h"
extern char D_00170533[];
extern int D_001967EC;
extern int rmb_origin_z;
extern int rmb_origin_y;
extern int rmb_origin_x;
extern char rmb_origin_yaw[];
extern int D_00196804;
extern char rmb_block[];
extern int block_origin_x;
extern int block_origin_z;
extern int stricmp();
extern int xn_terrain_height_at();

#pragma pack(1)
struct E { int f0; int f4; int f8; int fc; int f10; };
struct H { char pad[3]; struct E e[317]; char pad2[17]; char names[10][13]; };
#pragma pack()
#define HP (*(struct H **)rmb_block)

void town_block_place_building(struct building *building, int building_index)
{
    int unused1;
    int unused2;

    D_00196804 = HP->e[building_index].f0;
    D_001967EC = HP->e[building_index].f4;
    rmb_origin_x = block_origin_x + HP->e[building_index].f8;
    rmb_origin_z = block_origin_z - HP->e[building_index].fc;
    rmb_origin_y = xn_terrain_height_at(rmb_origin_x, rmb_origin_z) - 6;
    *(int *)rmb_origin_yaw = HP->e[building_index].f10;
    if (stricmp(HP->names[building_index], D_00170533) != 0) return;
    building->type = 11;
    building->faction_id = 414;
}
