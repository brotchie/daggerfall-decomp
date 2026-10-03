/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0008FE48 */
#pragma pack(1)
struct eff { unsigned char type; unsigned char sub; };
struct chance { unsigned char base; unsigned char plus; unsigned char per_level; };
struct mag { unsigned char base_min; unsigned char base_max; unsigned char plus_min; unsigned char plus_max; unsigned char per_level; };
struct spell {
    struct eff eff[3];          /* 0 */
    unsigned char f6;           /* 6 */
    unsigned char f7;           /* 7 */
    char pad8[6];
    struct chance chance[3];    /* 14 */
    struct chance dur[3];       /* 23 */
    struct mag mag[3];          /* 32 */
    char name[25];              /* 47 */
};
#pragma pack()
extern char D_00176E94[];
extern char D_00176EEC[];
extern unsigned char D_0017A94B[][12];
extern unsigned char D_0017B1CF[];
extern unsigned char D_001A9B8C[];
extern unsigned char D_001A9B94[];
extern unsigned char D_001A9BAC[];
extern short D_001AA3EC;
extern void func_00090261(unsigned char *, unsigned char *, unsigned char *, int);
extern void func_000A0AD9(char *, char *, int, char *, int);
extern void func_000A1023(void *, void *, int, char *, int, int);

int func_0008FE48(struct spell *sp)
{
    unsigned char w[8];
    unsigned char c;
    unsigned char x[8];
    int sum;
    unsigned char y[8];
    int i;
    unsigned char z[8];
    int n;

    func_000A1023(x, D_001A9B8C, D_001AA3EC, D_00176E94, 410, 8);
    func_000A1023(y, D_001A9BAC, D_001AA3EC, D_00176E94, 411, 8);
    func_000A1023(z, D_001A9B94, D_001AA3EC, D_00176E94, 412, 8);
    func_00090261(x, y, z, D_001AA3EC);
    n = D_001AA3EC;
    for (i = 0; n - 1 > i; i++) {
        if (x[i] == x[i + 1]) {
            w[i] = D_0017B1CF[x[i]];
            y[i] += y[i + 1];
            func_000A1023(x + i, x + (i + 1), 8 - i - 1, D_00176E94, 424, 4);
            func_000A1023(z + i, (i + 1) + z, 8 - i - 1, D_00176E94, 425, 4);
            func_000A1023(y + i, y + (i + 1), 8 - i - 1, D_00176E94, 426, 4);
            n--;
        }
    }
    func_00090261(y, x, z, n);
    sum = 0;
    if (n > 3) {
        for (i = 3; i < n; i++)
            sum += y[i];
        n = 3;
    }
    switch (n) {
    case 3:
        if ((y[0] >> 1) > y[1]) {
            n = 1;
            sum += y[1] + y[2];
        } else if ((y[1] >> 1) > y[2]) {
            n = 2;
            sum += y[2];
        }
        break;
    case 2:
        if ((y[0] >> 1) > y[1]) {
            n = 1;
            sum += y[1];
        }
        break;
    }
    for (i = 0; i < n; i++) {
        if (y[i] <= sum) {
            n = i;
            break;
        }
    }
    if (n == 0) return 0;
    sp->f6 = 4;
    sp->f7 = 0;
    func_000A0AD9(sp->name, D_00176EEC, 25, D_00176E94, 482);
    sp->eff[0].type = sp->eff[1].type = sp->eff[2].type = 255;
    for (i = 0; i < n; i++) {
        c = D_0017A94B[x[i]][z[i]];
        sp->eff[i].type = x[i];
        sp->eff[i].sub = z[i];
        if (c & 1)
            sp->chance[i].base = sp->chance[i].plus = sp->chance[i].per_level = 1;
        if (c & 2) {
            sp->dur[i].base = 70;
            sp->dur[i].plus = sp->dur[i].per_level = 1;
        }
        if (c & 4) {
            sp->mag[i].base_min = 10;
            sp->mag[i].base_max = sp->mag[i].plus_min = sp->mag[i].base_max = sp->mag[i].plus_min = 1;
        }
    }
    return 1;
}
