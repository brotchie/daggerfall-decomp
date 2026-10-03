/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0004BCFA */
struct find_t {             /* DOS find buffer */
    char reserved[21];
    char attrib;
    unsigned short wr_time;
    unsigned short wr_date;
    unsigned long size;
    char name[13];
};
extern char D_00174F47[];        /* __FILE__ */
extern char D_00174F50[];
extern char D_00174F57[];
extern char D_001903A4[];
extern char D_001917E4[];
extern char D_00191834[];
extern void func_0004BDB7(char *);
#pragma aux func_000A0ED9 parm routine [];
extern void func_000A0ED9(int, char *);
extern int func_000A0F5C(char *, char *, ...);
extern int func_000A13DA(char *, int, struct find_t *);
extern int func_000A13F7(struct find_t *);

void func_0004BCFA(void)
{
    struct find_t ff;
    int rc;

    func_000A0ED9(142, D_00174F47);
    func_000A0F5C(D_001903A4, D_00174F57, D_001917E4, D_00174F50);
    rc = func_000A13DA(D_001903A4, 0, &ff);
    if (rc != 0) {
        func_000A0ED9(146, D_00174F47);
        func_000A0F5C(D_001903A4, D_00174F57, D_00191834, D_00174F50);
        rc = func_000A13DA(D_001903A4, 0, &ff);
    }
    while (rc == 0) {
        func_0004BDB7(ff.name);
        rc = func_000A13F7(&ff);
    }
}
