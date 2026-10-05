/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0008E6C5 */
extern int D_00195AF4;
extern short D_001A9B40;
extern short D_001A9B42;
extern void object_foreach(int, int);
extern void object_find_item_cb(int);

int object_find_item(int a1, short a2, short a3)
{
    D_001A9B42 = a3;
    D_001A9B40 = a2;
    object_foreach(a1, (int)object_find_item_cb);
    if (D_001A9B42 == -1)
        return D_00195AF4;
    return 0;
}
