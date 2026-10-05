/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0004D1E6 */
#include "records.h"

extern char D_00174FAC[];
extern char D_00174FB3[];
extern char D_00174FC0[];
extern char D_00174FDF[];
extern char D_00174FEC[];
extern unsigned char D_001940D5;
extern struct record *player_object;
extern int window_image;
extern char *scratch_buffer;
extern unsigned char D_0019626F;
extern unsigned char D_00196272;
extern unsigned char game_mode;
extern int note_file_size;
extern int note_rci;
extern int note_search_text;
extern int note_search_from;
extern char *note_page;
extern char *note_page_backup;
extern short note_page_index;
extern short note_file;
extern unsigned char note_tool;
extern unsigned char note_silent;
extern int key_action_held(int);
extern void fatal_error(char *);
extern int sound_play(int, struct record *, int);
extern int disk_read_file(char *, int);
extern int disk_open_rw(char *);
extern int disk_create(char *);
extern void mc_memset(char *, int, int, char *, int, int);
extern long lseek(int, long, int);
extern char *mc_malloc(int, char *, int);
extern int read(int, char *, int);
extern int write(int, char *, int);
extern void mc_memcpy(char *, char *, int, char *, int, int);
extern int filelength(int);

int note_open_notebook(short mode)
{
    if (D_0019626F == 9 && game_mode == 8) return 1;
    note_silent = (mode == 100);
    if (mode != 0 || (game_mode == 0 && key_action_held(25) != 0)) {
        D_001940D5 |= 8;
        note_search_text = 0;
        note_search_from = 0;
        note_page = mc_malloc(3640, D_00174FAC, 67);
        note_page_backup = mc_malloc(3640, D_00174FAC, 68);
        if ((note_file = disk_open_rw(D_00174FB3)) < 1) {
            mc_memset(scratch_buffer, 0, 3640, D_00174FAC, 72, 4);
            if ((note_file = disk_create(D_00174FB3)) < 1)
                fatal_error(D_00174FC0);
            write(note_file, scratch_buffer, 3640);
            lseek(note_file, 0, 0);
        }
        read(note_file, note_page, 3640);
        mc_memcpy(note_page_backup, note_page, 3640, D_00174FAC, 79, 4);
        note_tool = 0;
        note_page_index = note_tool;
        note_file_size = filelength(note_file);
        if (note_silent == 0) {
            game_mode = 9;
            window_image = disk_read_file(D_00174FDF, 0);
            note_rci = disk_read_file(D_00174FEC, 0);
            D_00196272 = 1;
            sound_play(237, player_object, 100);
        }
    }
    return game_mode == 9;
}
