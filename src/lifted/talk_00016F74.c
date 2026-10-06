/* talk.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "ptrint.h"

extern char *talk_question_lines;
extern int talk_question_line_count;

extern void text_draw_coloured(iptr, int, int, int, unsigned char);

void talk_draw_question(int left, int top, int right, int bottom)
{
    int max_lines;
    int row;
    int line;

    row = 0;
    if (talk_question_line_count == 0) return;
    max_lines = bottom - (top / 7);
    if (talk_question_line_count < max_lines) {
        line = 0;
    } else {
        line = talk_question_line_count - max_lines;
    }
    while (line < talk_question_line_count) {
        text_draw_coloured(((iptr *)talk_question_lines)[line++], (int)(short)*(short *)&left, (int)(short)((row * 7) + top), 145, 156);
        row++;
    }
}
