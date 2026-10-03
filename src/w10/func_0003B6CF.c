/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0003B6CF */
struct bf8_2_1 { unsigned char _:2; unsigned char f:1; };
extern char D_0012DA44[];
extern char D_00170C67[];
extern char D_00170C6F[];
extern char D_00170C73[];
extern char D_0017B50C[];
extern char D_0017B5A2[];
extern char D_0017B5A6[];
extern char D_0017CBF3[];
extern char D_001903A4[];
extern char D_001940D9[];
extern char D_00195A08[];
extern char D_00195AA0[];
extern char D_00195B64[];
extern char D_00195BE0[];
extern char D_00195BEC[];
extern void func_0003C81C(void);
extern int func_0004A3EC(int);
extern void func_0007CA1F(char *, int, int, int, unsigned char);
extern void func_0007CA85(char *, int, int, int, unsigned char);
extern int func_0007F349(void);
extern int func_000801A4(void);
extern char *func_000A0DD9(int, char *, int);
extern int func_00144ED8();
#pragma aux func_000A0ED9 parm routine [];
extern int func_000A0ED9(int, char *);
extern int func_000A0F5C(char *, char *, ...);

void func_0003B6CF(void)
{
    int l_34;
    int l_30;
    int l_2C;
    short l_28;
    short l_24;
    short l_20;
    short l_1C;
    short l_18;

    func_0007CA1F(*(char **)D_00195BE0, 41, 4, 145, 141);
    func_0007CA1F(*(char **)D_00195BEC + 28, 46, 24, 145, 141);
    l_28 = *(unsigned char *)(*(char **)D_00195BE0 + 129);
    func_0007CA1F(func_000A0DD9(l_28, D_001903A4, 10), 45, 34, 145, 141);
    func_0007CA1F(*(char **)(D_0017CBF3 + (((int)(unsigned char)*(signed char *)(*(char **)D_00195BE0 + 67)) << 2)), 41, 14, 145, 141);
    func_000A0ED9(180, D_00170C67);
    func_000A0F5C(D_001903A4, D_00170C6F, func_0007F349());
    func_0007CA1F(D_001903A4, 39, 44, 145, 141);
    func_000A0ED9(183, D_00170C67);
    func_000A0F5C(D_001903A4, D_00170C73, (int)(short)*(short *)(*(char **)D_00195BE0 + 124), (int)(short)*(short *)(*(char **)D_00195BE0 + 126));
    func_0007CA85(D_001903A4, 72, 64, 145, 141);
    func_000A0ED9(185, D_00170C67);
    func_000A0F5C(D_001903A4, D_00170C73, ((int)(unsigned short)*(short *)(*(char **)D_00195BE0 + 155)) >> 6, ((int)(short)*(short *)(*(char **)D_00195BE0 + 32)) + ((int)(short)*(short *)(*(char **)D_00195BE0 + 40)));
    func_0007CA85(D_001903A4, 77, 54, 145, 141);
    l_1C = ((*(short *)D_0017B5A2 + *(short *)D_0017B5A6) >> 1) + 1;
    if (((struct bf8_2_1 *)&D_001940D9)->f) l_34 = *(int *)D_00195BE0 + 48;
    else l_34 = *(int *)D_00195BE0 + 32;
    for (l_24 = 0; l_24 < 8; l_24++) {
        if (l_24 == 0 && ((struct bf8_2_1 *)&D_001940D9)->f == 0)
            l_18 = *(short *)((char *)(l_24 * 2) + l_34) + *(short *)D_00195A08;
        else
            l_18 = *(short *)((char *)(l_24 * 2) + l_34);
        if (l_18 < *(short *)(*(char **)D_00195BE0 + 48 + l_24 * 2)) l_20 = 240;
        else if (l_18 > *(short *)(*(char **)D_00195BE0 + 48 + l_24 * 2)) l_20 = 96;
        else l_20 = 145;
        if (l_24 == 0 && ((struct bf8_2_1 *)&D_001940D9)->f == 0)
            func_0007CA85(func_000A0DD9(*(short *)((char *)(l_24 * 2) + l_34) + *(int *)D_00195A08, D_001903A4, 10), l_1C, (short)((*(short *)(D_0017B50C + (l_24 + 13) * 12) - *(short *)D_0012DA44) - 2), l_20, 141);
        else
            func_0007CA85(func_000A0DD9(*(short *)((char *)(l_24 * 2) + l_34), D_001903A4, 10), l_1C, (short)((*(short *)(D_0017B50C + (l_24 + 13) * 12) - *(short *)D_0012DA44) - 2), l_20, 141);
    }
    func_00144ED8(192, 1, 125, 197, *(int *)D_00195B64, 0);
    l_2C = func_0004A3EC(*(int *)D_00195AA0) >> 2;
    func_000A0ED9(218, D_00170C67);
    func_000A0F5C(D_001903A4, D_00170C73, l_2C, func_000801A4());
    func_0007CA1F(D_001903A4, 91, 74, 145, 141);
    func_0003C81C();
}
