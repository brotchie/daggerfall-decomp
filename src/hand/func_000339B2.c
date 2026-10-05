/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000339B2 */
#include "records.h"

extern char D_001970DD;
extern struct faction *faction_find(short);

int func_000339B2(struct location_door *door, struct qbn_person *qbn_person, struct building *building, int any_building)
{
    struct faction *faction;

    if (any_building != 0) {
        if ((door->flags & 0x5000) != 0x4000)
            return 0;
        if (qbn_person->kind == -6 && building->type < 17)
            return 1;
        if (qbn_person->kind == -6)
            return 0;
        if (qbn_person->kind > -1) {
            if (building->type == 11 && building->type == qbn_person->kind && building->faction_id == 40)
                return 1;
            if (qbn_person->kind == 40)
                return 0;
            if (qbn_person->kind >= 17 && qbn_person->kind <= 20 && building->type >= 17 && building->type <= 20)
                return 1;
            if (qbn_person->kind >= 17 && qbn_person->kind <= 20)
                return 0;
        }
        if (qbn_person->kind > -1 && building->type == qbn_person->kind)
            return 1;
        if (qbn_person->kind > -1)
            return 0;
        if (qbn_person->kind == -1 && (door->flags & 0x4000) != 0)
            return 1;
        if (qbn_person->kind == -1)
            return 0;
        if (qbn_person->kind == -2 && faction_find(door->flags & 0x3FF)->type == qbn_person->faction_id)
            return 1;
        if (qbn_person->kind == -2)
            return 0;
        faction = faction_find(building->faction_id);
        return faction->type == qbn_person->faction_id ? 1 : 0;
    }
    if ((door->flags & 0x2000) == 0)
        return 0;
    if ((int)(short)(qbn_person->flags & 0x600) != 0) {
        if (D_001970DD != 0 && (door->flags & 0x8000) == 0)
            return 0;
        if (D_001970DD == 0 && (door->flags & 0x8000) != 0)
            return 0;
    }
    if (qbn_person->kind == -6 && building->type < 17)
        return 1;
    if (qbn_person->kind == -6)
        return 0;
    if (qbn_person->kind > -1) {
        if (building->type == 11 && building->type == qbn_person->kind && building->faction_id == 40)
            return 1;
        if (qbn_person->kind == 40)
            return 0;
        if (qbn_person->kind >= 17 && qbn_person->kind <= 20 && building->type >= 17 && building->type <= 20)
            return 1;
        if (qbn_person->kind >= 17 && qbn_person->kind <= 20)
            return 0;
    }
    if (qbn_person->kind > -1 && building->type == qbn_person->kind)
        return 1;
    if (qbn_person->kind > -1)
        return 0;
    if (qbn_person->kind == -1 && (door->flags & 0x3FF) == qbn_person->faction_id)
        return 1;
    if (qbn_person->kind == -1)
        return 0;
    if (qbn_person->kind == -2 && faction_find(door->flags & 0x3FF)->type == qbn_person->faction_id)
        return 1;
    if (qbn_person->kind == -2)
        return 0;
    faction = faction_find(door->flags & 0x3FF);
    return faction->type == qbn_person->faction_id ? 1 : 0;
}
