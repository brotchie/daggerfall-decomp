/* note.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

struct bf8_4_1 { unsigned char _:4; unsigned char f:1; };
struct bf8_5_1 { unsigned char _:5; unsigned char f:1; };
extern short xn_cam_centre_x;
extern short xn_cam_centre_y;
extern short mouse_x;
extern short mouse_y;
extern signed char D_0012B508;
extern short font_height;
extern short D_00142928;
extern short D_0014292C;
extern char D_00174FAC[];
extern char D_00174FF5[];
extern char D_00175010[];
extern int D_0017D1E6;
extern int D_0017D1F2;
extern char note_colour[];
extern signed char D_00185201[];
extern signed char text_rsc_buffer[];
extern signed char D_001940D5;
extern signed char D_001940D8;
extern char frame_counter[];
extern struct record *player_object;
extern int window_image;
extern char scratch_buffer[];
extern short text_cursor_x;
extern unsigned short text_cursor_y;
extern signed char D_00196272;
extern signed char game_mode;
extern short D_001997B0;
extern short D_001997B2;
extern int note_file_size;
extern int note_rci;
extern int note_search_text;
extern char note_selected[];
extern int D_001997CC;
extern int note_search_from;
extern char note_page[];
extern int note_page_backup;
extern char note_font[];
extern char note_text_flags[];
extern short note_cursor_y;
extern short note_page_index;
extern short note_page_free;
extern short note_cursor_x;
extern short note_file;
extern signed char note_tool;
extern signed char D_001997EB;
extern signed char note_action;
extern signed char note_silent;

extern int sheet_open(int);
extern int note_text_hit_cb(int);
extern int note_page_walk(int, int, int);
extern int font_text_width(int);
extern int sound_play(int, int, int);
extern int close();
extern int mc_free();
extern int mc_memset();
extern int lseek();
extern int read();
extern int mc_strncpy();
extern int write();
extern int strlen();
extern int mc_set_location(int, int);
extern int mc_sprintf(int, ...);
extern int mc_memcpy();
extern int filelength();
extern int func_000A138E();
extern int strstr();
extern int xn_mouse_set_position();
extern int xn_font_select();
extern int xn_draw_line_to();
extern void msgbox_show_string(int, int);
extern void msgbox_show_rsc(int, int);
extern void note_add_line(short, short, short, short);
extern void text_draw(int, int, int);
extern void text_draw_centred(int, int, int);
extern void text_draw_coloured(int, int, int, int, unsigned char);
extern void text_draw_centred_coloured(int, int, int, int, unsigned char);
extern void inpstr_begin_number(int);
extern void inpstr_begin_text(int, short);
int note_find_match_cb(int);
int note_select_text_cb(int);
int note_select_line_cb(int);
int rect_overlap(int, int);
int note_keep_text_cb(int);
int note_keep_line_cb(int);
void note_save_page(void);
void note_reload_page(void);
void note_load_page(void);
void note_find(void);
void note_text_box(int, int);
void note_delete_selected(void);
#pragma aux mc_set_location parm routine [];

int note_close(void)
{
    note_save_page();
    if (*(int *)note_page != 0 && *(int *)note_page != (-1751672937)) {
        mc_free(*(int *)note_page, (int)D_00174FAC, 206);
        *(int *)note_page = -1751672937;
    }
    if (note_rci != 0 && note_rci != (-1751672937)) {
        mc_free(note_rci, (int)D_00174FAC, 207);
        note_rci = -1751672937;
    }
    if (note_page_backup != 0 && note_page_backup != (-1751672937)) {
        mc_free(note_page_backup, (int)D_00174FAC, 208);
        note_page_backup = -1751672937;
    }
    if (note_silent == 0) {
        if (window_image != 0 && window_image != (-1751672937)) {
            mc_free(window_image, (int)D_00174FAC, 212);
            window_image = -1751672937;
        }
        game_mode = 0;
        D_00196272 = 0;
    }
    note_silent = 0;
    D_001940D5 &= 247;
    close((int)(short)note_file);
    if (((struct bf8_5_1 *)&D_001940D8)->f != 0) {
        D_001940D8 &= 223;
        sheet_open(1);
    }
    return 1;
}

void note_click_page(void)
{
    int l_18;

    if (note_tool == 0 && note_action == 0) {
        D_001940D5 |= 4;
        l_18 = note_page_walk(*(int *)note_page, (int)note_text_hit_cb, 0);
        if (l_18 != 0) {
            if (((int)(unsigned char)(*(signed char *)((char *)l_18 + 6) & 1)) != 0) {
                text_cursor_x = (note_cursor_x = 160 - (font_text_width(l_18 + 11) >> 1));
                text_cursor_y = (note_cursor_y = *(short *)((char *)l_18 + 3));
            } else {
                text_cursor_x = (note_cursor_x = *(short *)((char *)l_18 + 1));
                text_cursor_y = (note_cursor_y = *(short *)((char *)l_18 + 3));
            }
            note_action = 1;
            xn_font_select((int)(short)((unsigned short)(unsigned char)D_00185201[(int)(unsigned char)*(signed char *)((char *)l_18 + 5)]));
            inpstr_begin_text(l_18 + 11, 79);
            *(signed char *)note_text_flags |= 32;
            *(int *)note_selected = l_18;
        } else {
            text_cursor_x = (note_cursor_x = mouse_x);
            text_cursor_y = (note_cursor_y = mouse_y);
            note_action = 1;
            mc_memset((int)text_rsc_buffer, 0, 81, (int)D_00174FAC, 270, 2048);
            xn_font_select((int)(short)((unsigned short)(unsigned char)D_00185201[(int)(short)*(short *)note_font]));
            inpstr_begin_text((int)text_rsc_buffer, 79);
        }
        return;
    }
    if (((int)(unsigned char)note_tool) == 2 && ((int)(unsigned char)note_action) != 2) {
        note_cursor_x = mouse_x;
        note_cursor_y = mouse_y;
        note_action = 2;
        return;
    }
    if (((int)(unsigned char)note_tool) == 2 && ((int)(unsigned char)note_action) == 2) {
        note_action = 0;
        note_add_line((int)(short)note_cursor_x, (int)(short)note_cursor_y, (int)(short)mouse_x, (int)(short)mouse_y);
        return;
    }
    if (((int)(unsigned char)note_tool) != 3 || note_action != 0) {
        return;
    }
    note_action = 3;
    D_001997B0 = mouse_x;
    D_001997B2 = mouse_y;
}

void note_draw_page(void)
{
    int l_18;

    l_18 = *(int *)note_page;
    while (*(signed char *)((char *)l_18) != 0) {
        switch (*(unsigned char *)((char *)l_18)) {
        case 1:
            if (note_silent == 0 && (((int)(short)(*(short *)note_text_flags & 32)) == 0 || l_18 != *(int *)note_selected)) {
                xn_font_select((int)(short)((unsigned short)(unsigned char)D_00185201[(int)(unsigned char)*(signed char *)((char *)l_18 + 5)]));
                if (((int)(unsigned char)(*(signed char *)((char *)l_18 + 6) & 64)) != 0) {
                    D_0012B508 = (*(signed char *)frame_counter & 15) + 240;
                } else {
                    D_0012B508 = *(signed char *)((char *)l_18 + 7);
                }
                if (((int)(unsigned char)(*(signed char *)((char *)l_18 + 6) & 2)) != 0) {
                    if (((int)(unsigned char)(*(signed char *)((char *)l_18 + 6) & 1)) != 0) {
                        text_draw_centred_coloured(l_18 + 11, 160, (int)(short)*(short *)((char *)l_18 + 3), (int)(short)((int)(unsigned char)D_0012B508), 156);
                    } else {
                        text_draw_coloured(l_18 + 11, (int)(short)*(short *)((char *)l_18 + 1), (int)(short)*(short *)((char *)l_18 + 3), (int)(short)((int)(unsigned char)D_0012B508), 156);
                    }
                } else if (((int)(unsigned char)(*(signed char *)((char *)l_18 + 6) & 1)) != 0) {
                    text_draw_centred(l_18 + 11, 160, (int)(short)*(short *)((char *)l_18 + 3));
                } else {
                    text_draw(l_18 + 11, (int)(short)*(short *)((char *)l_18 + 1), (int)(short)*(short *)((char *)l_18 + 3));
                }
            }
            l_18 += 91;
            break;
        case 2:
            if (note_silent != 0) {
                (*(char (**)[11])&l_18)++;
            } else {
                if (((int)(unsigned char)(*(signed char *)((char *)l_18 + 10) & 64)) != 0) {
                    D_0012B508 = (*(signed char *)frame_counter & 15) + 240;
                } else {
                    D_0012B508 = *(signed char *)((char *)l_18 + 9);
                }
                D_00142928 = *(short *)((char *)l_18 + 1);
                D_0014292C = *(short *)((char *)l_18 + 3);
                xn_draw_line_to((int)(short)*(short *)((char *)l_18 + 5), (int)(short)*(short *)((char *)l_18 + 7));
                (*(char (**)[11])&l_18)++;
            }
        }
    }
    note_page_free = 3640 - (l_18 - *(short *)note_page);
}

void note_add_text(int a1)
{
    int l_18;

    if (((unsigned)((int)(short)note_page_free)) < 91) {
        msgbox_show_rsc(1700, 1);
        return;
    }
    mc_memcpy(note_page_backup, *(int *)note_page, 3640, (int)D_00174FAC, 361, 4);
    D_001940D5 |= 16;
    l_18 = *(int *)note_page;
    while (*(signed char *)((char *)l_18) != 0) {
        if (((int)(unsigned char)*(signed char *)((char *)l_18)) == 1) {
            l_18 += 91;
        } else {
            (*(char (**)[11])&l_18)++;
        }
    }
    *(signed char *)((char *)l_18) = 1;
    *(short *)((char *)l_18 + 1) = note_cursor_x;
    *(short *)((char *)l_18 + 3) = note_cursor_y;
    *(signed char *)((char *)l_18 + 7) = *(signed char *)note_colour;
    *(signed char *)((char *)l_18 + 5) = *(signed char *)note_font;
    *(signed char *)((char *)l_18 + 6) = *(signed char *)note_text_flags;
    mc_strncpy(l_18 + 11, a1, 4, (int)D_00174FAC, 381);
    *(int *)note_selected = l_18;
    l_18 += 91;
    *(signed char *)((char *)l_18) = 0;
}

void note_save_page(void)
{
    if (((struct bf8_4_1 *)&D_001940D5)->f == 0) return;
    D_001940D5 &= 239;
    lseek((int)(short)note_file, ((int)(short)note_page_index) * 3640, 0);
    write((int)(short)note_file, *(int *)note_page, 3640);
    note_file_size = filelength((int)(short)note_file);
}

void note_reload_page(void)
{
    note_load_page();
    *(int *)note_selected = (note_search_from = 0);
}

void note_load_page(void)
{
    lseek((int)(short)note_file, ((int)(short)note_page_index) * 3640, 0);
    read((int)(short)note_file, *(int *)note_page, 3640);
    mc_memcpy(note_page_backup, *(int *)note_page, 3640, (int)D_00174FAC, 450, 4);
}

void note_goto_page_prompt(void)
{
    int l_18;

    D_0012B508 = 146;
    l_18 = *(int *)scratch_buffer + 55000;
    mc_set_location(459, (int)D_00174FAC);
    mc_sprintf(l_18, (int)D_00174FF5, D_0017D1E6);
    *(signed char *)((char *)(strlen(l_18) + l_18) + 1) = 0;
    msgbox_show_string(l_18, 2);
    inpstr_begin_number(((int)(short)note_page_index) + 1);
    note_action = 4;
}

void note_goto_page(int a1)
{
{
    int l_20;
    int l_1C;

    l_1C = ((int)(short)*(short *)&a1) * 3640;
    if (((unsigned)l_1C) > note_file_size) return;
    note_save_page();
    note_page_index = a1;
    note_load_page();
    if (note_silent != 0) return;
    sound_play(205, (int)player_object, 100);
}
}

void note_prev_page(void)
{
    if (note_page_index == 0) return;
    note_save_page();
    note_page_index--;
    note_reload_page();
    if (note_silent != 0) return;
    sound_play(205, (int)player_object, 100);
}

void note_next_page(void)
{
    short l_18;

    *(int *)&l_18 = ((unsigned)lseek((int)(short)note_file, 0, 2)) / 3640;
    note_save_page();
    if (((int)(short)note_page_index) < (((int)(short)l_18) - 1)) {
        note_page_index++;
        note_reload_page();
        if (note_silent == 0) sound_play(205, (int)player_object, 100);
        return;
    }
    if (*(signed char *)(*(char **)note_page) == 0) return;
    lseek((int)(short)note_file, 0, 2);
    mc_memset(*(int *)note_page, 0, 3640, (int)D_00174FAC, 507, 4);
    write((int)(short)note_file, *(int *)note_page, 3640);
    note_file_size = filelength((int)(short)note_file);
    note_page_index++;
    if (note_silent != 0) return;
    sound_play(205, (int)player_object, 100);
}

void note_cycle_tool(void)
{
    note_tool++;
    note_tool &= 3;
}

void note_pick_colour(void)
{
    *(short *)note_colour = (unsigned short)(unsigned char)func_000A138E((int)(short)(mouse_x - xn_cam_centre_x), (int)(short)((mouse_y - 1) - xn_cam_centre_y));
    D_001997EB = (((int)(short)mouse_x) - 52) >> 3;
}

void note_cycle_font(void)
{
    (*(short *)note_font)++;
    *(short *)note_font &= 3;
}

void note_find_prompt(void)
{
    int l_18;

    xn_font_select(4);
    note_search_text = *(int *)scratch_buffer + 56000;
    l_18 = *(int *)scratch_buffer + 55000;
    mc_set_location(540, (int)D_00174FAC);
    mc_sprintf(l_18, (int)D_00175010, D_0017D1F2);
    *(signed char *)((char *)(strlen(l_18) + l_18) + 1) = 0;
    msgbox_show_string(l_18, 2);
    mc_memset(note_search_text, 0, 24, (int)D_00174FAC, 543, 4);
    inpstr_begin_text(note_search_text, 23);
    *(signed char *)note_text_flags |= 128;
    *(int *)note_selected = *(int *)note_page;
}

int note_find_match_cb(int a1)
{
    if (strstr(a1 + 11, note_search_text) != 0) {
        *(int *)note_selected = a1;
        return 1;
    }
    return 0;
}

void note_find(void)
{
    int l_20;
    short l_1C;
    short l_18;

    if (((int)(unsigned char)game_mode) != 9 || ((int)(short)(*(short *)note_text_flags & 128)) == 0 || *(signed char *)(((char *)note_search_text)) == 0) {
        return;
    }
    *(short *)note_text_flags &= 127;
    l_18 = note_page_index;
    note_save_page();
    *(int *)&l_1C = ((unsigned)lseek((int)(short)note_file, 0, 2)) / 3640;
    if (note_search_from != 0) {
        *(int *)note_selected = note_search_from;
    } else {
        *(int *)note_selected = *(int *)note_page;
    }
    while (note_page_index < l_1C) {
        note_load_page();
        if (note_page_walk(*(int *)note_selected, (int)note_find_match_cb, 0) != 0) goto L4E6F9;
        note_page_index++;
        *(int *)note_selected = *(int *)note_page;
    }
    note_page_index = *(int *)&l_18;
    note_reload_page();
    msgbox_show_rsc(1701, 1);
    return;
L4E6F9:;
    xn_mouse_set_position((int)(short)*(short *)(*(char **)note_selected + 1), (int)(short)*(short *)(*(char **)note_selected + 3));
    note_load_page();
    note_search_from = *(int *)note_selected;
}

void note_find_next(void)
{
    if (note_search_text == 0) return;
    if (((int)(unsigned char)*(signed char *)(*(char **)&note_search_from)) == 1) {
        note_search_from = (int)(*(char **)&note_search_from + 91);
    } else {
        note_search_from += 11;
    }
    *(signed char *)note_text_flags |= 128;
    note_find();
}

void note_undo(void)
{
    mc_memcpy(*(int *)note_page, note_page_backup, 3640, (int)D_00174FAC, 611, 4);
}

void note_clear_page(void)
{
    mc_memcpy(note_page_backup, *(int *)note_page, 3640, (int)D_00174FAC, 616, 4);
    mc_memset(*(int *)note_page, 0, 3640, (int)D_00174FAC, 617, 4);
}

void note_toggle_shadow(void)
{
    mc_memcpy(note_page_backup, *(int *)note_page, 3640, (int)D_00174FAC, 622, 4);
    if (*(int *)note_selected == 0 || ((int)(unsigned char)*(signed char *)(*(char **)note_selected)) != 1) {
        return;
    }
    *(signed char *)(*(char **)note_selected + 6) ^= 2;
}

void note_toggle_centre(void)
{
    mc_memcpy(note_page_backup, *(int *)note_page, 3640, (int)D_00174FAC, 629, 4);
    if (*(int *)note_selected == 0 || ((int)(unsigned char)*(signed char *)(*(char **)note_selected)) != 1) {
        return;
    }
    *(signed char *)(*(char **)note_selected + 6) ^= 1;
}

int note_select_text_cb(int a1)
{
    {
        char l_28[12];

        *(signed char *)((char *)a1 + 6) &= 191;
        note_text_box(a1, (int)l_28);
        if (rect_overlap((int)&D_001997B0, (int)l_28) != 0) *(signed char *)((char *)a1 + 6) |= 64;
        return 0;
    }
}

int note_select_line_cb(int a1)
{
    {
        char l_34[12];
        char l_28[12];

        *(signed char *)((char *)a1 + 10) &= 191;
        *(short *)((char *)l_28 + 4) = *(short *)((char *)a1 + 1);
        *(short *)l_28 = *(int *)((char *)l_28 + 4);
        *(short *)((char *)l_28 + 6) = *(short *)((char *)a1 + 3);
        *(short *)((char *)l_28 + 2) = *(int *)((char *)l_28 + 6);
        *(short *)((char *)l_34 + 4) = *(short *)((char *)a1 + 5);
        *(short *)l_34 = *(int *)((char *)l_34 + 4);
        *(short *)((char *)l_34 + 6) = *(short *)((char *)a1 + 7);
        *(short *)((char *)l_34 + 2) = *(int *)((char *)l_34 + 6);
        if (rect_overlap((int)&D_001997B0, (int)l_28) != 0 || rect_overlap((int)&D_001997B0, (int)l_34) != 0) {
            *(signed char *)((char *)a1 + 10) |= 64;
        }
        return 0;
    }
}

int rect_overlap(int a1, int a2)
{
    if (*(short *)((char *)a2 + 4) < *(short *)((char *)a1)) return 0;
    if (*(short *)((char *)a2) > *(short *)((char *)a1 + 4)) return 0;
    if (*(short *)((char *)a2 + 6) < *(short *)((char *)a1 + 2)) return 0;
    if (*(short *)((char *)a2 + 2) > *(short *)((char *)a1 + 6)) return 0;
    return 1;
}

void note_text_box(int a1, int a2)
{
    int l_14;

    xn_font_select((int)(short)((unsigned short)(unsigned char)D_00185201[(int)(unsigned char)*(signed char *)((char *)a1 + 5)]));
    l_14 = font_text_width(a1 + 11);
    *(short *)((char *)a2) = *(short *)((char *)a1 + 1);
    *(short *)((char *)a2 + 2) = *(short *)((char *)a1 + 3);
    *(short *)((char *)a2 + 4) = *(short *)((char *)a1 + 1) + l_14;
    *(short *)((char *)a2 + 6) = *(short *)((char *)a1 + 3) + font_height;
    if (((int)(unsigned char)(*(signed char *)((char *)a1 + 6) & 1)) == 0) return;
    *(short *)((char *)a2) = 160 - (((int)(short)*(short *)&l_14) >> 1);
    *(short *)((char *)a2 + 4) = *(short *)((char *)a2) + l_14;
}

void note_delete_in_box(void)
{
    D_001997CC = *(int *)note_page;
    note_page_walk(*(int *)note_page, (int)note_select_text_cb, (int)note_select_line_cb);
    note_delete_selected();
}

int note_keep_text_cb(int a1)
{
    if (((int)(unsigned char)(*(signed char *)((char *)a1 + 6) & 64)) != 0) return 0;
    mc_memcpy(D_001997CC, a1, 91, (int)D_00174FAC, 731, 4);
    D_001997CC = (int)(*(char **)&D_001997CC + 91);
    return 0;
}

int note_keep_line_cb(int a1)
{
    if (((int)(unsigned char)(*(signed char *)((char *)a1 + 10) & 64)) != 0) return 0;
    mc_memcpy(D_001997CC, a1, 11, (int)D_00174FAC, 739, 4);
    D_001997CC += 11;
    return 0;
}

void note_delete_selected(void)
{
    note_page_walk(*(int *)note_page, (int)note_keep_text_cb, (int)note_keep_line_cb);
    *(signed char *)(*(char **)&D_001997CC) = 0;
}
