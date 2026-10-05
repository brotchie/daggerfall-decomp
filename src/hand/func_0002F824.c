/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0002F824 */
struct move { int x, y, z; int f12, f16, f20; char *name; short flags; char pad[26]; int dz; };
extern char D_00187B44[];
extern int D_00195C74;
extern unsigned char player_on_ground;
extern short collide_flags;
extern int collide_move_object(char *, int, struct move *, int);
extern void damage_apply(char *, int, int);
extern void func_000CE6E2(int, int, int *, int *);

void damage_knockback_move(char *a1, char *a2)
{
    struct move m;
    int l_30;
    int l_2C, l_28, l_24, l_20;     /* unused, but they have slots */
    int l_1C;
    int l_18;
    int l_14;

    l_1C = player_on_ground;
    l_18 = D_00195C74;
    if (*(short *)(a2 + 110) > 40)
        *(short *)(a2 + 110) = 40;
    func_000CE6E2(*(short *)(a2 + 108), *(short *)(a2 + 110) > 25 ? 25 : *(short *)(a2 + 110), &l_30, &m.dz);
    m.x = *(int *)(a1 + 7) + l_30;
    m.y = *(int *)(a1 + 11);
    m.z = *(int *)(a1 + 15) + m.dz;
    m.f12 = *(short *)(a1 + 1);
    m.f16 = *(short *)(a1 + 3);
    m.f20 = *(short *)(a1 + 5);
    m.name = D_00187B44;
    m.flags |= 1;
    collide_flags |= 4;
    collide_move_object(a1, 0, &m, 1);
    player_on_ground = l_1C;
    D_00195C74 = l_18;
    l_14 = collide_flags;
    if (l_14 & 2) {
        a2[64] &= 223;
        a2[65] |= 8;
        damage_apply(a1, *(short *)(a2 + 110) >> 1, 0);
    } else {
        *(short *)(a2 + 110) -= 5;
        if (*(short *)(a2 + 110) <= 5) {
            a2[64] &= 223;
            a2[65] |= 8;
        }
    }
    if (*(unsigned short *)(a2 + 64) & 32)
        a2[504] = 16;
}
