/* equip.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern char D_00185F88[];

extern int rand_range(int, int);
extern void item_init_from_template(unsigned short, short, short, struct item *);
extern void item_make_magic(struct item *, int);
extern void item_make_artifact(struct item *, int);

void func_0005E37F(short a1, int a2, int a3, struct item *a4)
{
    int l_14;

    if (a2 == a3) {
        l_14 = a2;
    } else {
        l_14 = rand_range(a2, a3);
    }
    switch ((unsigned short)*(int *)&a1) {
    case 5:
        item_make_artifact(a4, rand_range(a2, a3));
        return;
    case 4:
        item_make_magic(a4, -1);
        return;
    case 11:
        item_init_from_template(287, 27, 8, a4);
        return;
    default:
        item_init_from_template((int)(unsigned short)*(short *)((char *)(int)(*(char **)(D_00185F88 + (((int)(unsigned short)a1) << 2)) + (l_14 * 2))), (int)(short)a1, (int)(short)*(short *)&l_14, a4);
    }
}
