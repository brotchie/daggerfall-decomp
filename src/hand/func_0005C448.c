/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0005C448 */
#include "records.h"

/* a flying spell is a type-9 record: header +0x17 is its missile texture (bit 0 set once it
 * has hit), image2 0x8000 once it has hit; its velocity is kept at record +0x76 (the spell's
 * name); its first child is the light, whose +0x17 grows on impact */
struct Vec { int x; int y; int z; };
extern short spell_impact_sounds[];
extern struct record *player_object;
extern int frame_ticks;
extern int D_00195B84;
extern struct record *D_00195C48;
extern unsigned char collide_flags;
extern int collide_move_missile(struct record *, struct Vec *, struct Vec *);
extern void func_0005C856(struct record *, struct record *);
extern void links_trigger(struct record *, int);
extern int sound_play(int, struct record *, int);
extern struct record *monster_nearest_to_point(int, int, int);
extern int xn_vec_advance(char *, int, struct Vec *);
extern int xn_math_approx_dist2d();
extern int xn_math_approx_hypot();

int spell_missile_update(struct record *m, int a2)
{
    struct Vec pos;
    struct Vec ang;
    int hit;
    int dist;
    struct record *obj;

    if ((int)(unsigned short)(m->missile_texture & 1) != 0)
        return m->image2 == 0x8fff ? 1 : 0;
    dist = xn_math_approx_hypot(player_object->y - m->y, xn_math_approx_dist2d(player_object->x, player_object->z, m->x, m->z));
    if (dist > 2048) {
        m->missile_texture++;
        m->image2 = 0x8000;
        m->children->light_radius <<= 2;
        return 0;
    }
    pos.x = m->x;
    pos.y = m->y;
    pos.z = m->z;
    xn_vec_advance((char *)m + 118, frame_ticks * 400 / 1000, &pos);
    ang.x = m->angle_x;
    ang.y = m->yaw;
    ang.z = 0;
    collide_flags |= 4;
    hit = collide_move_missile(m, &pos, &ang);
    if (hit & 10) {
        links_trigger(D_00195C48, 6);
        sound_play(spell_impact_sounds[m->data.spell.element], m, 110);
        m->missile_texture |= 1;
        m->image2 = 0x8000;
        m->children->light_radius <<= 2;
        if ((hit & 8) && m->data.spell.target == 2)
            func_0005C856(m, D_00195C48);
    }
    obj = monster_nearest_to_point(m->x, m->y, m->z);
    if (obj == 0)
        return 0;
    if (D_00195B84 > 120)
        return 0;
    links_trigger(obj, 6);
    sound_play(spell_impact_sounds[m->data.spell.element], m, 110);
    m->missile_texture |= 1;
    m->image2 = 0x8000;
    m->children->light_radius <<= 2;
    func_0005C856(m, obj);
    return 0;
}
