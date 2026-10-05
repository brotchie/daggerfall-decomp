/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000339B2 */
#include "records.h"

struct door_entry { short f0; unsigned short flags; };  /* loaded_location_doors: 6 bytes */
struct cond { short f0; short flags; short type; short value; };
extern char D_001970DD;
extern struct faction *faction_find(short);

int func_000339B2(struct door_entry *a1, struct cond *a2, struct building *a3, int a4)
{
    struct faction *l_10;

    if (a4 != 0) {
        if ((a1->flags & 0x5000) != 0x4000)
            return 0;
        if (a2->type == -6 && a3->type < 17)
            return 1;
        if (a2->type == -6)
            return 0;
        if (a2->type > -1) {
            if (a3->type == 11 && a3->type == a2->type && a3->faction_id == 40)
                return 1;
            if (a2->type == 40)
                return 0;
            if (a2->type >= 17 && a2->type <= 20 && a3->type >= 17 && a3->type <= 20)
                return 1;
            if (a2->type >= 17 && a2->type <= 20)
                return 0;
        }
        if (a2->type > -1 && a3->type == a2->type)
            return 1;
        if (a2->type > -1)
            return 0;
        if (a2->type == -1 && (a1->flags & 0x4000) != 0)
            return 1;
        if (a2->type == -1)
            return 0;
        if (a2->type == -2 && faction_find(a1->flags & 0x3FF)->type == a2->value)
            return 1;
        if (a2->type == -2)
            return 0;
        l_10 = faction_find(a3->faction_id);
        return l_10->type == a2->value ? 1 : 0;
    }
    if ((a1->flags & 0x2000) == 0)
        return 0;
    if ((int)(short)(a2->flags & 0x600) != 0) {
        if (D_001970DD != 0 && (a1->flags & 0x8000) == 0)
            return 0;
        if (D_001970DD == 0 && (a1->flags & 0x8000) != 0)
            return 0;
    }
    if (a2->type == -6 && a3->type < 17)
        return 1;
    if (a2->type == -6)
        return 0;
    if (a2->type > -1) {
        if (a3->type == 11 && a3->type == a2->type && a3->faction_id == 40)
            return 1;
        if (a2->type == 40)
            return 0;
        if (a2->type >= 17 && a2->type <= 20 && a3->type >= 17 && a3->type <= 20)
            return 1;
        if (a2->type >= 17 && a2->type <= 20)
            return 0;
    }
    if (a2->type > -1 && a3->type == a2->type)
        return 1;
    if (a2->type > -1)
        return 0;
    if (a2->type == -1 && (a1->flags & 0x3FF) == a2->value)
        return 1;
    if (a2->type == -1)
        return 0;
    if (a2->type == -2 && faction_find(a1->flags & 0x3FF)->type == a2->value)
        return 1;
    if (a2->type == -2)
        return 0;
    l_10 = faction_find(a1->flags & 0x3FF);
    return l_10->type == a2->value ? 1 : 0;
}
