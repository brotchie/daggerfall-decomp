/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00096CCF */
extern char D_0017704C[];
extern char D_00195AC4[];
extern char D_00195B04[];
extern char D_00195B44[];
extern char D_00195BE0[];
extern char D_00195BF4[];
extern int func_0003A0C0(int, int);
extern int func_0005AFD5();
extern void func_00060270(int, int);
extern int func_0008DCE3(int, int, int);
extern int func_000A1023();

struct S89 { char p[73]; unsigned char f; char q[15]; };
struct E4 { short t; short v; };

void func_00096CCF(int a1, int a2)
{
    int l_24;
    int l_20;
    int l_1C;
    int l_18;
    int l_14;

    l_24 = 0;
    l_1C = a1 + 71;
    l_14 = 0;
L96CF9:;
    if (l_24 >= 10) goto L96D11;
    if (((int)(short)*(short *)((char *)((l_24 << 2) + l_1C) + 67)) != (-1)) goto L96D16;
L96D11:;
    goto L96E9B;
L96D16:;
    switch (*(unsigned short *)((char *)((l_24 << 2) + l_1C) + 67)) {
case 1:
    l_20 = 0;
L96D71:;
    if ((*(struct S89 **)D_00195B04)[l_20].f == ((struct E4 *)(l_1C + 67))[l_24].v) goto L96D98;
    l_20++;
    goto L96D71;
L96D98:;
    l_18 = func_0008DCE3(*(int *)D_00195AC4, 0, 89);
    *(signed char *)((char *)l_18) = 9;
    *(short *)((char *)l_18 + 21) = 3;
    func_000A1023(l_18 + 71, (int)(*(char **)D_00195B04 + (l_20 * 89)), 89, (int)D_0017704C, 2092, 4);
    l_14 = l_18 + 71;
    *(signed char *)((char *)l_14 + 72) = *(signed char *)&a2 + 200;
    l_20 = 0;
L96DFD:;
    if (l_20 < 3) goto L96E0D;
    goto L96E4C;
L96E05:;
    l_20++;
    goto L96DFD;
L96E0D:;
    if (((int)(unsigned char)*(signed char *)((char *)((l_20 * 2) + l_14))) == 255) goto L96E4A;
    *(signed char *)((char *)((l_20 * 3) + l_14) + 14) = 255;
    *(signed char *)((char *)((l_20 * 3) + l_14) + 15) = 0;
    *(signed char *)((char *)((l_20 * 3) + l_14) + 16) = 0;
L96E4A:;
    goto L96E05;
L96E4C:;
    func_0005AFD5(l_18);
    goto L96E90;
case 5:
    *(int *)D_00195B44 = *(int *)D_00195BF4;
    goto L96E90;
case 9:
    *(signed char *)(*(char **)D_00195BE0 + 138) |= 2;
    goto L96E90;
case 10:
    *(short *)(*(char **)D_00195BE0 + 157 + (((int)(short)*(short *)((char *)((l_24 << 2) + l_1C) + 69)) * 6)) += 15;
default:
L96E90:;
    l_24++;
    goto L96CF9;
L96E9B:;
    if (l_24 == 0) goto L96EA7;
    if (l_14 != 0) goto L96EA9;
L96EA7:;
    return;
L96EA9:;
    func_00060270(a1, func_0003A0C0(l_14, *(int *)D_00195BE0));
}
}
