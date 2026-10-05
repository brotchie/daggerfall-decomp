/* parse.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern char D_0017110C[];
extern char D_001711A4[];
extern signed char text_rsc_buffer[];
extern struct quest *current_quest;

extern int text_qrc_load(int, short, short, short);
extern int mc_free();
extern int mc_strncpy();
extern int func_000A0ED9(int, int);
extern int mc_sprintf(int, ...);
extern int mc_memcpy();
#pragma aux func_000A0ED9 parm routine [];

void quest_load_text(struct quest *a1, int a2, short a3, int a4)
{
    int l_10;
    char l_2C[16];

    current_quest = a1;
    if (a1->text_file != 0) {
        func_000A0ED9(2016, (int)D_0017110C);
        mc_sprintf((int)l_2C, (int)D_001711A4, a1->text_file);
    } else {
        mc_memcpy((int)l_2C, a1->name, 8, (int)D_0017110C, 2018, 13);
    }
    *(signed char *)((char *)l_2C + 8) = 0;
    l_10 = text_qrc_load((int)l_2C, (int)(short)*(short *)&a2, (int)(short)a3, (int)(short)*(short *)&a4);
    if (l_10 == 0) return;
    mc_strncpy((int)text_rsc_buffer, l_10, 2048, (int)D_0017110C, 2022);
    if (l_10 == 0 || l_10 == (-1751672937)) return;
    mc_free(l_10, (int)D_0017110C, 2023);
    l_10 = -1751672937;
}
