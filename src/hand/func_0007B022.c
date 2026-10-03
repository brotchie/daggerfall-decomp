/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0007B022 */
#pragma pack(1)
struct find_t {
    char reserved[21];
    char attrib;
    unsigned short wr_time;
    unsigned short wr_date;
    unsigned long size;
    char name[13];
};
extern char *D_00147954;
extern char D_00176884[];
extern char D_0017696F[];
extern char D_00176977[];
extern char D_001917E4[];
extern void func_0006CEFC(char *, int, char *);
extern void func_0007B0FC(void);
extern unsigned func_000A13DA(char *, unsigned, struct find_t *);
extern unsigned func_000A13F7(struct find_t *);
#pragma aux func_000A0ED9 parm routine [];
extern int func_000A0ED9(int, char *);
extern int func_000A0F5C(char *, char *, ...);

void func_0007B022(int a1)
{
    unsigned rc;
    struct find_t f;

    func_0007B0FC();
    func_000A0ED9(813, D_00176884);
    func_000A0F5C(D_00147954, D_0017696F, a1);
    rc = func_000A13DA(D_00147954, 0, &f);
    while (rc == 0) {
        func_0006CEFC(f.name, a1, D_001917E4);
        rc = func_000A13F7(&f);
    }
    func_000A0ED9(821, D_00176884);
    func_000A0F5C(D_00147954, D_00176977, a1);
    rc = func_000A13DA(D_00147954, 0, &f);
    while (rc == 0) {
        func_0006CEFC(f.name, a1, D_001917E4);
        rc = func_000A13F7(&f);
    }
}
