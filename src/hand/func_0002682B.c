/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0002682B */
#include "records.h"

struct bits8 { unsigned char b0:1, b1:1, b2:1, b3:1, b4:1, b5:1, b6:1, b7:1; };
extern char D_00170788[];
extern struct collide_probe D_00187B6E;
extern struct bits8 D_001940D7;
extern struct bits8 player_motion_flags;
extern int ceiling_height;
extern unsigned char player_on_ground;
extern short collide_flags;
extern int collide_move_object(struct record *, int, struct move_request *, int);
extern void mc_memset(void *, int, int, char *, int, int);
extern void mc_memcpy(char *, char *, int, char *, int, int);

void place_settle_creature(struct record *creature)
{
    int saved_ceiling;
    int saved_on_ground;
    struct move_request request;

    saved_on_ground = player_on_ground;
    saved_ceiling = ceiling_height;
    if ((creature->flags & 16) == 0)
        return;
    {
        D_001940D7.b5 = 1;
        D_001940D7.b7 = 1;
        mc_memcpy((char *)&request, (char *)&creature->x, 12, D_00170788, 477, 4);
        mc_memset(&request.angle_x, 0, 12, D_00170788, 478, 4);
        request.probe = &D_00187B6E;
        player_motion_flags.b3 = 1;
        collide_move_object(creature, 0, &request, 0);
        player_motion_flags.b3 = 0;
        player_on_ground = saved_on_ground;
        ceiling_height = saved_ceiling;
        if ((collide_flags & 1) == 0)
            return;
        creature->flags &= ~16;
    }
}
