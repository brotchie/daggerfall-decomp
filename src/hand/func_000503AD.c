/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000503AD */
#include "structs.h"
extern char D_0017531A[];
extern signed char text_buffer[];
extern struct flat_cfg flats_cfg[];
extern iptr scratch_buffer;
extern int flats_cfg_count;
extern void cfg_read_line(int *, char *);
extern int cfg_read_number(int *);
extern iptr disk_read_file(char *, iptr);

void flats_cfg_load(void)
{
    iptr fh;
    int a;
    int b;

    fh = disk_read_file(D_0017531A, scratch_buffer);
    flats_cfg_count = 0;
    for (;;) {
        a = cfg_read_number(&fh);
        if (a == 100000)
            return;
        b = cfg_read_number(&fh);
        flats_cfg[flats_cfg_count].image = (a << 7) | b;
        cfg_read_line(&fh, flats_cfg[flats_cfg_count].name);
        cfg_read_line(&fh, ((char *)text_buffer));
        if (((char *)text_buffer)[0] == '?') {
            flats_cfg[flats_cfg_count].flags |= 2;
            a = 1;
        } else {
            a = 0;
        }
        if (((char *)text_buffer)[a] == '2')
            flats_cfg[flats_cfg_count].flags |= 1;
        flats_cfg[flats_cfg_count].pad07 = cfg_read_number(&fh);
        flats_cfg[flats_cfg_count].pad08 = cfg_read_number(&fh);
        flats_cfg[flats_cfg_count].face = cfg_read_number(&fh);
        flats_cfg_count++;
    }
}
