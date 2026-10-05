/* note.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"
#include "bitfield.h"

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
extern struct rect D_001997B0;      /* the selection box */
extern int note_file_size;
extern int note_rci;
extern int note_search_text;
extern union note_entry *note_selected;
extern int D_001997CC;
extern int note_search_from;
extern union note_entry *note_page;
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

extern int sheet_open(short);
extern int note_text_hit_cb(int);
extern char *note_page_walk(int, int, int);
extern int font_text_width(char *);
extern int sound_play(int, struct record *, int);
extern int close();
extern int mc_free();
extern int mc_memset();
extern int lseek();
extern int read();
extern int mc_strncpy();
extern int write();
extern int strlen();
extern int mc_set_location(int, int);
extern int mc_sprintf(char *, ...);
extern int mc_memcpy();
extern int filelength();
extern int func_000A138E();
extern int strstr();
extern int xn_mouse_set_position();
extern int xn_font_select();
extern int xn_draw_line_to();
extern void msgbox_show_string(char *, short);
extern void msgbox_show_rsc(int, int);
extern void note_add_line(short, short, short, short);
extern void text_draw(char *, short, short);
extern void text_draw_centred(char *, int, short);
extern void text_draw_coloured(char *, int, int, int, unsigned char);
extern void text_draw_centred_coloured(char *, int, int, int, unsigned char);
extern void inpstr_begin_number(int);
extern void inpstr_begin_text(char *, int);
int note_find_match_cb(struct note_text *);
int note_select_text_cb(struct note_text *);
int note_select_line_cb(struct note_line *);
int rect_overlap(struct rect *, struct rect *);
int note_keep_text_cb(struct note_text *);
int note_keep_line_cb(struct note_line *);
void note_save_page(void);
void note_reload_page(void);
void note_load_page(void);
void note_find(void);
void note_text_box(struct note_text *, struct rect *);
void note_delete_selected(void);
#pragma aux mc_set_location parm routine [];

