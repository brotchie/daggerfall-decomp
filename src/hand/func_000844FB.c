/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000844FB */
#include "records.h"

extern struct record *D_00195AC4;
extern struct location *current_location;
extern struct record *object_create_child(struct record *, struct record *, int);

struct record *rmb_make_marker(struct record *a1, int a2)
{
    struct record *l_20;
    int l_1C;
    int l_18;

    l_1C = (a2 & 31) - 2;
    if (l_1C == 13 || l_1C == 14) {
        l_18 = 659;
        l_20 = object_create_child(a1, 0, l_18);
        l_20->mobile_id = 0;
    } else {
        l_20 = object_create_child(a1, 0, 0);
        l_20->mobile_id = 0;
    }
    l_20->type = 34;
    l_20->image2 = 0;
    l_20->mobile_id = 0;
    l_20->image = a2;
    if (l_1C == 9 || l_1C == 16) {
        l_20->id = D_00195AC4->id + current_location->marker_counter++;
    } else {
        l_20->id = D_00195AC4->id + current_location->object_counter++;
    }
    l_20->flags = 1;
    return l_20;
}
