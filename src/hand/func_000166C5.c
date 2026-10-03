/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000166C5 */
extern char D_001789FA[];
extern char D_00190D11[];
extern char D_00195A90[];
extern char D_00195A94[];
extern char D_00195AA4[];
extern char D_00195AC4[];
extern char D_00195D28[];
extern char D_00196488[];
extern char D_0019657C[];
extern char D_00196590[];
extern char D_001965DC[];
extern char D_001965E8[];
extern char D_001966B1[];
extern char D_001966B4[];
extern int func_00017DE3(int);
extern void func_000287BD(char *, int);
extern int func_0007D6AE(int, int);
extern char *func_0008E925(int, int);
extern int func_000C7FD9();


int func_000166C5(int a1)
{
    int l_38;
    int l_34;
    int l_30;
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    char *l_1C;

    l_38 = 0;
    if (*(unsigned char *)D_001966B1 == 3) {}
    if (*(int *)D_001965DC == 2) {
        if (*(short *)(*(char **)D_00195D28 + 4) == 0) {
            if (*(int *)(*(char **)D_00195D28 + 18) == 0) {
                if (func_00017DE3(*(unsigned char *)(D_00196488 + *(int *)D_001965E8)) != 0)
                    return 10;
                return 11;
            }
            l_1C = func_0008E925(*(int *)D_00195AC4, *(int *)(*(char **)(*(char **)D_00195D28 + 18) + 20));
            *(int *)D_00195A90 = *(int *)(l_1C + 7);
            *(int *)D_00195A94 = *(int *)(l_1C + 15);
            if (*(unsigned char *)D_001789FA == 1 && (func_000C7FD9(*(int *)(l_1C + 7), *(int *)(l_1C + 15), *(int *)(*(char **)D_00195AA4 + 7), *(int *)(*(char **)D_00195AA4 + 15)) < 2048 || func_0007D6AE(1, 100) <= 25)) {
                func_000287BD(l_1C, *(int *)(*(char **)D_00195D28 + 18));
                return 7332;
            }
            return 7333;
        }
    } else {
        if (*(unsigned short *)(*(char **)D_00196590 + 33) != 806 && *(unsigned short *)(*(char **)D_00196590 + 33) != 842)
            *(char *)D_001966B4 = 1;
        if ((*(short **)D_0019657C)[*(int *)D_001965E8 * 3 + 2] != 0) {
            *(char *)D_00190D11 = (*(short **)D_0019657C)[*(int *)D_001965E8 * 3 + 2];
            l_30 = 32768;
        } else
            l_30 = 0;
        if (a1 != 0 && (*(short **)D_0019657C)[*(int *)D_001965E8 * 3 + 1] != 0)
            return l_30 | (*(short **)D_0019657C)[*(int *)D_001965E8 * 3 + 1];
        if ((*(short **)D_0019657C)[*(int *)D_001965E8 * 3] != 0)
            return l_30 | (*(short **)D_0019657C)[*(int *)D_001965E8 * 3];
        return l_30 | (*(short **)D_0019657C)[*(int *)D_001965E8 * 3 + 1];
    }
    return 100;
}
