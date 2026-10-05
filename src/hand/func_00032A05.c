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

int quest_init_place(struct qbn_place *a1)
{
    struct R6 *base;
    struct R6 *ptr;
    struct record *l40;
    struct record *obj;
    struct location *l38;
    unsigned *list;
    struct building *p;
    int n;
    int count;
    int i;
    int tries;
    int unused;  /* [ebp-0x1c]: declared, never used */

    tries = 0;
retry:
    if (a1->scope == 0) {
        a1->scope = 10;
        obj = object_create_child(nonworld_root, 0, 26);
        obj->type = 40;
        a1->object = obj;
        obj->id = (a1->p1 << 16) | (unsigned short)(a1->p2 & 0xffff);
        obj->repair_due = obj->id;
        obj->flags = 0x202;
        obj->owner = current_quest->id;
        obj->quest_id = current_quest->id;
        return 1;
    }
    if (a1->scope > 0)
        a1->scope--;
    if (a1->scope != 0) {
        location_free(&D_001970C8);
        quest_pick_location(&D_001970C8, a1->p1, a1->p2, a1->scope);
        base = D_001970D0;
        n = D_001970CC;
        l40 = D_001970D4;
        l38 = D_001970D8;
    } else {
        base = loaded_location_doors;
        n = loaded_location_door_count;
        l40 = location_object;
        l38 = current_location;
    }
    list = scratch_buffer;
    count = 0;
    if (a1->p1 == 0) {
        for (count = i = 0, ptr = base; i < n; i++, ptr++) {
            if (func_000337AD(ptr, a1, &l38->buildings[ptr->w0]))
                list[count++] = (ptr->w0 << 16) + ptr->w4;
        }
    } else {
        for (i = 0, ptr = base; i < n; i++, ptr++) {
            switch (a1->p3) {
            case -1:
                list[count++] = ptr->w4;
                break;
            case 0:
                if ((int)(unsigned short)(ptr->flags & 0x4000))
                    list[count++] = ptr->w4;
                break;
            case 1:
                if ((int)(unsigned short)(ptr->flags & 0x1000))
                    list[count++] = ptr->w4;
                break;
            }
        }
    }
    i = 0;
    if (a1->scope > -1)
        a1->scope++;
    if (count == 0 && ++tries < 100)
        goto retry;
    a1->scope--;
    if (count == 0)
        return 0;
    i = rand() % count;
    if (quest_object_in_use((l40->id & 0xffff0000) + (list[i] & 0xffff)))
        goto retry;
    obj = object_create_child(nonworld_root, 0, 58);
    obj->type = 40;
    obj->flags = 0x202;
    obj->image = D_001970C8;
    obj->owner = current_quest->id;
    obj->id = (l40->id & 0xffff0000) + (list[i] & 0xffff);
    obj->repair_due = obj->id;
    obj->quest_id = current_quest->id;
    obj->link_flag = D_001970D8->kind;
    obj->region = (unsigned short)current_region;
    a1->object = obj;
    obj->x = l40->x;
    obj->y = l40->y;
    obj->z = l40->z;
    if (a1->scope != 1)
        obj->image2 = list[i] >> 16;
    else
        obj->image2 = 0xffff;
    p = &obj->data.building;
    if (p->faction_id == 0)
        p->faction_id = faction_find_type_in_region(current_region, 15)->id;
    if (a1->p1 != 1)
        mc_memcpy(p, &l38->buildings[list[i] >> 16], 26, D_00170A64, 483, 4);
    mc_strncpy((char *)p + 26, l38, 4, D_00170A64, 485);
    location_free(&D_001970C8);
    return 1;
}
