/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0006D6A4 */
struct find_t {             /* DOS find buffer */
    char reserved[21];
    char attrib;
    unsigned short wr_time;
    unsigned short wr_date;
    unsigned long size;
    char name[13];
};
extern char D_00175D00[];        /* __FILE__ */
extern char D_00175D88[];
extern char D_00175D8F[];
extern char D_00175D95[];
extern char D_00175D97[];
extern char D_00175D9A[];
extern void func_0006D430(char *);
extern void func_0006D491(char *);
extern void func_000A0AD9(char *, char *, int, char *, int);
#pragma aux func_000A0ED9 parm routine [];
extern void func_000A0ED9(int, char *);
extern int func_000A0DF4(char *);
extern int func_000A0E3B(char *, char *);
extern int func_000A0F5C(char *, char *, ...);
extern int func_000A13DA(char *, int, struct find_t *);
extern int func_000A13F7(struct find_t *);

void func_0006D6A4(char *a1)
{
    int rc;
    int i;
    char path[80];
    struct find_t ff;

    if (a1[func_000A0DF4(a1) - 1] != '\\') {
        func_000A0ED9(337, D_00175D00);
        func_000A0F5C(path, D_00175D88, a1);
    } else {
        func_000A0ED9(339, D_00175D00);
        func_000A0F5C(path, D_00175D8F, a1);
    }
    rc = func_000A13DA(path, 16, &ff);
    while (rc == 0) {
        if ((ff.attrib & 16) == 0)
            func_0006D491(ff.name);
        rc = func_000A13F7(&ff);
    }
    rc = func_000A13DA(path, 16, &ff);
    while (rc == 0) {
        if (ff.attrib & 16) {
            if (func_000A0E3B(ff.name, D_00175D95) == 0 || func_000A0E3B(ff.name, D_00175D97) == 0) {
                rc = func_000A13F7(&ff);
                continue;
            }
            func_0006D430(ff.name);
            func_000A0AD9(&path[func_000A0DF4(path) - 3], ff.name, 4, D_00175D00, 359);
            func_0006D6A4(path);
            i = func_000A0DF4(path) - 1;
            while (i != 0 && path[i] != '\\')
                i--;
            func_000A0AD9(path + i, D_00175D9A, 4, D_00175D00, 363);
        }
        rc = func_000A13F7(&ff);
    }
}
