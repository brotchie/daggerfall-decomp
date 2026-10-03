/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00092ED8 */
#pragma pack(1)
struct bf8_7_1 { unsigned char _:7; unsigned char f:1; };
struct Img { unsigned short x; unsigned short y; unsigned short w; unsigned short h; char pad[4]; char data[1]; };
struct Rect { short x0; short y0; short x1; short y1; char pad[4]; };
struct Ply { char pad[367]; int items[12]; };
extern char *D_00143550;
extern char D_0017704C[];
extern char D_00188281[];
extern char D_00188425[];
extern char D_00188569[];
extern char D_001885A5[];
extern char D_001903A4[];
extern unsigned char D_001940DB;
extern int D_001959E8;
extern char *D_00195B64;
extern struct Ply *D_00195BE0;
extern int D_00195BF4;
extern int D_00195D2C;
extern int D_00195D38;
extern int D_00195D44;
extern unsigned char D_00196268;
extern unsigned char D_00196288;
extern char *D_001AA41C;
extern char *D_001AA420;
extern struct Img *D_001AA434;
extern struct Img *D_001AA438;
extern struct Img *D_001AA43C;
extern int D_001AA57C;
extern unsigned char D_001AA5F8;
extern unsigned char D_001AA5F9;
extern int func_0004A98C(int, int);
extern void func_00058E15(int, int);
extern void func_0007CA1F(char *, short, short, int, unsigned char);
extern int func_0007F558(void);
extern void func_000932C9(int, char *);
extern void func_00093372(int, int);
extern int func_000934F6(int, short, char *);
extern void func_00095D2C(char *);
extern void func_00095E32(char *);
extern void func_00096997(void);
extern int func_00096A14(void);
extern void func_0009751E(void);
extern int func_00097764(void);
extern void func_000984E0(void);
extern void func_00098BE8(void);
extern char *func_000A0DD9(int, char *, int);
extern void func_000A1023(char *, char *, int, char *, int, int);
extern int func_000CE31C();
extern int func_00144F68();

void func_00092ED8(void)
{
    int sw;
    struct Rect *r;
    char *src;
    int y;
    int i;
    int unused1;
    int unused2;
    int key;
    int val;
    int unused3;

    func_000A1023(D_00143550, D_001AA41C, 64000, D_0017704C, 553, 4);
    if (D_00195D38 != 0)
        func_00144F68(D_001AA434->x, D_001AA434->y, D_001AA434->w, D_001AA434->h, D_001AA434->data);
    r = (struct Rect *)(((int)D_00188281 + D_00195D38 * 84) + D_00195D44 * 12);
    if (D_00195D38 == 0) {
        for (y = r->y0; r->y1 >= y; y++)
            func_000A1023(r->x0 + (D_00143550 + y * 320), D_001AA420 + y * 320 + r->x0, r->x1 - r->x0 + 1, D_0017704C, 566, 4);
    } else {
        src = D_001AA438->data;
        for (y = r->y0; r->y1 >= y; y++)
            func_000A1023(r->x0 + (D_00143550 + y * 320), (r->x0 - (unsigned short)*(short *)D_001AA438) + (src + D_001AA438->w * (y - D_001AA438->y)), r->x1 - r->x0 + 1, D_0017704C, 574, 4);
    }
    func_000932C9(D_001AA5F9 + 41, D_001AA420);
    func_00058E15(-147, 0);
    func_000CE31C(D_00195B64 + 1008, D_00143550 + 4209, 111, 184, 125);
    if (D_00195D38 != 0) {
        func_00144F68(D_001AA43C->x, D_001AA43C->y, D_001AA43C->w, D_001AA43C->h, D_001AA43C->data);
        key = func_0004A98C(D_00195BF4, D_00196268);
        if (key == 43 || ((struct bf8_7_1 *)&D_001940DB)->f)
            val = 0;
        else
            val = D_00195D2C;
        func_0007CA1F(func_000A0DD9(D_00195D2C, D_001903A4, 10), 77, 15, 145, 156);
        func_0007CA1F(func_000A0DD9(func_0007F558(), D_001903A4, 10), 107, 15, 145, 156);
    }
    func_00098BE8();
    func_00095D2C(D_00188569);
    func_00095E32(D_001885A5);
    for (i = 0; i <= 11; i++) {
        if (D_00195BE0->items[i] != 0)
            func_000934F6(D_00195BE0->items[i], i, D_00188425);
    }
    func_0009751E();
    if (D_001AA57C == D_001959E8)
        func_00093372(32, D_001AA5F8);
    else
        func_00093372(32, D_00196288);
    sw = D_00195D38;
    switch (sw) {
    case 0:
        break;
    case 3:
        func_00096A14();
        break;
    case 1:
        func_00096997();
        break;
    case 2:
        func_00097764();
        break;
    case 4:
        func_000984E0();
        break;
    }
}
