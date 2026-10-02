/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00053494 */
extern char D_0012B508[];
extern char D_00143550[];
extern char D_00175420[];
extern char D_0017CC27[];
extern char D_001854F4[];
extern char D_001854F6[];
extern char D_001855CC[];
extern char D_001855CE[];
extern char D_001855D0[];
extern char D_001855D2[];
extern char D_001903A4[];
extern char D_00190D64[];
extern char D_00190D66[];
extern char D_00190D6A[];
extern char D_00190DE8[];
extern char D_00190DF0[];
extern char D_00190DF4[];
extern char D_00195BEC[];
extern void func_0003F358(void);
extern void func_0005506F(void);
extern void func_0007CA1F(int, int, int, int, unsigned char);
extern void func_0007CA85(int, int, int, int, unsigned char);
extern int func_000A0DD9();
extern int func_000A1023();
extern int func_0012B2EB();
extern int func_0012B3ED();
extern int func_00144E84();
extern int func_00144ED8();
extern int func_00144FB4();

int func_00053494(short a1)
{
    short l_24;
    short l_18;
    short l_1C;

    func_0012B2EB();
    func_000A1023(*(int *)D_00143550, *(int *)D_00190DF4, 64000, (int)D_00175420, 235, 4);
    func_0005506F();
    if (((int)(short)(*(short *)D_00190D66 & 2)) == 0) goto L53516;
    func_00144ED8(44, (int)(short)*(short *)D_00190D6A, (int)(unsigned short)*(short *)(*(char **)D_00190DF0 + 4), (int)(unsigned short)*(short *)(*(char **)D_00190DF0 + 6), *(int *)D_00190DE8, 0);
    *(signed char *)D_00190D66 &= 253;
L53516:;
    func_00144E84(44, (int)(short)*(short *)D_00190D6A, (int)(unsigned short)*(short *)(*(char **)D_00190DF0 + 4), (int)(unsigned short)*(short *)(*(char **)D_00190DF0 + 6), *(int *)D_00190DE8, 0);
    *(signed char *)D_00190D66 |= 2;
    func_00144FB4(44, (int)(short)*(short *)D_00190D6A, (int)(unsigned short)*(short *)(*(char **)D_00190DF0 + 4), (int)(unsigned short)*(short *)(*(char **)D_00190DF0 + 6), (int)(*(char **)D_00190DF0 + 12));
    func_0007CA85(func_000A0DD9((int)(short)*(short *)D_00190D64, (int)D_001903A4, 10), (int)(short)(((((int)(unsigned short)*(short *)(*(char **)D_00190DF0 + 4)) + 1) >> 1) + 43), (int)(short)((((int)(short)*(short *)D_00190D6A) + (((int)(unsigned short)*(short *)(*(char **)D_00190DF0 + 6)) >> 1)) - 3), 145, 141);
    if (a1 != 0)
        func_0007CA1F(*(int *)D_00195BEC + 28, 110, 5, 145, 141);
    func_0007CA85(func_000A0DD9(*(unsigned char *)(*(char **)D_00195BEC + 52), (int)D_001903A4, 10), 287, 55, 145, 141);
    *(signed char *)D_0012B508 = 145;
    for (l_24 = 0; l_24 < 12; l_24++) {
        if (*(unsigned char *)(*(char **)D_00195BEC + 16 + l_24) < 35)
            func_0007CA1F(*(int *)(D_0017CC27 + (*(unsigned char *)(*(char **)D_00195BEC + 16 + l_24) << 2)), (short)(*(short *)(D_001854F4 + (l_24 + 2) * 12) + 2), (short)(*(short *)(D_001854F6 + (l_24 + 2) * 12) + 1), 145, 141);
    }
    l_18 = (*(short *)D_001855CC + *(short *)D_001855D0) >> 1;
    l_1C = ((*(short *)D_001855D2 + *(short *)D_001855CE) >> 1) - *(short *)D_001855CE + 3;
    for (l_24 = 0; l_24 < 8; l_24++) {
        func_0007CA85(func_000A0DD9(*(short *)(*(char **)D_00195BEC + l_24 * 2 + 58), (int)D_001903A4, 10), l_18, (short)(*(short *)(D_001854F6 + (l_24 + 18) * 12) + l_1C), 145, 141);
    }
    func_0003F358();
    func_0012B3ED();
    return 0;
}
