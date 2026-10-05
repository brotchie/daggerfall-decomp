/* career.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern char disk_last_file_size[];
extern signed char mouse_buttons;
extern short mouse_x;
extern short mouse_y;
extern int screen_buffer;
extern int D_00147954;
extern signed char xn_mouse_cursor_drawn;
extern char D_00170738[];
extern char D_00170741[];
extern char D_00170750[];
extern char D_0017075D[];
extern char D_0017077B[];
extern unsigned char D_00178630[];   /* _IsTable */
extern struct rect career_answer_boxes[];
extern signed char D_00179FF8[];
extern struct rect career_bio_buttons[];
extern signed char text_buffer[];
extern int D_00190C74;
extern char D_00190C78[];
extern int scratch_190cac;
extern char scratch_190d64[];
extern char scratch_190d66[];
extern short scratch_190d68;
extern short scratch_190d6a;
extern char scratch_190de4[];
extern struct record *player_object;
extern struct character *player_character;
extern int window_image;
extern char *scratch_buffer;
extern signed char text_macro_imperial;
extern signed char mouse_buttons_prev;
extern int D_00196D68;
extern int career_bio_text;
extern int D_00196D70;
extern int career_bio_lines;
extern int career_bio_page;
extern short reputation_baseline;
extern signed char career_bio_ask;

extern unsigned char *career_answer_effect(unsigned char *);
extern int career_nearest_class(void);
extern int place_marker_in_range(struct record *, int);
extern int text_rsc_load(int, int, int);
extern int sound_play(int, struct record *, int);
extern int disk_read_file(char *, int);
extern int disk_write_arena2_file(char *, int, int);
extern int rand_range(int, int);
extern int rand();
extern int mc_free();
extern int mc_memset();
extern int atoi();
extern int strlen();
extern int mc_set_location(int, int);
extern int mc_sprintf(int, ...);
extern int mc_memcpy();
extern int memchr();
extern int xn_gfx_present_inclusive();
extern int xn_str_find_byte_pair();
extern int xn_mouse_poll_clamped();
extern int xn_mouse_cursor_move();
extern void msgbox_show_rsc(int, int);
extern void text_draw_coloured(int, int, int, int, unsigned char);
extern void object_free_later(struct record *);
extern void cursor_draw_arrow(void);
unsigned char *career_skip_word(unsigned char *);
unsigned char *career_draw_lines(unsigned char *, int);
int career_bio_count_lines(void);
void career_find_question(int);
void career_show_question(unsigned char *);
void career_wait_answer(void);
void career_random_answer(void);
void career_apply_answer(int);
void career_bio_draw(void);
#pragma aux mc_set_location parm routine [];

void career_background_summary(int class_id, int ask)
{
    int question;
    int text;
    unsigned char *cursor;
    int start;

    career_bio_ask = *(signed char *)&ask;
    mc_memcpy((int)&reputation_baseline, (int)(signed char *)&player_character->reputation[0], 10, (int)D_00170738, 48, 10);
    text_macro_imperial = rand() % 6;
    scratch_190d68 = (unsigned short)(unsigned char)D_00179FF8[rand_range(0, 9)];
    scratch_190d6a = 1;
    D_00190C74 = rand();
    *(int *)D_00190C78 = rand();
    if (class_id == 18) class_id = career_nearest_class();
    mc_set_location(59, (int)D_00170738);
    mc_sprintf((int)text_buffer, (int)D_00170741, class_id);
    mc_memset((int)scratch_buffer, 0, 64000, (int)D_00170738, 60, 4);
    disk_read_file(text_buffer, (int)scratch_buffer + 1);
    *scratch_buffer = 10;
    if (ask != 0) disk_read_file(D_00170750, D_00147954);
    for (question = 0; question < 12; question++) {
        if (ask != 0) {
            mc_memcpy(screen_buffer, D_00147954, 64000, (int)D_00170738, 69, 4);
            xn_mouse_cursor_drawn &= 254;
        }
        career_find_question(question + 1);
        if (ask != 0) {
            career_wait_answer();
        } else {
            career_random_answer();
        }
        career_apply_answer(question);
    }
    text = text_rsc_load((int)(short)(class_id + 4116), 0, 0);
    cursor = (unsigned char *)text;
    start = (int)cursor;
    while (*cursor != 0) {
        if (*cursor == 253 || *cursor == 252) {
            *cursor = 0;
        } else if (*cursor == 251) {
            *cursor = 32;
        }
        cursor++;
    }
    disk_write_arena2_file(D_0017075D, start, (int)cursor - start + 1);
    if (start != 0 && start != (-1751672937)) {
        mc_free(start, (int)D_00170738, 94);
        start = -1751672937;
    }
    msgbox_show_rsc(35, 1);
}

