/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0005E450 */
extern short *D_00185F88[];
extern void func_0005DE74(unsigned short, short, short, char *);
extern void func_00060430(char *, int);
extern void func_0006077F(char *, int);
extern int func_0007D6AE(int, int);

void func_0005E450(unsigned short a1, char *a2)
{
    int r;
    int i;

    switch (a1) {
    case 5:
        func_0006077F(a2, func_0007D6AE(0, 22));
        break;
    case 4:
        func_00060430(a2, -1);
        break;
    case 11:
        func_0005DE74(287, 27, 8, a2);
        break;
    default:
        i = 0;
        while (D_00185F88[a1][i++] != -1)
            ;
        r = func_0007D6AE(0, i - 2);
        func_0005DE74(D_00185F88[a1][r], a1, r, a2);
        break;
    }
}
