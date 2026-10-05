/* book.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

struct bf8_7_1 { unsigned char _:7; unsigned char f:1; };
extern char mouse_buttons[];
extern char mouse_x[];
extern char mouse_y[];
extern char mouse_x_min[];
extern char mouse_x_max[];
extern char mouse_y_min[];
extern char mouse_y_max[];
extern char text_shadow_colour[];
extern char D_0012B508[];
extern char D_0012DA40[];
extern char D_0012DA48[];
extern char D_0012DA74[];
extern char key_down_esc[];
extern char D_00142928[];
extern char D_0014292C[];
extern char D_00142940[];
extern char D_00142944[];
extern char D_00142948[];
extern char D_0014294C[];
extern char screen_buffer[];
extern char D_001757A8[];
extern char D_001757AF[];
extern char D_001757C1[];
extern char D_001757CE[];
extern char D_001757D7[];
extern char D_0017D1E6[];
extern char book_buttons[];
extern char D_00185BE6[];
extern char D_00185BE8[];
extern char D_00185BEA[];
extern char D_00185BEC[];
extern char D_00187CA8[];
extern char text_buffer[];
extern char D_00190D64[];
extern char D_00190D66[];
extern char D_00190D68[];
extern char D_001940D6[];
extern char D_001940D8[];
extern struct record *player_entity;
extern struct record *player_object;
extern char inpstr_result[];
extern struct character *player_character;
extern char window_image[];
extern char D_00195C44[];
extern char trade_mode[];
extern char D_00196272[];
extern char game_mode[];
extern char mouse_buttons_prev[];
extern char inv_right_icon[];
extern char D_00199C2C[];
extern char book_page_offsets[];
extern char book_header[];
extern char book_file[];
extern char book_page[];
extern char D_00199D5E[];
extern char book_page_count[];

extern int sound_play(int, int, int);
extern int disk_read_file(int, int);
extern int disk_open_data(int);
extern int key_pressed_once(unsigned char);
extern int object_free_single(struct record *);
extern int object_delete(struct record *);
extern int inventory_open(int, int, int);
extern int func_0009DEA7();
extern int mc_free();
extern int mc_malloc();
extern int func_000A00CB();
extern int func_000A0DF4();
extern int func_000A0ED9(int, int);
extern int mc_sprintf(int, ...);
extern int mc_memcpy();
extern int func_0012DBCC();
extern int func_00144FB4();
extern void msgbox_show_string(int, int);
extern void book_draw_page(short);
extern void text_draw_centred(int, int, int);
extern void text_draw_colored(int, int, int, int, unsigned char);
extern void text_draw_centered_colored(int, int, int, int, unsigned char);
extern void spfx_effect_end(struct spell *, int, struct record *);
extern void spfx_effect_tick(struct record *, struct record *, int);
extern void inpstr_begin_number(int);
void book_close(void);
void func_0005A230(void);
void book_prev_page(void);
void book_next_page(void);
void spell_tick_spell(struct record *, struct record *);
#pragma aux func_000A0ED9 parm routine [];

void book_open(short a1)
{
    func_000A0ED9(37, (int)D_001757A8);
    mc_sprintf((int)text_buffer, (int)D_001757AF, (int)(short)a1);
    *(short *)book_file = disk_open_data((int)text_buffer);
    func_000A00CB((int)(short)*(short *)book_file, (int)book_header, 234);
    func_000A00CB((int)(short)*(short *)book_file, (int)book_page_count, 2);
    *(int *)book_page_offsets = mc_malloc(((int)(short)*(short *)book_page_count) << 2, (int)D_001757A8, 43);
    func_000A00CB((int)(short)*(short *)book_file, *(int *)book_page_offsets, ((int)(short)*(short *)book_page_count) << 2);
    *(int *)window_image = disk_read_file((int)D_001757C1, 0);
    func_0005A230();
    *(short *)book_page = 0;
    *(short *)D_00190D68 = 0;
    *(signed char *)game_mode = 11;
    *(signed char *)D_00196272 = 1;
    *(signed char *)D_00187CA8 = 0;
    sound_play(237, (int)player_object, 100);
}

void book_update(void)
{
    short l_18;

    if (((int)(short)*(short *)book_file) < 1) return;
    *(signed char *)D_001940D8 |= 16;
    mc_memcpy(*(int *)screen_buffer, *(int *)window_image, 64000, (int)D_001757A8, 65, 4);
    book_draw_page((int)(short)*(short *)book_page);
    if (*(short *)D_00190D68 == 0) goto L59CE9;
    if (((int)(unsigned char)*(signed char *)game_mode) != 8) goto L59CEB;
L59CE9:;
    goto L59D25;
L59CEB:;
    *(short *)D_00190D68 = 0;
    if (((int)(short)*(short *)book_page_count) < *(int *)inpstr_result) goto L59D25;
    sound_play(205, (int)player_object, 100);
    *(short *)book_page = *(short *)inpstr_result - 1;
L59D25:;
    if (*(signed char *)key_down_esc == 0) goto L59D33;
    book_close();
L59D33:;
    if (key_pressed_once(73) == 0) goto L59D46;
    book_prev_page();
L59D46:;
    if (key_pressed_once(81) == 0) goto L59D59;
    book_next_page();
L59D59:;
    if (*(signed char *)mouse_buttons == 0) goto L59D76;
    if (*(signed char *)mouse_buttons == 0) goto L59D74;
    if (*(signed char *)mouse_buttons_prev != 0) goto L59D76;
L59D74:;
    goto L59D7B;
L59D76:;
    return;
L59D7B:;
    *(int *)&l_18 = 0;
L59D82:;
    if (((int)(short)l_18) < 4) goto L59D98;
    return;
L59D90:;
    (*(int *)&l_18)++;
    goto L59D82;
L59D98:;
    if (*(short *)mouse_x <= *(short *)(book_buttons + (((int)(short)l_18) * 12))) goto L59DC6;
    if (*(short *)mouse_x < *(short *)(D_00185BE8 + (((int)(short)l_18) * 12))) goto L59DC8;
L59DC6:;
    goto L59DDF;
L59DC8:;
    if (*(short *)mouse_y > *(short *)(D_00185BE6 + (((int)(short)l_18) * 12))) goto L59DE1;
L59DDF:;
    goto L59DF8;
L59DE1:;
    if (*(short *)mouse_y < *(short *)(D_00185BEA + (((int)(short)l_18) * 12))) goto L59DFA;
L59DF8:;
    goto L59E1C;
L59DFA:;
    sound_play(203, (int)player_object, 100);
    ((int (*)())(*(int *)(D_00185BEC + (((int)(short)l_18) * 12))))();
L59E1C:;
    goto L59D90;
}

void book_close(void)
{
L59E39:;
    if (*(signed char *)key_down_esc != 0) goto L59E39;
    if (((int)(short)*(short *)book_file) < 1) return;
    *(signed char *)D_001940D8 &= 239;
    func_0009DEA7((int)(short)*(short *)book_file);
    if (*(int *)window_image == 0) goto L59E7A;
    if (*(int *)window_image != (-1751672937)) goto L59E7C;
L59E7A:;
    goto L59E9A;
L59E7C:;
    mc_free(*(int *)window_image, (int)D_001757A8, 101);
    *(int *)window_image = -1751672937;
L59E9A:;
    if (*(int *)book_page_offsets == 0) goto L59EAF;
    if (*(int *)book_page_offsets != (-1751672937)) goto L59EB1;
L59EAF:;
    goto L59ECF;
L59EB1:;
    mc_free(*(int *)book_page_offsets, (int)D_001757A8, 102);
    *(int *)book_page_offsets = -1751672937;
L59ECF:;
    func_0005A230();
    *(short *)book_file = 0;
    *(signed char *)game_mode = 0;
    *(signed char *)D_00196272 = 0;
    *(signed char *)D_00187CA8 = 1;
    if (((struct bf8_7_1 *)&D_001940D8)->f == 0) return;
    *(signed char *)D_001940D8 &= 127;
    inventory_open(2, *(int *)trade_mode, (int)(unsigned char)*(signed char *)inv_right_icon);
}

void book_flush_line(void)
{
    if (*(short *)D_00190D66 == 0) goto L5A17D;
    text_draw_centered_colored((int)text_buffer, 160, (int)(short)*(short *)D_0014292C, (int)(short)((int)(unsigned char)*(signed char *)D_0012B508), 156);
    goto L5A1A5;
L5A17D:;
    text_draw_colored((int)text_buffer, (int)(short)*(short *)D_00142928, (int)(short)*(short *)D_0014292C, (int)(short)((int)(unsigned char)*(signed char *)D_0012B508), 156);
L5A1A5:;
    *(short *)D_00190D66 = 0;
    *(short *)D_00190D64 = 0;
    *(signed char *)text_buffer = 0;
}

void func_0005A1C8(int a1)
{
    func_000A0ED9(218, (int)D_001757A8);
    mc_sprintf((int)text_buffer, (int)D_001757CE, a1);
    *(int *)(D_00199C2C + (((int)(short)(*(short *)D_00199D5E)++) << 2)) = disk_read_file((int)text_buffer, 0);
}

void func_0005A230(void)
{
    int l_18;

    l_18 = 0;
L5A245:;
    if (((int)(short)*(short *)D_00199D5E) > l_18) goto L5A25B;
    goto L5A2AB;
L5A253:;
    l_18++;
    goto L5A245;
L5A25B:;
    if (*(int *)(D_00199C2C + (l_18 << 2)) == 0) goto L5A27C;
    if (*(int *)(D_00199C2C + (l_18 << 2)) != (-1751672937)) goto L5A27E;
L5A27C:;
    goto L5A2A9;
L5A27E:;
    mc_free(*(int *)(D_00199C2C + (l_18 << 2)), (int)D_001757A8, 228);
    *(int *)(D_00199C2C + (l_18 << 2)) = -1751672937;
L5A2A9:;
    goto L5A253;
L5A2AB:;
    *(short *)D_00199D5E = 0;
}

void func_0005A2BE(void)
{
    int l_1C;
    short l_18;

    *(int *)&l_18 = 0;
L5A2D3:;
    if ((short)(short)*(int *)&l_18 < *(short *)D_00199D5E) goto L5A2E9;
    return;
L5A2E1:;
    (*(int *)&l_18)++;
    goto L5A2D3;
L5A2E9:;
    l_1C = *(int *)(D_00199C2C + (((int)(short)l_18) << 2));
    func_00144FB4((int)(unsigned short)*(short *)((char *)l_1C), (int)(unsigned short)*(short *)((char *)l_1C + 2), (int)(unsigned short)*(short *)((char *)l_1C + 4), (int)(unsigned short)*(short *)((char *)l_1C + 6), l_1C + 12);
    goto L5A2E1;
}

void book_prev_page(void)
{
    if (*(short *)book_page == 0) return;
    sound_play(205, (int)player_object, 100);
    (*(short *)book_page)--;
}

void book_next_page(void)
{
    if (((int)(short)*(short *)book_page) == (((int)(short)*(short *)book_page_count) - 1)) return;
    sound_play(205, (int)player_object, 100);
    (*(short *)book_page)++;
}

void book_goto_page_prompt(void)
{
    int l_18;

    *(signed char *)D_0012B508 = 146;
    l_18 = *(int *)D_00195C44 + 55000;
    func_000A0ED9(268, (int)D_001757A8);
    mc_sprintf(l_18, (int)D_001757D7, *(int *)D_0017D1E6);
    *(signed char *)((char *)(func_000A0DF4(l_18) + l_18) + 1) = 0;
    msgbox_show_string(l_18, 2);
    inpstr_begin_number(((int)(short)*(short *)book_page) + 1);
    *(short *)D_00190D68 = 4;
}

int font_char_width(unsigned char a1)
{
    int l_24;
    short l_1C;

    if (((int)(unsigned char)a1) != 32) goto L5A469;
    return (int)(short)*(short *)D_0012DA40;
L5A469:;
    *(int *)&l_1C = ((int)(unsigned char)a1) - 33;
    l_24 = (int)(*(char **)D_0012DA74 + 6 + (((int)(short)l_1C) << 2));
    return ((int)(short)*(short *)((char *)l_24)) + ((int)(short)*(short *)D_0012DA48);
}

void text_draw_shadow(int a1, short a2, short a3)
{
    unsigned char l_10;

    l_10 = *(signed char *)D_0012B508;
    *(signed char *)D_0012B508 = *(signed char *)text_shadow_colour;
    func_0012DBCC((int)(short)(*(int *)&a2 + 1), (int)(short)(*(int *)&a3 + 1), a1);
    *(signed char *)D_0012B508 = l_10;
    func_0012DBCC((int)(short)a2, (int)(short)a3, a1);
}

void text_draw_centred_shadow(int a1, int a2, int a3)
{
    unsigned char l_10;

    l_10 = *(signed char *)D_0012B508;
    *(signed char *)D_0012B508 = *(signed char *)text_shadow_colour;
    text_draw_centred(a1, a2 + 1, a3 + 1);
    *(signed char *)D_0012B508 = l_10;
    text_draw_centred(a1, a2, a3);
}

void mouse_set_bounds(int a1, int a2, int a3, int a4)
{
    *(short *)mouse_x_min = a1;
    *(short *)mouse_x_max = a3;
    *(short *)mouse_y_min = a2;
    *(short *)mouse_y_max = a4;
}

void func_0005A6A3(int a1, int a2, int a3, int a4)
{
    *(short *)D_00142940 = a1;
    *(short *)D_00142944 = a2;
    *(short *)D_00142948 = (a1 + a3) - 1;
    *(short *)D_0014294C = (a2 + a4) - 1;
}

void func_0005A6ED(int a1, int a2, int a3, int a4)
{
    *(short *)D_00142940 = a1;
    *(short *)D_00142944 = a2;
    *(short *)D_00142948 = a3;
    *(short *)D_0014294C = a4;
}

void spell_tick(struct record *a1)
{
    struct record *l_1C;
    struct record *l_18;

    if (a1->children == 0) return;
    if (a1 != player_entity) goto L5A75F;
    *(signed char *)D_001940D6 &= 239;
L5A75F:;
    l_1C = a1;
    a1 = a1->children;
L5A76E:;
    if (a1 == 0) return;
    l_18 = a1->next;
    switch (a1->type) {
case 9:
    spell_tick_spell(a1, l_1C);
    goto L5A7C3;
case 19:
    if (a1->image2-- != 0) goto L5A7C3;
    object_free_single(a1);
default:
L5A7C3:;
    a1 = l_18;
    goto L5A76E;
}
}

void spell_tick_spell(struct record *a1, struct record *a2)
{
    int l_20;
    int l_1C;
    int l_18;
    struct spell *l_14;

    l_14 = &a1->data.spell;
    l_20 = 0;
    l_18 = l_20;
    l_1C = l_18;
L5A804:;
    if (l_20 < 3) goto L5A817;
    goto L5A901;
L5A80F:;
    l_20++;
    goto L5A804;
L5A817:;
    if (l_14->effects[l_20].type == 255) goto L5A80F;
    l_1C++;
    if (((int)(signed char)l_14->durations[l_20].base) != (-1)) goto L5A8AE;
    if (player_character->equipped[l_14->icon - 200] == 0) goto L5A88E;
    if (player_character->equipped[l_14->icon - 200]->data.item.enchantments[0].type != (-1)) goto L5A890;
L5A88E:;
    goto L5A895;
L5A890:;
    goto L5A80F;
L5A895:;
    l_18++;
    spfx_effect_end(l_14, l_20, a2);
    goto L5A80F;
L5A8AE:;
    if (l_14->cast_durations[l_20] == 0) goto L5A8CB;
    spfx_effect_tick(a1, a2, l_20);
L5A8CB:;
    if (l_14->cast_durations[l_20] == 0) goto L5A8E8;
    l_14->cast_durations[l_20]--;
    goto L5A8FC;
L5A8E8:;
    l_18++;
    spfx_effect_end(l_14, l_20, a2);
L5A8FC:;
    goto L5A80F;
L5A901:;
    if (l_1C != l_18) return;
    object_delete(a1);
}
