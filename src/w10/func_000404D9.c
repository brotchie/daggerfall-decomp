/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000404D9 */
extern char D_00195BDC[];
extern char D_00196DA4[];

int func_000404D9(int x, int y)
{
    int r;
    int w;
    unsigned char *p;

    r = 0;
    x >>= 6;
    y >>= 6;
    y = (*(unsigned char *)(*(char **)D_00195BDC + 33) << 6) - y - 1;
    w = *(unsigned char *)(*(char **)D_00195BDC + 33) << 6;
    p = (unsigned char *)(*(char **)D_00196DA4 + ((*(unsigned char *)(*(char **)D_00195BDC + 32) << 6) * y + x));
    r = p[0] | p[-1] | p[1];
    r |= p[-w] | p[-w - 1] | p[-w + 1];
    r |= p[w] | p[w - 1] | p[w + 1];
    return (r == 0) ? 1 : 0;
}
