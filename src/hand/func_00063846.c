/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00063846 */
#include "records.h"

#pragma pack(1)
struct bits8 { unsigned char b0:1, b1:1, b2:1, b3:1, b4:1, b5:1, b6:1, b7:1; };
struct vec3 { int x, y, z; };
struct move {
    int x, y, z;
    int f12, f16, f20;
    char *name;                 /* 24 */
    unsigned short flags;       /* 28 */
};
#pragma pack()
extern int dungeon_water_level;
extern char D_00175934[];
extern char D_00187B44[];
extern int D_00187CA9;
extern unsigned char D_001940D7;
extern unsigned char D_001940DA;
extern struct record *player_object;
extern int frame_ticks;
extern int vertical_velocity;
extern struct character *player_character;
extern int ceiling_height;
extern struct bits8 ai_monster_flags;
extern unsigned char player_on_ground;
extern struct vec3 D_00196D54;
extern short collide_flags;
extern int collide_move_object(struct record *, int, struct move *, int);
extern void object_delete(struct record *);
extern int abs(int);
extern void mc_memcpy(void *, void *, int, char *, int, int);
extern int xn_math_approx_dist2d(int, int, int, int);
extern void xn_math_yaw_offset_xz(int, int, int *, int *);

int monster_move_step(struct record *o, struct record *target, int angle)
{
    struct vec3 saved;
    unsigned char saved277;
    int dx;
    struct vec3 oldpos;
    struct character *p;
    struct move mv;
    int dz;
    struct vec3 unused[2];
    int saved_c74;
    int dist;
    int dy;
    int speed;
    int r;
    int saved_ab8;

    p = &o->data.character;
    mc_memcpy(&oldpos, &o->x, 12, D_00175934, 1070, 4);
    speed = frame_ticks * (p->attributes[ATTR_SPD] - 50 + D_00187CA9) / 1000;
    if (p->fall_velocity != 0)
        dx = dz = 0;
    else
        xn_math_yaw_offset_xz(angle, speed, &dx, &dz);
    mv.x = o->x + dx;
    mv.y = o->y;
    mv.z = o->z + dz;
    if (ai_monster_flags.b0) {
        dy = mv.y - (target->y - 45);
        if (abs(dy) > 10) {
            dx = mv.y;
            if (dy < 0)
                mv.y += speed;
            else
                mv.y -= speed;
        }
        if (dungeon_water_level != 10000 && ai_monster_flags.b6 && dungeon_water_level + 40 > mv.y)
            mv.y = dx;
    }
    mv.f12 = o->angle_x;
    mv.f16 = o->yaw;
    mv.f20 = o->angle_z;
    saved277 = player_on_ground;
    saved_ab8 = vertical_velocity;
    saved_c74 = ceiling_height;
    collide_flags |= 4;
    r = (vertical_velocity = p->fall_velocity);
    D_001940D7 |= 128;
    mv.name = D_00187B44;
    mv.flags &= 65534;
    mc_memcpy(&saved, &D_00196D54, 12, D_00175934, 1114, 4);
    D_00196D54.x = o->x;
    D_00196D54.y = o->y - vertical_velocity / 256;
    D_00196D54.z = o->z;
    collide_move_object(o, 0, &mv, 0);
    player_on_ground = saved277;
    mc_memcpy(&D_00196D54, &saved, 12, D_00175934, 1121, 4);
    if (p != player_character)
        p->ceiling_y = ceiling_height;
    if ((collide_flags & (short)16) != 0 && !ai_monster_flags.b0)
        vertical_velocity = 1;
    p->fall_velocity = vertical_velocity;
    vertical_velocity = saved_ab8;
    ceiling_height = saved_c74;
    if (abs(o->y - player_object->y) > 4000) {
        D_001940DA |= 128;
        object_delete(o);
    }
    dist = xn_math_approx_dist2d(o->x, o->z, oldpos.x, oldpos.z);
    return dist > 2;
}
