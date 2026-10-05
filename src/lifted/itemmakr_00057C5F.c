/* itemmakr.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern short enchant_fixed_costs[];
extern char monster_soul_values[];

extern int enchant_spell_cost(unsigned char);

int enchant_value_slot_cost(int a1, unsigned char a2, unsigned char a3, int a4)
{
    int l_1C;

    if (a1 == 99) {
        l_1C = *(int *)(monster_soul_values + (((int)(unsigned char)a2) << 2));
        if (a3 != 0) l_1C = l_1C / 100;
        if (l_1C == 10) l_1C = l_1C * ((int)(unsigned char)a2);
        return -(l_1C);
    }
    if (a1 >= 5 && a1 <= 7) return enchant_spell_cost((int)(unsigned char)a2);
    return (int)(short)enchant_fixed_costs[a1];
}
