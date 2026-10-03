/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0004A0C0 */
#pragma pack(1)
struct rec { char pad0[24]; unsigned char kind; char pad19; };
#pragma pack()
extern int D_0018467C;
extern char *D_00195BDC;
extern int func_0007D6AE(int, int);
extern int func_0008BD50(struct rec *);

int func_0004A0C0(short a1)
{
    struct rec *p;
    short i;
    short c;

    p = *(struct rec **)(D_00195BDC + 43);
    for (c = i = 0; i < *(unsigned short *)(D_00195BDC + 41); i++, p++)
        if (p->kind == a1) c++;
    if (c == 0) return D_0018467C;
    if (c == 1)
        c = 0;
    else
        c = func_0007D6AE(0, c - 1) + 1;
    p = *(struct rec **)(D_00195BDC + 43);
    while (c != 0) {
        while (p->kind != a1) p++;
        c--;
    }
    return func_0008BD50(p);
}
