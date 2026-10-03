/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0008E6C5 */
extern int D_00195AF4;
extern short D_001A9B40;
extern short D_001A9B42;
extern void func_0008E3F7(int, int);
extern void func_0008E649(int);

int func_0008E6C5(int a1, short a2, short a3)
{
    D_001A9B42 = a3;
    D_001A9B40 = a2;
    func_0008E3F7(a1, (int)func_0008E649);
    if (D_001A9B42 == -1)
        return D_00195AF4;
    return 0;
}
