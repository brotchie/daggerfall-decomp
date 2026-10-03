/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00057147 */
struct rec { char a, b, c, d, e, f; char pad[4]; };
extern struct rec D_00199868[];

void func_00057147(short i, short a, short b, short c, char d, char e, char f)
{
    D_00199868[i].a = a;
    D_00199868[i].b = b;
    D_00199868[i].c = c;
    D_00199868[i].d = d;
    D_00199868[i].e = e;
    D_00199868[i].f = f;
}
