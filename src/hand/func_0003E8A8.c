/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0003E8A8 */
extern char D_00170D55[];
extern short D_00178A08;
extern int D_00199650;
extern int func_0003D412(int, int, int);
extern void func_0003DCF4(int, int);
extern void func_0003E942(int);
extern void func_000A0024(int, char *, int);

int func_0003E8A8(short a1, int a2, short a3)
{
    int h;
    int r;

    h = func_0003D412(a1, (short)(a3 | 0x8002), D_00178A08);
    func_0003DCF4(h, a2);
    if (D_00199650 != 0) {
        func_0003E942(a2);
        r = 0;
    } else {
        r = 1;
    }
    if (h != 0 && h != 0x97979797) {
        func_000A0024(h, D_00170D55, 599);
        h = 0x97979797;
    }
    return r;
}
