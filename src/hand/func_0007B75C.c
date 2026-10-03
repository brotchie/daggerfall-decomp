/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0007B75C */
extern char D_0012B508[];
extern char D_00142928[];
extern char D_0014292C[];
extern char D_00143550[];
extern char D_00147954[];
extern char D_00176884[];
extern char D_00187A90[];
extern char D_00187A92[];
extern char D_00187A94[];
extern char D_00187A96[];
extern char D_00195B5C[];
extern char D_00195BE8[];
extern char D_00195C44[];
extern void func_0007CA85(int, int, int, int, unsigned char);
extern int func_000A1023();
extern int func_00144F68();
extern int func_001532B4();

void func_0007B75C(int a1, int a2, int a3)
{
    int l_10;

    func_000A1023(*(int *)D_00143550, *(int *)D_00195BE8, 64000, (int)D_00176884, 977, 4);
    if (a1 == 0) goto L7B7D9;
    func_00144F68((int)(unsigned short)*(short *)(*(char **)D_00195B5C), (int)(unsigned short)*(short *)(*(char **)D_00195B5C + 2), (int)(unsigned short)*(short *)(*(char **)D_00195B5C + 4), (int)(unsigned short)*(short *)(*(char **)D_00195B5C + 6), (int)(*(char **)D_00195B5C + 12));
L7B7D9:;
    for (l_10 = 0; l_10 < 6; l_10++) {
        if (((1 << l_10) & a2) != 0) {
            func_00144F68((((l_10) < 3) ? 40 : 200), ((l_10 % 3) * 65) + 4, 80, 50, (int)(*(char **)D_00147954 + (l_10 * 4000)));
            func_0007CA85((int)(*(char **)D_00195C44 + (l_10 << 5)), (int)(short)(((l_10) < 3) ? 80 : 246), (int)(short)(((l_10 % 3) * 65) + 57), 145, 156);
        }
    }

    l_10 = ((a3 < 3) ? a3 : a3 + 3);
    *(signed char *)D_0012B508 = 146;
    *(short *)D_00142928 = *(short *)(D_00187A90 + (l_10 * 12)) - 1;
    *(short *)D_0014292C = *(short *)(D_00187A92 + (l_10 * 12)) - 1;
    func_001532B4((int)(short)(*(short *)(D_00187A94 + (l_10 * 12)) + 1), (int)(short)(*(short *)(D_00187A92 + (l_10 * 12)) - 1));
    func_001532B4((int)(short)(*(short *)(D_00187A94 + (l_10 * 12)) + 1), (int)(short)(*(short *)(D_00187A96 + (l_10 * 12)) + 1));
    func_001532B4((int)(short)(*(short *)(D_00187A90 + (l_10 * 12)) - 1), (int)(short)(*(short *)(D_00187A96 + (l_10 * 12)) + 1));
    func_001532B4((int)(short)(*(short *)(D_00187A90 + (l_10 * 12)) - 1), (int)(short)(*(short *)(D_00187A92 + (l_10 * 12)) - 1));
}
