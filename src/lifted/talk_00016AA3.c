/* talk.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "ptrint.h"

extern char D_00170404[];
extern char D_00170411[];
extern signed char D_00190D10;
extern iptr talk_face_image;

extern int disk_open_data(char *);
extern int rand_range(int, int);
extern int close();
extern int lseek();
extern int read();

void talk_load_face(int face)
{
    int file;
    int index;

    D_00190D10 = 1;
    if (face >= 1000) {
        index = ((face == 1000) ? 0 : 6);
        index += rand_range(0, 5);
        file = disk_open_data(D_00170404);
        lseek(file, index << 12, 0);
    } else {
        D_00190D10 = 1;
        file = disk_open_data(D_00170411);
        lseek(file, face << 12, 0);
    }
    read(file, talk_face_image, 4096);
    close(file);
}
