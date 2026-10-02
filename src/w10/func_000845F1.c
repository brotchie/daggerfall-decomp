/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000845F1 */
extern char D_00187D30[];
extern char D_00195AC4[];
extern char D_00195BDC[];
extern char D_00195BF8[];
extern char D_00196268[];
extern char *func_000504D8(int);
extern char *func_00084480(int, int, short);
extern char *func_000844FB(int, int);
extern char *func_0008DCE3(int, int, int);

char *func_000845F1(int a1, short a2, short a3, int a4)
{
    char *l_20;
    char *l_1C;

    if (a4 != 0) {
        switch (a2 >> 7) {
        case 199:
            l_1C = func_000844FB(a1, a2);
            break;
        case 210:
            l_1C = func_00084480(a1, a3 >> 8, a3 & 255);
            break;
        default:
            l_1C = func_0008DCE3(a1, 0, 0);
            *l_1C = 33;
            *(short *)(l_1C + 19) = 8000;
            *(short *)(l_1C + 27) = a2;
            *(int *)(l_1C + 31) = *(int *)(*(char **)D_00195AC4 + 31) + (*(unsigned short *)(*(char **)D_00195BDC + 37))++;
            l_20 = func_000504D8(a2);
            if ((l_20[6] & 2) && (**(unsigned short **)D_00195BF8 & 4))
                *(short *)(l_1C + 27) = 0;
            break;
        }
    } else {
        l_1C = func_0008DCE3(a1, 0, 3);
        *l_1C = 8;
        *(int *)(l_1C + 31) = *(int *)(*(char **)D_00195AC4 + 31) + (*(unsigned short *)(*(char **)D_00195BDC + 37))++;
        *(short *)(l_1C + 27) = a2;
        *(short *)(l_1C + 19) = 8000;
        l_20 = func_000504D8(a2);
        if ((l_20[6] & 2) && (**(unsigned short **)D_00195BF8 & 4))
            *(short *)(l_1C + 27) = 0;
        if (a3 == 0)
            a3 = *(short *)(D_00187D30 + *(unsigned char *)D_00196268 * 2);
        *(short *)(l_1C + 71) = a3;
    }
    return l_1C;
}
