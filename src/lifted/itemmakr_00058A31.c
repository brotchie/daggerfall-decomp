/* itemmakr.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern struct enchantment itemmaker_slots[];

extern void func_00058AF7(void);

int itemmaker_power_excluded(int power)
{
    int slot;

    func_00058AF7();
    for (slot = 0; slot < 10; slot++) {
        if (power == 25 && (itemmaker_slots[slot].type) == 25 && (itemmaker_slots[slot].param) == 5) {
            func_00058AF7();
            return 1;
        }
        if (power == 14 && (itemmaker_slots[slot].type) == 14 && (itemmaker_slots[slot].param) == 5) {
            func_00058AF7();
            return 1;
        }
    }
    func_00058AF7();
    return 0;
}
