/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000127EB */
#pragma aux sosconv "*" parm caller [] value [eax] modify [eax ebx ecx edx];
#pragma aux (sosconv) profile_delete_item;
extern char D_00170129[];
extern int mc_memmove();
struct stream;
extern int profile_find_item(struct stream *, ...);

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

int profile_delete_item(struct stream *s, int a2)
{
    char *p;
    int n;

    n = 0;
    if ((short)profile_find_item(s, a2) == 0) return 0;
    p = s->buf;
    while (p[n] != 10) n++;
    n++;
    mc_memmove(p, p + n, s->start + s->len - (p + n), D_00170129, 1119, 4);
    s->len -= n;
    s->flags |= 128;
    return 1;
}
