/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00012BBB */
#pragma aux sosconv "*" parm caller [] value [eax] modify [eax ebx ecx edx];
#pragma aux (sosconv) func_00012BBB;
extern char D_00170129[];

#pragma pack(1)
struct stream {
    unsigned char f0;
    unsigned char flags;
    char pad2[130];
    char *start;                /* 132 */
    unsigned len;               /* 136 */
    unsigned cap;               /* 140 */
    char *pos;                  /* 144 */
};
#pragma pack()

extern char *func_000A0DD9(int, char *, int);
extern int func_000A0DF4(char *);
extern int func_000A0E0D();
extern int func_00011E36(struct stream *, ...);
extern int func_000122F1(struct stream *, ...);

int func_00012BBB(struct stream *s, char *key, int val, int width, int radix)
{
    char buf[32];
    char *dst;
    int n;
    int i;

    if ((short)func_00011E36(s, key) != 0) {
        func_000122F1(s, val);
        return 1;
    }
    if (radix == 16) {
        buf[0] = '0';
        buf[1] = 'x';
        func_000A0DD9(val, buf + 2, 16);
    } else {
        func_000A0DD9(val, buf, 10);
    }
    dst = s->pos;
    n = width + 4 + func_000A0DF4(buf);
    if (s->len + n > s->cap) return 0;
    func_000A0E0D(dst + n, dst, s->start + s->len - dst, D_00170129, 1436, 4);
    while (*key != 0) {
        *dst++ = *key++;
        width--;
    }
    while (width--) *dst++ = ' ';
    *dst++ = '=';
    *dst++ = ' ';
    i = 0;
    while (buf[i] != 0) *dst++ = buf[i++];
    *dst++ = 13;
    *dst++ = 10;
    s->len += n;
    s->flags |= 128;
    return 1;
}
