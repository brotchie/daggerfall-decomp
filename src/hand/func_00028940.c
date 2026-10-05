/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00028940 */
#pragma pack(1)
struct find_t {
    char reserved[21];
    char attrib;
    unsigned short wr_time;
    unsigned short wr_date;
    unsigned long size;
    char name[13];
};
#pragma pack()
extern char D_001707AE[];
extern char D_001707D0[];
extern char D_001707DA[];
extern signed char text_buffer[];
extern char D_001917E4[];
extern unsigned game_minutes;
extern int disk_open_data(char *);
extern void func_0009DEA7(int);
extern int func_000A00CB(int, void *, unsigned);
extern int unlink(char *);
extern unsigned func_000A13DA(char *, unsigned, struct find_t *);
extern unsigned func_000A13F7(struct find_t *);
#pragma aux func_000A0ED9 parm routine [];
extern void func_000A0ED9(int, char *);
extern int mc_sprintf(char *, char *, ...);

void automap_purge_old_files(void)
{
    struct find_t ff;
    unsigned rc;
    unsigned t;
    int fh;

    func_000A0ED9(996, D_001707AE);
    mc_sprintf(((char *)text_buffer), D_001707D0, D_001917E4);
    rc = func_000A13DA(((char *)text_buffer), 0, &ff);
    while (rc == 0) {
        fh = disk_open_data(ff.name);
        func_000A00CB(fh, &t, 4);
        func_0009DEA7(fh);
        if (game_minutes - t > 129600)
            unlink(((char *)text_buffer));
        rc = func_000A13F7(&ff);
    }
    func_000A0ED9(1008, D_001707AE);
    mc_sprintf(((char *)text_buffer), D_001707DA, D_001917E4);
    rc = func_000A13DA(((char *)text_buffer), 0, &ff);
    while (rc == 0) {
        fh = disk_open_data(ff.name);
        func_000A00CB(fh, &t, 4);
        func_0009DEA7(fh);
        if (game_minutes - t > 129600)
            unlink(((char *)text_buffer));
        rc = func_000A13F7(&ff);
    }
}
