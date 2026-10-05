/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0008E7E0 */
extern short D_001A9B3C;
extern short D_001A9B42;
extern void object_foreach(int, void (*)());
extern void object_count_type_cb();

int object_count_type(int a1, short a2)
{
    D_001A9B42 = a2;
    D_001A9B3C = 0;
    object_foreach(a1, object_count_type_cb);
    return D_001A9B3C;
}
