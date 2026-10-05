/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000306FE */
#include "records.h"

extern int D_00196A28;
extern struct map_location *D_00196A9C;

void qaction_op19_reveal_location(struct quest *a1, struct qbn_op *a2, int a3)
{
    struct record *l_1C;
    struct map_location *p;
    int n;
    int i;

    l_1C = a2->args[1].object;
    n = l_1C->image;
    p = D_00196A9C;
    for (i = 0; i < D_00196A28; i++, p++) {
        if (p->dungeon_type != 255)
            if (n-- == 0) break;
    }
    p->x_type_flags |= 0x40000000;
}
