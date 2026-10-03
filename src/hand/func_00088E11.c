/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00088E11 */
extern char D_001832C4[];
extern char D_001940D8[];
extern char D_00195BF4[];
extern char D_001A99F4[];
extern void func_00088C0E(int);
extern void func_0008B054(int, int);

void func_00088E11(int a1, int a2, int a3)
{
    *(signed char *)D_001940D8 &= 254;
    *(int *)D_001A99F4 = *(unsigned short *)((char *)a1 + a2 * 2 + 145) + *(int *)D_00195BF4;
    func_0008B054((int)D_001832C4, (int)func_00088C0E);
}
