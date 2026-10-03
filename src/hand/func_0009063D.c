/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0009063D */
struct skill { short value; char pad[4]; };
struct player {
    unsigned char f0;
    char pad1[31];
    short f32[8];               /* 0x20 */
    short f48[8];               /* 0x30 */
    unsigned short f64;         /* 0x40 */
    char pad42;
    unsigned char f67;          /* 0x43 */
    char pad44[20];
    int f88;                    /* 0x58 */
    char pad5c[37];
    unsigned char f129;         /* 0x81 */
    char pad82[27];
    struct skill skills[35];    /* 0x9d */
};
struct career { char pad[16]; unsigned char skill[12]; };
#define FREED ((void *)0x97979797)
#define FREE(p, line) if ((p) != 0 && (p) != FREED) { func_000A0024((p), D_00176F41, (line)); (p) = FREED; }
extern unsigned char D_0012B508;
extern char D_00176F28[];
extern char D_00176F34[];
extern char D_00176F41[];       /* __FILE__ */
extern char D_00176F4C[];
extern char D_00176F5B[];
extern char D_00176F68[];
extern char D_00176F75[];
extern char D_00176F82[];
extern char D_00176F8F[];
extern char D_00176F9C[];
extern char D_00176FA9[];
extern char D_00176FB6[];
extern unsigned char D_001881FC[];
extern char D_001903A4[];
extern int D_00190BE8;
extern unsigned char D_00190CEE[];
extern short D_00190D64;
extern short D_00190D70;
extern short D_00190DEA;
extern short D_00190DEC;
extern short D_00190DEE;
extern unsigned char D_001940D5;
extern void *D_00195B5C;
extern void *D_00195B60;
extern struct player *D_00195BE0;
extern void *D_00195BE8;
extern struct career *D_00195BEC;
extern int D_00195C44;
extern void *D_001AA410;
extern void *D_001AA414;
extern unsigned char D_001AA418;
extern unsigned char D_001AA41A;
extern int func_0003D113(void);
extern void func_0003F358(void);
extern void func_00053A89(struct player *, short, int (*)(void));
extern void *func_0006CB53(char *, int);
extern int func_0007D6AE(int, int);
extern int func_00090C3A(void);
extern void func_00090E79(void);
extern int func_00090FA1(int, int);
extern void func_0009169B(int);
extern void func_00091A99(void);
extern void func_00091B80(int);
extern void func_000A0024(void *, char *, int);
extern void func_000A1023(void *, void *, int, char *, int, int);
#pragma aux func_000A0ED9 parm routine [];
extern void func_000A0ED9(int, char *);
extern int func_000A0F5C(char *, char *, ...);

int func_0009063D(void)
{
    int i;
    int unused1;            /* never used, but they have stack slots */
    int unused2;

    D_001AA418 = 0;
    for (i = 0; i < 35; i++) {
        if (D_00195BE0->skills[i].value == 0)
            D_00195BE0->skills[i].value = func_0007D6AE(3, 6);
    }
    for (i = 0; i < 12; i++) {
        D_00190CEE[i] = D_00195BE0->skills[D_00195BEC->skill[i]].value += D_001881FC[i];
    }
    D_00195BE0->f129 = 1;
    func_0006CB53(D_00176F28, D_00195C44);
    D_00190DEA = D_00190DEC = D_00190DEE = D_00190D64 = D_00190D70 = 0;
    D_00190BE8 = 0;
    D_00195BE0->f0 = 0;
    D_0012B508 = 146;
    for (i = 0; i < 8; i++) {
        D_00195BE0->f32[i] -= 5;
        D_00195BE0->f48[i] -= 5;
    }
    while (D_00195BE0->f0 == 0) {
        D_001AA41A = 1;
        D_00195BE8 = func_0006CB53(D_00176F34, 0);
        func_00053A89(D_00195BE0, 31, func_00090C3A);
        FREE(D_00195BE8, 114);
    }
    func_000A0ED9(117, D_00176F41);
    func_000A0F5C(D_001903A4, D_00176F4C, D_00195BE0->f64 & 1, D_00195BE0->f67);
    D_001AA410 = func_0006CB53(D_001903A4, 0);
    D_001AA41A = 2;
    D_00195BE8 = func_0006CB53(D_00176F5B, 0);
    func_00090FA1(33, 34);
    FREE(D_00195BE8, 122);
    FREE(D_001AA410, 123);
    func_00091A99();
    D_001AA41A = 4;
    D_00195BE8 = func_0006CB53(D_00176F68, 0);
    D_00195B5C = func_0006CB53(D_00176F75, 0);
    func_00091B80(0);
    func_00090FA1(20, 32);
    FREE(D_00195BE8, 131);
    D_001AA41A = 8;
    D_00195BE8 = func_0006CB53(D_00176F82, 0);
    D_00195B60 = func_0006CB53(D_00176F8F, 0);
    D_00190DEA = D_00190DEC = D_00190DEE = 6;
    for (i = 0; i < 12; i++) {
        D_00190CEE[i] = D_00195BE0->skills[D_00195BEC->skill[i]].value;
    }
    func_0009169B(2);
    func_0009169B(5);
    func_0009169B(8);
    func_00090FA1(2, 19);
    FREE(D_00195BE8, 142);
    FREE(D_00195B60, 143);
    D_001AA41A = 16;
    D_00195BE8 = func_0006CB53(D_00176F9C, 0);
    D_001AA414 = func_0006CB53(D_00176FA9, 0);
    func_00090FA1(35, 39);
    FREE(D_00195BE8, 149);
    FREE(D_001AA414, 150);
    D_001940D5 |= 32;
    func_0003F358();
    D_001AA41A = 255;
    func_000A0ED9(155, D_00176F41);
    func_000A0F5C(D_001903A4, D_00176F4C, D_00195BE0->f64 & 1, D_00195BE0->f67);
    D_001AA410 = func_0006CB53(D_001903A4, 0);
    D_00195B60 = func_0006CB53(D_00176F8F, 0);
    D_00195BE8 = func_0006CB53(D_00176FB6, 0);
    D_001AA414 = func_0006CB53(D_00176FA9, 0);
    func_000A1023(D_00195BE0->f48, D_00195BE0->f32, 16, D_00176F41, 161, 16);
    D_00195BE0->f88 = func_0003D113();
    if (func_00090FA1(0, 39)) {
        func_00090E79();
        return 1;
    }
    func_00090E79();
    return 0;
}
