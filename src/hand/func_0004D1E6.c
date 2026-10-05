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
extern char *D_00195C44;
extern unsigned char D_0019626F;
extern unsigned char D_00196272;
extern unsigned char game_mode;
extern int D_001997BC;
extern int D_001997C0;
extern int D_001997C4;
extern int D_001997D0;
extern char *note_page;
extern char *D_001997D8;
extern short D_001997E2;
extern short D_001997E8;
extern unsigned char note_tool;
extern unsigned char D_001997ED;
extern int key_action_held(int);
extern void fatal_error(char *);
extern int sound_play(int, struct record *, int);
extern int disk_read_file(char *, int);
extern int disk_open_rw(char *);
extern int disk_create(char *);
extern void mc_memset(char *, int, int, char *, int, int);
extern long lseek(int, long, int);
extern char *mc_malloc(int, char *, int);
extern int func_000A00CB(int, char *, int);
extern int write(int, char *, int);
extern void mc_memcpy(char *, char *, int, char *, int, int);
extern int filelength(int);

int func_0004D1E6(short a1)
{
    if (D_0019626F == 9 && game_mode == 8) return 1;
    D_001997ED = (a1 == 100);
    if (a1 != 0 || (game_mode == 0 && key_action_held(25) != 0)) {
        D_001940D5 |= 8;
        D_001997C4 = 0;
        D_001997D0 = 0;
        note_page = mc_malloc(3640, D_00174FAC, 67);
        D_001997D8 = mc_malloc(3640, D_00174FAC, 68);
        if ((D_001997E8 = disk_open_rw(D_00174FB3)) < 1) {
            mc_memset(D_00195C44, 0, 3640, D_00174FAC, 72, 4);
            if ((D_001997E8 = disk_create(D_00174FB3)) < 1)
                fatal_error(D_00174FC0);
            write(D_001997E8, D_00195C44, 3640);
            lseek(D_001997E8, 0, 0);
        }
        func_000A00CB(D_001997E8, note_page, 3640);
        mc_memcpy(D_001997D8, note_page, 3640, D_00174FAC, 79, 4);
        note_tool = 0;
        D_001997E2 = note_tool;
        D_001997BC = filelength(D_001997E8);
        if (D_001997ED == 0) {
            game_mode = 9;
            window_image = disk_read_file(D_00174FDF, 0);
            D_001997C0 = disk_read_file(D_00174FEC, 0);
            D_00196272 = 1;
            sound_play(237, player_object, 100);
        }
    }
    return game_mode == 9;
}
