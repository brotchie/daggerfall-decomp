/* generate.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "structs.h"

extern struct rect chargen_buttons[];
extern short scratch_190d6a;
extern short chargen_selected_attribute;


void chargen_select_attribute(slot16 attribute)
{
    chargen_selected_attribute = *(int *)&attribute;
    chargen_buttons[30].y0 = (scratch_190d6a = chargen_buttons[((int)(short)attribute) + 20].y0 + 1);
    chargen_buttons[30].y1 = scratch_190d6a + 6;
    chargen_buttons[31].y0 = scratch_190d6a + 13;
    chargen_buttons[31].y1 = scratch_190d6a + 19;
}
