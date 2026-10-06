/* talk.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "ptrint.h"
#include "clib.h"

extern char D_001703F0[];
extern signed char text_rsc_buffer[];
extern char *talk_answer_lines;
extern int talk_answer_line_count;
extern int talk_answer_scroll;

extern void text_draw_coloured(iptr, int, int, int, unsigned char);

void talk_draw_answer(int left, int top, int right, int bottom)
{
    int max_lines;
    int row;
    int line;
    int colour;
    int indent;
    int y;

    row = 0;
    if (talk_answer_line_count == 0) return;
    max_lines = bottom - (top / 7);
    line = talk_answer_scroll;
    do {
        if (line >= talk_answer_line_count) return;
        mc_strncpy((char *)text_rsc_buffer, ((char **)talk_answer_lines)[line++], 2048, D_001703F0, 1192);
        if (((int)(unsigned char)(text_rsc_buffer[0] & 128)) != 0) {
            colour = 96;
        } else {
            colour = 145;
        }
        text_rsc_buffer[0] &= 127;
        indent = 0;
        while (((int)(unsigned char)text_rsc_buffer[indent]) == 32) indent++;
        y = top + (row * 7);
        text_draw_coloured(((iptr)text_rsc_buffer) + indent, (int)(short)*(short *)&left, (int)(short)*(short *)&y, (int)(short)*(short *)&colour, 156);
        row++;
    } while (y < bottom);
}
