/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0008E7E0 */
#include "records.h"

extern short object_count_result;
extern short D_001A9B42;
extern void object_foreach(struct record *, void (*)());
extern void object_count_type_cb(struct record *);

int object_count_type(struct record *root, short type)
{
    D_001A9B42 = type;
    object_count_result = 0;
    object_foreach(root, object_count_type_cb);
    return object_count_result;
}
