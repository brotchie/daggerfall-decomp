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
extern short font_space_width;
extern short font_char_spacing;
extern char *D_0012DA74;
extern signed char key_down_esc;
extern short D_00142928;
extern short D_0014292C;
extern short xn_gfx_clip_left;
extern short xn_gfx_clip_top;
extern short xn_gfx_clip_right;
extern short xn_gfx_clip_bottom;
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
extern char scratch_190d64[];
extern char scratch_190d66[];
extern short scratch_190d68;
extern signed char D_001940D6;
extern signed char D_001940D8;
extern struct record *player_entity;
extern struct record *player_object;
extern char inpstr_result[];
extern struct character *player_character;
extern int window_image;
extern char scratch_buffer[];
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
extern int close();
extern int mc_free();
extern int mc_malloc();
extern int read();
extern int strlen();
extern int mc_set_location(int, int);
extern int mc_sprintf(char *, ...);
extern int mc_memcpy();
extern int xn_font_draw_string();
extern int xn_draw_image_transparent();
extern void msgbox_show_string(char *, int);
extern void book_draw_page(short);
extern void text_draw_centred(char *, int, int);
extern void text_draw_coloured(char *, int, int, int, unsigned char);
extern void text_draw_centred_coloured(char *, int, int, int, unsigned char);
extern void spfx_effect_end(struct spell *, int, struct record *);
extern void spfx_effect_tick(struct record *, struct record *, int);
extern void inpstr_begin_number(int);
void book_close(void);
void func_0005A230(void);
void book_prev_page(void);
void book_next_page(void);
void spell_tick_spell(struct record *, struct record *);
#pragma aux mc_set_location parm routine [];

void book_open(short book_id)
{
    mc_set_location(37, (int)D_001757A8);
    mc_sprintf((char *)text_buffer, (int)D_001757AF, (int)(short)book_id);
    book_file = disk_open_data((int)text_buffer);
    read((int)(short)book_file, (int)book_header, 234);
    read((int)(short)book_file, (int)&book_page_count, 2);
    book_page_offsets = mc_malloc(((int)(short)book_page_count) << 2, (int)D_001757A8, 43);
    read((int)(short)book_file, book_page_offsets, ((int)(short)book_page_count) << 2);
    window_image = disk_read_file((int)D_001757C1, 0);
    func_0005A230();
    book_page = 0;
    scratch_190d68 = 0;
    game_mode = 11;
    D_00196272 = 1;
    D_00187CA8 = 0;
    sound_play(237, (int)player_object, 100);
}

