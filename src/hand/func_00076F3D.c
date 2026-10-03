/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00076F3D */
struct move { int x, y, z; int f12, f16, f20; char *name; int f28; };
extern char D_00187B44[];
extern int D_00195C74;
extern unsigned char D_00196277;
extern short D_00196D64;
extern int func_0002257C(char *, int, struct move *, int);

int func_00076F3D(char *a1)
{
    int l_1C;
    int l_20;
    struct move m;

    l_20 = D_00196277;
    l_1C = D_00195C74;
    m.x = *(int *)(a1 + 7);
    m.y = *(int *)(a1 + 11);
    m.z = *(int *)(a1 + 15);
    m.f12 = *(short *)(a1 + 1);
    m.f16 = *(short *)(a1 + 3);
    m.f20 = *(short *)(a1 + 5);
    m.name = D_00187B44;
    func_0002257C(a1, 0, &m, 0);
    D_00196277 = l_20;
    D_00195C74 = l_1C;
    return (D_00196D64 & 10) == 0 ? 1 : 0;
}
