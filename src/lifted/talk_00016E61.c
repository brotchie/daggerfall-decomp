/* talk.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern char D_001703F0[];
extern signed char text_rsc_buffer[];
extern char *talk_answer_lines;
extern int talk_answer_line_count;
extern int talk_answer_scroll;

extern int mc_strncpy();
extern void text_draw_coloured(int, int, int, int, unsigned char);

void talk_draw_answer(int a1, int a2, int a3, int a4)
{
    int l_20;
    int l_1C;
    int l_18;
    int l_14;
    int l_10;
    int l_C;

    l_1C = 0;
    if (talk_answer_line_count == 0) return;
    l_20 = a4 - (a2 / 7);
    l_18 = talk_answer_scroll;
    do {
        if (l_18 >= talk_answer_line_count) return;
        mc_strncpy((int)text_rsc_buffer, *(int *)((char *)(int)(talk_answer_lines + (l_18++ << 2))), 2048, (int)D_001703F0, 1192);
        if (((int)(unsigned char)(text_rsc_buffer[0] & 128)) != 0) {
            l_14 = 96;
        } else {
            l_14 = 145;
        }
        text_rsc_buffer[0] &= 127;
        l_10 = 0;
        while (((int)(unsigned char)text_rsc_buffer[l_10]) == 32) l_10++;
        l_C = a2 + (l_1C * 7);
        text_draw_coloured(((int)text_rsc_buffer) + l_10, (int)(short)*(short *)&a1, (int)(short)*(short *)&l_C, (int)(short)*(short *)&l_14, 156);
        l_1C++;
    } while (l_C < a4);
}
