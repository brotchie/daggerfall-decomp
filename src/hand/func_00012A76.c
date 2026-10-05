/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00012A76 */
#pragma pack(1)
#pragma aux sosconv "*" parm caller [] value [eax] modify [eax ebx ecx edx];
#pragma aux (sosconv) profile_add_item_string;
#pragma aux (sosconv) profile_find_item;
#pragma aux (sosconv) profile_set_string;
struct Ini {
    char pad0;
    unsigned char flags;        /* 0x01 */
    char pad1[0x84 - 2];
    char *base;                 /* 0x84 */
    unsigned used;              /* 0x88 */
    unsigned cap;               /* 0x8c */
    char *wp;                   /* 0x90 */
};
extern char D_00170129[];
extern unsigned func_000A0DF4(char *);
extern void mc_memmove(char *, char *, unsigned, char *, int, int);
extern short profile_find_item(struct Ini *, char *);
extern void profile_set_string(struct Ini *, char *);

int profile_add_item_string(struct Ini *s, char *key, char *val, int width)
{
    char *p;
    unsigned len;

    if (profile_find_item(s, key) != 0) {
        profile_set_string(s, val);
        return 1;
    }
    p = s->wp;
    len = width + 4 + func_000A0DF4(val);
    if (s->used + len > s->cap)
        return 0;
    mc_memmove(p + len, p, s->base + s->used - p, D_00170129, 1324, 4);
    while (*key != 0) {
        *p++ = *key++;
        width--;
    }
    while (width-- != 0)
        *p++ = ' ';
    *p++ = '=';
    *p++ = ' ';
    while (*val != 0)
        *p++ = *val++;
    *p++ = '\r';
    *p++ = '\n';
    s->used += len;
    s->flags |= 0x80;
    return 1;
}
