/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000932C9 */
struct rect { short x0, y0, x1, y1; char pad[4]; };
extern char *D_00143550;
extern char D_0017704C[];        /* __FILE__ */
extern struct rect D_00188425[];
extern void func_000A1023(char *, char *, int, char *, int, int);

void func_000932C9(int a1, char *a2)
{
    int y;

    for (y = D_00188425[a1].y0; y <= D_00188425[a1].y1; y++)
        func_000A1023(y * 320 + D_00143550 + D_00188425[a1].x0, y * 320 + a2 + D_00188425[a1].x0, D_00188425[a1].x1 - D_00188425[a1].x0 + 1, D_0017704C, 631, 4);
}
