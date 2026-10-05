/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000231F5 */
#include "records.h"

extern unsigned char player_environment;
extern void func_0007E815(struct record *, int);
extern void town_grid_visit_near(struct record *, int);
extern void object_foreach_open(struct record *, int);

void collide_for_each_nearby(struct record *a1, int a2)
{
    int l_18;
    char l_28[12];

    if (player_environment != 3) {
        if (a1->parent->type != 1)
            object_foreach_open(a1->parent->children, a2);
        else
            town_grid_visit_near(a1, a2);
    } else {
        func_0007E815(a1, a2);
    }
}
