/* qmisc.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

struct bf8_5_1 { unsigned char _:5; unsigned char f:1; };
extern char D_0012B508[];
extern char D_001940D5[];
extern char D_001940DA[];
extern char D_00195F2E[];

extern void msgbox_show_quest_text(struct quest *, short, int);
extern void msgbox_show_rsc(int, int);
extern void msgbox_yes_no_quest(short);

void quest_show_message(struct quest *a1, short a2)
{
    *(signed char *)D_0012B508 = 144;
    *(signed char *)D_001940D5 &= 254;
    *(short *)D_00195F2E = 0;
    if (((int)(short)a2) == (-1)) return;
    if ((((int)(short)a2) & 32768) == 0) goto L31B19;
    msgbox_show_rsc((int)(short)((short)*(int *)&a2 & 32767), 1);
    return;
L31B19:;
    if (((struct bf8_5_1 *)&D_001940DA)->f == 0) goto L31B2D;
    msgbox_yes_no_quest((int)(short)a2);
    return;
L31B2D:;
    msgbox_show_quest_text(a1, (int)(short)a2, 1);
}
