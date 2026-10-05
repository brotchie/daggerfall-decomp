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

void quest_pick_location(struct loaded_location *n, unsigned kind, int a3, int mode)
{
    int unused[3];
    int l_10;
    int r;
    int done;
    int l_1C;
    int saved;

    saved = region_locations;
    done = 0;
    mc_memset(n, 0, 20, D_00176C94, 1045, 4);
    if (mode == 0) {
        location_load_exterior(n, location_object->image);
        return;
    }
    while (done == 0) {
        location_free(n);
        switch (kind) {
        case 0:
            while (a3 == -1 || a3 == 1)
                a3 = rand() % 21;
            if (a3 > 16) {
                location_pick_random_town(n);
            } else {
                a3 = D_00187EE4[a3];
                location_pick_random_with_service(n, a3, -1);
            }
            break;
        case 1:
            if (a3 != -1) {
                if (region_dungeon_type_counts[a3] != 0) {
                    r = rand() % region_dungeon_type_counts[a3];
                    location_load_nth_dungeon_of_type(n, a3, r);
                    break;
                }
            }
            r = rand() % region_dungeon_count;
            location_load_dungeon(n, r);
            break;
        }
        if (mode == 1)
            done = n->object->image != location_object->image;
        else
            done++;
    }
}
