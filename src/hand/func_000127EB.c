/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000127EB */
#pragma aux sosconv "*" parm caller [] value [eax] modify [eax ebx ecx edx];
#pragma aux (sosconv) func_000127EB;
extern char D_00170129[];
extern int func_000A0E0D();
struct stream;
extern int func_00011E36(struct stream *, ...);

#pragma pack(1)
struct stream {
    unsigned char f0;
    unsigned char flags;
    char pad2[130];
    char *start;
    int len;
    char pad8c[16];
    char *buf;
};
#pragma pack()

int func_000127EB(struct stream *s, int a2)
{
    char *p;
    int n;

    n = 0;
    if ((short)func_00011E36(s, a2) == 0) return 0;
    p = s->buf;
    while (p[n] != 10) n++;
    n++;
    func_000A0E0D(p, p + n, s->start + s->len - (p + n), D_00170129, 1119, 4);
    s->len -= n;
    s->flags |= 128;
    return 1;
}
