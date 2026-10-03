/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00054B27 */
extern char D_0012AC00;
extern short D_0012AC04;
extern short D_0012AC06;
extern short D_0012DA44;
extern char **D_00185370[];
extern char **D_0018549C[];
extern short D_00190D76;
extern short D_00190D7A;
extern short D_00190D7C;
extern unsigned char D_00190D7E[];
extern short D_00190D82;
extern char **D_00190E0C;
extern char D_00196279;
extern char D_00199820[][7][2];
extern void func_00054D75(void);
extern void func_000551B1(short, int);
extern void func_000553B2(short, int);
extern void func_0007CA1F(char *, short, short, int, unsigned char);
extern int func_0012B136();

void func_00054B27(void)
{
    short l_24;
    short l_1C;
    short l_20;
    char **l_28;
    short l_18;

    l_28 = D_00190E0C;
    l_20 = 30;
    l_18 = (D_0012AC00 != 0 && D_0012AC00 != D_00196279 && D_0012AC04 > 10 && D_0012AC04 < 161) ? 1 : 0;
    D_00190D7A = -1;
    l_1C = 0;
    while (*l_28 != 0) {
        func_0007CA1F(*l_28, 10, l_20, 145, 141);
        l_24 = l_20;
        l_20 += D_0012DA44;
        if (l_18 && D_0012AC06 > l_24 && D_0012AC06 < l_20) {
            D_00190D7A = l_1C;
            while (D_0012AC00 != 0)
                func_0012B136();
        }
        l_1C++;
        l_28++;
    }
    l_1C = D_00190D7E[D_00190D82];
    D_00199820[D_00190D82][l_1C][D_00190D7C] = D_00190D7A;
    if (D_00190D7C == 0)
        D_00199820[D_00190D82][l_1C][1] = 0;
    if (D_00190D7A != -1 && D_00190D7C != 0) {
        if (D_00190D82 == 0)
            func_000551B1(l_1C, 0);
        else
            func_000553B2(l_1C, 0);
        func_00054D75();
    }
    if ((int)(short)(D_00190D76 & 16) != 0 && D_00190D7A != -1 && (int)(short)(D_00190D76 & 15) == 2) {
        D_00190E0C = D_00185370[D_00190D7A];
        D_00190D7C = 1;
        if (D_00190E0C == 0)
            func_00054D75();
    } else if ((int)(short)(D_00190D76 & 16) != 0 && D_00190D7A != -1) {
        D_00190E0C = D_0018549C[D_00190D7A];
        D_00190D7C = 1;
        if (D_00190E0C == 0)
            func_00054D75();
    }
}
