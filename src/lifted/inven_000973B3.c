/* inven.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern struct record *player_entity;
extern struct record *found_object;

extern struct record *object_create_child(struct record *, int, int);
extern int object_find(struct record *, int);
extern int inv_match_arrows(int);
extern void item_make(int, int, struct item *);
extern void inv_store_item(struct record *);

void inv_add_arrows(struct record *a1, int a2)
{
    struct record *l_18;
    int l_14;

    found_object = 0;
    object_find(a1->children, (int)inv_match_arrows);
    if (found_object == 0) {
        l_18 = object_create_child(a1, 0, 107);
        found_object = l_18;
        l_18->type = 2;
        l_18->image2 = 998;
        l_18->image = 0;
        item_make(3, 18, &l_18->data.item);
        l_18->data.item.stack_count = *(signed char *)&a2;
        if (a1 == player_entity) inv_store_item(l_18);
        return;
    }
    l_14 = a2 + found_object->data.item.stack_count;
    if (l_14 >= 200) l_14 = 199;
    found_object->data.item.stack_count = *(signed char *)&l_14;
}