int note_close(void)
{
    note_save_page();
    if ((int)note_page != 0 && (int)note_page != (-1751672937)) {
        mc_free((int)note_page, (int)D_00174FAC, 206);
        note_page = (union note_entry *)-1751672937;
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
    struct note_text *entry;

    if (note_tool == 0 && note_action == 0) {
        D_001940D5 |= 4;
        entry = (struct note_text *)note_page_walk((int)note_page, (int)note_text_hit_cb, 0);
        if (entry != 0) {
            if ((entry->flags & 1) != 0) {
                text_cursor_x = (note_cursor_x = 160 - (font_text_width(entry->text) >> 1));
                text_cursor_y = (note_cursor_y = entry->y);
            } else {
                text_cursor_x = (note_cursor_x = entry->x);
                text_cursor_y = (note_cursor_y = entry->y);
            }
            note_action = 1;
            xn_font_select((int)(short)((unsigned short)(unsigned char)D_00185201[entry->font]));
            inpstr_begin_text(entry->text, 79);
            *(signed char *)note_text_flags |= 32;
            note_selected = (union note_entry *)entry;
        } else {
            text_cursor_x = (note_cursor_x = mouse_x);
            text_cursor_y = (note_cursor_y = mouse_y);
            note_action = 1;
            mc_memset((int)text_rsc_buffer, 0, 81, (int)D_00174FAC, 270, 2048);
            xn_font_select((int)(short)((unsigned short)(unsigned char)D_00185201[(int)(short)*(short *)note_font]));
            inpstr_begin_text((char *)text_rsc_buffer, 79);
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
    D_001997B0.x0 = mouse_x;
    D_001997B0.y0 = mouse_y;
}

void note_draw_page(void)
{
    char *entry;

    entry = (char *)note_page;
    while (*(signed char *)entry != 0) {
        switch (*(unsigned char *)entry) {
        case 1:
            if (note_silent == 0 && (((int)(short)(*(short *)note_text_flags & 32)) == 0 || entry != (char *)note_selected)) {
                xn_font_select((int)(short)((unsigned short)(unsigned char)D_00185201[(int)(unsigned char)*(signed char *)(entry + 5)]));
                if (((int)(unsigned char)(*(signed char *)(entry + 6) & 64)) != 0) {
                    D_0012B508 = (*(signed char *)frame_counter & 15) + 240;
                } else {
                    D_0012B508 = *(signed char *)(entry + 7);
                }
                if (((int)(unsigned char)(*(signed char *)(entry + 6) & 2)) != 0) {
                    if (((int)(unsigned char)(*(signed char *)(entry + 6) & 1)) != 0) {
                        text_draw_centred_coloured(entry + 11, 160, (int)(short)*(short *)(entry + 3), (int)(short)((int)(unsigned char)D_0012B508), 156);
                    } else {
                        text_draw_coloured(entry + 11, (int)(short)*(short *)(entry + 1), (int)(short)*(short *)(entry + 3), (int)(short)((int)(unsigned char)D_0012B508), 156);
                    }
                } else if (((int)(unsigned char)(*(signed char *)(entry + 6) & 1)) != 0) {
                    text_draw_centred(entry + 11, 160, (int)(short)*(short *)(entry + 3));
                } else {
                    text_draw(entry + 11, (int)(short)*(short *)(entry + 1), (int)(short)*(short *)(entry + 3));
                }
            }
            entry += 91;
            break;
        case 2:
            if (note_silent != 0) {
                (*(char (**)[11])&entry)++;
            } else {
                if (((int)(unsigned char)(*(signed char *)(entry + 10) & 64)) != 0) {
                    D_0012B508 = (*(signed char *)frame_counter & 15) + 240;
                } else {
                    D_0012B508 = *(signed char *)(entry + 9);
                }
                D_00142928 = *(short *)(entry + 1);
                D_0014292C = *(short *)(entry + 3);
                xn_draw_line_to((int)(short)*(short *)(entry + 5), (int)(short)*(short *)(entry + 7));
                (*(char (**)[11])&entry)++;
            }
        }
    }
    note_page_free = 3640 - ((int)entry - *(short *)&note_page);
}

void note_add_text(char *text)
{
    char *entry;

    if (((unsigned)((int)(short)note_page_free)) < 91) {
        msgbox_show_rsc(1700, 1);
        return;
    }
    mc_memcpy(note_page_backup, (int)note_page, 3640, (int)D_00174FAC, 361, 4);
    D_001940D5 |= 16;
    entry = (char *)note_page;
    while (*(signed char *)entry != 0) {
        if (((int)(unsigned char)*(signed char *)entry) == 1) {
            entry += 91;
        } else {
            (*(char (**)[11])&entry)++;
        }
    }
    *(signed char *)entry = 1;
    *(short *)(entry + 1) = note_cursor_x;
    *(short *)(entry + 3) = note_cursor_y;
    *(signed char *)(entry + 7) = *(signed char *)note_colour;
    *(signed char *)(entry + 5) = *(signed char *)note_font;
    *(signed char *)(entry + 6) = *(signed char *)note_text_flags;
    mc_strncpy(entry + 11, text, 4, (int)D_00174FAC, 381);
    note_selected = (union note_entry *)entry;
    entry += 91;
    *(signed char *)entry = 0;
}

void note_save_page(void)
{
    if (((struct bf8_4_1 *)&D_001940D5)->f == 0) return;
    D_001940D5 &= 239;
    lseek((int)(short)note_file, ((int)(short)note_page_index) * 3640, 0);
    write((int)(short)note_file, (int)note_page, 3640);
    note_file_size = filelength((int)(short)note_file);
}

void note_reload_page(void)
{
    note_load_page();
    note_selected = (union note_entry *)(note_search_from = 0);
}

void note_load_page(void)
{
    lseek((int)(short)note_file, ((int)(short)note_page_index) * 3640, 0);
    read((int)(short)note_file, (int)note_page, 3640);
    mc_memcpy(note_page_backup, (int)note_page, 3640, (int)D_00174FAC, 450, 4);
}

void note_goto_page_prompt(void)
{
    char *prompt;

    D_0012B508 = 146;
    prompt = *(char **)scratch_buffer + 55000;
    mc_set_location(459, (int)D_00174FAC);
    mc_sprintf(prompt, (int)D_00174FF5, D_0017D1E6);
    *(signed char *)((char *)(strlen(prompt) + prompt) + 1) = 0;
    msgbox_show_string(prompt, 2);
    inpstr_begin_number(((int)(short)note_page_index) + 1);
    note_action = 4;
}

void note_goto_page(int page_index)
{
{
    int unused;
    int offset;

    offset = ((int)(short)*(short *)&page_index) * 3640;
    if (((unsigned)offset) > note_file_size) return;
    note_save_page();
    note_page_index = page_index;
    note_load_page();
    if (note_silent != 0) return;
    sound_play(205, player_object, 100);
}
}

void note_prev_page(void)
{
    if (note_page_index == 0) return;
    note_save_page();
    note_page_index--;
    note_reload_page();
    if (note_silent != 0) return;
    sound_play(205, player_object, 100);
}

void note_next_page(void)
{
    short page_count;

    *(int *)&page_count = ((unsigned)lseek((int)(short)note_file, 0, 2)) / 3640;
    note_save_page();
    if (((int)(short)note_page_index) < (((int)(short)page_count) - 1)) {
        note_page_index++;
        note_reload_page();
        if (note_silent == 0) sound_play(205, player_object, 100);
        return;
    }
    if (note_page->kind == 0) return;
    lseek((int)(short)note_file, 0, 2);
    mc_memset((int)note_page, 0, 3640, (int)D_00174FAC, 507, 4);
    write((int)(short)note_file, (int)note_page, 3640);
    note_file_size = filelength((int)(short)note_file);
    note_page_index++;
    if (note_silent != 0) return;
    sound_play(205, player_object, 100);
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
    char *prompt;

    xn_font_select(4);
    note_search_text = *(int *)scratch_buffer + 56000;
    prompt = *(char **)scratch_buffer + 55000;
    mc_set_location(540, (int)D_00174FAC);
    mc_sprintf(prompt, (int)D_00175010, D_0017D1F2);
    *(signed char *)((char *)(strlen(prompt) + prompt) + 1) = 0;
    msgbox_show_string(prompt, 2);
    mc_memset(note_search_text, 0, 24, (int)D_00174FAC, 543, 4);
    inpstr_begin_text((char *)note_search_text, 23);
    *(signed char *)note_text_flags |= 128;
    note_selected = note_page;
}

int note_find_match_cb(struct note_text *entry)
{
    if (strstr(entry->text, note_search_text) != 0) {
        note_selected = (union note_entry *)entry;
        return 1;
    }
    return 0;
}

void note_find(void)
{
    int unused;
    short page_count;
    short start_page;

    if (((int)(unsigned char)game_mode) != 9 || ((int)(short)(*(short *)note_text_flags & 128)) == 0 || *(signed char *)(((char *)note_search_text)) == 0) {
        return;
    }
    *(short *)note_text_flags &= 127;
    start_page = note_page_index;
    note_save_page();
    *(int *)&page_count = ((unsigned)lseek((int)(short)note_file, 0, 2)) / 3640;
    if (note_search_from != 0) {
        note_selected = (union note_entry *)note_search_from;
    } else {
        note_selected = note_page;
    }
    while (note_page_index < page_count) {
        note_load_page();
        if (note_page_walk((int)note_selected, (int)note_find_match_cb, 0) != 0) goto L4E6F9;
        note_page_index++;
        note_selected = note_page;
    }
    note_page_index = *(int *)&start_page;
    note_reload_page();
    msgbox_show_rsc(1701, 1);
    return;
L4E6F9:;
    xn_mouse_set_position(note_selected->text.x, note_selected->text.y);
    note_load_page();
    note_search_from = (int)note_selected;
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
    mc_memcpy((int)note_page, note_page_backup, 3640, (int)D_00174FAC, 611, 4);
}

void note_clear_page(void)
{
    mc_memcpy(note_page_backup, (int)note_page, 3640, (int)D_00174FAC, 616, 4);
    mc_memset((int)note_page, 0, 3640, (int)D_00174FAC, 617, 4);
}

void note_toggle_shadow(void)
{
    mc_memcpy(note_page_backup, (int)note_page, 3640, (int)D_00174FAC, 622, 4);
    if (note_selected == 0 || note_selected->kind != 1) {
        return;
    }
    note_selected->text.flags ^= 2;
}

void note_toggle_centre(void)
{
    mc_memcpy(note_page_backup, (int)note_page, 3640, (int)D_00174FAC, 629, 4);
    if (note_selected == 0 || note_selected->kind != 1) {
        return;
    }
    note_selected->text.flags ^= 1;
}

int note_select_text_cb(struct note_text *entry)
{
    {
        struct rect box;

        entry->flags &= 191;
        note_text_box(entry, &box);
        if (rect_overlap(&D_001997B0, &box) != 0) entry->flags |= 64;
        return 0;
    }
}

int note_select_line_cb(struct note_line *line)
{
    {
        struct rect end_box;
        struct rect start_box;

        line->flags &= 191;
        start_box.x0 = start_box.x1 = line->x0;
        start_box.y0 = start_box.y1 = line->y0;
        end_box.x0 = end_box.x1 = line->x1;
        end_box.y0 = end_box.y1 = line->y1;
        if (rect_overlap(&D_001997B0, &start_box) != 0 || rect_overlap(&D_001997B0, &end_box) != 0) {
            line->flags |= 64;
        }
        return 0;
    }
}

int rect_overlap(struct rect *box_a, struct rect *box_b)
{
    if (box_b->x1 < box_a->x0) return 0;
    if (box_b->x0 > box_a->x1) return 0;
    if (box_b->y1 < box_a->y0) return 0;
    if (box_b->y0 > box_a->y1) return 0;
    return 1;
}

void note_text_box(struct note_text *entry, struct rect *box)
{
    int width;

    xn_font_select((int)(short)((unsigned short)(unsigned char)D_00185201[entry->font]));
    width = font_text_width(entry->text);
    box->x0 = entry->x;
    box->y0 = entry->y;
    box->x1 = entry->x + width;
    box->y1 = entry->y + font_height;
    if ((entry->flags & 1) == 0) return;
    box->x0 = 160 - (((int)(short)*(short *)&width) >> 1);
    box->x1 = box->x0 + width;
}

void note_delete_in_box(void)
{
    D_001997CC = (int)note_page;
    note_page_walk((int)note_page, (int)note_select_text_cb, (int)note_select_line_cb);
    note_delete_selected();
}

int note_keep_text_cb(struct note_text *entry)
{
    if ((entry->flags & 64) != 0) return 0;
    mc_memcpy(D_001997CC, entry, 91, (int)D_00174FAC, 731, 4);
    D_001997CC = (int)(*(char **)&D_001997CC + 91);
    return 0;
}

int note_keep_line_cb(struct note_line *line)
{
    if ((line->flags & 64) != 0) return 0;
    mc_memcpy(D_001997CC, line, 11, (int)D_00174FAC, 739, 4);
    D_001997CC += 11;
    return 0;
}

void note_delete_selected(void)
{
    note_page_walk((int)note_page, (int)note_keep_text_cb, (int)note_keep_line_cb);
    *(signed char *)(*(char **)&D_001997CC) = 0;
}
