/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000537B6 */
#pragma pack(1)
struct R { short x0; short y0; short x1; short y1; char pad[4]; };
struct C { char pad[0x91]; short v[5]; };
extern char D_0012B508;
extern int D_00143550;
extern char D_00175420[];
extern struct R D_0018565C[];
extern char D_001903A4[];
extern int D_00190DF4;
extern char *D_00190DF8;
extern struct C *D_00195BE0;
extern void func_0003F358(void);
extern void func_0007CA85(char *, short, short, int, unsigned char);
extern char *func_000A0DD9(int, char *, int);
extern int func_000A1023();
extern int func_0012B2EB();
extern int func_0012B3ED();
extern void func_00144D00(short, short, short, short);
extern int func_00144F68();

void func_000537B6(void)
{
    short n;
    short h;
    short mid;
    short total;
    short m;

    func_0012B2EB();
    func_000A1023(D_00143550, D_00190DF4, 64000, D_00175420, 283, 4);
    func_00144F68(39, 5, *(unsigned short *)(D_00190DF8 + 4), *(unsigned short *)(D_00190DF8 + 6), D_00190DF8 + 12);
    h = D_0018565C[0].x1 - D_0018565C[0].x0 + 1;
    mid = (D_0018565C[0].y0 + D_0018565C[0].y1) >> 1;
    total = n = 0;
    for (; n < 5; n++) {
        m = D_00195BE0->v[n] * 5;
        if (D_00195BE0->v[n] < 0) {
            D_0012B508 = 0xf6;
            func_00144D00(D_0018565C[n].x0, 82, h, -m);
        } else if (D_00195BE0->v[n] > 0) {
            D_0012B508 = 0xc5;
            func_00144D00(D_0018565C[n].x0, 81 - m, h, m);
        }
        func_0007CA85(func_000A0DD9(D_00195BE0->v[n], D_001903A4, 10), n * 33 + 58, 149, 145, 141);
        total += D_00195BE0->v[n];
    }
    func_0007CA85(func_000A0DD9(-total, D_001903A4, 10), 105, 179, 145, 141);
    func_0003F358();
    func_0012B3ED();
}
