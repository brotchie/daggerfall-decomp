/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0007B0FC */
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
extern char *D_00147954;
extern char D_00176884[];
extern char D_001768DF[];
extern char D_0017696F[];
extern char D_00176977[];
extern char D_001917E4[];
extern int func_000A1004(char *);
extern unsigned func_000A13DA(char *, unsigned, struct find_t *);
extern unsigned func_000A13F7(struct find_t *);
#pragma aux func_000A0ED9 parm routine [];
extern void func_000A0ED9(int, char *);
extern int func_000A0F5C(char *, char *, ...);

void automap_delete_files(void)
{
    struct find_t ff;
    unsigned rc;

    func_000A0ED9(835, D_00176884);
    func_000A0F5C(D_00147954, D_0017696F, D_001917E4);
    rc = func_000A13DA(D_00147954, 0, &ff);
    while (rc == 0) {
        func_000A0ED9(839, D_00176884);
        func_000A0F5C(D_00147954, D_001768DF, D_001917E4, ff.name);
        func_000A1004(D_00147954);
        rc = func_000A13F7(&ff);
    }
    func_000A0ED9(844, D_00176884);
    func_000A0F5C(D_00147954, D_00176977, D_001917E4);
    rc = func_000A13DA(D_00147954, 0, &ff);
    while (rc == 0) {
        func_000A0ED9(848, D_00176884);
        func_000A0F5C(D_00147954, D_001768DF, D_001917E4, ff.name);
        func_000A1004(D_00147954);
        rc = func_000A13F7(&ff);
    }
}
