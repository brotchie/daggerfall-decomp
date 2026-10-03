/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000699D8 */
struct msg {
    unsigned char type;
    char pad1[6];
    int a;          /* 7 */
    int b;          /* 11 */
    int c;          /* 15 */
    char pad13[53];
};
struct slot { int used; char pad[264]; };
extern int D_00195CB4;
extern struct slot D_001A3BE4[];
extern char D_001A3F5D;
extern int func_00068F5E(int, int, struct msg *, int);
extern int func_00085A51(int);

int func_000699D8(int a1, int a2, int a3, int a4, int a5)
{
    int h;
    struct msg m;
    int n;

    if (D_001A3F5D == 0)
        return -1;
    m.type = 0;
    m.a = a2;
    m.b = a3;
    m.c = a4;
    h = func_00085A51(a1);
    n = func_00068F5E(h, D_00195CB4, &m, a5);
    if (n > -1)
        D_001A3BE4[n].used = 0;
    return n;
}
