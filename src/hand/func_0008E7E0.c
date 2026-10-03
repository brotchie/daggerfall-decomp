/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0008E7E0 */
extern short D_001A9B3C;
extern short D_001A9B42;
extern void func_0008E3F7(int, void (*)());
extern void func_0008E7AE();

int func_0008E7E0(int a1, short a2)
{
    D_001A9B42 = a2;
    D_001A9B3C = 0;
    func_0008E3F7(a1, func_0008E7AE);
    return D_001A9B3C;
}
