/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00087F76 */
#include "records.h"
extern char D_00176C94[];       /* __FILE__ */
extern int D_00187EE4[];
extern struct record *location_object;
extern int region_dungeon_type_counts[];
extern int region_dungeon_count;
extern int region_locations;
extern void location_load_dungeon(struct loaded_location *, int);
extern void location_load_nth_dungeon_of_type(struct loaded_location *, int, int);
extern void location_load_exterior(struct loaded_location *, unsigned short);
extern void location_free(struct loaded_location *);
extern void location_pick_random_with_service(struct loaded_location *, int, short);
extern void location_pick_random_town(struct loaded_location *);
extern int rand(void);
extern void mc_memset(void *, int, int, char *, int, int);

void quest_pick_location(struct loaded_location *location, unsigned kind, int sub_kind, int mode)
{
    int unused[3];
    int unused1;
    int pick;
    int done;
    int unused2;
    int saved;

    saved = region_locations;
    done = 0;
    mc_memset(location, 0, 20, D_00176C94, 1045, 4);
    if (mode == 0) {
        location_load_exterior(location, location_object->image);
        return;
    }
    while (done == 0) {
        location_free(location);
        switch (kind) {
        case 0:
            while (sub_kind == -1 || sub_kind == 1)
                sub_kind = rand() % 21;
            if (sub_kind > 16) {
                location_pick_random_town(location);
            } else {
                sub_kind = D_00187EE4[sub_kind];
                location_pick_random_with_service(location, sub_kind, -1);
            }
            break;
        case 1:
            if (sub_kind != -1) {
                if (region_dungeon_type_counts[sub_kind] != 0) {
                    pick = rand() % region_dungeon_type_counts[sub_kind];
                    location_load_nth_dungeon_of_type(location, sub_kind, pick);
                    break;
                }
            }
            pick = rand() % region_dungeon_count;
            location_load_dungeon(location, pick);
            break;
        }
        if (mode == 1)
            done = location->object->image != location_object->image;
        else
            done++;
    }
}
