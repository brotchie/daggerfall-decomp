/* click.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern char D_0017629C[];
extern char D_001762B5[];
extern struct record *player_entity;

extern int object_weight(struct record *);
extern iptr hud_message_add(char *);
extern int carry_capacity(void);
extern void quest_raise_event(short, struct record *, struct record *);
extern void inventory_open_container(struct record *, int, int);
extern void inv_store_item(struct record *);

void pick_up_item(struct record *object)
{
    struct item *item;
    int weight;
    int carried_weight;
    int capacity;

    item = &object->data.item;
    if (object->children != 0 && object->children->type == 2) {
        inventory_open_container(object, 0, 5);
        return;
    }
    weight = object_weight(object);
    carried_weight = object_weight(player_entity);
    capacity = carry_capacity() << 2;
    if (weight > capacity) {
        hud_message_add(D_0017629C);
        return;
    }
    if ((weight + carried_weight) > capacity) {
        hud_message_add(D_001762B5);
        return;
    }
    quest_raise_event(3, object, 0);
    inv_store_item(object);
}
