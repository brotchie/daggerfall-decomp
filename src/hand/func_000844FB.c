/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000844FB */
struct item {
    char type;          /* 0 */
    char pad1[18];
    short f19;          /* 19 */
    short f21;          /* 21 */
    char pad2[4];
    short f27;          /* 27 */
    short f29;          /* 29 */
    int f31;            /* 31: a name address */
};
extern char *D_00195AC4;
extern char *D_00195BDC;
extern struct item *func_0008DCE3(int, int, int);

struct item *func_000844FB(int a1, int a2)
{
    struct item *l_20;
    int l_1C;
    int l_18;

    l_1C = (a2 & 31) - 2;
    if (l_1C == 13 || l_1C == 14) {
        l_18 = 659;
        l_20 = func_0008DCE3(a1, 0, l_18);
        l_20->f19 = 0;
    } else {
        l_20 = func_0008DCE3(a1, 0, 0);
        l_20->f19 = 0;
    }
    l_20->type = 34;
    l_20->f29 = 0;
    l_20->f19 = 0;
    l_20->f27 = a2;
    if (l_1C == 9 || l_1C == 16) {
        l_20->f31 = *(int *)(D_00195AC4 + 31) + (*(unsigned short *)(D_00195BDC + 39))++;
    } else {
        l_20->f31 = *(int *)(D_00195AC4 + 31) + (*(unsigned short *)(D_00195BDC + 37))++;
    }
    l_20->f21 = 1;
    return l_20;
}
