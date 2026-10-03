/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0003B0B4 */
extern char D_0012AC00;
extern short D_0012AC04;
extern short D_0012AC06;
extern unsigned char D_0012B508;
extern char *D_00143550;
extern unsigned char D_00147964;
extern char D_00170B88[];
extern unsigned char D_001940D4;
extern unsigned char D_001940D5;
extern char *D_00195C44;
extern unsigned char D_00196033;
extern unsigned char D_00196034;
extern unsigned char D_0019608F;
extern unsigned char D_00196090;
extern unsigned char D_00196091;
extern unsigned char D_00196271;
extern void func_0003F09F(int, int);
extern void func_0003F358(void);
extern void func_000425F2(void);
extern int func_0006CB53(char *, char *);
extern int func_000A1023();
extern int func_0012B136();
extern int func_0012B2D3();
extern int func_00143914();

int func_0003B0B4(short a1, short a2, short a3, char *a4, unsigned char a5, unsigned char a6)
{
    D_00196271 = 0;
    D_001940D4 &= 254;
    while (D_0012AC00 != 0)
        func_0012B136();
    D_0012B508 = 146;
    D_00196033 = a5;
    D_00196034 = a6;
    D_0019608F = a2;
    D_00196090 = a3;
    D_00196091 = 0;
    if (a4 != 0) {
        func_0006CB53(a4, D_00195C44);
        func_000A1023(D_00143550, D_00195C44, 64000, D_00170B88, 399, 4);
    } else {
        func_00143914(0);
    }
    D_001940D5 |= 128;
    func_0003F09F(a1, 5);
    D_00147964 &= 254;
    while (D_00196271 == 0) {
        func_000425F2();
        func_0012B136();
        func_0003F358();
        func_0012B2D3(D_0012AC04, D_0012AC06);
        func_000A1023(655360, D_00143550, 64000, D_00170B88, 412, 4);
    }
    return D_00196271 - 1;
}
