/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00040FC1 */
struct row { short v; char pad[78]; };
struct ent { char pad[0x7c]; short f7c; };
extern struct row D_0018F08E[];
extern int D_00195AA0;
extern struct ent *D_00195BE0;
extern unsigned char D_00196268;
extern unsigned char D_0019627E;
extern int func_00020C87(unsigned char);
extern void func_0002ECBE(int);
extern int func_0009DC25(void);

void func_00040FC1(int a1)
{
    int v;

    v = D_0018F08E[D_00196268].v;
    D_00195BE0->f7c = 1;
    if (v < -20 && a1 == 0) {
        func_0002ECBE(D_00195AA0);
    } else if (v >= -20 && v <= 0) {
        if ((func_0009DC25() & 1) && a1 == 0)
            func_0002ECBE(D_00195AA0);
        else
            func_00020C87(D_0019627E);
    } else {
        func_00020C87(D_0019627E);
    }
}
