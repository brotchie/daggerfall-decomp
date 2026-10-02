/* talk.c */

#include "dagger.h"

void func_000164EF(void) { }

int func_00016507(int a) { return 0; }

int func_0001811D(char *p)
{
    char *l;
    l = func_0008E925(D_00195AC4, *(int *)(p + 0x14));
    return func_000C7FD9(*(int *)(l + 7), *(int *)(l + 0xf),
                         *(int *)(D_00195AA4 + 7), *(int *)(D_00195AA4 + 0xf));
}

/* between the talk.c and faction.c runs: unit not certain */
int func_000192EE(short a)
{
    int r;
    r = func_00019323(D_0019672C, a);
    return r;
}

/* between the talk.c and faction.c runs: unit not certain */
int func_0001939C(int a, int b)
{
    D_00190BE4 = a;
    D_00195B84 = 0;
    func_000193DD(b);
    return D_00195B84;
}
