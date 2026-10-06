/* inven.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern struct record *player_entity;
extern struct record *found_object;

extern struct record *object_create_child(struct record *, struct record *, int);
extern int object_find(struct record *, iptr (*)());
extern iptr inv_match_arrows(struct record *);
extern void item_make(int, int, struct item *);
extern void inv_store_item(struct record *);

void inv_add_arrows(struct record *owner, int count)
{
    struct record *stack;
    int total;

    found_object = 0;
    object_find(owner->children, inv_match_arrows);
    if (found_object == 0) {
        stack = object_create_child(owner, 0, 107);
        found_object = stack;
        stack->type = 2;
        stack->image2 = 998;
        stack->image = 0;
        item_make(3, 18, &stack->data.item);
        stack->data.item.stack_count = *(signed char *)&count;
        if (owner == player_entity) inv_store_item(stack);
        return;
    }
    total = count + found_object->data.item.stack_count;
    if (total >= 200) total = 199;
    found_object->data.item.stack_count = *(signed char *)&total;
}
