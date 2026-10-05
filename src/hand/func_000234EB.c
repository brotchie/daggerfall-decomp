/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000234EB */
#include "records.h"

struct bf8_4_1 { unsigned char _:4; unsigned char f:1; };
struct bf8_5_1 { unsigned char _:5; unsigned char f:1; };
extern unsigned char player_environment;
extern char D_00179F48[];
extern unsigned char D_001940D7;
extern signed char player_motion_flags;
extern struct record *player_object;
extern struct character *player_character;
extern char D_00195C70[];
extern char D_00195CB8[];
extern int D_00195CD4;
extern int D_00195CD8;
extern short D_00195F5A;
extern signed char player_on_ground;
extern signed char D_00196296;
extern int collide_candidate_count;
extern char D_00196D4C[];
extern int D_00196D50;
extern char collide_flags[];
extern int func_0002294E(struct record *, int, struct move_request *);
extern void collide_for_each_nearby(struct record *, int);
extern void func_0002325A(int);
extern void automap_mark_seen(int);
extern int func_000657B2(int);
extern int func_0009DEAC();
extern int func_0014BDDD();

struct plane {
    int f0;
    int f4;
    int f8;
    int f12;
    int nx;
    int ny;
    int nz;
    short f28;
};

struct planes {
    int count;
    struct plane p[1];
};

#define PLANES (((struct planes *)D_00196D50))
#define PL (PLANES->p)
#define FLAGS (*(short *)collide_flags)

struct vec3 {
    int x;
    int y;
    int z;
};

int collide_move_player(struct record *a1, int a2, struct move_request *a3, int a4)
{
    int l_44;
    int l_40;           /* l_40, l_3C and l_34 are never read: they only shape the frame */
    int l_3C;
    struct vec3 l_5C;
    int l_34;
    int l_30;
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    int l_18;
    int l_14;
    short *l_10;

    l_34 = 0;
    *(int *)D_00195CB8 = *(int *)D_00195C70 = 0;
    D_00195F5A = 10000;
    D_00195CD8 = D_00195CD4 = 0;
    D_00196D50 = 0;
    collide_candidate_count = 0;
    (*(struct vec3 **)D_00196D4C = (struct vec3 *)D_00179F48)->x = a3->x;
    (*(struct vec3 **)D_00196D4C)->y = a3->y;
    (*(struct vec3 **)D_00196D4C)->z = a3->z;
    collide_for_each_nearby(a1, (int)func_0002325A);
    if (!((player_character->conditions & 0x8) || collide_candidate_count != 0 || player_environment == 1))
        return FLAGS = 16;
    l_14 = FLAGS;
    player_on_ground = 1;
    l_44 = func_0002294E(a1, a2, a3);
    D_001940D7 &= 223;
    if ((char)player_on_ground != 0 && (char)D_00196296 != 0)
        D_00196296 = 0;
    if (*(int *)D_00195CB8 != 0 && (l_10 = (short *)func_000657B2(*(int *)D_00195CB8)) != 0) {
        if (l_10[0] != 0 || l_10[2] != 0) {
            a3->x += l_10[0];
            a3->y += l_10[1];
            a3->z += l_10[2];
            *(unsigned char *)collide_flags |= 4;
            l_44 = func_0002294E(a1, a2, a3);
        }
    }
    if (*(int *)D_00195C70 != 0 && (l_10 = (short *)func_000657B2(*(int *)D_00195C70)) != 0) {
        a3->x += l_10[0];
        a3->y += l_10[1];
        a3->z += l_10[2];
        *(unsigned char *)collide_flags |= 4;
        l_44 = func_0002294E(a1, a2, a3);
    }
    if (*(int *)D_00195CB8 != 0)
        automap_mark_seen(*(int *)D_00195CB8);
    if (*(int *)D_00195C70 != 0)
        automap_mark_seen(*(int *)D_00195C70);
    if (l_44 & 2)
        D_00195F5A = 0;
    if (a1 == player_object && ((struct bf8_5_1 *)&player_motion_flags)->f)
        return 0;
    if (!(l_44 & 10) || !(l_14 & 4))
        return 0;
    l_5C.x = a3->x - a1->x;
    l_5C.y = a3->y - a1->y;
    l_5C.z = a3->z - a1->z;
    func_0014BDDD((int)&l_5C);
    if (D_00196D50 == 0)
        return 1;
    if (PLANES->count > 1) {
        l_24 = 0;
        l_1C = 1000000;
        for (l_20 = 0; l_20 < PLANES->count; l_20++) {
            l_18 = l_5C.x * PL[l_20].nx + l_5C.z * PL[l_20].nz;
            if (l_18 < l_1C) {
                l_1C = l_18;
                l_24 = l_20;
            }
        }
    } else {
        l_1C = l_5C.x * PL[0].nx + l_5C.z * PL[0].nz;
        l_24 = 0;
    }
    l_30 = (a3->x - a1->x) * PL[l_24].nx;
    l_2C = (a3->y - a1->y) * PL[l_24].ny;
    l_28 = (a3->z - a1->z) * PL[l_24].nz;
    l_30 = l_28 + (l_30 + l_2C);
    l_2C = l_30 * PL[l_24].ny;
    l_28 = l_30 * PL[l_24].nz;
    l_30 = l_30 * PL[l_24].nx;
    l_30 >>= 8;
    l_2C >>= 8;
    l_28 >>= 8;
    l_30 = (a3->x - a1->x) - (l_30 >> 8);
    l_2C = (a3->y - a1->y) - (l_2C >> 8);
    l_28 = (a3->z - a1->z) - (l_28 >> 8);
    a3->x = a1->x + l_30;
    a3->y = a1->y + l_2C;
    a3->z = a1->z + l_28;
    D_00195F5A = func_0009DEAC(l_1C >> 16);
    if (l_30 != 0 || l_2C != 0 || l_28 != 0) {
        D_00195CD8 = D_00195CD4 = 0;
        l_14 = FLAGS;
        player_on_ground = 1;
        l_44 = func_0002294E(a1, a2, a3);
        D_001940D7 &= 223;
        if ((char)player_on_ground != 0 && (char)D_00196296 != 0)
            D_00196296 = 0;
        if (*(int *)D_00195CB8 != 0)
            automap_mark_seen(*(int *)D_00195CB8);
        if (*(int *)D_00195C70 != 0)
            automap_mark_seen(*(int *)D_00195C70);
        if (a1 == player_object && ((struct bf8_4_1 *)&player_motion_flags)->f)
            return 0;
        if (!(l_44 & 10) || !(l_14 & 4))
            return 0;
        return 1;
    }
    return FLAGS;
}
