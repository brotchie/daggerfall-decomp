/* itemmakr.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "ptrint.h"

extern char *enchant_spell_lists[];
extern short enchant_fixed_costs[];
extern char monster_soul_values[];

extern int enchant_spell_cost(unsigned char);

int enchant_slot_cost(int cost_code, unsigned char param, unsigned char as_points, int power)
{
    int cost;

    if (cost_code == 99) {
        cost = *(int *)(monster_soul_values + (((int)(unsigned char)param) << 2));
        if (as_points != 0) cost = cost / 100;
        if (cost == 10) cost = cost * ((int)(unsigned char)param);
        return -(cost);
    }
    if (cost_code >= 5 && cost_code <= 7) {
        return enchant_spell_cost((int)(unsigned char)*(signed char *)((char *)(iptr)(enchant_spell_lists[power] + ((int)(unsigned char)param))));
    }
    return (int)(short)enchant_fixed_costs[cost_code];
}
