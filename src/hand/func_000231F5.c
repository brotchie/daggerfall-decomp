/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000231F5 */
#include "records.h"

extern unsigned char player_environment;
extern void func_0007E815(struct record *, int);
extern void town_grid_visit_near(struct record *, int);
extern void object_foreach_open(struct record *, int);

void collide_for_each_nearby(struct record *object, int callback)
{
    int unused1;
    char unused2[12];

    if (player_environment != 3) {
        if (object->parent->type != 1)
            object_foreach_open(object->parent->children, callback);
        else
            town_grid_visit_near(object, callback);
    } else {
        func_0007E815(object, callback);
    }
}
