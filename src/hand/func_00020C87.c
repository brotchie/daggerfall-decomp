/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00020C87 */
struct row { short v; char pad[78]; };
struct shop { unsigned char mul; char pad1; short base; short min; short max; };
struct who { char pad[0x1b]; unsigned short id; };
struct res { int f0; int f4; };
extern char *D_00143550;
extern char D_001706D4[];
extern char D_001706E1[];       /* __FILE__ */
extern unsigned char D_001789FA;
extern int D_00179EAC[];
extern struct shop D_00179EE0[];
extern char D_00187CA8;
extern struct row D_0018F08E[];
extern int D_00190CAC;
extern char D_00190D16;
extern signed char D_00190D17;
extern char D_00190D18;
extern short D_00190DC8;
extern unsigned char D_001940D5;
extern struct who *D_00195AC4;
extern int D_00195B14;
extern char *D_00195BE8;
extern int D_00195D68;
extern short D_00195F34;
extern unsigned char D_00196268;
extern unsigned char D_0019626F;
extern char D_00196271;
extern char D_00196272;
extern unsigned char D_00196274;
extern char D_0019629C;
extern int D_00196B08;
extern struct res *D_001A4FA0;
extern void func_00020C55(int);
extern void func_00021603(void);
extern void func_00021680(void);
extern void func_00021775(void);
extern void func_0003F09F(int, int);
extern void func_0006974E(int);
extern char *func_0006CB53(char *, int);
extern unsigned char *func_000702A0(unsigned char);
extern void func_0007D62B(short, unsigned char, unsigned char, int, unsigned char, unsigned char, unsigned char);
extern int func_0007D6AE(int, int);
extern void func_0007D723(void);
extern int func_0007F349(void);
extern void func_000876AD(int, int, int, int);
extern void func_0008E3F7(struct who *, void (*)(int));
extern int func_0009DC25(void);
extern void func_000A1023(char *, char *, int, char *, int, int);
extern int func_000CDD81();

int func_00020C87(int n)
{
    int a;
    int b;
    int gold;
    int i;
    int cnt;
    int over;
    unsigned char *item;

    a = 0;
    b = 0;
    if (D_00196274 == 21 || D_0019626F == 21)
        return 1;
    if (n != 0) {
        n--;
        func_0006974E(D_001A4FA0->f4);
        D_0019629C = 1;
        D_00190D17 = n;
        D_00196B08 = D_00179EAC[n] >> 1;
        if (D_001789FA == 2)
            func_000876AD(D_00196268, 1, D_00195AC4->id, 0);
        if (D_0018F08E[D_00196268].v < 0) {
            a = -D_0018F08E[D_00196268].v;
            if (a > 75)
                a = 75;
            b = -D_0018F08E[D_00196268].v / 2;
            if (b > 75)
                b = 75;
        }
        if (func_0007D6AE(1, 100) <= b)
            D_00190D16 = 0;
        else if (func_0007D6AE(1, 100) <= a)
            D_00190D16 = 0;
        else
            D_00190D16 = 2;
        if (D_0018F08E[D_00196268].v < 0)
            gold = D_00179EE0[n].base - D_0018F08E[D_00196268].v * D_00179EE0[n].mul;
        else
            gold = D_00179EE0[n].base + D_0018F08E[D_00196268].v * D_00179EE0[n].mul;
        if (D_00179EE0[n].min > gold)
            gold = D_00179EE0[n].min;
        else if (D_00179EE0[n].max < gold)
            gold = D_00179EE0[n].max;
        cnt = gold / 40;
        for (gold = i = D_00190DC8 = 0; i < cnt; i++) {
            if (func_0009DC25() & 1)
                gold += 40;
            else
                D_00190DC8 += 3;
        }
        if (func_0007F349() < gold) {
            over = gold - func_0007F349();
            D_00190DC8 += over / 40;
            gold -= over;
        }
        D_00196274 = 21;
        D_00196272 = 1;
        D_00195BE8 = func_0006CB53(D_001706D4, 0);
        func_000A1023(D_00143550, D_00195BE8, 64000, D_001706E1, 114, 4);
        func_000CDD81(1);
        D_00190CAC = gold;
        D_00187CA8 = 0;
        D_001940D5 |= 64;
        D_00195F34 = 194;
        item = func_000702A0(0);
        if ((D_00190D17 == 4 || D_00190D17 == 3) && item != 0 && *item >= func_0007D6AE(0, 19)) {
            func_0003F09F(551, 1);
            func_00021775();
            func_00021603();
            D_00195D68 = 0;
            func_0008E3F7(D_00195AC4, func_00020C55);
            func_0007D723();
            D_00195B14 = 0;
            func_00021680();
            return 0;
        }
        item = func_000702A0(3);
        if ((D_00190D17 <= 2 || D_00190D17 == 11) && item != 0 && *item >= func_0007D6AE(0, 19)) {
            func_0003F09F(550, 1);
            func_00021775();
            func_00021603();
            D_00195D68 = 0;
            func_0008E3F7(D_00195AC4, func_00020C55);
            func_0007D723();
            D_00195B14 = 0;
            func_00021680();
            return 0;
        }
        D_00196271 = 0;
        if (D_00190DC8 == 0)
            func_0007D62B(8050, 16, 17, 0, 103, 110, 0);
        else if (gold == 0)
            func_0007D62B(8050, 16, 17, 0, 103, 110, 0);
        else
            func_0007D62B(8050, 16, 17, 0, 103, 110, 0);
        D_00190D18 = 1;
    }
    return D_00196274 == 21;
}
