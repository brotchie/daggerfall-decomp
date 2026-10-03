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
extern struct slot D_0018E044[];
extern char D_001903A4[];
extern struct flags16 *D_00195BF8;
extern struct savehdr *D_00195C44;
extern char *D_00195D80;
extern unsigned short D_00195F22;
extern int func_0006CD6E(char *);
extern void func_0009DEA7(int);
extern int func_000A00CB(int, void *, int);
extern int func_000A0D13(char *);
extern int func_000A13DA(char *, int, struct find_t *);
extern int func_000A13F7(struct find_t *);
#pragma aux func_000A0ED9 parm routine [];
extern void func_000A0ED9(int, char *);
extern int func_000A0F5C(char *, char *, ...);

void func_0005F81F(void)
{
    int fd;
    int rc;
    struct find_t ff;
    struct savehdr *buf;

    D_00195F22 = 0;
    buf = D_00195C44;
    func_000A0ED9(694, D_001758B8);
    func_000A0F5C(D_001903A4, D_00175913, D_00195D80, D_0017590A);
    rc = func_000A13DA(D_001903A4, 0, &ff);
    while (rc == 0) {
        D_0018E044[D_00195F22].id = func_000A0D13(ff.name + 3);
        func_000A0ED9(699, D_001758B8);
        func_000A0F5C(D_001903A4, D_0017591E, ff.name);
        fd = func_0006CD6E(D_001903A4);
        func_000A00CB(fd, buf, 234);
        func_0009DEA7(fd);
        if (buf->f128 == 0 || (D_00195BF8->f0 & 4) == 0)
            D_0018E044[D_00195F22++].f2 = buf->f228 - 1;
        rc = func_000A13F7(&ff);
    }
}
