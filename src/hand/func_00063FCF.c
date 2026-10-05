/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00063FCF */
#include "records.h"

#pragma pack(1)
struct Vec { int x; int y; int z; };
struct Hit {
    int a;                      /* 0 */
    int y;                      /* 4 */
    int b;                      /* 8 */
    int ax;                     /* 12 */
    int ay;                     /* 16 */
    int az;                     /* 20 */
    char *tbl;                  /* 24 */
    short flags;                /* 28 */
    char pad[14];
};
extern char D_00175934[];
extern char D_00187B44[];
extern unsigned char D_001940D7;
extern unsigned char D_001940DA;
extern struct record *player_object;
extern int vertical_velocity;
extern int D_00195C74;
extern char player_on_ground;
extern struct Vec D_00196D54;
extern int D_00196D58;
extern int D_00196D5C;
extern short collide_flags;
extern int collide_move_object(struct record *, int, struct Hit *, int);
extern int object_delete(struct record *);
extern int func_0009DEAC();
extern int mc_memcpy(void *, void *, int, char *, int, int);
extern int func_000C7FD9();

int func_00063FCF(struct record *m, int a2, int a3)
{
    struct Hit hit;
    struct Vec unused1;
    int unused2;
    struct Vec pos;
    struct character *sub;
    int unused3;
    int unused4;
    struct Vec saved;
    int save28;
    int dist;
    int h;
    int unused5;
    char save10;
    int save14;

    sub = &m->data.character;
    mc_memcpy(&pos, &m->x, 12, D_00175934, 1271, 4);
    hit.a = a2;
    hit.y = m->y;
    hit.b = a3;
    hit.ax = m->angle_x;
    hit.ay = m->yaw;
    hit.az = m->angle_z;
    save10 = player_on_ground;
    save14 = vertical_velocity;
    save28 = D_00195C74;
    *(unsigned char *)&collide_flags |= 4;
    h = vertical_velocity = sub->fall_velocity;
    D_001940D7 |= 128;
    hit.tbl = D_00187B44;
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
    D_00195C74 = save28;
    if (func_0009DEAC(m->y - player_object->y) > 3000) {
        D_001940DA |= 128;
        object_delete(m);
    }
    dist = func_000C7FD9(m->x, m->z, pos.x, pos.z);
    return dist > 2 ? 1 : 0;
}
