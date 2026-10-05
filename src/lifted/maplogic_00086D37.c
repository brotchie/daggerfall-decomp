/* maplogic.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern char dungeon_water_level[];
extern char D_00176C94[];
extern char player_environment[];
extern char D_00187F28[];
extern char D_00187F2C[];
extern struct record *D_00190504[];
extern struct record *camera_object;
extern struct record *player_entity;
extern struct record *player_object;
extern struct record *D_00195AC4;
extern char creature_count[];
extern struct location *current_location;
extern char D_00195DB8[];
extern char D_00195FB1[];
extern struct loaded_location loaded_location;
extern char D_001A949D[];
extern char D_001A94A0[];
extern char D_001A94B0[];
extern char D_001A94C4[];

extern int object_reparent(struct record *, struct record *);
extern int mc_memset();
extern int mc_memcpy();
extern int func_000C2FF5();
extern void func_000279B9(void);
extern void automap_save(void);
extern void people_clear(void);
extern void func_00064301(void);
extern void sound_stop_ambient(void);
extern void location_store_objects(void);
extern void location_free(struct loaded_location *);
extern void object_foreach(struct record *, int);
extern void func_0008EAF1(int, int);
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
    if (D_00195AC4->twin == 0) goto L86D89;
    D_00195AC4->twin->twin = 0;
    D_00195AC4->twin = 0;
L86D89:;
    object_reparent(D_00195AC4, player_object);
    object_foreach(player_entity->children, (int)inv_assign_item_id);
    people_clear();
    *(int *)creature_count = 0;
    mc_memset((int)((char *)D_00190504), 0, 512, (int)D_00176C94, 450, 512);
    func_0008EAF1((int)D_00195AC4->children, D_00195AC4->id);
    l_1C = D_00195AC4->id;
    D_00195AC4->image = 65535;
    D_00195AC4->id = -65535;
    *(signed char *)D_001A949D = 1;
    *(int *)D_00187F2C = 0;
    location_free(&loaded_location);
    mc_memset((int)&loaded_location, 0, 20, (int)D_00176C94, 461, 4);
    mc_memset((int)current_location, 0, 48, (int)D_00176C94, 462, 4);
    automap_save();
    func_000279B9();
    if (((int)(unsigned char)*(signed char *)player_environment) != 3) goto L86F34;
    *(signed char *)player_environment = 1;
    *(int *)dungeon_water_level = 10000;
    *(int *)D_001A94C4 = -1;
    func_00064301();
    func_000C2FF5();
    mc_memset((int)D_001A94B0, 0, 16, (int)D_00176C94, 477, 16);
    mc_memset((int)D_001A94A0, 0, 16, (int)D_00176C94, 478, 16);
    camera_object->yaw = player_object->yaw;
    if ((l_1C - 65536) != *(int *)D_00187F28) goto L86F24;
    mc_memcpy((int)player_object, (int)D_00195FB1, 55, (int)D_00176C94, 483, 4);
    goto L86F2A;
L86F24:;
    (*(int *)D_00187F2C)++;
L86F2A:;
    *(int *)D_00195DB8 = 5;
L86F34:;
    *(int *)D_00187F28 = l_1C;
    func_0008EB52();
    sound_stop_ambient();
}
}
