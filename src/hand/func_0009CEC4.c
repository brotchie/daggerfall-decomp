/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0009CEC4 */
#pragma pack(1)
struct Ply {
    char pad0[32];
    short a;                    /* 32 */
    char pad1[6];
    short b;                    /* 40 */
    char pad2[124 - 42];
    short c;                    /* 124 */
    short c2;                   /* 126 */
    char pad3[141 - 128];
    short d;                    /* 141 */
    short d2;                   /* 143 */
    char pad4[155 - 145];
    unsigned short e;           /* 155 */
};
extern char *D_00143550;
extern char D_0017743D[];
extern unsigned short D_00178A0E;
extern char D_00190CE8;
extern struct Ply *D_00195BE0;
extern char *D_00195BEC;
extern int D_001AA674;
extern char *D_001AA690;
extern int D_001AA698;
extern unsigned char D_001AA6A6;
extern int func_0002001F(void);
extern void func_0004AB2F(int);
extern unsigned char *func_000702A0(unsigned char);
extern int func_0009D242(int, int);
extern void func_0009D39E(void);
extern void func_0009D515(void);
extern int func_0009DEAC();
extern int func_000A0024(char *, char *, int);
extern char *func_000A00AF(int, char *, int);
extern int func_000A1023(char *, char *, int, char *, int, int);

int func_0009CEC4(int x0, int y0, int x1, int y1, int a5)
{
    int dx;
    int dy;
    int n;
    int adx;
    int ady;
    int i;
    int sx;
    int sy;
    int err;
    int sum;
    int v;
    int flag;
    int save;
    int unused;
    unsigned char *p;

    flag = 0;
    if (D_001AA698 != 0) {
        D_001AA690 = func_000A00AF(64000, D_0017743D, 984);
        func_000A1023(D_001AA690, D_00143550, 64000, D_0017743D, 985, 4);
    }
    save = D_00195BE0->e;
    x0 = x0 / 32768;
    y0 = y0 / 32768;
    x1 = x1 / 32768;
    y1 = y1 / 32768;
    dx = x1 - x0;
    dy = y1 - y0;
    adx = func_0009DEAC(dx);
    ady = func_0009DEAC(dy);
    n = adx > ady ? adx : ady;
    if (dx < 0)
        sx = -1;
    else
        sx = 1;
    if (dy < 0)
        sy = -1;
    else
        sy = 1;
    func_0009D39E();
    D_001AA674 = sum = err = i = 0;
    for (; i < n; i++) {
        if (n == adx) {
            x0 += sx;
            err += ady;
            if (err > adx) {
                err -= adx;
                y0 += sy;
            }
        } else {
            y0 += sy;
            err += adx;
            if (err > ady) {
                err -= ady;
                x0 += sx;
            }
        }
        v = func_0009D242(x0, y0);
        if ((int)(unsigned short)(D_00178A0E & 32) != 0)
            v = v * 300 / 256;
        sum += v;
    }
    if (!(a5 == 0 || D_001AA6A6 == 100)) {
        if (D_00190CE8 != 0)
            func_0009D515();
        if ((int)(unsigned short)(D_00178A0E & 3) == 2)
            sum = (sum << 7) / 256;
        func_0004AB2F(sum);
        if ((int)(unsigned short)(D_00178A0E & 3) == 2) {
            D_00195BE0->e = save;
        } else {
            D_00195BE0->e = (D_00195BE0->a + D_00195BE0->b) << 6;
            D_00195BE0->c = D_00195BE0->c2;
            if ((int)(unsigned short)(*(unsigned short *)(D_00195BEC + 4) & 8) == 0)
                D_00195BE0->d = D_00195BE0->d2;
        }
    } else {
        func_0002001F();
    }
    if (D_001AA698 != 0) {
        if (!(D_001AA690 == 0 || D_001AA690 == (char *)0x97979797)) {
            func_000A0024(D_001AA690, D_0017743D, 1058);
            D_001AA690 = (char *)0x97979797;
        }
    }
    p = func_000702A0(145);
    if (p != 0)
        return sum * (((95 - *p) << 8) / 100) / 256;
    return sum;
}
