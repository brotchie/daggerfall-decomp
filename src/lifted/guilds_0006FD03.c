/* guilds.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern struct spell *selected_spell;
extern char D_001940D5[];

extern int spell_effect_text_index(short);
extern void msgbox_show_rsc(int, int);

void func_0006FD03(short a1)
{
    if (selected_spell->effects[(int)(short)a1].type == 255) return;
    *(signed char *)D_001940D5 |= 1;
    msgbox_show_rsc((int)(short)(spell_effect_text_index((int)(short)a1) + 1200), 1);
}
