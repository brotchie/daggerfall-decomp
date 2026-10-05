/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00031B8A */
#include "records.h"

#pragma pack(1)
struct Kind { char pad[6]; unsigned char flags; };
struct Ent6 { unsigned short a; unsigned short b; unsigned short c; };
#pragma pack()
extern char D_00170A64[];
extern unsigned char D_0017A25C[];
extern short D_0017A270[];
extern struct record *nonworld_root;
extern struct record *player_object;
extern struct record *location_object;
extern struct location *current_location;
extern char *scratch_buffer;
extern struct record *D_00195CE8;
extern unsigned char current_region;
extern int loaded_location_door_count;
extern struct Ent6 *loaded_location_doors;
extern char D_001970C8[];
extern int D_001970CC;
extern struct Ent6 *D_001970D0;
extern struct record *D_001970D4;
extern struct location *D_001970D8;
extern char D_001970DC;
extern unsigned char D_001970DD;
extern struct quest *current_quest;
extern struct faction *faction_find_type_in_region(short, int);
extern struct faction *faction_find(short);
extern struct faction *faction_random_of_type(unsigned char);
extern struct qbn_person *quest_record(struct quest *, int, int);
extern int func_000339B2(struct Ent6 *, struct qbn_person *, struct building *, int);
extern struct faction *pick_random_of_three(struct faction **);
extern int faction_random_hostile_id(void);
extern int quest_object_in_use(int);
extern struct Kind *flats_cfg_find(unsigned short);
extern int rand_range(int, int);
extern struct building *object_building(struct record *);
extern void location_free(char *);
extern void location_pick_random_town(char *);
extern struct record *object_create_child(struct record *, struct record *, int);
extern int object_reparent(struct record *, struct record *);
extern int object_new_id(int);
extern int rand(void);
extern int mc_strncpy(char *, char *, int, char *, int);
extern int mc_memcpy(void *, void *, int, char *, int, int);

