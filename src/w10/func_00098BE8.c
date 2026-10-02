/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00098BE8 */
extern char D_00188249[];
extern char D_0018824D[];
extern char D_00190FE4[];
extern char D_00195BE0[];
extern void func_0007CA1F(char *, int, int, int, unsigned char);
extern char *func_000A0DD9(int, char *, int);

void func_00098BE8(void)
{
    int i;

    for (i = 0; i < 7; i++)
        func_0007CA1F(func_000A0DD9((100 - ((signed char *)(*(char **)D_00195BE0 + 68))[i]) / 5, D_00190FE4, 10), *(short *)(D_00188249 + (i << 3)), *(short *)(D_0018824D + (i << 3)), 145, 156);
}