unsigned char *career_skip_word(unsigned char *text)
{
    while (*text > 32) text++;
    while (*text <= 32) text++;
    return text;
}

void career_find_question(int number)
{
    unsigned char *line;
    unsigned char found;

    found = 0;
    line = (unsigned char *)scratch_buffer;
    while (found == 0) {
        line = (unsigned char *)memchr(line, 10, 2000);
        line++;
        if ((D_00178630[(unsigned char)((signed char)*line + 1)] & 32) != 0 && atoi(line) == number) {
            career_show_question(line);
            found = 1;
        }
    }
}

void career_show_question(unsigned char *text)
{
    text = career_skip_word(text);
    *(short *)scratch_190d64 = 0;
    text = career_draw_lines(text, 0);
    *(unsigned char **)scratch_190de4 = text;
    while ((D_00178630[(unsigned char)((signed char)*text + 1)] & 32) == 0 && *text != 0) {
        (*(short *)scratch_190d64)++;
        text = career_draw_lines(career_skip_word(text), 1);
        while ((D_00178630[(unsigned char)((signed char)*text + 1)] & 224) == 0 && *text != 0) {
            text = (unsigned char *)memchr(text, 10, 2000);
            text++;
        }
    }
}

unsigned char *career_draw_lines(unsigned char *text, int one_line)
{
    unsigned char *line_end;
    int x;
    int y;

    x = (career_answer_boxes[(int)(short)*(short *)scratch_190d64].x0) + 21;
    y = career_answer_boxes[(int)(short)*(short *)scratch_190d64].y0;
    if (*(short *)scratch_190d64 != 0) y += 5;
    while (1) {
        line_end = (unsigned char *)memchr(text, 13, 2000);
        *line_end = 0;
        if (career_bio_ask != 0) {
            text_draw_coloured((int)text, (int)(short)*(short *)&x, (int)(short)*(short *)&y, 145, 141);
        }
        y += 10;
        *line_end = 13;
        text = line_end + 2;
        if (one_line != 0 || *text != 9) return text;
        while (*text == 9) text++;
    }
}

void career_wait_answer(void)
{
    int answer;

    *(short *)scratch_190d66 = 65535;
    while (mouse_buttons != 0) xn_mouse_poll_clamped();
    while (((int)(short)*(short *)scratch_190d66) == (-1)) {
        while (mouse_buttons == 0) {
            xn_mouse_poll_clamped();
            xn_mouse_cursor_move((int)(short)mouse_x, (int)(short)mouse_y);
            xn_gfx_present_inclusive(1);
        }
        while (mouse_buttons != 0) xn_mouse_poll_clamped();
        for (answer = 0; ((int)(short)*(short *)scratch_190d64) > answer; answer++) {
            if (mouse_x > career_answer_boxes[answer + 1].x0 && mouse_x < career_answer_boxes[answer + 1].x1 && mouse_y > career_answer_boxes[answer + 1].y0 && mouse_y < career_answer_boxes[answer + 1].y1) {
                sound_play(203, player_object, 100);
                *(short *)scratch_190d66 = answer;
            }
        }
    }
}

void career_random_answer(void)
{
    *(short *)scratch_190d66 = rand_range(0, ((int)(short)*(short *)scratch_190d64) - 1);
}

void career_apply_answer(int question)
{
    int letter;
    unsigned char *text;

    scratch_190cac = question;
    letter = ((int)(short)*(short *)scratch_190d66) + 97;
    text = (unsigned char *)xn_str_find_byte_pair(*(int *)scratch_190de4, letter + 11776, 2000);
    text = (unsigned char *)memchr(text, 10, 2000);
    text++;
    while (*text == 9) {
        text = career_answer_effect(text);
    }
}

