/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00063FCF */
#include "records.h"

#pragma pack(1)
/* a struct move_request (records.h) and 14 bytes of the frame after it (the shared struct moves
 * the other locals: it does not match) */
struct Hit {
    int x;                      /* 0 */
    int y;                      /* 4 */
    int z;                      /* 8 */
    int angle_x;                /* 12 */
    int yaw;                    /* 16 */
    int angle_z;                /* 20 */
    char *probe;                /* 24 */
    short flags;                /* 28 */
    char pad[14];
};
extern char D_00175934[];
extern char D_00187B44[];
extern unsigned char D_001940D7;
extern unsigned char D_001940DA;
extern struct record *player_object;
extern int vertical_velocity;
extern int ceiling_height;
extern char player_on_ground;
extern struct vec3 D_00196D54;
extern int D_00196D58;
extern int D_00196D5C;
extern short collide_flags;
extern int collide_move_object(struct record *, int, struct Hit *, int);
extern struct record *object_delete(struct record *);
extern int abs();
extern int mc_memcpy(void *, void *, int, char *, int, int);
extern int xn_math_approx_dist2d();

int func_00063FCF(struct record *m, int dest_x, int dest_z)
{
    struct Hit hit;
    struct vec3 unused1;
    int unused2;
    struct vec3 pos;
    struct character *sub;
    int unused3;
    int unused4;
    struct vec3 saved;
    int save28;
    int dist;
    int h;
    int unused5;
    char save10;
    int save14;

    sub = &m->data.character;
    mc_memcpy(&pos, &m->x, 12, D_00175934, 1271, 4);
    hit.x = dest_x;
    hit.y = m->y;
    hit.z = dest_z;
    hit.angle_x = m->angle_x;
    hit.yaw = m->yaw;
    hit.angle_z = m->angle_z;
    save10 = player_on_ground;
    save14 = vertical_velocity;
    save28 = ceiling_height;
    *(unsigned char *)&collide_flags |= 4;
    h = vertical_velocity = sub->fall_velocity;
    D_001940D7 |= 128;
    hit.probe = D_00187B44;
    hit.flags &= ~1;
    mc_memcpy(&saved, &D_00196D54, 12, D_00175934, 1293, 4);
    D_00196D54.x = m->x;
    D_00196D58 = m->y - vertical_velocity / 256;
    D_00196D5C = m->z;
    collide_move_object(m, 0, &hit, 0);
    player_on_ground = save10;
    mc_memcpy(&D_00196D54, &saved, 12, D_00175934, 1300, 4);
    if ((int)(short)(collide_flags & 16) != 0)
        vertical_velocity = 1;
    sub->fall_velocity = vertical_velocity;
    vertical_velocity = save14;
    ceiling_height = save28;
    if (abs(m->y - player_object->y) > 3000) {
        D_001940DA |= 128;
        object_delete(m);
    }
    dist = xn_math_approx_dist2d(m->x, m->z, pos.x, pos.z);
    return dist > 2 ? 1 : 0;
}
