/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0008E6C5 */
#include "records.h"

extern struct record *found_object;
extern short D_001A9B40;
extern short D_001A9B42;
extern void object_foreach(struct record *, int);
extern void object_find_item_cb(struct record *);

struct record *object_find_item(struct record *root, short group, short index)
{
    D_001A9B42 = index;
    D_001A9B40 = group;
    object_foreach(root, (int)object_find_item_cb);
    if (D_001A9B42 == -1)
        return found_object;
    return 0;
}
