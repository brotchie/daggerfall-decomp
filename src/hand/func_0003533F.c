/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0003533F */
struct w0 { unsigned short f0; };
struct w2 { unsigned short f0; unsigned short f2; };
struct dun { unsigned char f0; char pad[79]; };
extern short D_0014294C;
extern unsigned char D_001789FA;
extern char D_00187CA8;
extern struct dun D_0018F08C[];
extern struct w2 *D_00195B68;
extern struct w0 *D_00195BF8;
extern unsigned char D_00195E2A[];
extern unsigned char D_00196268;
extern int func_0001FFF1(void);
extern void func_000C9A89(void);
extern void func_000C9CB9(void);

void func_0003533F(void)
{
    int n;
    int kind;
    int old;

    n = func_0001FFF1();
    if (D_00187CA8 == 0)
        return;
    if (D_001789FA != 1)
        return;
    kind = D_00195E2A[n];
    if (D_0018F08C[D_00196268].f0 != 0)
        kind = D_0018F08C[D_00196268].f0 - 1;
    if ((kind & 127) == 5) {
        old = D_0014294C;
        D_0014294C = (D_00195BF8->f0 & 1) ? 199 : D_00195B68->f2 - 2;
        func_000C9A89();
        D_0014294C = old;
    } else if ((kind & 127) == 4) {
        old = D_0014294C;
        D_0014294C = (D_00195BF8->f0 & 1) ? 199 : D_00195B68->f2 - 2;
        func_000C9CB9();
        D_0014294C = old;
    }
}
