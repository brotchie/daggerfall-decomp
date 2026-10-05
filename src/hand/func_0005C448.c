/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0005C448 */
#pragma pack(1)
struct Vec { int x; int y; int z; };
struct Sub { char pad[23]; short v; };
struct Mob {
    char pad0;
    short ax;                   /* 0x01 */
    short ay;                   /* 0x03 */
    char pad1[2];
    struct Vec pos;             /* 0x07 */
    char pad2[23 - 19];
    unsigned short flags;       /* 0x17 */
    char pad3[29 - 25];
    unsigned short state;       /* 0x1d */
    char pad4[63 - 31];
    struct Sub *sub;            /* 0x3f */
    char pad5[77 - 67];
    unsigned char type;         /* 0x4d */
    unsigned char kind;         /* 0x4e */
    char pad6[118 - 79];
    char path[1];               /* 0x76 */
};
extern short spell_impact_sounds[];
extern struct Mob *player_object;
extern int D_00195AB0;
extern int D_00195B84;
extern int D_00195C48;
extern unsigned char collide_flags;
extern int collide_move_missile(struct Mob *, struct Vec *, struct Vec *);
extern void func_0005C856(struct Mob *, int);
extern void links_trigger(int, int);
extern int sound_play(int, struct Mob *, int);
extern int func_00073F5D(int, int, int);
extern int func_000C2043(char *, int, struct Vec *);
extern int func_000C7FD9();
extern int func_000C7FF4();

int spell_missile_update(struct Mob *m, int a2)
{
    struct Vec pos;
    struct Vec ang;
    int hit;
    int dist;
    int obj;

    if ((int)(unsigned short)(m->flags & 1) != 0)
        return m->state == 0x8fff ? 1 : 0;
    dist = func_000C7FF4(player_object->pos.y - m->pos.y, func_000C7FD9(player_object->pos.x, player_object->pos.z, m->pos.x, m->pos.z));
    if (dist > 2048) {
        m->flags++;
        m->state = 0x8000;
        m->sub->v <<= 2;
        return 0;
    }
    pos.x = m->pos.x;
    pos.y = m->pos.y;
    pos.z = m->pos.z;
    func_000C2043(m->path, D_00195AB0 * 400 / 1000, &pos);
    ang.x = m->ax;
    ang.y = m->ay;
    ang.z = 0;
    collide_flags |= 4;
    hit = collide_move_missile(m, &pos, &ang);
    if (hit & 10) {
        links_trigger(D_00195C48, 6);
        sound_play(spell_impact_sounds[m->type], m, 110);
        m->flags |= 1;
        m->state = 0x8000;
        m->sub->v <<= 2;
        if ((hit & 8) && m->kind == 2)
            func_0005C856(m, D_00195C48);
    }
    obj = func_00073F5D(m->pos.x, m->pos.y, m->pos.z);
    if (obj == 0)
        return 0;
    if (D_00195B84 > 120)
        return 0;
    links_trigger(obj, 6);
    sound_play(spell_impact_sounds[m->type], m, 110);
    m->flags |= 1;
    m->state = 0x8000;
    m->sub->v <<= 2;
    func_0005C856(m, obj);
    return 0;
}
