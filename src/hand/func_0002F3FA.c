/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0002F3FA */
struct rect { unsigned short w, h; };
extern short D_0014294C;
extern struct rect *D_00195B68;
extern unsigned short *D_00195BF8;
extern void func_00010F9A(int);
extern int func_000CD53C(void);

void func_0002F3FA(int n)
{
    int saved;

    saved = D_0014294C;
    D_0014294C = (*D_00195BF8 & 1) ? 199 : D_00195B68->h - 2;
    func_000CD53C();
    D_0014294C = saved;
    if (n < 4)
        n = 4;
    else if (n > 32)
        n = 32;
    func_00010F9A(n << 3);
}
