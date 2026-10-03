/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0006FF7E */
extern int D_00187023[];
extern char *D_0019671C;
extern int func_000700EE(int *, int, int);

int func_0006FF7E(int a1)
{
    int l_28;
    int l_24;
    int l_20;
    int l_1C;

    if (*(short *)(D_0019671C + 29) < 0)
        return 1;
    l_20 = func_000700EE(&l_28, D_00187023[a1], -1);
    if (l_20 < 22)
        return 2;
    l_1C = func_000700EE(&l_28, D_00187023[a1], l_28);
    if (l_1C < 4)
        return 2;
    return 0;
}
