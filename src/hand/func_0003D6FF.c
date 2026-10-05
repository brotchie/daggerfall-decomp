/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0003D6FF */
struct entry { short id; int offset; };
extern char D_00170D55[];
extern char D_00170D7F[];
extern struct entry *D_00195C44;
extern int D_00195D6C;
extern int text_expand_wrap(unsigned short, short, unsigned char *, char *, char *);
extern void func_000A006E(int, int, int);
extern void *func_000A00AF(int, char *, int);
extern int func_000A00CB(int, void *, int);
#pragma aux func_000A0ED9 parm routine [];
extern void func_000A0ED9(int, char *);
extern int func_000A0F5C(char *, char *, ...);

int text_rsc_load_variant(short a1, unsigned short a2, short a3, int a4)
{
    short n;
    short i;
    short k;
    int size;
    unsigned char *buf;
    char *b2;
    char *b3;

    func_000A006E(D_00195D6C, 0, 0);
    func_000A00CB(D_00195D6C, &n, 2);
    func_000A00CB(D_00195D6C, D_00195C44, n);
    n /= 6;
    for (i = 0; i < n; i++) {
        if (D_00195C44[i].id == a1)
            break;
    }
    if (i >= n) {
        buf = func_000A00AF(1024, D_00170D55, 121);
        func_000A0ED9(122, D_00170D55);
        func_000A0F5C((char *)buf, D_00170D7F, a1);
        return (int)buf;
    }
    size = D_00195C44[i + 1].offset - D_00195C44[i].offset;
    func_000A006E(D_00195D6C, D_00195C44[i].offset, 0);
    buf = func_000A00AF(size + 16, D_00170D55, 129);
    b2 = func_000A00AF(size < 4096 ? 8192 : size * 2, D_00170D55, 130);
    b3 = func_000A00AF(size < 4096 ? 8192 : size * 2, D_00170D55, 131);
    func_000A00CB(D_00195D6C, buf, size + 8);
    k = 0;
    i = 1;
    while (buf[k] != 0xfe) {
        if (buf[k] == 0xff && buf[k + 1] != 0xfe)
            i++;
        k++;
    }
    i = a4;
    k = 0;
    while (i != 0) {
        while (buf[k++] != 0xff)
            ;
        i--;
    }
    while (buf[k] < 0xfe)
        buf[i++] = buf[k++];
    buf[i] = 0;
    return text_expand_wrap(a2, a3, buf, b2, b3);
}
