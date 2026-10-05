/* kludge.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"


extern struct record *object_create_child(struct record *, struct record *, int);
extern struct record *object_reparent(struct record *, struct record *);
extern void item_make_random(unsigned short, struct item *);
extern void item_make(int, int, struct item *);

struct record *kludge_add_random_item(struct record *container, int group)
{
    struct record *object;
    struct record *bottle;
    struct item *item;

    object = object_create_child(container, 0, 107);
    object->type = 2;
    item = &object->data.item;
    item_make_random((int)(unsigned short)*(short *)&group, item);
    if ((item->item_flags & 8) != 0) {
        bottle = object_create_child(container, 0, 107);
        bottle->type = 2;
        object_reparent(bottle, object);
        item = &bottle->data.item;
        item_make(1, 1, item);
        return bottle;
    }
    return object;
}
