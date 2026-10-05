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
extern void close(int);
extern void mc_free(void *, char *, int);
extern int lseek(int, int, int);
extern void *mc_malloc(int, char *, int);
extern int read(int, void *, int);
extern void mc_strncpy(char *, char *, int, char *, int);
extern int open(char *, ...);

int profile_open(struct res *r, char *name)
{
    int fd;

    mc_strncpy(r->name, name, 4, D_00170129, 77);
    fd = open(name, 512);
    if (fd == -1)
        return 0;
    r->size = lseek(fd, 0, 2);
    r->bufsize = r->size + 1024;
    lseek(fd, 0, 0);
    if ((r->buf = mc_malloc(r->bufsize, D_00170129, 94)) == 0) {
        close(fd);
        return 0;
    }
    if (read(fd, r->buf, r->size) != r->size) {
        close(fd);
        if (r->buf != 0 && r->buf != (char *)0x97979797) {
            mc_free(r->buf, D_00170129, 110);
            r->buf = (char *)0x97979797;
        }
        return 0;
    }
    close(fd);
    r->pos = r->buf;
    r->f94 = 0;
    r->fa0 = 0;
    r->fa4 = 0;
    r->f9c = 0;
    r->fa8 = 0;
    r->flags1 &= 127;
    return 1;
}
