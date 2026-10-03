/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000383CC */
extern unsigned char D_0012AC00;
extern char D_00170B13[];
extern unsigned char D_0017A87F[];
extern char *D_00182686[];
extern unsigned char D_001940D5;
extern void (*D_00195B7C)(int);
extern char *D_00195C44;
extern short func_00037D5A(void);
extern void func_00038228(int);
extern int func_0003853C(short);
extern void func_0003F09F(int, int);
extern void func_0007D24F(char *);
extern void func_000A0AD9(char *, char *, int, char *, int);
extern int func_000A0DF4(char *);

int func_000383CC(void)
{
    short i;
    char *list;
    short cnt;
    char *str;
    short mode;

    if ((int)(unsigned char)(D_0012AC00 & 2) != 0) {
        D_001940D5 |= 1;
        func_0003F09F(1811, 1);
        return 0;
    }
    if (func_0003853C(255) == -1) {
        func_0003F09F(1707, 1);
        return 0;
    }
    mode = func_00037D5A();
    str = D_00195C44;
    list = D_00195C44 + 32000;
    *str = i = cnt = 0;
    for (; i < 51; i++) {
        if (D_00182686[i] == 0)
            continue;
        if (!((D_0017A87F[i] == 2 || mode == 2) || D_0017A87F[i] == mode))
            continue;
        list[cnt++] = i;
        func_000A0AD9(str, D_00182686[i], 4, D_00170B13, 1108);
        str = func_000A0DF4(str) + str + 1;
    }
    *str = 0;
    D_00195B7C = func_00038228;
    func_0007D24F(D_00195C44);
    return 0;
}
