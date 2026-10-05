/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000503AD */
struct entry {
    short id;                   /* 0x00 */
    short f2;
    short f4;                   /* 0x04 */
    unsigned char flags;        /* 0x06 */
    unsigned char f7;           /* 0x07 */
    unsigned char f8;           /* 0x08 */
    char name[31];              /* 0x09 */
};
extern char D_0017531A[];
extern signed char text_buffer[];
extern struct entry flats_cfg[];
extern int scratch_buffer;
extern int flats_cfg_count;
extern void cfg_read_line(int *, char *);
extern int cfg_read_number(int *);
extern int disk_read_file(char *, int);

void flats_cfg_load(void)
{
    int fh;
    int a;
    int b;

    fh = disk_read_file(D_0017531A, scratch_buffer);
    flats_cfg_count = 0;
    for (;;) {
        a = cfg_read_number(&fh);
        if (a == 100000)
            return;
        b = cfg_read_number(&fh);
        flats_cfg[flats_cfg_count].id = (a << 7) | b;
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
        flats_cfg[flats_cfg_count].f7 = cfg_read_number(&fh);
        flats_cfg[flats_cfg_count].f8 = cfg_read_number(&fh);
        flats_cfg[flats_cfg_count].f4 = cfg_read_number(&fh);
        flats_cfg_count++;
    }
}
