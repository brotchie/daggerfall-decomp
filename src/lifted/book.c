/* book.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

struct bf8_7_1 { unsigned char _:7; unsigned char f:1; };
extern signed char mouse_buttons;
extern short mouse_x;
extern short mouse_y;
extern short mouse_x_min;
extern short mouse_x_max;
extern short mouse_y_min;
extern short mouse_y_max;
extern signed char text_shadow_colour;
extern signed char D_0012B508;
extern short D_0012DA40;
extern short D_0012DA48;
extern char *D_0012DA74;
extern signed char key_down_esc;
extern short D_00142928;
extern short D_0014292C;
extern short D_00142940;
extern short D_00142944;
extern short D_00142948;
extern short D_0014294C;
extern int screen_buffer;
extern char D_001757A8[];
extern char D_001757AF[];
extern char D_001757C1[];
extern char D_001757CE[];
extern char D_001757D7[];
extern int D_0017D1E6;
extern char book_buttons[];
extern char D_00185BE6[];
extern char D_00185BE8[];
extern char D_00185BEA[];
extern char D_00185BEC[];
extern signed char D_00187CA8;
extern signed char text_buffer[];
extern char D_00190D64[];
extern char D_00190D66[];
extern short D_00190D68;
extern signed char D_001940D6;
extern signed char D_001940D8;
extern struct record *player_entity;
extern struct record *player_object;
extern char inpstr_result[];
extern struct character *player_character;
extern int window_image;
extern int D_00195C44;
extern int trade_mode;
extern signed char D_00196272;
extern signed char game_mode;
extern signed char mouse_buttons_prev;
extern signed char inv_right_icon;
extern int D_00199C2C[];
extern int book_page_offsets;
extern char book_header[];
extern short book_file;
extern short book_page;
extern short D_00199D5E;
extern short book_page_count;

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
    book_file = disk_open_data((int)text_buffer);
    func_000A00CB((int)(short)book_file, (int)book_header, 234);
    func_000A00CB((int)(short)book_file, (int)&book_page_count, 2);
    book_page_offsets = mc_malloc(((int)(short)book_page_count) << 2, (int)D_001757A8, 43);
    func_000A00CB((int)(short)book_file, book_page_offsets, ((int)(short)book_page_count) << 2);
    window_image = disk_read_file((int)D_001757C1, 0);
    func_0005A230();
    book_page = 0;
    D_00190D68 = 0;
    game_mode = 11;
    D_00196272 = 1;
    D_00187CA8 = 0;
    sound_play(237, (int)player_object, 100);
}

void book_update(void)
{
    short l_18;

    if (((int)(short)book_file) < 1) return;
    D_001940D8 |= 16;
    mc_memcpy(screen_buffer, window_image, 64000, (int)D_001757A8, 65, 4);
    book_draw_page((int)(short)book_page);
    if (D_00190D68 != 0 && ((int)(unsigned char)game_mode) != 8) {
        D_00190D68 = 0;
        if (((int)(short)book_page_count) >= *(int *)inpstr_result) {
            sound_play(205, (int)player_object, 100);
            book_page = *(short *)inpstr_result - 1;
        }
    }
    if (key_down_esc != 0) book_close();
    if (key_pressed_once(73) != 0) book_prev_page();
    if (key_pressed_once(81) != 0) book_next_page();
    if (mouse_buttons == 0 || (mouse_buttons != 0 && mouse_buttons_prev != 0)) {
        return;
    }
    *(int *)&l_18 = 0;
    for (; ((int)(short)l_18) < 4; (*(int *)&l_18)++) {
        if (mouse_x > *(short *)(book_buttons + (((int)(short)l_18) * 12)) && mouse_x < *(short *)(D_00185BE8 + (((int)(short)l_18) * 12)) && mouse_y > *(short *)(D_00185BE6 + (((int)(short)l_18) * 12)) && mouse_y < *(short *)(D_00185BEA + (((int)(short)l_18) * 12))) {
            sound_play(203, (int)player_object, 100);
            ((int (*)())(*(int *)(D_00185BEC + (((int)(short)l_18) * 12))))();
        }
    }
}

void book_close(void)
{
    do {
    } while (key_down_esc != 0);
    if (((int)(short)book_file) < 1) return;
    D_001940D8 &= 239;
    func_0009DEA7((int)(short)book_file);
    if (window_image != 0 && window_image != (-1751672937)) {
        mc_free(window_image, (int)D_001757A8, 101);
        window_image = -1751672937;
    }
    if (book_page_offsets != 0 && book_page_offsets != (-1751672937)) {
        mc_free(book_page_offsets, (int)D_001757A8, 102);
        book_page_offsets = -1751672937;
    }
    func_0005A230();
    book_file = 0;
    game_mode = 0;
    D_00196272 = 0;
    D_00187CA8 = 1;
    if (((struct bf8_7_1 *)&D_001940D8)->f == 0) return;
    D_001940D8 &= 127;
    inventory_open(2, trade_mode, (int)(unsigned char)inv_right_icon);
}

void book_flush_line(void)
{
    if (*(short *)D_00190D66 != 0) {
        text_draw_centered_colored((int)text_buffer, 160, (int)(short)D_0014292C, (int)(short)((int)(unsigned char)D_0012B508), 156);
    } else {
        text_draw_colored((int)text_buffer, (int)(short)D_00142928, (int)(short)D_0014292C, (int)(short)((int)(unsigned char)D_0012B508), 156);
    }
    *(short *)D_00190D66 = 0;
    *(short *)D_00190D64 = 0;
    text_buffer[0] = 0;
}

void func_0005A1C8(int a1)
{
    func_000A0ED9(218, (int)D_001757A8);
    mc_sprintf((int)text_buffer, (int)D_001757CE, a1);
    D_00199C2C[((int)(short)(D_00199D5E)++)] = disk_read_file((int)text_buffer, 0);
}

void func_0005A230(void)
{
    int l_18;

    for (l_18 = 0; ((int)(short)D_00199D5E) > l_18; l_18++) {
        if (D_00199C2C[l_18] != 0 && D_00199C2C[l_18] != (-1751672937)) {
            mc_free(D_00199C2C[l_18], (int)D_001757A8, 228);
            D_00199C2C[l_18] = -1751672937;
        }
    }
    D_00199D5E = 0;
}

void func_0005A2BE(void)
{
    int l_1C;
    short l_18;

    *(int *)&l_18 = 0;
    for (; (short)(short)*(int *)&l_18 < D_00199D5E; (*(int *)&l_18)++) {
        l_1C = D_00199C2C[((int)(short)l_18)];
        func_00144FB4((int)(unsigned short)*(short *)((char *)l_1C), (int)(unsigned short)*(short *)((char *)l_1C + 2), (int)(unsigned short)*(short *)((char *)l_1C + 4), (int)(unsigned short)*(short *)((char *)l_1C + 6), l_1C + 12);
    }
}

void book_prev_page(void)
{
    if (book_page == 0) return;
    sound_play(205, (int)player_object, 100);
    (book_page)--;
}

void book_next_page(void)
{
    if (((int)(short)book_page) == (((int)(short)book_page_count) - 1)) return;
    sound_play(205, (int)player_object, 100);
    (book_page)++;
}

void book_goto_page_prompt(void)
{
    int l_18;

    D_0012B508 = 146;
    l_18 = D_00195C44 + 55000;
    func_000A0ED9(268, (int)D_001757A8);
    mc_sprintf(l_18, (int)D_001757D7, D_0017D1E6);
    *(signed char *)((char *)(func_000A0DF4(l_18) + l_18) + 1) = 0;
    msgbox_show_string(l_18, 2);
    inpstr_begin_number(((int)(short)book_page) + 1);
    D_00190D68 = 4;
}

int font_char_width(unsigned char a1)
{
    int l_24;
    short l_1C;

    if (((int)(unsigned char)a1) == 32) return (int)(short)D_0012DA40;
    *(int *)&l_1C = ((int)(unsigned char)a1) - 33;
    l_24 = (int)(D_0012DA74 + 6 + (((int)(short)l_1C) << 2));
    return ((int)(short)*(short *)((char *)l_24)) + ((int)(short)D_0012DA48);
}

void text_draw_shadow(int a1, short a2, short a3)
{
    unsigned char l_10;

    l_10 = D_0012B508;
    D_0012B508 = text_shadow_colour;
    func_0012DBCC((int)(short)(*(int *)&a2 + 1), (int)(short)(*(int *)&a3 + 1), a1);
    D_0012B508 = l_10;
    func_0012DBCC((int)(short)a2, (int)(short)a3, a1);
}

void text_draw_centred_shadow(int a1, int a2, int a3)
{
    unsigned char l_10;

    l_10 = D_0012B508;
    D_0012B508 = text_shadow_colour;
    text_draw_centred(a1, a2 + 1, a3 + 1);
    D_0012B508 = l_10;
    text_draw_centred(a1, a2, a3);
}

void mouse_set_bounds(int a1, int a2, int a3, int a4)
{
    mouse_x_min = a1;
    mouse_x_max = a3;
    mouse_y_min = a2;
    mouse_y_max = a4;
}

void func_0005A6A3(int a1, int a2, int a3, int a4)
{
    D_00142940 = a1;
    D_00142944 = a2;
    D_00142948 = (a1 + a3) - 1;
    D_0014294C = (a2 + a4) - 1;
}

void func_0005A6ED(int a1, int a2, int a3, int a4)
{
    D_00142940 = a1;
    D_00142944 = a2;
    D_00142948 = a3;
    D_0014294C = a4;
}

void spell_tick(struct record *a1)
{
    struct record *l_1C;
    struct record *l_18;

    if (a1->children == 0) return;
    if (a1 == player_entity) D_001940D6 &= 239;
    l_1C = a1;
    a1 = a1->children;
    while (a1 != 0) {
        l_18 = a1->next;
        switch (a1->type) {
        case 9:
            spell_tick_spell(a1, l_1C);
            break;
        case 19:
            if (a1->image2-- == 0) object_free_single(a1);
        }
        a1 = l_18;
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
    for (; l_20 < 3; l_20++) {
        if (l_14->effects[l_20].type == 255) continue;
        l_1C++;
        if (((int)l_14->durations[l_20].base) == (-1)) {
            if (player_character->equipped[l_14->icon - 200] != 0 && player_character->equipped[l_14->icon - 200]->data.item.enchantments[0].type != (-1)) {
                continue;
            }
            l_18++;
            spfx_effect_end(l_14, l_20, a2);
            continue;
        }
        if (l_14->cast_durations[l_20] != 0) spfx_effect_tick(a1, a2, l_20);
        if (l_14->cast_durations[l_20] != 0) {
            l_14->cast_durations[l_20]--;
        } else {
            l_18++;
            spfx_effect_end(l_14, l_20, a2);
        }
    }
    if (l_1C != l_18) return;
    object_delete(a1);
}