void func_000252C7(int unused)
{
}

void func_000252E2(int unused)
{
}

void career_bio_page_down(void)
{
    int i;

    if ((D_00196D68 + 21) >= career_bio_lines) return;
    for (i = 0; i < 21; i++) {
        while (*(signed char *)((char *)(career_bio_page)++) != 0);
        D_00196D68++;
    }
}

void career_bio_page_up(void)
{
    int i;

    if (career_bio_page == career_bio_text) return;
    for (i = 0; i < 21; i++) {
        career_bio_page -= 2;
        while (*(signed char *)(((char *)career_bio_page)) != 0) (career_bio_page)--;
        D_00196D68--;
    }
    if (((unsigned)career_bio_page) >= career_bio_text) return;
    career_bio_page = career_bio_text;
    D_00196D68 = 0;
}

void career_show_biography(void)
{
    int button;
    int done;

    done = 0;
    window_image = disk_read_file(D_0017077B, 0);
    career_bio_page = (career_bio_text = disk_read_file(D_0017075D, 0));
    sound_play(237, player_object, 100);
    D_00196D70 = (int)(*(char **)&career_bio_text + *(int *)disk_last_file_size);
    career_bio_lines = career_bio_count_lines();
    D_00196D68 = 0;
    while (done == 0) {
        career_bio_draw();
        if (((int)(unsigned char)(mouse_buttons & 1)) != 0 && ((int)(unsigned char)(mouse_buttons_prev & 1)) == 0) {
            for (button = 0; button < 3; button++) {
                if (mouse_x > career_bio_buttons[button].x0 && mouse_x < career_bio_buttons[button].x1 && mouse_y > career_bio_buttons[button].y0 && mouse_y < career_bio_buttons[button].y1) {
                    if (button == 2) {
                        done = 1;
                    } else {
                        career_bio_buttons[button].handler();
                    }
                }
            }
        }
    }
    if (career_bio_text != 0 && career_bio_text != (-1751672937)) {
        mc_free(career_bio_text, (int)D_00170738, 506);
        career_bio_text = -1751672937;
    }
    if (window_image == 0 || window_image == (-1751672937)) return;
    mc_free(window_image, (int)D_00170738, 507);
    window_image = -1751672937;
}

void career_bio_draw(void)
{
    int row;
    int line;

    row = 0;
    line = career_bio_page;
    mc_memcpy(screen_buffer, window_image, 64000, (int)D_00170738, 515, 4);
    while (((unsigned)line) < D_00196D70 && row < 21) {
        text_draw_coloured(line, 10, (int)(short)((row * 7) + 25), 145, 156);
        line += strlen(line) + 1;
        row++;
    }
    cursor_draw_arrow();
    mouse_buttons_prev = mouse_buttons;
    xn_mouse_poll_clamped();
    xn_gfx_present_inclusive(1);
}

int career_bio_count_lines(void)
{
    int line;
    int count;

    line = career_bio_text;
    count = 0;
    while (((unsigned)line) < D_00196D70) {
        count++;
        line += strlen(line) + 1;
    }
    return count;
}

int monster_despawn_to_marker(struct record *object)
{
    struct character *character;
    struct item *item;

    if (place_marker_in_range(object, 0) != 0) return 0;
    character = &object->data.character;
    if (character->mobile_id == 146) {
        object_free_later(object);
        return 1;
    }
    object->type = 34;
    if (((int)(unsigned short)(character->flags & 64)) != 0) {
        object->image = 25487;
    } else {
        object->image = 25488;
    }
    object->mobile_id = (unsigned short)character->mobile_id;
    object->spawn_seed = character->spawn_seed;
    character->target = 0;
    object = object->children;
    while (object != 0) {
        item = &object->data.item;
        if (object->quest_id == 0) {
            if (object->type != 2 || item->index != 18 || item->group != 3) object_free_later(object);
        }
        object = object->next;
    }
    return 1;
}
