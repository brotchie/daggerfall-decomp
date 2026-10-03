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
extern char D_001903A4[];
extern char D_001917E4[];
extern unsigned D_00195BF4;
extern int func_0006CD6E(char *);
extern void func_0009DEA7(int);
extern int func_000A00CB(int, void *, unsigned);
extern int func_000A1004(char *);
extern unsigned func_000A13DA(char *, unsigned, struct find_t *);
extern unsigned func_000A13F7(struct find_t *);
#pragma aux func_000A0ED9 parm routine [];
extern void func_000A0ED9(int, char *);
extern int func_000A0F5C(char *, char *, ...);

void func_00028940(void)
{
    struct find_t ff;
    unsigned rc;
    unsigned t;
    int fh;

    func_000A0ED9(996, D_001707AE);
    func_000A0F5C(D_001903A4, D_001707D0, D_001917E4);
    rc = func_000A13DA(D_001903A4, 0, &ff);
    while (rc == 0) {
        fh = func_0006CD6E(ff.name);
        func_000A00CB(fh, &t, 4);
        func_0009DEA7(fh);
        if (D_00195BF4 - t > 129600)
            func_000A1004(D_001903A4);
        rc = func_000A13F7(&ff);
    }
    func_000A0ED9(1008, D_001707AE);
    func_000A0F5C(D_001903A4, D_001707DA, D_001917E4);
    rc = func_000A13DA(D_001903A4, 0, &ff);
    while (rc == 0) {
        fh = func_0006CD6E(ff.name);
        func_000A00CB(fh, &t, 4);
        func_0009DEA7(fh);
        if (D_00195BF4 - t > 129600)
            func_000A1004(D_001903A4);
        rc = func_000A13F7(&ff);
    }
}
