/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00032A05 */
#include "records.h"

#pragma pack(1)
struct R6 { unsigned short w0; unsigned short flags; unsigned short w4; };
struct T {                  /* a QBN place (struct qbn_place): the fields its pad holds */
    char pad0[3];
    signed char cnt;        /* 0x03 */
    unsigned short w4;      /* 0x04 */
    short s6;               /* 0x06 */
    short mode;             /* 0x08 */
    char pad0a[6];
    struct record *obj;     /* 0x10 */
};
#pragma pack()
extern char D_00170A64[];
extern struct record *nonworld_root;
extern struct record *D_00195AC4;
extern struct location *current_location;
extern unsigned *D_00195C44;
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
extern int func_000337AD(struct R6 *, struct T *, struct building *);
extern int quest_object_in_use(int);
extern void location_free(short *);
extern void func_00087F76(short *, unsigned short, short, int);
extern struct record *object_create_child(struct record *, struct record *, int);
extern int rand();
extern int mc_strncpy();
extern int mc_memcpy();

int quest_init_place(struct T *a1)
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
    if (a1->cnt == 0) {
        a1->cnt = 10;
        obj = object_create_child(nonworld_root, 0, 26);
        obj->type = 40;
        a1->obj = obj;
        obj->id = (a1->w4 << 16) | (unsigned short)(a1->s6 & 0xffff);
        obj->repair_due = obj->id;
        obj->flags = 0x202;
        obj->owner = current_quest->id;
        obj->quest_id = current_quest->id;
        return 1;
    }
    if (a1->cnt > 0)
        a1->cnt--;
    if (a1->cnt != 0) {
        location_free(&D_001970C8);
        func_00087F76(&D_001970C8, a1->w4, a1->s6, a1->cnt);
        base = D_001970D0;
        n = D_001970CC;
        l40 = D_001970D4;
        l38 = D_001970D8;
    } else {
        base = loaded_location_doors;
        n = loaded_location_door_count;
        l40 = D_00195AC4;
        l38 = current_location;
    }
    list = D_00195C44;
    count = 0;
    if (a1->w4 == 0) {
        for (count = i = 0, ptr = base; i < n; i++, ptr++) {
            if (func_000337AD(ptr, a1, &l38->buildings[ptr->w0]))
                list[count++] = (ptr->w0 << 16) + ptr->w4;
        }
    } else {
        for (i = 0, ptr = base; i < n; i++, ptr++) {
            switch (a1->mode) {
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
    if (a1->cnt > -1)
        a1->cnt++;
    if (count == 0 && ++tries < 100)
        goto retry;
    a1->cnt--;
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
    obj->pad19 = (unsigned short)current_region;
    a1->obj = obj;
    obj->x = l40->x;
    obj->y = l40->y;
    obj->z = l40->z;
    if (a1->cnt != 1)
        obj->image2 = list[i] >> 16;
    else
        obj->image2 = 0xffff;
    p = (struct building *)&obj->data;
    if (p->faction_id == 0)
        p->faction_id = faction_find_type_in_region(current_region, 15)->id;
    if (a1->w4 != 1)
        mc_memcpy(p, &l38->buildings[list[i] >> 16], 26, D_00170A64, 483, 4);
    mc_strncpy((char *)p + 26, l38, 4, D_00170A64, 485);
    location_free(&D_001970C8);
    return 1;
}
