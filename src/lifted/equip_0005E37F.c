/* equip.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern char item_group_templates[];

extern int rand_range(int, int);
extern void item_init_from_template(unsigned short, short, short, struct item *);
extern void item_make_magic(struct item *, int);
extern void item_make_artifact(struct item *, int);

void item_make_in_range(unsigned short group, int min_index, int max_index, struct item *item)
{
    int index;

    if (min_index == max_index) {
        index = min_index;
    } else {
        index = rand_range(min_index, max_index);
    }
    switch ((unsigned short)*(int *)&group) {
    case 5:
        item_make_artifact(item, rand_range(min_index, max_index));
        return;
    case 4:
        item_make_magic(item, -1);
        return;
    case 11:
        item_init_from_template(287, 27, 8, item);
        return;
    default:
        item_init_from_template((int)(unsigned short)*(short *)((char *)(iptr)(*(char **)(item_group_templates + (((int)(unsigned short)group) << 2)) + (index * 2))), (int)(short)group, (int)(short)*(short *)&index, item);
    }
}
