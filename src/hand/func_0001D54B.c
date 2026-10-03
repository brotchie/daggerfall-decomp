/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0001D54B */
#pragma pack(1)
struct Item { char pad[37]; unsigned short flags; };
struct Rec {
    short id1;                  /* 0 */
    short id2;                  /* 2 */
    int kind;                   /* 4 */
    char owner;                 /* 8 */
    unsigned char flags;        /* 9 */
};
extern char D_00196268;
extern struct Item *func_000192EE(short);

int func_0001D54B(struct Rec *r, short a2, int a3, int a4)
{
    int unused;
    struct Item *p1;
    struct Item *p2;

    p1 = 0;
    p2 = 0;
    if (a3 != 0) {
        if (D_00196268 != r->owner)
            return 0;
        return (unsigned char)(r->flags & 1);
    }
    if ((int)(unsigned char)(r->flags & 12) == 0)
        return 0;
    if (r->id1 != 0)
        p1 = func_000192EE(r->id1);
    if (r->id2 != 0)
        p2 = func_000192EE(r->id2);
    if (!(p1 || p2 || r->kind != 100))
        return 1;
    if (p1 != 0 && (int)(unsigned short)(p1->flags & 1) != 0 || p2 != 0 && (int)(unsigned short)(p2->flags & 1) != 0) {
        if (a4 <= 75)
            return 0;
    }
    return 1;
}
