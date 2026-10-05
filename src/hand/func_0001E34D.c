/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0001E34D */
struct E { char pad[12]; unsigned char kind; char pad2[4]; };
extern int D_00196A28;
extern struct E *D_00196A9C;
extern void location_load_dungeon(int, int);

void func_0001E34D(int a1, int a2, int a3)
{
    struct E *p;
    int i;
    int n;

    p = D_00196A9C;
    for (i = n = 0; i < D_00196A28; i++, p++) {
        if (p->kind == a2) {
            if (a3-- == 0) {
                location_load_dungeon(a1, n);
                return;
            }
            n++;
        }
    }
    location_load_dungeon(a1, 0);
}