int quest_init_person(struct qbn_person *qbn_person)
{
    struct Ent6 *doors;
    struct Ent6 *door;
    int door_count;
    int candidate_count;
    int i;
    int any_building;
    int tries;
    int npc_faction_id;
    struct record *loc_object;
    struct record *object;
    struct record *place_object;
    struct location *location;
    int *candidates;
    struct building *building;
    struct Kind *flat_cfg;
    struct faction *faction;
    struct qbn_person *linked_person;

    npc_faction_id = 0;
    qbn_person->flags &= 0x0fff;
    if (qbn_person->kind == 21) {
        if (D_00195CE8 == 0 || D_00195CE8->twin != 0)
            return 0;
        flat_cfg = flats_cfg_find(D_00195CE8->image);
        object = object_create_child(nonworld_root, 0, 58);
        building = object_building(player_object);
        if (building != 0) {
            if (building->faction_id == 65535)
                building->faction_id = 510;
            if (building->faction_id == 0)
                building->faction_id = faction_find_type_in_region(current_region, 15)->id;
            mc_memcpy(&object->data, building, 26, D_00170A64, 64, 4);
        } else if (D_00195CE8->data.person.faction_id != 0) {
            object->data.building.faction_id = D_00195CE8->data.person.faction_id;
        }
        if (D_00195CE8->data.person.faction_id != 0)
            object->faction_id = D_00195CE8->data.person.faction_id;
        else
            object->faction_id = building->faction_id;
        mc_strncpy(RECORD_DATA(object) + 26, (char *)current_location, 4, D_00170A64, 76);
        mc_memcpy(&object->x, &D_00195CE8->x, 12, D_00170A64, 77, 4);
        object->type = 41;
        object->image = D_00195CE8->image;
        qbn_person->object = object;
        object->flags = 514;
        object->flags |= (int)(unsigned char)(flat_cfg->flags & 1) != 0 ? 4 : 0;
        object->home_region = (unsigned short)current_region;
        object->quest_id = current_quest->id;
        object->id = D_00195CE8->id;
        object->repair_due = object->id;
        if ((int)(unsigned short)(object->flags & 4) != 0)
            *(RECORD_DATA(object) + 2) |= 16;
        else
            *(RECORD_DATA(object) + 2) &= ~16;
        return 1;
    }
    if (qbn_person->kind == -8) {
        object = object_create_child(nonworld_root, 0, 0);
        object->type = 65;
        object->image = 0;
        qbn_person->object = object;
        object->repair_due = 0;
        object->id = object_new_id(800);
        object->flags = 514;
        object->faction_id = qbn_person->faction_id;
        object->flags |= (int)(unsigned char)(flats_cfg_find(object->image)->flags & 1) != 0 ? 4 : 0;
        object->home_region = (unsigned short)current_region;
        object->quest_id = current_quest->id;
        return 1;
    }
    if (!(qbn_person->kind != -1 || (int)(short)(qbn_person->flags & 0x100) != 0)) {
        faction = faction_find(qbn_person->faction_id);
        if (faction->type == 4) {
            flat_cfg = flats_cfg_find(faction->flats[0]);
            object = object_create_child(nonworld_root, 0, 58);
            object->faction_id = faction->id;
            object->type = 41;
            object->image = faction->flats[0];
            object->flags = 514;
            object->flags |= (int)(unsigned char)(flat_cfg->flags & 1) != 0 ? 4 : 0;
            object->home_region = (unsigned short)current_region;
            object->quest_id = current_quest->id;
            qbn_person->object = object;
            object->repair_due = object->id = object_new_id(800);
            object->data.building.faction_id = qbn_person->faction_id;
            return 1;
        }
    }
    if (qbn_person->kind == -2) {
        if (qbn_person->faction_id == 10)
            npc_faction_id = D_0017A270[rand_range(0, 9)];
        else
            npc_faction_id = faction_random_of_type(qbn_person->faction_id)->id;
        qbn_person->kind = -1;
    }
    if (!(qbn_person->kind != -4 || qbn_person->faction_id != 10000)) {
        i = faction_random_hostile_id();
        if (i == 0) {
            qbn_person->kind = -6;
        } else {
            qbn_person->kind = -1;
            qbn_person->faction_id = i;
        }
    }
    if (qbn_person->kind >= -5 && qbn_person->kind <= -3 || qbn_person->kind == -7) {
        linked_person = quest_record(current_quest, 3, qbn_person->faction_id);
        if (linked_person == 0) {
            qbn_person->object = 0;
            D_001970DC++;
            return 1;
        }
        object = linked_person->object;
        if (object == 0) {
            qbn_person->object = 0;
            D_001970DC++;
            return 1;
        }
        switch ((unsigned short)(qbn_person->kind + 7)) {
        case 0:
            ((unsigned char *)&qbn_person->flags)[1] &= 0xf9;
            *(short *)&qbn_person->flags |= (int)(unsigned short)(object->flags & 4) != 0 ? 1024 : 512;
            qbn_person->kind = -6;
            break;
        case 2:
            qbn_person->faction_id = object->data.building.faction_id;
            qbn_person->kind = -1;
            break;
        case 3:
            faction = faction_find(object->data.building.faction_id);
            if (faction != 0) {
                faction = pick_random_of_three(faction->enemies);
            } else {
                qbn_person->object = 0;
                qbn_person->kind = -5;
                D_001970DC++;
                return 1;
            }
            if (faction != 0)
                qbn_person->faction_id = faction->id;
            else
                qbn_person->faction_id = 510;
            qbn_person->kind = -1;
            break;
        case 4:
            faction = faction_find(object->data.building.faction_id);
            if (faction != 0) {
                faction = pick_random_of_three(faction->allies);
            } else {
                qbn_person->object = 0;
                qbn_person->kind = -5;
                D_001970DC++;
                return 1;
            }
            if (faction != 0)
                qbn_person->faction_id = faction->id;
            else
                qbn_person->faction_id = 510;
            qbn_person->kind = -1;
            break;
        }
    }
    if ((int)(short)(qbn_person->flags & 0xff) == 255)
        qbn_person->flags = (qbn_person->flags & 0xff00) + (rand() & 1);
    tries = 0;
retry:
    if ((int)(short)(qbn_person->flags & 0xff) == 0) {
        doors = loaded_location_doors;
        door_count = loaded_location_door_count;
        loc_object = location_object;
        location = current_location;
    } else {
        location_free(D_001970C8);
        location_pick_random_town(D_001970C8);
        door_count = D_001970CC;
        doors = D_001970D0;
        loc_object = D_001970D4;
        location = D_001970D8;
    }
    candidates = (int *)scratch_buffer;
    candidate_count = 0;
    any_building = 0;
    if ((int)(short)(qbn_person->flags & 0x600) != 0)
        D_001970DD = (int)(short)(qbn_person->flags & 0x200) != 0 ? 1 : 0;
    else
        D_001970DD = rand() & 1;
    if ((int)(short)(qbn_person->flags & 0x100) == 0) {
        for (candidate_count = i = 0, door = doors; i < door_count; i++, door++) {
            if (door->a != 65535 && func_000339B2(door, qbn_person, &location->buildings[door->a], 0) != 0)
                candidates[candidate_count++] = (door->a << 16) + door->c;
        }
    }
    if (qbn_person->kind != -6 && candidate_count == 0 || (int)(short)(qbn_person->flags & 0x100) != 0) {
        any_building = 1;
        for (i = 0, door = doors; i < door_count; i++, door++) {
            if ((int)(short)(qbn_person->flags & 0x100) != 0 && func_000339B2(door, qbn_person, &location->buildings[door->a], 1) != 0)
                candidates[candidate_count++] = (door->a << 16) + door->c;
            else if (door->a != 65535 && func_000339B2(door, qbn_person, &location->buildings[door->a], 1) != 0)
                candidates[candidate_count++] = (door->a << 16) + door->c;
        }
    }
    if (candidate_count == 0 && (int)(short)(qbn_person->flags & 0xff) == 0) {
        if (qbn_person->kind == -1)
            npc_faction_id = qbn_person->faction_id;
        if (qbn_person->kind == -2)
            npc_faction_id = faction_random_of_type(qbn_person->faction_id)->id;
        qbn_person->kind = -6;
    }
    if (candidate_count == 0 && qbn_person->kind != -6) {
        if (qbn_person->kind == -1)
            npc_faction_id = qbn_person->faction_id;
        if (qbn_person->kind == -2)
            npc_faction_id = faction_random_of_type(qbn_person->faction_id)->id;
        if (tries < 50)
            goto retry;
        qbn_person->kind = -6;
        tries = 0;
        goto retry;
    }
    if (candidate_count == 0)
        return 0;
    i = rand() % candidate_count;
    if (any_building != 0 && quest_object_in_use((loc_object->id & 0xffff0000) + (candidates[i] & 0xffff)) != 0 && ++tries < 100)
        goto retry;
    object = object_create_child(nonworld_root, 0, 58);
    mc_memcpy(&object->x, &loc_object->x, 12, D_00170A64, 307, 4);
    mc_memcpy(&object->data, &location->buildings[(unsigned)candidates[i] >> 16], 26, D_00170A64, 308, 4);
    mc_strncpy(RECORD_DATA(object) + 26, (char *)location, 4, D_00170A64, 309);
    if (object->data.building.faction_id == 0)
        object->data.building.faction_id = faction_find_type_in_region(current_region, 15)->id;
    if (npc_faction_id != 0)
        object->data.building.faction_id = npc_faction_id;
    else if (qbn_person->kind == -1)
        object->data.building.faction_id = qbn_person->faction_id;
    object->type = 41;
    object->image = 0;
    qbn_person->object = object;
    if (any_building != 0 && (int)(short)(qbn_person->flags & 0x100) != 0) {
        object->id = object_new_id((unsigned)loc_object->id >> 16);
        object->repair_due = object->id;
    } else {
        object->id = (loc_object->id & 0xffff0000) + (candidates[i] & 0xffff);
        object->repair_due = object->id;
    }
    object->flags = 514;
    if (npc_faction_id != 0) {
        object->image = faction_find(npc_faction_id)->flats[D_001970DD];
        object->faction_id = npc_faction_id;
    } else if (qbn_person->kind == -1) {
        object->image = faction_find(qbn_person->faction_id)->flats[D_001970DD];
        object->faction_id = qbn_person->faction_id;
    } else if ((short)object->data.building.faction_id > 0) {
        object->image = faction_find(object->data.building.faction_id)->flats[D_001970DD];
        object->faction_id = object->data.building.faction_id;
    } else {
        object->image = D_0017A25C[D_001970DD * 10 + rand_range(0, 9)] + 23296;
        object->faction_id = 510;
    }
    flat_cfg = flats_cfg_find(object->image);
    object->flags |= (int)(unsigned char)(flat_cfg->flags & 1) != 0 ? 4 : 0;
    object->home_region = (unsigned short)current_region;
    object->quest_id = current_quest->id;
    if (any_building != 0 && (int)(short)(qbn_person->flags & 0x100) == 0) {
        place_object = object_create_child(nonworld_root, 0, 26);
        place_object->type = 40;
        place_object->id = object->id;
        place_object->quest_id = current_quest->id;
        object_reparent(place_object, object);
        object->id = object_new_id((unsigned)object->id >> 16);
    }
    location_free(D_001970C8);
    return 1;
}
