/* logbook.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"
#include "bitfield.h"
#include "clib.h"


extern char disk_last_file_size[];
extern signed char mouse_buttons;
extern short mouse_x;
extern short mouse_y;
extern signed char D_0012B508;
extern short font_height;
extern signed char key_down_esc;
extern short D_00142928;
extern short D_0014292C;
extern iptr screen_buffer;
extern iptr D_00147954;
extern char D_00175C6C[];
extern char D_00175C86[];
extern struct rect logbook_buttons[];
extern iptr logbook_notes_file;
extern signed char D_00187CA8;
extern signed char text_buffer[];
extern char scratch_190be4[];
extern char scratch_190d64[];
extern char scratch_190d66[];
extern signed char text_rsc_buffer[];
extern signed char D_001940D8;
extern struct record *logbook_object;
extern struct record *player_object;
extern iptr window_image;
extern char *scratch_buffer;
extern iptr text_macro_city;
extern signed char D_00196272;
extern signed char game_mode;
extern signed char mouse_buttons_prev;
extern signed char text_missing_ok;
extern int logbook_first_entry;
extern char logbook_show_notes[];
extern int logbook_entry_count;

extern int sheet_open(short);
extern struct quest *quest_find_by_id(int);
extern int font_char_width(unsigned char);
extern int sound_play(int, struct record *, int);
extern int logbook_open(int);
extern char *str_list_skip(char *, int);
extern iptr disk_read_file(iptr, iptr);
extern int disk_write_arena2_file(iptr, iptr, int);
extern int disk_open_rw(iptr);
extern int disk_create(iptr);
extern int disk_file_exists(iptr);
extern int xn_font_select(int);
extern void quest_load_text(struct quest *, int, short, int);
extern void book_flush_line(void);
extern void func_0005A1C8(char *);
int logbook_close(void);
void logbook_draw_entry(char *);
void logbook_build_entries(void);
void logbook_load_notes(void);
void logbook_draw(void);
void logbook_toggle_notes(void);
void logbook_trim_notes(void);

void logbook_update(void)
{
    int i;

    if (logbook_open(0) == 0) return;
    mc_memcpy((void *)screen_buffer, (void *)window_image, 64000, D_00175C86, 53, 4);
    logbook_draw();
    if (key_down_esc != 0) logbook_close();
    if (mouse_buttons == 0 || (mouse_buttons != 0 && mouse_buttons_prev != 0)) {
        return;
    }
    for (i = 0; i < 4; i++) {
        if (mouse_x > logbook_buttons[i].x0 && mouse_x < logbook_buttons[i].x1 && mouse_y > logbook_buttons[i].y0 && mouse_y < logbook_buttons[i].y1) {
            logbook_buttons[i].handler();
        }
    }
}

int logbook_close(void)
{
    text_macro_city = 0;
    while (key_down_esc != 0);
    if (*(int *)logbook_show_notes != 0) {
        logbook_toggle_notes();
        return 0;
    }
    game_mode = 0;
    D_00196272 = 0;
    if (window_image != 0 && window_image != (-1751672937)) {
        mc_free((void *)window_image, D_00175C86, 80);
        window_image = -1751672937;
    }
    if (((struct bf8_5_1 *)&D_001940D8)->f != 0) {
        D_001940D8 &= 223;
        sheet_open(1);
    } else {
        D_00187CA8 = 1;
    }
    return 1;
}

void logbook_draw_entry(char *text)
{
    int unused2;
    int unused1;
    slot16 line_height;

    D_0012B508 = 146;
    line_height = font_height;
    while (*(signed char *)text != 0) {
        switch (*(unsigned char *)text) {
        case 251:
            D_00142928 = *(short *)(text + 1);
            text += 3;
            break;
        case 247:
            func_0005A1C8(text + 1);
            while (*(signed char *)(text++) != 0);
            break;
        case 250:
            D_0012B508 = *(signed char *)(text + 1);
            text += 2;
            break;
        case 249:
            book_flush_line();
            xn_font_select((int)(short)((unsigned short)(unsigned char)*(signed char *)(text + 1)));
            text += 2;
            if ((short)(short)*(int *)&line_height < font_height) line_height = font_height;
            break;
        case 252:
            line_height = font_height;
            D_00142928 = 30;
            D_0014292C += *(int *)&line_height;
            text++;
            text_buffer[(int)(short)*(short *)scratch_190d64] = 0;
            book_flush_line();
            *(short *)scratch_190d64 = 0;
            if (((int)(short)D_0014292C) > 160) goto L6AB05;
            break;
        case 253:
            line_height = font_height;
            D_00142928 = 30;
            D_0014292C += *(int *)&line_height;
            text++;
            text_buffer[(int)(short)*(short *)scratch_190d64] = 0;
            *(short *)scratch_190d66 = 1;
            book_flush_line();
            *(short *)scratch_190d64 = 0;
            if (((int)(short)D_0014292C) > 160) goto L6AB05;
            break;
        case 1:
            text++;
            text_buffer[(int)(short)*(short *)scratch_190d64] = 0;
            book_flush_line();
            break;
        default:
            text_buffer[(int)(short)(*(short *)scratch_190d64)++] = *(signed char *)(text++);
        }
    }
    text_buffer[(int)(short)*(short *)scratch_190d64] = 0;
    book_flush_line();
L6AB05:;
    D_00142928 = 30;
    line_height = font_height;
    D_0014292C += *(int *)&line_height;
}

void logbook_prev_page(void)
{
    if (logbook_first_entry == 0) return;
    sound_play(205, player_object, 100);
    logbook_first_entry--;
}

void logbook_next_page(void)
{
    if ((logbook_entry_count - 1) <= logbook_first_entry) return;
    sound_play(205, player_object, 100);
    logbook_first_entry++;
}

void logbook_build_entries(void)
{
    struct logbook *logbook;
    int slot;
    int j;
    struct quest *quest;
    char *out;

    out = scratch_buffer + 20000;
    if (*(int *)logbook_show_notes != 0 && disk_file_exists(logbook_notes_file) != 0) {
        logbook_load_notes();
        return;
    }
    text_missing_ok = 1;
    logbook_entry_count = 0;
    logbook = &logbook_object->data.logbook;
    for (slot = 0; slot < 32; slot++) {
        if (logbook->quest_ids[slot] == 0) continue;
        quest = quest_find_by_id((int)logbook->quest_ids[slot]);
        if (quest == 0) {
            logbook->quest_ids[slot] = 0;
            mc_memset(logbook->message_ids[slot], 0, 20, D_00175C86, 204, 20);
            continue;
        }
        for (j = 0; j < 10; j++) {
            if (logbook->message_ids[slot][j] == 0) continue;
            text_macro_city = (iptr)logbook->places[slot];
            *(int *)scratch_190be4 = logbook->message_times[slot][j];
            text_rsc_buffer[0] = 0;
            quest_load_text(quest, (int)logbook->message_ids[slot][j], 0, 0);
            if (text_rsc_buffer[0] == 0) continue;
            mc_strncpy(out, (char *)text_rsc_buffer, 4, D_00175C86, 216);
            out += strlen(out) + 1;
            logbook_entry_count++;
        }
    }
    *(signed char *)(out++) = 0;
    text_macro_city = 0;
    text_missing_ok = 0;
}

void logbook_load_notes(void)
{
    char *text;
    int unused;

    text = scratch_buffer + 20000;
    mc_memset(text, 0, 35000, D_00175C86, 233, 4);
    disk_read_file((iptr)D_00175C6C, (iptr)text);
    while (*(signed char *)text != 0) {
        text += strlen(text) + 1;
        logbook_entry_count++;
    }
    *(signed char *)(text++) = 0;
    *(signed char *)(text++) = 0;
}

void logbook_draw(void)
{
    int i;
    char *entry;

    D_00142928 = 30;
    D_0014292C = 25;
    *(short *)scratch_190d66 = 0;
    xn_font_select(4);
    entry = str_list_skip(scratch_buffer + 20000, logbook_first_entry);
    for (i = logbook_first_entry; i < logbook_entry_count; i++) {
        if (((int)(short)D_0014292C) > 160) return;
        logbook_draw_entry(entry);
        entry += strlen(entry) + 1;
        if (*(signed char *)entry == 0) {
            entry++;
            D_0014292C += 8;
        }
    }
}

void logbook_toggle_notes(void)
{
    *(signed char *)logbook_show_notes ^= 1;
    logbook_first_entry = 0;
    logbook_build_entries();
    sound_play(237, player_object, 100);
}

void logbook_copy_text(char *text)
{
    int file;
    int file_size;
    int line_width;
    int ch;

    *(signed char *)&ch = 0;
    if (disk_file_exists(logbook_notes_file) != 0) {
        file = disk_open_rw(logbook_notes_file);
    } else {
        file = disk_create(logbook_notes_file);
    }
    if (file == (-1)) return;
    file_size = lseek(file, 0, 2);
    line_width = 0;
    while (*(signed char *)text != 0) {
        line_width += font_char_width((int)(unsigned char)*(signed char *)text);
        if (line_width > 240 && ((int)(unsigned char)*(signed char *)text) == 32) {
            write(file, &ch, 1);
            line_width = 0;
        } else {
            write(file, text, 1);
        }
        text++;
    }
    write(file, text, 1);
    *(signed char *)&ch = 32;
    write(file, &ch, 1);
    *(signed char *)&ch = 0;
    write(file, &ch, 1);
    close(file);
    if (file_size <= 32768) return;
    logbook_trim_notes();
}

void logbook_trim_notes(void)
{
    char *line_end;

    disk_read_file(logbook_notes_file, D_00147954);
    while (*(int *)disk_last_file_size > 32768) {
        line_end = (char *)D_00147954;
        while (*(signed char *)line_end != 0) line_end++;
        mc_memcpy((void *)D_00147954, line_end + 1, 40960, D_00175C86, 401, 4);
        *(int *)disk_last_file_size -= (int)((line_end + 1) - (char *)D_00147954);
    }
    disk_write_arena2_file(logbook_notes_file, D_00147954, *(int *)disk_last_file_size);
}

void logbook_prune_quests(void)
{
    struct logbook *logbook;
    int slot;
    struct quest *quest;

    logbook = &logbook_object->data.logbook;
    for (slot = 0; slot < 32; slot++) {
        if (logbook->quest_ids[slot] != 0) {
            quest = quest_find_by_id((int)logbook->quest_ids[slot]);
            if (quest == 0) {
                logbook->quest_ids[slot] = 0;
                mc_memset(logbook->message_ids[slot], 0, 20, D_00175C86, 422, 20);
            }
        }
    }
}
