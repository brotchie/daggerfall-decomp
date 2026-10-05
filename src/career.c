/* career.c */

#include "dagger.h"

int career_slot_weight(int a)
{
    if (a > 5) return 1;
    if (a > 2) return 2;
    return 3;
}
