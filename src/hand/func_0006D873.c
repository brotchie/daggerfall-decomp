/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0006D873 */
struct find_t {             /* DOS find buffer */
    char reserved[21];
    char attrib;
    unsigned short wr_time;
    unsigned short wr_date;
    unsigned long size;
    char name[13];
};
extern char D_00175D00[];        /* __FILE__ */
extern char D_00175D60[];
#pragma aux mc_set_location parm routine [];
extern void mc_set_location(int, char *);
extern int mc_sprintf(char *, char *, ...);
extern int unlink(char *);
extern int func_000A13DA(char *, int, struct find_t *);
extern int func_000A13F7(struct find_t *);

void disk_delete_matching(int a1, int a2)
{
    int rc;
    char path[80];
    struct find_t ff;
    int unused;

    mc_set_location(376, D_00175D00);
    mc_sprintf(path, D_00175D60, a1, a2);
    rc = func_000A13DA(path, 0, &ff);
    while (rc == 0) {
        mc_set_location(380, D_00175D00);
        mc_sprintf(path, D_00175D60, a1, ff.name);
        unlink(path);
        rc = func_000A13F7(&ff);
    }
}
