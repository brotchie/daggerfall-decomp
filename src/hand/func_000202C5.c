/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000202C5 */
extern char D_00170604[];
extern char D_00170634[];
extern char D_0017063D[];
extern char D_00179E60[];
extern char D_00179E66[];
extern unsigned char D_00179E7F[];
extern unsigned char D_00179E90[];
extern unsigned char D_00179E94[];
extern unsigned char D_001850D4[];
extern int D_001850E5[];
extern char D_001903A4[];
extern unsigned D_00195BF4;
extern unsigned short *D_00195BF8;
extern int D_00195D30;
extern unsigned char D_00195E2A[];
extern char D_001961F5[];
extern unsigned char D_00196271;
extern unsigned char *D_0019671C;
extern int D_00199764;
extern unsigned char *func_000192EE(short);
extern int func_0001FFF1(void);
extern void func_0003EC2A(char *, int);
extern void func_0003F09F(int, int);
extern int func_00051DCF(int, char *, int, int);
extern unsigned char *func_00063495(int);
extern int func_0007D6AE(int, int);
extern void func_0007DDC9(int);
extern void func_0007F1E3(int);
extern int func_0007F2A8(int);
extern int func_0009DC25(void);
extern int func_0009DC49(int);
extern int func_000A0040();
extern int func_000A0AD9();
extern char *func_000CE45E(char *, short, int);
#pragma aux func_000A0ED9 parm routine [];
extern int func_000A0ED9(int, char *);
extern int func_000A0F5C(char *, ...);

#define REGION(o) D_001850D4[D_00179E94[*(unsigned short *)((o) + 33)]]

void func_000202C5(unsigned char *a1)
{
    char l_70[44];
    unsigned char *l_40;
    unsigned char *l_3C;
    char *l_38;
    int l_34;
    int l_30;
    int l_2C;
    int l_28;
    unsigned char *l_24;
    unsigned char *l_20;
    unsigned char l_1C;
    unsigned char l_18;

    l_18 = 48;
    l_20 = a1 + 71;
    l_40 = func_000192EE(*(short *)l_20);
    while (*(unsigned char **)(l_40 + 88) != 0)
        l_40 = *(unsigned char **)(l_40 + 88);
    if (*(unsigned short *)(l_40 + 33) == 40 || *l_40 == 8) {
    } else {
        l_40 = *(unsigned char **)(l_40 + 84);
    }
    switch (*(unsigned short *)(l_40 + 33)) {
    case 40:
        l_38 = func_000CE45E(D_00179E60, D_00195BF4 % 518400 / 1440, 16);
        if (l_38 == 0 || D_00179E66 == l_38) {
            func_0003F09F(480, 1);
            return;
        }
        l_34 = (l_38 - D_00179E60) / 2 + 1;
        l_3C = func_000192EE(l_34);
        D_0019671C = l_3C;
        if (REGION(l_3C) == 56) {
            if (*D_00195BF8 & 4) {
                func_0003F09F(400, 1);
                return;
            }
            l_18 = 120;
        }
        D_00195D30 = (100 - *(short *)(l_40 + 29)) * 1000 + 100000;
        func_0007DDC9(481);
        if (D_00196271 == 2) return;
        l_30 = 30;
        break;
    case 21:
    case 22:
    case 24:
    case 26:
    case 27:
    case 29:
    case 33:
    case 35:
    case 36:
    case 82:
    case 84:
    case 88:
    case 92:
    case 94:
    case 98:
    case 106:
        l_38 = func_000CE45E(D_00179E60, D_00195BF4 % 518400 / 1440, 16);
        if (l_38 == 0 || D_00179E66 == l_38) {
            func_0003F09F(480, 1);
            return;
        }
        l_34 = (l_38 - D_00179E60) / 2 + 1;
        for (l_2C = 0; l_2C < 3; l_2C++) {
            if (*(unsigned char **)(l_40 + l_2C * 4 + 68) != 0 && *(unsigned short *)(*(unsigned char **)(l_40 + l_2C * 4 + 68) + 33) == l_34) {
                func_0003EC2A(D_00170604, 1);
                return;
            }
        }
        l_3C = func_000192EE(l_34);
        D_0019671C = l_3C;
        if (REGION(l_3C) == 56) {
            if (*D_00195BF8 & 4) {
                func_0003F09F(400, 1);
                return;
            }
            l_18 = 120;
        }
        D_00195D30 = (100 - *(short *)(l_40 + 29)) * 1000 + 100000;
        func_0007DDC9(481);
        if (D_00196271 == 2) return;
        l_30 = 30;
        break;
    default:
        l_28 = func_0009DC25();
        func_0009DC49(D_00195BF4 / 1440);
        l_34 = 4;
        if (*(unsigned short *)(l_40 + 33) != 419) {
            while (l_34 == 4)
                l_34 = func_0007D6AE(1, 16);
        }
        l_3C = func_000192EE(l_34);
        D_0019671C = l_3C;
        if (REGION(l_3C) == 56) {
            if (*D_00195BF8 & 4) {
                func_0003F09F(400, 1);
                return;
            }
            l_18 = 120;
        }
        D_00195D30 = (100 - *(short *)(l_40 + 29)) * 1000 + 100000;
        func_0007DDC9(481);
        if (D_00196271 == 2) return;
        l_30 = 30;
        func_0009DC49(l_28);
        break;
    }
    if (D_00195D30 < 0)
        D_00195D30 = -D_00195D30;
    if (D_00195D30 > 200000)
        D_00195D30 = 200000;
    if (func_0007F2A8(D_00195D30) == 0) {
        func_0003F09F(454, 1);
        return;
    }
    l_1C = D_00195E2A[func_0001FFF1()];
    l_2C = 5;
    if (l_1C == 6)
        l_2C = 15;
    if (func_0009DC25() % 101 <= l_2C) {
        l_34 = 9;
        l_3C = func_000192EE(l_34);
    }
    l_30 += *(short *)(l_3C + 29);
    if (D_00179E7F[l_34] == 100 || l_1C == D_00179E7F[l_34])
        l_30 += 30;
    func_0007F1E3(D_00195D30);
    if (func_0007D6AE(1, 100) > l_30) {
        func_0003F09F(484, 1);
        return;
    }
    if (*(unsigned short *)(l_3C + 37) & 64) {
        l_24 = &REGION(l_3C);
        func_000A0040(l_70, 0, 44, D_00170634, 189, 4);
        D_00199764 = 0;
        func_00051DCF(D_001850E5[l_24 - D_001850D4], l_70, 482, 0);
        a1 = func_00063495(D_00179E90[func_0007D6AE(0, 4)]);
        if (a1 != 0)
            a1[624] = 1;
        return;
    }
    *(unsigned short *)(l_3C + 37) |= 64;
    func_000A0ED9(200, D_00170634);
    func_000A0F5C(D_001903A4, D_0017063D, REGION(l_3C), l_18);
    func_000A0AD9(D_001961F5, D_001903A4, 13, D_00170634, 201);
}
