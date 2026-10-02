/* qmisc.c */

#include "dagger.h"

void func_0003081B(unsigned char a)
{
    func_0008ECBD(D_00195AC4, a);
    func_0008ECBD(D_001959A8, a);
}

int func_000309E8(char *p1, short p2)
{
    short l;
    l = ((short *)(p1 + 0x24))[p2];
    return (int)(p1 + l);
}

int func_00030A23(char *p1, short p2, short p3)
{
    int l;
    l = func_000309E8(p1, p2);
    return l + p3 * D_00199788[p2];
}
