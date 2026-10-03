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
#pragma aux func_000A0ED9 parm routine [];
extern void func_000A0ED9(int, char *);
extern int func_000A0F5C(char *, char *, ...);
extern int func_000A1004(char *);
extern int func_000A13DA(char *, int, struct find_t *);
extern int func_000A13F7(struct find_t *);

void func_0006D873(int a1, int a2)
{
    int rc;
    char path[80];
    struct find_t ff;
    int unused;

    func_000A0ED9(376, D_00175D00);
    func_000A0F5C(path, D_00175D60, a1, a2);
    rc = func_000A13DA(path, 0, &ff);
    while (rc == 0) {
        func_000A0ED9(380, D_00175D00);
        func_000A0F5C(path, D_00175D60, a1, ff.name);
        func_000A1004(path);
        rc = func_000A13F7(&ff);
    }
}
