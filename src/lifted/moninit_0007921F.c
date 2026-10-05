/* moninit.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"


extern int rand_range(int, int);
extern struct record *object_create_child(struct record *, int, int);
extern int object_new_id(int);
extern void func_0005E37F(unsigned short, int, int, struct item *);

struct record *monster_make_item(struct record *a1, int a2, int a3, int a4, int a5, int a6)
{
    struct record *l_14;
    struct item *l_10;

    if (rand_range(1, 100) > a6) return 0;
    l_14 = object_create_child(a1, 0, 107);
    l_14->type = 2;
    l_14->id = object_new_id(((unsigned)a1->id) >> 16);
    l_10 = &l_14->data.item;
    do {
        func_0005E37F((int)(unsigned short)*(short *)&a2, a3, a4, l_10);
    } while (l_10->index == a5);
    return l_14;
}
