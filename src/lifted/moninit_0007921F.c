/* moninit.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"


extern int rand_range(int, int);
extern struct record *object_create_child(struct record *, struct record *, int);
extern int object_new_id(int);
extern void item_make_in_range(unsigned short, int, int, struct item *);

struct record *monster_make_item(struct record *monster, int group, int index_lo, int index_hi, int excluded_index, int chance)
{
    struct record *item;
    struct item *item_data;

    if (rand_range(1, 100) > chance) return 0;
    item = object_create_child(monster, 0, 107);
    item->type = 2;
    item->id = object_new_id(((unsigned)monster->id) >> 16);
    item_data = &item->data.item;
    do {
        item_make_in_range((int)(unsigned short)*(short *)&group, index_lo, index_hi, item_data);
    } while (item_data->index == excluded_index);
    return item;
}
