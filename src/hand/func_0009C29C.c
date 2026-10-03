/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0009C29C */
struct ent17 { char pad[4]; int x; int y; char pad2[5]; };
extern unsigned char D_0012AC00;
extern unsigned char D_00187CA8;
extern unsigned char D_001889BC;
extern int D_001889BD;
extern unsigned char D_00190CE5;
extern char *D_00195AA4;
extern char *D_00195BE0;
extern unsigned char D_00196279;
extern unsigned char D_00196294;
extern unsigned char D_001962A9;
extern struct ent17 *D_00196A9C;
extern int D_001AA678;
extern int D_001AA67C;
extern int func_00069938(int, char *, int);
extern void func_000874C0(int);
extern void func_000876AD(int, int, int, int);
extern void func_0009B2E2(int);
extern void func_0009BE38(void);
extern int func_0009CEC4(int, int, int, int, int);
extern int func_000C808D(int, int, int, int);

void func_0009C29C(void)
{
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    l_18 = (((func_000C808D(*(int *)(D_00195AA4 + 7), *(int *)(D_00195AA4 + 15), D_00196A9C[D_001889BD].x & 0x1ffffff, D_00196A9C[D_001889BD].y & 0xffffff) >> 2) + 32) & 511) >> 6;
    if ((D_0012AC00 & 1) == 0 || (D_00196279 & 1) != 0)
        return;
    func_00069938(203, D_00195AA4, 110);
    D_00190CE5 = 0;
    func_0009BE38();
    D_001962A9 = 1;
    D_00196294 = 1;
    D_00187CA8 = 1;
    l_1C = *(unsigned short *)(D_00195BE0 + 155);
    l_20 = func_0009CEC4(*(int *)(D_00195AA4 + 7), *(int *)(D_00195AA4 + 15), D_001AA678, D_001AA67C, 1);
    func_0009BE38();
    func_0009B2E2(100);
    if (l_20 != -1)
        func_000876AD(D_001889BC, 1, D_001889BD, 0);
    if (l_20 != -1)
        func_000874C0(l_18);
    D_001962A9 = 0;
    D_00196294 = 0;
}
