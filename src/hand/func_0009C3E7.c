/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0009C3E7 */
#pragma pack(1)
struct obj { char pad0[7]; int x; int y; int z; };
struct marker { char pad0[4]; int f4; int f8; char pad12[5]; };
#pragma pack()
extern unsigned char D_0012AC00;
extern int D_00143550;
extern char D_0017743D[];
extern short D_00178A0E;
extern unsigned char D_00187CA8;
extern unsigned char D_001889BC;
extern int D_001889BD;
extern unsigned char D_00190CE5;
extern unsigned D_00195998;
extern struct obj *D_00195AA4;
extern char *D_00195BE0;
extern char *D_00195BEC;
extern unsigned D_00195BF4;
extern int D_00195D48;
extern unsigned char D_00196271;
extern unsigned char D_00196279;
extern unsigned char D_00196280;
extern unsigned char D_00196294;
extern unsigned char D_0019629B;
extern unsigned char D_001962A2;
extern unsigned char D_001962A8;
extern unsigned char D_001962A9;
extern struct marker *D_00196A9C;
extern int D_001AA678;
extern int D_001AA67C;
extern int D_001AA698;
extern int func_0003C6F1(void);
extern void func_0003CCA3(void);
extern void func_0004AB2F(int);
extern int func_00069938(int, struct obj *, int);
extern void func_0007DDC9(int);
extern void func_0007F1E3(int);
extern int func_0007F2A8(int);
extern void func_000874C0(int);
extern void func_000876AD(int, int, int, int);
extern void func_0009B2E2(int);
extern void func_0009BE38(void);
extern int func_0009CEC4(int, int, int, int, int);
extern int func_0009D61E(void);
extern void func_000A0040(int, int, int, char *, int, int);
extern int func_000C808D(int, int, int, int);

void func_0009C3E7(void)
{
    int r;
    int t;
    unsigned saved;
    int dir;

    dir = (((func_000C808D(D_00195AA4->x, D_00195AA4->z, D_00196A9C[D_001889BD].f4 & 33554431, D_00196A9C[D_001889BD].f8 & 16777215) >> 2) + 32) & 511) >> 6;
    if (((unsigned char)D_0012AC00 & 1) == 0 || ((unsigned char)D_00196279 & 1) != 0) return;
    r = func_0003C6F1();
    if (r != 0 || D_001962A2 != 0) {
        func_0007DDC9(1010);
        if (D_00196271 != 1) return;
    }
    if (func_0007F2A8(func_0009D61E()) != 0)
        func_0007F1E3(func_0009D61E());
    else
        *(int *)(D_00195BE0 + 133) = 0;
    func_00069938(203, D_00195AA4, 110);
    D_00190CE5 = 0;
    func_0009BE38();
    D_001962A9 = 1;
    D_00196294 = 1;
    D_00187CA8 = 1;
    D_001962A8 = 1;
    saved = *(unsigned short *)(D_00195BE0 + 155);
    D_001AA698 = 1;
    t = func_0009CEC4(D_00195AA4->x, D_00195AA4->z, D_001AA678, D_001AA67C, 1);
    D_001AA698 = 0;
    func_0009BE38();
    func_0009B2E2(100);
    if (t != -1)
        func_000876AD(D_001889BC, 1, D_001889BD, 0);
    if (t != -1 && (unsigned short)(D_00178A0E & 3) == 1 && D_00196280 == 0) {
        if (*(unsigned char *)(D_00195BE0 + 67) != 8) {
            t = D_00195BF4 % 1440;
            if (t >= 1080)
                func_0004AB2F(1440 - t + 370);
            else
                func_0004AB2F(360 - t + 10);
        }
    }
    if (D_00196280 != 0 && (*(unsigned char *)(D_00195BE0 + 67) == 8 || ((unsigned short)*(short *)(D_00195BEC + 4) & 16) != 0)) {
        t = D_00195BF4 % 1440;
        if (t < 1080)
            func_0004AB2F(1080 - t + 10);
    }
    if ((unsigned short)(D_00178A0E & 3) == 1)
        *(short *)(D_00195BE0 + 155) = (*(short *)(D_00195BE0 + 32) + *(short *)(D_00195BE0 + 40)) << 6;
    else
        *(short *)(D_00195BE0 + 155) = saved;
    if (t != -1)
        func_000874C0(dir);
    if (D_00195BF4 - D_00195998 > 360) {
        D_00195998 = D_00195BF4;
        func_0003CCA3();
    }
    func_000A0040(655360, 0, 64000, D_0017743D, 782, 4);
    func_000A0040(D_00143550, 0, 64000, D_0017743D, 783, 4);
    D_00196294 = 0;
    D_001962A8 = 0;
    D_001962A9 = 0;
    D_00195D48 = 10000;
    D_0019629B = 0;
}
