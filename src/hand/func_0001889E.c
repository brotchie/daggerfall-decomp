/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0001889E */
extern char *D_00147954;
extern char D_001703F0[];        /* __FILE__ */
extern char D_00170431[];
extern short D_00179D55[];
extern short *D_0019657C;
extern int D_001965B8;
extern int D_001965E8;
extern int D_001965F0;
extern int D_001965F4;
extern char D_00196620[][65];
extern short D_001966A2;
extern unsigned char D_001966B8;
extern void func_00017A61(char *, int, int, int, int);
extern void func_0001839C(void);
extern char *func_000192EE(short);
extern void func_000A0AD9(char *, char *, int, char *, int);

void func_0001889E(void)
{
    int i;
    int color;
    char *str;

    D_0019657C = (short *)(D_00147954 + 16384);
    D_001965B8 = 0;
    D_001966A2 = 1;
    D_0019657C[0] = D_0019657C[1] = D_0019657C[2] = 0;
    if (D_001965B8 >= D_001965F4 && D_001965B8 <= D_001965F0) {
        if (D_001965E8 == D_001965B8) {
            color = 244;
            func_000A0AD9(D_00196620[D_001966B8], D_00170431, 4, D_001703F0, 1884);
        } else {
            color = 145;
        }
        func_00017A61(D_00170431, 6, (D_001965B8 - D_001965F4) * 7 + 71, color, 156);
    }
    D_001965B8++;
    func_0001839C();
    for (i = 0; i < 34; i++) {
        D_0019657C[D_001966A2 * 3] = i > 7 ? i + 861 : i + 860;
        D_0019657C[D_001966A2 * 3 + 1] = 0;
        D_0019657C[D_001966A2 * 3 + 2] = 0;
        D_001966A2++;
        if (D_001965B8 < D_001965F4 || D_001965B8 > D_001965F0) {
            D_001965B8++;
            continue;
        }
        str = func_000192EE(D_00179D55[i]) + 3;
        if (D_001965E8 == D_001965B8) {
            color = 244;
            func_000A0AD9(D_00196620[D_001966B8], str, 4, D_001703F0, 1914);
        } else {
            color = 145;
        }
        func_00017A61(str, 6, (D_001965B8 - D_001965F4) * 7 + 71, color, 156);
        D_001965B8++;
    }
}
