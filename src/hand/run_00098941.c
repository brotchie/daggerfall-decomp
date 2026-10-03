/* matched by the real Watcom C32 10.0a (-d2): a run of inven from 0x98538 to 0x98941, kept together for its switch table's alignment */
struct bits8 { unsigned char b0:1, b1:1, b2:1, b3:1, b4:1, b5:1, b6:1, b7:1; };
struct button { short x0, y0, x1, y1; void (*fn)(int); };
#define FREED ((char *)0x97979797)
extern char D_0012AC00;
extern short D_0012AC04;
extern short D_0012AC06;
extern char D_0017704C[];
extern char D_0017729E[];
extern char D_001772BC[];
extern char D_001772E6[];
extern char D_001772F3[];
extern unsigned char D_001789FA;
extern char *D_001832B4;
extern char *D_001832BC;
extern struct button D_00188641[];
extern signed char D_00190D16;
extern unsigned char D_001940DB;
extern unsigned char *D_00195B20;
extern char *D_00195B5C;
extern char *D_00195B60;
extern char *D_00195BE0;
extern int D_00195BF4;
extern unsigned char D_00196268;
extern char D_00196279;
extern unsigned char *D_001AA55C;
extern void func_0003EC2A(char *, int);
extern int func_0004A98C(int, int);
extern char *func_0006CB53(char *, int);
extern int func_0007CBA1(char *);
extern int func_0007F2A8(int);
extern void func_0008059B(void);
extern void func_00082487(void);
extern int func_00098B20(void);
extern void func_00098A15(void);
extern void func_0009D39E(void);
extern int func_000A0024(char *, char *, int);
extern int func_000CDD81();
extern int func_0012B136();
extern int func_00144F68();

void func_00098538(void)
{
    unsigned char *l_18;

    l_18 = *(unsigned char **)(D_00195B20 + 63);
    while (l_18 != 0) {
        l_18[113] |= 32;
        l_18 = *(unsigned char **)(l_18 + 55);
    }
}

int func_00098573(void)
{
    unsigned char *l_24;
    int l_20;
    int l_1C;

    l_24 = D_001AA55C + 71;
    if (*(short *)(l_24 + 67) != -1) {
        if (*(unsigned short *)(l_24 + 42) & 32) {
            func_0003EC2A(D_001832BC, 1);
            return 0;
        }
        l_20 = func_0004A98C(D_00195BF4, D_00196268);
        l_1C = (unsigned)(*(int *)(l_24 + 36) * 25) >> 8;
        if (l_20 != 43 && func_0007F2A8(l_1C) == 0 && ((struct bits8 *)&D_001940DB)->b7 == 0) {
            func_0003EC2A(D_0017729E, 1);
            return 0;
        }
        return 1;
    }
    func_0003EC2A(D_001832B4, 1);
    return 0;
}

void func_00098651(void)
{
    int l_20;
    int l_1C;
    int l_18;

    l_20 = 0;
    l_18 = 17;
    if (D_001789FA != 1) {
        func_0007CBA1(D_001772BC);
        return;
    }
    D_00195B5C = func_0006CB53(D_001772E6, 0);
    D_00195B60 = func_0006CB53(D_001772F3, 0);
    D_00190D16 = 0;
    func_0009D39E();
    if (func_00098B20() != 0) {
        l_18 = 24;
    } else {
        if ((D_00190D16 & 2) && D_001789FA == 1)
            l_18 |= 2;
        if ((D_00190D16 & 1) && D_001789FA == 1)
            l_18 |= 4;
        if (*(int *)(D_00195BE0 + 120) != 0 && D_001789FA != 3)
            l_18 |= 8;
    }
    while (l_20 == 0) {
        D_00196279 = D_0012AC00;
        func_0012B136();
        func_00144F68(*(unsigned short *)D_00195B5C, *(unsigned short *)(D_00195B5C + 2), *(unsigned short *)(D_00195B5C + 4), *(unsigned short *)(D_00195B5C + 6), D_00195B5C + 12);
        for (l_1C = 0; l_1C < 4; l_1C++) {
            if (((1 << l_1C) & l_18) == 0)
                func_00144F68(D_00188641[l_1C].x0, D_00188641[l_1C].y0, *(unsigned short *)(D_00195B60 + 4), 9, D_00195B60 + 12 + l_1C * (*(unsigned short *)(D_00195B60 + 4) * 9));
        }
        func_0008059B();
        if (D_0012AC00 != 0 && D_00196279 == 0) {
            for (l_1C = 0; l_1C < 5; l_1C++) {
                if ((1 << l_1C) & l_18) {
                    if (D_0012AC04 > D_00188641[l_1C].x0 && D_0012AC04 < D_00188641[l_1C].x1 && D_0012AC06 > D_00188641[l_1C].y0 && D_0012AC06 < D_00188641[l_1C].y1) {
                        if (D_00188641[l_1C].fn != 0)
                            D_00188641[l_1C].fn(l_1C);
                        l_20 = 1;
                    }
                }
            }
        }
        func_000CDD81(0);
    }
    if (D_00195B5C != 0 && D_00195B5C != FREED) {
        func_000A0024(D_00195B5C, D_0017704C, 2870);
        D_00195B5C = FREED;
    }
    if (D_00195B60 != 0 && D_00195B60 != FREED) {
        func_000A0024(D_00195B60, D_0017704C, 2871);
        D_00195B60 = FREED;
    }
}

void func_00098941(int a1)
{
    switch (a1) {
    case 0:
        D_00195BE0[65] &= 249;
        func_00082487();
        break;
    case 1:
        D_00190D16 = 0;
        func_0009D39E();
        if (D_00190D16 & 2) {
            D_00195BE0[65] |= 2;
            D_00195BE0[65] &= 251;
            D_001940DB &= 251;
        }
        break;
    case 2:
        D_00190D16 = 0;
        func_0009D39E();
        if (D_00190D16 & 1) {
            D_00195BE0[65] |= 4;
            D_00195BE0[65] &= 253;
            D_001940DB &= 251;
        }
        break;
    case 3:
        D_00195BE0[65] &= 249;
        func_00098A15();
        func_00082487();
        break;
    }
}
