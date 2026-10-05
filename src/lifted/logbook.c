/* logbook.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

struct bf8_5_1 { unsigned char _:5; unsigned char f:1; };
extern char disk_last_file_size[];
extern signed char mouse_buttons;
extern short mouse_x;
extern short mouse_y;
extern signed char D_0012B508;
extern short D_0012DA44;
extern signed char key_down_esc;
extern short D_00142928;
extern short D_0014292C;
extern int screen_buffer;
extern int D_00147954;
extern char D_00175C6C[];
extern char D_00175C86[];
extern char logbook_buttons[];
extern char D_00186DF2[];
extern char D_00186DF4[];
extern char D_00186DF6[];
extern char D_00186DF8[];
extern int logbook_notes_file;
extern signed char D_00187CA8;
extern signed char text_buffer[];
extern char D_00190BE4[];
extern char D_00190D64[];
extern char D_00190D66[];
extern signed char text_rsc_buffer[];
extern signed char D_001940D8;
extern struct record *logbook_object;
extern struct record *player_object;
extern int window_image;
extern int D_00195C44;
extern int D_00195D94;
extern signed char D_00196272;
extern signed char game_mode;
extern signed char mouse_buttons_prev;
extern signed char D_00196295;
extern int logbook_first_entry;
extern char logbook_show_notes[];
extern int logbook_entry_count;

extern int sheet_open(int);
extern struct quest *quest_find_by_id(int);
extern int font_char_width(unsigned char);
extern int sound_play(int, int, int);
extern int logbook_open(int);
extern int func_0006AE87(int, int);
extern int disk_read_file(int, int);
extern int disk_write_arena2_file(int, int, int);
extern int disk_open_rw(int);
extern int disk_create(int);
extern int disk_file_exists(int);
extern int func_0009DEA7();
extern int mc_free();
extern int mc_memset();
extern int lseek();
extern int mc_strncpy();
extern int write();
extern int func_000A0DF4();
extern int mc_memcpy();
extern int func_0012DB50();
extern void quest_load_text(struct quest *, int, int, int);
extern void book_flush_line(void);
extern void func_0005A1C8(int);
int logbook_close(void);
void logbook_draw_entry(int);
void logbook_build_entries(void);
void logbook_load_notes(void);
void logbook_draw(void);
void logbook_toggle_notes(void);
void func_0006B24F(void);

void logbook_update(void)
{
    int l_18;

    if (logbook_open(0) == 0) return;
    mc_memcpy(screen_buffer, window_image, 64000, (int)D_00175C86, 53, 4);
    logbook_draw();
    if (key_down_esc != 0) logbook_close();
    if (mouse_buttons == 0 || (mouse_buttons != 0 && mouse_buttons_prev != 0)) {
        return;
    }
    for (l_18 = 0; l_18 < 4; l_18++) {
        if (mouse_x > *(short *)(logbook_buttons + (l_18 * 12)) && mouse_x < *(short *)(D_00186DF4 + (l_18 * 12)) && mouse_y > *(short *)(D_00186DF2 + (l_18 * 12)) && mouse_y < *(short *)(D_00186DF6 + (l_18 * 12))) {
            ((int (*)())(*(int *)(D_00186DF8 + (l_18 * 12))))();
        }
    }
}

int logbook_close(void)
{
    D_00195D94 = 0;
    do {
    } while (key_down_esc != 0);
    if (*(int *)logbook_show_notes != 0) {
        logbook_toggle_notes();
        return 0;
    }
    game_mode = 0;
    D_00196272 = 0;
    if (window_image != 0 && window_image != (-1751672937)) {
        mc_free(window_image, (int)D_00175C86, 80);
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

void logbook_draw_entry(int a1)
{
    int l_20;
    int l_1C;
    short l_18;

    D_0012B508 = 146;
    l_18 = D_0012DA44;
    while (*(signed char *)((char *)a1) != 0) {
        switch (*(unsigned char *)((char *)a1)) {
        case 251:
            D_00142928 = *(short *)((char *)a1 + 1);
            a1 += 3;
            break;
        case 247:
            func_0005A1C8(a1 + 1);
            do {
            } while (*(signed char *)((char *)a1++) != 0);
            break;
        case 250:
            D_0012B508 = *(signed char *)((char *)a1 + 1);
            a1 += 2;
            break;
        case 249:
            book_flush_line();
            func_0012DB50((int)(short)((unsigned short)(unsigned char)*(signed char *)((char *)a1 + 1)));
            a1 += 2;
            if ((short)(short)*(int *)&l_18 < D_0012DA44) l_18 = D_0012DA44;
            break;
        case 252:
            l_18 = D_0012DA44;
            D_00142928 = 30;
            D_0014292C += *(int *)&l_18;
            a1++;
            text_buffer[(int)(short)*(short *)D_00190D64] = 0;
            book_flush_line();
            *(short *)D_00190D64 = 0;
            if (((int)(short)D_0014292C) > 160) goto L6AB05;
            break;
        case 253:
            l_18 = D_0012DA44;
            D_00142928 = 30;
            D_0014292C += *(int *)&l_18;
            a1++;
            text_buffer[(int)(short)*(short *)D_00190D64] = 0;
            *(short *)D_00190D66 = 1;
            book_flush_line();
            *(short *)D_00190D64 = 0;
            if (((int)(short)D_0014292C) > 160) goto L6AB05;
            break;
        case 1:
            a1++;
            text_buffer[(int)(short)*(short *)D_00190D64] = 0;
            book_flush_line();
            break;
        default:
            text_buffer[(int)(short)(*(short *)D_00190D64)++] = *(signed char *)((char *)a1++);
        }
    }
    text_buffer[(int)(short)*(short *)D_00190D64] = 0;
    book_flush_line();
L6AB05:;
    D_00142928 = 30;
    l_18 = D_0012DA44;
    D_0014292C += *(int *)&l_18;
}

void logbook_prev_page(void)
{
    if (logbook_first_entry == 0) return;
    sound_play(205, (int)player_object, 100);
    (logbook_first_entry)--;
}

void logbook_next_page(void)
{
    if ((logbook_entry_count - 1) <= logbook_first_entry) return;
    sound_play(205, (int)player_object, 100);
    (logbook_first_entry)++;
}

void logbook_build_entries(void)
{
    int l_28;
    int l_24;
    int l_20;
    struct quest *l_1C;
    int l_18;

    l_18 = D_00195C44 + 20000;
    if (*(int *)logbook_show_notes != 0 && disk_file_exists(logbook_notes_file) != 0) {
        logbook_load_notes();
        return;
    }
    D_00196295 = 1;
    logbook_entry_count = 0;
    l_28 = (int)RECORD_DATA(logbook_object);
    for (l_24 = 0; l_24 < 32; l_24++) {
        if (*(short *)((char *)((l_24 * 2) + l_28)) == 0) continue;
        l_1C = quest_find_by_id((int)(short)*(short *)((char *)((l_24 * 2) + l_28)));
        if (l_1C == 0) {
            *(short *)((char *)((l_24 * 2) + l_28)) = 0;
            mc_memset((l_28 + 64) + (l_24 * 20), 0, 20, (int)D_00175C86, 204, 20);
            continue;
        }
        for (l_20 = 0; l_20 < 10; l_20++) {
            if (*(short *)((char *)(int)((char *)((l_24 * 20) + l_28) + (l_20 * 2)) + 64) == 0) continue;
            D_00195D94 = (l_28 + 1984) + (l_24 << 5);
            *(int *)D_00190BE4 = *(int *)((char *)((l_28 + (l_24 * 40)) + (l_20 << 2)) + 704);
            text_rsc_buffer[0] = 0;
            quest_load_text(l_1C, (int)(short)*(short *)((char *)(((l_24 * 20) + l_28) + (l_20 * 2)) + 64), 0, 0);
            if (text_rsc_buffer[0] == 0) continue;
            mc_strncpy(l_18, (int)text_rsc_buffer, 4, (int)D_00175C86, 216);
            l_18 += func_000A0DF4(l_18) + 1;
            (logbook_entry_count)++;
        }
    }
    *(signed char *)((char *)l_18++) = 0;
    D_00195D94 = 0;
    D_00196295 = 0;
}

void logbook_load_notes(void)
{
    int l_1C;
    int l_18;

    l_1C = D_00195C44 + 20000;
    mc_memset(l_1C, 0, 35000, (int)D_00175C86, 233, 4);
    disk_read_file((int)D_00175C6C, l_1C);
    while (*(signed char *)((char *)l_1C) != 0) {
        l_1C += func_000A0DF4(l_1C) + 1;
        (logbook_entry_count)++;
    }
    *(signed char *)((char *)l_1C++) = 0;
    *(signed char *)((char *)l_1C++) = 0;
}

void logbook_draw(void)
{
    int l_1C;
    int l_18;

    D_00142928 = 30;
    D_0014292C = 25;
    *(short *)D_00190D66 = 0;
    func_0012DB50(4);
    l_18 = func_0006AE87(D_00195C44 + 20000, logbook_first_entry);
    for (l_1C = logbook_first_entry; l_1C < logbook_entry_count; l_1C++) {
        if (((int)(short)D_0014292C) > 160) return;
        logbook_draw_entry(l_18);
        l_18 += func_000A0DF4(l_18) + 1;
        if (*(signed char *)((char *)l_18) == 0) {
            l_18++;
            D_0014292C += 8;
        }
    }
}

void logbook_toggle_notes(void)
{
    *(signed char *)logbook_show_notes ^= 1;
    logbook_first_entry = 0;
    logbook_build_entries();
    sound_play(237, (int)player_object, 100);
}

void logbook_copy_text(int a1)
{
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    *(signed char *)&l_18 = 0;
    if (disk_file_exists(logbook_notes_file) != 0) {
        l_24 = disk_open_rw(logbook_notes_file);
    } else {
        l_24 = disk_create(logbook_notes_file);
    }
    if (l_24 == (-1)) return;
    l_20 = lseek(l_24, 0, 2);
    l_1C = 0;
    while (*(signed char *)((char *)a1) != 0) {
        l_1C += font_char_width((int)(unsigned char)*(signed char *)((char *)a1));
        if (l_1C > 240 && ((int)(unsigned char)*(signed char *)((char *)a1)) == 32) {
            write(l_24, (int)&l_18, 1);
            l_1C = 0;
        } else {
            write(l_24, a1, 1);
        }
        a1++;
    }
    write(l_24, a1, 1);
    *(signed char *)&l_18 = 32;
    write(l_24, (int)&l_18, 1);
    *(signed char *)&l_18 = 0;
    write(l_24, (int)&l_18, 1);
    func_0009DEA7(l_24);
    if (l_20 <= 32768) return;
    func_0006B24F();
}

void func_0006B24F(void)
{
    int l_18;

    disk_read_file(logbook_notes_file, D_00147954);
    while (*(int *)disk_last_file_size > 32768) {
        l_18 = D_00147954;
        while (*(signed char *)((char *)l_18) != 0) l_18++;
        mc_memcpy(D_00147954, l_18 + 1, 40960, (int)D_00175C86, 401, 4);
        *(int *)disk_last_file_size -= (l_18 + 1) - D_00147954;
    }
    disk_write_arena2_file(logbook_notes_file, D_00147954, *(int *)disk_last_file_size);
}

void logbook_prune_quests(void)
{
    int l_20;
    int l_1C;
    struct quest *l_18;

    l_20 = (int)RECORD_DATA(logbook_object);
    for (l_1C = 0; l_1C < 32; l_1C++) {
        if (*(short *)((char *)((l_1C * 2) + l_20)) != 0) {
            l_18 = quest_find_by_id((int)(short)*(short *)((char *)((l_1C * 2) + l_20)));
            if (l_18 == 0) {
                *(short *)((char *)((l_1C * 2) + l_20)) = 0;
                mc_memset((l_20 + 64) + (l_1C * 20), 0, 20, (int)D_00175C86, 422, 20);
            }
        }
    }
}
