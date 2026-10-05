/* click.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern char D_0017629C[];
extern char D_001762B5[];
extern struct record *player_entity;

extern int object_weight(struct record *);
extern int hud_message_add(int);
extern int carry_capacity(void);
extern void quest_raise_event(int, struct record *, int);
extern void inventory_open_container(struct record *, int, int);
extern void inv_store_item(struct record *);

void pick_up_item(struct record *a1)
{
    struct item *l_24;
    int l_20;
    int l_1C;
    int l_18;

    l_24 = &a1->data.item;
    if (a1->children == 0) goto L7586E;
    if (a1->children->type == 2) goto L75870;
L7586E:;
    goto L75881;
L75870:;
    inventory_open_container(a1, 0, 5);
    return;
L75881:;
    l_20 = object_weight(a1);
    l_1C = object_weight(player_entity);
    l_18 = carry_capacity() << 2;
    if (l_20 <= l_18) goto L758B8;
    hud_message_add((int)D_0017629C);
    return;
L758B8:;
    if ((l_20 + l_1C) <= l_18) goto L758CF;
    hud_message_add((int)D_001762B5);
    return;
L758CF:;
    quest_raise_event(3, a1, 0);
    inv_store_item(a1);
}
