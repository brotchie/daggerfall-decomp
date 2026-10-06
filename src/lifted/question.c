/* question.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "ptrint.h"

extern signed char mouse_buttons;
extern short mouse_y;
extern short font_height;
extern signed char key_down_a;
extern signed char key_down_c;
extern signed char key_down_b;
extern iptr screen_buffer;
extern char D_0017539B[];
extern char D_001753DB[];
extern char D_001753E7[];
extern int class_answer_cels[];
extern signed char text_buffer[];
extern char scratch_190d64[];
extern char scratch_190d66[];
extern short scratch_190d68;
extern char frame_counter[];
extern char scratch_buffer[];
extern signed char mouse_buttons_prev;
extern signed char class_question_answer_row[];
extern char class_questions_asked[];
extern signed char D_00199816;
extern signed char D_00199817;
extern signed char D_00199818;
extern signed char class_answer_counts;
extern signed char D_0019981A;
extern signed char D_0019981B;

extern char *text_rsc_load(short, unsigned short, short);
extern int pflc_play(int, iptr);
extern int sound_play_ui(int);
extern iptr disk_read_file(char *, iptr);
extern int rand();
extern int mc_free();
extern int mc_memset();
extern iptr mc_malloc();
extern int atoi();
extern int strlen();
extern int mc_memcpy();
extern iptr memchr();
extern iptr strchr();
extern int func_000A1944();
extern int xn_draw_cel_frame();
extern int xn_str_copy_until();
extern int xn_str_append_char();
extern void fatal_error(char *);
extern void class_question_scroll(int *, short);
extern void text_draw(iptr, int, int);
int class_question_answer_span(short, short, short *, short *);

void class_question_show(int *scroll_cels)
{
    char *paren;
    char *rsc_text;
    char *text;
    int unused;
    char *saved_screen;
    int question;
    int line;

    line = 0;
    question = line;
    *(short *)scratch_190d64 = 0;
    while (memchr(class_questions_asked, (short)question, 10) != 0) {
        question = (rand() % 40) + 1;
    }
    xn_str_append_char(class_questions_asked, (short)question);
    text = (char *)text_rsc_load(9000, 8, 0);
    rsc_text = text;
    scratch_190d68 = question;
    do {
        text = (char *)memchr(text, 123, 32000) + 1;
        xn_str_copy_until(text_buffer, text, 46);
    } while (atoi(text_buffer) != (short)question);
    text += 2;
    *(signed char *)memchr(text, 123, 32000) = 0;
    saved_screen = (char *)mc_malloc(64000, D_0017539B, 176);
    mc_memcpy(saved_screen, screen_buffer, 64000, D_0017539B, 178, 4);
    mc_memset(screen_buffer, 0, 64000, D_0017539B, 179, 4);
    while (*(signed char *)text != 0 && *(signed char *)(text - 1) != 0) {
        xn_str_copy_until(text_buffer, text, 252);
        text += strlen(text_buffer) + 1;
        paren = (char *)strchr(text_buffer, 41);
        if (paren != 0) {
            class_question_answer_row[(unsigned char)paren[-1]] = *(signed char *)&line;
        }
        text_draw((iptr)text_buffer, 20, font_height * (short)line);
        (*(short *)&line)++;
    }
    mc_memcpy(*(int *)scratch_buffer, screen_buffer, 64000, D_0017539B, 193, 4);
    mc_memcpy(screen_buffer, saved_screen, 64000, D_0017539B, 194, 4);
    if (saved_screen != 0 && saved_screen != (char *)(iptr)-1751672937) {
        mc_free(saved_screen, D_0017539B, 195);
        saved_screen = (char *)(iptr)-1751672937;
    }
    *(short *)scratch_190d66 = (font_height * line) - 52;
    class_question_scroll(scroll_cels, 0);
    if (rsc_text == 0 || rsc_text == (char *)(iptr)-1751672937) return;
    mc_free(rsc_text, D_0017539B, 199);
    rsc_text = (char *)(iptr)-1751672937;
}

void class_question_scroll_step(int *scroll_cels, short delta)
{
    if (delta < 0 && *(short *)scratch_190d64 != 0) {
        *(short *)scratch_190d64 += *(int *)&delta;
    } else if (delta > 0 && *(short *)scratch_190d64 < *(short *)scratch_190d66) {
        *(short *)scratch_190d64 += *(int *)&delta;
    }
    *(int *)frame_counter = (int)(short)(*(short *)scratch_190d64 & 7);
    xn_draw_cel_frame(scroll_cels[(short)(*(short *)scratch_190d64 & 15) >> 3], 0, 119);
}

void class_question_answer_anim(short kind)
{
{
    char flc[44];

    func_000A1944((iptr)flc, 0, 44);
    *(short *)flc = 16;
    sound_play_ui(18);
    pflc_play(class_answer_cels[((int)(short)kind)], (iptr)flc);
}
}

int class_question_get_answer(void)
{
    short top;
    short bottom;

    if (key_down_a != 0) return 1;
    if (key_down_b != 0) return 2;
    if (key_down_c != 0) return 3;
    if (((int)(unsigned char)(mouse_buttons & 1)) != 0 && mouse_buttons_prev != mouse_buttons) {
        if (class_question_answer_span((int)(short)((int)(unsigned char)D_00199816), (int)(short)((int)(unsigned char)D_00199817), &top, &bottom) != 0) {
            if (mouse_y > top && mouse_y < bottom) return 1;
        }
        if (class_question_answer_span((int)(short)((int)(unsigned char)D_00199817), (int)(short)((int)(unsigned char)D_00199818), &top, &bottom) != 0) {
            if (mouse_y > top && mouse_y < bottom) return 2;
        }
        if (class_question_answer_span((int)(short)((int)(unsigned char)D_00199818), (int)(short)(*(short *)scratch_190d66 + 48), &top, &bottom) != 0) {
            if (mouse_y > top && mouse_y < bottom) return 3;
        }
    }
    return 0;
}

int class_question_answer_span(short first_row, short end_row, short *top, short *bottom)
{
    first_row = *(int *)&first_row * font_height;
    end_row = *(int *)&end_row * font_height;
    if (((int)(short)first_row) >= (((int)(short)*(short *)scratch_190d64) + 48) || (short)(short)*(int *)&end_row <= *(short *)scratch_190d64) {
        return 0;
    }
    if ((short)(short)*(int *)&first_row < *(short *)scratch_190d64) {
        *top = 135;
    } else {
        *top = (*(int *)&first_row - *(short *)scratch_190d64) + 135;
    }
    if (((int)(short)end_row) > (((int)(short)*(short *)scratch_190d64) + 48)) {
        *bottom = 183;
    } else {
        *bottom = (*(int *)&end_row - *(short *)scratch_190d64) + 135;
    }
    return 1;
}

int class_question_pick_class(void)
{
    short entry;
    int answers;
    short i;

    disk_read_file(D_001753DB, *(int *)scratch_buffer);
    *(int *)&entry = *(int *)scratch_buffer + 18;
    answers = ((((int)(unsigned char)D_0019981B) << 16) | (((int)(unsigned char)D_0019981A) << 8)) | ((int)(unsigned char)class_answer_counts);
    *(int *)&i = 0;
    for (; ((int)(short)i) < 48; (*(int *)&i)++) {
        if ((*(int *)(*(char **)&entry) & 16777215) == answers) return ((int)(short)i) >> 2;
        *(int *)&entry += 3;
    }
    *(int *)&i = 0;
    for (; ((int)(short)i) < 18; (*(int *)&i)++) {
        if ((*(int *)(*(char **)&entry) & 16777215) == answers) return (((int)(short)i) >> 2) + 12;
        *(int *)&entry += 3;
    }
    fatal_error(D_001753E7);
    return 0;
}
