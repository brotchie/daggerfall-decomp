/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00014334 */
struct sel { int flags; struct obj *obj; };
struct obj { unsigned char type; short id; char pad[44]; int owner; };
extern char key_down_alt;
extern int frame_counter;
extern int D_00196478;
extern struct sel *D_00196484;

int func_00014334(struct obj *a1)
{
    int l_24;
    int l_20;
    int l_1C;

    if (D_00196484->flags & 1)
        return 0;
    switch (a1->type) {
    case 2:
    case 8:
    case 18:
    case 33:
    case 34:
    case 44:
    case 53:
        if (a1->owner == D_00196478 && (frame_counter & 0xffff) == (a1->id & 0xffff)) {
            if (key_down_alt && a1->type != 34)
                return 0;
            D_00196484->flags |= 3;
            D_00196484->obj = a1;
            return 1;
        }
    default:
        return 0;
    }
}
