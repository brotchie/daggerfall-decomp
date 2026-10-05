/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00011A6B */
#pragma aux sosconv "*" parm caller [] value [eax] modify [eax ebx ecx edx];
#pragma aux (sosconv) profile_open;
struct res {                /* a file loaded whole into memory */
    unsigned char flags0;
    unsigned char flags1;
    char pad2[2];
    char name[128];
    char *buf;              /* 0x84 */
    int size;               /* 0x88 */
    int bufsize;            /* 0x8c */
    char *pos;              /* 0x90 */
    int f94;
    int f98;
    int f9c;
    int fa0;
    int fa4;
    int fa8;
};
extern char D_00170129[];        /* __FILE__ */
extern void func_0009DEA7(int);
extern void func_000A0024(void *, char *, int);
extern int func_000A006E(int, int, int);
extern void *func_000A00AF(int, char *, int);
extern int func_000A00CB(int, void *, int);
extern void func_000A0AD9(char *, char *, int, char *, int);
extern int func_0009DC59(char *, ...);

int profile_open(struct res *r, char *name)
{
    int fd;

    func_000A0AD9(r->name, name, 4, D_00170129, 77);
    fd = func_0009DC59(name, 512);
    if (fd == -1)
        return 0;
    r->size = func_000A006E(fd, 0, 2);
    r->bufsize = r->size + 1024;
    func_000A006E(fd, 0, 0);
    if ((r->buf = func_000A00AF(r->bufsize, D_00170129, 94)) == 0) {
        func_0009DEA7(fd);
        return 0;
    }
    if (func_000A00CB(fd, r->buf, r->size) != r->size) {
        func_0009DEA7(fd);
        if (r->buf != 0 && r->buf != (char *)0x97979797) {
            func_000A0024(r->buf, D_00170129, 110);
            r->buf = (char *)0x97979797;
        }
        return 0;
    }
    func_0009DEA7(fd);
    r->pos = r->buf;
    r->f94 = 0;
    r->fa0 = 0;
    r->fa4 = 0;
    r->f9c = 0;
    r->fa8 = 0;
    r->flags1 &= 127;
    return 1;
}
