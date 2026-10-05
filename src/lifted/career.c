/* career.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern char disk_last_file_size[];
extern char mouse_buttons[];
extern char mouse_x[];
extern char mouse_y[];
extern char screen_buffer[];
extern char D_00147954[];
extern char D_00147964[];
extern char D_00170738[];
extern char D_00170741[];
extern char D_00170750[];
extern char D_0017075D[];
extern char D_0017077B[];
extern char D_00178630[];
extern char career_answer_boxes[];
extern char D_00179F76[];
extern char D_00179F78[];
extern char D_00179F7A[];
extern char D_00179FF8[];
extern char career_bio_buttons[];
extern char D_0017A004[];
extern char D_0017A006[];
extern char D_0017A008[];
extern char D_0017A00A[];
extern char text_buffer[];
extern char D_00190C74[];
extern char D_00190C78[];
extern char D_00190CAC[];
extern char D_00190D64[];
extern char D_00190D66[];
extern char D_00190D68[];
extern char D_00190D6A[];
extern char text_macro_fpc[];
extern struct record *player_object;
extern struct character *player_character;
extern char window_image[];
extern char D_00195C44[];
extern char D_00196266[];
extern char mouse_buttons_prev[];
extern char D_00196D68[];
extern char career_bio_text[];
extern char D_00196D70[];
extern char career_bio_lines[];
extern char career_bio_page[];
extern char reputation_baseline[];
extern char career_bio_ask[];

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

    *(signed char *)career_bio_ask = *(signed char *)&a2;
    mc_memcpy((int)reputation_baseline, (int)(signed char *)&player_character->reputation[0], 10, (int)D_00170738, 48, 10);
    *(signed char *)D_00196266 = rand() % 6;
    *(short *)D_00190D68 = (unsigned short)(unsigned char)*(signed char *)(D_00179FF8 + rand_range(0, 9));
    *(short *)D_00190D6A = 1;
    *(int *)D_00190C74 = rand();
    *(int *)D_00190C78 = rand();
    if (a1 != 18) goto L2468B;
    a1 = career_nearest_class();
L2468B:;
    func_000A0ED9(59, (int)D_00170738);
    mc_sprintf((int)text_buffer, (int)D_00170741, a1);
    mc_memset(*(int *)D_00195C44, 0, 64000, (int)D_00170738, 60, 4);
    disk_read_file((int)text_buffer, *(int *)D_00195C44 + 1);
    *(signed char *)(*(char **)D_00195C44) = 10;
    if (a2 == 0) goto L246F9;
    disk_read_file((int)D_00170750, *(int *)D_00147954);
L246F9:;
    l_20 = 0;
L24700:;
    if (l_20 < 12) goto L24710;
    goto L24760;
L24708:;
    l_20++;
    goto L24700;
L24710:;
    if (a2 == 0) goto L2473B;
    mc_memcpy(*(int *)screen_buffer, *(int *)D_00147954, 64000, (int)D_00170738, 69, 4);
    *(signed char *)D_00147964 &= 254;
L2473B:;
    career_find_question(l_20 + 1);
    if (a2 == 0) goto L24751;
    career_wait_answer();
    goto L24756;
L24751:;
    career_random_answer();
L24756:;
    career_apply_answer(l_20);
    goto L24708;
L24760:;
    l_1C = text_rsc_load((int)(short)(a1 + 4116), 0, 0);
    l_18 = l_1C;
    l_14 = l_18;
L24781:;
    if (*(signed char *)((char *)l_18) == 0) goto L247D2;
    if (((int)(unsigned char)*(signed char *)((char *)l_18)) == 253) goto L247AB;
    if (((int)(unsigned char)*(signed char *)((char *)l_18)) != 252) goto L247B3;
L247AB:;
    *(signed char *)((char *)l_18) = 0;
    goto L247CA;
L247B3:;
    if (((int)(unsigned char)*(signed char *)((char *)l_18)) != 251) goto L247CA;
    *(signed char *)((char *)l_18) = 32;
L247CA:;
    l_18++;
    goto L24781;
L247D2:;
    disk_write_arena2_file((int)D_0017075D, l_14, (int)&*(signed char *)((char *)(l_18 - l_14) + 1));
    if (l_14 == 0) goto L247F7;
    if (l_14 != (-1751672937)) goto L247F9;
L247F7:;
    goto L24812;
L247F9:;
    mc_free(l_14, (int)D_00170738, 94);
    l_14 = -1751672937;
L24812:;
    msgbox_show_rsc(35, 1);
}

int career_skip_word(int a1)
{
L2483B:;
    if (((int)(unsigned char)*(signed char *)((char *)a1)) <= 32) goto L24852;
    a1++;
    goto L2483B;
L24852:;
    if (((int)(unsigned char)*(signed char *)((char *)a1)) > 32) goto L24869;
    a1++;
    goto L24852;
L24869:;
    return a1;
}

void career_find_question(int a1)
{
    int l_1C;
    unsigned char l_18;

    l_18 = 0;
    l_1C = *(int *)D_00195C44;
L24899:;
    if (l_18 != 0) return;
    l_1C = memchr(l_1C, 10, 2000);
    l_1C++;
    if (((int)(unsigned char)(*(signed char *)(D_00178630 + ((int)(unsigned char)(*(signed char *)((char *)l_1C) + 1))) & 32)) == 0) goto L248E4;
    if (atoi(l_1C) == a1) goto L248E6;
L248E4:;
    goto L248F2;
L248E6:;
    career_show_question(l_1C);
    l_18 = 1;
L248F2:;
    goto L24899;
}

void career_show_question(int a1)
{
    a1 = career_skip_word(a1);
    *(short *)D_00190D64 = 0;
    a1 = career_draw_lines(a1, 0);
    *(int *)text_macro_fpc = a1;
L24938:;
    if (((int)(unsigned char)(*(signed char *)(D_00178630 + ((int)(unsigned char)(*(signed char *)((char *)a1) + 1))) & 32)) != 0) goto L2495D;
    if (*(signed char *)((char *)a1) != 0) goto L2495F;
L2495D:;
    return;
L2495F:;
    (*(short *)D_00190D64)++;
    a1 = career_draw_lines(career_skip_word(a1), 1);
L2497B:;
    if (((int)(unsigned char)(*(signed char *)(D_00178630 + ((int)(unsigned char)(*(signed char *)((char *)a1) + 1))) & 224)) != 0) goto L249A0;
    if (*(signed char *)((char *)a1) != 0) goto L249A2;
L249A0:;
    goto L249BF;
L249A2:;
    a1 = memchr(a1, 10, 2000);
    a1++;
    goto L2497B;
L249BF:;
    goto L24938;
}

int career_draw_lines(int a1, int a2)
{
    int l_20;
    int l_1C;
    int l_18;

    l_1C = ((int)(short)*(short *)(career_answer_boxes + (((int)(short)*(short *)D_00190D64) * 12))) + 21;
    l_18 = (int)(short)*(short *)(D_00179F76 + (((int)(short)*(short *)D_00190D64) * 12));
    if (*(short *)D_00190D64 == 0) goto L24A1A;
    l_18 += 5;
L24A1A:;
    l_20 = memchr(a1, 13, 2000);
    *(signed char *)((char *)l_20) = 0;
    if (*(signed char *)career_bio_ask == 0) goto L24A59;
    text_draw_colored(a1, (int)(short)*(short *)&l_1C, (int)(short)*(short *)&l_18, 145, 141);
L24A59:;
    l_18 += 10;
    *(signed char *)((char *)l_20) = 13;
    a1 = l_20 + 2;
    if (a2 != 0) goto L24A81;
    if (((int)(unsigned char)*(signed char *)((char *)a1)) == 9) goto L24A89;
L24A81:;
    return a1;
L24A89:;
    if (((int)(unsigned char)*(signed char *)((char *)a1)) != 9) goto L24AA0;
    a1++;
    goto L24A89;
L24AA0:;
    goto L24A1A;
}

void career_wait_answer(void)
{
    int l_18;

    *(short *)D_00190D66 = 65535;
L24AC8:;
    if (*(signed char *)mouse_buttons == 0) goto L24AD8;
    func_0012B136();
    goto L24AC8;
L24AD8:;
    if (((int)(short)*(short *)D_00190D66) != (-1)) return;
L24AE8:;
    if (*(signed char *)mouse_buttons != 0) goto L24B15;
    func_0012B136();
    func_0012B2D3((int)(short)*(short *)mouse_x, (int)(short)*(short *)mouse_y);
    func_000CDD81(1);
    goto L24AE8;
L24B15:;
    if (*(signed char *)mouse_buttons == 0) goto L24B25;
    func_0012B136();
    goto L24B15;
L24B25:;
    l_18 = 0;
L24B2C:;
    if (((int)(short)*(short *)D_00190D64) > l_18) goto L24B45;
    goto L24BCA;
L24B3D:;
    l_18++;
    goto L24B2C;
L24B45:;
    if (*(short *)mouse_x <= *(short *)(career_answer_boxes + ((l_18 + 1) * 12))) goto L24B73;
    if (*(short *)mouse_x < *(short *)(D_00179F78 + ((l_18 + 1) * 12))) goto L24B75;
L24B73:;
    goto L24B8C;
L24B75:;
    if (*(short *)mouse_y > *(short *)(D_00179F76 + ((l_18 + 1) * 12))) goto L24B8E;
L24B8C:;
    goto L24BA5;
L24B8E:;
    if (*(short *)mouse_y < *(short *)(D_00179F7A + ((l_18 + 1) * 12))) goto L24BA7;
L24BA5:;
    goto L24BC5;
L24BA7:;
    sound_play(203, (int)player_object, 100);
    *(short *)D_00190D66 = l_18;
L24BC5:;
    goto L24B3D;
L24BCA:;
    goto L24AD8;
}

void career_random_answer(void)
{
    *(short *)D_00190D66 = rand_range(0, ((int)(short)*(short *)D_00190D64) - 1);
}

void career_apply_answer(int a1)
{
    int l_1C;
    int l_18;

    *(int *)D_00190CAC = a1;
    l_1C = ((int)(short)*(short *)D_00190D66) + 97;
    l_18 = func_000CE7A7(*(int *)text_macro_fpc, l_1C + 11776, 2000);
    l_18 = memchr(l_18, 10, 2000);
    l_18++;
L24C62:;
    if (((int)(unsigned char)*(signed char *)((char *)l_18)) != 9) return;
    l_18 = career_answer_effect(l_18);
    goto L24C62;
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

    if ((*(int *)D_00196D68 + 21) >= *(int *)career_bio_lines) return;
    l_18 = 0;
L25507:;
    if (l_18 < 21) goto L25517;
    return;
L2550F:;
    l_18++;
    goto L25507;
L25517:;
    if (*(signed char *)((char *)(*(int *)career_bio_page)++) != 0) goto L25517;
    (*(int *)D_00196D68)++;
    goto L2550F;
}

void career_bio_page_up(void)
{
    int l_18;

    if (*(int *)career_bio_page == *(int *)career_bio_text) return;
    l_18 = 0;
L2555B:;
    if (l_18 < 21) goto L2556B;
    goto L2558C;
L25563:;
    l_18++;
    goto L2555B;
L2556B:;
    *(int *)career_bio_page -= 2;
L25572:;
    if (*(signed char *)(*(char **)career_bio_page) == 0) goto L25584;
    (*(int *)career_bio_page)--;
    goto L25572;
L25584:;
    (*(int *)D_00196D68)--;
    goto L25563;
L2558C:;
    if (((unsigned)*(int *)career_bio_page) >= *(int *)career_bio_text) return;
    *(int *)career_bio_page = *(int *)career_bio_text;
    *(int *)D_00196D68 = 0;
}

void career_show_biography(void)
{
    int l_1C;
    int l_18;

    l_18 = 0;
    *(int *)window_image = disk_read_file((int)D_0017077B, 0);
    *(int *)career_bio_page = (*(int *)career_bio_text = disk_read_file((int)D_0017075D, 0));
    sound_play(237, (int)player_object, 100);
    *(int *)D_00196D70 = (int)(*(char **)career_bio_text + *(int *)disk_last_file_size);
    *(int *)career_bio_lines = career_bio_count_lines();
    *(int *)D_00196D68 = 0;
L25633:;
    if (l_18 != 0) goto L256F7;
    career_bio_draw();
    if (((int)(unsigned char)(*(signed char *)mouse_buttons & 1)) == 0) goto L25662;
    if (((int)(unsigned char)(*(signed char *)mouse_buttons_prev & 1)) == 0) goto L25667;
L25662:;
    goto L256F2;
L25667:;
    l_1C = 0;
L2566E:;
    if (l_1C < 3) goto L25681;
    goto L256F2;
L25679:;
    l_1C++;
    goto L2566E;
L25681:;
    if (*(short *)mouse_x <= *(short *)(career_bio_buttons + (l_1C * 12))) goto L256A9;
    if (*(short *)mouse_x < *(short *)(D_0017A006 + (l_1C * 12))) goto L256AB;
L256A9:;
    goto L256BF;
L256AB:;
    if (*(short *)mouse_y > *(short *)(D_0017A004 + (l_1C * 12))) goto L256C1;
L256BF:;
    goto L256D5;
L256C1:;
    if (*(short *)mouse_y < *(short *)(D_0017A008 + (l_1C * 12))) goto L256D7;
L256D5:;
    goto L256F0;
L256D7:;
    if (l_1C != 2) goto L256E6;
    l_18 = 1;
    goto L256F0;
L256E6:;
    ((int (*)())(*(int *)(D_0017A00A + (l_1C * 12))))();
L256F0:;
    goto L25679;
L256F2:;
    goto L25633;
L256F7:;
    if (*(int *)career_bio_text == 0) goto L2570C;
    if (*(int *)career_bio_text != (-1751672937)) goto L2570E;
L2570C:;
    goto L2572C;
L2570E:;
    mc_free(*(int *)career_bio_text, (int)D_00170738, 506);
    *(int *)career_bio_text = -1751672937;
L2572C:;
    if (*(int *)window_image == 0) goto L25741;
    if (*(int *)window_image != (-1751672937)) goto L25743;
L25741:;
    return;
L25743:;
    mc_free(*(int *)window_image, (int)D_00170738, 507);
    *(int *)window_image = -1751672937;
}

void career_bio_draw(void)
{
    int l_1C;
    int l_18;

    l_1C = 0;
    l_18 = *(int *)career_bio_page;
    mc_memcpy(*(int *)screen_buffer, *(int *)window_image, 64000, (int)D_00170738, 515, 4);
L257A9:;
    if (((unsigned)l_18) >= *(int *)D_00196D70) goto L257BA;
    if (l_1C < 21) goto L257BC;
L257BA:;
    goto L257F4;
L257BC:;
    text_draw_colored(l_18, 10, (int)(short)((l_1C * 7) + 25), 145, 156);
    l_18 += func_000A0DF4(l_18) + 1;
    l_1C++;
    goto L257A9;
L257F4:;
    cursor_draw_arrow();
    *(signed char *)mouse_buttons_prev = *(signed char *)mouse_buttons;
    func_0012B136();
    func_000CDD81(1);
}

int career_bio_count_lines(void)
{
    int l_20;
    int l_1C;

    l_20 = *(int *)career_bio_text;
    l_1C = 0;
L25839:;
    if (((unsigned)l_20) >= *(int *)D_00196D70) goto L25858;
    l_1C++;
    l_20 += func_000A0DF4(l_20) + 1;
    goto L25839;
L25858:;
    return l_1C;
}

int func_0002586B(struct record *a1)
{
    struct character *l_20;
    struct item *l_1C;

    if (func_00026081(a1, 0) == 0) goto L25896;
    return 0;
L25896:;
    l_20 = &a1->data.character;
    if (l_20->mobile_id != 146) goto L258C8;
    object_free_later(a1);
    return 1;
L258C8:;
    a1->type = 34;
    if (((int)(unsigned short)(l_20->flags & 64)) == 0) goto L258EE;
    a1->image = 25487;
    goto L258F7;
L258EE:;
    a1->image = 25488;
L258F7:;
    a1->mobile_id = (unsigned short)l_20->mobile_id;
    a1->spawn_seed = l_20->spawn_seed;
    l_20->target = 0;
    a1 = a1->children;
L2592D:;
    if (a1 == 0) goto L2598B;
    l_1C = &a1->data.item;
    if (a1->quest_id != 0) goto L25980;
    if (a1->type != 2) goto L25965;
    if (l_1C->index == 18) goto L25967;
L25965:;
    goto L25978;
L25967:;
    if (l_1C->group == 3) goto L25980;
L25978:;
    object_free_later(a1);
L25980:;
    a1 = a1->next;
    goto L2592D;
L2598B:;
    return 1;
}
