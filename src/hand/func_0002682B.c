/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0002682B */
#include "records.h"

struct bits8 { unsigned char b0:1, b1:1, b2:1, b3:1, b4:1, b5:1, b6:1, b7:1; };
struct desc { char pad[12]; char *str; char pad16[4]; };
extern char D_00170788[];
extern char D_00187B6E[];
extern struct bits8 D_001940D7;
extern struct bits8 player_motion_flags;
extern int D_00195C74;
extern unsigned char player_on_ground;
extern short collide_flags;
extern int collide_move_object(struct record *, int, char *, int);
extern void mc_memset(void *, int, int, char *, int, int);
extern void mc_memcpy(char *, char *, int, char *, int, int);

void func_0002682B(struct record *a1)
{
    int saved2;
    int saved1;
    struct desc d;

    saved1 = player_on_ground;
    saved2 = D_00195C74;
    if ((a1->flags & 16) == 0)
        return;
    {
        char pos[12];

        D_001940D7.b5 = 1;
        D_001940D7.b7 = 1;
        mc_memcpy(pos, (char *)&a1->x, 12, D_00170788, 477, 4);
        mc_memset(&d, 0, 12, D_00170788, 478, 4);
        d.str = D_00187B6E;
        player_motion_flags.b3 = 1;
        collide_move_object(a1, 0, pos, 0);
        player_motion_flags.b3 = 0;
        player_on_ground = saved1;
        D_00195C74 = saved2;
        if ((collide_flags & 1) == 0)
            return;
        a1->flags &= ~16;
    }
}
