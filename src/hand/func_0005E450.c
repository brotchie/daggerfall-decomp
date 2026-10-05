/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0005E450 */
#include "records.h"

extern short *item_group_templates[];
extern void item_init_from_template(unsigned short, short, short, struct item *);
extern void item_make_magic(struct item *, int);
extern void item_make_artifact(struct item *, int);
extern int rand_range(int, int);

void item_make_random(unsigned short group, struct item *item)
{
    int r;
    int i;

    switch (group) {
    case 5:
        item_make_artifact(item, rand_range(0, 22));
        break;
    case 4:
        item_make_magic(item, -1);
        break;
    case 11:
        item_init_from_template(287, 27, 8, item);
        break;
    default:
        i = 0;
        while (item_group_templates[group][i++] != -1)
            ;
        r = rand_range(0, i - 2);
        item_init_from_template(item_group_templates[group][r], group, r, item);
        break;
    }
}
