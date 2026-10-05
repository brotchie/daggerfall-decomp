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
extern signed char D_00147964;
extern char D_00170738[];
extern char D_00170741[];
extern char D_00170750[];
extern char D_0017075D[];
extern char D_0017077B[];
extern signed char D_00178630[];
extern char career_answer_boxes[];
extern char D_00179F76[];
extern char D_00179F78[];
extern char D_00179F7A[];
extern signed char D_00179FF8[];
extern char career_bio_buttons[];
extern char D_0017A004[];
extern char D_0017A006[];
extern char D_0017A008[];
extern char D_0017A00A[];
extern signed char text_buffer[];
extern int D_00190C74;
extern char D_00190C78[];
extern int D_00190CAC;
extern char D_00190D64[];
extern char D_00190D66[];
extern short D_00190D68;
extern short D_00190D6A;
extern char text_macro_fpc[];
extern struct record *player_object;
extern struct character *player_character;
extern int window_image;
extern char D_00195C44[];
extern signed char D_00196266;
extern signed char mouse_buttons_prev;
extern int D_00196D68;
extern int career_bio_text;
extern int D_00196D70;
extern int career_bio_lines;
extern int career_bio_page;
extern short reputation_baseline;
extern signed char career_bio_ask;

extern int career_answer_effect(int);
extern int career_nearest_class(void);
extern int func_00026081(struct record *, int);
extern int text_rsc_load(int, int, int);
extern int sound_play(int, int, int);
extern int disk_read_file(int, int);
extern int disk_write_arena2_file(int, int, int);
extern int rand_range(int, int);
extern int rand();
extern int mc_free();
extern int mc_memset();
extern int atoi();
extern int func_000A0DF4();
extern int func_000A0ED9(int, int);
extern int mc_sprintf(int, ...);
extern int mc_memcpy();
extern int memchr();
extern int func_000CDD81();
extern int func_000CE7A7();
extern int func_0012B136();
extern int func_0012B2D3();
extern void msgbox_show_rsc(int, int);
extern void text_draw_colored(int, int, int, int, unsigned char);
extern void object_free_later(struct record *);
extern void cursor_draw_arrow(void);
int career_skip_word(int);
int career_draw_lines(int, int);
int career_bio_count_lines(void);
void career_find_question(int);
void career_show_question(int);
void career_wait_answer(void);
void career_random_answer(void);
void career_apply_answer(int);
void career_bio_draw(void);
#pragma aux func_000A0ED9 parm routine [];

void career_background_summary(int a1, int a2)
{
    int l_20;
    int l_1C;
    int l_18;
    int l_14;

    career_bio_ask = *(signed char *)&a2;
    mc_memcpy((int)&reputation_baseline, (int)(signed char *)&player_character->reputation[0], 10, (int)D_00170738, 48, 10);
    D_00196266 = rand() % 6;
    D_00190D68 = (unsigned short)(unsigned char)D_00179FF8[rand_range(0, 9)];
    D_00190D6A = 1;
    D_00190C74 = rand();
    *(int *)D_00190C78 = rand();
    if (a1 == 18) a1 = career_nearest_class();
    func_000A0ED9(59, (int)D_00170738);
    mc_sprintf((int)text_buffer, (int)D_00170741, a1);
    mc_memset(*(int *)D_00195C44, 0, 64000, (int)D_00170738, 60, 4);
    disk_read_file((int)text_buffer, *(int *)D_00195C44 + 1);
    *(signed char *)(*(char **)D_00195C44) = 10;
    if (a2 != 0) disk_read_file((int)D_00170750, D_00147954);
    for (l_20 = 0; l_20 < 12; l_20++) {
        if (a2 != 0) {
            mc_memcpy(screen_buffer, D_00147954, 64000, (int)D_00170738, 69, 4);
            D_00147964 &= 254;
        }
        career_find_question(l_20 + 1);
        if (a2 != 0) {
            career_wait_answer();
        } else {
            career_random_answer();
        }
        career_apply_answer(l_20);
    }
    l_1C = text_rsc_load((int)(short)(a1 + 4116), 0, 0);
    l_18 = l_1C;
    l_14 = l_18;
    while (*(signed char *)((char *)l_18) != 0) {
        if (((int)(unsigned char)*(signed char *)((char *)l_18)) == 253 || ((int)(unsigned char)*(signed char *)((char *)l_18)) == 252) {
            *(signed char *)((char *)l_18) = 0;
        } else if (((int)(unsigned char)*(signed char *)((char *)l_18)) == 251) {
            *(signed char *)((char *)l_18) = 32;
        }
        l_18++;
    }
    disk_write_arena2_file((int)D_0017075D, l_14, (int)&*(signed char *)((char *)(l_18 - l_14) + 1));
    if (l_14 != 0 && l_14 != (-1751672937)) {
        mc_free(l_14, (int)D_00170738, 94);
        l_14 = -1751672937;
    }
    msgbox_show_rsc(35, 1);
}

