/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00054526 */
struct AB { unsigned char a; unsigned char b; };
extern short D_0012AC06;
extern short D_0012DA44;
extern int D_00143550;
extern char D_00175420[];
extern char *D_0018084E[];
extern char *D_0018092E[];
extern char **D_00185370[];
extern char **D_0018549C[];
extern short D_00190D76;
extern unsigned char D_00190D7E[];
extern short D_00190D82;
extern short D_00190D84;
extern int D_00190DF4;
extern char *D_00190DFC;
extern char *D_00190E00;
extern char **D_00190E0C;
extern struct AB D_00199820[][7];
extern void func_0003F09F(int, int);
extern void func_0003F358(void);
extern int func_00054484(void);
extern int func_000549DB(short);
extern void func_00054AE0(void);
extern void func_00054D75(void);
extern int func_00054DC5(void);
extern void func_0005506F(void);
extern void func_000551B1(short, int);
extern void func_000553B2(short, int);
extern int func_000555E5(short, int, short);
extern void func_0007CA1F(char *, short, short, int, unsigned char);
extern int func_000A1023();
extern int func_0012B2EB();
extern int func_0012B3ED();
extern int func_0012DB50();
extern int func_00144F68();

void func_00054526(void)
{
    short n;
    short a;
    short b;
    short y;
    short k;
    short y0;
    short y1;

    func_0012DB50(3);
    if ((short)(D_00190D76 & 15) == 2)
        n = 2;
    else
        n = 0;
    if ((int)(short)(D_00190D76 & 16) != 0)
        n++;
    func_0012B2EB();
    func_000A1023(D_00143550, D_00190DF4, 64000, D_00175420, 675, 4);
    func_00144F68(0, 0, *(unsigned short *)(D_00190E00 + 4), *(unsigned short *)(D_00190E00 + 6), D_00190E00 + 12);
    if (n == 2 || n == 3)
        func_00144F68(0, 0, *(unsigned short *)(D_00190DFC + 4), *(unsigned short *)(D_00190DFC + 6), D_00190DFC + 12);
    if ((int)(short)(D_00190D76 & 16) != 0) {
        k = func_00054484();
        if (k == 0) {
            D_00190D7E[D_00190D82]--;
            func_00054D75();
            goto done;
        }
        k--;
        n = D_00190D7E[D_00190D82];
        D_00199820[D_00190D82][n].a = k;
        D_00199820[D_00190D82][n].b = 0;
        if ((short)(D_00190D76 & 15) == 2)
            D_00190E0C = D_00185370[k];
        else
            D_00190E0C = D_0018549C[k];
        if (D_00190E0C == 0) {
            if (func_000549DB(D_00190D82) == 0) {
                if (D_00190D82 == 0)
                    func_000551B1(n, 0);
                else
                    func_000553B2(n, 0);
            }
            func_00054D75();
            goto done;
        }
        func_00054AE0();
        k = func_00054484();
        if (k == 0) {
            D_00190D7E[D_00190D82]--;
            func_00054D75();
            goto done;
        }
        k--;
        n = D_00190D7E[D_00190D82];
        D_00199820[D_00190D82][n].b = k;
        if (func_000555E5(D_00190D82, D_00199820[D_00190D82][n].a, k)) {
            D_00190D7E[D_00190D82]--;
            func_0003F09F(1350, 1);
        } else if (func_000549DB(D_00190D82) == 0) {
            if (D_00190D82 == 0)
                func_000551B1(n, 0);
            else
                func_000553B2(n, 0);
        }
        func_00054D75();
    } else {
        D_00190D84 = -1;
        y = 36;
        for (n = 0; D_00190D7E[D_00190D82] > n; n++, y += D_0012DA44 * 2) {
            y0 = y;
            if (D_00190D76 == 2) {
                a = D_00199820[0][n].a;
                b = D_00199820[0][n].b;
                func_0007CA1F(D_0018084E[a], 10, y, 145, 141);
                if (D_00185370[a]) {
                    y += D_0012DA44;
                    func_0007CA1F(D_00185370[a][b], 10, y, 145, 141);
                }
            } else {
                a = D_00199820[1][n].a;
                b = D_00199820[1][n].b;
                func_0007CA1F(D_0018092E[a], 10, y, 145, 141);
                if (D_0018549C[a]) {
                    y += D_0012DA44;
                    func_0007CA1F(D_0018549C[a][b], 10, y, 145, 141);
                }
            }
            y1 = y + D_0012DA44;
            if (D_0012AC06 >= y0 && D_0012AC06 <= y1)
                D_00190D84 = n;
        }
    }
done:
    func_0012DB50(4);
    func_00054DC5();
    func_0005506F();
    func_0003F358();
    func_0012B3ED();
}
