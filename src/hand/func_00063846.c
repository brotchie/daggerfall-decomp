/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00063846 */
#pragma pack(1)
struct bits8 { unsigned char b0:1, b1:1, b2:1, b3:1, b4:1, b5:1, b6:1, b7:1; };
struct vec3 { int x, y, z; };
struct obj {
    unsigned char type;
    short a1;                   /* 1 */
    short a3;                   /* 3 */
    short a5;                   /* 5 */
    struct vec3 pos;            /* 7 */
};
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
extern struct obj *player_object;
extern int D_00195AB0;
extern int vertical_velocity;
extern char *player_character;
extern int D_00195C74;
extern struct bits8 ai_monster_flags;
extern unsigned char player_on_ground;
extern struct vec3 D_00196D54;
extern short collide_flags;
extern int collide_move_object(struct obj *, int, struct move *, int);
extern void object_delete(struct obj *);
extern int func_0009DEAC(int);
extern void func_000A1023(void *, void *, int, char *, int, int);
extern int func_000C7FD9(int, int, int, int);
extern void func_000CE6E2(int, int, int *, int *);

int monster_move_step(struct obj *o, struct obj *target, int angle)
{
    struct vec3 saved;
    unsigned char saved277;
    int dx;
    struct vec3 oldpos;
    char *p;
    struct move mv;
    int dz;
    struct vec3 unused[2];
    int saved_c74;
    int dist;
    int dy;
    int speed;
    int r;
    int saved_ab8;

    p = (char *)o + 71;
    func_000A1023(&oldpos, &o->pos, 12, D_00175934, 1070, 4);
    speed = D_00195AB0 * (*(short *)(p + 44) - 50 + D_00187CA9) / 1000;
    if (*(int *)(p + 76) != 0)
        dx = dz = 0;
    else
        func_000CE6E2(angle, speed, &dx, &dz);
    mv.x = o->pos.x + dx;
    mv.y = o->pos.y;
    mv.z = o->pos.z + dz;
    if (ai_monster_flags.b0) {
        dy = mv.y - (target->pos.y - 45);
        if (func_0009DEAC(dy) > 10) {
            dx = mv.y;
            if (dy < 0)
                mv.y += speed;
            else
                mv.y -= speed;
        }
        if (dungeon_water_level != 10000 && ai_monster_flags.b6 && dungeon_water_level + 40 > mv.y)
            mv.y = dx;
    }
    mv.f12 = o->a1;
    mv.f16 = o->a3;
    mv.f20 = o->a5;
    saved277 = player_on_ground;
    saved_ab8 = vertical_velocity;
    saved_c74 = D_00195C74;
    collide_flags |= 4;
    r = (vertical_velocity = *(int *)(p + 76));
    D_001940D7 |= 128;
    mv.name = D_00187B44;
    mv.flags &= 65534;
    func_000A1023(&saved, &D_00196D54, 12, D_00175934, 1114, 4);
    D_00196D54.x = o->pos.x;
    D_00196D54.y = o->pos.y - vertical_velocity / 256;
    D_00196D54.z = o->pos.z;
    collide_move_object(o, 0, &mv, 0);
    player_on_ground = saved277;
    func_000A1023(&D_00196D54, &saved, 12, D_00175934, 1121, 4);
    if (p != player_character)
        *(int *)(p + 88) = D_00195C74;
    if ((collide_flags & (short)16) != 0 && !ai_monster_flags.b0)
        vertical_velocity = 1;
    *(int *)(p + 76) = vertical_velocity;
    vertical_velocity = saved_ab8;
    D_00195C74 = saved_c74;
    if (func_0009DEAC(o->pos.y - player_object->pos.y) > 4000) {
        D_001940DA |= 128;
        object_delete(o);
    }
    dist = func_000C7FD9(o->pos.x, o->pos.z, oldpos.x, oldpos.z);
    return dist > 2;
}
