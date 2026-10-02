/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00038911 */
#pragma pack(1)
struct bf8_2_1 { unsigned char _:2; unsigned char f:1; };
struct Ent { unsigned char type; char pad; };
struct P {
    struct Ent e[4];            /* 0x00 */
    unsigned short v[3];        /* 0x08 */
    signed char a[3][3];        /* 0x0e */
    signed char b[3][3];        /* 0x17 */
    signed char c[3][5];        /* 0x20 */
};
struct Box { short x0; short y0; short x1; short y1; void (*fn)(); };
struct E6 { short s; char pad[4]; };
struct C { char pad[0x9d]; struct E6 e[1]; };
extern unsigned char D_0012AC00;
extern short D_0012AC04;
extern short D_0012AC06;
extern struct P *D_00178A0A;
extern unsigned char D_0017A84C[];
extern unsigned char D_0017B202[];
extern struct Box D_0017B317[];
extern unsigned char D_00185C34[];
extern char D_001903A4[];
extern unsigned char D_001940D5;
extern struct C *D_00195BE0;
extern short D_00195F30;
extern short D_00195F34;
extern short D_00195F52;
extern short D_00195F54;
extern short D_00195F66;
extern unsigned char D_00196270;
extern char D_00196274;
extern unsigned char D_00196279;
extern int D_00199624;
extern short D_00199628;
extern short D_0019962A;
extern unsigned char D_0019962E;
extern int func_00039224(short);
extern void func_0003F181(int, int);
extern void func_0003F358(void);
extern void func_0007CA1F(char *, short, short, int, unsigned char);
extern void func_0007CAEB(short, short, short, short, short, short, short);
extern int func_0009DEAC();
extern char *func_000A0DD9(int, char *, int);
extern int func_000CAE07();
extern int func_000CB552();
extern int func_0012B49E();

void func_00038911(void)
{
    int val;
    int r;
    short i;

    if (D_0019962E == 0)
        return;
    if (D_00196270 != 4) {
        func_0003F181(func_00039224(D_00195F30) + 1500, 4);
        D_00196274 = 2;
        D_001940D5 |= 64;
        D_00195F34 = 88;
    }
    if ((int)(unsigned char)(D_0012AC00 & 1) != 0 && D_00196279 != D_0012AC00 && D_0012AC04 > 281
      && D_0012AC04 < 304 && D_0012AC06 > 94 && D_0012AC06 < 109) {
        D_001940D5 |= 32;
        func_0003F358();
        D_0019962E = 0;
        return;
    }
    func_000CB552(D_00199624);
    func_0003F358();
    if ((int)(unsigned char)(D_0012AC00 & 1) != 0 && D_0012AC00 != D_00196279) {
        for (i = 11; i < 33; i++) {
            if (D_0012AC04 > D_0017B317[i].x0 && D_0012AC04 < D_0017B317[i].x1
              && D_0012AC06 > D_0017B317[i].y0 && D_0012AC06 < D_0017B317[i].y1) {
                if (i < 22)
                    D_00195F66 = 1;
                else
                    D_00195F66 = -1;
                r = i % 11;
                if (r < 3 && (int)(unsigned char)(D_0019962E & 1) != 0
                  || r < 6 && (int)(unsigned char)(D_0019962E & 2) != 0
                  || (int)(unsigned char)(D_0019962E & 4) != 0)
                    D_0017B317[i % 11].fn(D_00195F66);
                goto done;
            }
        }
        D_00195F66 = i = 0;
        for (; i < 11; i++) {
            if (D_0012AC04 > D_0017B317[i].x0 && D_0012AC04 < D_0017B317[i].x1
              && D_0012AC06 > D_0017B317[i].y0 && D_0012AC06 < D_0017B317[i].y1)
                D_00195F66 = i + 1;
        }
        if (D_00195F66 == 0)
            goto done;
        D_0019962A = D_00195F66 - 1;
        ((struct bf8_2_1 *)&D_001940D5)->f = 1;
        D_00195F54 = D_0012AC04;
        D_00195F52 = D_0012AC06;
        D_00199628 = 0;
    } else if (D_0012AC00 == 0 && ((struct bf8_2_1 *)&D_001940D5)->f) {
        ((struct bf8_2_1 *)&D_001940D5)->f = 0;
        func_0012B49E(D_00195F54, D_00195F52);
    } else if (((struct bf8_2_1 *)&D_001940D5)->f) {
        D_00199628 -= D_00195F66;
        if (func_0009DEAC(D_00199628) > 30) {
            if (D_0019962A < 3 && (int)(unsigned char)(D_0019962E & 1) != 0
              || D_0019962A < 6 && (int)(unsigned char)(D_0019962E & 2) != 0
              || (int)(unsigned char)(D_0019962E & 4) != 0)
                D_0017B317[D_0019962A].fn(D_00199628 / 30);
            D_00199628 = 0;
        }
    }
done:
    val = D_00178A0A->v[D_00195F30];
    val = (110 - D_00195BE0->e[D_00185C34[D_0017A84C[D_00178A0A->e[D_00195F30].type]]].s) * val / 100;
    func_0007CA1F(func_000A0DD9(val, D_001903A4, 10), 275, 119, 145, 156);
    if ((int)(unsigned char)(D_0019962E & 1) != 0) {
        func_0007CAEB(64, 94, 87, 109, D_00178A0A->a[D_00195F30][0], 145, 156);
        func_0007CAEB(104, 94, 127, 109, D_00178A0A->a[D_00195F30][1], 145, 156);
        func_0007CAEB(160, 94, 183, 109, D_00178A0A->a[D_00195F30][2], 145, 156);
    }
    if ((int)(unsigned char)(D_0019962E & 2) != 0) {
        func_0007CAEB(64, 114, 87, 129, D_00178A0A->b[D_00195F30][0], 145, 156);
        func_0007CAEB(104, 114, 127, 129, D_00178A0A->b[D_00195F30][1], 145, 156);
        func_0007CAEB(160, 114, 183, 129, D_00178A0A->b[D_00195F30][2], 145, 156);
    }
    if ((int)(unsigned char)(D_0019962E & 4) != 0) {
        func_0007CAEB(64, 134, 87, 149, D_00178A0A->c[D_00195F30][0], 145, 156);
        func_0007CAEB(104, 134, 127, 149, D_00178A0A->c[D_00195F30][1], 145, 156);
        func_0007CAEB(144, 134, 167, 149, D_00178A0A->c[D_00195F30][2], 145, 156);
        func_0007CAEB(184, 134, 207, 149, D_00178A0A->c[D_00195F30][3], 145, 156);
        func_0007CAEB(240, 134, 263, 149, D_00178A0A->c[D_00195F30][4], 145, 156);
    }
    D_00178A0A->v[D_00195F30] = func_000CAE07(D_0017B202[D_00178A0A->e[D_00195F30].type] - 1);
}