void book_update(void)
{
    short i;

    if (((int)(short)book_file) < 1) return;
    D_001940D8 |= 16;
    mc_memcpy(screen_buffer, window_image, 64000, (int)D_001757A8, 65, 4);
    book_draw_page((int)(short)book_page);
    if (scratch_190d68 != 0 && ((int)(unsigned char)game_mode) != 8) {
        scratch_190d68 = 0;
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
    *(int *)&i = 0;
    for (; ((int)(short)i) < 4; (*(int *)&i)++) {
        if (mouse_x > *(short *)(book_buttons + (((int)(short)i) * 12)) && mouse_x < *(short *)(D_00185BE8 + (((int)(short)i) * 12)) && mouse_y > *(short *)(D_00185BE6 + (((int)(short)i) * 12)) && mouse_y < *(short *)(D_00185BEA + (((int)(short)i) * 12))) {
            sound_play(203, (int)player_object, 100);
            ((int (*)())(*(int *)(D_00185BEC + (((int)(short)i) * 12))))();
        }
    }
}

void book_close(void)
{
    while (key_down_esc != 0);
    if (((int)(short)book_file) < 1) return;
    D_001940D8 &= 239;
    close((int)(short)book_file);
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
    if (*(short *)scratch_190d66 != 0) {
        text_draw_centred_coloured((char *)text_buffer, 160, (int)(short)D_0014292C, (int)(short)((int)(unsigned char)D_0012B508), 156);
    } else {
        text_draw_coloured((char *)text_buffer, (int)(short)D_00142928, (int)(short)D_0014292C, (int)(short)((int)(unsigned char)D_0012B508), 156);
    }
    *(short *)scratch_190d66 = 0;
    *(short *)scratch_190d64 = 0;
    text_buffer[0] = 0;
}

void func_0005A1C8(char *name)
{
    mc_set_location(218, (int)D_001757A8);
    mc_sprintf((char *)text_buffer, (int)D_001757CE, name);
    D_00199C2C[((int)(short)(D_00199D5E)++)] = disk_read_file((int)text_buffer, 0);
}

void func_0005A230(void)
{
    int i;

    for (i = 0; ((int)(short)D_00199D5E) > i; i++) {
        if (D_00199C2C[i] != 0 && D_00199C2C[i] != (-1751672937)) {
            mc_free(D_00199C2C[i], (int)D_001757A8, 228);
            D_00199C2C[i] = -1751672937;
        }
    }
    D_00199D5E = 0;
}

void func_0005A2BE(void)
{
    char *image;
    short i;

    *(int *)&i = 0;
    for (; (short)(short)*(int *)&i < D_00199D5E; (*(int *)&i)++) {
        image = (char *)D_00199C2C[((int)(short)i)];
        xn_draw_image_transparent((int)(unsigned short)*(short *)image, (int)(unsigned short)*(short *)(image + 2), (int)(unsigned short)*(short *)(image + 4), (int)(unsigned short)*(short *)(image + 6), image + 12);
    }
}

void book_prev_page(void)
{
    if (book_page == 0) return;
    sound_play(205, (int)player_object, 100);
    book_page--;
}

void book_next_page(void)
{
    if (((int)(short)book_page) == (((int)(short)book_page_count) - 1)) return;
    sound_play(205, (int)player_object, 100);
    book_page++;
}

void book_goto_page_prompt(void)
{
    char *prompt;

    D_0012B508 = 146;
    prompt = *(char **)scratch_buffer + 55000;
    mc_set_location(268, (int)D_001757A8);
    mc_sprintf(prompt, (int)D_001757D7, D_0017D1E6);
    *(signed char *)((char *)(strlen(prompt) + prompt) + 1) = 0;
    msgbox_show_string(prompt, 2);
    inpstr_begin_number(((int)(short)book_page) + 1);
    scratch_190d68 = 4;
}

int font_char_width(unsigned char ch)
{
    char *glyph;
    short glyph_index;

    if (((int)(unsigned char)ch) == 32) return (int)(short)font_space_width;
    *(int *)&glyph_index = ((int)(unsigned char)ch) - 33;
    glyph = D_0012DA74 + 6 + (((int)(short)glyph_index) << 2);
    return ((int)(short)*(short *)glyph) + ((int)(short)font_char_spacing);
}

void text_draw_shadow(char *text, short x, short y)
{
    unsigned char saved_colour;

    saved_colour = D_0012B508;
    D_0012B508 = text_shadow_colour;
    xn_font_draw_string((int)(short)(*(int *)&x + 1), (int)(short)(*(int *)&y + 1), text);
    D_0012B508 = saved_colour;
    xn_font_draw_string((int)(short)x, (int)(short)y, text);
}

void text_draw_centred_shadow(char *text, int x, int y)
{
    unsigned char saved_colour;

    saved_colour = D_0012B508;
    D_0012B508 = text_shadow_colour;
    text_draw_centred(text, x + 1, y + 1);
    D_0012B508 = saved_colour;
    text_draw_centred(text, x, y);
}

void mouse_set_bounds(int x_min, int y_min, int x_max, int y_max)
{
    mouse_x_min = x_min;
    mouse_x_max = x_max;
    mouse_y_min = y_min;
    mouse_y_max = y_max;
}

void clip_set_rect_size(int x, int y, int width, int height)
{
    xn_gfx_clip_left = x;
    xn_gfx_clip_top = y;
    xn_gfx_clip_right = (x + width) - 1;
    xn_gfx_clip_bottom = (y + height) - 1;
}

void clip_set_rect(int left, int top, int right, int bottom)
{
    xn_gfx_clip_left = left;
    xn_gfx_clip_top = top;
    xn_gfx_clip_right = right;
    xn_gfx_clip_bottom = bottom;
}

void spell_tick(struct record *object)
{
    struct record *target;
    struct record *next;

    if (object->children == 0) return;
    if (object == player_entity) D_001940D6 &= 239;
    target = object;
    object = object->children;
    while (object != 0) {
        next = object->next;
        switch (object->type) {
        case 9:
            spell_tick_spell(object, target);
            break;
        case 19:
            if (object->image2-- == 0) object_free_single(object);
        }
        object = next;
    }
}

void spell_tick_spell(struct record *spell_object, struct record *target)
{
    int i;
    int effect_count;
    int ended_count;
    struct spell *spell;

    spell = &spell_object->data.spell;
    i = 0;
    ended_count = i;
    effect_count = ended_count;
    for (; i < 3; i++) {
        if (spell->effects[i].type == 255) continue;
        effect_count++;
        if (((int)spell->durations[i].base) == (-1)) {
            if (player_character->equipped[spell->icon - 200] != 0 && player_character->equipped[spell->icon - 200]->data.item.enchantments[0].type != (-1)) {
                continue;
            }
            ended_count++;
            spfx_effect_end(spell, i, target);
            continue;
        }
        if (spell->cast_durations[i] != 0) spfx_effect_tick(spell_object, target, i);
        if (spell->cast_durations[i] != 0) {
            spell->cast_durations[i]--;
        } else {
            ended_count++;
            spfx_effect_end(spell, i, target);
        }
    }
    if (effect_count != ended_count) return;
    object_delete(spell_object);
}
