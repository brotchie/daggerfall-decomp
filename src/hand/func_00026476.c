/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00026476 */
extern unsigned char D_001789FA;
extern unsigned D_0019599C;
extern char *D_00195A04;
extern unsigned D_00195BF4;
extern void func_00026508(void);
extern int func_0007D6AE(int, int);

void func_00026476(void)
{
    int i;
    char *p;

    if (D_001789FA == 3) return;
    if (D_0019599C > D_00195BF4) return;
    D_0019599C = D_00195BF4 + func_0007D6AE(1400, 1700);
    p = D_00195A04 + 71;
    for (i = 0; i < 62; i++) {
        if (*(unsigned *)(p + 8) == 0) continue;
        if (*(unsigned *)(p + 8) < D_00195BF4)
            func_00026508();
    }
}
