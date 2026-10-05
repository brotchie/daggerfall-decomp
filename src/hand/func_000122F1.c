/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000122F1 */
#pragma aux sosconv "*" parm caller [] value [eax] modify [eax ebx ecx edx];
#pragma aux (sosconv) profile_set_number;
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
    char *line;             /* 0xa0 */
    char *next;             /* 0xa4 */
};
extern char D_00170129[];        /* __FILE__ */
extern char *func_000A0DD9(int, signed char *, int);       /* itoa */
extern unsigned func_000A0DF4(signed char *);              /* strlen */
extern void mc_memmove(char *, char *, int, char *, int, int);

int profile_set_number(struct res *r, int value)
{
    signed char buf[32];
    char *p;
    char *q;
    int i;
    int n;
    unsigned len;
    int cnt;

    if (r->line == 0)
        return 0;
    p = r->line;
    while (*p == ' ')
        p++;
    q = p;
    n = 0;
    while (*p != '\r')
        buf[n++] = *p++;
    buf[n] = 0;
    if (buf[1] == 'x')
        func_000A0DD9(value, buf + 2, 16);
    else
        func_000A0DD9(value, buf, 10);
    len = func_000A0DF4(buf);
    if (len < n) {
        cnt = (r->buf + r->size) - q - (n - len);
        mc_memmove(q, q + (n - len), cnt, D_00170129, 768, 4);
        r->size -= n - len;
    } else if (len > n) {
        if ((len - n) + r->size > r->bufsize)
            return 0;
        cnt = (r->buf + r->size) - q + (len - n);
        mc_memmove(q + (len - n), q, cnt, D_00170129, 786, 4);
        r->size += len - n;
    }
    i = 0;
    while (buf[i] != 0)
        *q++ = buf[i++];
    r->flags1 |= 128;
    return 1;
}
