/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0008A113 */
struct tbl { char pad[74]; unsigned short f4a[3]; unsigned short f50[1]; };
struct ent {
    char pad0[0x64];
    int f64;
    char pad68[0x8b - 0x68];
    unsigned char f8b;
    char pad8c[0x219 - 0x8c];
    int f219;
};
extern int game_minutes;

int spfx_shield(char *a1, int a2, char *a3)
{
    struct ent *e;
    struct tbl *t;

    t = (struct tbl *)(a1 + 71);
    e = (struct ent *)(a3 + 71);
    e->f8b |= 64;
    e->f219 = t->f50[a2];
    e->f64 = t->f4a[a2] + game_minutes;
    return 1;
}
