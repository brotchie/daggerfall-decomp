/* talk.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern char *talk_question_lines;
extern int talk_question_line_count;

extern void text_draw_colored(int, int, int, int, unsigned char);

void talk_draw_question(int a1, int a2, int a3, int a4)
{
    int l_14;
    int l_10;
    int l_C;

    l_10 = 0;
    if (talk_question_line_count == 0) return;
    l_14 = a4 - (a2 / 7);
    if (talk_question_line_count < l_14) {
        l_C = 0;
    } else {
        l_C = talk_question_line_count - l_14;
    }
    while (l_C < talk_question_line_count) {
        text_draw_colored(*(int *)((char *)(int)(talk_question_lines + (l_C++ << 2))), (int)(short)*(short *)&a1, (int)(short)((l_10 * 7) + a2), 145, 156);
        l_10++;
    }
}
