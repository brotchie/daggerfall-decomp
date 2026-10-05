/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0005F81F */
struct find_t {             /* DOS find buffer */
    char reserved[21];
    char attrib;
    unsigned short wr_time;
    unsigned short wr_date;
    unsigned long size;
    char name[13];
};
struct savehdr { char pad[128]; char f128; char pad81[99]; short f228; };
struct slot { short id; short f2; };
struct flags16 { unsigned short f0; };
extern char D_001758B8[];        /* __FILE__ */
extern char D_0017590A[];
extern char D_00175913[];
extern char D_0017591E[];
extern struct slot book_list[];
extern signed char text_buffer[];
extern struct flags16 *game_settings;
extern struct savehdr *scratch_buffer;
extern char *books_path;
extern unsigned short book_count;
extern int disk_open_data(char *);
extern void close(int);
extern int read(int, void *, int);
extern int atoi(char *);
extern int func_000A13DA(char *, int, struct find_t *);
extern int func_000A13F7(struct find_t *);
#pragma aux mc_set_location parm routine [];
extern void mc_set_location(int, char *);
extern int mc_sprintf(char *, char *, ...);

void books_scan(void)
{
    int fd;
    int rc;
    struct find_t ff;
    struct savehdr *buf;

    book_count = 0;
    buf = scratch_buffer;
    mc_set_location(694, D_001758B8);
    mc_sprintf(((char *)text_buffer), D_00175913, books_path, D_0017590A);
    rc = func_000A13DA(((char *)text_buffer), 0, &ff);
    while (rc == 0) {
        book_list[book_count].id = atoi(ff.name + 3);
        mc_set_location(699, D_001758B8);
        mc_sprintf(((char *)text_buffer), D_0017591E, ff.name);
        fd = disk_open_data(((char *)text_buffer));
        read(fd, buf, 234);
        close(fd);
        if (buf->f128 == 0 || (game_settings->f0 & 4) == 0)
            book_list[book_count++].f2 = buf->f228 - 1;
        rc = func_000A13F7(&ff);
    }
}
