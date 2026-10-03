/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000633BC */
struct mobile {
    char pad0[64];
    unsigned short flags;       /* 0x40 */
    char pad42[440];
    unsigned char f506;         /* 0x1fa */
};
struct thing { unsigned char type; char pad[70]; struct mobile mob; };
extern void func_00069938(int, struct thing *, int);
extern int func_0009DC25(void);

void func_000633BC(struct thing *a1, int a2)
{
    struct mobile *m;
    int snd;

    m = &a1->mob;
    snd = m->f506 * 10 + 10000;
    if (m->flags & 384) {
        if (a2 < 128)
            func_00069938(func_0009DC25() & 3 ? snd + 2 : snd + 1, a1, 100);
        else if (func_0009DC25() < 32000)
            func_00069938(snd + 1, a1, 100);
        else
            func_00069938(snd, a1, 100);
    } else
        func_00069938(snd, a1, 100);
}
