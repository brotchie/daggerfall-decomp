/* equip.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern signed char D_001861AA[];
extern iptr D_0018642F[];
extern struct record *player_object;
extern struct record *location_object;
extern struct character *player_character;

extern struct record *item_add_to_container(struct record *, int, int, int);
extern int rand_range(int, int);
extern struct record *object_create_child(struct record *, struct record *, int);
extern int object_new_id(int);
extern int rand();
extern void item_make_random(unsigned short, struct item *);
extern void item_make(int, int, struct item *);
extern void item_make_magic(struct item *, int);
extern void loot_add_potion(struct record *);

void loot_generate(int table_index, struct record *container, int level, int female)
{
    int category;
    int chance;
    int more;
    unsigned short *table;
    struct item *item_data;
    struct record *item;

    table = (unsigned short *)D_0018642F[table_index];
    if (table[0] != 0 || table[1] != 0) {
        item = object_create_child(container, 0, 107);
        item->type = 2;
        item->x = player_object->x;
        item->y = player_object->y;
        item->z = player_object->z;
        item->id = object_new_id(((unsigned)location_object->id) >> 16);
        item_data = &item->data.item;
        item_make(28, 0, item_data);
        item_data->value = level * rand_range(table[0], table[1]);
    }
    for (category = 2; category < 15; category++) {
        if (category <= 5) {
            chance = level * table[category];
        } else {
            chance = table[category];
        }
        if ((rand() % 100) <= chance) {
            more = 1;
            while (more != 0) {
                item = object_create_child(container, 0, 107);
                item->type = 2;
                item->x = player_object->x;
                item->y = player_object->y;
                item->z = player_object->z;
                item->image2 = 998;
                item->id = object_new_id(((unsigned)location_object->id) >> 16);
                if (((int)(unsigned char)D_001861AA[category]) == 255) {
                    if (((int)(unsigned short)(player_character->flags & 1)) == 0) {
                        item_make_random(6, &item->data.item);
                    } else {
                        item_make_random(12, &item->data.item);
                    }
                } else if (((int)(unsigned char)D_001861AA[category]) == 4) {
                    item_make_magic(&item->data.item, -1);
                } else if (((int)(unsigned char)D_001861AA[category]) == 7) {
                    item_make(7, (level + 3) / 5, &item->data.item);
                } else {
                    item_make_random((int)(unsigned short)((unsigned short)(unsigned char)D_001861AA[category]), &item->data.item);
                }
                item_data = &item->data.item;
                if (item_data->group != 3 || item_data->index != 18) item->image2 = 0;
                chance >>= 1;
                if ((rand() % 100) > chance) more = 0;
            }
        }
    }
    if (rand_range(1, 100) < 3) loot_add_potion(container);
    if (rand_range(1, 100) >= 2) return;
    item_add_to_container(container, 27, 4, 0);
}
