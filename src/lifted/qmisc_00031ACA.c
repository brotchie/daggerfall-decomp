/* qmisc.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"
#include "bitfield.h"

extern signed char D_0012B508;
extern signed char D_001940D5;
extern signed char D_001940DA;
extern short D_00195F2E;

extern void msgbox_show_quest_text(struct quest *, short, int);
extern void msgbox_show_rsc(int, int);
extern void msgbox_yes_no_quest(short);

void quest_show_message(struct quest *quest, int message)
{
    D_0012B508 = 144;
    D_001940D5 &= 254;
    D_00195F2E = 0;
    if (((int)(short)message) == (-1)) return;
    if ((((int)(short)message) & 32768) != 0) {
        msgbox_show_rsc((int)(short)((short)*(int *)&message & 32767), 1);
        return;
    }
    if (((struct bf8_5_1 *)&D_001940DA)->f != 0) {
        msgbox_yes_no_quest((int)(short)message);
        return;
    }
    msgbox_show_quest_text(quest, (int)(short)message, 1);
}
