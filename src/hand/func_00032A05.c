/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00032A05 */
#include "records.h"

#pragma pack(1)
struct R6 { unsigned short w0; unsigned short flags; unsigned short w4; };
#pragma pack()
extern char D_00170A64[];
extern struct record *nonworld_root;
extern struct record *location_object;
extern struct location *current_location;
extern unsigned *scratch_buffer;
extern unsigned char current_region;
extern int loaded_location_door_count;
extern struct R6 *loaded_location_doors;
extern short D_001970C8;
extern int D_001970CC;
extern struct R6 *D_001970D0;
extern struct record *D_001970D4;
extern struct location *D_001970D8;
extern struct quest *current_quest;
extern struct faction *faction_find_type_in_region(short, short);
extern int func_000337AD(struct R6 *, struct qbn_place *, struct building *);
extern int quest_object_in_use(int);
extern void location_free(short *);
extern void quest_pick_location(short *, unsigned short, short, int);
extern struct record *object_create_child(struct record *, struct record *, int);
extern int rand();
extern int mc_strncpy();
extern int mc_memcpy();

int quest_init_place(struct qbn_place *place)
{
    struct R6 *doors;
    struct R6 *door;
    struct record *loc_object;
    struct record *place_object;
    struct location *location;
    unsigned *candidates;
    struct building *building;
    int door_count;
    int candidate_count;
    int i;
    int tries;
    int unused;  /* [ebp-0x1c]: declared, never used */

    tries = 0;
retry:
    if (place->scope == 0) {
        place->scope = 10;
        place_object = object_create_child(nonworld_root, 0, 26);
        place_object->type = 40;
        place->object = place_object;
        place_object->id = (place->p1 << 16) | (unsigned short)(place->p2 & 0xffff);
        place_object->repair_due = place_object->id;
        place_object->flags = 0x202;
        place_object->owner = current_quest->id;
        place_object->quest_id = current_quest->id;
        return 1;
    }
    if (place->scope > 0)
        place->scope--;
    if (place->scope != 0) {
        location_free(&D_001970C8);
        quest_pick_location(&D_001970C8, place->p1, place->p2, place->scope);
        doors = D_001970D0;
        door_count = D_001970CC;
        loc_object = D_001970D4;
        location = D_001970D8;
    } else {
        doors = loaded_location_doors;
        door_count = loaded_location_door_count;
        loc_object = location_object;
        location = current_location;
    }
    candidates = scratch_buffer;
    candidate_count = 0;
    if (place->p1 == 0) {
        for (candidate_count = i = 0, door = doors; i < door_count; i++, door++) {
            if (func_000337AD(door, place, &location->buildings[door->w0]))
                candidates[candidate_count++] = (door->w0 << 16) + door->w4;
        }
    } else {
        for (i = 0, door = doors; i < door_count; i++, door++) {
            switch (place->p3) {
            case -1:
                candidates[candidate_count++] = door->w4;
                break;
            case 0:
                if ((int)(unsigned short)(door->flags & 0x4000))
                    candidates[candidate_count++] = door->w4;
                break;
            case 1:
                if ((int)(unsigned short)(door->flags & 0x1000))
                    candidates[candidate_count++] = door->w4;
                break;
            }
        }
    }
    i = 0;
    if (place->scope > -1)
        place->scope++;
    if (candidate_count == 0 && ++tries < 100)
        goto retry;
    place->scope--;
    if (candidate_count == 0)
        return 0;
    i = rand() % candidate_count;
    if (quest_object_in_use((loc_object->id & 0xffff0000) + (candidates[i] & 0xffff)))
        goto retry;
    place_object = object_create_child(nonworld_root, 0, 58);
    place_object->type = 40;
    place_object->flags = 0x202;
    place_object->image = D_001970C8;
    place_object->owner = current_quest->id;
    place_object->id = (loc_object->id & 0xffff0000) + (candidates[i] & 0xffff);
    place_object->repair_due = place_object->id;
    place_object->quest_id = current_quest->id;
    place_object->link_flag = D_001970D8->kind;
    place_object->region = (unsigned short)current_region;
    place->object = place_object;
    place_object->x = loc_object->x;
    place_object->y = loc_object->y;
    place_object->z = loc_object->z;
    if (place->scope != 1)
        place_object->image2 = candidates[i] >> 16;
    else
        place_object->image2 = 0xffff;
    building = &place_object->data.building;
    if (building->faction_id == 0)
        building->faction_id = faction_find_type_in_region(current_region, 15)->id;
    if (place->p1 != 1)
        mc_memcpy(building, &location->buildings[candidates[i] >> 16], 26, D_00170A64, 483, 4);
    mc_strncpy((char *)building + 26, location, 4, D_00170A64, 485);
    location_free(&D_001970C8);
    return 1;
}
