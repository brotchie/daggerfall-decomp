/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00043F9C */
struct button { short x1, y1, x2, y2; int (*fn)(int); };
extern char D_0012AC00;
extern short D_0012AC04;
extern short D_0012AC06;
extern char D_00170EE8[];
extern char D_00170F2D[];
extern char D_00170F3A[];
extern char D_00170F47[];
extern struct button D_0017BA0C[];
extern char *D_00195AA4;
extern int D_00195B5C;
extern int D_00195B60;
extern unsigned char D_00195E7C;
extern unsigned char D_00195E7D;
extern char D_00196279;
extern void func_000441B6(int);
extern int func_00069938(int, char *, int);
extern int func_0006CB53(char *, int);
extern void func_0008059B(void);
extern void func_000A0024(int, char *, int);
extern void func_000CDD81(int);
extern void func_000CE8A0(unsigned char *, unsigned char *);
extern void func_0012B136(void);

int func_00043F9C(void)
{
    int l_24;
    int l_20;
    int l_1C;

    l_20 = 0;
    l_1C = func_0006CB53(D_00170F2D, 0);
    D_00195B5C = func_0006CB53(D_00170F3A, 0);
    D_00195B60 = func_0006CB53(D_00170F47, 0);
    func_000CE8A0(&D_00195E7C, &D_00195E7D);
    D_00195E7C /= 6;
    D_00195E7D /= 6;
    while (l_20 == 0) {
        D_00196279 = D_0012AC00;
        func_0012B136();
        func_000441B6(l_1C);
        func_0008059B();
        if (D_0012AC00 != 0 && D_00196279 == 0) {
            for (l_24 = 0; l_24 < 7; l_24++) {
                if (D_0012AC04 > D_0017BA0C[l_24].x1 && D_0012AC04 < D_0017BA0C[l_24].x2 && D_0012AC06 > D_0017BA0C[l_24].y1 && D_0012AC06 < D_0017BA0C[l_24].y2) {
                    func_00069938(203, D_00195AA4, 100);
                    l_20 = D_0017BA0C[l_24].fn(l_24);
                }
            }
        }
        func_000CDD81(0);
    }
    if (D_00195B60 != 0 && D_00195B60 != 0x97979797) {
        func_000A0024(D_00195B60, D_00170EE8, 468);
        D_00195B60 = 0x97979797;
    }
    if (D_00195B5C != 0 && D_00195B5C != 0x97979797) {
        func_000A0024(D_00195B5C, D_00170EE8, 469);
        D_00195B5C = 0x97979797;
    }
    if (l_1C != 0 && l_1C != 0x97979797) {
        func_000A0024(l_1C, D_00170EE8, 470);
        l_1C = 0x97979797;
    }
    return 0;
}
