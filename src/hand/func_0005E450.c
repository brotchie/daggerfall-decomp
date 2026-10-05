/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0005E450 */
#include "records.h"

extern short *D_00185F88[];
extern void item_init_from_template(unsigned short, short, short, struct item *);
extern void item_make_magic(struct item *, int);
extern void item_make_artifact(struct item *, int);
extern int rand_range(int, int);

void item_make_random(unsigned short a1, struct item *a2)
{
    int r;
    int i;

    switch (a1) {
    case 5:
        item_make_artifact(a2, rand_range(0, 22));
        break;
    case 4:
        item_make_magic(a2, -1);
        break;
    case 11:
        item_init_from_template(287, 27, 8, a2);
        break;
    default:
        i = 0;
        while (D_00185F88[a1][i++] != -1)
            ;
        r = rand_range(0, i - 2);
        item_init_from_template(D_00185F88[a1][r], a1, r, a2);
        break;
    }
}
