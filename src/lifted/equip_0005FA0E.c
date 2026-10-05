/* equip.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern signed char D_001861AA[];
extern int D_0018642F[];
extern struct record *player_object;
extern struct record *D_00195AC4;
extern struct character *player_character;

extern int item_add_to_container(struct record *, int, int, int);
extern int rand_range(int, int);
extern struct record *object_create_child(struct record *, int, int);
extern int object_new_id(int);
extern int rand();
extern void item_make_random(unsigned short, struct item *);
extern void item_make(int, int, struct item *);
extern void item_make_magic(struct item *, int);
extern void func_000614FB(struct record *);

void func_0005FA0E(int a1, struct record *a2, int a3, int a4)
{
    int l_20;
    int l_1C;
    int l_18;
    int l_14;
    struct item *l_10;
    struct record *l_C;

    l_14 = D_0018642F[a1];
    if (*(short *)((char *)l_14) != 0 || *(short *)((char *)l_14 + 2) != 0) {
        l_C = object_create_child(a2, 0, 107);
        l_C->type = 2;
        l_C->x = player_object->x;
        l_C->y = player_object->y;
        l_C->z = player_object->z;
        l_C->id = object_new_id(((unsigned)D_00195AC4->id) >> 16);
        l_10 = &l_C->data.item;
        item_make(28, 0, l_10);
        l_10->value = a3 * rand_range((int)(unsigned short)*(short *)((char *)l_14), (int)(unsigned short)*(short *)((char *)l_14 + 2));
    }
    for (l_20 = 2; l_20 < 15; l_20++) {
        if (l_20 <= 5) {
            l_1C = a3 * ((int)(unsigned short)*(short *)((char *)((l_20 * 2) + l_14)));
        } else {
            l_1C = (int)(unsigned short)*(short *)((char *)((l_20 * 2) + l_14));
        }
        if ((rand() % 100) <= l_1C) {
            l_18 = 1;
            while (l_18 != 0) {
                l_C = object_create_child(a2, 0, 107);
                l_C->type = 2;
                l_C->x = player_object->x;
                l_C->y = player_object->y;
                l_C->z = player_object->z;
                l_C->image2 = 998;
                l_C->id = object_new_id(((unsigned)D_00195AC4->id) >> 16);
                if (((int)(unsigned char)D_001861AA[l_20]) == 255) {
                    if (((int)(unsigned short)(player_character->flags & 1)) == 0) {
                        item_make_random(6, &l_C->data.item);
                    } else {
                        item_make_random(12, &l_C->data.item);
                    }
                } else if (((int)(unsigned char)D_001861AA[l_20]) == 4) {
                    item_make_magic(&l_C->data.item, -1);
                } else if (((int)(unsigned char)D_001861AA[l_20]) == 7) {
                    item_make(7, (a3 + 3) / 5, &l_C->data.item);
                } else {
                    item_make_random((int)(unsigned short)((unsigned short)(unsigned char)D_001861AA[l_20]), &l_C->data.item);
                }
                l_10 = &l_C->data.item;
                if (l_10->group != 3 || l_10->index != 18) l_C->image2 = 0;
                l_1C >>= 1;
                if ((rand() % 100) > l_1C) l_18 = 0;
            }
        }
    }
    if (rand_range(1, 100) < 3) func_000614FB(a2);
    if (rand_range(1, 100) >= 2) return;
    item_add_to_container(a2, 27, 4, 0);
}