int career_skip_word(int a1)
{
    while (((int)(unsigned char)*(signed char *)((char *)a1)) > 32) a1++;
    while (((int)(unsigned char)*(signed char *)((char *)a1)) <= 32) a1++;
    return a1;
}

void career_find_question(int a1)
{
    int l_1C;
    unsigned char l_18;

    l_18 = 0;
    l_1C = *(int *)D_00195C44;
    while (l_18 == 0) {
        l_1C = memchr(l_1C, 10, 2000);
        l_1C++;
        if (((int)(unsigned char)(D_00178630[(int)(unsigned char)(*(signed char *)((char *)l_1C) + 1)] & 32)) != 0 && atoi(l_1C) == a1) {
            career_show_question(l_1C);
            l_18 = 1;
        }
    }
}

void career_show_question(int a1)
{
    a1 = career_skip_word(a1);
    *(short *)D_00190D64 = 0;
    a1 = career_draw_lines(a1, 0);
    *(int *)text_macro_fpc = a1;
    while (((int)(unsigned char)(D_00178630[(int)(unsigned char)(*(signed char *)((char *)a1) + 1)] & 32)) == 0 && *(signed char *)((char *)a1) != 0) {
        (*(short *)D_00190D64)++;
        a1 = career_draw_lines(career_skip_word(a1), 1);
        while (((int)(unsigned char)(D_00178630[(int)(unsigned char)(*(signed char *)((char *)a1) + 1)] & 224)) == 0 && *(signed char *)((char *)a1) != 0) {
            a1 = memchr(a1, 10, 2000);
            a1++;
        }
    }
}

int career_draw_lines(int a1, int a2)
{
    int l_20;
    int l_1C;
    int l_18;

    l_1C = ((int)(short)*(short *)(career_answer_boxes + (((int)(short)*(short *)D_00190D64) * 12))) + 21;
    l_18 = (int)(short)*(short *)(D_00179F76 + (((int)(short)*(short *)D_00190D64) * 12));
    if (*(short *)D_00190D64 != 0) l_18 += 5;
    while (1) {
        l_20 = memchr(a1, 13, 2000);
        *(signed char *)((char *)l_20) = 0;
        if (career_bio_ask != 0) {
            text_draw_colored(a1, (int)(short)*(short *)&l_1C, (int)(short)*(short *)&l_18, 145, 141);
        }
        l_18 += 10;
        *(signed char *)((char *)l_20) = 13;
        a1 = l_20 + 2;
        if (a2 != 0 || ((int)(unsigned char)*(signed char *)((char *)a1)) != 9) return a1;
        while (((int)(unsigned char)*(signed char *)((char *)a1)) == 9) a1++;
    }
}

void career_wait_answer(void)
{
    int l_18;

    *(short *)D_00190D66 = 65535;
    while (mouse_buttons != 0) func_0012B136();
    while (((int)(short)*(short *)D_00190D66) == (-1)) {
        while (mouse_buttons == 0) {
            func_0012B136();
            func_0012B2D3((int)(short)mouse_x, (int)(short)mouse_y);
            func_000CDD81(1);
        }
        while (mouse_buttons != 0) func_0012B136();
        for (l_18 = 0; ((int)(short)*(short *)D_00190D64) > l_18; l_18++) {
            if (mouse_x > *(short *)(career_answer_boxes + ((l_18 + 1) * 12)) && mouse_x < *(short *)(D_00179F78 + ((l_18 + 1) * 12)) && mouse_y > *(short *)(D_00179F76 + ((l_18 + 1) * 12)) && mouse_y < *(short *)(D_00179F7A + ((l_18 + 1) * 12))) {
                sound_play(203, (int)player_object, 100);
                *(short *)D_00190D66 = l_18;
            }
        }
    }
}

