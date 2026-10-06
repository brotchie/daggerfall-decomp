/* parse.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"
#include "clib.h"

extern char D_0017110C[];
extern char D_001711A4[];
extern signed char text_rsc_buffer[];
extern struct quest *current_quest;

extern char *text_qrc_load(char *, short, short, short);
#pragma aux mc_set_location parm routine [];

void quest_load_text(struct quest *quest, int message_id, short flags, int width)
{
    char *text;
    char file_name[16];

    current_quest = quest;
    if (quest->text_file != 0) {
        mc_set_location(2016, D_0017110C);
        mc_sprintf(file_name, D_001711A4, quest->text_file);
    } else {
        mc_memcpy(file_name, quest->name, 8, D_0017110C, 2018, 13);
    }
    file_name[8] = 0;
    text = (char *)text_qrc_load(file_name, (int)(short)*(short *)&message_id, (int)(short)flags, (int)(short)*(short *)&width);
    if (text == 0) return;
    mc_strncpy((char *)text_rsc_buffer, text, 2048, D_0017110C, 2022);
    if (text == 0 || text == (char *)0x97979797) return;
    mc_free(text, D_0017110C, 2023);
    text = (char *)0x97979797;
}
