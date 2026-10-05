/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0008E7E0 */
#include "records.h"

extern short object_count_result;
extern short D_001A9B42;
extern void object_foreach(struct record *, void (*)());
extern void object_count_type_cb();

int object_count_type(struct record *a1, short a2)
{
    D_001A9B42 = a2;
    object_count_result = 0;
    object_foreach(a1, object_count_type_cb);
    return object_count_result;
}