void career_random_answer(void)
{
    *(short *)D_00190D66 = rand_range(0, ((int)(short)*(short *)D_00190D64) - 1);
}

void career_apply_answer(int a1)
{
    int l_1C;
    int l_18;

    D_00190CAC = a1;
    l_1C = ((int)(short)*(short *)D_00190D66) + 97;
    l_18 = func_000CE7A7(*(int *)text_macro_fpc, l_1C + 11776, 2000);
    l_18 = memchr(l_18, 10, 2000);
    l_18++;
    while (((int)(unsigned char)*(signed char *)((char *)l_18)) == 9) {
        l_18 = career_answer_effect(l_18);
    }
}

void func_000252C7(int a1)
{
}

void func_000252E2(int a1)
{
}

void career_bio_page_down(void)
{
    int l_18;

    if ((D_00196D68 + 21) >= career_bio_lines) return;
    for (l_18 = 0; l_18 < 21; l_18++) {
        while (*(signed char *)((char *)(career_bio_page)++) != 0);
        D_00196D68++;
    }
}

void career_bio_page_up(void)
{
    int l_18;

    if (career_bio_page == career_bio_text) return;
    for (l_18 = 0; l_18 < 21; l_18++) {
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
    int l_1C;
    int l_18;

    l_18 = 0;
    window_image = disk_read_file((int)D_0017077B, 0);
    career_bio_page = (career_bio_text = disk_read_file((int)D_0017075D, 0));
    sound_play(237, (int)player_object, 100);
    D_00196D70 = (int)(*(char **)&career_bio_text + *(int *)disk_last_file_size);
    career_bio_lines = career_bio_count_lines();
    D_00196D68 = 0;
    while (l_18 == 0) {
        career_bio_draw();
        if (((int)(unsigned char)(mouse_buttons & 1)) != 0 && ((int)(unsigned char)(mouse_buttons_prev & 1)) == 0) {
            for (l_1C = 0; l_1C < 3; l_1C++) {
                if (mouse_x > *(short *)(career_bio_buttons + (l_1C * 12)) && mouse_x < *(short *)(D_0017A006 + (l_1C * 12)) && mouse_y > *(short *)(D_0017A004 + (l_1C * 12)) && mouse_y < *(short *)(D_0017A008 + (l_1C * 12))) {
                    if (l_1C == 2) {
                        l_18 = 1;
                    } else {
                        ((int (*)())(*(int *)(D_0017A00A + (l_1C * 12))))();
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
    int l_1C;
    int l_18;

    l_1C = 0;
    l_18 = career_bio_page;
    mc_memcpy(screen_buffer, window_image, 64000, (int)D_00170738, 515, 4);
    while (((unsigned)l_18) < D_00196D70 && l_1C < 21) {
        text_draw_colored(l_18, 10, (int)(short)((l_1C * 7) + 25), 145, 156);
        l_18 += func_000A0DF4(l_18) + 1;
        l_1C++;
    }
    cursor_draw_arrow();
    mouse_buttons_prev = mouse_buttons;
    func_0012B136();
    func_000CDD81(1);
}

int career_bio_count_lines(void)
{
    int l_20;
    int l_1C;

    l_20 = career_bio_text;
    l_1C = 0;
    while (((unsigned)l_20) < D_00196D70) {
        l_1C++;
        l_20 += func_000A0DF4(l_20) + 1;
    }
    return l_1C;
}

int func_0002586B(struct record *a1)
{
    struct character *l_20;
    struct item *l_1C;

    if (func_00026081(a1, 0) != 0) return 0;
    l_20 = &a1->data.character;
    if (l_20->mobile_id == 146) {
        object_free_later(a1);
        return 1;
    }
    a1->type = 34;
    if (((int)(unsigned short)(l_20->flags & 64)) != 0) {
        a1->image = 25487;
    } else {
        a1->image = 25488;
    }
    a1->mobile_id = (unsigned short)l_20->mobile_id;
    a1->spawn_seed = l_20->spawn_seed;
    l_20->target = 0;
    a1 = a1->children;
    while (a1 != 0) {
        l_1C = &a1->data.item;
        if (a1->quest_id == 0) {
            if (a1->type != 2 || l_1C->index != 18 || l_1C->group != 3) object_free_later(a1);
        }
        a1 = a1->next;
    }
    return 1;
}
