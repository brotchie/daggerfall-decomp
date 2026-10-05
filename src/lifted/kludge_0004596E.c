/* kludge.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"


extern struct record *object_create_child(struct record *, int, int);
extern int object_reparent(struct record *, struct record *);
extern void item_make_random(unsigned short, struct item *);
extern void item_make(int, int, struct item *);

struct record *kludge_add_random_item(struct record *a1, int a2)
{
    struct record *l_20;
    struct record *l_1C;
    struct item *l_18;

    l_20 = object_create_child(a1, 0, 107);
    l_20->type = 2;
    l_18 = &l_20->data.item;
    item_make_random((int)(unsigned short)*(short *)&a2, l_18);
    if ((l_18->item_flags & 8) != 0) {
        l_1C = object_create_child(a1, 0, 107);
        l_1C->type = 2;
        object_reparent(l_1C, l_20);
        l_18 = &l_1C->data.item;
        item_make(1, 1, l_18);
        return l_1C;
    }
    return l_20;
}
