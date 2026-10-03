/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000843E0 */
extern char *D_00195AC4;
extern char *D_00195BDC;
extern char *func_0008DCE3(int, int, int);

char *func_000843E0(int a1, short a2, short a3, int a4)
{
    char *l_1C;

    l_1C = func_0008DCE3(a1, 0, 62);
    *l_1C = a4 ? 32 : 6;
    *(short *)(l_1C + 23) = 0;
    *(short *)(l_1C + 29) = a2;
    *(short *)(l_1C + 27) = a3;
    *(short *)(l_1C + 19) = 8000;
    *(int *)(l_1C + 31) = *(int *)(D_00195AC4 + 31) + (*(unsigned short *)(D_00195BDC + 37))++;
    return l_1C;
}
