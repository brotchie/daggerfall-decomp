/* smc.c: helpers for the self-modifying-code designs (xnsmc.h). */
#include "xnsmc.h"

int xn_planted_count(const void *body, int stride, int max)
{
    const u8 *p = (const u8 *)body;
    int n;

    for (n = 0; n < max; n++, p += stride)
        if (*p == 0xC3)
            return n;
    return max;
}
