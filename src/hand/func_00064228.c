/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00064228 */
#include "records.h"
extern char D_00175940[];
extern struct record *location_object;
extern struct link links[];
extern int link_count;
extern char D_001A3A80;
extern char D_001A3A81;
extern void fatal_error(char *);
extern struct record *object_find_by_id(struct record *, iptr);

void links_resolve(void)
{
    struct link *e;
    struct link *end;

    if (link_count >= 1024)
        fatal_error(D_00175940);
    for (e = links, end = links + link_count; e < end; e++) {
        if (e->trigger == 6)
            e->trigger = 2;
        if (e->param == 108)
            e->param = 100;
        e->object = object_find_by_id(location_object, location_object->id + e->object_id - 1);
        if (e->object != 0)
            e->object->link_flag = 255;
    }
    D_001A3A80 = D_001A3A81 = 0;
}
