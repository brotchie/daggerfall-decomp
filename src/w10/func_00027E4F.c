/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00027E4F */
extern char D_00143550[];
extern char D_001707AE[];
extern char D_0017A103[];
extern char D_00190DE4[];
extern char D_00195AA4[];
extern char D_00195AC4[];
extern char D_00195BDC[];
extern char D_00196D88[];
extern char D_00196D8C[];
extern char D_00196D90[];
extern char D_00196D98[];
extern char D_00196DA4[];
extern void func_000286F6(void);
extern int func_000A1023();
extern void func_000A134C(short, short, int);

struct kb { unsigned char _:3; unsigned char f:1; };

#define SCR (*(unsigned char **)D_00143550)
#define MAP (*(unsigned char **)D_00196DA4)
#define VX (*(int *)D_00196D88)
#define VY (*(int *)D_00196D8C)

void func_00027E4F(void)
{
    int l_44;
    int l_40;
    int l_3C;
    int l_38;
    int l_34;
    int l_30;
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;

    func_000A1023(*(int *)D_00143550, *(int *)D_00190DE4, 64000, D_001707AE, 695, 4);
    l_30 = *(int *)(*(char **)D_00195AA4 + 7) - *(int *)(*(char **)D_00195AC4 + 7);
    l_2C = *(int *)(*(char **)D_00195AA4 + 15) - *(int *)(*(char **)D_00195AC4 + 15);
    l_30 >>= 6;
    l_2C >>= 6;
    l_2C = (*(unsigned char *)(*(char **)D_00195BDC + 33) << 6) - l_2C - 1;
    VX = l_30;
    VY = l_2C;
    VX -= 37;
    VY -= 20;
    VX += *(int *)D_00196D98;
    VY += *(int *)D_00196D90;
    l_40 = *(unsigned char *)(*(char **)D_00195BDC + 33) << 6;
    l_3C = *(unsigned char *)(*(char **)D_00195BDC + 32) << 6;
    if (VX < 0) {
        *(int *)D_00196D98 -= VX;
        VX = 0;
    }
    if (VX > l_3C - 38) {
        *(int *)D_00196D98 -= VX - (l_3C - 38);
    }
    if (VY < 0) {
        *(int *)D_00196D90 -= VY;
        VY = 0;
    }
    if (VY > l_40 - 10) {
        *(int *)D_00196D90 -= VY - (l_40 - 10);
    }
    l_24 = 10;
    l_38 = VY;
    while (l_24 < 170 && l_38 < l_40) {
        for (l_44 = 0; l_44 < 2; l_44++) {
            l_34 = VX;
            l_28 = 10;
            while (l_28 < 310 && l_34 < l_3C) {
                l_1C = MAP[l_3C * l_38 + l_34];
                if (!(l_1C == 0 || l_1C == 251 || l_1C == 250)) {
                    l_20 = D_0017A103[MAP[l_3C * l_38 + l_34]];
                    SCR[l_24 * 320 + l_28] = l_20;
                    SCR[l_24 * 320 + l_28 + 1] = l_20;
                }
                l_28 += 2;
                l_34++;
            }
            l_24++;
        }
        l_38++;
    }
    func_000286F6();
    if ((*(struct kb *)0x46c).f == 0) return;
    l_28 = (l_30 - VX) * 2 + 10;
    l_24 = (l_2C - VY) * 2 + 10;
    if (l_24 >= 169) return;
    func_000A134C(l_28, l_24, 145);
    func_000A134C(l_28 + 1, l_24, 145);
    func_000A134C(l_28, l_24 + 1, 145);
    func_000A134C(l_28 + 1, l_24 + 1, 145);
}
