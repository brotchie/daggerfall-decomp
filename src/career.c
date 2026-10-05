/* career.c */

#include "dagger.h"

int career_slot_weight(int slot)
{
    if (slot > 5) return 1;
    if (slot > 2) return 2;
    return 3;
}
