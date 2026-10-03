/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0007D62B */
extern char D_0012AC00;
extern unsigned char D_0012B508;
extern unsigned char D_001940D4;
extern unsigned char D_00196033;
extern unsigned char D_00196034;
extern unsigned char D_00196035;
extern unsigned char D_0019608F;
extern unsigned char D_00196090;
extern unsigned char D_00196091;
extern char D_00196271;
extern void func_0003F09F(int, int);
extern void func_0012B136(void);

void func_0007D62B(short a1, short a2, short a3, short a4, unsigned char a5, unsigned char a6, unsigned char a7)
{
    D_00196271 = 0;
    D_001940D4 |= 1;
    while (D_0012AC00 != 0)
        func_0012B136();
    D_0012B508 = 146;
    D_00196033 = a5;
    D_00196034 = a6;
    D_00196035 = a7;
    D_0019608F = a2;
    D_00196090 = a3;
    D_00196091 = a4;
    func_0003F09F(a1, 5);
}
