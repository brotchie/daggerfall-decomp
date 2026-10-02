/* steal.c */

#include "dagger.h"

/* between the steal.c and camera.c runs: unit not certain */
int func_00013A00(char *p1, int p2, int p3)
{
    D_00195AF4 = 0;
    D_00190BE4 = p2;
    D_00190BE8 = p3;
    func_0008E3F7(*(int *)(p1 + 0x3f), func_00013981);
    return D_00195AF4;
}
