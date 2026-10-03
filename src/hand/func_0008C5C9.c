/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0008C5C9 */
extern unsigned char D_0012B508;
extern short D_0012DA44;
extern short D_00142928;
extern short D_0014292C;
extern char D_00176E2C[];
extern char D_00190B44[];
extern int D_00195B8C;
extern int D_00195B94;
extern short D_00195F36;
extern short D_00195F38;
extern short D_001A9AAE;
extern void func_0005A54A(int, unsigned short, unsigned short);
extern unsigned char func_0008C462(void);
extern int func_0008C6E9(unsigned char);
extern short func_0008C9D2(int, short);
extern void func_000A0AD9(int, char *, int, char *, int);
extern void func_001531F0(int, int, int, int);

int func_0008C5C9(void)
{
    unsigned char c;
    char x;
    char y;

    D_0012B508 += 5;
    D_00142928 = func_0008C9D2(D_00195B94, D_001A9AAE) + D_00195F36;
    D_0014292C = D_00195F38;
    {
        int *clk;

        clk = (int *)0x46c;
        if (*clk & 4)
            func_001531F0(D_00142928, D_0014292C, D_00142928, (short)(D_0014292C + D_0012DA44 - 1));
    }
    D_0012B508 -= 5;
    func_0005A54A(D_00195B94, D_00195F36, D_00195F38);
    c = func_0008C462();
    if (c == 0)
        return 0;
    {
        int r;

        r = func_0008C6E9(c);
        if (r == 0x87654321)
            return 0;
        if (r == 0x8000) {
            func_000A0AD9(D_00195B94, D_00190B44, 4, D_00176E2C, 157);
            return 2;
        }
        D_00195B8C = r;
        return 1;
    }
}
