/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00010010 */
extern int D_0012AA04;
extern unsigned char D_0012B504;
extern char D_00170004[];
extern char D_0017000B[];
extern char D_00170035[];
extern char D_00170049[];
extern int D_0018DBFC;
extern int D_0018DC0C;
extern int D_0018DC1C;
extern int D_0018DC24;
extern int D_001959AC;
extern int D_00195D7C;
extern short D_00195F64;
extern short D_00195F66;
extern unsigned char D_00196283;
extern unsigned char D_001962A5;
extern void func_0001025B(void);
extern void func_00013A52(void);
extern void func_00019676(int);
extern void func_0002B5EF(void);
extern void func_0003A325(void);
extern void func_0003A3CE(void);
extern void func_000425F2(void);
extern void func_00044D03(int);
extern void func_0004AB2F(int);
extern void func_0004BA70(void);
extern void func_0004ED36(void);
extern void func_0004FDB8(void);
extern void func_0004FF11(void);
extern void func_00050E3F(void);
extern void func_0005A661(int, int, int, int);
extern void func_0005DB95(int);
extern void func_00069E3C(void);
extern void func_0006A606(void);
extern void func_00081425(void);
extern void func_000827C3(char *);
extern void func_000994F0(void *, int);
extern void func_0009960A(void);
extern void func_0009DB11(int);
extern void func_0009DBF9(void);
extern void func_0009DC49(int);
extern void func_0009DEA7(int);
extern void func_000C0520(void);
extern void func_000C7F00(void);
extern void func_000CDD81(int);
extern int func_000CE7BB(void);
extern void func_000CE8C4(void);
extern void func_0012A254(int);
extern void func_0012B47E(short *, short *);
#pragma aux func_0009DA1C parm routine [];
extern void func_0009DA1C(int, char *);
#pragma aux func_0009DB3F parm routine [];
extern void func_0009DB3F(void (*)(void));
#pragma aux func_0009DBFE parm routine [];
extern void func_0009DBFE(void (*)(void));
extern void func_0009DAEE(char *, ...);
extern int func_0009DC59(char *, ...);

int func_00010010(short a1, char **a2)
{
    int l_3C;
    char l_u0;                  /* unused, but they have slots */
    int l_u1;
    int l_u2;
    int l_44;
    int l_u3;
    char l_u4;

    if (a1 != 2) {
        func_0009DA1C(54, D_00170004);
        func_0009DAEE(D_0017000B);
        func_0009DB11(-1);
    }
    func_00069E3C();
    func_0009DB3F(func_0006A606);
    func_0009DBFE(func_0009DBF9);
    func_0009960A();
    func_000C0520();
    func_000994F0(func_00010010, 2048000);
    func_0009DC49(*(int *)0x46c);
    D_0018DC1C = *(int *)0x46c;
    func_000827C3(a2[1]);
    l_44 = func_0009DC59(D_00170035, 546, 384);
    if (l_44 < 0) {
        func_0009DA1C(75, D_00170004);
        func_0009DAEE(D_00170049);
        func_0009DB11(-1);
    }
    func_0009DEA7(l_44);
    func_0005A661(0, 0, 319, 199);
    func_0004FDB8();
    func_00044D03(0);
    func_0004ED36();
    func_0004FF11();
    func_0004BA70();
    func_0003A325();
    for (l_44 = 0; l_44 < 12; l_44++) {
        D_001962A5 = (l_44 == 11);
        func_00019676(1);
        func_00019676(2);
    }
restart:
    func_0003A3CE();
    D_0018DC0C = *(int *)0x46c + 18;
    func_0004AB2F(1);
    D_0012AA04 = 23;
    func_0012A254(8);
    func_000CE8C4();
    D_0018DC24 = 1;
    for (;;) {
        D_0018DBFC = 0;
        func_0001025B();
        D_0018DBFC = 1;
        if (D_00195D7C < 0) {
            func_00050E3F();
            D_00195D7C = 0;
            goto restart;
        }
        func_0005DB95(0);
        l_3C = func_000CE7BB();
        D_0018DBFC = 2;
        func_0012B47E(&D_00195F64, &D_00195F66);
        func_00081425();
        func_000425F2();
        func_0002B5EF();
        D_0018DBFC = 3;
        D_0012B504 = D_00196283;
        func_000CDD81(1);
        func_00013A52();
        D_001959AC++;
        func_000C7F00();
    }
    return 0;
}
