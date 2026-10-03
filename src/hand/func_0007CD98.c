/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0007CD98 */
extern unsigned char D_0012AC00;
extern char D_001903A4[];
extern unsigned char D_00196272;
extern unsigned char D_00196275;
extern unsigned char D_00196279;
extern char *D_001A59E8;
extern short func_0005A4A9(char *);
extern void func_0005A54A(char *, short, short);
extern void func_000CD0F1(int, int, int, int);
extern char *func_000CE300(char *, char *);

void func_0007CD98(void)
{
    char *p;
    short i;
    short n;
    short x;
    short y;
    short w;

    n = 0;
    i = 0;
    w = 0;
    if (D_001A59E8 == 0)
        return;
    p = D_001A59E8;
    do {
        p = func_000CE300(D_001903A4, p);
        i = func_0005A4A9(D_001903A4);
        if (i > w)
            w = i;
        n++;
    } while (p[-1] != 0);
    w = (w + 10) / 2;
    func_000CD0F1(160 - w, 100 - n * 5 - 5, w * 2, n * 10 + 10);
    p = D_001A59E8;
    x = 160 - w + 5;
    y = 100 - n * 5;
    for (i = 0; i < n; i++, y += 10) {
        p = func_000CE300(D_001903A4, p);
        func_0005A54A(D_001903A4, x, y);
    }
    if (D_00196272 == 1 && (D_0012AC00 & 1) == 0 && (D_00196279 & 1) != 0) {
        D_00196272 = D_00196275;
        D_001A59E8 = 0;
    }
}
