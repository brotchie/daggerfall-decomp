/* kludge.c */

#include "dagger.h"

char *func_00045AED(char *p1, int p2)
{
    char *l1;
    char *l2;
    l1 = func_0008DCE3(p1, 0, 0x6b);
    *l1 = 2;
    l2 = l1 + 0x47;
    func_0005E450((unsigned short)p2, l2);
    return l1;
}
