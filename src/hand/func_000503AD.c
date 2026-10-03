/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000503AD */
struct entry {
    short id;                   /* 0x00 */
    short f2;
    short f4;                   /* 0x04 */
    unsigned char flags;        /* 0x06 */
    unsigned char f7;           /* 0x07 */
    unsigned char f8;           /* 0x08 */
    char name[31];              /* 0x09 */
};
extern char D_0017531A[];
extern char D_001903A4[];
extern struct entry D_001918D4[];
extern int D_00195C44;
extern int D_00195C90;
extern void func_00050540(int *, char *);
extern int func_000505A3(int *);
extern int func_0006CB53(char *, int);

void func_000503AD(void)
{
    int fh;
    int a;
    int b;

    fh = func_0006CB53(D_0017531A, D_00195C44);
    D_00195C90 = 0;
    for (;;) {
        a = func_000505A3(&fh);
        if (a == 100000)
            return;
        b = func_000505A3(&fh);
        D_001918D4[D_00195C90].id = (a << 7) | b;
        func_00050540(&fh, D_001918D4[D_00195C90].name);
        func_00050540(&fh, D_001903A4);
        if (D_001903A4[0] == '?') {
            D_001918D4[D_00195C90].flags |= 2;
            a = 1;
        } else {
            a = 0;
        }
        if (D_001903A4[a] == '2')
            D_001918D4[D_00195C90].flags |= 1;
        D_001918D4[D_00195C90].f7 = func_000505A3(&fh);
        D_001918D4[D_00195C90].f8 = func_000505A3(&fh);
        D_001918D4[D_00195C90].f4 = func_000505A3(&fh);
        D_00195C90++;
    }
}
